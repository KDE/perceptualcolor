// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helperconversion.h"

#include "lchdouble.h"
#include <cmath>
#include <qgenericmatrix.h>

namespace
{
// Nameless namespace is better than “static” keyword,
// see https://stackoverflow.com/a/4422554

using ConversionMatrix = QGenericMatrix<3, 3, double>;
}

namespace PerceptualColor
{
/** @internal
 *
 * @brief Type conversion.
 * @param value An LCH value
 * @returns Same LCH value as <tt>cmsCIELCh</tt>. */
cmsCIELCh toCmsCieLch(const LchDouble &value)
{
    cmsCIELCh result;
    result.L = value.l;
    result.C = value.c;
    result.h = value.h;
    return result;
}

/** @internal
 *
 * @brief Type conversion.
 * @param value An LCH value
 * @returns Same LCH value as @ref LchDouble. */
LchDouble toCielchDouble(const cmsCIELCh &value)
{
    LchDouble result;
    result.l = value.L;
    result.c = value.C;
    result.h = value.h;
    return result;
}

/** @internal
 *
 * @brief Conversion to @ref LchDouble
 * @param value a point in Lab representation
 * @returns the same point in @ref LchDouble representation */
LchDouble toCielchDouble(const cmsCIELab &value)
{
    cmsCIELCh tempLch;
    cmsLab2LCh(&tempLch, &value);
    return toCielchDouble(tempLch);
}

/** @internal
 *
 * @brief Conversion to <tt>cmsCIELab</tt>
 * @param value the value to convert
 * @returns the same value as <tt>cmsCIELab</tt> */
cmsCIELab toCmsCieLab(const cmsCIELCh &value)
{
    cmsCIELab lab; // uses cmsFloat64Number internally
    // convert from LCH to Lab
    cmsLCh2Lab(&lab, &value);
    return lab;
}

/** @internal
 *
 * @brief Conversion from
 * <a href="https://en.wikipedia.org/wiki/CIE_1931_color_space#Definition_of_the_CIE_XYZ_color_space">
 * CIE 1931 XYZ color space</a> to
 * <a href="https://bottosson.github.io/posts/oklab/">
 * Oklab color space</a>.
 *
 * @param value The value to be converted
 *
 * @pre The XYZ value has
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab">
 * “a D65 whitepoint and white as Y=1”</a>.
 *
 * @note <a href="https://bottosson.github.io/posts/oklab/">
 * Oklab</a> does not specify which
 * <a href="https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_standard_observer">
 * observer</a> the D65 whitepoint should use. But it states that
 * <em>“Oklab uses a D65 whitepoint, since this is what sRGB and other
 * common color spaces use.”</em>. As
 * <a href="https://en.wikipedia.org/wiki/SRGB">sRGB</a>
 * uses the <em>CIE 1931 2° Standard Observer</em>, this
 * might be a good choice.
 *
 * @returns the same color in
 * <a href="https://bottosson.github.io/posts/oklab/">
 * Oklab color space</a>. */
QGenericMatrix<1, 3, double> fromXyzd65ToOklab(const QGenericMatrix<1, 3, double> &value)
{
    // Constants
    // clang-format off
    constexpr double m1array[] =
        {+0.8189330101, +0.3618667424, -0.1288597137,
         +0.0329845436, +0.9293118715, +0.0361456387,
         +0.0482003018, +0.2643662691, +0.6338517070};
    constexpr double m2array[] =
        {+0.2104542553, +0.7936177850, -0.0040720468,
         +1.9779984951, -2.4285922050, +0.4505937099,
         +0.0259040371, +0.7827717662, -0.8086757660};
    // clang-format on
    static const ConversionMatrix m1{m1array};
    static const ConversionMatrix m2{m2array};
    // NOTE If later we need this matrices also in other places, we could
    // move it into a nameless namespace within this very same file:
    // namespace {
    // // Nameless namespace is better than “static” keyword,
    // // see https://stackoverflow.com/a/4422554
    // Q_GLOBAL_STATIC_WITH_ARGS(const ConversionMatrix, m1, (m1array))
    // }

    // The following algorithm is as described in
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    //
    // Oklab: “First the XYZ coordinates are converted to an approximate
    // cone responses:”
    auto lms = m1 * value; // NOTE Might contain negative entries
    // LMS (long, medium, short) is the response of the three types of
    // cones of the human eye.

    // Oklab: “A non-linearity is applied:”
    // NOTE The original paper of Björn Ottosson, available at
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    // proposes to calculate this: “x raised to the power of ⅓”. However,
    // x might be negative. The original paper does not explicitly explain
    // what the expected behaviour is, as “x raised to the power of ⅓”
    // is not universally defined for negative x values. Also,
    // std::pow(x, 1.0/3) would return “nan” for negative x. The
    // original paper does not provide a reference implementation for
    // the conversion between XYZ and Oklab. But it provides a reference
    // implementation for a direct (shortcut) conversion between sRGB
    // and Oklab, and this reference implementation uses std::cbrtf()
    // instead of std::pow(x, 1.0/3). And std::cbrtf() seems to allow
    // a negative radicand. This makes round-trip conversations possible,
    // because it gives unique results for each x value. Therefore, here
    // we do the same, but using std::cbrt() instead of std::cbrtf() to
    // allow double precision instead of float precision.
    lms(/*row*/ 0, /*column*/ 0) = std::cbrt(lms(/*row*/ 0, /*column*/ 0));
    lms(/*row*/ 1, /*column*/ 0) = std::cbrt(lms(/*row*/ 1, /*column*/ 0));
    lms(/*row*/ 2, /*column*/ 0) = std::cbrt(lms(/*row*/ 2, /*column*/ 0));

    // Oklab: “Finally, this is transformed into the Lab-coordinates:”
    return m2 * lms;
}

/** @internal
 *
 * @brief Conversion from
 * <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * CIELab D50 color space</a> to
 * <a href="https://bottosson.github.io/posts/oklab/">Oklab color space</a>.
 *
 * @param cielabD50 The CIELab D50 value to be converted.
 *
 * @returns the same color in
 * <a href="https://bottosson.github.io/posts/oklab/">Oklab color space</a>. */
cmsCIELab fromCmscielabD50ToOklab(const cmsCIELab &cielabD50)
{
    // Constants:
    // https://fujiwaratko.sakura.ne.jp/infosci/colorspace/bradford_e.html
    // proposes a “D65 to D50 transformation matrix” for XYZ:
    //  1.047886  0.022919 -0.050216
    //  0.029582  0.990484 -0.017079
    // -0.009252  0.015073  0.751678
    // To do the inverse operation, we use the inverse of this matrix,
    // (calculated with https://matrix.reshish.com/inverCalculation.php):
    // clang-format off
    constexpr double xyzD50ToXyzD65Array[] =
        {+0.955512609517083167880, -0.023073214184644801330, +0.063308961782106566635,
         -0.028324949364887475422, +1.009942432477107357000, +0.021054814890111626368,
         +0.012328875695482643347, -0.020535835374141285089, +1.330713916450354270600};
    // clang-format on
    static const ConversionMatrix xyzD50ToXyzD65{xyzD50ToXyzD65Array};

    // Implementation:
    cmsCIEXYZ xyzD50;
    cmsLab2XYZ(cmsD50_XYZ(), // white point (for both, XYZ and also Lab)
               &xyzD50, // output
               &cielabD50); // input
    const double xyzD50Array[]{xyzD50.X, xyzD50.Y, xyzD50.Z};
    const QGenericMatrix<1, 3, double> xyzD50Matrix(xyzD50Array);
    const auto resultMatrix = fromXyzd65ToOklab(xyzD50ToXyzD65 * xyzD50Matrix);
    const cmsCIELab result = {resultMatrix(0, 0), //
                              resultMatrix(1, 0), //
                              resultMatrix(2, 0)};
    return result;
}

} // namespace PerceptualColor
