// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_SETTING_H
#define PERCEPTUALCOLOR_SETTING_H

#include "settingbase.h"
#include "settings.h"
#include <qbytearray.h>
#include <qglobal.h>
#include <qmetaobject.h>
#include <qmetatype.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <qvariant.h>

class QObject;

namespace PerceptualColor
{

/*!
 *
 *
 * \class PerceptualColor::Setting
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A single setting within \l Settings.
 *
 * \b T Type of the setting value. The type must qualify for registration
 *           as \c {QMetaType} and also provide a stream operator.
 *           \l PerceptualSettings::ColorList and many build-in types
 *           qualify. However, enum types only qualify if both, they are
 *           declared with \c {Q_ENUM} \e {and} their underlying type
 *           is \c {int}. */
template<typename T>
class Setting : public SettingBase
{
public:
    /*!
     * \brief Constructor.
     *
     * \a key \c {QSettings} key for the value.
     *
     * For maximum portability, choose your \key carefully:
     *
     * \list
     * \li No upper case should ever be used.
     *   (Some systems, like the INI that we are using, are case-insensitive.
     *   And even if we always use INI, having both capital and small letters
     *   is error-prone because typos are not checked by the compiler.)
     * \li Only the letters a-z should be used.
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
     * \a settings Corresponding \l Settings object. This object must
     *        stay available during the live-time of this object.
     *
     * \a parent The parent object (if any).
     *
     * \warning You must not create more than one instance with the same
     * combination between key and \l Settings object. This would result
     * in undefined behaviour. (Probably some values would be out-of-sync.) */
    Setting(const QString &key, Settings *settings, QObject *parent = nullptr);

    /*!
     * \brief Destructor.
     */
    virtual ~Setting() override;

    /*!
     * \brief Getter.
     *
     *  Returns the value.
     */
    [[nodiscard]] T value() const;

    /*!
     * \brief Setter.
     *
     * \a newValue The new value.
     */
    void setValue(const T &newValue);

private:
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    Setting(const Setting &) = delete;

    Setting &operator=(const Setting &) = delete;

    /*!
     * \brief If the type is an enum type or not. */
    static constexpr bool m_isEnum = std::is_enum_v<T>;

    /*!
     * \brief Meta data for enum types. */
    QMetaEnum m_qMetaEnum;

    /*!
     * \brief Internal storage for the value. */
    T m_value = T();

    /*!
     * \brief Updates the value to the corresponding value
     * from \l underlyingQSettings().
     *
     * Only reads from \l underlyingQSettings() and does never write back
     * to \l underlyingQSettings().
     */
    void updateFromQSettings();

    friend class TestSetting;
};

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

template<typename T>
Setting<T>::~Setting()
{
}

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

template<typename T>
T Setting<T>::value() const
{
    return m_value;
}

template<typename T>
void Setting<T>::setValue(const T &newValue)
{
    if (newValue != m_value) {
        m_value = newValue;
        if constexpr (m_isEnum) {
            const auto newValueAsIntegral = static_cast<quint64>(newValue);
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

#endif // PERCEPTUALCOLOR_SETTING_H
