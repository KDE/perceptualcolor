// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "perceptualsettings.h"

#include <qcoreapplication.h>
#include <qdebug.h>
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
    //   (Also, some characters like the slash and backslash are not allowed on
    //   many platforms.)
    // - Key strings must follow the format “groupname/keyname”. Each keyname
    //   should belong to a group, separated by a slash. You can
    //   use nested groups like “groupname/subgroupname/keyname”. Use the
    //   name of the class associated with the setting as the group name. This
    //   convention improves readability of the settings file: Omitting a group
    //   causes the system to assign the key to a default “General” group,
    //   which can be unexpected and confusing. Using class names as group
    //   names clarifies the origin and purpose of each setting.
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
PerceptualSettings &PerceptualSettings::getInstance()
{
    // Pattern: Meyer's singleton.
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
