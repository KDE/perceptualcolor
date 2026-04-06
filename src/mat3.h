// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_MAT3_H
#define PERCEPTUALCOLOR_MAT3_H

#include "vec3.h"
#include <optional>
#include <qdebug.h>
#include <qmetatype.h>
#include <qpoint.h>

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Three-by-three matrix class.
 *
 * Template class representing a 3x3 matrix in row-major order.
 * Provides element access, type conversion, matrix multiplication,
 * vector multiplication, determinant calculation, and inversion.
 *
 * @tparam T Floating-point type (float, double, long double).
 */
template<typename T>
struct Mat3 {
    static_assert(std::is_floating_point_v<T>, "T must be floating point");

    /**
     * @brief Matrix elements in row-major order
     */
    std::array<T, 9> m{}; // in row-major order

    /**
     * @brief Default constructor.
     *
     * Initializes all elements to zero.
     */
    constexpr Mat3() = default;

    /**
     * @brief Construct a matrix from nine elements.
     *
     * @param m00 Element at row 0, column 0
     * @param m01 Element at row 0, column 1
     * @param m02 Element at row 0, column 2
     * @param m10 Element at row 1, column 0
     * @param m11 Element at row 1, column 1
     * @param m12 Element at row 1, column 2
     * @param m20 Element at row 2, column 0
     * @param m21 Element at row 2, column 1
     * @param m22 Element at row 2, column 2
     */
    constexpr Mat3(T m00, T m01, T m02, T m10, T m11, T m12, T m20, T m21, T m22)
        : m{{m00, m01, m02, m10, m11, m12, m20, m21, m22}}
    {
    }

    /**
     * @brief Access an element by row and column (mutable).
     *
     * @param r Row index (0..2)
     * @param c Column index (0..2)
     * @return Reference to the element
     */
    constexpr T &operator()(size_t r, size_t c)
    {
        return m[r * 3 + c];
    }

    /**
     * @brief Access an element by row and column (const).
     *
     * @param r Row index (0..2)
     * @param c Column index (0..2)
     * @return Const reference to the element
     */
    constexpr const T &operator()(size_t r, size_t c) const
    {
        return m[r * 3 + c];
    }

    /**
     * @brief Explicit conversion to another Mat3 type.
     *
     * Converts all elements to type U.
     *
     * @tparam U Target floating-point type
     * @return Converted matrix
     */
    template<typename U>
    constexpr explicit operator Mat3<U>() const
    {
        Mat3<U> result;
        for (size_t i = 0; i < 9; ++i)
            result.m[i] = static_cast<U>(m[i]);
        return result;
    }

    /**
     * @brief Matrix multiplication.
     *
     * @param rhs Right-hand side matrix
     * @return Product matrix
     */
    constexpr Mat3 operator*(const Mat3 &rhs) const
    {
        Mat3 result{};

        result.m[0] = m[0] * rhs.m[0] + m[1] * rhs.m[3] + m[2] * rhs.m[6];
        result.m[1] = m[0] * rhs.m[1] + m[1] * rhs.m[4] + m[2] * rhs.m[7];
        result.m[2] = m[0] * rhs.m[2] + m[1] * rhs.m[5] + m[2] * rhs.m[8];

        result.m[3] = m[3] * rhs.m[0] + m[4] * rhs.m[3] + m[5] * rhs.m[6];
        result.m[4] = m[3] * rhs.m[1] + m[4] * rhs.m[4] + m[5] * rhs.m[7];
        result.m[5] = m[3] * rhs.m[2] + m[4] * rhs.m[5] + m[5] * rhs.m[8];

        result.m[6] = m[6] * rhs.m[0] + m[7] * rhs.m[3] + m[8] * rhs.m[6];
        result.m[7] = m[6] * rhs.m[1] + m[7] * rhs.m[4] + m[8] * rhs.m[7];
        result.m[8] = m[6] * rhs.m[2] + m[7] * rhs.m[5] + m[8] * rhs.m[8];

        return result;
    }

    /**
     * @brief Matrix-vector multiplication.
     *
     * @param v Input vector
     * @return Resulting vector
     */
    constexpr Vec3<T> operator*(const Vec3<T> &v) const
    {
        return Vec3<T>{m[0] * v(0) + m[1] * v(1) + m[2] * v(2), //
                       m[3] * v(0) + m[4] * v(1) + m[5] * v(2), //
                       m[6] * v(0) + m[7] * v(1) + m[8] * v(2)};
    }

    /**
     * @brief Compute the determinant of the matrix.
     *
     * @return Determinant value
     */
    constexpr T determinant() const
    {
        return m[0] * (m[4] * m[8] - m[5] * m[7]) //
            - m[1] * (m[3] * m[8] - m[5] * m[6]) //
            + m[2] * (m[3] * m[7] - m[4] * m[6]);
    }

    /**
     * @brief Compute the inverse of the matrix.
     *
     * Returns std::nullopt if the matrix is not invertible
     * (determinant equals zero).
     *
     * @return Optional containing the inverse matrix if invertible,
     *         otherwise std::nullopt
     */
    constexpr std::optional<Mat3> inverse() const
    {
        T d = determinant();

        if (d == static_cast<T>(0)) {
            return std::nullopt;
        }

        Mat3 inv;
        inv.m[0] = (m[4] * m[8] - m[5] * m[7]) / d;
        inv.m[1] = (m[2] * m[7] - m[1] * m[8]) / d;
        inv.m[2] = (m[1] * m[5] - m[2] * m[4]) / d;
        inv.m[3] = (m[5] * m[6] - m[3] * m[8]) / d;
        inv.m[4] = (m[0] * m[8] - m[2] * m[6]) / d;
        inv.m[5] = (m[2] * m[3] - m[0] * m[5]) / d;
        inv.m[6] = (m[3] * m[7] - m[4] * m[6]) / d;
        inv.m[7] = (m[1] * m[6] - m[0] * m[7]) / d;
        inv.m[8] = (m[0] * m[4] - m[1] * m[3]) / d;

        return inv;
    }
};

/**
 * @internal
 *
 * @typedef Mat3f
 * @brief Alias for a 3x3 matrix with float elements.
 *
 * This is a specialization of Mat3<T> with T = float.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Mat3f = Mat3<float>;

/**
 * @internal
 *
 * @typedef Mat3d
 * @brief Alias for a 3x3 matrix with double elements.
 *
 * This is a specialization of Mat3<T> with T = double.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Mat3d = Mat3<double>;

/**
 * @internal
 *
 * @typedef Mat3ld
 * @brief Alias for a 3x3 matrix with long double elements.
 *
 * This is a specialization of Mat3<T> with T = long double.
 * Declared as a Qt metatype for use in QVariant and signals/slots.
 */
using Mat3ld = Mat3<long double>;

QDebug operator<<(QDebug dbg, const Mat3d &value);

QDebug operator<<(QDebug dbg, const Mat3f &value);

QDebug operator<<(QDebug dbg, const Mat3ld &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::Mat3f)
Q_DECLARE_METATYPE(PerceptualColor::Mat3d)
Q_DECLARE_METATYPE(PerceptualColor::Mat3ld)

#endif // PERCEPTUALCOLOR_MAT3_H
