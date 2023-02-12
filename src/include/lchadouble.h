﻿// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef LCHADOUBLE_H
#define LCHADOUBLE_H

#include "importexport.h"
#include <qdebug.h>
#include <qmetatype.h>

namespace PerceptualColor
{
/** @brief A LCH color with alpha channel.
 *
 * Storage of floating point LCH values with <tt>double</tt> precision.
 *
 * The data is not default-initialized; it is undefined when the object
 * is created.
 *
 * Example:
 * @snippet testLchaDouble.cpp Use LchaDouble
 *
 * More details about the valid range: @ref lchrange
 *
 * @note This class intentionally does not provide the operators
 * <em>equal to</em> (<tt>operator==</tt>) and <em>not equal to</em>
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
 * @ref operator<<(QDebug dbg, const PerceptualColor::LchaDouble &value)
 *
 * @internal
 *
 * @todo We could normalize @ref LchaDouble values, just like @ref PolarPointF
 * also does. Performance should not matter for this use case! But: Does
 * it make sense? */
struct PERCEPTUALCOLOR_IMPORTEXPORT LchaDouble {
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
    /** @brief Opacity (alpha channel)
     *
     * The valid range is <tt>[0, 1]</tt>. <tt>0</tt> is fully
     * transparent, <tt>1</tt> is fully opaque. */
    double a;
    [[nodiscard]] bool hasSameCoordinates(const LchaDouble &other) const;
};

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::LchaDouble &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::LchaDouble)

#endif // LCHADOUBLE_H
