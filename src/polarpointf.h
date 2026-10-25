// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_POLARPOINTF_H
#define PERCEPTUALCOLOR_POLARPOINTF_H

#include <qdebug.h>
#include <qmetatype.h>
#include <qpoint.h>

namespace PerceptualColor
{
/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A point in a
 * \l {https://en.wikipedia.org/wiki/Polar_coordinate_system} {polar
 * coordinate system}.
 *
 * Polar coordinates are important for color
 * handling because many color models use the
 * \l {https://en.wikipedia.org/wiki/Cylindrical_coordinate_system}
 * {cylindrical coordinate system} which extends the two-dimensional
 * polar coordinate system to three dimensions by adding a (linear)
 * \e {z} coordinate.
 *
 * Polar coordinate systems represent points by a radial coordinate
 * (\e {radius}, also called \e {r} or \e {ρ}) and an angular
 * coordinate (\e {angle}, also called \e {azimuth}, \e {φ},
 * \e {θ} or \e {t}).
 *
 * Polar coordinates allow multiple representations for a single point:
 *
 * \list
 *     \li An angle of 0° is the same as 360° is the same as 720° is the same
 *         as −360°.
 *     \li A radius of 1 and an angle of 0° is the same as a radius of −1 and
 *         an angle of 180°.
 *     \li If the radius is 0, the angle is meaningless: A radius of 0 and an
 *         angle of 57° is the same as a radius of 0 and an angle of 233°.
 * \endlist
 *
 * The polar coordinates in this class are always normalized. See
 * \l normalizePolar360 for details.
 *
 * To provide a clear API, there is no \e {equal} operator. Use
 * \l isSamePoint() instead..
 *
 * This type is declared as type to Qt’s type system via
 * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
 * example if you want to use for \e {queued} signal-slot connections),
 * you might consider calling \c {qRegisterMetaType()} for
 * this type, once you have a QApplication object.
 *
 * This data type can be passed to QDebug thanks to
 * \l {operator<<(QDebug dbg, const PerceptualColor::PolarPointF value)}
 */
class PolarPointF
{
public:
    /*!
     * \brief Constructor
     *
     * Constructs an object with radius() = 0 and angleDegree() = 0 */
    explicit PolarPointF() = default;

    /*!
     * \brief Default copy constructor
     *
     * \a other the object to copy
     */
    PolarPointF(const PolarPointF &other) = default;

    /*!
     * \brief Default move constructor
     *
     * \a other the object to move
     */
    PolarPointF(PolarPointF &&other) noexcept = default;

    explicit PolarPointF(const double newRadius, const double newAngleDegree);

    explicit PolarPointF(const QPointF cartesianCoordiantes);

    // Clazy, our static code checker, complains about the next line of code
    // as follows:
    //     “Pass small and trivially-copyable type by value”
    // However, this is a copy constructor. We cannot pass the argument
    // by value, because the compiler would complain as follows:
    //     “the parameter for an explicitly-defaulted copy assignment
    //      operator must be an lvalue reference type”
    // Therefore, we exclude the following line from this specific clazy check,
    // by adding a magic comment after it.
    /*!
     * \brief Default assignment operator
     *
     * \a other the object to assign
     */
    PolarPointF &operator=(const PolarPointF &other) = default; // clazy:exclude=function-args-by-value

    /*!
     * \brief Default move assignment operator
     *
     * \a other the object to move-assign
     */
    PolarPointF &operator=(PolarPointF &&other) noexcept = default;

    [[nodiscard]] double angleDegree() const;

    [[nodiscard]] bool isSamePoint(const PolarPointF other) const;

    [[nodiscard]] double radius() const;

    QPointF toCartesian() const;

private:
    /*!
     * \brief Holds the \l angleDegree() value. */
    double m_angleDegree{0};

    /*!
     * \brief Holds the radius() value. */
    double m_radius{0};
};

QDebug operator<<(QDebug dbg, const PerceptualColor::PolarPointF value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::PolarPointF)

#endif // PERCEPTUALCOLOR_POLARPOINTF_H
