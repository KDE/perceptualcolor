// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef MULTICOLOR_H
#define MULTICOLOR_H

#include "lchdouble.h"
#include <optional>
#include <qcolor.h>
#include <qdebug.h>
#include <qlist.h>
#include <qsharedpointer.h>

namespace PerceptualColor
{
class RgbColorSpace;
struct RgbDouble;

/** @internal
 *
 * @brief A color stored in multiple different color formats.
 *
 * Unlike <tt>QColor</tt> (which is essentially a C++ <tt>union</tt> of
 * different color formats, so only one of them is actually saved),
 * @ref MultiColor actually stores <em>all</em> available color formats.
 *
 * This data type is just an implementation detail of @ref ColorDialog.
 * For simplicity, data members are accessible directly, without write
 * protection. Usage: Create instances of this class with one of the static
 * factory functions, and assign them to <tt>const</tt> variables. The factory
 * functions guarantee that all data members have correct values representing
 * the <em>same</em> color.
 *
 * Changes to some values of some color formats under certain
 * circumstances do not change the color:
 * - Hue: When the color is on the gray axis.
 * - HSL saturation: When the color is either black (L = 0%) or
 *   white (L = 100%).
 * - HSV/HSB saturation: When the color is black (V/B = 0%).
 *
 * The color conversion of this class provides meaningful values for
 * <em>hue</em> in all color formats, also for colors on the gray axis.
 * It also provides meaningful and predictable <em>HSL-saturation</em> and
 * <em>HSV-saturation</em> values.
 *
 * @note When creating an object from a RGB-based color (HSL, HSV…), the
 * resulting @ref cielch and @ref ciehlc is <em>not</em> guaranteed to be
 * @ref RgbColorSpace::isInGamut. Reason: All in-range RGB values will actually
 * always be in-gamut by definition; just because of possible rounding errors
 * @ref RgbColorSpace::isInGamut might return <tt>false</tt>, and modifying
 * them so that @ref RgbColorSpace::isInGamut returns <tt>true</tt> might be
 * slow. Note that values outside the normal RGB range [0, 255] respectively
 * [0.0, 1.0] might return useless @ref cielch and @ref ciehlc values.
 *
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::MultiColor &value)
 *
 * @internal
 *
 * @note About other color models: There is no support for Munsell because
 * there seems to be no C++ code available freely but only R code. There exists
 * a <a href="http://www.brucelindbloom.com/index.html?UPLab.html">color
 * transformation ICC profile to produce a Munsell-like LAB space</a>,
 * but it’s about 1.2 MB heavy. Anyway, Munsell is not used so wide-spread.
 * Other color systems like the <em>Natural Color System</em> are
 * proprietary and therefore difficult to implement. */
class MultiColor final
{
public:
    [[nodiscard]] static MultiColor fromHsl(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color);
    [[nodiscard]] static MultiColor fromHsv(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color);
    [[nodiscard]] static MultiColor fromHwb(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color);
    [[nodiscard]] static MultiColor fromLch(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &color);
    [[nodiscard]] static MultiColor fromRgb(const QSharedPointer<RgbColorSpace> &colorSpace, const QList<double> &color);
    [[nodiscard]] static MultiColor fromRgbQColor(const QSharedPointer<RgbColorSpace> &colorSpace, const QColor &color);

    /** @brief Constructor for an uninitialized object.
     *
     * This constructor is quite useless except for declaring variables
     * of this type. Use the static functions to get an actual color object.
     *
     * @warning As the data members are uninitialized, this implies that the
     * count of <tt>QList</tt> items is not correct! */
    MultiColor();
    /** @brief Default copy constructor
     * @param other the object to copy */
    MultiColor(const MultiColor &other) = default;
    /** @brief Default copy assignment operator
     * @param other the object to copy
     * @returns The default implementation’s return value. */
    MultiColor &operator=(const MultiColor &other) = default;
    // NOTE About move constructor and move assignment operator:
    // Declaring them with “= default” will create a compiler-generated
    // implementation that, apparently, does not copy correctly
    // the data members of type QList. Using “=delete” however
    // will create a move constructor that it forbidden to be used.
    // When some code needs a move constructor, the overload resolution
    // will choose this move constructor, and the compilation fails
    // because it is forbidden to be used. For details on this behaviour,
    // see https://blog.knatten.org/2021/10/15/ Therefore, we do not declare
    // the move constructor at all: Because the copy constructor exists, the
    // move constructor will not be generated implicitly at all. When some
    // code needs a move constructor, the overload resolution does not find
    // one and falls back to the copy constructor (which is the default
    // implementation, but apparently works correctly also for QList members).
    // MultiColor &operator=(MultiColor &&other) noexcept = default;
    // MultiColor(MultiColor &&other) noexcept = default;

    [[nodiscard]] bool operator==(const MultiColor &other) const;

    /** @brief HLC representation.
     *
     * @sa @ref cielch */
    QList<double> ciehlc;
    /** @brief LCH representation.
     *
     * Range: See @ref lchrange
     *
     * @sa @ref ciehlc */
    LchDouble cielch;
    /** @brief HWB representation.
     *
     * Range: [0, 360], [0, 100], [0, 100] */
    QList<double> hwb;
    /** @brief HSL representation.
     *
     * Range: [0, 360], [0, 100], [0, 100] */
    QList<double> hsl;
    /** @brief HSV representation.
     *
     * Range: [0, 360], [0, 100], [0, 100] */
    QList<double> hsv;
    /** @brief Oklch representation.
     *
     * @todo xxx Range: [0, 360], [0, 100], [0, 100] */
    QList<double> oklch;
    /** @brief RGB representation.
     *
     * Range: [0, 255] */
    QList<double> rgb;
    /** @brief QColor representation.
     *
     * <tt>QColor::spec()</tt> is <tt>QColor::Rgb</tt>. */
    QColor rgbQColor;

private:
    /** @brief The distance threshold for near-to-gray detection.
     *
     * Using intentionally a value < 0.499 which seems enough for the profiles
     * that we have tested. And it allows still to display to the user
     * integer values without confusing rounding problems, because values
     * equal or below 0.499 would have been rounded anyway to 0.
     *
     * @note It would be nice if this value could be smaller. However, testing
     * with <tt>0.0499</tt> did not work well: For values on the gray axis,
     * when changing the HSV-hue by 1°, in about 50% of the cases the LCH-hue
     * changes, and in about 50% it did not. The reason is maybe be the low
     * 16-bit precision in QColor, which is used for some of the conversions.
     * And for values near to the gray axis, the effect seems to be bigger.
     * Maybe this could be solved by implementing our own conversion algorithms
     * with double precision instead of relying on QColor. */
    static constexpr double colorDifferenceThreshold = 0.499;
    static_assert(colorDifferenceThreshold < 0.5);

    void fillLchAndDerivatesFromRgbAndDerivates(const QSharedPointer<RgbColorSpace> &colorSpace);
    void fillRgbAndDerivates(QColor color, std::optional<double> hue);
    static QColor fromRgbDoubleToQColor(const RgbDouble &color);
};

QDebug operator<<(QDebug dbg, const PerceptualColor::MultiColor &value);

} // namespace PerceptualColor

#endif // MULTICOLOR_H
