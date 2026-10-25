// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_INTERLACINGPASS_H
#define PERCEPTUALCOLOR_INTERLACINGPASS_H

#include "helpermath.h"
#include <qsize.h>

namespace PerceptualColor
{
/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Describes an interlacing pass.
 *
 * Objects of this class provide information about interlacing passes
 * for \l {https://en.wikipedia.org/wiki/Adam7_algorithm} {Adam7}-like
 * interlacing.
 *
 * To do \l {https://en.wikipedia.org/wiki/Adam7_algorithm} {Adam7}
 * interlacing, construct an object of this class with a \c {passCount}
 * of \c {7}. Do the first pass using the values provided
 * by this object. Then, call \l switchToNextPass to update the
 * values and do the next interlacing pass.
 *
 * The pixels in your image are divided in lines, who in turn are subdivided
 * in columns. Within each interlacing pass, go through every
 * \l lineFrequency th line, starting with the line at \l lineOffset.
 * Within each line, go through every \l columnFrequency th column, starting
 * with \l columnOffset. Draw a rectangle with the size \l rectangleSize
 * who’s top-left pixel is at the current column and line.
 *
 * \note As this is just an internal class which is not part of the public API,
 * there is direct access to its data members for simplicity reasons. However,
 * it is not allowed to change them directly! Use them read-only. */
class InterlacingPass
{
public:
    explicit InterlacingPass(const double passCount);

    // NOTE QDoc unfortunately does not support documentation for function
    // templates that are members of non-template classes. Therefore, the
    // following documentation will be ignored by QDoc.
    /*!
     * \brief Factory constructor
     *
     * Constructs an object for a new interlacing cycle.
     *
     * \b passCount Number of passes within this interlacing
     * cycle. Use \c {7}
     * for \l {https://en.wikipedia.org/wiki/Adam7_algorithm} {Adam7}
     * interlacing, or any other positive odd number for
     * \l {https://en.wikipedia.org/wiki/Adam7_algorithm} {Adam7}-like
     * interlacing, but with a different number of steps.
     * The value must be an integer type, a \c {constexpr} and
     * a positive odd number.
     *
     * This function checks for valid parameters at compile time.
     *
     * \sa InterlacingPass
     */
    template<int passCount>
    [[nodiscard]] static InterlacingPass make()
    {
        static_assert(passCount > 0, "passCount must be positive.");
        static_assert(isOdd(passCount), "passCount must be odd.");
        return InterlacingPass(passCount);
    }

    void switchToNextPass();

    /*!
     * \brief Size of the rectangles drawn during this pass. */
    QSize rectangleSize;
    /*!
     * \brief Draw a rectangle every umpteenth column. */
    int columnFrequency;
    /*!
     * \brief First column on a given line to draw a rectangle. */
    int columnOffset;
    /*!
     * \brief Draw a rectangle every umpteenth line. */
    int lineFrequency;
    /*!
     * \brief First line to process. */
    int lineOffset;
    /*!
     * \brief Pass \l countdown.
     *
     * Inverse counting of the interlacing passes.
     *
     * Example for
     * \l {https://en.wikipedia.org/wiki/Adam7_algorithm} {Adam7}:
     *
     * \table
     *     \header \li Pass            \li \l countdown
     *     \row    \li 1\sup st        \li 7
     *     \row    \li 2\sup nd        \li 6
     *     \row    \li 3\sup rd        \li 5
     *     \row    \li 4\sup th        \li 4
     *     \row    \li 5\sup th        \li 3
     *     \row    \li 6\sup th        \li 2
     *     \row    \li 7\sup th (last) \li 1
     * \endtable
     */
    int countdown;

private:
    friend class TestInterlacingPass;

    [[nodiscard]] static int roundToNearestPositiveOdd(const double value);
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_INTERLACINGPASS_H
