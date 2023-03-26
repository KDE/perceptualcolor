﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef LCHDOUBLE_H
#define LCHDOUBLE_H

#include "importexport.h"
#include <qdebug.h>
#include <qmetatype.h>

namespace PerceptualColor
{
/** @brief A LCH color (Oklch, CielchD50, CielchD65…)
 *
 * Storage of floating point LCH values with <tt>double</tt> precision.
 *
 * The data is not default-initialized; it is undefined when the object
 * is created.
 *
 * Example:
 * @snippet testlchdouble.cpp Use LchDouble
 *
 * More details about the valid range: @ref lchrange
 *
 * This class intentionally does not provide the operators <em>equal
 * to</em> (<tt>operator==</tt>) and <em>not equal to</em>
 * (<tt>operator!=</tt>). As LCH colors are polar coordinates,
 * there are various valid representations of the same angle.
 * And h is even meaningless when C is zero; on the other hand,
 * there might nevertheless be an interest in preserving h. And
 * invalid values with L=200 or L=300: Should they be equal because
 * both are invalid? Or are they different? The answer to all
 * these questions depends on your use case. To avoid confusion,
 * no comparison operators are provided by this class. See also
 * @ref hasSameCoordinates.
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 *
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::LchDouble &value)
 *
 * @internal
 *
 * @todo Would it make sense to normalize the hue (1° instead
 * of 361°, and only non-negative radials)? */
struct PERCEPTUALCOLOR_IMPORTEXPORT LchDouble {
public:
    /** @brief Lightness, mesured in percent.
     *
     * The valid range is <tt>[0, 100]</tt>. */
    double l;
    /** @brief Chroma.
     *
     * <tt>0</tt> means no chroma (grayscale). The maximum value depends on
     * the gamut. For sRGB for example it’s a given value, but other gamuts
     * can be bigger, but the practical limit is the gamut of the
     * @ref lchrange "human perception", beyond which a
     * Chroma value does not make sense. */
    double c;
    /** @brief Hue, measured in degree.
     *
     * The valid range is <tt>[0, 360[</tt>. */
    double h;
    [[nodiscard]] bool hasSameCoordinates(const LchDouble &other) const;
};

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::LchDouble &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::LchDouble)

#endif // LCHDOUBLE_H
