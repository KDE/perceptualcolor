// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef VERSION_H
#define VERSION_H

#include "importexport.h"

#include <qglobal.h>
#include <qversionnumber.h>

/** @file
 *
 * This file provides version information for this library at compile
 * time and at run time.
 *
 * @note Do not include this file itself! Instead, include <tt>version.h</tt>.
 *
 * @internal
 *
 * @note This file is configuration input: CMake processes it,
 * puts the actual version numbers (<tt>\@…\@</tt>) and saves the result
 * as <tt>version.h</tt>.
 *
 * @note This file has the extension .hpp because we want a different
 * extension for configuration input than for headers. This allows to
 * <em>not</em> process this file by clang-format. (clang-format does not
 * understand the CMake configuration syntax with <tt>\@…\@</tt> and would
 * break it.) */

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR
 *
 * @returns The <em>major</em> version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR (@MAJOR_VERSION@)

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR
 *
 * @returns The <em>minor</em> version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR (@MINOR_VERSION@)

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH
 *
 * @returns The <em>patch</em> version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH (@PATCH_VERSION@)

/** @def PERCEPTUALCOLOR_COMPILE_TIME_VERSION
 *
 * @brief Against which version of this library you are <em>compiling</em>.
 *
 * This macro has the same semantic as <tt>QT_VERSION</tt>
 *
 * @returns This macro expands a numeric value of the form 0xMMNNPP
 * (MM = major, NN = minor, PP = patch) that specifies the version number.
 * Note that there is no pre-release identifier included. For example, if
 * you compile your application against version 1.2.3-alpha of this library,
 * the macro will expand to 0x010203. You can use this macro to use the
 * latest features where available. Example:
 *
 * @snippet testversion.cpp Version Macro usage
 *
 * To use this macro, include <tt>version.h</tt>
 *
 * @sa @ref versioninfo */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION (QT_VERSION_CHECK(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH))

namespace PerceptualColor
{
[[nodiscard]] PERCEPTUALCOLOR_IMPORTEXPORT QVersionNumber perceptualColorRunTimeVersion();

} // namespace PerceptualColor

#endif // VERSION_H
