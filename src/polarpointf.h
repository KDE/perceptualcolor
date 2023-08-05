// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef POLARPOINTF_H
#define POLARPOINTF_H

#include <qdebug.h>
#include <qmetatype.h>
#include <qpoint.h>

namespace PerceptualColor
{
/** @internal
 *
 * @brief A point in a
 * <a href="https://en.wikipedia.org/wiki/Polar_coordinate_system">polar
 * coordinate system</a>
 *
 * Polar coordinates are important for color
 * handling because many color models use the
 * <a href="https://en.wikipedia.org/wiki/Cylindrical_coordinate_system">
 * cylindrical coordinate system</a> which extends the two-dimensional
 * polar coordinate system to three dimensions by adding a (linear)
 * <em>z</em> coordinate.
 *
 * Polar coordinate systems represent points by a radial coordinate
 * (<em>radius</em>, also called <em>r</em> or <em>ρ</em>) and an angular
 * coordinate (<em>angle</em>, also called <em>azimuth</em>, <em>φ</em>,
 * <em>θ</em> or <em>t</em>).
 *
 * Polar coordinates allow multiple representations for a single point:
 * - An angle of 0° is the same as 360° is the same as 720° is the same
 *   as −360°.
 * - A radius of 1 and an angle of 0° is the same as a radius of −1 and an
 *   angle of 180°.
 * - If the radius is 0, the angle is meaningless: A radius of 0 and an angle
 *   of 57° is the same as a radius of 0 and an angle of 233°.
 *
 * @invariant The polar coordinates are normalized. See @ref normalizePolar360
 * for details.
 *
 * To provide a clear API, there is no <em>equal</em> operator. Use
 * @ref isSamePoint() instead..
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

    explicit PolarPointF(const double newRadius, const double newAngleDegree);

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

    [[nodiscard]] double angleDegree() const;

    [[nodiscard]] bool isSamePoint(const PolarPointF other) const;

    [[nodiscard]] double radius() const;

    QPointF toCartesian() const;

private:
    /** @brief Holds the @ref angleDegree() value. */
    double m_angleDegree{0};

    /** @brief Holds the @ref radius() value. */
    double m_radius{0};
};

QDebug operator<<(QDebug dbg, const PerceptualColor::PolarPointF value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::PolarPointF)

#endif // POLARPOINTF_H
