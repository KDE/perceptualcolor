// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_RGBCOLOR_H
#define PERCEPTUALCOLOR_RGBCOLOR_H

#include "genericcolor.h"
#include <optional>
#include <qcolor.h>
#include <qdebug.h>
#include <qstring.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief An in-gamut RGB color stored in multiple different RGB
 * transformations.
 *
 * When constructed from an out-of-gamut input value, all channels will be
 * bound to the gamut.
 *
 * Unlike \c {QColor} (which is essentially a C++ \c {union} of
 * different color formats, so only one of them is actually saved),
 * \l RgbColor \e {actually} stores \e {all} available color
 * transformations.
 *
 * This data type is just an (ugly) implementation detail of \l ColorDialog.
 * For simplicity, data members are accessible directly, without write
 * protection. Usage: Create instances of this class with one of the static
 * factory functions, and assign them to \c {const} variables. The factory
 * functions guarantee that all data members have correct values representing
 * the \e {same} color.
 *
 * Changes to some values of some color formats under certain
 * circumstances do not change the color:
 *
 * \list
 *     \li HSL saturation: When the color is either black (L = 0%) or
 *         white (L = 100%).
 *     \li HSV/HSB saturation: When the color is black (V/B = 0%).
 * \endlist
 *
 * The color conversion of this class provides meaningful and predictable
 * \e {HSL-saturation} and \e {HSV-saturation} values.
 *
 * This data type can be passed to QDebug thanks to
 * \l operator<<(QDebug dbg, const PerceptualColor::RgbColor &value)
 *
 * \sa AbsoluteColor
 */
class RgbColor
{
public:
    [[nodiscard]] static RgbColor fromHsl(const GenericColor &color);
    [[nodiscard]] static RgbColor fromHsv(const GenericColor &color);
    [[nodiscard]] static RgbColor fromHwb(const GenericColor &color);
    [[nodiscard]] static RgbColor fromRgb1(const GenericColor &color, std::optional<double> hue = std::optional<double>());
    [[nodiscard]] static RgbColor fromRgb255(const GenericColor &color, std::optional<double> hue = std::optional<double>());
    [[nodiscard]] static RgbColor fromRgbQColor(const QColor &color);

    /*!
     * \brief Constructor for an uninitialized object.
     *
     * This constructor is quite useless except for declaring variables
     * of this type. Use the static functions to get an actual color object.
     */
    RgbColor();
    /*!
     * \brief Default copy constructor
     *
     * \a other the object to copy
     */
    RgbColor(const RgbColor &other) = default;
    /*!
     * \brief Default copy assignment operator
     *
     * \a other the object to copy
     *
     * Returns the default implementation’s return value.
     */
    RgbColor &operator=(const RgbColor &other) = default;

    /*!
     * \brief Equal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if all data members have exactly the same
     * coordinates. \c {false} otherwise.
     */
    [[nodiscard]] bool operator==(const RgbColor &other) const = default;

    /*!
     * \brief Not-equal operator
     *
     * \a other The object to compare with.
     *
     * Returns the logical opposite of operator==.
     */
    [[nodiscard]] bool operator!=(const RgbColor &other) const = default;

    /*!
     * \brief HSL representation.
     *
     * Range: [0, 360], [0, 100], [0, 100] */
    GenericColor hsl;
    /*!
     * \brief HSV representation.
     *
     * Range: [0, 360], [0, 100], [0, 100] */
    GenericColor hsv;
    /*!
     * \brief HWB representation.
     *
     * Range: [0, 360], [0, 100], [0, 100] */
    GenericColor hwb;
    /*!
     * \brief RGB representation.
     *
     * Range: [0, 1] */
    GenericColor rgb_1;
    /*!
     * \brief RGB representation.
     *
     * Range: [0, 255] */
    GenericColor rgb_255;
    /*!
     * \brief RGB representation with 6 hexadecimal digits.
     *
     * Each of red, green and blue is represented by 2 digits.
     *
     * Range: [\#000000, \#FFFFFF]
     */
    QString rgbHex6;
    /*!
     * \brief QColor representation.
     *
     * \c {QColor::spec()} is \c {QColor::Rgb}. */
    QColor rgbQColor;

private:
    void fillAll(QColor color, std::optional<double> hue);
};

QDebug operator<<(QDebug dbg, const PerceptualColor::RgbColor &value);

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_RGBCOLOR_H
