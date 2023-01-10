// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "rgbcolorspacefactory.h"

#include "rgbcolorspace.h"
#include <qdir.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qlist.h>
#include <qstring.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>

namespace PerceptualColor
{
/** @brief Create an sRGB color space object.
 *
 * This is build-in, no external ICC file is used.
 *
 * @pre This function is called from the main thread.
 *
 * @returns A shared pointer to the newly created color space object.
 *
 * @internal
 *
 * @todo This should be implemented as singleton with on-demand
 * initialization. This requires however changes to @ref RgbColorSpace
 * which should <em>not</em> guarantee that properties like
 * @ref RgbColorSpace::profileName() are constant. Instead,
 * for the sRGB profiles, the translation should be dynamic. */
QSharedPointer<PerceptualColor::RgbColorSpace> RgbColorSpaceFactory::createSrgb()
{
    return RgbColorSpace::createSrgb();
}

/** @brief Try to create a color space object for a given ICC file.
 *
 * @note This function may fail to create the color space object when it
 * cannot open the given file, or when the file cannot be interpreted.
 *
 * @pre This function is called from the main thread.
 *
 * @param fileName The file name. See <tt>QFile</tt> documentation for what
 * are valid file names. The file is only used during the execution of this
 * function and it is closed again at the end of this function. The created
 * object does not need the file anymore, because all necessary information
 * has already been loaded into memory. Accepted are RGB-based ICC profiles
 * up to version 4.
 *
 * @returns A shared pointer to a newly created color space object on success.
 * A shared pointer to <tt>nullptr</tt> on fail. */
QSharedPointer<PerceptualColor::RgbColorSpace> RgbColorSpaceFactory::createFromFile(const QString &fileName)
{
    return RgbColorSpace::createFromFile(fileName);
}

/** @brief List of directories where color profiles are typically
 * stored on the current system.
 *
 * Often, but not always, operating systems have an API to
 * get access to this directories with color profiles or
 * to get the actual color profile of a specific device
 * (screen, printer…). On Linux, this is typically provided by
 * <a href="https://www.freedesktop.org/software/colord/index.html">colord</a>.
 * Also on Windows, there are specific API calls
 * (<a href="https://learn.microsoft.com/en-us/windows/win32/wcs/profile-management-functions">[1]</a>
 * <a href="https://learn.microsoft.com/en-us/windows/win32/api/icm/nf-icm-wcsgetdefaultcolorprofile">[1]</a>
 * <a href="https://learn.microsoft.com/en-us/windows/win32/api/icm/nf-icm-getcolordirectoryw">[1]</a>
 * <a href="https://learn.microsoft.com/en-us/uwp/api/windows.graphics.display.displayinformation.getcolorprofileasync?view=winrt-22621">[1]</a>)
 * Some other operating systems have similar APIs.
 *
 * The best solution is to rely on the operating system’s API. However,
 * if you can’t use this API for some reasons, this function provides a
 * last-resort alternative. Not all operating systems have standardised
 * directories for colour profiles. This function provide a list of typical
 * storage locations of ICC profile files and works satisfactorily for at
 * least Linux, BSD, MacOS and Windows.
 *
 * @returns A preference-ordered list of typical storage locations of
 * color profiles on the current system. The list might  be empty if no
 * color profile directories are found on the system. The returned directories
 * use '/' as file separator regardless of the operating system, just
 * as <tt>QFile</tt> expects. To find color profiles, parse these directories
 * recursively, including subdirectories. Note that ICC colour profiles
 * traditionally have a file name ending in <tt>.icm</tt> on Windows systems
 * and a name ending in <tt>.icc</tt> on all other operating systems,
 * but today on every operating system you might find actually both file name
 * endings.
 *
 * @note This function takes into account environment variables, home
 * directories and so on. Potential colour profile directories that do not
 * actually exist on the current system are not returned. Since these values
 * could change, another call of this function could return a different result.
 *
 * @internal
 *
 * @note Internal implementation details: User directories appear at the top
 * of the list, system-wide directories appear at the bottom. The returned
 * directories are absolute paths with all symlinks removed. There are no
 * duplicates in the list. All returned directories actually exist. */
QStringList RgbColorSpaceFactory::colorProfileDirectories()
{
    // https://web.archive.org/web/20140625123925/http://nadeausoftware.com/articles/2012/01/c_c_tip_how_use_compiler_predefined_macros_detect_operating_system
    // describes well how to recognize the current system by compiler-defined
    // macros. But Qt makes things more comfortable for us…
    //
    // Q_OS_WIN is defined on all Windows-like systems.
    //
    // Q_OS_UNIX is defined on all other systems.
    //
    // Q_OS_DARWIN is defined on MacOS-like systems along with Q_OS_UNIX.
    //
    // Q_OS_UNIX and Q_OS_WIN are mutually exclusive: They never appear at
    // the same time, not even on Cygwin. Reference: Q_OS_UNIX definition at
    // https://code.woboq.org/qt6/qtbase/src/corelib/global/qsystemdetection.h.html

    QStringList candidates;

#ifdef Q_OS_DARWIN
    // MacOS-like systems (including iOS and other derivatives)
    // The Qt version we are relying on requires at least MacOS X. Starting
    // with MacOS X, those are the relevant dictionaries, as also
    // https://stackoverflow.com/a/32729370 describes:

    // User-supplied settings:
    candidates.append( //
        QDir::homePath() + QStringLiteral(u"/Library/ColorSync/Profiles/"));

    // Settings supplied by the local machine:
    candidates.append(QStringLiteral(u"/Library/ColorSync/Profiles/"));

    // Settings supplied by the network administrator:
    candidates.append(QStringLiteral(u"/Network/Library/ColorSync/Profiles/"));

    // Hard-coded settings of MacOS itself, that cannot be changed:
    candidates.append(QStringLiteral(u"/System/Library/ColorSync/Profiles/"));

    // Printer drivers also might have color profiles:
    candidates.append(QStringLiteral(u"/Library/Printers/")); // TODO Useful?

    // Adobe’s applications also might have color profiles:
    candidates.append( // TODO Is it useful to support particular programs?
        QStringLiteral(u"/Library/Application Support/Adobe/Color/Profiles/"));

#elif defined(Q_OS_UNIX)
    // Unix-like systems (including BSD, Linux, Android), excluding those which
    // have Q_OS_DARWIN defined (we are after this in the #elif statement).
    // The following settings will work probably well on Linux and BSD,
    // but not so well on Android which does not seem to have a real standard.

    const QString subdirectory1 = QStringLiteral(u"/color/icc/");
    const QString subdirectory2 = QStringLiteral(u"/icc/");
    QString baseDirectory;
    baseDirectory = qEnvironmentVariable("XDG_DATA_HOME");
    if (!baseDirectory.isEmpty()) {
        candidates.append(baseDirectory + subdirectory1);
        candidates.append(baseDirectory + subdirectory2);
    }
    baseDirectory = QDir::homePath() + QStringLiteral(u"/.local/share/");
    candidates.append(baseDirectory + subdirectory1);
    candidates.append(baseDirectory + subdirectory2);
    baseDirectory = QDir::homePath();
    candidates.append(baseDirectory + subdirectory1);
    candidates.append(baseDirectory + subdirectory2);
    candidates.append(baseDirectory + QStringLiteral(u"/.color/icc/"));

    QStringList baseDirectoryList = //
        qEnvironmentVariable("XDG_DATA_DIRS").split(QStringLiteral(u":"));
    // Fallback values for empty XDG_DATA_DIRS, as defined in
    // the Free Desktop Specification:
    baseDirectoryList.append(QStringLiteral(u"/usr/local/share/"));
    baseDirectoryList.append(QStringLiteral(u"/usr/share/"));
    // Custom search directory:
    baseDirectoryList.append(QStringLiteral(u"/var/lib/"));
    for (const QString &path : std::as_const(baseDirectoryList)) {
        if (!path.isEmpty()) {
            candidates.append(path + QStringLiteral(u"/color/icc/"));
            candidates.append(path + QStringLiteral(u"/icc/"));
        }
    }

#elif defined(Q_OS_WIN)
    // Windows-like systems

    // NOTE It is possible to get the Windows system directory with
    // Windows API calls. However, we want to reduce our dependencies and
    // therefore avoid to link against this API. So the following code
    // is commented out.
    //
    // // If an array is initialized and the array length is larger than
    // // the number of initialization values, the remaining array values
    // // will be initialized to zero. Therefore, the following code
    // initializes the entire (!) array to 0.
    // wchar_t sysDir[MAX_PATH + 1] = {0};
    // GetSystemDirectoryW(sysDir, MAX_PATH);
    // QString winSysDir = QString::fromWCharArray(sysDir);

    QString winSysDir = qEnvironmentVariable("windir");

    // Starting with XP, this is the default directory:
    candidates.append(winSysDir + QStringLiteral(u"/Spool/Drivers/Color/"));
    // In Windows 95, 98, this was the default directory:
    candidates.append(winSysDir + QStringLiteral(u"/Color/")); // TODO Useful?
#endif

    // Prepare the return value:
    QFileInfo info; // QFileInfo isn’t only about files, but also directories!
    QStringList result;
    // cppcheck-suppress knownEmptyContainer // false positive
    for (const QString &path : std::as_const(candidates)) {
        // cleanPath() has redundant separators removed.
        info = QFileInfo(QDir::cleanPath(path));
        if (info.isDir()) {
            // canonicalFilePath() returns an absolute path without redundant
            // '.' or '.. ' elements (or an empty string if this is
            // not possible):
            result.append(info.canonicalFilePath());
        }
    }
    result.removeDuplicates();
    result.removeAll(QString()); // Remove empty strings
    return result;
}

} // namespace PerceptualColor
