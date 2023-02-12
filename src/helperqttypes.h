// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERQTTYPES_H
#define HELPERQTTYPES_H

#include <qcolor.h>
#include <qlist.h>

/** @internal
 *
 * @file
 *
 * Type declarations for types that Qt uses in its API and that have
 * changed between Qt5 and Qt6. */

namespace PerceptualColor
{

/** @internal
 *
 * The type that <tt>QColor</tt> uses for floating point values.
 *
 * This type might vary: On Qt6, it’s <tt>float</tt> while on Qt5 it
 * is <tt>qreal</tt> which might be either <tt>float</tt> or <tt>double</tt>
 * depending on the options with which Qt was compiled. */
using QColorFloatType = decltype(std::declval<QColor>().redF());
static_assert(std::is_floating_point<QColorFloatType>());
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().redF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().greenF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().blueF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().alphaF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().valueF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().lightnessF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().saturationF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().hsvSaturationF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().hslSaturationF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().hsvHueF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().hslHueF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().hueF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().magentaF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().cyanF())>);
static_assert( //
    std::is_same_v<QColorFloatType, decltype(std::declval<QColor>().yellowF())>);

/** @internal
 *
 * The type that <tt>QList</tt> uses for index.
 *
 * This type might vary between Qt5 and Qt6. */
using QListSizeType = decltype(std::declval<QList<int>>().count());
static_assert( //
    std::is_same_v<QListSizeType, decltype(std::declval<QList<int>>().count())>);
static_assert( //
    std::is_same_v<QListSizeType, decltype(std::declval<QList<int>>().size())>);
static_assert( //
    std::is_same_v<QListSizeType, decltype(std::declval<QList<int>>().indexOf(0))>);
static_assert( //
    std::is_same_v<QListSizeType, decltype(std::declval<QList<int>>().lastIndexOf(0))>);

/** @internal
 *
 * The type that <tt>QString</tt> uses for length.
 *
 * This type might vary between Qt5 and Qt6.
 *
 * @sa @ref QStringSize */
using QStringLength = decltype(std::declval<QString>().length());
static_assert( //
    std::is_same_v<QStringLength, decltype(std::declval<QString>().length())>);
static_assert( //
    std::is_same_v<QStringLength, decltype(std::declval<QString>().size())>);

/** @internal
 *
 * The type that <tt>QString</tt> uses for size.
 *
 * This type might vary between Qt5 and Qt6.
 *
 * @sa @ref QStringLength */
using QStringSize = decltype(std::declval<QString>().size());
static_assert( //
    std::is_same_v<QStringLength, decltype(std::declval<QString>().length())>);
static_assert( //
    std::is_same_v<QStringLength, decltype(std::declval<QString>().size())>);

} // namespace PerceptualColor

#endif // HELPERQTTYPES_H
