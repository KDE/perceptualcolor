// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "genericcolor.h"

#include <lcms2.h>

namespace PerceptualColor
{

/** @brief Constructor.
 *
 * @param list Initial values. Only the first 4 elements are considered.
 *             Excess elements are ignored. Missing elements are
 *             interpreted as 0. */
GenericColor::GenericColor(const QList<double> &list)
    : first(list.value(0, 0))
    , second(list.value(1, 0))
    , third(list.value(2, 0))
    , fourth(list.value(3, 0))
{
}

/** @brief The values @ref first, @ref second, @ref third as @ref Trio.
 *
 * @returns The values @ref first, @ref second, @ref third as @ref Trio. */
Trio GenericColor::toTrio() const
{
    return createMatrix<1, 3, double>(first, second, third);
}

/** @brief The values @ref first, @ref second, @ref third as QList.
 *
 * @returns The values @ref first, @ref second, @ref third as QList. */
QList<double> GenericColor::toQList3() const
{
    return QList<double>{first, second, third};
}

/** @brief Type conversion.
 *
 * @warning Interprets the current data members as XZY.
 *
 * @returns Type conversion. */
cmsCIEXYZ GenericColor::reinterpretAsXyzToCmsciexyz() const
{
    return cmsCIEXYZ{first, second, third};
}

/** @brief Type conversion.
 *
 * @warning Interprets the current data members as Lab.
 *
 * @returns Type conversion. */
cmsCIELab GenericColor::reinterpretAsLabToCmscielab() const
{
    return cmsCIELab{first, second, third};
}

/** @brief Type conversion.
 *
 * @warning Interprets the current data members as LCh.
 *
 * @returns Type conversion. */
cmsCIELCh GenericColor::reinterpretAsLchToCmscielch() const
{
    return cmsCIELCh{first, second, third};
}

/** @brief Equal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if equal, <tt>false</tt> otherwise. */
bool GenericColor::operator==(const GenericColor &other) const
{
    return ( //
        (first == other.first) //
        && (second == other.second) //
        && (third == other.third) //
        && (fourth == other.fourth) //
    );
}

/** @brief Unequal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if unequal, <tt>false</tt> otherwise. */
bool GenericColor::operator!=(const GenericColor &other) const
{
    return !(*this == other);
}

/** @internal
 *
 * @brief Adds QDebug() support for data type
 * @ref PerceptualColor::GenericColor
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::GenericColor &value)
{
    dbg.nospace() //
        << "GenericColor(" << //
        value.first << ", " //
        << value.second << ", " //
        << value.third << ", " //
        << value.fourth << ")";
    return dbg.maybeSpace();
}

} // namespace PerceptualColor
