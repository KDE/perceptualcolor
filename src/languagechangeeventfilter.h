// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_LANGUAGECHANGEEVENTFILTER_H
#define PERCEPTUALCOLOR_LANGUAGECHANGEEVENTFILTER_H

#include <qglobal.h>
#include <qobject.h>
#include <qtmetamacros.h>
class QEvent;

namespace PerceptualColor
{
/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief An event filter that discards \c {QEvent::LanguageChange}
 * events.
 *
 * All other events are kept.
 *
 * To use it, create an instance of this class. Then, call
 * \c {QObject::installEventFilter()} on the object for which
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

    friend class TestLanguageChangeEventFilter;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_LANGUAGECHANGEEVENTFILTER_H
