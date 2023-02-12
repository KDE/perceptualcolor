// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "interlacingpass.h"

#include "helpermath.h"
#include <qglobal.h>
#include <qmath.h>
#include <type_traits>

namespace PerceptualColor
{
/** @brief Constructor
 *
 * Constructs an object for a new interlacing cycle.
 *
 * @param passCount Number of passes within this interlacing cycle. This MUST
 * be a positive odd number (otherwise an exception is thrown). Use <tt>7</tt>
 * for <a href="https://en.wikipedia.org/wiki/Adam7_algorithm">Adam7</a>
 * interlacing, or any other positive odd number for
 * <a href="https://en.wikipedia.org/wiki/Adam7_algorithm">Adam7</a>-like
 * interlacing, but with a different number of steps.
 *
 * @exception int Thrown when the parameter is not a positive odd number. */
InterlacingPass::InterlacingPass(const int passCount)
{
    if (!isOdd(passCount)) {
        throw 0;
    }
    if (passCount < 1) {
        throw 0;
    }

    const int floorOfHalfCoundown = qFloor(passCount / 2.0);
    const int baseSize = qRound(qPow(2, floorOfHalfCoundown));

    countdown = passCount;
    rectangleSize.setWidth(baseSize);
    rectangleSize.setHeight(baseSize);
    columnFrequency = baseSize;
    columnOffset = 0;
    lineFrequency = baseSize;
    lineOffset = 0;
}

/** @brief Switches to the next pass, reducing @ref countdown by 1 and
 * changing all other values accordingly.
 *
 * If @ref countdown ≤ 0 than nothing happens. */
void InterlacingPass::switchToNextPass()
{
    if (countdown <= 1) {
        return;
    }

    countdown--;

    const int floorOfHalfCoundown = qFloor(countdown / 2.0);
    const int baseSize = qRound(qPow(2, floorOfHalfCoundown));

    if (isOdd(countdown)) {
        rectangleSize.setWidth(baseSize);
        rectangleSize.setHeight(baseSize);
        columnFrequency = baseSize;
        columnOffset = 0;
        lineFrequency = baseSize * 2;
        lineOffset = baseSize;
    } else {
        const int halfBaseSize = baseSize / 2; // Dividing without rounding
        // problems because baseSize is always an even number (it’s always
        // a power of two and bigger than 2 while countdown is ≥ 1).
        rectangleSize.setWidth(halfBaseSize);
        rectangleSize.setHeight(baseSize);
        columnFrequency = baseSize;
        columnOffset = halfBaseSize;
        lineFrequency = baseSize;
        lineOffset = 0;
    }
}

static_assert(std::is_trivially_copyable_v<InterlacingPass>);

static_assert(std::is_standard_layout_v<InterlacingPass>);

} // namespace PerceptualColor
