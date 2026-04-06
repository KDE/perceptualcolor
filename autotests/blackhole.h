// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_BLACKHOLE_H
#define PERCEPTUALCOLOR_BLACKHOLE_H

#include <atomic>

namespace PerceptualColor
{

/**
 * @brief Blackhole function to prevent compiler optimizations.
 *
 * This utility function acts as a “black hole” for computed values:
 * it consumes the given value so that the compiler cannot eliminate
 * the computation as dead code.
 *
 * @tparam T Type of the value to consume. The type must have a
 * default-constructor and a copy-constructor.
 * @param value The value to sink into the black hole.
 *
 * @internal
 *
 * A more typical solution might be: <br/>
 *
 * <tt>static std::atomic<T> sink;<br/>
 * sink.store(value, std::memory_order_relaxed);</tt>
 *
 * The value is stored into a static
 * <tt>std::atomic</tt> using relaxed memory ordering.
 * memory_order_relaxed is used to minimize overhead. It guarantees
 * that the store operation is performed atomically, but does not
 * impose any synchronization or ordering constraints. This is
 * sufficient for preventing unwanted compiler optimizations.
 * Unlike <tt>volatile</tt>, <tt>std::atomic</tt> is standard-conform and
 * safe. <tt>volatile</tt> only prevents certain optimizations, but does not
 * provide atomicity or memory barriers. <tt>std::atomic</tt> ensures the
 * store is performed and visible, even if used in threads and the value is
 * never read back.
 * However, this gave problems with libatomic, which must be linked on some
 * platforms, and must not be linked on others. Therefore we prefer a simple
 * static variable plus some memory barriers.
 */
template<typename T>
void blackhole(const T &value)
{
    static T sink;
    std::atomic_signal_fence(std::memory_order_seq_cst);
    sink = value;
    std::atomic_signal_fence(std::memory_order_seq_cst);
    Q_UNUSED(sink)
}

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_BLACKHOLE_H
