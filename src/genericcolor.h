// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_GENERICCOLOR_H
#define PERCEPTUALCOLOR_GENERICCOLOR_H

#include "vec3.h"
#include <array>
#include <qdebug.h>
#include <qlist.h>
#include <qmetatype.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Numeric representation of an opaque color with up to four components.
 *
 * This type is declared as type to Qt’s type system via
 * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
 * example if you want to use for \e {queued} signal-slot connections),
 * you might consider calling \c {qRegisterMetaType()} for
 * this type, once you have a QApplication object.
 */
struct GenericColor {
public:
    /*!
     * \brief Default constructor. */
    constexpr GenericColor() noexcept = default;

    /*!
     * \brief Constructor.
     *
     * \a init Initial value.
     */
    explicit constexpr GenericColor(const Vec3d &init) noexcept
        : first(init.v[0])
        , second(init.v[1])
        , third(init.v[2])
    {
    }

    /*!
     * \brief Constructor.
     *
     * \a v1 Initial value for \l first
     *
     * \a v2 Initial value for \l second
     *
     * \a v3 Initial value for \l third
     */
    constexpr GenericColor(const double v1, const double v2, const double v3) noexcept
        : first(v1)
        , second(v2)
        , third(v3)
    {
    }

    explicit GenericColor(const QList<double> &list);

    /*!
     * \brief Equal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if equal, \c {false} otherwise. */
    [[nodiscard]] constexpr bool operator==(const GenericColor &other) const noexcept = default;

    /*!
     * \brief Unequal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if unequal, \c {false} otherwise. */
    [[nodiscard]] constexpr bool operator!=(const GenericColor &other) const noexcept = default;

    [[nodiscard]] QList<double> toQList3() const;

    /*!
     * \brief The values \l first, \l second, \l third as \l Vec3d.
     *
     * Returns the values \l first, \l second, \l third as \l Vec3d.
     */
    [[nodiscard]] constexpr Vec3d toVec3d() const
    {
        return Vec3d{first, second, third};
    }

    /*!
     * \brief First value. */
    double first = 0;
    /*!
     * \brief Second value. */
    double second = 0;
    /*!
     * \brief Third value. */
    double third = 0;
};

QDebug operator<<(QDebug dbg, const PerceptualColor::GenericColor &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::GenericColor)

#endif // PERCEPTUALCOLOR_GENERICCOLOR_H
