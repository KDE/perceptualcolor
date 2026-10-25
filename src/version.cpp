// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "version.h"

#include <limits>

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR >= 0);

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR >= 0);

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH >= 0);

static_assert(std::numeric_limits<decltype(PERCEPTUALCOLOR_COMPILE_TIME_VERSION)>::is_integer);
static_assert(PERCEPTUALCOLOR_COMPILE_TIME_VERSION >= 1);

namespace PerceptualColor
{
} // namespace PerceptualColor
