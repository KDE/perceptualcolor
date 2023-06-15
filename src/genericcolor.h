// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef GENERICCOLOR_H
#define GENERICCOLOR_H

#include "helpermath.h"
#include "lchdouble.h"
#include <lcms2.h>
#include <qdebug.h>
#include <qlist.h>

namespace PerceptualColor
{

/** @internal
 *
 * @brief Numeric representation of a color without specifying the color
 * space. */
struct GenericColor {
public:
    /** @brief Default constructor. */
    constexpr GenericColor() = default;

    /** @brief Constructor.
     *
     * @param init Initial value. @ref fourth is set to <tt>0</tt>. */
    explicit GenericColor(const Trio &init)
        : first(init(0, 0))
        , second(init(1, 0))
        , third(init(2, 0))
        , fourth(0)
    {
    }

    /** @brief Constructor.
     *
     * @param init Initial value. @ref fourth is set to <tt>0</tt>. */
    explicit constexpr GenericColor(const cmsCIELab &init)
        : first(init.L)
        , second(init.a)
        , third(init.b)
        , fourth(0)
    {
    }

    /** @brief Constructor.
     *
     * @param init Initial value. @ref fourth is set to <tt>0</tt>. */
    explicit constexpr GenericColor(const cmsCIEXYZ &init)
        : first(init.X)
        , second(init.Y)
        , third(init.Z)
        , fourth(0)
    {
    }

    /** @brief Constructor.
     *
     * @param init Initial value. @ref fourth is set to <tt>0</tt>. */
    explicit constexpr GenericColor(const LchDouble &init)
        : first(init.l)
        , second(init.c)
        , third(init.h)
        , fourth(0)
    {
    }

    /** @brief Constructor.
     *
     * @param v1 Initial value for @ref first
     * @param v2 Initial value for @ref second
     * @param v3 Initial value for @ref third
     *
     * @ref fourth is set to <tt>0</tt>. */
    constexpr GenericColor(const double v1, const double v2, const double v3)
        : first(v1)
        , second(v2)
        , third(v3)
        , fourth(0)
    {
    }

    /** @brief Constructor.
     *
     * @param v1 Initial value for @ref first
     * @param v2 Initial value for @ref second
     * @param v3 Initial value for @ref third
     * @param v4 Initial value for @ref fourth */
    constexpr GenericColor(const double v1, const double v2, const double v3, const double v4)
        : first(v1)
        , second(v2)
        , third(v3)
        , fourth(v4)
    {
    }

    bool operator==(const GenericColor &other) const;
    bool operator!=(const GenericColor &other) const;

    [[nodiscard]] cmsCIELab reinterpretAsLabToCmscielab() const;
    [[nodiscard]] LchDouble reinterpretAsLchToLchDouble() const;
    [[nodiscard]] cmsCIEXYZ reinterpretAsXyzToCmsciexyz() const;
    [[nodiscard]] QList<double> toQList3() const;
    [[nodiscard]] Trio toTrio() const;

    /** @brief First value. */
    double first = 0;
    /** @brief Second value. */
    double second = 0;
    /** @brief Third value. */
    double third = 0;
    /** @brief Forth value. */
    double fourth = 0;
};

QDebug operator<<(QDebug dbg, const PerceptualColor::GenericColor &value);

} // namespace PerceptualColor

#endif // GENERICCOLOR_H
