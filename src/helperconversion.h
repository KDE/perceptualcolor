// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERCONVERSION_H
#define HELPERCONVERSION_H

#include "helperqttypes.h"
#include "lchdouble.h"
#include <lcms2.h>
#include <qcolor.h>
#include <qglobal.h>
#include <type_traits>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

/** @internal
 *
 * @file
 *
 * Provides type conversions. */

namespace PerceptualColor
{

Q_NAMESPACE

struct RgbDouble;

/** @brief Identifiers for color spaces */
enum class ColorModel {
    CielabD50, /**< Cielab color space using a D50 illuminant.
        Lightness: [0, 100].<br/>
        a: unbound.<br/>
        b: unbound. */
    CielchD50, /**< Cielch color space using a D50 illuminant.
        Lightness: [0, 100].<br/>
        Chroma: unbound.<br/>
        Hue: [0, 360[. */
    XyzD50, /**< Xyz color space using a D50 illuminant.
        X: unbound.<br/>
        Y: [0, 1].<br/>
        Z: unbound. */
    XyzD65, /**< Xzy color space using a D65 illuminant.
        X: unbound.<br/>
        Y: [0, 1].<br/>
        Z: unbound. */
    OklabD65, /**< Oklab color space, which by definition always and
        exclusively uses a D65 illuminant.

        Lightness: [0, 1].<br/>
        a: unbound.<br/>
        b: unbound. */
    OklchD65 /**< Oklch color space, which by definition always and
        exclusively uses a D65 illuminant.

        Lightness: [0, 1].<br/>
        Chroma: unbound.<br/>
        Hue: [0, 360[. */
};
Q_ENUM_NS(ColorModel)

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
uint qHash(const ColorModel t, uint seed = 0); // clazy:exclude=qt6-qhash-signature
#endif

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

QColor fromRgbDoubleToQColor(const RgbDouble &color);

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

[[nodiscard]] cmsCIELab toCmsLab(const cmsCIELCh &value);

[[nodiscard]] cmsCIELCh toCmsLch(const LchDouble &value);

[[nodiscard]] LchDouble toLchDouble(const cmsCIELCh &value);

[[nodiscard]] LchDouble toLchDouble(const cmsCIELab &value);

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
// TODO xxx This seems to be not longer used!?
constexpr cmsCIEXYZ whitePointD65TwoDegree{0.95047, 1.00000, 1.08883};

} // namespace PerceptualColor

#endif // HELPERCONVERSION_H
