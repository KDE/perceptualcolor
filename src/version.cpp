// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "version.h"

#include <limits>

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR >= 0);

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR >= 0);

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH >= 0);

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION >= 1);

namespace PerceptualColor
{
/** @brief Against which version of this library you are <em>running</em>.
 *
 * @returns The library version with major, minor and patch version.
 * Note that there is no pre-release identifier included. For example, if
 * your application is running against version 1.2.3-alpha of this library,
 * this function will return <tt>QVersionNumber(1, 2, 3)</tt>.
 *
 * To use this function, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
QVersionNumber perceptualColorRunTimeVersion()
{
    return QVersionNumber(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH);
}

} // namespace PerceptualColor
