// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "settings.h"

#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qfilesystemwatcher.h>
#include <qlist.h>

namespace PerceptualColor
{

/** @brief Constructor.
 *
 * @pre There exists a QCoreApplication object. (Otherwise, this
 * function will throw an exception.)
 *
 * @param scope Passed to the underlying <tt>QSettings</tt> object’s
 *        constructor.
 * @param organization Passed to the underlying <tt>QSettings</tt> object’s
 *        constructor.
 * @param application Passed to the underlying <tt>QSettings</tt> object’s
 *        constructor. Keep this string short and without special characters,
 *        as it might be used as file name for the configuration file, and
 *        the underlying file system might have restrictions.
 */
Settings::Settings(QSettings::Scope scope, const QString &organization, const QString &application)
    // There are important reasons to use <tt>QSettings::IniFormat</tt>.
    //
    // - It makes <tt>QSettings</tt> behave identical on all platforms.
    //   Though <tt>QSettings</tt> is an abstraction, it has still a lot
    //   of platform-dependant behaviour, like the fact the numbers are
    //   saved as numbers but read back as QString when using
    //   <tt>QSettings::IniFormat</tt> or when using the native format
    //   and the native platform uses Ini (like Linux); other platforms
    //   preserve the type information. By using <tt>QSettings::IniFormat</tt>
    //   the behaviour becomes at least predictable and is identical also
    //   cross-platform.
    //
    // - <tt>QSettings::IniFormat</tt> is a file-based approach (while the
    //   native approach for example on Windows is the Windows Registry
    //   instead of a file). Using a file is necessary to be able to monitor
    //   changes that other processes might make.
    : m_qSettings(QSettings::IniFormat, scope, organization, application)
{
    if (QCoreApplication::instance() == nullptr) {
        // A QCoreApplication object is required because otherwise
        // the QFileSystemWatcher will not do anything and print the
        // highly confusing warning “QSocketNotifier: Can only be used
        // with threads started with QThread”. It's better to give clear
        // feedback:
        throw 0;
    }

    m_watcher.addPath(m_qSettings.fileName());

    connect(&m_watcher, //
            &QFileSystemWatcher::fileChanged, //
            this, //
            &PerceptualColor::Settings::updateFromFile //
    );

    // Initialize
    updateFromFile();
}

/** @brief Destructor. */
Settings::~Settings()
{
}

/** @brief Updates all @ref Setting values to the corresponding values from
 * the underlying file of @ref m_qSettings.
 *
 * This is done by emitting the @ref updatedAfterFileChange() signal, to which
 * the @ref Setting objects are supposed to connect. */
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

    Q_EMIT updatedAfterFileChange();
}

} // namespace PerceptualColor
