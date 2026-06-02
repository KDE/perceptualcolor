// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_INITIALIZETRANSLATION_H
#define PERCEPTUALCOLOR_INITIALIZETRANSLATION_H

#include <optional>

#include <qcontainerfwd.h>

class QCoreApplication;

/** @internal @file
 *
 * Provides the @ref PerceptualColor::initializeTranslation() function.
 */

namespace PerceptualColor
{

void initializeTranslation(QCoreApplication *instance, std::optional<QStringList> newUiLanguages);

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_INITIALIZETRANSLATION_H
