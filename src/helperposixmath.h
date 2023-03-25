// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERPOSIXMATH_H
#define HELPERPOSIXMATH_H

#include <qmath.h>

/** @internal
 *
 * @file
 *
 * From <tt>&lt;qmath.h&gt;</tt> documentation:
 *
 * > The header also ensures some constants specified in POSIX, but not
 * > present in C++ standards (so absent from <tt>&lt;math.h&gt;</tt> on some
 * > platforms), are defined:
 *
 * To ensure that we never forget to include <tt>&lt;qmath.h&gt;</tt>, we
 * define our own constexpr with these values here. When these constexpr are
 * used, it is ensured that <em>this</em> header is included, and thus
 * indirectly also <tt>&lt;qmath.h&gt;</tt>. Furthermore, the constexpr are
 * much more convenient to use than the POSIX constants, which are macros. */

namespace PerceptualColor
{

/** @internal
 *
 * @brief pi */
constexpr auto pi = M_PI;

} // namespace PerceptualColor

#endif // HELPERPOSIXMATH_H
