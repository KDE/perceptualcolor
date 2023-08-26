// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "settingbase.h"

#include "settings.h"

class QSettings;

namespace PerceptualColor
{

// // Example stream operator
// QDataStream& operator<<(QDataStream& out, const Settings::ColorList& colorList)
// {
//     out << static_cast<qint32>(colorList.size());
//     for (const QColor& color : colorList) {
//         out << color;
//     }
//     return out;
// }
//
// // Example stream operator
// QDataStream& operator>>(QDataStream& in, Settings::ColorList& colorList)
// {
//     colorList.clear();
//     qint32 size;
//     in >> size;
//     for (int i = 0; i < size; ++i) {
//         QColor color;
//         in >> color;
//         colorList.append(color);
//     }
//     return in;
// }

/** @brief Constructor.
 *
 * @param key <tt>QSettings</tt> key for the value.
 * For maximum portability:
 * - No upper case should ever be used.
 *   (Some systems, like the INI that we are using, are case-insensitive.
 *   And even if we always use INI, having both capital and small letters
 *   is error-prone because typos are not checked by the compiler.)
 * - Only the letters a-z should be used.
 *   (Also, some characters like the backslash are not allowed on some
 *   platforms.)
 * - “group/key”: Each key has exactly one group. Don't use subgroups.
 *   Use the class name as group name.
 *   (This makes the settings file well readable for humans. Missing
 *   groups are confusing because the system generates a “General”
 *   group which is not easy to understand. And using class identifiers
 *   helps to understand the structure of the settings file.)
 * - In C++, use “const” variables to define key strings, instead of
 *   manually typing the key strings.
 *   (This avoids typing errors.)
 * @param settings Corresponding @ref Settings object.
 * @param parent The parent object (if any). */
SettingBase::SettingBase(const QString &key, Settings *settings, QObject *parent)
    : QObject(parent)
    , m_key(key)
    , m_settings(settings)
{
}

/** @brief Destructor. */
SettingBase::~SettingBase()
{
}

/** @brief The underlying <tt>QSettings</tt> object of @ref m_settings.
 *
 * @returns the underlying <tt>QSettings</tt> object of @ref m_settings. */
QSettings *SettingBase::underlyingQSettings()
{
    return &(m_settings->m_qSettings);
}

} // namespace PerceptualColor
