// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

/** @internal @file staticasserts.cpp
 *
 * This file defines various static asserts for compiling this
 * library. */

#include <qglobal.h>

static_assert(
    // Test if the compiler treats the source code actually as UTF-8.
    // We use the character “🖌” who’s code point is U+1F58C.
    // We create a string literal in the form U"🖌" which creates a UTF-32
    // encoded Unicode string. So we expect the first code unit of this string
    // to be 0x1F58C, which is the correct UTF-32 representation. We do
    // a static_assert to control if the compiler has actually correctly
    // interpreted the source code.
    (*(U"🖌")) == 0x1F58C,
    "Compiler must use UTF-8 as input character set.\n"
    "(The source code has to be interpreted as UTF-8 by the compiler.)");

static_assert(
    // Check if actually the narrow execution character set is UTF-8.
    // Character 1, first code unit
    (static_cast<quint8>(*(("🖌") + 0)) == 0xF0)
        // Character 1, second code unit
        && (static_cast<quint8>(*(("🖌") + 1)) == 0x9F)
        // Character 1, third code unit
        && (static_cast<quint8>(*(("🖌") + 2)) == 0x96)
        // Character 1, fourth code unit
        && (static_cast<quint8>(*(("🖌") + 3)) == 0x8C)
        // Character 2
        && (static_cast<quint8>(*(("🖌") + 4)) == 0x00),
    // Provide an error message:
    "Compiler must use UTF-8 as narrow execution character set.\n"
    "(char* must contain UTF-8 encoded data.)\n"
    "Example: gcc -fexec-charset=UTF-8");
