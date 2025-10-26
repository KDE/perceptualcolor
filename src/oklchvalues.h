// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef OKLCHVALUES_H
#define OKLCHVALUES_H

namespace PerceptualColor
{
/** @internal
 *
 * @brief Oklch default values
 *
 * The @ref lchrangerationale "range of Oklch values" it known.
 * But what could be useful default values? This struct provides some
 * proposals. All values are <tt>constexpr</tt>. */
struct OklchValues final {
public:
    /** @brief Maximum chroma value as defined in @ref lchrangerationale. */
    static constexpr int maximumChroma = 2;

private:
    /** @brief Delete the constructor to disallow creating an instance
     * of this class. */
    OklchValues() = delete;
};

} // namespace PerceptualColor

#endif // OKLCHVALUES_H
