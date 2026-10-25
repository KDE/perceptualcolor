// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "mat3.h"

#include <algorithm>
#include <qbytearray.h>
#include <qglobal.h>
#include <qstring.h>
namespace PerceptualColor
{

/*!
 * \relates PerceptualColor::Mat3
 *
 * \brief Stream to QDebug.
 *
 * Prints the matrix in a compact format.
 *
 * \a dbg QDebug stream
 *
 * \a value Matrix to print
 */
QDebug operator<<(QDebug dbg, const PerceptualColor::Mat3d &value)
{
    QDebugStateSaver saver(dbg);

    // Convert numbers to strings with fixed decimal count
    // and calculate maximum width
    QString strings[9];
    QByteArray encoded[9];
    for (size_t column = 0; column < 3; ++column) {
        qsizetype maxWidth = 0;
        for (size_t row = 0; row < 3; ++row) {
            const size_t index = row * 3 + column;
            strings[index] = //
                QString::number(static_cast<double>(value.m[index]), 'f', 3);
            maxWidth = std::max<qsizetype>(maxWidth, strings[index].size());
        }
        for (size_t row = 0; row < 3; ++row) {
            const size_t index = row * 3 + column;
            encoded[index] = strings[index].rightJustified(maxWidth).toUtf8();
        }
    }

    // Output with Unicode braces, right-aligned
    dbg.nospace() << "\n⎡ " << encoded[0] << " " << encoded[1] << " " << encoded[2] << " ⎤\n"
                  << "⎢ " << encoded[3] << " " << encoded[4] << " " << encoded[5] << " ⎥\n"
                  << "⎣ " << encoded[6] << " " << encoded[7] << " " << encoded[8] << " ⎦";

    return dbg;
}

/*!
 * \relates PerceptualColor::Mat3
 *
 * \brief Stream to QDebug.
 *
 * Prints the matrix in a compact format.
 *
 * \a dbg QDebug stream
 *
 * \a value Matrix to print
 */
QDebug operator<<(QDebug dbg, const PerceptualColor::Mat3f &value)
{
    return dbg << static_cast<Mat3d>(value);
}

/*!
 * \relates PerceptualColor::Mat3
 *
 * \brief Stream to QDebug.
 *
 * Prints the matrix in a compact format.
 *
 * \a dbg QDebug stream
 *
 * \a value Matrix to print
 */
QDebug operator<<(QDebug dbg, const PerceptualColor::Mat3ld &value)
{
    return dbg << static_cast<Mat3d>(value);
}

} // namespace PerceptualColor
