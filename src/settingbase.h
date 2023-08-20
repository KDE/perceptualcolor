// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SETTINGBASE_H
#define SETTINGBASE_H

#include "settings.h"
#include <qcolor.h>
#include <qdebug.h>
#include <qfilesystemwatcher.h>
#include <qglobal.h>
#include <qlist.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qpointer.h>
#include <qsettings.h>
#include <qstring.h>
#include <qstringliteral.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

namespace PerceptualColor
{

/** @internal
 *
 * @brief Base class for @ref Setting.
 *
 * @internal
 *
 * @note <a href="https://stackoverflow.com/a/63021891">The <tt>Q_OBJECT</tt>
 * macro and templates cannot be combined.</a> Therefore,
 * @ref SettingBase serves as a base class to provide
 * signals for @ref Setting. */
class SettingBase : public QObject
{
    Q_OBJECT

public:
    virtual ~SettingBase() override;

Q_SIGNALS:
    /** @brief Notify signal for the value. */
    void valueChanged();

protected:
    SettingBase(const QString &key, Settings *settings, QObject *parent = nullptr);

    /** @brief <tt>QSettings</tt> key for the value. */
    const QString m_key;

    /** @brief Corresponding @ref Settings object. */
    QPointer<Settings> m_settings;

    QSettings *underlyingQSettings();

private:
    /** @internal @brief Only for unit tests. */
    friend class TestSettingBase;
};

} // namespace PerceptualColor

#endif // SETTINGBASE_H
