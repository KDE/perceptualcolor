// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "perceptualsettings.h"

#include <qcoreapplication.h>
#include <qsettings.h>
#include <qstringliteral.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#else
#include <qdatastream.h>
#endif

namespace PerceptualColor
{

/** @brief Private constructor to prevent instantiation. */
PerceptualSettings::PerceptualSettings()
    : Settings(QSettings::UserScope, QStringLiteral("kde.org"), QStringLiteral("libperceptualcolor"))
    // For maximum portability:
    // - No upper case should ever be used.
    //   (Some systems, like the INI that we are using, are case-insensitive.
    //   And even if we always use INI, having both capital and small letters
    //   is error-prone because typos are not checked by the compiler.)
    // - Only the letters a-z should be used.
    //   (Also, some characters like the backslash are not allowed on some
    //   platforms.)
    // - “group/key”: Each key has exactly one group. Don't use subgroups.
    //   Use the class name as group name.
    //   (This makes the settings file well readable for humans. Missing
    //   groups are confusing because the system generates a “General”
    //   group which is not easy to understand. And using class identifiers
    //   helps to understand the structure of the settings file.)
    // - In C++, use “const” variables to define key strings, instead of
    //   manually typing the key strings.
    //   (This avoids typing errors.)
    , customColors(QStringLiteral("colordialog/customcolors"), this)
    , lastColor(QStringLiteral("colordialog/lastcolor"), this)
    , history(QStringLiteral("colordialog/history"), this)
    , swatchBookPage(QStringLiteral("colordialog/swatchbookpage"), this)
    , tab(QStringLiteral("colordialog/tab"), this)
    , tabExpanded(QStringLiteral("colordialog/tabexpanded"), this)
{
}

/** @brief Destructor. */
PerceptualSettings::~PerceptualSettings()
{
}

/** @brief Get a reference to the singleton instance.
 *
 * @pre There exists a QCoreApplication object. (Otherwise, this
 * function will throw an exception.)
 *
 * @returns A reference to the instance.
 *
 * To use it, assign the return value to a reference (not a normal variable):
 *
 * @snippet testperceptualsettings.cpp PerceptualSettings Instance */
PerceptualSettings &PerceptualSettings::instance()
{
    if (QCoreApplication::instance() == nullptr) {
        // A QCoreApplication object is required because otherwise
        // the QFileSystemWatcher will not do anything and print the
        // highly confusing warning “QSocketNotifier: Can only be used
        // with threads started with QThread”. It's better to give clear
        // feedback:
        throw 0;
    }
    static PerceptualSettings myInstance;
    return myInstance;
}

} // namespace PerceptualColor
