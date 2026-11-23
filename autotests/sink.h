// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_SINK_H
#define PERCEPTUALCOLOR_SINK_H

#include <atomic>

namespace PerceptualColor
{

/*
 * \brief Sink function to prevent compiler optimizations.
 *
 * This utility function acts as a “sink” or “black hole” for computed values:
 * it consumes the given value so that the compiler cannot eliminate
 * the computation as dead code.
 *
 * \b T Type of the value to consume.
 *
 * \a value The value to sink into the black hole.
 */
template<typename T>
void sink(const T &value)
{
    auto p = reinterpret_cast<volatile const unsigned char *>(&value);
    static_cast<void>(*p);
    Q_UNUSED(p)
}

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_SINK_H
