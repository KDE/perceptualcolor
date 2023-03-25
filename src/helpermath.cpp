// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helpermath.h"

#include <optional>
#include <qgenericmatrix.h>
#include <qmath.h>

namespace PerceptualColor
{

/** @internal
 *
 * @brief Convenience constructor for @ref SquareMatrix3.
 *
 * @param r0c0 row 0, column 0
 * @param r0c1 row 0, column 1
 * @param r0c2 row 0, column 2
 * @param r1c0 row 1, column 0
 * @param r1c1 row 1, column 1
 * @param r1c2 row 1, column 2
 * @param r2c0 row 2, column 0
 * @param r2c1 row 2, column 1
 * @param r2c2 row 2, column 2
 *
 * @returns The corresponding @ref SquareMatrix3. */
SquareMatrix3 createSquareMatrix3(double r0c0, double r0c1, double r0c2, double r1c0, double r1c1, double r1c2, double r2c0, double r2c1, double r2c2)
{
    // clang-format off
    const double valueArray[] = {r0c0, r0c1, r0c2,
                                 r1c0, r1c1, r1c2,
                                 r2c0, r2c1, r2c2};
    // clang-format on
    return SquareMatrix3(valueArray);
}

/** @internal
 *
 * @brief Convenience constructor for @ref Trio.
 *
 * @param first first value
 * @param second second value
 * @param third third value
 *
 * @returns The corresponding @ref Trio. */
Trio createTrio(double first, double second, double third)
{
    const double valueArray[] = {first, second, third};
    return Trio(valueArray);
}

/** @internal
 *
 * @brief Try to find the inverse matrix.
 *
 * @param matrix The original matrix.
 *
 * @returns If the original matrix is invertible, its inverse matrix.
 * An empty value otherwise. */
std::optional<SquareMatrix3> inverseMatrix(const SquareMatrix3 &matrix)
{
    const double &a = matrix(0, 0);
    const double &b = matrix(0, 1);
    const double &c = matrix(0, 2);
    const double &d = matrix(1, 0);
    const double &e = matrix(1, 1);
    const double &f = matrix(1, 2);
    const double &g = matrix(2, 0);
    const double &h = matrix(2, 1);
    const double &i = matrix(2, 2);
    const auto determinant = a * e * i //
        + b * f * g //
        + c * d * h //
        - c * e * g //
        - b * d * i //
        - a * f * h;
    if (determinant == 0) {
        return std::nullopt;
    }
    // clang-format off
    const auto temp = createSquareMatrix3(
        e * i - f * h, c * h - b * i, b * f - c * e,
        f * g - d * i, a * i - c * g, c * d - a * f,
        d * h - e * g, b * g - a * h, a * e - b * d);
    // clang-format on
    return temp / determinant;
}

/** @brief Calculates the required number of decimals to achieve the requested
 * number of significant figures within the given range.
 *
 * @param rangeMax The maximum value of the range [0, rangeMax].
 * @param significantFigures The requested number of significant figures.
 *
 * | maxRange | decimalPlaces(maxRange, 2) | decimalPlaces(maxRange, 3) | decimalPlaces(maxRange, 4) |
 * | -------: | -------------------------: | -------------------------: | -------------------------: |
 * |        1 |                          1 |                          2 |                          3 |
 * |        2 |                          1 |                          2 |                          3 |
 * |      100 |                          0 |                          0 |                          1 |
 * |      255 |                          0 |                          0 |                          1 |
 * |      360 |                          0 |                          0 |                          1 |
 *
 * @returns The number of decimal places after the decimal point (in addition
 * to the whole number part) required to achieve the requested
 * number of significant figures within the given range. */
int decimalPlaces(const int rangeMax, const int significantFigures)
{
    const auto myLog10Value = std::log10(qAbs(rangeMax));
    const int wholeNumberDigits = (rangeMax == 0) //
        ? 1 // special case
        : qFloor(myLog10Value) + 1;
    return qMax(0, significantFigures - wholeNumberDigits);
}

} // namespace PerceptualColor
