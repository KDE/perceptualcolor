// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERCONVERSION_H
#define HELPERCONVERSION_H

#include "helperqttypes.h"
#include "lchdouble.h"
#include <lcms2.h>
#include <qcolor.h>
#include <qgenericmatrix.h>
#include <qglobal.h>
#include <type_traits>

/** @internal
 *
 * @file
 *
 * Provides type conversions. */

namespace PerceptualColor
{

struct RgbDouble;

[[nodiscard]] cmsCIELab fromCmscielabD50ToOklab(const cmsCIELab &cielabD50);

/** @internal
 *
 * @brief Converts from <tt>[0, 1]</tt> to <tt>[0, 255]</tt>.
 *
 * @param original A value on a scale <tt>[0, 1]</tt>.
 *
 * @returns Value converted to the scale <tt>[0, 255]</tt>, applying correct
 * rounding. Out-of-range values are bound to the valid range. */
template<typename T>
[[nodiscard]] constexpr quint8 fromFloatingToEightBit(const T &original)
{
    static_assert( //
        std::is_floating_point<T>::value, //
        "Template fromFloatingToEightBit() only works with floating point types");
    const int rounded = qRound(original * 255);
    const auto bounded = qBound<int>(0, rounded, 255);
    return static_cast<quint8>(bounded);
}

[[nodiscard]] QGenericMatrix<1, 3, double> fromOklabToXyzd65(const QGenericMatrix<1, 3, double> &value);

[[nodiscard]] cmsCIELab fromOklabToCmscielabD50(const cmsCIELab &oklab);

QColor fromRgbDoubleToQColor(const RgbDouble &color);

[[nodiscard]] QGenericMatrix<1, 3, double> fromXyzd65ToOklab(const QGenericMatrix<1, 3, double> &value);

/** @internal
 *
 * @brief Like <tt>QColor::fromRgbF</tt> but for all floating point types.
 *
 * @param red Red component. Range: <tt>[0, 1]</tt>
 * @param green See above.
 * @param blue See above.
 * @returns A corresponding <tt>QColor</tt> object. */
template<typename T>
[[nodiscard]] QColor qColorFromRgbDouble(T red, T green, T blue)
{
    static_assert( //
        std::is_floating_point<T>::value, //
        "Template fromFloatingToEightBit() only works with floating point types");
    return QColor::fromRgbF(static_cast<QColorFloatType>(red), //
                            static_cast<QColorFloatType>(green), //
                            static_cast<QColorFloatType>(blue));
}

[[nodiscard]] cmsCIELab toCmsCieLab(const cmsCIELCh &value);

[[nodiscard]] cmsCIELCh toCmsCieLch(const LchDouble &value);

[[nodiscard]] LchDouble toCielchDouble(const cmsCIELCh &value);

[[nodiscard]] LchDouble toCielchDouble(const cmsCIELab &value);

/** @internal
 *
 * @brief White point D65 for 2°-observer.
 *
 * According to
 * <a href="https://en.wikipedia.org/w/index.php?title=Illuminant_D65&oldid=1100467073#Definition">
 * Wikipedia</a>:
 *
 * > “Using the standard 2° observer […] of D65 […] Normalizing for
 * > relative luminance (i.e. set Y=100), the XYZ tristimulus
 * > values are:<br/>
 * > X = 95.047<br/>
 * > Y = 100<br/>
 * > Z = 108.883”
 *
 * Normalizing this to Y = 1 as expected by LittleCMS, gives this value. */
constexpr cmsCIEXYZ whitePointD65TwoDegree{0.95047, 1.00000, 1.08883};

} // namespace PerceptualColor

#endif // HELPERCONVERSION_H
