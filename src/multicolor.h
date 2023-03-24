// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef MULTICOLOR_H
#define MULTICOLOR_H

#include "lchdouble.h"
#include <multirgb.h>
#include <qdebug.h>
#include <qlist.h>
#include <qsharedpointer.h>

namespace PerceptualColor
{
class RgbColorSpace;

/** @internal
 *
 * @brief A color stored in multiple different color formats.
 *
 * Unlike <tt>QColor</tt> (which is essentially a C++ <tt>union</tt> of
 * different color formats, so only one of them is actually saved),
 * @ref MultiColor <em>actually</em> stores <em>all</em> available color
 * formats.
 *
 * This data type is just an (ugly) implementation detail of @ref ColorDialog.
 * For simplicity, data members are accessible directly, without write
 * protection. Usage: Create instances of this class with one of the static
 * factory functions, and assign them to <tt>const</tt> variables. The factory
 * functions guarantee that all data members have correct values representing
 * the <em>same</em> color.
 *
 * Changes to some values of some color formats under certain
 * circumstances do not change the color:
 * - Hue: When the color is on the gray axis.
 *
 * The color conversion of this class provides meaningful values for
 * <em>hue</em> in all color formats, also for colors on the gray axis.
 *
 * @note When creating an object from a RGB-based color (HSL, HSV…),
 * the resulting Oklab, Oklch, Cielab and Cielch values are
 * <em>not</em> guaranteed to be recognized by @ref RgbColorSpace
 * as in-gamut. Reason: All in-range RGB values will actually
 * always be in-gamut by definition; just because of possible rounding errors
 * @ref RgbColorSpace might return <tt>false</tt> when testing if in-gamut,
 * and having code that makes sure to return <tt>true</tt> here might be
 * slow. Note that values outside the normal RGB range [0, 255] respectively
 * [0.0, 1.0] might return useless @ref cielchD50 and @ref ciehlcD50 values.
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
    [[nodiscard]] static MultiColor fromCielchD50(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &color);
    [[nodiscard]] static MultiColor fromMultiRgb(const QSharedPointer<RgbColorSpace> &colorSpace, const MultiRgb &color);
    [[nodiscard]] static MultiColor fromOklch(const QSharedPointer<RgbColorSpace> &colorSpace, const LchDouble &color);

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
     * @sa @ref cielchD50 */
    QList<double> ciehlcD50;
    /** @brief LCH representation.
     *
     * Range: See @ref lchrange
     *
     * @sa @ref ciehlcD50 */
    LchDouble cielchD50;
    /** @brief Oklch representation.
     *
     * @todo xxx Range: [0, 360], [0, 100], [0, 100] */
    QList<double> oklch;
    /** @brief Various RGB-based representation */
    MultiRgb multiRgb;

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
     * changes, and in about 50% it did not. The reason is maybe the low
     * 16-bit precision in QColor, which is used for some of the conversions.
     * And for values near to the gray axis, the effect seems to be bigger.
     * Maybe this could be solved by implementing our own conversion algorithms
     * with double precision instead of relying on QColor. */
    static constexpr double colorDifferenceThreshold = 0.499;
    static_assert(colorDifferenceThreshold < 0.5);

    void fillLchAndDerivatesFromRgbAndDerivates(const QSharedPointer<RgbColorSpace> &colorSpace);
};

QDebug operator<<(QDebug dbg, const PerceptualColor::MultiColor &value);

} // namespace PerceptualColor

#endif // MULTICOLOR_H
