// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SETTING_H
#define SETTING_H

#include "settingbase.h"
#include "settings.h"
#include <qcolor.h>
#include <qdebug.h>
#include <qfilesystemwatcher.h>
#include <qglobal.h>
#include <qlist.h>
#include <qmetatype.h>
#include <qobject.h>
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
 * @brief A single setting within @ref Settings.
 *
 * @tparam T Type of the setting value. The type must qualify for
 *           registration as QMetaType and also provide a stream
 *           operator. @ref PerceptualSettings::ColorList and many
 *           build-in types qualify.) */
template<typename T>
class Setting final : public SettingBase
{
public:
    Setting(const QString &key, Settings *settings, QObject *parent = nullptr);
    virtual ~Setting() override;

    T value() const;

    void setValue(const T &newValue);

private:
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    Setting(const Setting &) = delete;
    Setting &operator=(const Setting &) = delete;

    /** @brief Internal storage for the value. */
    T m_value = T();

    void updateFromQSettings();

    /** @internal @brief Only for unit tests. */
    friend class TestSetting;
};

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
 * @param settings Corresponding @ref Settings object. This object must
 *        stay available during the live-time of this object.
 * @param parent The parent object (if any).
 *
 * @warning You must not create more than one instance with the same
 * combination between key and @ref Settings object. This would result
 * in undefined behaviour. (Probably some values would be out-of-sync.) */
template<typename T>
Setting<T>::Setting(const QString &key, Settings *settings, QObject *parent)
    : SettingBase(key, settings, parent)
{
    // QSettings seems to use indirectly QMetaType::load() which requires
    // to register all custom types as QMetaType.
    qRegisterMetaType<T>();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // Also stream operators are required.
    qRegisterMetaTypeStreamOperators<T>();
#endif

    // Initialize the internal value:
    updateFromQSettings();

    // Make sure further updates are processed.
    connect(settings, //
            &Settings::updatedAfterFileChange, //
            this, //
            &PerceptualColor::Setting<T>::updateFromQSettings //
    );
}

/** @brief Destructor. */
template<typename T>
Setting<T>::~Setting()
{
}

/** @brief Updates the value to the corresponding value
 * from @ref underlyingQSettings().
 *
 * Only reads from @ref underlyingQSettings() and does never write back
 * to @ref underlyingQSettings(). */
template<typename T>
void Setting<T>::updateFromQSettings()
{
    // WARNING: Do not use the setter, as this may trigger
    // unnecessary file writes even if the property hasn't changed. If
    // another instance tries to write to the same file at the same time,
    // it could cause a deadlock since our code would perform two file
    // access operations. Another process could potentially lock the file
    // just in between the two writes, leading to a deadlock. To prevent
    // such issues, our code only reads from QSettings and never writes
    // back directly or indirectly. Instead, we modify the property's
    // internal storage directly and emit the notify signal if necessary.

    const QVariant newValueVariant = underlyingQSettings()->value(m_key);
    const T newValue = newValueVariant.value<T>();
    if (newValue != m_value) {
        m_value = newValue;
        Q_EMIT valueChanged();
    }
}

/** @brief Getter.
 *  @returns the value. */
template<typename T>
T Setting<T>::value() const
{
    return m_value;
}

/** @brief Setter.
 *
 * @param newValue The new value. */
template<typename T>
void Setting<T>::setValue(const T &newValue)
{
    if (newValue != m_value) {
        m_value = newValue;
        const auto newVariant = QVariant::fromValue<T>(m_value);
        underlyingQSettings()->setValue(m_key, newVariant);
        Q_EMIT valueChanged();
    }
}

} // namespace PerceptualColor

#endif // SETTING_H
