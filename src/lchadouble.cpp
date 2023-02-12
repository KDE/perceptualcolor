// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "lchadouble.h"

#include <type_traits>

#include <lcms2.h>

/** @internal
 *
 * @file
 *
 * This file defines some static asserts for the data type
 * @ref PerceptualColor::LchaDouble. */

namespace PerceptualColor
{
// We are using double. Check that we stay compatible with cmsCIELCh
// which is based on cmsFloat64Number.
static_assert(std::is_same_v<cmsFloat64Number, double>);

static_assert(sizeof(double) == sizeof(cmsFloat64Number));

static_assert(std::is_trivially_copyable_v<LchaDouble>);
static_assert(std::is_trivial_v<LchaDouble>);

static_assert(std::is_standard_layout_v<LchaDouble>);

static_assert(std::is_default_constructible_v<LchaDouble>);
static_assert(std::is_trivially_default_constructible_v<LchaDouble>);
static_assert(std::is_nothrow_default_constructible_v<LchaDouble>);

static_assert(std::is_copy_constructible_v<LchaDouble>);
static_assert(std::is_trivially_copy_constructible_v<LchaDouble>);
static_assert(std::is_nothrow_copy_constructible_v<LchaDouble>);

static_assert(std::is_move_constructible_v<LchaDouble>);
static_assert(std::is_trivially_move_constructible_v<LchaDouble>);
static_assert(std::is_nothrow_move_constructible_v<LchaDouble>);

/** @brief Adds QDebug() support for data type
 * @ref PerceptualColor::LchaDouble
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in
 *
 * @internal
 *
 * @todo This is originally declared in the global namespace instead of
 * the @ref PerceptualColor namespace, because the supported value was
 * a <tt>typedef</tt> for a LittleCMS type in the global; when declaring
 * this function in @ref PerceptualColor namespace, it did not work
 * in the global namespace. Now, things have changed. But we should write
 * a unit test for if it works in global namespace! */
QDebug operator<<(QDebug dbg, const PerceptualColor::LchaDouble &value)
{
    dbg.nospace() << "LchaDouble(" << value.l << "% " << value.c << " " << value.h << "° " << value.a << ")";
    return dbg.maybeSpace();
}

/** @brief Compares coordinates with another object.
 *
 * @param other The object to compare with
 * @returns <tt>true</tt> if all three coordinates @ref l, @ref c and @ref h
 * of <em>this</em> object are all equal to the coordinates of <em>other</em>.
 * <tt>false</tt> otherwise. Note that two objects with equal @ref l and
 * equal @ref c, but one with h = 5° and the other with h = 365°, are
 * considered non-equal thought both describe the same point in the
 * coordinate space. */
bool LchaDouble::hasSameCoordinates(const PerceptualColor::LchaDouble &other) const
{
    return ((l == other.l) && (c == other.c) && (h == other.h) && (a == other.a));
}

} // namespace PerceptualColor
