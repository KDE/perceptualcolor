// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "vec3.h"

#include <algorithm>
#include <qbytearray.h>
#include <qglobal.h>
#include <qlist.h>
#include <qstring.h>

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Stream to QDebug.
 *
 * Prints the vector in a compact format.
 *
 * @param dbg QDebug stream
 * @param value Vector to print
 * @return QDebug stream
 */
QDebug operator<<(QDebug dbg, const Vec3d &value)
{
    QDebugStateSaver saver(dbg);

    // Convert numbers to strings with fixed decimal count
    // and calculate maximum width
    qsizetype maxWidth = 0;
    QString strings[3];
    for (size_t i = 0; i < 3; ++i) {
        strings[i] = QString::number(static_cast<double>(value(i)), 'f', 3);
        maxWidth = std::max<qsizetype>(maxWidth, strings[i].size());
    }

    QList<QByteArray> encoded;
    encoded.reserve(9);
    for (const auto &s : strings) {
        encoded << s.rightJustified(maxWidth).toUtf8();
    }

    // Output with Unicode braces, right-aligned
    dbg.nospace() << "\n⎡ " << encoded[0].constData() << " ⎤\n"
                  << "⎢ " << encoded[1].constData() << " ⎥\n"
                  << "⎣ " << encoded[2].constData() << " ⎦";

    return dbg;
}

/**
 * @internal
 *
 * @brief Stream to QDebug.
 *
 * Prints the vector in a compact format.
 *
 * @param dbg QDebug stream
 * @param value Vector to print
 * @return QDebug stream
 */
QDebug operator<<(QDebug dbg, const Vec3f &value) // clazy:exclude=function-args-by-value
{
    return dbg << static_cast<Vec3d>(value);
}

/**
 * @internal
 *
 * @brief Stream to QDebug.
 *
 * Prints the vector in a compact format.
 *
 * @param dbg QDebug stream
 * @param value Vector to print
 * @return QDebug stream
 */
QDebug operator<<(QDebug dbg, const Vec3ld &value)
{
    return dbg << static_cast<Vec3d>(value);
}

} // namespace PerceptualColor
