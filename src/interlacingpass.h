// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef InterlacingPass_H
#define InterlacingPass_H

#include <qsize.h>

namespace PerceptualColor
{
/** @internal
 *
 * @brief Describes an interlacing pass.
 *
 * Objects of this class provide information about interlacing passes
 * for <a href="https://en.wikipedia.org/wiki/Adam7_algorithm">Adam7</a>-like
 * interlacing.
 *
 * To do <a href="https://en.wikipedia.org/wiki/Adam7_algorithm">Adam7</a>
 * interlacing, construct an object of this class with a <tt>passCount</tt>
 * of <tt>7</tt>. Do the first pass using the values provided
 * by this object. Then, call @ref switchToNextPass to update the
 * values and do the next interlacing pass.
 *
 * The pixels in your image are divided in lines, who in turn are subdivided
 * in columns. Within each interlacing pass, go through every
 * @ref lineFrequency th line, starting with the line at @ref lineOffset.
 * Within each line, go through every @ref columnFrequency th column, starting
 * with @ref columnOffset. Draw a rectangle with the size @ref rectangleSize
 * who’s top-left pixel is at the current column and line.
 *
 * @note As this is just an internal class which is not part of the public API,
 * there is direct access to its data members for simplicity reasons. However,
 * it is not allowed to change them directly! Use them read-only. */
class InterlacingPass final
{
public:
    explicit InterlacingPass(const int passCount);

    void switchToNextPass();

    /** @brief Size of the rectangles drawn during this pass. */
    QSize rectangleSize;
    /** @brief Draw a rectangle every umpteenth column. */
    int columnFrequency;
    /** @brief First column on a given line to draw a rectangle. */
    int columnOffset;
    /** @brief Draw a rectangle every umpteenth line. */
    int lineFrequency;
    /** @brief First line to process. */
    int lineOffset;
    /** @brief Pass countdown.
     *
     * Inverse counting of the interlacing passes.
     *
     * Example for
     * <a href="https://en.wikipedia.org/wiki/Adam7_algorithm">Adam7</a>:
     *
     * |    Pass     | @ref countdown |
     * | :---------- | :------------: |
     * | 1st         |       7        |
     * | 2nd         |       6        |
     * | 3rd         |       5        |
     * | 4th         |       4        |
     * | 5th         |       3        |
     * | 6th         |       2        |
     * | 7th (last)  |       1        | */
    int countdown;
};

} // namespace PerceptualColor

#endif // InterlacingPass_H
