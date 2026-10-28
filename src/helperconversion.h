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

namespace PerceptualColor
{

/*!
 *
 *
 * \fn template<typename T> [[nodiscard]] constexpr quint8 fromFloatingToEightBit(const T &original)
 *
 * \brief Converts from \c {[0, 1]} to \c {[0, 255]}, applying correct
 * rounding.
 *
 * Out-of-range values are bound to the valid range.
 *
 * \a original A value on a scale \c {[0, 1]}.
 */
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

[[nodiscard]] QColor toRgbExact(const QColor &color);

/*!
 *
 *
 * \brief White point D50 for 2°-observer, normalized to Y = 1.
 *
 * \note According to
 * \l {https://en.wikipedia.org/wiki/CIELAB_color_space#From_CIE_XYZ_to_CIELAB}
 * {Wikipedia}, this would be (0.964212, 1, 0.825188).
 * However, many software implementations, for example
 * \l {http:://mathworks.com/help/images/ref/whitepoint.html} {Mathworks}, use
 * a different value (0.9642, 1, 0.8249). Not sure which values are the correct
 * ones.
 */
inline constexpr Vec3ld whitePointD50TwoDegree{0.9642L, 1.0000L, 0.8249L};

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief White point D65 for 2°-observer, normalized to Y = 1.
 *
 * According to
 * \l {https://en.wikipedia.org/w/index.php?title=Illuminant_D65&oldid=1100467073#Definition}
 * {Wikipedia}:
 *
 * \quotation
 *     Using the standard 2° observer […] of D65 […] Normalizing for
 *     relative luminance (i.e. set Y=100), the XYZ tristimulus
 *     values are:
 *
 *     X = 95.047
 *
 *     Y = 100
 *
 *     Z = 108.883
 * \endquotation
 *
 * Normalizing this to Y = 1 gives this value. */
inline constexpr Vec3ld whitePointD65TwoDegree{0.95047L, 1.00000L, 1.08883L};

// clang-format off

/*!
 *
 *
 * \brief Oklab M1 matrix.
 *
 * As in the
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab}
 * {Oklab definition}.
 */
inline constexpr Mat3ld oklabM1 {
    +0.8189330101L, +0.3618667424L, -0.1288597137L,
    +0.0329845436L, +0.9293118715L, +0.0361456387L,
    +0.0482003018L, +0.2643662691L, +0.6338517070L};

/*!
 *
 *
 * \brief Oklab M1 inverse matrix.
 *
 * As in the
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab}
 * {Oklab definition}.
 */
inline constexpr Mat3ld oklabM1inverse = oklabM1.inverse().value();

/*!
 *
 *
 * \brief Oklab M2 matrix.
 *
 * As in the
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab}
 * {Oklab definition}.
 */
inline constexpr Mat3ld oklabM2 {
    +0.2104542553L, +0.7936177850L, -0.0040720468L,
    +1.9779984951L, -2.4285922050L, +0.4505937099L,
    +0.0259040371L, +0.7827717662L, -0.8086757660L};

/*!
 *
 *
 * \brief Oklab M2 inverse matrix.
 *
 * As in the
 * \l {https://bottosson.github.io/posts/oklab/#converting-from-xyz-to-oklab}
 * {Oklab definition}.
 */
inline constexpr Mat3ld oklabM2inverse = oklabM2.inverse().value();

/*!
 *
 *
 * \brief xyzD65 to xyzD50 conversion matrix.
 *
 * As in the
 * \l {https://fujiwaratko.sakura.ne.jp/infosci/colorspace/bradford_e.html}
 * {Bradford transformation}.
 */
inline constexpr Mat3ld xyzD65ToXyzD50Matrix {
    +1.047886L, +0.022919L, -0.050216L,
    +0.029582L, +0.990484L, -0.017079L,
    -0.009252L, +0.015073L, +0.751678L};

/*!
 *
 *
 * \brief xyzD50 to xyzD65 conversion matrix.
 *
 * As in the
 * \l {https://fujiwaratko.sakura.ne.jp/infosci/colorspace/bradford_e.html}
 * {Bradford transformation}.
 */
inline constexpr Mat3ld xyzD50ToXyzD65Matrix = xyzD65ToXyzD50Matrix.inverse().value();

/*!
 *
 *
 * \brief Linear sRGB to XYZ D65 matrix.
 *
 * As in \l {https://en.wikipedia.org/wiki/SRGB#Primaries}
 * {Wikipedia’s definition}.
 */
inline constexpr Mat3ld linearSRgbToXyzD65Matrix {
    0.4124L, 0.3576L, 0.1805L,
    0.2126L, 0.7152L, 0.0722L,
    0.0193L, 0.1192L, 0.9505L};

/*!
 *
 *
 * \brief XYZ D65 to Linear sRGB matrix.
 *
 * As in \l {https://en.wikipedia.org/wiki/SRGB#Primaries}
 * {Wikipedia’s definition}.
 */
inline constexpr Mat3ld xyzD65ToLinearSRgbMatrix = linearSRgbToXyzD65Matrix.inverse().value();

// clang-format on

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_HELPERCONVERSION_H
