// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_VEC3_H
#define PERCEPTUALCOLOR_VEC3_H

#include <qdebug.h>
#include <qmetatype.h>
#include <qpoint.h>

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Three-dimensional vector class template.
 *
 * Template class representing a 3D vector with floating-point elements.
 * Provides element access, type conversion, and basic construction.
 *
 * @tparam T Floating-point type.
 */
template<typename T>
struct Vec3 {
    static_assert(std::is_floating_point_v<T>, "T must be floating point");

    /**
     * @brief Vector elements
     */
    std::array<T, 3> v{};

    /**
     * @brief Default constructor.
     *
     * Initializes all elements to zero.
     */
    constexpr Vec3() = default;

    /**
     * @brief Construct a vector from three elements.
     *
     * @param v0 First element
     * @param v1 Second element
     * @param v2 Third element
     */
    constexpr Vec3(T v0, T v1, T v2)
        : v{{v0, v1, v2}}
    {
    }

    /**
     * @brief Access a element by index (mutable).
     *
     * @param i Index (0..2)
     * @return Reference to the element
     */
    constexpr T &operator()(size_t i)
    {
        return v[i];
    }

    /**
     * @brief Access a element by index (const).
     *
     * @param i Index (0..2)
     * @return Const reference to the element
     */
    constexpr const T &operator()(size_t i) const
    {
        return v[i];
    }

    /**
     * @brief Explicit conversion to another Vec3 type.
     *
     * Converts all elements to type U.
     *
     * @tparam U Target floating-point type
     * @return Converted vector
     */
    template<typename U>
    constexpr explicit operator Vec3<U>() const
    {
        return Vec3<U>{static_cast<U>(v[0]), //
                       static_cast<U>(v[1]), //
                       static_cast<U>(v[2])};
    }
};

/**
 * @internal
 *
 * @typedef Vec3f
 * @brief Alias for a three-dimensional vector with float elements.
 *
 * This is a specialization of Vec3<T> with T = float.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Vec3f = Vec3<float>;

/**
 * @internal
 *
 * @typedef Vec3d
 * @brief Alias for a three-dimensional vector with double elements.
 *
 * This is a specialization of Vec3<T> with T = double.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Vec3d = Vec3<double>;

/**
 * @internal
 *
 * @typedef Vec3ld
 * @brief Alias for a three-dimensional vector with long double elements.
 *
 * This is a specialization of Vec3<T> with T = long double.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Vec3ld = Vec3<long double>;

QDebug operator<<(QDebug dbg, const Vec3d &value);

QDebug operator<<(QDebug dbg, const Vec3f &value);

QDebug operator<<(QDebug dbg, const Vec3ld &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::Vec3f)
Q_DECLARE_METATYPE(PerceptualColor::Vec3d)
Q_DECLARE_METATYPE(PerceptualColor::Vec3ld)

#endif // PERCEPTUALCOLOR_VEC3_H
