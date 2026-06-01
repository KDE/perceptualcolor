// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_LOGGING_H
#define PERCEPTUALCOLOR_LOGGING_H

#include <qloggingcategory.h>

/**
 * @internal
 *
 * @file
 *
 * Provides the logging category.
 */

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief The <a href="https://doc.qt.io/qt-6/qloggingcategory.html">logging
 * category</a> of this library.
 *
 * As required by KDE
 * <a href="# https://community.kde.org/Frameworks/Frameworks_Logging_Policy">
 * Frameworks Logging Policy</a>.
 *
 * @returns The <a href="https://doc.qt.io/qt-6/qloggingcategory.html">logging
 * category</a> of this library.
 */
Q_DECLARE_LOGGING_CATEGORY(logging)

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_LOGGING_H
