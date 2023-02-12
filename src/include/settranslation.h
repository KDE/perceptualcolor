// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SETTRANSLATION_H
#define SETTRANSLATION_H

#include "importexport.h"
#include <qglobal.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#else
#include <qstringlist.h>
#endif

class QCoreApplication;

/** @file
 *
 * Provides the @ref PerceptualColor::setTranslation() function. */

namespace PerceptualColor
{

void PERCEPTUALCOLOR_IMPORTEXPORT setTranslation(QCoreApplication *instance, const QStringList &newUiLanguages);

} // namespace PerceptualColor

#endif // SETTRANSLATION_H
