// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_VERSION_H
#define PERCEPTUALCOLOR_VERSION_H

#include "importexport.h"

#include <qglobal.h>
#include <qversionnumber.h>

// This file provides version information for this library at compile
// time and at run time.
//
// Do not include this file itself! Instead, include version.h
//
// This file is configuration input: CMake processes it,
// puts the actual version numbers {\@…\@} and saves the result
// as version.h
//
// NOTE This file has the extension .hpp because we want a different
// extension for configuration input than for headers. This allows to
// not process this file by clang-format. (clang-format does not
// understand the CMake configuration syntax with {\@…\@} and would
// break it.)

// No need for PERCEPTUALCOLOR_IMPORTEXPORT because this header only
// contains macros, and no actual C++ symbols.

/*!
 * \headerfile version.h
 *
 * \inmodule ModulePublicApi
 *
 * This library uses
 * \l {https://semver.org/} {Semantic Versioning 2.0.0}.
 */

/*!
 * \macro PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR
 *
 * \relates version.h
 *
 * \ingroup GroupPublicSymbols
 *
 * \brief Against which version of this library you are \e {compiling}.
 *
 * To use this macro, include \c {version.h}
 *
 * Returns the \e {major} version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR (@MAJOR_VERSION@)

/*!
 * \macro PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR
 *
 * \relates version.h
 *
 * \ingroup GroupPublicSymbols
 *
 * \brief Against which version of this library you are \e {compiling}.
 *
 * To use this macro, include \c {version.h}
 *
 * Returns the \e {minor} version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR (@MINOR_VERSION@)

/*!
 * \macro PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH
 *
 * \relates version.h
 *
 * \ingroup GroupPublicSymbols
 *
 * \brief Against which version of this library you are \e {compiling}.
 *
 * To use this macro, include \c {version.h}
 *
 * Returns the \e {patch} version (as integer) of this library
 * against which you are compiling. You can use this macro to use the
 * latest features where available.
 */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH (@PATCH_VERSION@)

/*!
 * \macro PERCEPTUALCOLOR_COMPILE_TIME_VERSION
 *
 * \relates version.h
 *
 * \ingroup GroupPublicSymbols
 *
 * \brief Against which version of this library you are \e {compiling}.
 *
 * To use this macro, include \c {version.h}
 *
 * This macro has the same semantic as \c {QT_VERSION}
 *
 * This macro expands a numeric value of the form 0xMMNNPP
 * (MM = major, NN = minor, PP = patch) that specifies the version number.
 * Note that there is no pre-release identifier included. For example, if
 * you compile your application against version 1.2.3-alpha of this library,
 * the macro will expand to 0x010203. You can use this macro to use the
 * latest features where available. Example:
 *
 * \snippet testversion.cpp Version Macro usage
 */
#define PERCEPTUALCOLOR_COMPILE_TIME_VERSION (QT_VERSION_CHECK(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR, PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH))

#endif // PERCEPTUALCOLOR_VERSION_H
