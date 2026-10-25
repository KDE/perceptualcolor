// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_SETTINGBASE_H
#define PERCEPTUALCOLOR_SETTINGBASE_H

#include <qobject.h>
#include <qpointer.h>
#include <qstring.h>
#include <qtmetamacros.h>

class QSettings;

namespace PerceptualColor
{

class Settings;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Base class for \l Setting.
 *
 *
 *
 * \note \l {https://stackoverflow.com/a/63021891} {The Q_OBJECT
 * macro and templates cannot be combined.} Therefore,
 * \l SettingBase serves as a base class to provide
 * signals for \l Setting. */
class SettingBase : public QObject
{
    Q_OBJECT

public:
    virtual ~SettingBase() override;

Q_SIGNALS:
    /*!
     * \brief Notify signal for the value. */
    void valueChanged();

protected:
    SettingBase(const QString &key, Settings *settings, QObject *parent = nullptr);

    /*!
     * \brief \c {QSettings} key for the value. */
    const QString m_key;

    /*!
     * \brief Corresponding \l Settings object. */
    QPointer<Settings> m_settings;

    QSettings *underlyingQSettings();

private:
    friend class TestSettingBase;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_SETTINGBASE_H
