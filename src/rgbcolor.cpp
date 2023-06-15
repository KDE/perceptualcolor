// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "rgbcolor.h"

#include "helperqttypes.h"
#include <qglobal.h>
#include <type_traits>

namespace PerceptualColor
{

// TODO xxx Decide on the commented-out static_assert statements. Ideally
// get them working!

// static_assert(std::is_trivially_copyable_v<RgbColor>);
// static_assert(std::is_trivial_v<RgbColor>);

static_assert(std::is_standard_layout_v<RgbColor>);

static_assert(std::is_default_constructible_v<RgbColor>);
// static_assert(std::is_trivially_default_constructible_v<RgbColor>);
// static_assert(std::is_nothrow_default_constructible_v<RgbColor>);

static_assert(std::is_copy_constructible_v<RgbColor>);
// static_assert(std::is_trivially_copy_constructible_v<RgbColor>);
// static_assert(std::is_nothrow_copy_constructible_v<RgbColor>);

// static_assert(std::is_move_constructible_v<RgbColor>);
// static_assert(std::is_trivially_move_constructible_v<RgbColor>);
// static_assert(std::is_nothrow_move_constructible_v<RgbColor>);

RgbColor::RgbColor()
{
}

/** @brief Set all member variables.
 *
 * @param color The new color as <tt>QColor</tt> object. Might be of any
 * <tt>QColor::Spec</tt>.
 * @param hue When empty, the hue is calculated automatically. Otherwise,
 * this value is used instead. Valid range: [0, 360[
 *
 * @post @ref hsl, @ref hsv, @ref hwb, @ref rgb255 and @ref rgbQColor are
 * set. */
void RgbColor::fillAll(QColor color, std::optional<double> hue)
{
    rgb255 = QList<double>({static_cast<double>(color.redF() * 255), //
                            static_cast<double>(color.greenF() * 255), //
                            static_cast<double>(color.blueF() * 255)});

    rgbQColor = color.toRgb();

    // The hue is identical for HSL, HSV and HWB.
    const double hueDegree = hue.value_or( //
        qBound(0., rgbQColor.hueF() * 360, 360.));

    // HSL
    const double hslSaturationPercentage = //
        qBound(0., static_cast<double>(color.hslSaturationF()) * 100, 100.);
    const double hslLightnessPercentage = //
        qBound(0., static_cast<double>(color.lightnessF()) * 100, 100.);
    hsl = QList<double>({hueDegree, //
                         hslSaturationPercentage, //
                         hslLightnessPercentage});

    // HSV
    const double hsvSaturationPercentage = //
        qBound(0.0, static_cast<double>(color.hsvSaturationF()) * 100, 100.0);
    const double hsvValuePercentage = //
        qBound<double>(0.0, static_cast<double>(color.valueF()) * 100, 100.0);
    hsv = QList<double>({hueDegree, //
                         hsvSaturationPercentage, //
                         hsvValuePercentage});

    const double hwbWhitenessPercentage = //
        qBound(0.0, (1 - color.hsvSaturationF()) * color.valueF() * 100, 100.0);
    const double hwbBlacknessPercentage = //
        qBound(0.0, (1 - color.valueF()) * 100, 100.0);
    hwb = QList<double>({hueDegree, //
                         hwbWhitenessPercentage, //
                         hwbBlacknessPercentage});
}

/** @brief Static convenience function that returns a @ref RgbColor
 * constructed from the given color.
 *
 * @param color Original color. Valid range: [0, 255]
 * @param hue If not empty, this value is used instead of the actually
 *            calculated hue value. Valid range: [0, 360[
 * @returns A @ref RgbColor object representing this color. */
RgbColor RgbColor::fromRgb255(const QList<double> &color, std::optional<double> hue)
{
    RgbColor result;
    if (color.count() < 3) {
        qWarning() << "The private-API function" << __func__ //
                   << "in file" << __FILE__ //
                   << "near to line" << __LINE__ //
                   << "was called with the invalid “color“ argument “" //
                   << color //
                   << "” that does not have exactly 3 values." //
                   << "An uninitialized value was returned. This is a bug.";
        return result;
    }
    const auto red = static_cast<QColorFloatType>(color.at(0) / 255.0);
    const auto green = static_cast<QColorFloatType>(color.at(1) / 255.0);
    const auto blue = static_cast<QColorFloatType>(color.at(2) / 255.0);
    constexpr auto zero = static_cast<QColorFloatType>(0);
    constexpr auto one = static_cast<QColorFloatType>(1);
    const QColor newRgbQColor = QColor::fromRgbF(qBound(zero, red, one), //
                                                 qBound(zero, green, one), //
                                                 qBound(zero, blue, one));
    result.fillAll(newRgbQColor, hue);
    result.rgb255 = color;

    return result;
}

/** @brief Static convenience function that returns a @ref RgbColor
 * constructed from the given color.
 *
 * @param color Original color.
 * @returns A @ref RgbColor object representing this color. */
RgbColor RgbColor::fromRgbQColor(const QColor &color)
{
    RgbColor result;
    result.fillAll(color, std::optional<double>());

    return result;
}

/** @brief Static convenience function that returns a @ref RgbColor
 * constructed from the given color.
 *
 * @param color Original color.
 * @returns A @ref RgbColor object representing this color. */
RgbColor RgbColor::fromHsl(const QList<double> &color)
{
    RgbColor result;
    if (color.count() < 3) {
        qWarning() << "The private-API function" << __func__ //
                   << "in file" << __FILE__ //
                   << "near to line" << __LINE__ //
                   << "was called with the invalid “color“ argument “" //
                   << color //
                   << "” that does not have exactly 3 values." //
                   << "An uninitialized value was returned. This is a bug.";
        return result;
    }

    constexpr auto zero = static_cast<QColorFloatType>(0);
    constexpr auto one = static_cast<QColorFloatType>(1);
    const auto hslHue = //
        qBound(zero, static_cast<QColorFloatType>(color.at(0) / 360.0), one);
    const auto hslSaturation = //
        qBound(zero, static_cast<QColorFloatType>(color.at(1) / 100.0), one);
    const auto hslLightness = //
        qBound(zero, static_cast<QColorFloatType>(color.at(2) / 100.0), one);
    const QColor newRgbQColor = //
        QColor::fromHslF(hslHue, hslSaturation, hslLightness).toRgb();
    result.fillAll(newRgbQColor, color.at(0));
    // Override again with the original value:
    result.hsl = color;
    if (result.hsl.at(2) == 0) {
        // Color is black. So neither changing HSV-saturation or changing
        // HSL-saturation will change the color itself. To give a better
        // user experience, we synchronize both values.
        result.hsv[1] = result.hsl.at(1);
    }

    return result;
}

/** @brief Static convenience function that returns a @ref RgbColor
 * constructed from the given color.
 *
 * @param color Original color.
 * @returns A @ref RgbColor object representing this color. */
RgbColor RgbColor::fromHsv(const QList<double> &color)
{
    RgbColor result;
    if (color.count() < 3) {
        qWarning() << "The private-API function" << __func__ //
                   << "in file" << __FILE__ //
                   << "near to line" << __LINE__ //
                   << "was called with the invalid “color“ argument “" //
                   << color //
                   << "” that does not have exactly 3 values." //
                   << "An uninitialized value was returned. This is a bug.";
        return result;
    }
    constexpr auto zero = static_cast<QColorFloatType>(0);
    constexpr auto one = static_cast<QColorFloatType>(1);
    const auto hsvHue = //
        qBound(zero, static_cast<QColorFloatType>(color.at(0) / 360.0), one);
    const auto hsvSaturation = //
        qBound(zero, static_cast<QColorFloatType>(color.at(1) / 100.0), one);
    const auto hsvValue = //
        qBound(zero, static_cast<QColorFloatType>(color.at(2) / 100.0), one);
    const QColor newRgbQColor = //
        QColor::fromHsvF(hsvHue, hsvSaturation, hsvValue);
    result.fillAll(newRgbQColor, color.at(0));
    // Override again with the original value:
    result.hsv = color;
    if (result.hsv.at(2) == 0) {
        // Color is black. So neither changing HSV-saturation or changing
        // HSL-saturation will change the color itself. To give a better
        // user experience, we synchronize both values.
        result.hsl[1] = result.hsv.at(1);
    }

    return result;
}

/** @brief Static convenience function that returns a @ref RgbColor
 * constructed from the given color.
 *
 * @param color Original color.
 * @returns A @ref RgbColor object representing this color. */
RgbColor RgbColor::fromHwb(const QList<double> &color)
{
    RgbColor result;
    if (color.count() < 3) {
        qWarning() << "The private-API function" << __func__ //
                   << "in file" << __FILE__ //
                   << "near to line" << __LINE__ //
                   << "was called with the invalid “color“ argument “" //
                   << color //
                   << "” that does not have exactly 3 values." //
                   << "An uninitialized value was returned. This is a bug.";
        return result;
    }
    QList<double> normalizedHwb = color;
    const auto whitenessBlacknessSum = //
        normalizedHwb.at(1) + normalizedHwb.at(2);
    if (whitenessBlacknessSum > 100) {
        normalizedHwb[1] *= 100 / whitenessBlacknessSum;
        normalizedHwb[2] *= 100 / whitenessBlacknessSum;
    }

    const double quotient = (100 - normalizedHwb.at(2));
    const auto newHsvSaturation = //
        (quotient == 0) // This is only the case for pure black.
        ? 0 // Avoid division by 0 in the formula below. Instead, set
            // an arbitrary (in-range) value, because the HSV saturation
            // is meaningless when value/brightness is 0, which is the case
            // for black.
        : qBound<double>(0, 100 - normalizedHwb.at(1) / quotient * 100, 100);
    const auto newHsvValue = qBound<double>(0, 100 - normalizedHwb.at(2), 100);
    const QList<double> newHsv = QList<double>({normalizedHwb.at(0), //
                                                newHsvSaturation, //
                                                newHsvValue});
    const QColor newRgbQColor = //
        QColor::fromHsvF( //
            static_cast<QColorFloatType>(newHsv.at(0) / 360), //
            static_cast<QColorFloatType>(newHsv.at(1) / 100), //
            static_cast<QColorFloatType>(newHsv.at(2) / 100));
    result.fillAll(newRgbQColor, normalizedHwb.at(0));
    // Override again with the original value:
    result.hsv = newHsv;
    result.hwb = color; // Intentionally not normalized, but original value.

    return result;
}

/** @brief Equal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if all data members have exactly the same
 * coordinates. <tt>false</tt> otherwise. */
bool RgbColor::operator==(const RgbColor &other) const
{
    // Test equality for all data members
    return (hsl == other.hsl) //
        && (hsv == other.hsv) //
        && (hwb == other.hwb) //
        && (rgb255 == other.rgb255) //
        && (rgbQColor == other.rgbQColor);
}

/** @internal
 *
 * @brief Adds QDebug() support for data type
 * @ref PerceptualColor::RgbColor
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::RgbColor &value)
{
    dbg.nospace() //
        << "RgbColor(\n"
        << " - hsl: " << value.hsl << "\n"
        << " - hsv: " << value.hsv << "\n"
        << " - hwb: " << value.hwb << "\n"
        << " - rgb: " << value.rgb255 << "\n"
        << " - rgbQColor: " << value.rgbQColor << "\n"
        << ")";
    return dbg.maybeSpace();
}

static_assert(std::is_standard_layout_v<RgbColor>);

} // namespace PerceptualColor
