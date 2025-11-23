// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_HELPERPOSIXMATH_H
#define PERCEPTUALCOLOR_HELPERPOSIXMATH_H

/** @internal
 *
 * @file
 * */

namespace PerceptualColor
{

/** @internal
 *
 * @brief pi
 *
 * Alternaive to <tt>std::numbers::pi</tt> (only available starting with
 * C++20, be we have C++17 compatibility here), and to M_PI.
 *
 * From <tt>&lt;qmath.h&gt;</tt> documentation:
 *
 * > The header also ensures some constants specified in POSIX, but not
 * > present in C++ standards (so absent from <tt>&lt;math.h&gt;</tt> on some
 * > platforms), are defined:
 *
 * So <tt>&lt;qmath.h&gt;</tt>  provides M_PI from POSIX. But we could forget
 * to include  <tt>&lt;qmath.h&gt;</tt>, and it would work on Unix but fail on
 * Windows, which does not provide POSIX. Also, a constexpr is much more
 * convenient to use than M_PI, which are macros.
 */
inline constexpr double pi = 3.141592653589793238462643383279502884197169399375;

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_HELPERPOSIXMATH_H
