// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef LANGUAGECHANGEEVENTFILTER_H
#define LANGUAGECHANGEEVENTFILTER_H

#include <qglobal.h>
#include <qobject.h>
class QEvent;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
/** @internal
 *
 * @brief An event filter that discards <tt>QEvent::LanguageChange</tt>
 * events.
 *
 * All other events are kept.
 *
 * To use it, create an instance of this class. Then, call
 * <tt>QObject::installEventFilter()</tt> on the object for which
 * you want to filter the events. */
class LanguageChangeEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit LanguageChangeEventFilter(QObject *parent = nullptr);
    virtual ~LanguageChangeEventFilter() override;
    [[nodiscard]] virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Q_DISABLE_COPY(LanguageChangeEventFilter)

    /** @internal @brief Only for unit tests. */
    friend class TestLanguageChangeEventFilter;
};

} // namespace PerceptualColor

#endif // LANGUAGECHANGEEVENTFILTER_H
