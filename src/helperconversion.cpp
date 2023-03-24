// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helperconversion.h"

#include "helpermath.h"
#include "lchdouble.h"
#include "rgbdouble.h"
#include <array>
#include <cmath>
#include <optional>
#include <qgenericmatrix.h>
#include <qglobal.h>

namespace PerceptualColor
{

// Doxygen doesn’t handle correctly the Q_GLOBAL_STATIC_WITH_ARGS macro, so
// we instruct Doxygen with the @cond command to ignore  this part of the code.

/// @cond

// clang-format off

// https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
Q_GLOBAL_STATIC_WITH_ARGS(
    const SquareMatrix3,
    m1,
    (std::array<double, 9>{{
        +0.8189330101, +0.3618667424, -0.1288597137,
        +0.0329845436, +0.9293118715, +0.0361456387,
        +0.0482003018, +0.2643662691, +0.6338517070}}.data()))

// https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
Q_GLOBAL_STATIC_WITH_ARGS(
    const SquareMatrix3,
    m2,
    (std::array<double, 9>{{
        +0.2104542553, +0.7936177850, -0.0040720468,
        +1.9779984951, -2.4285922050, +0.4505937099,
        +0.0259040371, +0.7827717662, -0.8086757660}}.data()))

// https://fujiwaratko.sakura.ne.jp/infosci/colorspace/bradford_e.html
Q_GLOBAL_STATIC_WITH_ARGS(
    const SquareMatrix3,
    xyzD65ToXyzD50,
    (std::array<double, 9>{{
        +1.047886, +0.022919, -0.050216,
        +0.029582, +0.990484, -0.017079,
        -0.009252, +0.015073, +0.751678}}.data()))

// clang-format on

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    m1inverse,
    (inverseMatrix(*m1).value_or(SquareMatrix3())))

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    m2inverse,
    (inverseMatrix(*m2).value_or(SquareMatrix3())))

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    xyzD50ToXyzD65,
    (inverseMatrix(*xyzD65ToXyzD50).value_or(SquareMatrix3())))

/// @endcond

/** @internal
 *
 * @brief Type conversion.
 * @param value An LCH value
 * @returns Same LCH value as <tt>cmsCIELCh</tt>. */
cmsCIELCh toCmsLch(const LchDouble &value)
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
LchDouble toLchDouble(const cmsCIELCh &value)
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
LchDouble toLchDouble(const cmsCIELab &value)
{
    cmsCIELCh tempLch;
    cmsLab2LCh(&tempLch, &value);
    return toLchDouble(tempLch);
}

/** @internal
 *
 * @brief Conversion to <tt>cmsCIELab</tt>
 * @param value the value to convert
 * @returns the same value as <tt>cmsCIELab</tt> */
cmsCIELab toCmsLab(const cmsCIELCh &value)
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
Trio fromXyzd65ToOklab(const Trio &value)
{
    // The following algorithm is as described in
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    //
    // Oklab: “First the XYZ coordinates are converted to an approximate
    // cone responses:”
    auto lms = (*m1) * value; // NOTE Might contain negative entries
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
    return (*m2) * lms;
}

/** @internal
 *
 * @brief Conversion from <a href="https://bottosson.github.io/posts/oklab/">
 * Oklab color space</a> to
 * <a href="https://en.wikipedia.org/wiki/CIE_1931_color_space#Definition_of_the_CIE_XYZ_color_space">
 * CIE 1931 XYZ color space</a>.
 *
 * @param value The value to be converted
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
 * <a href="https://en.wikipedia.org/wiki/CIE_1931_color_space#Definition_of_the_CIE_XYZ_color_space">
 * CIE 1931 XYZ color space</a>. The XYZ value has
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab">
 * “a D65 whitepoint and white as Y=1”</a>. */
Trio fromOklabToXyzd65(const Trio &value)
{
    // The following algorithm is as described in
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    //
    // Oklab: “The inverse operation, going from Oklab to XYZ is done with
    // the following steps:”
    auto lms = (*m2inverse) * value; // NOTE Might contain negative entries
    // LMS (long, medium, short) is the response of the three types of
    // cones of the human eye.

    lms(/*row*/ 0, /*column*/ 0) = std::pow(lms(/*row*/ 0, /*column*/ 0), 3);
    lms(/*row*/ 1, /*column*/ 0) = std::pow(lms(/*row*/ 1, /*column*/ 0), 3);
    lms(/*row*/ 2, /*column*/ 0) = std::pow(lms(/*row*/ 2, /*column*/ 0), 3);

    return (*m1inverse) * lms;
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
    cmsCIEXYZ xyzD50;
    cmsLab2XYZ(cmsD50_XYZ(), // white point (for both, XYZ and also Lab)
               &xyzD50, // output
               &cielabD50); // input
    const double xyzD50Array[]{xyzD50.X, xyzD50.Y, xyzD50.Z};
    const Trio xyzD50Matrix(xyzD50Array);
    const auto resultMatrix = fromXyzd65ToOklab( //
        (*xyzD50ToXyzD65) * xyzD50Matrix);
    const cmsCIELab result = {resultMatrix(0, 0), //
                              resultMatrix(1, 0), //
                              resultMatrix(2, 0)};
    return result;
}

/** @internal
 *
 * @brief Conversion from
 * <a href="https://bottosson.github.io/posts/oklab/">Oklab color space</a>
 * to <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * CIELab D50 color space</a>.
 *
 * @param oklab The Oklab value to be converted.
 *
 * @returns the same color in
 * <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * CIELab D50 color space</a>. */
cmsCIELab fromOklabToCmscielabD50(const cmsCIELab &oklab)
{
    const double oklabArray[] = {oklab.L, oklab.a, oklab.b};
    const Trio oklabMatrix(oklabArray);
    const auto xyzD65 = fromOklabToXyzd65(oklabMatrix);
    const auto xyzD50 = (*xyzD65ToXyzD50) * xyzD65;
    const cmsCIEXYZ cmsXyzD50{xyzD50(0, 0), xyzD50(1, 0), xyzD50(2, 0)};
    cmsCIELab result;
    cmsXYZ2Lab(cmsD50_XYZ(), // white point (for both, XYZ and also Lab)
               &result, // output
               &cmsXyzD50); // input
    return result;
}

/** @internal
 *
 * @brief Converts from @ref RgbDouble to a <tt>QColor</tt> with
 * <tt>spec()</tt> value <tt>QColor::Rgb</tt>.
 *
 * @param color The original color
 *
 * @return The same color as <tt>QColor</tt> with
 * <tt>spec()</tt> value <tt>QColor::Rgb</tt>.
 * If the original color has out-of-range values,
 * than these values are silently clipped to the
 * valid range. */
QColor fromRgbDoubleToQColor(const RgbDouble &color)
{
    return QColor::fromRgbF( //
        static_cast<QColorFloatType>(qBound<QColorFloatType>(0, color.red, 1)), //
        static_cast<QColorFloatType>(qBound<QColorFloatType>(0, color.green, 1)), //
        static_cast<QColorFloatType>(qBound<QColorFloatType>(0, color.blue, 1)));
}

} // namespace PerceptualColor
