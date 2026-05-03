// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_LCHVALUES_H
#define PERCEPTUALCOLOR_LCHVALUES_H

#include "perceptualcolornamespace.h"
#include <genericcolor.h>
#include <stdint.h>

namespace PerceptualColor
{
/**
 * @internal
 *
 * @brief Default values for Lch color spaces.
 *
 * The @ref lchrangerationale "range of CIELCh-D50 values" it known.
 * But what could be useful default values? This struct provides some
 * proposals. All values are <tt>constexpr</tt>.
 *
 * @sa @ref cielchD50Values
 * @sa @ref oklchValues
 */
struct LchValues {
public:
    /**
     * @brief Maximum lightness value.
     */
    uint_fast8_t maximumLightness;

    /**
     *@brief Maximum chroma value.
     */
    uint_fast8_t maximumChroma;

    /**
     * @brief Maximum hue value.
     */
    uint_fast16_t maximumHue;

    /**
     * @brief Neutral lightness value
     *
     *  For the lightness, a neutral value of 50% seems a good choice. As it
     *  is in the middle of the gamut solid, it allows for quite big values
     *  for chroma at different hues without falling out-of-gamut. Combined
     *  with a chroma on the gray axis, it also approximates the color with
     *  the highest possible contrast against the hole surface of the
     *  gamut solid; this is interesting for background colors of
     *  gamut diagrams.
     */
    double neutralLightness;

    /**
     * @brief Neutral chroma value
     *
     *  For chroma, a neutral value on the gray axis might be a good
     *  choice because it is less likely to make  out-of-gamut problems on
     *  any lightness (except maybe extreme white or extreme black). And
     *  it results in an achromatic color and is therefore perceived as
     *  neutral.
     */
    uint_fast8_t neutralChroma;

    /**
     * @brief Neutral hue value
     *
     *  A default value by convention.
     */
    uint_fast8_t neutralHue;

    /** @internal
     *
     * @brief Amount of single step for lightness, a, b, and chroma (but
     * <em>not</em> hue) components.
     *
     * The smaller of two natural steps that a widget provides and
     * typically corresponds to the user pressing a key or using the mouse
     * wheel: The value will be incremented/decremented by the amount of this
     * value.
     *
     * Use @ref pageStepFactor to calculate corresponding page step (the
     * larger of two natural steps).
     */
    double singleStepLabc;

    /**
     * @brief Neutral gray color as Lab value.
     *
     * Neutral gray is a good choice for the background, as it is equally
     * distant from black and white, and also quite distant from any
     * saturated color.
     *
     * @returns Neutral gray color as Lab value.
     */
    constexpr GenericColor neutralGray() const noexcept
    {
        return GenericColor(static_cast<double>(neutralLightness), //
                            neutralChroma,
                            neutralHue);
    }
};

/**
 * @internal
 *
 * @brief @ref LchValues for CielchD50 as of @ref lchrangerationale.
 */
constexpr LchValues cielchD50Values{100, 255, 360, 50, 0, 0, 1};

/**
 * @internal
 *
 * @brief @ref LchValues for Oklch as of @ref lchrangerationale.
 */
constexpr LchValues oklchValues{1, 2, 360, 0.5, 0, 0, 0.01};

/**
 * @internal
 *
 * @brief @ref LchValues for @ref LchSpace
 *
 * @param space The space
 *
 * @returns @ref LchValues for @ref LchSpace
 */
constexpr LchValues makeLchValues(LchSpace space) noexcept
{
    switch (space) {
    case LchSpace::CielchD50:
        return cielchD50Values;
    case LchSpace::Oklch:
        return oklchValues;
    }
    return cielchD50Values;
}

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_LCHVALUES_H
