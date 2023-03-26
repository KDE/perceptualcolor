// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "multicolor.h"

#include "helperconversion.h"
#include "helperqttypes.h"
#include "lchdouble.h"
#include "multirgb.h"
#include "rgbcolorspace.h"
#include "rgbdouble.h"
#include <QtCore/qsharedpointer.h>
#include <lcms2.h>
#include <optional>
#include <qcolor.h>
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
// static_assert(std::is_nothrow_default_constructible_v<MultiColor>);

static_assert(std::is_copy_constructible_v<MultiColor>);
// static_assert(std::is_trivially_copy_constructible_v<MultiColor>);
// static_assert(std::is_nothrow_copy_constructible_v<MultiColor>);

// static_assert(std::is_move_constructible_v<MultiColor>);
// static_assert(std::is_trivially_move_constructible_v<MultiColor>);
// static_assert(std::is_nothrow_move_constructible_v<MultiColor>);

MultiColor::MultiColor()
    : cielchD50{0, 0, 0}
{
}

/** @brief Fills the Lab-based color formats based on the RGB-based color
 * formats.
 *
 * @pre The values for RGB-based color formats are correct: @ref multiRgb
 *
 * @post The values for Lab-based color formats are set
 * accordingly: @ref ciehlcD50, @ref cielchD50.
 *
 * @param colorSpace The color space in which the object is created. */
void MultiColor::fillLchAndDerivatesFromRgbAndDerivates(const QSharedPointer<RgbColorSpace> &colorSpace)
{
    cielchD50 = colorSpace->toCielchD50Double(multiRgb.rgbQColor.rgba64());
    if (cielchD50.c < colorDifferenceThreshold) {
        cielchD50.c = 0;
        // Get a similar, but more chromatic color. To do so, we raise the
        // HSL saturation. If the color is black or white or a nearby color,
        // we also move it a bit towards neutral gray, because black and
        // white to not allow to raise chroma.
        const double correctedLightness = qBound( //
            colorDifferenceThreshold, //
            multiRgb.hsl.at(2), //
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
        const double correctedHslSaturation = multiRgb.hsl.at(1) //
            + colorDifferenceThreshold * hslSaturationCorrectionFactor;
        const QColor saturatedHslQColor = //
            QColor::fromHslF( //
                static_cast<QColorFloatType>(qBound(0., multiRgb.hsl.at(0) / 360, 1.)),
                static_cast<QColorFloatType>(qBound(0., correctedHslSaturation / 100, 1.)),
                static_cast<QColorFloatType>(qBound(0., correctedLightness / 100, 1.)) //
                )
                .toRgb();
        cielchD50.h = colorSpace->toCielchD50Double(saturatedHslQColor.rgba64()).h;
        // Changing the hue might make the color out-of-gamut because some
        // gamuts have strange forms around the white point and the black
        // point. We push it back into the gamut:
        cielchD50 = colorSpace->reduceChromaToFitIntoGamut(cielchD50);
    }

    ciehlcD50 = QList<double>({cielchD50.h, cielchD50.l, cielchD50.c});

    const auto cielabD50 = colorSpace->toCielabD50(multiRgb.rgbQColor.rgba64());
    const auto oklab = fromCmscielabD50ToOklab(cielabD50);
    // TODO xxx Missing support of Oklch to prevent arbitrary hue changes
    // near the gray axis, like we prevent it yet for cielch.
    const auto oklchdouble = toLchDouble(oklab);
    oklch = QList<double>({oklchdouble.l, oklchdouble.c, oklchdouble.h});
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
MultiColor MultiColor::fromCielch(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &color)
{
    MultiColor result;
    result.cielchD50 = color;
    result.ciehlcD50 = QList<double>({result.cielchD50.h, result.cielchD50.l, result.cielchD50.c});
    const cmsCIELCh tempcmscielch = toCmsLch(result.cielchD50);
    const cmsCIELab tempcmscielab = toCmsLab(tempcmscielch);
    const auto cmsoklab = fromCmscielabD50ToOklab(tempcmscielab);
    const auto oklchdouble = toLchDouble(cmsoklab);
    // TODO xxx Missing support of Oklch to prevent arbitrary hue changes
    // near the gray axis, like we prevent it yet for cielch vs RGB.
    result.oklch = QList<double>({oklchdouble.l, oklchdouble.c, oklchdouble.h});
    std::optional<double> hue;
    if (result.cielchD50.c < colorDifferenceThreshold) {
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
        LchDouble saturatedCielchD50 = result.cielchD50;
        // Avoid black and white, as for these values, non-zero chroma is
        // out-of-gamut and therefore would not produce a meaningful result.
        saturatedCielchD50.l = qBound(colorDifferenceThreshold, //
                                      result.cielchD50.l, //
                                      100 - colorDifferenceThreshold);
        // Use a more saturated value:
        saturatedCielchD50.c = colorDifferenceThreshold;
        const RgbDouble saturatedRgbDouble = colorSpace->fromCielchD50ToRgbDoubleUnbound( //
            saturatedCielchD50);
        // TODO xxx The hue shouldn’t be calculated like here by QColor::hueF()
        // but rather using our own MultiRgb. Reasons:
        // 1. Consistency!
        // 2. If in the future MultiRgb becomes more accurate, this code
        //    would also benefit from this.
        hue = fromRgbDoubleToQColor(saturatedRgbDouble).hueF() * 360;
    }
    const RgbDouble temp = colorSpace->fromCielchD50ToRgbDoubleUnbound(color);
    const QList<double> newRgbList({temp.red * 255, //
                                    temp.green * 255, //
                                    temp.blue * 255});
    result.multiRgb = MultiRgb::fromRgb(newRgbList, hue);

    return result;
}

/** @brief Static convenience function that returns a @ref MultiColor
 * constructed from the given color.
 *
 * @param colorSpace The color space in which the object is created.
 * @param color Original color.
 * @returns A @ref MultiColor object representing this color. */
MultiColor MultiColor::fromMultiRgb(const QSharedPointer<RgbColorSpace> &colorSpace, const MultiRgb &color)
{
    MultiColor result;
    result.multiRgb = color;
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
    return (ciehlcD50 == other.ciehlcD50) //
        && (cielchD50.hasSameCoordinates(other.cielchD50)) //
        && (multiRgb == other.multiRgb) //
        && (oklch == other.oklch);
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
        << " - ciehlc: " << value.ciehlcD50 << "\n"
        << " - cielch: " << value.cielchD50.l << "\n"
        << " - multirgb: " << value.multiRgb << "\n"
        << " - oklch: " << value.oklch << "\n"
        << ")";
    return dbg.maybeSpace();
}

static_assert(std::is_standard_layout_v<MultiColor>);

} // namespace PerceptualColor
