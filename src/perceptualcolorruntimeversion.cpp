// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "perceptualcolorruntimeversion.h"

#include "version.h"

namespace PerceptualColor
{
/*!
 * \relates perceptualcolorruntimeversion.h
 *
 * \ingroup GroupPublicSymbols
 *
 * \brief Against which version of this library you are \e {running}.
 *
 * Returns the library version with major, minor and patch version.
 * Note that there is no pre-release identifier included. For example, if
 * your application is running against version 1.2.3-alpha of this library,
 * this function will return \c {QVersionNumber(1, 2, 3)}.
 */
QVersionNumber perceptualColorRunTimeVersion()
{
    return QVersionNumber(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH);
}

} // namespace PerceptualColor
