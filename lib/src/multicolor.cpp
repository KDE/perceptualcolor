// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own header
#include "multicolor.h"

#include "PerceptualColor/lchdouble.h"
#include "helperconversion.h"
#include "helperqttypes.h"
#include "rgbcolorspace.h"
#include "rgbdouble.h"
#include <QtCore/qsharedpointer.h>
#include <lcms2.h>
#include <qglobal.h>
#include <type_traits>

namespace PerceptualColor
{

// TODO xxx Decide on the commented-out static_assert statements. Ideally
// get them working!

// static_assert(std::is_trivially_copyable_v<MultiColor>);
// static_assert(std::is_trivial_v<MultiColor>);

static_assert(std::is_standard_layout_v<MultiColor>);

static_assert(std::is_default_constructible_v<MultiColor>);
// static_assert(std::is_trivially_default_constructible_v<MultiColor>);
static_assert(std::is_nothrow_default_constructible_v<MultiColor>);

static_assert(std::is_copy_constructible_v<MultiColor>);
// static_assert(std::is_trivially_copy_constructible_v<MultiColor>);
// static_assert(std::is_nothrow_copy_constructible_v<MultiColor>);

// static_assert(std::is_move_constructible_v<MultiColor>);
// static_assert(std::is_trivially_move_constructible_v<MultiColor>);
// static_assert(std::is_nothrow_move_constructible_v<MultiColor>);

/** @brief Fills the Lab-based color formats based on the RGB-based color
 * formats.
 *
 * @pre The values for RGB-based color formats are correct: @ref hsl, @ref hsv,
 * @ref hwb, @ref rgb, @ref rgbQColor.
 *
 * @post The values for Lab-based color formats are set
 * accordingly: @ref ciehlc, @ref cielch.
 *
 * @param colorSpace The color space in which the object is created. */
void MultiColor::fillLchAndDerivatesFromRgbAndDerivates(const QSharedPointer<RgbColorSpace> &colorSpace)
{
    cielch = colorSpace->toCielchDouble(rgbQColor.rgba64());
    if (cielch.c < colorDifferenceThreshold) {
        cielch.c = 0;
        // Get a similar, but more chromatic color. To do so, we raise the
        // HSL saturation. If the color is black or white or a nearby color,
        // we also move it a bit towards neutral gray, because black and
        // white to not allow to raise chroma.
        const double correctedLightness = qBound( //
            colorDifferenceThreshold, //
            hsl.at(2), //
            100 - colorDifferenceThreshold);
        // Changing the HSL-saturation near to black or near to white results
        // in an actual chroma (!) change which is much lower than when the
        // HSL-lightness is 50%. Therefore, we use a correction factor that
        // makes sure we get the same chroma change all over the full
        // HSL-lightness range.
        const double hslSaturationCorrectionFactor = //
            50 / (50 - qAbs(50 - correctedLightness));
        // Apply the correction factor. Note that correctedHslSaturation might
        // be out-of-range now.
        const double correctedHslSaturation = hsl.at(1) //
            + colorDifferenceThreshold * hslSaturationCorrectionFactor;
        const QColor saturatedHslQColor = //
            QColor::fromHslF( //
                static_cast<QColorFloatType>(qBound(0., hsl.at(0) / 360, 1.)),
                static_cast<QColorFloatType>(qBound(0., correctedHslSaturation / 100, 1.)),
                static_cast<QColorFloatType>(qBound(0., correctedLightness / 100, 1.)) //
                )
                .toRgb();
        cielch.h = colorSpace->toCielchDouble(saturatedHslQColor.rgba64()).h;
        // Changing the hue might make the color out-of-gamut because some
        // gamuts have strange forms around the white point and the black
        // point. We push it back into the gamut:
        cielch = colorSpace->reduceChromaToFitIntoGamut(cielch);
    }

    ciehlc = QList<double>({cielch.h, cielch.l, cielch.c});

    const auto cielab = colorSpace->toCielab(rgbQColor.rgba64());
    const auto oklab = fromCmscielabD50ToOklab(cielab);
    // TODO xxx Missing support of Oklch to prevent arbitrary hue changes
    // near the gray axis, like we prevent it yet for cielch.
    const auto oklchdouble = toCielchDouble(oklab);
    oklch = QList<double>({oklchdouble.l, oklchdouble.c, oklchdouble.h});
}

/** @brief Set all RGB-based color formats.
 *
 * @param color The new color as <tt>QColor</tt> object. Might be of any
 * <tt>QColor::Spec</tt>.
 * @param hue When empty, the hue is calculated automatically. Otherwise,
 * this value is used instead. Valid range: [0, 360[
 *
 * @post @ref hsl, @ref hsv, @ref hwb, @ref rgb and @ref rgbQColor are set. */
void MultiColor::fillRgbAndDerivates(QColor color, std::optional<double> hue)
{
    rgb = QList<double>({static_cast<double>(color.redF() * 255), //
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

/** @brief Converts from @ref RgbDouble to a <tt>QColor</tt> with
 * <tt>spec()</tt> value <tt>QColor::Rgb</tt>.
 *
 * @param color The original color
 *
 * @return The same color as <tt>QColor</tt> with
 * <tt>spec()</tt> value <tt>QColor::Rgb</tt>.
 * If the original color has out-of-range values,
 * than these values are silently clipped to the
 * valid range. */
QColor MultiColor::fromRgbDoubleToQColor(const RgbDouble &color)
{
    return QColor::fromRgbF( //
        static_cast<QColorFloatType>(qBound<QColorFloatType>(0, color.red, 1)), //
        static_cast<QColorFloatType>(qBound<QColorFloatType>(0, color.green, 1)), //
        static_cast<QColorFloatType>(qBound<QColorFloatType>(0, color.blue, 1)));
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color Original color
 * @returns A @ref MultiColor object representing this color.
 * @note The original color will neither be normalised nor moved into gamut.
 * If it’s an out-of-gamut color, the resulting RGB-based representations will
 * nevertheless be in-gamut and therefore be an incorrect color. */
MultiColor MultiColor::fromLch(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &color)
{
    MultiColor result;
    result.cielch = color;
    result.ciehlc = QList<double>({result.cielch.h, result.cielch.l, result.cielch.c});
    const cmsCIELCh tempcmscielch = toCmsCieLch(result.cielch);
    const cmsCIELab tempcmscielab = toCmsCieLab(tempcmscielch);
    const auto cmsoklab = fromCmscielabD50ToOklab(tempcmscielab);
    const auto oklchdouble = toCielchDouble(cmsoklab);
    // TODO xxx Missing support of Oklch to prevent arbitrary hue changes
    // near the gray axis, like we prevent it yet for cielch vs RGB.
    result.oklch = QList<double>({oklchdouble.l, oklchdouble.c, oklchdouble.h});
    std::optional<double> hue;
    if (result.cielch.c < colorDifferenceThreshold) {
        // If we are very close to the cylindrical axis, a big numeric
        // difference in the hue is a very small difference in color. On the
        // cylindrical axis itself the hue is completely meaningless. However,
        // a hue value that is jumping during conversion from LCH/HLC to HSL
        // (because the conversion can have rounding errors and because the
        // gray axis of LCH and RGB is not necessarily exactly identical)
        // is confusing. Therefore, for values that are near to the cylindrical
        // axis, we snap them exactly to the axis, and then use the hue
        // that corresponds to the same color, but with slightly higher
        // chroma/saturation.
        LchDouble saturatedLch = result.cielch;
        // Avoid black and white, as for these values, non-zero chroma is
        // out-of-gamut and therefore would not produce a meaningful result.
        saturatedLch.l = qBound(colorDifferenceThreshold, //
                                result.cielch.l, //
                                100 - colorDifferenceThreshold);
        // Use a more saturated value:
        saturatedLch.c = colorDifferenceThreshold;
        const RgbDouble saturatedRgbDouble = colorSpace->toRgbDoubleUnbound( //
            saturatedLch);
        hue = fromRgbDoubleToQColor(saturatedRgbDouble).hueF() * 360;
    }
    const RgbDouble temp = colorSpace->toRgbDoubleUnbound(color);
    const QList<double> newRgbList({temp.red * 255, //
                                    temp.green * 255, //
                                    temp.blue * 255});
    const QColor newRgbQColor = fromRgbDoubleToQColor(temp);
    result.fillRgbAndDerivates(newRgbQColor, hue);
    // Override with original values:
    result.rgb = newRgbList;
    return result;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color Original color.
 * @returns A @ref MultiColor object representing this color. */
MultiColor MultiColor::fromRgb(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color)
{
    MultiColor result;
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
    result.fillRgbAndDerivates(newRgbQColor, std::optional<double>());
    result.rgb = color;

    result.fillLchAndDerivatesFromRgbAndDerivates(colorSpace);

    return result;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color Original color.
 * @returns A @ref MultiColor object representing this color. */
MultiColor MultiColor::fromRgbQColor(const QSharedPointer<RgbColorSpace> &colorSpace, const QColor &color)
{
    MultiColor result;
    result.fillRgbAndDerivates(color, std::optional<double>());

    result.fillLchAndDerivatesFromRgbAndDerivates(colorSpace);

    return result;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color Original color.
 * @returns A @ref MultiColor object representing this color. */
MultiColor MultiColor::fromHsl(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color)
{
    MultiColor result;
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
    result.fillRgbAndDerivates(newRgbQColor, color.at(0));
    // Override again with the original value:
    result.hsl = color;
    if (result.hsl.at(2) == 0) {
        // Color is black. So neither changing HSV-saturation or changing
        // HSL-saturation will change the color itself. To give a better
        // user experience, we synchronize both values.
        result.hsv[1] = result.hsl.at(1);
    }

    result.fillLchAndDerivatesFromRgbAndDerivates(colorSpace);

    return result;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color Original color.
 * @returns A @ref MultiColor object representing this color. */
MultiColor MultiColor::fromHsv(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color)
{
    MultiColor result;
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
    result.fillRgbAndDerivates(newRgbQColor, color.at(0));
    // Override again with the original value:
    result.hsv = color;
    if (result.hsv.at(2) == 0) {
        // Color is black. So neither changing HSV-saturation or changing
        // HSL-saturation will change the color itself. To give a better
        // user experience, we synchronize both values.
        result.hsl[1] = result.hsv.at(1);
    }

    result.fillLchAndDerivatesFromRgbAndDerivates(colorSpace);

    return result;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color Original color.
 * @returns A @ref MultiColor object representing this color. */
MultiColor MultiColor::fromHwb(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color)
{
    MultiColor result;
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
    result.fillRgbAndDerivates(newRgbQColor, normalizedHwb.at(0));
    // Override again with the original value:
    result.hsv = newHsv;
    result.hwb = color; // Intentionally not normalized, but original value.

    result.fillLchAndDerivatesFromRgbAndDerivates(colorSpace);

    return result;
}

/** @brief Equal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if all data members have exactly the same
 * coordinates. <tt>false</tt> otherwise. */
bool MultiColor::operator==(const MultiColor &other) const
{
    // Test equality for all data members
    return (ciehlc == other.ciehlc) //
        && (cielch.hasSameCoordinates(other.cielch)) //
        && (hsl == other.hsl) //
        && (hsv == other.hsv) //
        && (hwb == other.hwb) //
        && (oklch == other.oklch) //
        && (rgb == other.rgb) //
        && (rgbQColor == other.rgbQColor);
}

/** @internal
 *
 * @brief Adds QDebug() support for data type
 * @ref PerceptualColor::MultiColor
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::MultiColor &value)
{
    dbg.nospace() //
        << "MultiColor(\n"
        << " - ciehlc: " << value.ciehlc << "\n"
        << " - cielch: " << value.cielch.l << "\n"
        << " - hsl: " << value.hsl << "\n"
        << " - hsv: " << value.hsv << "\n"
        << " - hwb: " << value.hwb << "\n"
        << " - oklch: " << value.oklch << "\n"
        << " - rgb: " << value.rgb << "\n"
        << " - rgbQColor: " << value.rgbQColor << "\n"
        << ")";
    return dbg.maybeSpace();
}

static_assert(std::is_standard_layout_v<MultiColor>);

} // namespace PerceptualColor
