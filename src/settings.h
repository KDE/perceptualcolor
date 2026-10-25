// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_SETTINGS_H
#define PERCEPTUALCOLOR_SETTINGS_H

#include "internalimportexport.h"
#include <qfilesystemwatcher.h>
#include <qobject.h>
#include <qsettings.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Represents the settings file and allows for inter-process
 * communication of changes.
 *
 * This object provides persistent, platform-independent settings and writes
 * changes to the settings file relatively quickly. Changes made to the
 * settings file by other processes are read in immediately, and the
 * corresponding NOTIFY signal is emitted for changed properties.
 *
 * Usage: The functionality is based on a tight collaboration between
 * \l Settings, \l Setting and \l SettingBase. To use it, subclass
 * \l Settings and add public data members of type \l Setting for each
 * setting you want to use. It might be useful to implement the subclass as
 * a singleton. Example:
 *
 * Header:
 *
 * \quotefile perceptualsettings.h
 *
 * Source:
 *
 * \quotefile perceptualsettings.cpp
 *
 * \warning This object is not thread-safe.
 * It must only be used in the main (widget) thread!
 *
 * \note For more sophisticated use cases requiring
 * type-safe access to \c {QSettings}, there are
 * \l {https://www.vikingsoftware.com/blog/more-type-safety-with-qsettings/}
 * {alternative approaches available}, but they may be overkill for our
 * limited requirements here. */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT Settings : public QObject
{
    Q_OBJECT

public:
    Settings(QSettings::Scope scope, const QString &organization, const QString &application);
    virtual ~Settings() override;
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    Settings(const Settings &) = delete;
    Settings &operator=(const Settings &) = delete;

Q_SIGNALS:

    /*!
     * \brief Emitted after the underlying file has changed and synchronized.
     *
     * This signal is emitted when the underlying file has changed on disk,
     * forcing the internal \c QSettings object to synchronize.
     *
     * The file may have been modified by the same process or by another
     * process.
     *
     * Note that the setting values within the internal \c QSettings object
     * may or may not have changed as a result.
     */
    void updatedAfterFileChange();

private:
    /*!
     *\brief The internal QSettings object.
     */
    QSettings m_qSettings;

    /*!
     * \brief A watcher for the file used by \l m_qSettings.
     *
     * This allows to react immediately to settings changes done by other
     * applications using this library. This is also useful as simple but
     * cross-platform inter-process communication for synchronizing for
     * example custom colors between various simultaneously running
     * applications using this library. */
    QFileSystemWatcher m_watcher;

    void updateFromFile();

    friend class TestSettings;

    friend class SettingBase;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_SETTINGS_H
