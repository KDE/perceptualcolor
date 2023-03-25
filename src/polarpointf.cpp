// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "polarpointf.h"

// Other includes
#include "helpermath.h"
#include "helperposixmath.h"
#include <cmath>
#include <qdebug.h>
#include <qmath.h>
#include <type_traits>

namespace PerceptualColor
{
/** @brief Constructor
 *
 * Normalizes the given polar coordinates and constructs an object with
 * the <em>normalized</em> polar coordinates. See the general class
 * description for details about the normalization.
 *
 * @param newRadius the @ref radius() value
 * @param newAngleDegree the @ref angleDegree() value */
PolarPointF::PolarPointF(const qreal newRadius, const qreal newAngleDegree)
{
    if (newRadius < 0) {
        m_radius = newRadius * (-1);
        m_angleDegree = normalizedAngleDegree(newAngleDegree + 180);
    } else {
        m_radius = newRadius;
        m_angleDegree = normalizedAngleDegree(newAngleDegree);
    }
}

/** @brief Constructor
 *
 * Constructs an object converting from the given Cartesian coordinates.
 *
 * If the Cartesian coordinates are (0, 0) than the @ref angleDegree (which is
 * meaningless for a @ref radius of 0) is set to 0°.
 *
 * @param cartesianCoordiantes the Cartesian coordinates */
PolarPointF::PolarPointF(const QPointF cartesianCoordiantes)
{
    m_radius = sqrt(pow(cartesianCoordiantes.x(), 2) + pow(cartesianCoordiantes.y(), 2));
    if (m_radius == 0) {
        m_angleDegree = 0;
        return;
    }
    if (cartesianCoordiantes.y() >= 0) {
        m_angleDegree = qRadiansToDegrees(acos(cartesianCoordiantes.x() / m_radius));
    } else {
        m_angleDegree = qRadiansToDegrees(2 * pi - acos(cartesianCoordiantes.x() / m_radius));
    }
}

/** @brief Compares with another @ref PolarPointF
 *
 * @param other the polar coordinates to compare with
 *
 * @returns <tt>true</tt> if both, <tt>this</tt> and <tt>other</tt>,
 * are the same point in the coordinate space. <tt>false</tt> otherwise.
 * Therefore <tt>[@ref radius() 0, @ref angleDegree() 50]</tt> is considered
 * to be the same point as <tt>[@ref radius() 0, @ref angleDegree() 80]</tt>
   because the @ref angleDegree() is meaningless if the @ref radius() is 0.*/
bool PolarPointF::isSamePoint(const PolarPointF other) const
{
    return (
        // radius has to be identical
        (m_radius == other.m_radius) &&
        // angle has to be identical (except when radius is 0, because
        // then angle is meaningless)
        ((m_angleDegree == other.m_angleDegree) || (m_radius == 0)));
}

/** @brief Normalized radius
 *
 * @returns the normalized radius value, guaranteed to be ≥ 0. */
qreal PolarPointF::radius() const
{
    return m_radius;
}

/** @brief Normalized angle
 *
 * @returns the normalized angle value (coordinates in degree), guaranteed to
 * be 0° ≤ value < 360° */
qreal PolarPointF::angleDegree() const
{
    return m_angleDegree;
}

/** @brief Convert to Cartesian coordinates
 *
 * @returns the corresponding Cartesian coordinates */
QPointF PolarPointF::toCartesian() const
{
    return QPointF(m_radius * cos(qDegreesToRadians(m_angleDegree)), m_radius * sin(qDegreesToRadians(m_angleDegree)));
}

/** @internal
 *
 * @brief Adds QDebug() support for data type
 * @ref PerceptualColor::PolarPointF
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::PolarPointF value)
{
    dbg.nospace() << "PolarPointF(radius: " << value.radius() << ", angleDegree: " << value.angleDegree() << "°)";
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
