// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "genericcolor.h"

namespace PerceptualColor
{

/*!
 * \brief Constructor.
 *
 * \a list Initial values. Only the first 4 elements are considered.
 *             Excess elements are ignored. Missing elements are
 *             interpreted as 0. */
GenericColor::GenericColor(const QList<double> &list)
    : first(list.value(0, 0))
    , second(list.value(1, 0))
    , third(list.value(2, 0))
{
}

/*!
 * \brief The values \l first, \l second, \l third as QList.
 *
 * Returns the values \l first, \l second, \l third as QList. */
QList<double> GenericColor::toQList3() const
{
    return QList<double>{first, second, third};
}

/*!
 * \relates PerceptualColor::GenericColor
 *
 * \brief Adds QDebug() support for data type \l PerceptualColor::GenericColor
 *
 * \a dbg Existing debug object
 *
 * \a value Value to stream into the debug object
 */
QDebug operator<<(QDebug dbg, const PerceptualColor::GenericColor &value)
{
    dbg.nospace() //
        << "GenericColor(" << //
        value.first << ", " //
        << value.second << ", " //
        << value.third << ")";
    return dbg.maybeSpace();
}

} // namespace PerceptualColor
