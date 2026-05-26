// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORSPACEINFO_H
#define PERCEPTUALCOLOR_COLORSPACEINFO_H

#include "helpermath.h"
#include "lchvalues.h"
#include "perceptualcolornamespace.h"
#include <map>
#include <qcolor.h>
#include <qglobal.h>

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Access to color-space related data.
 */
class ColorSpaceInfo
{
public:
    [[nodiscard]] static double cielabD50BlackpointL();
    [[nodiscard]] static double cielabD50WhitepointL();
    [[nodiscard]] static QColor maxChromaColorByCielchD50Hue360(double hue360);
    [[nodiscard]] static QColor maxChromaColorByOklabHue360(double hue360);
    [[nodiscard]] static double maxCielchD50Chroma();
    [[nodiscard]] static double maxOklchChroma();
    [[nodiscard]] static double oklabBlackpointL();
    [[nodiscard]] static double oklabWhitepointL();

    /**
     * @brief Unusual shapes in @ref ChromaLightnessDiagram
     *
     * The image maps chroma on the x-axis and lightness on the y-axis,
     * drawn line by line from low chroma to high chroma at a fixed lightness.
     * Once a pixel in a row is out-of-gamut, all subsequent
     * pixels to the right are likely out-of-gamut as well. This suggests
     * a possible optimization: break the loop early. However, this assumption
     * is not universally valid, so we must explicitly mark ranges where
     * optimization is unsafe.
     *
     * Test renderings at 5000-pixel lightness resolution revealed exceptions:
     * - In CIELCH: hue 95.4518°–102.575° combined with lightness 93.64%–98.6%.
     *   To ensure coverage, we expand this region by ~10%: hue 95°–103°,
     *   lightness 93%–99%.
     * - In OKLCH: hue 264.052°–264.208° combined with lightness 0.0028–0.4704.
     *   Expanded by ~10%: hue 264.03°–264.23°, lightness 0.00–0.53.
     *
     * Within these unsafe ranges, early termination must be disabled to
     * guarantee correctness.
     *
     * @param projectionSpace The projection color space.
     * @param hue360 The hue to test. Must be in the range [0..360[.
     *
     * @returns <tt>true</tt> if there is an unusual shape. <tt>false</tt>
     * otherwise.
     */
    static constexpr bool isUnusualShapeAtHue(LchSpace projectionSpace, double hue360) noexcept
    {
        switch (projectionSpace) {
        case LchSpace::Oklch:
            return isInRange(264.03, hue360, 264.23);
        case LchSpace::CielchD50:
            return isInRange(95.0, hue360, 103.0);
        }
        return false; // default fallback
    }

    /**
     * @brief Unusual shapes in @ref ChromaLightnessDiagram
     *
     * The hole range of lightness values affected somewhere across
     * @ref isUnusualShapeAtHue.
     *
     * @param projectionSpace The projection color space.
     * @param lightness The lightness. The valid range is according to the
     * projection space.
     *
     * @returns <tt>true</tt> if there is an unusual shape. <tt>false</tt>
     * otherwise.
     */
    static constexpr bool isUnusualShapeAtLightness(LchSpace projectionSpace, double lightness) noexcept
    {
        switch (projectionSpace) {
        case LchSpace::Oklch:
            return isInRange(0.0, lightness, 0.53);
        case LchSpace::CielchD50:
            return isInRange(93.0, lightness, 99.0);
        }
        return false; // default fallback
    }

private:
    Q_DISABLE_COPY(ColorSpaceInfo)

    /**
     * @internal
     * @brief Only for unit tests.
     */
    friend class TestColorSpaceInfo;

    ColorSpaceInfo();
    [[nodiscard]] static const ColorSpaceInfo &instance();
    virtual ~ColorSpaceInfo() noexcept;

    [[nodiscard]] static QColor maxChromaColorByHue360(double hue360, PerceptualColor::LchSpace type);

    /**
     * @brief All RGB colors located on the chromaticity boundary, ordered by
     * CielchD50 hue.
     *
     * The chromaticity boundary consists of the spectral locus and the purple
     * line.
     *
     * This dataset includes all integer RGB values (i.e., values representable
     * with three one-byte channels) that lie on the chromaticity boundary.
     * To ensure smooth cyclic transitions, the dataset includes duplicates
     * of the lowest and highest hues positioned outside the [0, 360] boundary.
     * For instance, the lowest original angle of 2° has a duplicate at 362°,
     * and the highest original angle of 357° has a duplicate at -3°.
     *
     * The hue is normalized to the range [0, 360].
     */
    std::map<double, QColor> m_chromaticityBoundaryByCielchD50Hue360;
    /**
     * @brief All RGB colors located on the chromaticity boundary, ordered by
     * Oklab hue.
     *
     * The chromaticity boundary consists of the spectral locus and the purple
     * line.
     *
     * This dataset includes all integer RGB values (i.e., values representable
     * with three one-byte channels) that lie on the chromaticity boundary.
     * To ensure smooth cyclic transitions, the dataset includes duplicates
     * of the lowest and highest hues positioned outside the [0, 360] boundary.
     * For instance, the lowest original angle of 2° has a duplicate at 362°,
     * and the highest original angle of 357° has a duplicate at -3°.
     *
     * The hue is normalized to the range [0, 360].
     */
    std::map<double, QColor> m_chromaticityBoundaryByOklabHue360;

    /** @brief The darkest in-gamut point on the L* axis.
     *
     * @sa @ref m_cielabD50WhitepointL()
     */
    qreal m_cielabD50BlackpointL = 0;
    /** @brief The lightest in-gamut point on the L* axis.
     *
     * @sa @ref m_cielabD50BlackpointL()
     */
    qreal m_cielabD50WhitepointL = 100;
    /** @brief The darkest in-gamut point on the L* axis.
     *
     * @sa @ref m_oklabWhitepointL()
     */
    qreal m_oklabBlackpointL = 0;
    /** @brief The lightest in-gamut point on the L* axis.
     *
     * @sa @ref m_oklabBlackpointL()
     */
    qreal m_oklabWhitepointL = 1;
    /**
     * @brief Internal storage.
     */
    double m_profileMaximumCielchD50Chroma = cielchD50Values.maximumChroma;
    /**
     * @brief Internal storage.
     */
    double m_profileMaximumOklchChroma = oklchValues.maximumChroma;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORSPACEINFO_H
