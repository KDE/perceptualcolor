// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CIELCHVALUES_H
#define CIELCHVALUES_H

#include "lchdouble.h"

namespace PerceptualColor
{
/** @internal
 *
 * @brief CIELCH default values
 *
 * The @ref lchrangerationale "range of CIELCh values" it known.
 * But what could be useful default values? This struct provides some
 * proposals. All values are <tt>constexpr</tt>. */
struct CielchValues final {
public:
    /** @brief Maximum chroma value as defined in @ref lchrangerationale. */
    static constexpr int maximumChroma = 255;
    /** @brief Neutral chroma value
     *
     *  For chroma, a neutral value of 0 might be a good
     *  choice because it is less likely to make  out-of-gamut problems on
     *  any lightness (except maybe extreme white or extreme black). And
     *  it results in an achromatic color and is therefore perceived as
     *  neutral. */
    static constexpr int neutralChroma = 0;
    /** @brief Neutral hue value
     *
     *  For the hue, a default value of 0 might be used by convention. */
    static constexpr int neutralHue = 0;
    /** @brief Neutral lightness value
     *
     *  For the lightness, a neutral value of 50 seems a good choice as it
     *  is half the way in the defined lightness range of <tt>[0, 100]</tt>
     *  (thought not all gamuts offer the hole range of <tt>[0, 100]</tt>). As
     *  it is quite in the middle of the gamut solid, it allows for quite big
     *  values for chroma at different hues without falling out-of-gamut.
     *  Combined with a chroma of 0, it also approximates the color with
     *  the highest possible contrast against the hole surface of the
     *  gamut solid; this is interesting for background colors of
     *  gamut diagrams. */
    static constexpr int neutralLightness = 50;
    /** @brief Neutral gray color as Lab value.
     *
     * Neutral gray is a good choice for the background, as it is equally
     * distant from black and white, and also quite distant from any
     * saturated color. */
    static constexpr LchDouble neutralGray{neutralLightness, //
                                           neutralChroma, //
                                           neutralHue};
    /** @brief Versatile chroma value in
     * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
     * sRGB gamut
     *
     *  Depending on the use case, there might be an alternative to
     *  the neutral gray @ref neutralChroma. For a lightness of 50, this
     *  value is the maximum chroma available at all possible hues within
     *  a usual sRGB gamut.
     *
     *  @sa @ref neutralChroma
     *
     * @todo This value should be removed in favor of using individual
     * values for each gamut. */
    static constexpr int srgbVersatileChroma = 32;
    /** @brief Versatile initial color for widgets in
     * <a href="http://www.littlecms.com/">LittleCMS</a>’ build-in
     * sRGB gamut
     *
     * This provides a colorful (non-gray) initial color.
     *
     * The lightness and the chroma value are somewhere in the middle of
     * the valid range. In sRGB this is always a valid value pair, even if
     * the hue changes. Both, lightness and chroma, are linear ranges,
     * so it seems appropriate to have values somewhere in the middle
     * as initial color: The widgets will display the handles somewhere
     * in the middle, which is easier to spot for new users than having
     * them at the very beginning or the very end.
     *
     * The hue range is not linear, but circular – and so are the widgets
     * displaying him: The handle has the same visibility, wherever he is
     * and whatever the hue value is. Therefore, we use 0° as default value
     * which seems to be the natural choice.
     *
     * @note An alternative to 0° might be 180°. If the lightness is 50%,
     * among 0°, 90°, 180° and 270°, it’s 180° that has the lowest possible
     * maximum chroma. So when choose 180°, with a given chroma, the resulting
     * color is more vivid and clean than those at 0°, 90° and 270°. */
    static constexpr LchDouble srgbVersatileInitialColor = {neutralLightness, srgbVersatileChroma, neutralHue};

private:
    /** @brief Delete the constructor to disallow creating an instance
     * of this class. */
    CielchValues() = delete;
};

} // namespace PerceptualColor

#endif // CIELCHVALUES_H
