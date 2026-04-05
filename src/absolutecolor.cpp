// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "absolutecolor.h"

#include "helperconversion.h"
#include "helperimage.h"
#include "helpermath.h"
#include "helperposixmath.h"
#include <cmath>
#include <lcms2.h>
#include <optional>
#include <qgenericmatrix.h>
#include <qglobal.h>
#include <qmath.h>

namespace PerceptualColor
{

// Doxygen doesn’t handle correctly the Q_GLOBAL_STATIC_WITH_ARGS macro, so
// we instruct Doxygen with the @cond command to ignore  this part of the code.
/// @cond

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    m1,
    (oklabM1.data()))

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    m2,
    (oklabM2.data()))

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    xyzD65ToXyzD50,
    (xyzD65ToXyzD50Matrix.data()))

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    linearSRgbToXyzD65,
    (linearSRgbToXyzD65Matrix.data()))

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

Q_GLOBAL_STATIC_WITH_ARGS( //
    const SquareMatrix3,
    xyzD65ToLinearSRgb,
    (inverseMatrix(*linearSRgbToXyzD65).value_or(SquareMatrix3())))

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

/**
 * @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color
 */
GenericColor AbsoluteColor::fromLinearSRgbToSRgb(const GenericColor &value)
{
    return GenericColor(channelFromLinearSRgbToSRgb(value.first), //
                        channelFromLinearSRgbToSRgb(value.second), //
                        channelFromLinearSRgbToSRgb(value.third), //
                        value.fourth);
}

/**
 * @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color
 */
GenericColor AbsoluteColor::fromSRgbToLinearSRgb(const GenericColor &value)
{
    return GenericColor(channelFromSRgbToLinearSRgb(value.first), //
                        channelFromSRgbToLinearSRgb(value.second), //
                        channelFromSRgbToLinearSRgb(value.third), //
                        value.fourth);
}

/**
 * @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color
 */
GenericColor AbsoluteColor::fromXyzD65ToLinearSRgb(const GenericColor &value)
{
    auto result = GenericColor((*xyzD65ToLinearSRgb) * value.toTrio());
    result.fourth = value.fourth;
    return result;
}

/**
 * @internal
 *
 * @brief Color conversion.
 *
 * @param value Color to be converted.
 *
 * @returns the converted color
 */
GenericColor AbsoluteColor::fromLinearSRgbToXyzD65(const GenericColor &value)
{
    auto result = GenericColor((*linearSRgbToXyzD65) * value.toTrio());
    result.fourth = value.fourth;
    return result;
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

/**
 * @internal
 *
 * @brief Round <tt>float</tt> to <tt>quint8</tt>.
 *
 * @param x A floating point value in the range [0..255].
 *
 * @returns The rounded value as <tt>quint8</tt>. If the input value
 * differes from the valid input range by 0.5 or more, than it returns
 * an arbitrary value.
 */
[[nodiscard]] quint8 AbsoluteColor::toByte(float x)
{
    return static_cast<quint8>(x * 255.f + 0.5f);
}

/** @brief Conversion to QRgb.
 *
 * @param lab the original color
 *
 * @returns An opaque sRGB color matching the original one if it is within the
 * gamut. Otherwise, returns a fully transparent color (alpha and RGB channels
 * set to 0 to ensure compatibility with both, premultiplied and
 * non-premultiplied data).
 *
 * @sa @ref ColorEngine::fromCielchD50ToQRgbBound()
 *
 * @internal
 *
 * @note This function is optimized for speed. It uses <tt>float</tt> instead
 * of <tt>double</tt>, as double‑precision arithmetic is roughly 50% slower in
 * this context. All functions called here are defined within the same
 * translation unit, allowing the compiler to have their code inlined.
 * Benchmarks show that these calls introduce no measurable overhead compared
 * to writing the logic directly in place.
 *
 * @note This function is based on the
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-linear-srgb-to-oklab">
 * original Oklab code</a>.
 *
 * @note Unfortunately, it cannot be <tt>constexpr</tt>, because it
 * calls @ref channelFromLinearSRgbToSRgb() which is not <tt>constexpr</tt>.
 */
// GenericColor might be an alternative, but it has double precision, while here
// float precision would be appropriate.
[[nodiscard]] QRgb AbsoluteColor::fastFromOklabToSRgbOrTransparent(const cmsCIELab &lab)
{
    // Original Lab values as float:
    const float oLabL = static_cast<float>(lab.L);
    const float oLabA = static_cast<float>(lab.a);
    const float oLabB = static_cast<float>(lab.b);

    const float l_ = oLabL + 0.3963377774f * oLabA + 0.2158037573f * oLabB;
    const float m_ = oLabL - 0.1055613458f * oLabA - 0.0638541728f * oLabB;
    const float s_ = oLabL - 0.0894841775f * oLabA - 1.2914855480f * oLabB;

    const float l = l_ * l_ * l_;
    const float m = m_ * m_ * m_;
    const float s = s_ * s_ * s_;

    const float r = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    const float g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    const float b = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

    // Check for in-range yet now to avoid unnecessary calls of toByte() and
    // linearToSRgb(). Furthermore, linearToSRgb() has undefined behaviour
    // for parameters < 0.
    if (r < 0.f || r > 1.f || g < 0.f || g > 1.f || b < 0.f || b > 1.f) {
        return qRgbTransparent;
    }

    return qRgba(toByte(channelFromLinearSRgbToSRgb(r)), //
                 toByte(channelFromLinearSRgbToSRgb(g)), //
                 toByte(channelFromLinearSRgbToSRgb(b)),
                 255);
}

/** @brief Conversion to QRgb.
 *
 * @param oklab the original color
 *
 * @returns An opaque sRGB color matching the original one if it is within the
 * gamut. Otherwise, returns a more or less similar color.
 *
 * @sa @ref ColorEngine::fromCielchD50ToQRgbBound()
 *
 * @internal
 *
 * @note This function is optimized for speed. It uses <tt>float</tt> instead
 * of <tt>double</tt>, as double‑precision arithmetic is roughly 50% slower in
 * this context. All functions called here are defined within the same
 * translation unit, allowing the compiler to have their code inlined.
 * Benchmarks show that these calls introduce no measurable overhead compared
 * to writing the logic directly in place.
 *
 * @note This function is based on the
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-linear-srgb-to-oklab">
 * original Oklab code</a>.
 *
 * @note Unfortunately, it cannot be <tt>constexpr</tt>, because it
 * calls @ref channelFromLinearSRgbToSRgb() which is not <tt>constexpr</tt>.
 */
[[nodiscard]] QRgb AbsoluteColor::fastFromOklabToSRgbClamped(const GenericColor &oklab)
{
    // Original Lab values as float:
    const float oLabL = static_cast<float>(oklab.first);
    const float oLabA = static_cast<float>(oklab.second);
    const float oLabB = static_cast<float>(oklab.third);

    const float l_ = oLabL + 0.3963377774f * oLabA + 0.2158037573f * oLabB;
    const float m_ = oLabL - 0.1055613458f * oLabA - 0.0638541728f * oLabB;
    const float s_ = oLabL - 0.0894841775f * oLabA - 1.2914855480f * oLabB;

    const float l = l_ * l_ * l_;
    const float m = m_ * m_ * m_;
    const float s = s_ * s_ * s_;

    const float r = +4.0767416621f * l - 3.3077115913f * m + 0.2309699292f * s;
    const float g = -1.2684380046f * l + 2.6097574011f * m - 0.3413193965f * s;
    const float b = -0.0041960863f * l - 0.7034186147f * m + 1.7076147010f * s;

    return qRgba(toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(r), 0, 1)), //
                 toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(g), 0, 1)), //
                 toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(b), 0, 1)),
                 255);
}

} // namespace PerceptualColor
