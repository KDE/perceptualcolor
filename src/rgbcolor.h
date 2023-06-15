// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef RGBCOLOR_H
#define RGBCOLOR_H

#include <optional>
#include <qcolor.h>
#include <qdebug.h>
#include <qlist.h>

namespace PerceptualColor
{

/** @internal
 *
 * @brief An RGB color stored in multiple different RGB transformations.
 *
 * Unlike <tt>QColor</tt> (which is essentially a C++ <tt>union</tt> of
 * different color formats, so only one of them is actually saved),
 * @ref RgbColor <em>actually</em> stores <em>all</em> available color
 * transformations.
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
 * - HSL saturation: When the color is either black (L = 0%) or
 *   white (L = 100%).
 * - HSV/HSB saturation: When the color is black (V/B = 0%).
 *
 * The color conversion of this class provides meaningful and predictable
 * <em>HSL-saturation</em> and <em>HSV-saturation</em> values.
 *
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::RgbColor &value)
 *
 * @sa @ref AbsoluteColor */
class RgbColor final
{
public:
    [[nodiscard]] static RgbColor fromHsl(const QList<double> &color);
    [[nodiscard]] static RgbColor fromHsv(const QList<double> &color);
    [[nodiscard]] static RgbColor fromHwb(const QList<double> &color);
    [[nodiscard]] static RgbColor fromRgb255(const QList<double> &color, std::optional<double> hue = std::optional<double>());
    [[nodiscard]] static RgbColor fromRgbQColor(const QColor &color);

    /** @brief Constructor for an uninitialized object.
     *
     * This constructor is quite useless except for declaring variables
     * of this type. Use the static functions to get an actual color object.
     *
     * @warning As the data members are uninitialized, this implies that the
     * count of <tt>QList</tt> items is not correct! */
    RgbColor();
    /** @brief Default copy constructor
     * @param other the object to copy */
    RgbColor(const RgbColor &other) = default;
    /** @brief Default copy assignment operator
     * @param other the object to copy
     * @returns The default implementation’s return value. */
    RgbColor &operator=(const RgbColor &other) = default;
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
    // RgbColor &operator=(RgbColor &&other) noexcept = default;
    // RgbColor(RgbColor &&other) noexcept = default;

    [[nodiscard]] bool operator==(const RgbColor &other) const;

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
    /** @brief RGB representation.
     *
     * Range: [0, 255] */
    QList<double> rgb255;
    /** @brief QColor representation.
     *
     * <tt>QColor::spec()</tt> is <tt>QColor::Rgb</tt>. */
    QColor rgbQColor;

private:
    void fillAll(QColor color, std::optional<double> hue);
};

QDebug operator<<(QDebug dbg, const PerceptualColor::RgbColor &value);

} // namespace PerceptualColor

#endif // RGBCOLOR_H
