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
    : cielch{0, 0, 0}
{
}

/** @brief Fills the Lab-based color formats based on the RGB-based color
 * formats.
 *
 * @pre The values for RGB-based color formats are correct: @ref multiRgb
 *
 * @post The values for Lab-based color formats are set
 * accordingly: @ref ciehlc, @ref cielch.
 *
 * @param colorSpace The color space in which the object is created. */
void MultiColor::fillLchAndDerivatesFromRgbAndDerivates(const QSharedPointer<RgbColorSpace> &colorSpace)
{
    cielch = colorSpace->toCielchDouble(multiRgb.rgbQColor.rgba64());
    if (cielch.c < colorDifferenceThreshold) {
        cielch.c = 0;
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
        cielch.h = colorSpace->toCielchDouble(saturatedHslQColor.rgba64()).h;
        // Changing the hue might make the color out-of-gamut because some
        // gamuts have strange forms around the white point and the black
        // point. We push it back into the gamut:
        cielch = colorSpace->reduceChromaToFitIntoGamut(cielch);
    }

    ciehlc = QList<double>({cielch.h, cielch.l, cielch.c});

    const auto cielab = colorSpace->toCielab(multiRgb.rgbQColor.rgba64());
    const auto oklab = fromCmscielabD50ToOklab(cielab);
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
    result.cielch = color;
    result.ciehlc = QList<double>({result.cielch.h, result.cielch.l, result.cielch.c});
    const cmsCIELCh tempcmscielch = toCmsLch(result.cielch);
    const cmsCIELab tempcmscielab = toCmsLab(tempcmscielch);
    const auto cmsoklab = fromCmscielabD50ToOklab(tempcmscielab);
    const auto oklchdouble = toLchDouble(cmsoklab);
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
        // TODO xxx The hue shouldn’t be calculated like here by QColor::hueF()
        // but rather using our own MultiRgb. Reasons:
        // 1. Consistency!
        // 2. If in the future MultiRgb becomes more accurate, this code
        //    would also benefit from this.
        hue = fromRgbDoubleToQColor(saturatedRgbDouble).hueF() * 360;
    }
    const RgbDouble temp = colorSpace->toRgbDoubleUnbound(color);
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
    return (ciehlc == other.ciehlc) //
        && (cielch.hasSameCoordinates(other.cielch)) //
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
        << " - ciehlc: " << value.ciehlc << "\n"
        << " - cielch: " << value.cielch.l << "\n"
        << " - multirgb: " << value.multiRgb << "\n"
        << " - oklch: " << value.oklch << "\n"
        << ")";
    return dbg.maybeSpace();
}

static_assert(std::is_standard_layout_v<MultiColor>);

} // namespace PerceptualColor
