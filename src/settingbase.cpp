// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "settingbase.h"

#include "settings.h"

class QSettings;

namespace PerceptualColor
{

/*!
 * \brief Constructor.
 *
 * \a key \c {QSettings} key for the value.
 *
 * For maximum portability, choose your \c key carefully:
 *
 * \list
 * \li No upper case should ever be used.
 *   (Some systems, like the INI that we are using, are case-insensitive.
 *   And even if we always use INI, having both capital and small letters
 *   is error-prone because typos are not checked by the compiler.)
 * \li Only the letters a-z should be used.                   *
 *   (Also, some characters like the slash and backslash are not allowed on
 *   many platforms.)
 * \li Key strings must follow the format “groupname/keyname”. Each keyname
 *   should belong to a group, separated by a slash. You can
 *   use nested groups like “groupname/subgroupname/keyname”. Use the
 *   name of the class associated with the setting as the group name. This
 *   convention improves readability of the settings file: Omitting a group
 *   causes the system to assign the key to a default “General” group,
 *   which can be unexpected and confusing. Using class names as group
 *   names clarifies the origin and purpose of each setting.
 * \li In C++, use “const” variables to define key strings, instead of
 *   manually typing the key strings.
 *   (This avoids typing errors.)
 * \endlist
 *
 * \a settings Corresponding \l Settings object.
 *
 * \a parent The parent object (if any).
 */
SettingBase::SettingBase(const QString &key, Settings *settings, QObject *parent)
    : QObject(parent)
    , m_key(key)
    , m_settings(settings)
{
}

/*!
 * \brief Destructor. */
SettingBase::~SettingBase()
{
}

/*!
 * \brief The underlying \c {QSettings} object of \l m_settings.
 */
QSettings *SettingBase::underlyingQSettings()
{
    return &(m_settings->m_qSettings);
}

} // namespace PerceptualColor
