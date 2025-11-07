// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SETTING_H
#define SETTING_H

#include "settingbase.h"
#include "settings.h"
#include <qbytearray.h>
#include <qglobal.h>
#include <qmetaobject.h>
#include <qmetatype.h>
#include <qstring.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

class QObject;

namespace PerceptualColor
{

/** @internal
 *
 * @brief A single setting within @ref Settings.
 *
 * @tparam T Type of the setting value. The type must qualify for registration
 *           as <tt>QMetaType</tt> and also provide a stream operator.
 *           @ref PerceptualSettings::ColorList and many build-in types
 *           qualify. However, enum types only qualify if both, they are
 *           declared with <tt>Q_ENUM</tt> <em>and</em> their underlying type
 *           is <tt>int</tt>. */
template<typename T>
class Setting : public SettingBase
{
public:
    Setting(const QString &key, Settings *settings, QObject *parent = nullptr);
    virtual ~Setting() override;

    // cppcheck-suppress returnByReference // false positive
    T value() const;

    void setValue(const T &newValue);

private:
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    Setting(const Setting &) = delete;
    Setting &operator=(const Setting &) = delete;

    /** @brief If the type is an enum type or not. */
    static constexpr bool m_isEnum = std::is_enum_v<T>;

    /** @brief Meta data for enum types. */
    QMetaEnum m_qMetaEnum;

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
 *   (Also, some characters like the slash and backslash are not allowed on
 *   many platforms.)
 * - Key strings must follow the format “groupname/keyname”. Each keyname
 *   should belong to a group, separated by a slash. You can
 *   use nested groups like “groupname/subgroupname/keyname”. Use the
 *   name of the class associated with the setting as the group name. This
 *   convention improves readability of the settings file: Omitting a group
 *   causes the system to assign the key to a default “General” group,
 *   which can be unexpected and confusing. Using class names as group
 *   names clarifies the origin and purpose of each setting.
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
    if constexpr (m_isEnum) {
        static_assert( //
            sizeof(T) <= sizeof(int), //
            // Reason: We do conversions using QMetaEnum, which uses “int”
            // up to Qt 6.8. (Starting with Qt 6.9, quint64 is also available.)
            "Class template 'Setting': If 'typename T' is an enum, its "
            "underlying type must not exceed the size of 'int'."
            // signed/unsigned does not matter for QMetaEnum!
        );
    }

    // QSettings seems to use indirectly QMetaType::load() which requires
    // to register all custom types as QMetaType.
    qRegisterMetaType<T>();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // Also stream operators are required.
    qRegisterMetaTypeStreamOperators<T>();
#endif

    if constexpr (m_isEnum) {
        m_qMetaEnum = QMetaEnum::fromType<T>();
    }

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

    // Get new value.
    const QVariant newValueVariant = underlyingQSettings()->value(m_key);
    T newValue;
    if constexpr (m_isEnum) {
        const QByteArray byteArray = newValueVariant.toString().toUtf8();
        const int enumInteger = //
            byteArray.isEmpty() //
            ? 0 //
            : m_qMetaEnum.keysToValue(byteArray.constData());
        newValue = static_cast<T>(enumInteger);
    } else {
        newValue = newValueVariant.value<T>();
    }

    // Apply new value.
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
        if constexpr (m_isEnum) {
            const auto newValueAsIntegral = //
#if QT_VERSION >= QT_VERSION_CHECK(6, 9, 0)
                static_cast<quint64>(newValue);
#else
                static_cast<int>(newValue);
#endif
            const QString string = QString::fromUtf8( //
                m_qMetaEnum.valueToKeys(newValueAsIntegral));
            underlyingQSettings()->setValue(m_key, string);
        } else {
            const auto newVariant = QVariant::fromValue<T>(m_value);
            underlyingQSettings()->setValue(m_key, newVariant);
        }
        Q_EMIT valueChanged();
    }
}

} // namespace PerceptualColor

#endif // SETTING_H
