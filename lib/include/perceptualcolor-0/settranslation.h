// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef SETTRANSLATION_H
#define SETTRANSLATION_H

#include "perceptualcolor-0/importexport.h"

#include <qstringlist.h>

class QCoreApplication;

/** @file
 *
 * Provides the @ref PerceptualColor::setTranslation() function. */

namespace PerceptualColor
{

void PERCEPTUALCOLOR_IMPORTEXPORT setTranslation(QCoreApplication *instance, const QStringList &newUiLanguages);

} // namespace PerceptualColor

#endif // SETTRANSLATION_H
