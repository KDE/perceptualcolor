// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own header
#include "rgbdouble.h"

#include <type_traits>

#include <lcms2.h>

#include <qdebug.h>

/** @internal
 *
 * @file
 *
 * This file defines some static asserts for the data type
 * @ref PerceptualColor::RgbDouble. */

namespace PerceptualColor
{

/** @internal
 *
 * @brief Adds QDebug() support for data type
 * @ref PerceptualColor::RgbDouble
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::RgbDouble &value)
{
    dbg.nospace() //
        << "RgbDouble(" //
        << value.red //
        << " " //
        << value.green //
        << " " //
        << value.blue //
        << ")";
    return dbg.maybeSpace();
}

static_assert(std::is_same_v<cmsFloat64Number, decltype(RgbDouble::red)>);

static_assert(std::is_same_v<cmsFloat64Number, decltype(RgbDouble::green)>);

static_assert(std::is_same_v<cmsFloat64Number, decltype(RgbDouble::blue)>);

static_assert(sizeof(double) == sizeof(cmsFloat64Number));

// Assert that RgbDouble also works fine as buffer.
static_assert(sizeof(RgbDouble) == sizeof(cmsFloat64Number[3]));

static_assert(std::is_trivially_copyable_v<RgbDouble>);
static_assert(std::is_trivial_v<RgbDouble>);

static_assert(std::is_standard_layout_v<RgbDouble>);

} // namespace PerceptualColor
