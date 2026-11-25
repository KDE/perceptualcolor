// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_PERCEPTUALCOLORRUNTIMEVERSION_H
#define PERCEPTUALCOLOR_PERCEPTUALCOLORRUNTIMEVERSION_H

#include <qversionnumber.h>

#include "importexport.h"

/*!
 * \headerfile perceptualcolorruntimeversion.h
 *
 * \inmodule ModulePublicApi
 *
 * This library uses
 * \l {https://semver.org/} {Semantic Versioning 2.0.0}.
 */

namespace PerceptualColor
{
[[nodiscard]] PERCEPTUALCOLOR_IMPORTEXPORT QVersionNumber perceptualColorRunTimeVersion();

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_PERCEPTUALCOLORRUNTIMEVERSION_H
