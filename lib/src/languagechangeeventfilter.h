// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef LANGUAGECHANGEEVENTFILTER_H
#define LANGUAGECHANGEEVENTFILTER_H

#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
class QEvent;

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
    LanguageChangeEventFilter(QObject *parent = nullptr);
    virtual ~LanguageChangeEventFilter() override;
    [[nodiscard]] virtual bool eventFilter(QObject *watched, QEvent *event) override;

private:
    Q_DISABLE_COPY(LanguageChangeEventFilter)

    /** @internal @brief Only for unit tests. */
    friend class TestLanguageChangeEventFilter;
};

} // namespace PerceptualColor

#endif // LANGUAGECHANGEEVENTFILTER_H
