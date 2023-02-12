// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef POLARPOINTF_H
#define POLARPOINTF_H

#include <qdebug.h>
#include <qglobal.h>
#include <qmetatype.h>
#include <qpoint.h>

namespace PerceptualColor
{
/** @internal
 *
 * @brief A point in the polar coordinate system
 *
 * Defines a point in the two-dimensional polar coordinate system using
 * floating point precision.
 *
 * Polar coordinates allow multiple representations for a single point:
 * - An angle of 0° is the same as 360° is the same as 720° is the same
 *   as −360°.
 * - A radius of 1 and an angle of 0° is the same as a radius of −1 and an
 *   angle of 180°.
 * - If the radius is 0, the angle is meaningless: A radius of 0 and an angle
 *   of 57° is the same as a radius of 0 and an angle of 233°.
 *
 * This class automatically normalizes the values:
 * @invariant
 * - the @ref radius() is normalized to value ≥ 0. If it was < 0 (but not if
 *   it was 0 with a negative sign) its  sign is changed and @ref angleDegree
 *   is turned by 180°.
 * - the @ref angleDegree() is normalized to 0° ≤ value < 360°
 *   (see @ref normalizedAngleDegree() for details)
 *
 * When the radius is 0, often by convention the (meaningless) angle is set
 * also 0. This class does <em>not</em> normalize the angle to 0 when the
 * radius is 0 as long as initialized with a radius and an angle. However,
 * when initialized with Cartesian coordinates (0, 0) then the result is
 * radius 0 and angle 0. See also @ref isSamePoint().
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 *
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::PolarPointF value)
 */
class PolarPointF final
{
public:
    /** @brief Constructor
     *
     * Constructs an object with @ref radius() = 0 and @ref angleDegree() = 0 */
    explicit PolarPointF() = default;

    /** @brief Default copy constructor
     *
     * @param other the object to copy */
    PolarPointF(const PolarPointF &other) = default;

    /** @brief Default move constructor
     *
     * @param other the object to move */
    PolarPointF(PolarPointF &&other) noexcept = default;

    explicit PolarPointF(const qreal newRadial, const qreal newAngleDegree);

    explicit PolarPointF(const QPointF cartesianCoordiantes);

    /** @brief Default assignment operator
     *
     * @returns The default implementation’s return value.
     *
     * @param other the object to assign */
    // Clazy, our static code checker, complains about the next line of code
    // as follows:
    //     “Pass small and trivially-copyable type by value”
    // However, this is a copy constructor. We cannot pass the argument
    // by value, because the compiler would complain as follows:
    //     “the parameter for an explicitly-defaulted copy assignment
    //      operator must be an lvalue reference type”
    // Therefore, we exclude the following line from this specific clazy check,
    // by adding a magic comment after it.
    PolarPointF &operator=(const PolarPointF &other) = default; // clazy:exclude=function-args-by-value

    /** @brief Default move assignment operator
     *
     * @returns The default implementation’s return value.
     *
     * @param other the object to move-assign */
    PolarPointF &operator=(PolarPointF &&other) noexcept = default;

    [[nodiscard]] qreal angleDegree() const;

    [[nodiscard]] bool isSamePoint(const PolarPointF other) const;

    [[nodiscard]] qreal radius() const;

    QPointF toCartesian() const;

private:
    /** @brief Holds the @ref angleDegree() value. */
    qreal m_angleDegree{0};

    /** @brief Holds the @ref radius() value. */
    qreal m_radius{0};
};

QDebug operator<<(QDebug dbg, const PerceptualColor::PolarPointF value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::PolarPointF)

#endif // POLARPOINTF_H
