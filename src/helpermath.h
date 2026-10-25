// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_HELPERMATH_H
#define PERCEPTUALCOLOR_HELPERMATH_H

#include <cmath>
#include <limits>
#include <qglobal.h>
#include <type_traits>

namespace PerceptualColor
{

int decimalPlaces(const int rangeMax, const int significantFigures);

int roundToNearestMultiple(double num, int multiple);

/*!
 *
 *
 * \brief Template function to test if a value is within a certain range
 *
 * \a low the lower limit
 *
 * \a x the value that will be tested
 *
 * \a high the higher limit
 *
 * Returns:
 *
 * \snippet helpermath.h Helper isInRange
 */
template<typename T>
[[nodiscard]] constexpr bool isInRange(const T &low, const T &x, const T &high)
{
    return (
        //! [Helper isInRange]
        (low <= x) && (x <= high)
        //! [Helper isInRange]
    );
}

/*!
 *
 *
 * \brief Test if an integer is odd.
 *
 * \a number The number to test. Must be an integer type
 *
 * Returns \c {true} if the number is odd, \c {false} otherwise. */
template<typename T>
[[nodiscard]] constexpr bool isOdd(const T &number)
{
    static_assert(std::is_integral_v<T>, //
                  "Template isOdd() only works with integer types.");
    constexpr T two = 2;
    return static_cast<bool>(number % two);
}

/*!
 *
 *
 * \brief Test if two floating point values are nearly equal.
 *
 * Comparison is done in a relative way, where the
 * exactness is stronger the smaller the numbers are.
 * \l {https://embeddeduse.com/2019/08/26/qt-compare-two-floats/}
 * {This is the reasonable behaviour for floating point comparisons.}
 * Unlike \c {qFuzzyCompare} and \c {qFuzzyIsNull} this function
 * works for both cases: numbers near to 0 and numbers far from 0.
 *
 * \b T Must be a floating point type.
 *
 * \a a one of the values to compare
 *
 * \a b one of the values to compare
 *
 * \a epsilon indicator for desired precision assuming that the values
 *        to compare are close to 1. Values lower the the compiler-epsilon
 *        from type T will be replaced by the compiler-epsilon from type T.
 *        If epsilon is infinity or near to the maximum value of type T,
 *        the result of this function might be wrong.
 *
 * Returns \c {true} if the values are nearly equal. \c {false}
 * otherwise.
 *
 * \note \l isNearlyEqual(A a, B b) provides a default epsilon.
 */
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
    return qAbs(a - b) < std::max(actualEpsilon, actualEpsilon * norm);
}

/*!
 *
 *
 * \brief Test if two floating point values are nearly equal, using
 * a default epsilon.
 *
 * Calls \l isNearlyEqual(T a, T b, T epsilon) with a default epsilon
 * who’s value depends on the type with \e {less} precision among A and B.
 *
 * \a a one of the values to compare. Must be a floating point type.
 *
 * \a b one of the values to compare. Must be a floating point type.
 *
 * Returns \c {true} if the values are nearly equal. \c {false}
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

/*!
 * \inmodule ModulePrivateApi
 *
 * \brief Normalizes an angle.
 *
 * \table
 *     \header \li Value       \li Normalized Value
 *     \row    \li \c {  0°  } \li \c {  0°  }
 *     \row    \li \c {359.9°} \li \c {359.9°}
 *     \row    \li \c {360°  } \li \c {  0°  }
 *     \row    \li \c {361.2°} \li \c {  1.2°}
 *     \row    \li \c {720°  } \li \c {  0°  }
 *     \row    \li \c { −1°  } \li \c {359°  }
 *     \row    \li \c { −1.3°} \li \c {358.7°}
 * \endtable
 *
 * \a value an angle (coordinates in degree)
 *
 * Returns the value, normalized to the range 0° ≤ value < 360°
 */
template<typename T>
T normalizedAngle360(T value)
{
    static_assert( //
        std::is_floating_point<T>::value, //
        "Template normalizeAngle360() only works with floating point types");
    constexpr T min = 0;
    constexpr T max = 360;
    qreal temp = fmod(value, max);
    if (temp < min) {
        temp += max;
    }
    return temp;
}

/*!
 *
 *
 * \brief Normalizes polar coordinates.
 *
 * \a radius Reference to the radius. It will get normalized to value ≥ 0.
 *        If it was < 0 (but not if it was 0 with a negative sign) its  sign
 *        is changed and angleDegree is turned by 180°.
 *
 * \a angleDegree Reference to the angle (measured in degree). It will get
 *        normalized to 0° ≤ value < 360° (see \l normalizedAngle360() for
 *        details)
 *
 * \note When the radius is 0, one could set by convention the (meaningless)
 * angle also 0. However, note that this function does \e {not} do that!
 */
template<typename T>
void normalizePolar360(T &radius, T &angleDegree)
{
    if (radius < 0) {
        radius *= (-1);
        angleDegree = normalizedAngle360(angleDegree + 180);
    } else {
        angleDegree = normalizedAngle360(angleDegree);
    }
}

/*!
 *
 *
 * \brief Rounds a floating-point value to a specified number of digits.
 *
 * \b T Floating-point type
 *
 * \a value The value to be rounded.
 *
 * \a precision Number of digits to round to: For positive values, it rounds
 *    to that many decimal places. For negative values, it rounds to tens,
 *    hundreds, thousands, etc.
 *
 * Returns the rounded value
 *
 * Examples:
 *
 * \snippet testhelpermath.cpp roundToDigitsExample
 *
 * \note This function is constexpr only starting with C++23.
 */
template<typename T>
[[nodiscard]] constexpr T roundToDigits(T value, int precision)
{
    static_assert( //
        std::is_floating_point<T>::value, //
        "Template roundToDigits() only works with floating point types");

    // Compute 10^precision manually. It would be easier to use
    // const T multiplier = std::pow(static_cast<T>(10), precision);
    // but std::pow() is not constexpr before probably C++26, and qPow() is
    // also not constexpr.
    T multiplier = static_cast<T>(1);
    if (precision > 0) {
        for (int i = 0; i < precision; ++i)
            multiplier *= static_cast<T>(10);
    } else if (precision < 0) {
        // cppcheck-suppress knownConditionTrueFalse // false positive
        for (int i = 0; i < -precision; ++i)
            multiplier /= static_cast<T>(10);
    }

    // std::round is constexpr starting with C++23
    // constexpr qRound() is less exact, returns integers instead of floating
    // point value, and therefore not an alternative.
    return std::round(value * multiplier) / multiplier;
}

double roundToFloatMultiple(double num, double multiple);

int roundToIntMultiple(double num, int multiple);

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_HELPERMATH_H
