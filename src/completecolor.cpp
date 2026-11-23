// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "completecolor.h"

// Other includes
#include "absolutecolor.h"
#include "helpermath.h"
#include <cmath>
#include <numbers>
#include <qdebug.h>
#include <qmath.h>
#include <qtypes.h>
#include <type_traits>

namespace PerceptualColor
{

/*!
 * \brief Constructs an object from a given absolute color.
 *
 * \a abs A hash containing all absolute colors as of \l AbsoluteColor.
 *
 * \a a The alpha value.
 */
CompleteColor CompleteColor::fromAbsColor(const QHash<PerceptualColor::ColorModel, PerceptualColor::GenericColor> &abs, const qreal a)
{
    CompleteColor result;

    result.isValid = true;
    result.alpha = a;
    result.opaqueColorAbs = abs;
    result.opaqueColorSRgb = RgbColor::fromRgb_1(abs.value(ColorModel::SRgb_1));

    return result;
}

/*!
 * \brief Constructs an object from a given sRGB color.
 *
 * \a rgb sRGB values.
 *
 * \a a The alpha value.
 */
CompleteColor CompleteColor::fromSRgbColor(const RgbColor &rgb, const qreal a)
{
    CompleteColor result;

    result.isValid = true;
    result.alpha = a;
    result.opaqueColorAbs = //
        AbsoluteColor::allConversions(ColorModel::SRgb_1, rgb.rgb_1);
    result.opaqueColorSRgb = rgb;

    return result;
}

/*!
 * \relates PerceptualColor::RgbColor
 *
 * \brief Adds QDebug() support for data type
 * \l PerceptualColor::RgbColor
 *
 * \a dbg Existing debug object
 *
 * \a value Value to stream into the debug object
 */
QDebug operator<<(QDebug dbg, const PerceptualColor::CompleteColor &value)
{
    dbg.nospace() //
        << "CompleteColor BEGIN\n"
        << value.opaqueColorAbs << "\n"
        << value.opaqueColorSRgb << "\n"
        << "alpha: " << value.alpha << "\n"
        << "CompleteColor END\n";
    return dbg.maybeSpace();
}

} // namespace PerceptualColor
