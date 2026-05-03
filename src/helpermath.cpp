// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helpermath.h"

#include <qmath.h>

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Calculates the required number of decimals to achieve the requested
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

/**
 * @internal
 *
 * @brief Rounds a given floating point value to the nearest multiple of a
 * specified integer.
 *
 * This function takes a floating point value and rounds it to the nearest
 * multiple of an integer. If the multiple is zero, the original number is
 * returned (rounded).
 *
 * @param num The integer to be rounded.
 * @param multiple The multiple to which the number will be rounded.
 *
 * @return The rounded integer.
 */
int roundToNearestMultiple(double num, int multiple)
{
    if (multiple == 0) {
        return qRound(num);
    }
    return qRound(num / multiple) * multiple;
}

/**
 * @internal
 *
 * @brief Rounds a floating-point number to the nearest multiple of an integer.
 *
 * @param num The number to be rounded
 * @param multiple The multiple to round to
 *
 * @returns int The rounded result, rounding halfway cases away from zero.
 */
int roundToIntMultiple(double num, int multiple)
{
    if (multiple == 0) {
        // Avoid division by zero
        multiple = 1;
    }
    return static_cast<int>(std::round(num / multiple) * multiple);
}

/**
 * @internal
 *
 * @brief Rounds a floating-point number to the nearest multiple of
 * a floating-point number.
 *
 * @param num The number to be rounded
 * @param multiple The multiple to round to
 *
 * @returns int The rounded result, rounding halfway cases away from zero.
 */
double roundToFloatMultiple(double num, double multiple)
{
    if (multiple == 0) {
        return num;
    }
    return std::round(num / multiple) * multiple;
}

} // namespace PerceptualColor
