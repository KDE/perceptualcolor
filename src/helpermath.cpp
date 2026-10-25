// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helpermath.h"

#include <qmath.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \brief Calculates the required number of decimals to achieve the requested
 * number of significant figures within the given range.
 *
 * \a rangeMax The maximum value of the range [0, rangeMax].
 *
 * \a significantFigures The requested number of significant figures.
 *
 * \table
 *     \header
 *         \li maxRange
 *         \li decimalPlaces(maxRange, 2)
 *         \li decimalPlaces(maxRange, 3)
 *         \li decimalPlaces(maxRange, 4)
 *     \row \li   1 \li 1 \li 2 \li 3
 *     \row \li   2 \li 1 \li 2 \li 3
 *     \row \li 100 \li 0 \li 0 \li 1
 *     \row \li 255 \li 0 \li 0 \li 1
 *     \row \li 360 \li 0 \li 0 \li 1
 * \endtable
 *
 * Returns the number of decimal places after the decimal point (in addition
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

/*!
 *
 *
 * \brief Rounds a given floating point value to the nearest multiple of a
 * specified integer.
 *
 * This function takes a floating point value and rounds it to the nearest
 * multiple of an integer. If the multiple is zero, the original number is
 * returned (rounded).
 *
 * \a num The integer to be rounded.
 *
 * \a multiple The multiple to which the number will be rounded.
 *
 * Returns the rounded integer.
 */
int roundToNearestMultiple(double num, int multiple)
{
    if (multiple == 0) {
        return qRound(num);
    }
    return qRound(num / multiple) * multiple;
}

/*!
 *
 *
 * \brief Rounds a floating-point number to the nearest multiple of an integer.
 *
 * \a num The number to be rounded
 *
 * \a multiple The multiple to round to
 *
 * Returns he rounded result, rounding halfway cases away from zero.
 */
int roundToIntMultiple(double num, int multiple)
{
    if (multiple == 0) {
        // Avoid division by zero
        multiple = 1;
    }
    return static_cast<int>(std::round(num / multiple) * multiple);
}

/*!
 *
 *
 * \brief Rounds a floating-point number to the nearest multiple of
 * a floating-point number.
 *
 * \a num The number to be rounded
 *
 * \a multiple The multiple to round to
 *
 * Returns he rounded result, rounding halfway cases away from zero.
 */
double roundToFloatMultiple(double num, double multiple)
{
    if (multiple == 0) {
        return num;
    }
    return std::round(num / multiple) * multiple;
}

} // namespace PerceptualColor
