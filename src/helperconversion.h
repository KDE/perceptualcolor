// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_HELPERCONVERSION_H
#define PERCEPTUALCOLOR_HELPERCONVERSION_H

#include "genericcolor.h"
#include "mat3.h"
#include "perceptualcolornamespace.h"
#include <array>
#include <qcolor.h>
#include <qglobal.h>
#include <qtmetamacros.h>
#include <type_traits>

/** @internal
 *
 * @file
 *
 * Provides type conversions. */

namespace PerceptualColor
{

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
 * Normalizing this to Y = 1 gives this value. */
constexpr Vec3ld whitePointD65TwoDegree{0.95047L, 1.00000L, 1.08883L};

// clang-format off

/**
 * @brief Oklab M1 matrix.
 *
 * As in the
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab">
 * Oklab definition</a>.
 */
inline constexpr Mat3ld oklabM1 {
    +0.8189330101L, +0.3618667424L, -0.1288597137L,
    +0.0329845436L, +0.9293118715L, +0.0361456387L,
    +0.0482003018L, +0.2643662691L, +0.6338517070L};

/**
 * @brief Oklab M1 inverse matrix.
 *
 * As in the
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab">
 * Oklab definition</a>.
 */
inline constexpr Mat3ld oklabM1inverse = oklabM1.inverse().value();

/**
 * @brief Oklab M2 matrix.
 *
 * As in the
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab">
 * Oklab definition</a>.
 */
inline constexpr Mat3ld oklabM2 {
    +0.2104542553L, +0.7936177850L, -0.0040720468L,
    +1.9779984951L, -2.4285922050L, +0.4505937099L,
    +0.0259040371L, +0.7827717662L, -0.8086757660L};

/**
 * @brief Oklab M2 inverse matrix.
 *
 * As in the
 * <a href="https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab">
 * Oklab definition</a>.
 */
inline constexpr Mat3ld oklabM2inverse = oklabM2.inverse().value();

/**
 * @brief xyzD65 to xyzD50 conversion matrix.
 *
 * As in the
 * <a href="https://fujiwaratko.sakura.ne.jp/infosci/colorspace/bradford_e.html">
 * Bradford transformation</a>.
 */
inline constexpr Mat3ld xyzD65ToXyzD50Matrix {
    +1.047886L, +0.022919L, -0.050216L,
    +0.029582L, +0.990484L, -0.017079L,
    -0.009252L, +0.015073L, +0.751678L};

/**
 * @brief xyzD50 to xyzD65 conversion matrix.
 *
 * As in the
 * <a href="https://fujiwaratko.sakura.ne.jp/infosci/colorspace/bradford_e.html">
 * Bradford transformation</a>.
 */
inline constexpr Mat3ld xyzD50ToXyzD65Matrix = xyzD65ToXyzD50Matrix.inverse().value();

/**
 * @brief Linear sRGB to XYZ D65 matrix.
 *
 * As in <a href="https://en.wikipedia.org/wiki/SRGB#Primaries">
 * Wikipedia’s definition</a>.
 */
inline constexpr Mat3ld linearSRgbToXyzD65Matrix {
    0.4124L, 0.3576L, 0.1805L,
    0.2126L, 0.7152L, 0.0722L,
    0.0193L, 0.1192L, 0.9505L};

/**
 * @brief XYZ D65 to Linear sRGB matrix.
 *
 * As in <a href="https://en.wikipedia.org/wiki/SRGB#Primaries">
 * Wikipedia’s definition</a>.
 */
inline constexpr Mat3ld XyzD65ToLinearSRgbMatrix = linearSRgbToXyzD65Matrix.inverse().value();

// clang-format on

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_HELPERCONVERSION_H
