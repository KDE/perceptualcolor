// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef HELPER_H
#define HELPER_H

#include <qcoreapplication.h>
#include <qglobal.h>
#include <qimage.h>
#include <qthread.h>

class QWheelEvent;
class QWidget;

/** @internal
 *
 * @file
 *
 * General helper elements. */

namespace PerceptualColor
{

void drawQWidgetStyleSheetAware(QWidget *widget);

/** @brief Convenience function template that tests if a value is in a list.
 *
 * @param first The value
 * @param t The list
 *
 * Usage:
 * @snippet testhelper.cpp isInUsage
 *
 * @returns <tt>true</tt> if “value” is in “list”. <tt>false</tt> otherwise. */
template<typename First, typename... T>
bool isIn(First &&first, T &&...t)
{
    // Solution as proposed by Nikos C. in https://stackoverflow.com/a/15181949
    return ((first == t) || ...);
}

[[nodiscard]] qreal standardWheelStepCount(QWheelEvent *event);

[[nodiscard]] QImage transparencyBackground(qreal devicePixelRatioF);

/** @brief Force processing of events in a delayed fashion.
 *
 * When there is no running event loop (like in unit tests or in tools
 * like the screenshot generator), some parts of the asynchronous API
 * of this library does not work. Calling this function fixes this by
 * forcing the processing of pending events, but with some delay
 * in-between, so that maybe existing parallel threads have also
 * a chance to terminate their work.
 *
 * @param msecWaitInitially Delay before starting event processing.
 * @param msecWaitBetweenEventLoopPasses Delay before each pass through
 *        through the pending events.
 * @param numberEventLoopPasses Number of passes through the pending events.
 *
 * @internal
 *
 * @note This is declared as template to prevent that this code is compiled
 * into the library itself, which does <em>not</em> actually use it itself,
 * but includes this header file. */
template<typename T = void>
void delayedEventProcessing(unsigned long msecWaitInitially = 50, unsigned long msecWaitBetweenEventLoopPasses = 50, int numberEventLoopPasses = 3)
{
    // Some OSes might round the sleep time up to 15 ms. We do it ourself
    // here to make the behaviour a little bit more predictable.
    msecWaitInitially = qMax<unsigned long>( //
        msecWaitInitially, //
        15);
    msecWaitBetweenEventLoopPasses = //
        qMax<unsigned long>(msecWaitBetweenEventLoopPasses, 15);

    QThread::msleep(msecWaitInitially);
    // Hopefully, now the render function has terminated…
    for (int i = 0; i < numberEventLoopPasses; ++i) {
        // Wait again (apparently, threaded event processing needs some time…)
        QThread::msleep(msecWaitBetweenEventLoopPasses);
        QCoreApplication::processEvents();
    }
}

} // namespace PerceptualColor

#endif // HELPER_H
