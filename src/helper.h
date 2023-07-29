// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPER_H
#define HELPER_H

#include <qcoreapplication.h>
#include <qglobal.h>
#include <qimage.h>
#include <qmetaobject.h>
#include <qpair.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qthread.h>

class QWheelEvent;
class QWidget;

namespace PerceptualColor
{

void drawQWidgetStyleSheetAware(QWidget *widget);

/** @internal
 *
 * @brief Convenience function template that tests if a value is in a list.
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

/** @internal
 *
 * @brief Force processing of events in a delayed fashion.
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

[[nodiscard]] QPair<QString, QString> getPrefixSuffix(const QString &formatString);

/** @brief The full-qualified C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * @tparam T The enumeration.
 *
 * @pre The enumeration type is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * @returns The full-qualified C++ identifier as QString. */
template<typename T>
QString enumerationToFullString()
{
    const auto myMeta = QMetaEnum::fromType<T>();
    const auto scope = QString::fromUtf8(myMeta.scope());
    const auto name = QString::fromUtf8(myMeta.name());
    return QStringLiteral("%1::%2").arg(scope, name);
}

/** @brief The full-qualified C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * @tparam T The enumeration type. Can usually be omitted.
 *
 * @param enumerator An enumerator.
 *
 * @pre The enumeration type of the enumerator is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * @returns The full-qualified C++ identifier as QString, followed by the
 * underlying integer value in parenthesis. If the enumerator does not
 * exist (for example because you have done a static_cast of an invalid
 * integer to  the enum class), an empty String is returned instead. If
 * the enumerator has synonyms (that means, there exist other enumerators
 * that share the same integer with the current enumerator), all synonym
 * enumerators are returned.
 *
 * @sa @ref enumeratorToString() */
template<typename T>
QString enumeratorToFullString(const T &enumerator)
{
    const auto value = static_cast<int>(enumerator);
    const auto myMeta = QMetaEnum::fromType<T>();

    // QMetaEnum::valueToKeys (identifier with a final s) returns all existing
    // (synonym) keys for a given value. But it also returns happily
    // fantasy strings for non-existing values. Therefore, we have check
    // first with QMetaEnum::valueToKeys (identifier with a final s) which
    // does only return one single key for each value, but is guaranteed to
    // return nullptr if the value has no key.
    if (!myMeta.valueToKey(value)) {
        return QString();
    }

    const auto scope = QString::fromUtf8(myMeta.scope());
    const auto name = QString::fromUtf8(myMeta.name());
    const auto keys = QString::fromUtf8(myMeta.valueToKeys(value));
    return QStringLiteral("%1::%2::%3(%4)").arg(scope, name, keys).arg(value);
}

/** @brief The C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * @tparam T The enumeration type. Can usually be omitted.
 *
 * @param enumerator An enumerator.
 *
 * @pre The enumeration type of the enumerator is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * @returns The C++ identifier as QString, followed by the
 * underlying integer value in parenthesis. If the enumerator does not
 * exist (for example because you have done a static_cast of an invalid
 * integer to  the enum class), an empty String is returned instead. If
 * the enumerator has synonyms (that means, there exist other enumerators
 * that share the same integer with the current enumerator), all synonym
 * enumerators are returned.
 *
 * @sa @ref enumeratorToFullString() */
template<typename T>
QString enumeratorToString(const T &enumerator)
{
    const auto value = static_cast<int>(enumerator);
    const auto myMeta = QMetaEnum::fromType<T>();

    // QMetaEnum::valueToKeys (identifier with a final s) returns all existing
    // (synonym) keys for a given value. But it also returns happily
    // fantasy strings for non-existing values. Therefore, we have check
    // first with QMetaEnum::valueToKeys (identifier with a final s) which
    // does only return one single key for each value, but is guaranteed to
    // return nullptr if the value has no key.
    if (!myMeta.valueToKey(value)) {
        return QString();
    }

    const auto keys = QString::fromUtf8(myMeta.valueToKeys(value));
    return QStringLiteral("%1(%2)").arg(keys).arg(value);
}

} // namespace PerceptualColor

#endif // HELPER_H
