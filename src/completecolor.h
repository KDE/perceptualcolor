// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COMPLETECOLOR_H
#define PERCEPTUALCOLOR_COMPLETECOLOR_H

#include "perceptualcolornamespace.h"
#include "rgbcolor.h"
#include <qdebug.h>
#include <qmetatype.h>
#include <qpoint.h>

namespace PerceptualColor
{

/*!
 * \inmodule ModulePrivateApi
 *
 * \brief Simple struct providing various color representations.
 *
 * This type is declared as type to Qt’s type system via
 * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
 * example if you want to use for \e {queued} signal-slot connections),
 * you might consider calling \c {qRegisterMetaType()} for
 * this type, once you have a QApplication object.
 *
 * This data type can be passed to QDebug thanks to
 * \l {operator<<(QDebug dbg, const PerceptualColor::CompleteColor &value)}
 */
struct CompleteColor {
public:
    /*!
     * \brief Wether the color is valid.
     */
    bool isValid = false;

    /*!
     * \brief The alpha channel.
     *
     * \note If not \l isValid, this value is undefined.
     */
    qreal alpha = 1;

    /*! \brief Current color without alpha information
     *
     * Holds the color in absolutely defined color models.
     *
     * \note If not \l isValid, this value is undefined.
     */
    QHash<ColorModel, GenericColor> opaqueColorAbs;

    /*!
     * \brief Current color without alpha information
     *
     * Holds the color in the RGB color model and derived color models,
     * using the sRGB gamut.
     *
     * \note If not \l isValid, this value is undefined.
     */
    RgbColor opaqueColorSRgb;

    /*!
     * \brief Equal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if equal, \c {false} otherwise.
     */
    [[nodiscard]] bool operator==(const CompleteColor &other) const = default;

    /*!
     * \brief Unequal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if unequal, \c {false} otherwise.
     */
    [[nodiscard]] bool operator!=(const CompleteColor &other) const = default;

    [[nodiscard]] static CompleteColor fromAbsColor(const QHash<PerceptualColor::ColorModel, PerceptualColor::GenericColor> &abs, const qreal a);

    [[nodiscard]] static CompleteColor fromSRgbColor(const RgbColor &rgb, const qreal a);
};

QDebug operator<<(QDebug dbg, const PerceptualColor::CompleteColor &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::CompleteColor)

#endif // PERCEPTUALCOLOR_COMPLETECOLOR_H
