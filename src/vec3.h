// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_VEC3_H
#define PERCEPTUALCOLOR_VEC3_H

#include <array>
#include <qdebug.h>
#include <qmetatype.h>
#include <stddef.h>

namespace PerceptualColor
{

/*!
 * \class PerceptualColor::Vec3
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Three-dimensional vector class template.
 *
 * Template class representing a 3D vector with floating-point elements.
 * Provides element access, type conversion, and basic construction.
 *
 * \b T Floating-point type.
 */
template<typename T>
struct Vec3 {
    static_assert(std::is_floating_point_v<T>, "T must be floating point");

    /*!
     * \brief Vector elements
     */
    std::array<T, 3> v{};

    /*!
     * \brief Default constructor.
     *
     * Initializes all elements to zero.
     */
    constexpr Vec3() = default;

    /*!
     * \brief Construct a vector from three elements.
     *
     * \a v0 First element
     *
     * \a v1 Second element
     *
     * \a v2 Third element
     */
    constexpr Vec3(T v0, T v1, T v2)
        : v{{v0, v1, v2}}
    {
    }

    /*!
     * \brief Access a element by index (mutable).
     *
     * \a i Index (0..2)
     *
     * Returns a reference to the element
     */
    [[nodiscard]] constexpr T &operator()(size_t i)
    {
        return v[i];
    }

    /*!
     * \brief Access a element by index (const).
     *
     * \a i Index (0..2)
     *
     * Returns a c\ const reference to the element
     */
    [[nodiscard]] constexpr const T &operator()(size_t i) const
    {
        return v[i];
    }

    /*!
     * \brief Explicit conversion to another Vec3 type.
     *
     * Converts all elements to type U.
     *
     * \b U Target floating-point type
     */
    template<typename U>
    [[nodiscard]] constexpr explicit operator Vec3<U>() const
    {
        return Vec3<U>{static_cast<U>(v[0]), //
                       static_cast<U>(v[1]), //
                       static_cast<U>(v[2])};
    }
};

/*!
 * \relates PerceptualColor::Vec3
 *
 * \typealias PerceptualColor::Vec3f
 *
 * \relates PerceptualColor
 *
 * \brief Alias for a three-dimensional vector with float elements.
 *
 * This is a specialization of Vec3<T> with T = float.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Vec3f = Vec3<float>;

/*!
 * \relates PerceptualColor::Vec3
 *
 * \typealias PerceptualColor::Vec3d
 *
 * \relates PerceptualColor
 *
 * \brief Alias for a three-dimensional vector with double elements.
 *
 * This is a specialization of Vec3<T> with T = double.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Vec3d = Vec3<double>;

/*!
 * \relates PerceptualColor::Vec3
 *
 * \typealias PerceptualColor::Vec3ld
 *
 * \relates PerceptualColor
 *
 * \brief Alias for a three-dimensional vector with long double elements.
 *
 * This is a specialization of Vec3<T> with T = long double.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Vec3ld = Vec3<long double>;

QDebug operator<<(QDebug dbg, const PerceptualColor::Vec3d &value);

QDebug operator<<(QDebug dbg, const PerceptualColor::Vec3f &value);

QDebug operator<<(QDebug dbg, const PerceptualColor::Vec3ld &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::Vec3f)
Q_DECLARE_METATYPE(PerceptualColor::Vec3d)
Q_DECLARE_METATYPE(PerceptualColor::Vec3ld)

#endif // PERCEPTUALCOLOR_VEC3_H
