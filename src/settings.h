// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SETTINGS_H
#define SETTINGS_H

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
 * @brief Represents the settings file and allows for inter-process
 * communication of changes.
 *
 * This object provides persistent, platform-independent settings and writes
 * changes to the settings file relatively quickly. Changes made to the
 * settings file by other processes are read in immediately, and the
 * corresponding NOTIFY signal is emitted for changed properties.
 *
 * Usage: The functionality is based on a tight collaboration between
 * @ref Settings, @ref Setting and @ref SettingBase. To use it, subclass
 * @ref Settings and add public data members of type @ref Setting for each
 * setting you want to use. It might be useful to implement the subclass as
 * a singleton. Example:
 *
 * Header:
 * @include src/perceptualsettings.h
 *
 * Source:
 * @include src/perceptualsettings.cpp
 *
 * @warning This object is not thread-safe.
 * It must only be used in the main (widget) thread!
 *
 * @note For more sophisticated use cases requiring
 * type-safe access to <tt>QSettings</tt>, there are
 * <a href="https://www.vikingsoftware.com/blog/more-type-safety-with-qsettings/">
 * alternative approaches available</a>, but they may be overkill for our
 * limited requirements here. */
class Settings : public QObject
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
    /** @brief The underlying file has changed.
     *
     * Notify that underlying file has changed and that @ref m_qSettings has
     * been forced to synchronize with the underlying file again.
     *
     * The setting values in @ref m_qSettings might or might not have changed.
     * This underlying file might have been changed by <em>this</em> process
     * or by <em>another</em> process. */
    void updatedAfterFileChange();

private:
    /** @brief The internal QSettings object. */
    QSettings m_qSettings;
    /** @brief A watcher for the file used by @ref m_qSettings.
     *
     * This allows to react immediately to settings changes done by other
     * applications using this library. This is also useful as simple but
     * cross-platform inter-process communication for synchronizing for
     * example custom colors between various simultaneously running
     * applications using this library. */
    QFileSystemWatcher m_watcher;

    void updateFromFile();

    /** @internal @brief Only for unit tests. */
    friend class TestSettings;

    /** @internal @brief @ref Settings, @ref SettingBase and @ref Setting have
     * tight collaboration. */
    friend class SettingBase;
};

} // namespace PerceptualColor

#endif // SETTINGS_H
