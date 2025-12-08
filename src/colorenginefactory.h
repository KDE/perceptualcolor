// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORENGINEFACTORY_H
#define PERCEPTUALCOLOR_COLORENGINEFACTORY_H

#include "importexport.h"
#include <qcontainerfwd.h>
#include <qsharedpointer.h>
#include <qstring.h>

/** @file
 *
 * Factory for color engine objects.
 *
 * These non-disclosed color engine objects are needed in the constructors
 * of various classes of this library.
 *
 * Creating color engine objects can be slow. But once created, they can be
 * used simultaneously on various widgets. Thanks to the QSharedPointer, you
 * can easily create a color engine object, pass it to the widget constructors
 * you like, and then forget about it – it will be deleted automatically when
 * the last widget that used it has been deleted. And passing the shared
 * pointer to widget constructors is fast! Usage example:
 *
 * @snippet testcolorenginefactory.cpp Create */

namespace PerceptualColor
{
class ColorEngine;

[[nodiscard]] PERCEPTUALCOLOR_IMPORTEXPORT QSharedPointer<PerceptualColor::ColorEngine> createSrgbColorEngine();
[[nodiscard]] PERCEPTUALCOLOR_IMPORTEXPORT QSharedPointer<PerceptualColor::ColorEngine> tryCreateColorEngineFromFile(const QString &fileName);
[[nodiscard]] PERCEPTUALCOLOR_IMPORTEXPORT QStringList colorProfileDirectories();

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORENGINEFACTORY_H
