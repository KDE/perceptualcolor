// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "perceptualcolornamespace.h"

/**
 * @internal
 *
 * @file
 *
 * This source file exists solely to trigger Qt’s Meta‑Object Compiler (moc)
 * to generate the meta‑object code for the @ref PerceptualColor namespace.
 * The header @ref perceptualcolornamespace.h declares the namespace with
 * Q_NAMESPACE and registers enums via Q_ENUM_NS. Moc, however, only emits
 * the implementation of staticMetaObject when the header is explicitly
 * included by a .cpp file that shares its name.
 *
 * Including the header in other sources provides only declarations; without
 * this dedicated .cpp file, moc would not produce the meta‑object
 * implementation and linking would fail with “<tt>undefined reference to
 * PerceptualColor::staticMetaObject</tt>”.
 *
 * By including the header here, we ensure moc processes it, generates the
 * required code, and properly registers all Q_ENUM_NS declarations in the
 * namespace.
 */

namespace PerceptualColor
{

} // namespace PerceptualColor
