// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "polarpointf.h"

// Other includes
#include "helpermath.h"
#include <cmath>
#include <numbers>
#include <qdebug.h>
#include <qmath.h>
#include <type_traits>

namespace PerceptualColor
{
/*!
 * \brief Constructor
 *
 * Normalizes the given polar coordinates and constructs an object with
 * the \e {normalized} polar coordinates. See the general class
 * description for details about the normalization.
 *
 * \a newRadius the \l radius() value
 *
 * \a newAngleDegree the \l angleDegree() value
 */
PolarPointF::PolarPointF(const double newRadius, const double newAngleDegree)
    : m_angleDegree(newAngleDegree)
    , m_radius(newRadius)
{
    normalizePolar360(m_radius, m_angleDegree);
}

/*!
 * \brief Constructor
 *
 * Constructs an object converting from the given Cartesian coordinates.
 *
 * If the Cartesian coordinates are (0, 0) than the \l angleDegree (which is
 * meaningless for a radius of 0) is set to 0°.
 *
 * \a cartesianCoordiantes the Cartesian coordinates
 */
PolarPointF::PolarPointF(const QPointF cartesianCoordiantes)
{
    m_radius = sqrt( //
        pow(cartesianCoordiantes.x(), 2) + pow(cartesianCoordiantes.y(), 2));
    if (m_radius == 0) {
        m_angleDegree = 0;
        return;
    }
    // NOTE As explained in https://en.wikipedia.org/wiki/Atan2
    // the function std::atan2() can be used to calculate the angle without
    // an “if” statement. However, it returns angles in the interval [-π, +π]
    // while we need [0°, 360°], so finally we could avoind an “if” statement
    // but need to add code for the range correction. Therefore, the current
    // approach seems fine.
    if (cartesianCoordiantes.y() >= 0) {
        m_angleDegree = qRadiansToDegrees( //
            std::acos(cartesianCoordiantes.x() / m_radius));
    } else {
        m_angleDegree = qRadiansToDegrees( //
            2 * std::numbers::pi - std::acos(cartesianCoordiantes.x() / m_radius));
    }
}

/*!
 * \brief Compares with another \l PolarPointF
 *
 * \a other the polar coordinates to compare with
 *
 * Returns \c {true} if both, \c {this} and \c {other},
 * are the same point in the coordinate space. \c {false} otherwise.
 * Therefore \c {[ radius() 0, angleDegree() 50]} is considered
 * to be the same point as \c {[ radius() 0, angleDegree() 80]}
   because the angleDegree() is meaningless if the radius() is 0.*/
bool PolarPointF::isSamePoint(const PolarPointF other) const
{
    return (
        // radius has to be identical
        (m_radius == other.m_radius) &&
        // angle has to be identical (except when radius is 0, because
        // then angle is meaningless)
        ((m_angleDegree == other.m_angleDegree) || (m_radius == 0)));
}

/*!
 * \brief Normalized radius
 *
 * Returns the normalized radius value, guaranteed to be ≥ 0.
 */
double PolarPointF::radius() const
{
    return m_radius;
}

/*!
 * \brief Normalized angle
 *
 * Returns the normalized angle value (coordinates in degree), guaranteed to
 * be 0° ≤ value < 360° */
double PolarPointF::angleDegree() const
{
    return m_angleDegree;
}

/*!
 * \brief Convert to Cartesian coordinates
 *
 * Returns the corresponding Cartesian coordinates
 */
QPointF PolarPointF::toCartesian() const
{
    return QPointF(m_radius * std::cos(qDegreesToRadians(m_angleDegree)), //
                   m_radius * std::sin(qDegreesToRadians(m_angleDegree)));
}

/*!
 * \relates PerceptualColor::PolarPointF
 *
 * \brief Adds QDebug() support for data type
 * \l PerceptualColor::PolarPointF
 *
 * \a dbg Existing debug object
 *
 * \a value Value to stream into the debug object
 */
QDebug operator<<(QDebug dbg, const PerceptualColor::PolarPointF value)
{
    dbg.nospace() //
        << "PolarPointF(radius: " //
        << value.radius() //
        << ", angleDegree: " //
        << value.angleDegree() //
        << "°)";
    return dbg.maybeSpace();
}

static_assert(std::is_trivially_copyable_v<PolarPointF>);
// static_assert(std::is_trivial_v<PolarPointF>);

static_assert(std::is_standard_layout_v<PolarPointF>);

static_assert(std::is_default_constructible_v<PolarPointF>);
// static_assert(std::is_trivially_default_constructible_v<PolarPointF>);
static_assert(std::is_nothrow_default_constructible_v<PolarPointF>);

static_assert(std::is_copy_constructible_v<PolarPointF>);
static_assert(std::is_trivially_copy_constructible_v<PolarPointF>);
static_assert(std::is_nothrow_copy_constructible_v<PolarPointF>);

static_assert(std::is_move_constructible_v<PolarPointF>);
static_assert(std::is_trivially_move_constructible_v<PolarPointF>);
static_assert(std::is_nothrow_move_constructible_v<PolarPointF>);

} // namespace PerceptualColor
