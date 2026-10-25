// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_SETTRANSLATION_H
#define PERCEPTUALCOLOR_SETTRANSLATION_H

#include "importexport.h"
#include <qcontainerfwd.h>

class QCoreApplication;

/*!
 * \headerfile settranslation.h
 *
 * \inmodule ModulePublicApi
 */

namespace PerceptualColor
{

PERCEPTUALCOLOR_IMPORTEXPORT void setTranslation(QCoreApplication *instance, const QStringList &newUiLanguages);

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_SETTRANSLATION_H
