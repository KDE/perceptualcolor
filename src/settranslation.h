// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SETTRANSLATION_H
#define SETTRANSLATION_H

#include "importexport.h"
#include <qcontainerfwd.h>

class QCoreApplication;

namespace PerceptualColor
{

void PERCEPTUALCOLOR_IMPORTEXPORT setTranslation(QCoreApplication *instance, const QStringList &newUiLanguages);

} // namespace PerceptualColor

#endif // SETTRANSLATION_H
