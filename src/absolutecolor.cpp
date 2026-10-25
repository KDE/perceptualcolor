// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "absolutecolor.h"

#include "colorspaceinfo.h"
#include "helperconstants.h"
#include "helperconversion.h"
#include "helperimage.h"
#include "helpermath.h"
#include "mat3.h"
#include "vec3.h"
#include <algorithm>
#include <cmath>
#include <numbers>
#include <optional>
#include <qglobal.h>
#include <qmath.h>

namespace PerceptualColor
{

/*!
 * \inmodule ModulePrivateApi
 *
 * \brief List of all available conversions from this color model.
 *
 * \a model The color model from which to convert.
 */
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

/*!
 * \brief Adds some \l GenericColor to an existing hash table.
 *
 * \a values A hash table with color values.
 *
 * \a model The color model from which to perform conversions.
 *
 * \b Precondition: \e {values} contains the key \e {model}.
 *
 * For all available direct conversions from \e {model}, it is
 * checked whether a value for the destination color model is already
 * available in \e {values}. If not, this value is calculated and added
 * to \e {values}, and this function is called recursively again for this
 * destination color model. */
void AbsoluteColor::addDirectConversionsRecursivly(QHash<ColorModel, GenericColor> *values, const ColorModel model)
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

/*!
 * \brief Calculate conversions to all color models.
 *
 * \a model The original color model
 *
 * \a value The original color value
 *
 * Returns a list containing the original value and containing conversions
 * to all other \l ColorModel. */
QHash<ColorModel, GenericColor> AbsoluteColor::allConversions(const ColorModel model, const GenericColor &value)
{
    QHash<ColorModel, GenericColor> result;
    result.insert(model, value);
    addDirectConversionsRecursivly(&result, model);
    return result;
}

/*!
 *
 *
 * \brief Conversion from \l {https://bottosson.github.io/posts/oklab/}
 * {Oklab color space} to
 * \l {https://en.wikipedia.org/wiki/CIE_1931_color_space#Definition_of_the_CIE_XYZ_color_space}
 * {CIE 1931 XYZ color space}.
 *
 * \a value The value to be converted
 *
 * \note \l {https://bottosson.github.io/posts/oklab/}
 * {Oklab} does not specify which
 * \l {https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_standard_observer}
 * {observer} the D65 whitepoint should use. But it states that
 * \e {“Oklab uses a D65 whitepoint, since this is what sRGB and other
 * common color spaces use.”}. As
 * \l {https://en.wikipedia.org/wiki/SRGB} {sRGB}
 * uses the \e {CIE 1931 2° Standard Observer}, this
 * might be a good choice.
 *
 * Returns the same color in
 * \l {https://en.wikipedia.org/wiki/CIE_1931_color_space#Definition_of_the_CIE_XYZ_color_space}
 * {CIE 1931 XYZ color space}. The XYZ value has
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab}
 * {“a D65 whitepoint and white as Y=1”}. */
GenericColor AbsoluteColor::fromOklabToXyzD65(const GenericColor &value)
{
    // The following algorithm is as described in
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    //
    // Oklab: “The inverse operation, going from Oklab to XYZ is done with
    // the following steps:”

    auto lms = static_cast<Mat3d>(oklabM2inverse) * value.toVec3d(); // NOTE Entries might be negative.
    // LMS (long, medium, short) is the response of the three types of
    // cones of the human eye.

    lms(0) = std::pow(lms(0), 3);
    lms(1) = std::pow(lms(1), 3);
    lms(2) = std::pow(lms(2), 3);

    return GenericColor(static_cast<Mat3d>(oklabM1inverse) * lms);
}

/*!
 *
 *
 * \brief Conversion from
 * \l {https://en.wikipedia.org/wiki/CIE_1931_color_space#Definition_of_the_CIE_XYZ_color_space}
 * {CIE 1931 XYZ color space} to
 * \l {https://bottosson.github.io/posts/oklab/}
 * {Oklab color space}.
 *
 * \a value The value to be converted
 *
 * \b Precondition: The XYZ value has
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab}
 * {“a D65 whitepoint and white as Y=1”}.
 *
 * \note \l {https://bottosson.github.io/posts/oklab/}
 * {Oklab} does not specify which
 * \l {https://en.wikipedia.org/wiki/CIE_1931_color_space#CIE_standard_observer}
 * {observer} the D65 whitepoint should use. But it states that
 * \e {“Oklab uses a D65 whitepoint, since this is what sRGB and other
 * common color spaces use.”}. As
 * \l {https://en.wikipedia.org/wiki/SRGB} {sRGB}
 * uses the \e {CIE 1931 2° Standard Observer}, this
 * might be a good choice.
 *
 * Returns the same color in
 * \l {https://bottosson.github.io/posts/oklab/}
 * {Oklab color space}. */
GenericColor AbsoluteColor::fromXyzD65ToOklab(const GenericColor &value)
{
    // The following algorithm is as described in
    // https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab
    //
    // Oklab: “First the XYZ coordinates are converted to an approximate
    // cone responses:”
    auto lms = static_cast<Mat3d>(oklabM1) * value.toVec3d(); // NOTE Entries might be negative.
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
    lms(0) = std::cbrt(lms(0));
    lms(1) = std::cbrt(lms(1));
    lms(2) = std::cbrt(lms(2));

    // Oklab: “Finally, this is transformed into the Lab-coordinates:”
    return GenericColor(static_cast<Mat3d>(oklabM2) * lms);
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromXyzD65ToXyzD50(const GenericColor &value)
{
    return GenericColor( //
        static_cast<Mat3d>(xyzD65ToXyzD50Matrix) * value.toVec3d());
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromXyzD50ToXyzD65(const GenericColor &value)
{
    return GenericColor( //
        static_cast<Mat3d>(xyzD50ToXyzD65Matrix) * value.toVec3d());
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromXyzD50ToCielabD50(const GenericColor &value)
{
    // Conversion function as described in
    // https://en.wikipedia.org/wiki/CIELAB_color_space#From_CIE_XYZ_to_CIELAB

    constexpr Vec3d whitepoint = static_cast<Vec3d>(whitePointD50TwoDegree);

    auto f = [](double t) {
        constexpr double delta = 6.0 / 29.0;
        constexpr double delta2 = delta * delta;
        constexpr double delta3 = delta * delta * delta;
        if (t > delta3) {
            return std::cbrt(t);
        } else {
            return (t / (3.0 * delta2)) + (4.0 / 29.0);
        }
    };

    const auto fx = f(value.first / whitepoint(0));
    const auto fy = f(value.second / whitepoint(1));
    const auto fz = f(value.third / whitepoint(2));

    const double l = 116.0 * fy - 16.0;
    const double a = 500.0 * (fx - fy);
    const double b = 200.0 * (fy - fz);

    return {l, a, b};
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromCielabD50ToXyzD50(const GenericColor &value)
{
    // Conversion function as described in
    // https://en.wikipedia.org/wiki/CIELAB_color_space#From_CIE_XYZ_to_CIELAB

    constexpr Vec3d whitepoint = static_cast<Vec3d>(whitePointD50TwoDegree);

    const double fy = (value.first + 16.0) / 116.0;
    const double fz = fy - value.third / 200.0;
    const double fx = value.second / 500.0 + fy;

    auto f_1 = [](const double f) {
        constexpr double delta = 6.0 / 29.0;
        constexpr double delta2 = delta * delta;
        if (f > delta) {
            return f * f * f;
        } else {
            return 3.0 * delta2 * (f - 4.0 / 29.0);
        }
    };

    const double X = whitepoint(0) * f_1(fx);
    const double Y = whitepoint(1) * f_1(fy);
    const double Z = whitepoint(2) * f_1(fz);

    return {X, Y, Z};
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
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
        result.third = qRadiansToDegrees(std::acos(x / radius));
    } else {
        result.third = qRadiansToDegrees(2 * std::numbers::pi - std::acos(x / radius));
    }
    return result;
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 *
 * This is a generic function converting between polar coordinates
 * (format: ignored, radius, angleDegree, ignored) and Cartesian coordinates
 * (format: ignored, x, y, ignored). */
GenericColor AbsoluteColor::fromPolarToCartesian(const GenericColor &value)
{
    const auto &radius = value.second;
    const auto &angleDegree = value.third;
    const auto angleRadians = qDegreesToRadians(angleDegree);
    return GenericColor(value.first, //
                        radius * std::cos(angleRadians),
                        radius * std::sin(angleRadians));
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromLinearSRgbToSRgb(const GenericColor &value)
{
    return GenericColor(channelFromLinearSRgbToSRgb(value.first), //
                        channelFromLinearSRgbToSRgb(value.second), //
                        channelFromLinearSRgbToSRgb(value.third));
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromSRgbToLinearSRgb(const GenericColor &value)
{
    return GenericColor(channelFromSRgbToLinearSRgb(value.first), //
                        channelFromSRgbToLinearSRgb(value.second), //
                        channelFromSRgbToLinearSRgb(value.third));
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromXyzD65ToLinearSRgb(const GenericColor &value)
{
    auto result = GenericColor( //
        static_cast<Mat3d>(xyzD65ToLinearSRgbMatrix) * value.toVec3d());
    return result;
}

/*!
 *
 *
 * \brief Color conversion.
 *
 * \a value Color to be converted.
 *
 * Returns the converted color
 */
GenericColor AbsoluteColor::fromLinearSRgbToXyzD65(const GenericColor &value)
{
    auto result = GenericColor( //
        static_cast<Mat3d>(linearSRgbToXyzD65Matrix) * value.toVec3d());
    return result;
}

/*!
 * \brief Convert a color from one color model to another.
 *
 * \a from The color model from which the conversion is made.
 *
 * \a value The value being converted.
 *
 * \a to The color model to which the conversion is made.
 *
 * Returns the value converted into the new color model.
 *
 * \note This function is \e {not} speed-optimized. */
std::optional<GenericColor> AbsoluteColor::convert(const ColorModel from, const GenericColor &value, const ColorModel to)
{
    const auto temp = allConversions(from, value);
    if (temp.contains(to)) {
        return temp.value(to);
    }
    return std::nullopt;
}

/*!
 * \brief Conversion to QRgb.
 *
 * \a oklab the original color
 *
 * Returns an opaque sRGB color matching the original one if it is within the
 * gamut. Otherwise, returns a fully transparent color (alpha and RGB channels
 * set to 0 to ensure compatibility with both, premultiplied and
 * non-premultiplied data).
 *
 * \sa AbsoluteColor::fastFromCielchD50ToSRgbClamped()
 *
 *
 *
 * \note This function is optimized for speed. It uses \c {float} instead
 * of \c {double}, as double‑precision arithmetic is roughly 50% slower in
 * this context. All functions called here are defined within the same
 * translation unit, allowing the compiler to have their code inlined.
 * Benchmarks show that these calls introduce no measurable overhead compared
 * to writing the logic directly in place.
 *
 * \note This function is based on the
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-linear-srgb-to-oklab}
 * {original Oklab code}.
 */
QRgb AbsoluteColor::fastFromOklabToSRgbOrTransparent(const GenericColor &oklab)
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

/*!
 * \brief Check if a color is within the sRGB gamut.
 *
 * \a oklab the color
 *
 * Returns \c {true} if the color is in the sRGB gamut.
 * \c {false} otherwise.
 */
bool AbsoluteColor::isOklabInSRgbGamut(const GenericColor &oklab)
{
    if (!isInRange<decltype(oklab.first)>(0, oklab.first, 1)) {
        return false;
    }

    const auto xyzD65 = AbsoluteColor::fromOklabToXyzD65(oklab);
    const auto linearSRgb = AbsoluteColor::fromXyzD65ToLinearSRgb(xyzD65);

    const auto &r = linearSRgb.first;
    const auto &g = linearSRgb.second;
    const auto &b = linearSRgb.third;

    return !(r < 0. || r > 1. || g < 0. || g > 1. || b < 0. || b > 1.);
}

/*!
 * \brief Check if a color is within the sRGB gamut.
 *
 * \a oklch the color
 *
 * Returns \c {true} if the color is in the sRGB gamut.
 * \c {false} otherwise.
 */
bool AbsoluteColor::isOklchInSRgbGamut(const GenericColor &oklch)
{
    return isOklabInSRgbGamut(AbsoluteColor::fromPolarToCartesian(oklch));
}

/*!
 * \brief Check if a color is within the sRGB gamut.
 *
 * \a lab the color
 *
 * \a lchSpace the color space
 *
 * Returns \c {true} if the color is in the sRGB gamut.
 * \c {false} otherwise.
 */
bool AbsoluteColor::isLabInSRgbGamut(const GenericColor &lab, const LchSpace lchSpace)
{
    if (lchSpace == LchSpace::CielchD50) {
        return isCielabD50InSRgbGamut(lab);
    }
    return isOklabInSRgbGamut(lab);
}

/*!
 * \brief Check if a color is within the sRGB gamut.
 *
 * \a lch the color
 *
 * \a lchSpace the color space
 *
 * Returns \c {true} if the color is in the sRGB gamut.
 * \c {false} otherwise.
 */
bool AbsoluteColor::isLchInSRgbGamut(const GenericColor &lch, const LchSpace lchSpace)
{
    if (lchSpace == LchSpace::CielchD50) {
        return isCielchD50InSRgbGamut(lch);
    }
    return isOklchInSRgbGamut(lch);
}

/*!
 * \brief Conversion to QRgb.
 *
 * \a oklch the original color
 *
 * Returns an opaque sRGB color matching the original one if it is within the
 * gamut. Otherwise, returns a more or less similar color.
 *
 * \sa AbsoluteColor::fastFromCielchD50ToSRgbClamped()
 *
 *
 *
 * \note This function is optimized for speed. It uses \c {float} instead
 * of \c {double}, as double‑precision arithmetic is roughly 50% slower in
 * this context. All functions called here are defined within the same
 * translation unit, allowing the compiler to have their code inlined.
 * Benchmarks show that these calls introduce no measurable overhead compared
 * to writing the logic directly in place.
 *
 * \note This function is based on the
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-linear-srgb-to-oklab}
 * {original Oklab code}.
 */
[[nodiscard]] QRgb AbsoluteColor::fastFromOklchToSRgbClamped(const GenericColor &oklch)
{
    const auto oklab = AbsoluteColor::fromPolarToCartesian(oklch);

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

    return qRgba( //
        toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(r), 0, 1)), //
        toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(g), 0, 1)), //
        toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(b), 0, 1)),
        255);
}

/*!
 * \brief Check if a color is within the gamut.
 *
 * \a cielchD50 the color
 *
 * Returns \c {true} if the color is in the gamut.
 * \c {false} otherwise.
 */
bool AbsoluteColor::isCielchD50InSRgbGamut(const GenericColor &cielchD50)
{
    return isCielabD50InSRgbGamut( //
        AbsoluteColor::fromPolarToCartesian(cielchD50));
}

/*!
 * \brief Check if a color is within the gamut.
 *
 * \a cielabD50 the color
 *
 * Returns \c {true} if the color is in the gamut.
 * \c {false} otherwise.
 */
bool AbsoluteColor::isCielabD50InSRgbGamut(const GenericColor &cielabD50)
{
    if (!isInRange<double>(0, cielabD50.first, 100)) {
        return false;
    }

    const auto xyzD50 = AbsoluteColor::fromCielabD50ToXyzD50(cielabD50);
    const auto xyzD65 = AbsoluteColor::fromXyzD50ToXyzD65(xyzD50);
    const auto linearSRgb = AbsoluteColor::fromXyzD65ToLinearSRgb(xyzD65);

    const auto &r = linearSRgb.first;
    const auto &g = linearSRgb.second;
    const auto &b = linearSRgb.third;

    return !(r < 0. || r > 1. || g < 0. || g > 1. || b < 0. || b > 1.);
}

/*!
 * \brief Conversion to QRgb.
 *
 * \b Precondition: Input Lightness: 0 ≤ lightness ≤ 100
 *
 * \a cielabD50 the original color
 *
 * Returns an opaque color matching the original if it is within the gamut.
 *          Otherwise, returns a fully transparent color (alpha and RGB
 *          channels set to 0 to ensure ).
 *
 * \sa fastFromCielchD50ToSRgbClamped
 */
QRgb AbsoluteColor::fastFromCielabD50ToSRgbOrTransparent(const GenericColor &cielabD50)
{
    // Conversion function as described in
    // https://en.wikipedia.org/wiki/CIELAB_color_space#From_CIE_XYZ_to_CIELAB

    constexpr Vec3f whitepoint = static_cast<Vec3f>(whitePointD50TwoDegree);

    const float fy = (static_cast<float>(cielabD50.first) + 16.0f) / 116.0f;
    const float fz = fy - static_cast<float>(cielabD50.third) / 200.0f;
    const float fx = static_cast<float>(cielabD50.second) / 500.0f + fy;

    auto f_1 = [](const float f) {
        constexpr float delta = 6.0f / 29.0f;
        constexpr float delta2 = delta * delta;
        if (f > delta) {
            return f * f * f;
        } else {
            return 3.0f * delta2 * (f - 4.0f / 29.0f);
        }
    };

    const float X = whitepoint(0) * f_1(fx);
    const float Y = whitepoint(1) * f_1(fy);
    const float Z = whitepoint(2) * f_1(fz);

    const Vec3f xyzD50{X, Y, Z};

    constexpr Mat3f xyzD50ToLinearSRgbMatrix = static_cast<Mat3f>( //
        xyzD65ToLinearSRgbMatrix * xyzD50ToXyzD65Matrix);

    auto linearSRgb = xyzD50ToLinearSRgbMatrix * xyzD50;

    const auto &r = linearSRgb(0);
    const auto &g = linearSRgb(1);
    const auto &b = linearSRgb(2);

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

/*!
 * \brief Conversion to QRgb.
 *
 * \a cielchD50 The original color.
 *
 * Returns: If the original color is in-gamut, the corresponding
 * (opaque) in-range RGB value. If the original color is out-of-gamut,
 * a more or less similar (opaque) in-range RGB value.
 *
 * \note There is no guarantee \e {which} specific algorithm is used
 * to fit out-of-gamut colors into the gamut.
 *
 * \sa fastFromCielabD50ToSRgbOrTransparent
 */
QRgb AbsoluteColor::fastFromCielchD50ToSRgbClamped(const GenericColor &cielchD50)
{
    const float angleRadians = qDegreesToRadians( //
        static_cast<float>(cielchD50.third));
    const float radius = static_cast<float>(cielchD50.second);
    const float cielabD50a = radius * std::cos(angleRadians);
    const float cielabD50b = radius * std::sin(angleRadians);

    // Conversion function as described in
    // https://en.wikipedia.org/wiki/CIELAB_color_space#From_CIE_XYZ_to_CIELAB

    constexpr Vec3f whitepoint = static_cast<Vec3f>(whitePointD50TwoDegree);

    const float fy = (static_cast<float>(cielchD50.first) + 16.0f) / 116.0f;
    const float fz = fy - static_cast<float>(cielabD50b) / 200.0f;
    const float fx = static_cast<float>(cielabD50a) / 500.0f + fy;

    auto f_1 = [](const float f) {
        constexpr float delta = 6.0f / 29.0f;
        constexpr float delta2 = delta * delta;
        if (f > delta) {
            return f * f * f;
        } else {
            return 3.0f * delta2 * (f - 4.0f / 29.0f);
        }
    };

    const float X = whitepoint(0) * f_1(fx);
    const float Y = whitepoint(1) * f_1(fy);
    const float Z = whitepoint(2) * f_1(fz);

    const Vec3f xyzD50{X, Y, Z};

    constexpr Mat3f xyzD50ToLinearSRgbMatrix = static_cast<Mat3f>( //
        xyzD65ToLinearSRgbMatrix * xyzD50ToXyzD65Matrix);

    auto linearSRgb = xyzD50ToLinearSRgbMatrix * xyzD50;

    const auto &r = linearSRgb(0);
    const auto &g = linearSRgb(1);
    const auto &b = linearSRgb(2);

    return qRgba( //
        toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(r), 0.f, 1.f)), //
        toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(g), 0.f, 1.f)), //
        toByte(std::clamp<float>(channelFromLinearSRgbToSRgb(b), 0.f, 1.f)),
        255);
}

/*!
 * \brief Reduces the chroma until the color fits into the gamut.
 *
 * It always preserves the hue. It preservers the lightness whenever
 * possible.
 *
 * \note In some cases with very curvy color spaces, the nearest in-gamut
 * color (with the same lightness and hue) might be at \e {higher}
 * chroma. As this function always \e {reduces} the chroma,
 * in this case the result is not the nearest in-gamut color.
 *
 * \a lch The color that will be adapted.
 *
 * \a lchSpace The color space
 *
 * Returns an sRGB in-gamut color. */
GenericColor AbsoluteColor::reduceChromaToFitIntoGamut(const GenericColor &lch, const LchSpace lchSpace)
{
    GenericColor referenceColor = lch;

    // Normalize the LCH coordinates
    normalizePolar360(referenceColor.second, referenceColor.third);

    // Bound to valid range:
    const auto maxChroma = (lchSpace == LchSpace::CielchD50) //
        ? ColorSpaceInfo::maxCielchD50Chroma() //
        : ColorSpaceInfo::maxOklchChroma();
    referenceColor.second = qMin<double>(referenceColor.second, maxChroma);
    const auto minLightness = (lchSpace == LchSpace::CielchD50) //
        ? ColorSpaceInfo::cielabD50BlackpointL() //
        : ColorSpaceInfo::oklabBlackpointL();
    const auto maxLightness = (lchSpace == LchSpace::CielchD50) //
        ? ColorSpaceInfo::cielabD50WhitepointL() //
        : ColorSpaceInfo::oklabWhitepointL();
    referenceColor.first = qBound<double>(minLightness,
                                          referenceColor.first, //
                                          maxLightness);

    // Test special case: If we are yet in-gamut…
    if (AbsoluteColor::isLchInSRgbGamut(referenceColor, lchSpace)) {
        return referenceColor;
    }

    // Now we know: We are out-of-gamut.
    GenericColor temp;

    // Create an in-gamut point on the gray axis:
    GenericColor lowerChroma{referenceColor.first, 0, referenceColor.third};
    if (!AbsoluteColor::isLchInSRgbGamut(lowerChroma, lchSpace)) {
        // This is quite strange because every point between the blackpoint
        // and the whitepoint on the gray axis should be in-gamut on
        // normally shaped gamuts. But as we never know, we need a fallback,
        // which is guaranteed to be in-gamut. The blackpoint is 0 and it
        // is in-gamut for sRGB both in CielchD50 projection as in Oklab
        // projection.
        referenceColor.first = 0;
        lowerChroma.first = 0;
    }
    // Do a quick-approximate search:
    GenericColor upperChroma{referenceColor};
    // Now we know for sure that lowerChroma is in-gamut
    // and upperChroma is out-of-gamut…
    temp = upperChroma;
    while (upperChroma.second - lowerChroma.second > gamutPrecisionOklab) {
        // Our test candidate is half the way between lowerChroma
        // and upperChroma:
        temp.second = ((lowerChroma.second + upperChroma.second) / 2);
        if (isLchInSRgbGamut(temp, lchSpace)) {
            lowerChroma = temp;
        } else {
            upperChroma = temp;
        }
    }
    return lowerChroma;
}

} // namespace PerceptualColor
