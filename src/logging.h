// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_LOGGING_H
#define PERCEPTUALCOLOR_LOGGING_H

#include <qloggingcategory.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \fn const QLoggingCategory &logging()
 *
 * \brief The \l {https://doc.qt.io/qt-6/qloggingcategory.html} {logging
 * category} of this library.
 *
 * As required by KDE
 * \l {https://community.kde.org/Frameworks/Frameworks_Logging_Policy}
 * {Frameworks Logging Policy}.
 *
 * Returns the \l {https://doc.qt.io/qt-6/qloggingcategory.html} {logging
 * category} of this library.
 */
Q_DECLARE_LOGGING_CATEGORY(logging)

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_LOGGING_H
