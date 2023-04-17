// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "settings.h"

#include <qcoreapplication.h>
#include <qfilesystemwatcher.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qlist.h>
#else
#include <qdatastream.h>
#include <qstringlist.h>
#endif

namespace PerceptualColor
{

/** @brief Private constructor to prevent instantiation. */
Settings::Settings()
{
    // QSettings seems to use indirectly QMetaType::load() which requires
    // to register all custom types as QMetaType.
    qRegisterMetaType<ColorList>();
#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
    // Also stream operators are required. However, as long as ColorList is
    // a QList<QColor> we do not need to actually provide an implementation.
    // This might change if we change from QColor to another data type.
    qRegisterMetaTypeStreamOperators<ColorList>();
#endif

    m_watcher.addPath(m_qSettings.fileName());

    connect(&m_watcher, //
            &QFileSystemWatcher::fileChanged, //
            this, //
            &PerceptualColor::Settings::updateFromFile //
    );

    // Initialize the properties with the values from the settings file.
    updateFromFile();
}

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

/** @brief Get a reference to the singleton instance.
 *
 * @pre There exists a QCoreApplication object. (Otherwise, this
 * function will throw an exception.)
 *
 * @returns A reference to the instance.
 *
 * To use it, assign the return value to a reference (not a normal variable):
 *
 * @snippet testsettings.cpp Settings Instance */
Settings &Settings::instance()
{
    if (QCoreApplication::instance() == nullptr) {
        // A QCoreApplication object is required because otherwise
        // the QFileSystemWatcher will not do anything and print the
        // highly confusing warning “QSocketNotifier: Can only be used
        // with threads started with QThread”. It's better to give clear
        // feedback:
        throw 0;
    }
    static Settings myInstance;
    return myInstance;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString Settings::tab() const
{
    return m_tab;
}

/** @brief Setter for @ref tab property.
 *
 * @param newTab the new property value */
void Settings::setTab(const QString &newTab)
{
    if (newTab != m_tab) {
        m_tab = newTab;
        const auto newVariant = QVariant::fromValue<QString>(m_tab);
        m_qSettings.setValue(keyTab, newVariant);
        Q_EMIT tabChanged(m_tab);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString Settings::tabExpanded() const
{
    return m_tabExpanded;
}

/** @brief Setter for @ref tabExpanded property.
 *
 * @param newTab the new property value */
void Settings::setTabExpanded(const QString &newTab)
{
    if (newTab != m_tabExpanded) {
        m_tabExpanded = newTab;
        const auto newVariant = QVariant::fromValue<QString>(m_tabExpanded);
        m_qSettings.setValue(keyTabExpanded, newVariant);
        Q_EMIT tabExpandedChanged(m_tabExpanded);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
Settings::ColorList Settings::customColors() const
{
    return m_customColors;
}

/** @brief Setter for @ref customColors property.
 *
 * @param newCustomColors the new property value */
void Settings::setCustomColors(const ColorList &newCustomColors)
{
    if (newCustomColors != m_customColors) {
        m_customColors = newCustomColors;
        const auto newVariant = QVariant::fromValue<ColorList>(m_customColors);
        m_qSettings.setValue(keyCustomColors, newVariant);
        Q_EMIT customColorsChanged(m_customColors);
    }
}

/** @brief Updates all properties to the corresponding values in the
 * settings file. */
void Settings::updateFromFile()
{
    // From Qt documentation:
    // “Note: As a safety measure, many applications save
    //  an open file by writing a new file and then deleting
    //  the old one. In your slot function, you can check
    //  watcher.files().contains(path). If it returns false,
    //  check whether the file still exists and then call
    //  addPath() to continue watching it.”
    if (!m_watcher.files().contains(m_qSettings.fileName())) {
        m_watcher.addPath(m_qSettings.fileName());
    }

    m_qSettings.sync();

    // WARNING: Do not use setters for properties, as this may trigger
    // unnecessary file writes even if the property hasn't changed. If
    // another instance tries to write to the same file at the same time,
    // it could cause a deadlock since our code would perform two file
    // access operations. Another process could potentially lock the file
    // just in between the two writes, leading to a deadlock. To prevent
    // such issues, our code only reads from QSettings and never writes
    // back directly or indirectly. Instead, we modify the property's
    // internal storage directly and emit the notify signal if necessary.

    const auto newCustomColors = m_qSettings.value(keyCustomColors).value<ColorList>();
    if (newCustomColors != m_customColors) {
        m_customColors = newCustomColors;
        Q_EMIT customColorsChanged(newCustomColors);
    }

    const auto newTab = m_qSettings.value(keyTab).toString();
    if (newTab != m_tab) {
        m_tab = newTab;
        Q_EMIT tabChanged(newTab);
    }
}

} // namespace PerceptualColor
