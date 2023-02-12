// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERMATH_H
#define HELPERMATH_H

#include "lchdouble.h"
#include <cmath>
#include <lcms2.h>
#include <limits>
#include <qcolor.h>
#include <qcoreapplication.h>
#include <qglobal.h>
#include <qimage.h>
#include <qlist.h>
#include <qstring.h>
#include <qthread.h>
#include <stdlib.h>
#include <type_traits>

/** @internal
 *
 * @file
 *
 * Mathematical helper functions. */

namespace PerceptualColor
{

/** @internal
 *
 * @brief Template function to test if a value is within a certain range
 * @param low the lower limit
 * @param x the value that will be tested
 * @param high the higher limit
 * @returns @snippet this Helper isInRange */
template<typename T>
[[nodiscard]] constexpr bool isInRange(const T &low, const T &x, const T &high)
{
    return (
        // The Doxygen comments contain @private because apparently
        // the tag @internal is not enough to hide it in the API documentation.
        // The snippet marker [] is hidden within HTML comments to avoid
        // that is shows up literally in the private documentation, and this
        // independent from the HIDE_IN_BODY_DOCS parameter in Doxyfile.
        //! @private @internal <!-- [Helper isInRange] -->
        (low <= x) && (x <= high)
        //! @private @internal <!-- [Helper isInRange] -->
    );
}

/** @internal
 *
 * @brief Test if an integer is odd.
 *
 * @param number The number to test. Must be an integer type
 *
 * @returns <tt>true</tt> if the number is odd, <tt>false</tt> otherwise. */
template<typename T>
[[nodiscard]] constexpr bool isOdd(const T &number)
{
    static_assert(std::is_integral_v<T>, //
                  "Template isOdd() only works with integer types.");
    constexpr T two = 2;
    return static_cast<bool>(number % two);
}

/** @internal
 *
 * @brief Round floating point numbers to a certain number of digits
 *
 * @tparam T a floating point type
 * @param value the value that will be rounded
 * @param precision the number of decimal places to which rounding takes place
 * @returns the rounded value */
template<typename T>
[[nodiscard]] constexpr T roundToDigits(T value, int precision)
{
    static_assert( //
        std::is_floating_point<T>::value, //
        "Template roundToDigits() only works with floating point types");
    const T multiplier = std::pow(
        // Make sure that pow returns a T:
        static_cast<T>(10),
        precision);
    return std::round(value * multiplier) / multiplier;
}

/** @brief Test if two floating point values are nearly equal.
 *
 * Comparison is done in a relative way, where the
 * exactness is stronger the smaller the numbers are.
 * <a href="https://embeddeduse.com/2019/08/26/qt-compare-two-floats/">
 * This is the reasonable behaviour for floating point comparisons.</a>
 * Unlike <tt>qFuzzyCompare</tt> and <tt>qFuzzyIsNull</tt> this function
 * works for both cases: numbers near to 0 and numbers far from 0.
 *
 * @tparam T Must be a floating point type.
 * @param a one of the values to compare
 * @param b one of the values to compare
 * @param epsilon indicator for desired precision assuming that the values
 *        to compare are close to 1. Values lower the the compiler-epsilon
 *        from type T will be replaced by the compiler-epsilon from type T.
 *        If epsilon is infinity or near to the maximum value of type T,
 *        the result of this function might be wrong.
 * @returns <tt>true</tt> if the values are nearly equal. <tt>false</tt>
 * otherwise.
 *
 * @sa @ref isNearlyEqual(A a, B b) provides a default epsilon. */
template<typename T>
[[nodiscard]] constexpr bool isNearlyEqual(T a, T b, T epsilon)
{
    static_assert( //
        std::is_floating_point<T>::value, //
        "Template isNearlyEqual(T a, T b, T epsilon) only works with floating point types");

    // Implementation based on https://stackoverflow.com/a/32334103
    const auto actualEpsilon = //
        qMax(std::numeric_limits<T>::epsilon(), epsilon);

    if ((a == b) && (!std::isnan(epsilon))) {
        // Not explicitly checking if a or b are NaN, because if any of those
        // is NaN, the code above will return “false” anyway.
        return true;
    }

    const auto norm = qMin<T>( //
        qAbs(a) + qAbs(b), //
        std::numeric_limits<T>::max());
    return std::abs(a - b) < std::max(actualEpsilon, actualEpsilon * norm);
}

/** @brief Test if two floating point values are nearly equal, using
 * a default epsilon.
 *
 * Calls @ref isNearlyEqual(T a, T b, T epsilon) with a default epsilon
 * who’s value depends on the type with <em>less</em> precision among A and B.
 *
 * @tparam A Must be a floating point type.
 * @tparam B Must be a floating point type.
 * @param a one of the values to compare
 * @param b one of the values to compare
 * @returns <tt>true</tt> if the values are nearly equal. <tt>false</tt>
 * otherwise. */
template<typename A, typename B>
[[nodiscard]] constexpr bool isNearlyEqual(A a, B b)
{
    static_assert( //
        std::is_floating_point<A>::value, //
        "Template isNearlyEqual(A a, B b) only works with floating point types");
    static_assert( //
        std::is_floating_point<B>::value, //
        "Template isNearlyEqual(A a, B b) only works with floating point types");

    // Define a factor to multiply with. Our epsilon has to be bigger than
    // std::numeric_limits<>::epsilon(), which represents the smallest
    // representable difference for the value 1.0. Doing various consecutive
    // floating point operations will increase the error, therefore we need
    // a factor with which we multiply std::numeric_limits<>::epsilon().
    // The choice is somewhat arbitrary. Qt’s qFuzzyCompare uses this:
    //
    // float:
    // std::numeric_limits<>::epsilon() is around 1.2e-07.
    // Qt uses 1e-5.
    // Factor is around 100
    //
    // double:
    // std::numeric_limits<>::epsilon() is around 2.2e-16.
    // Qt uses 1e-12.
    // Factor is around 5000
    //
    // long double:
    // std::numeric_limits<>::epsilon() might vary depending on implementation,
    // as “long double” might have different sizes on different implementations.
    // Qt does not support “long double” in qFuzzyCompare.
    constexpr auto factor = 100;

    // Use the type with less precision to get epsilon, but use the type
    // with more precision to the the actual comparison.
    if constexpr (sizeof(A) > sizeof(B)) {
        return PerceptualColor::isNearlyEqual<A>( //
            a, //
            static_cast<A>(b), //
            static_cast<A>(std::numeric_limits<B>::epsilon() * factor));
    } else {
        return PerceptualColor::isNearlyEqual<B>( //
            static_cast<B>(a), //
            b, //
            static_cast<B>(std::numeric_limits<A>::epsilon() * factor));
    }
}

/** @brief Normalizes an angle.
 *
 * |      Value      | Normalized Value |
 * | :-------------: | :--------------: |
 * | <tt>  0°  </tt> | <tt>  0°  </tt>  |
 * | <tt>359.9°</tt> | <tt>359.9°</tt>  |
 * | <tt>360°  </tt> | <tt>  0°  </tt>  |
 * | <tt>361.2°</tt> | <tt>  1.2°</tt>  |
 * | <tt>720°  </tt> | <tt>  0°  </tt>  |
 * | <tt> −1°  </tt> | <tt>359°  </tt>  |
 * | <tt> −1.3°</tt> | <tt>358.7°</tt>  |
 *
 * @param value an angle (coordinates in degree)
 * @returns the value, normalized to the range 0° ≤ value < 360° */
template<typename T>
T normalizedAngleDegree(T value)
{
    static_assert( //
        std::is_floating_point<T>::value, //
        "Template normalizeAngleDegree() only works with floating point types");
    constexpr T min = 0;
    constexpr T max = 360;
    qreal temp = fmod(value, max);
    if (temp < min) {
        temp += max;
    }
    return temp;
}

} // namespace PerceptualColor

#endif // HELPERMATH_H
