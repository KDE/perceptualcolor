// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERCONVERSION_H
#define HELPERCONVERSION_H

#include "genericcolor.h"
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

/** @brief Identifiers for color models.
 *
 * @internal
 *
 * @note Maybe
 * <a href="https://doc-snapshots.qt.io/qt6-dev/qcolorspace.html#ColorModel-enum">
 * <tt>enum QColorSpace::NamedColorSpace</tt></a> might also be useful in
 * the future.
 */
enum class ColorModel {
    CielabD50, /**< The absolute Cielab color space using a D50 illuminant.
        Lightness: [0, 100].<br/>
        a: unbound.<br/>
        b: unbound. */
    CielchD50, /**< The absolute Cielch color space using a D50 illuminant.
        Lightness: [0, 100].<br/>
        Chroma: unbound.<br/>
        Hue: [0, 360[. */
    Hsl360_1_1, /**< Any color space using the HSL color model, which is a
        transformation of the RGB color model. Color values only have a
        well-defined absolute meaning when associated with a corresponding
        RGB color profile.

        Hue: [0, 360[.<br/>
        Saturation: [0, 1].<br/>
        Lightness: [0, 1]. */
    Hwb360_1_1, /**< Any color space using the HSL color model, which is a
        transformation of the HWB color model. Color values only have a
        well-defined absolute meaning when associated with a corresponding
        RGB color profile.

        Hue: [0, 360[.<br/>
        Whiteness: [0, 1].<br/>
        Blackness: [0, 1]. */
    Invalid, /**< Represents invalid data. */
    OklabD65, /**< The absolute Oklab color space, which by definition always
        and exclusively uses a D65 illuminant.

        Lightness: [0, 1].<br/>
        a: unbound.<br/>
        b: unbound. */
    OklchD65, /**< The absolute Oklch color space, which by definition always
        and exclusively uses a D65 illuminant.

        Lightness: [0, 1].<br/>
        Chroma: unbound.<br/>
        Hue: [0, 360[. */
    Rgb1, /**< Any color space using the RGB color model. Color values only
        have a well-defined absolute meaning when associated with a
        corresponding RGB color profile.

        R: [0, 1].<br/>
        G: [0, 1].<br/>
        B: [0, 1]. */
    XyzD50, /**< The absolute XYZ color space assuming chromatic adaption
        for the D50 illuminant.

        X: unbound.<br/>
        Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0<br/>
        Z: unbound. */
    XyzD65 /**< The absolute XYZ color space assuming chromatic adaption
        for the D65 illuminant.

        X: unbound.<br/>
        Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0<br/>
        Z: unbound. */
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
    return QColor::fromRgbF(static_cast<float>(red), //
                            static_cast<float>(green), //
                            static_cast<float>(blue));
}

[[nodiscard]] cmsCIELab toCmsLab(const cmsCIELCh &value);

[[nodiscard]] GenericColor toGenericColorCielabD50(const cmsCIELab &value);

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
