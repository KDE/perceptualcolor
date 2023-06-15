// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "absolutecolor.h"

#include "helpermath.h"
#include "helperposixmath.h"
#include <cmath>
#include <lcms2.h>
#include <optional>
#include <qgenericmatrix.h>
#include <qglobal.h>
#include <qmath.h>

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
#include <qhashfunctions.h>
#include <type_traits>
#endif

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

/** @brief List of all available conversions from this color model.
 *
 * @param model The color model from which to convert.
 *
 * @returns List of all available conversions from this color model. */
QList<AbsoluteColor::Conversion> AbsoluteColor::conversionsFrom(const ColorModel model)
{
    QList<AbsoluteColor::Conversion> result;
    for (const auto &item : conversionList) {
        if (item.from == model) {
            result.append(item);
        }
    }
    return result;
}

/** @brief Adds some @ref GenericColor to an existing hash table.
 *
 * @param values A hash table with color values.
 * @param model The color model from which to perform conversions.
 *
 * @pre <em>values</em> contains the key <em>model</em>.
 *
 * @post For all available direct conversions from <em>model</em>, it is
 * checked whether a value for the destination color model is already
 * available in <em>values</em>. If not, this value is calculated and added
 * to <em>values</em>, and this function is called recursively again for this
 * destination color model. */
void AbsoluteColor::addDirectConversionsRecursivly(QHash<ColorModel, GenericColor> *values, ColorModel model)
{
    const auto availableConversions = conversionsFrom(model);
    const auto currentValue = values->value(model);
    for (const auto &conversion : availableConversions) {
        if (!values->contains(conversion.to)) {
            values->insert(conversion.to, conversion.conversionFunction(currentValue));
            addDirectConversionsRecursivly(values, conversion.to);
        }
    }
}

/** @brief Calculate conversions to all color models.
 *
 * @param model The original color model
 * @param value The original color value
 *
 * @returns A list containing the original value and containing conversions
 * to all other @ref ColorModel. */
QHash<ColorModel, GenericColor> AbsoluteColor::allConversions(const ColorModel model, const GenericColor &value)
{
    QHash<ColorModel, GenericColor> result;
    result.insert(model, value);
    addDirectConversionsRecursivly(&result, model);
    return result;
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
GenericColor AbsoluteColor::fromOklabToXyzD65(const GenericColor &value)
{
    // The following algorithm is as described in
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    //
    // Oklab: “The inverse operation, going from Oklab to XYZ is done with
    // the following steps:”

    auto lms = (*m2inverse) * value.toTrio(); // NOTE Entries might be negative.
    // LMS (long, medium, short) is the response of the three types of
    // cones of the human eye.

    lms(/*row*/ 0, /*column*/ 0) = std::pow(lms(/*row*/ 0, /*column*/ 0), 3);
    lms(/*row*/ 1, /*column*/ 0) = std::pow(lms(/*row*/ 1, /*column*/ 0), 3);
    lms(/*row*/ 2, /*column*/ 0) = std::pow(lms(/*row*/ 2, /*column*/ 0), 3);

    return GenericColor((*m1inverse) * lms);
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
GenericColor AbsoluteColor::fromXyzD65ToOklab(const GenericColor &value)
{
    // The following algorithm is as described in
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    //
    // Oklab: “First the XYZ coordinates are converted to an approximate
    // cone responses:”
    auto lms = (*m1) * value.toTrio(); // NOTE Entries might be negative.
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
    return GenericColor((*m2) * lms);
}

/** @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color */
GenericColor AbsoluteColor::fromXyzD65ToXyzD50(const GenericColor &value)
{
    return GenericColor((*xyzD65ToXyzD50) * value.toTrio());
}

/** @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color */
GenericColor AbsoluteColor::fromXyzD50ToXyzD65(const GenericColor &value)
{
    return GenericColor((*xyzD50ToXyzD65) * value.toTrio());
}

/** @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color */
GenericColor AbsoluteColor::fromXyzD50ToCielabD50(const GenericColor &value)
{
    const cmsCIEXYZ cmsXyzD50 = value.reinterpretAsXyzToCmsciexyz();
    cmsCIELab result;
    cmsXYZ2Lab(cmsD50_XYZ(), // white point (for both, XYZ and also Cielab)
               &result, // output
               &cmsXyzD50); // input
    return GenericColor(result);
}

/** @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color */
GenericColor AbsoluteColor::fromCielabD50ToXyzD50(const GenericColor &value)
{
    const auto temp = value.reinterpretAsLabToCmscielab();
    cmsCIEXYZ xyzD50;
    cmsLab2XYZ(cmsD50_XYZ(), // white point (for both, XYZ and also Lab)
               &xyzD50, // output
               &temp); // input
    return GenericColor(xyzD50);
}

/** @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color
 *
 * This is a generic function converting between polar coordinates
 * (format: ignored, radius, angleDegree, ignored) and Cartesian coordinates
 * (format: ignored, x, y, ignored). */
GenericColor AbsoluteColor::fromCartesianToPolar(const GenericColor &value)
{
    GenericColor result = value;
    const auto &x = value.second;
    const auto &y = value.third;
    const auto radius = sqrt(pow(x, 2) + pow(y, 2));
    result.second = radius;
    if (radius == 0) {
        result.third = 0;
        return result;
    }
    if (y >= 0) {
        result.third = qRadiansToDegrees(acos(x / radius));
    } else {
        result.third = qRadiansToDegrees(2 * pi - acos(x / radius));
    }
    return result;
}

/** @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color
 *
 * This is a generic function converting between polar coordinates
 * (format: ignored, radius, angleDegree, ignored) and Cartesian coordinates
 * (format: ignored, x, y, ignored). */
GenericColor AbsoluteColor::fromPolarToCartesian(const GenericColor &value)
{
    const auto &radius = value.second;
    const auto &angleDegree = value.third;
    return GenericColor(value.first, //
                        radius * cos(qDegreesToRadians(angleDegree)),
                        radius * sin(qDegreesToRadians(angleDegree)),
                        value.fourth);
}

/** @brief Convert a color from one color model to another.
 *
 * @param from The color model from which the conversion is made.
 * @param value The value being converted.
 * @param to The color model to which the conversion is made.
 *
 * @returns The value converted into the new color model.
 *
 * @note This function is <em>not</em> speed-optimized. */
std::optional<GenericColor> AbsoluteColor::convert(const ColorModel from, const GenericColor &value, const ColorModel to)
{
    const auto temp = allConversions(from, value);
    if (temp.contains(to)) {
        return temp.value(to);
    }
    return std::nullopt;
}

} // namespace PerceptualColor
