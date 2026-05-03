// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromainfo.h"

#include "absolutecolor.h"
#include "genericcolor.h"
#include "helperconstants.h"
#include <algorithm>
#include <qcolor.h>
#include <qlist.h>
#include <utility>

namespace PerceptualColor
{

/**
 * @brief Meyer’s singleton: Provides the instance of this class as reference.
 *
 * @returns Meyer’s singleton: Provides the instance of this class as reference.
 */
const ChromaInfo &ChromaInfo::instance()
{
    static ChromaInfo s;
    return s;
}

/**
 * @brief Destructor.
 */
ChromaInfo::~ChromaInfo() noexcept
{
}

/**
 * @brief The maximum possible chroma value in the sRGB gamut.
 *
 * @return The maximum possible chroma value in the sRGB gamut.
 */
double ChromaInfo::maxCielchD50Chroma()
{
    return instance().m_profileMaximumCielchD50Chroma;
}

/**
 * @brief The maximum possible chroma value in the sRGB gamut.
 *
 * @return The maximum possible chroma value in the sRGB gamut.
 */
double ChromaInfo::maxOklchChroma()
{
    return instance().m_profileMaximumOklchChroma;
}

/**
 * @brief Initialization for various data items related to the chromatic
 * boundary.
 */
ChromaInfo::ChromaInfo()
{
    // Find blackpoint and whitepoint.
    // For CielabD50 make sure that: 0 <= blackpoint < whitepoint <= 100
    GenericColor candidate;
    candidate.second = 0;
    candidate.third = 0;
    candidate.first = 0;
    while (!AbsoluteColor::isLchInSRgbGamut(candidate, LchSpace::CielchD50)) {
        candidate.first += gamutPrecisionCielab;
        if (candidate.first >= 100) {
            candidate.first = 100;
            break;
        }
    }
    m_cielabD50BlackpointL = candidate.first;
    candidate.first = 100;
    while (!AbsoluteColor::isLchInSRgbGamut(candidate, LchSpace::CielchD50)) {
        candidate.first -= gamutPrecisionCielab;
        if (candidate.first <= m_cielabD50BlackpointL) {
            candidate.first = m_cielabD50BlackpointL;
            break;
        }
    }
    m_cielabD50WhitepointL = candidate.first;
    // For Oklab make sure that: 0 <= blackbpoint < whitepoint <= 1
    candidate.first = 0;
    while (!AbsoluteColor::isLchInSRgbGamut(candidate, LchSpace::Oklch)) {
        candidate.first += gamutPrecisionOklab;
        if (candidate.first >= 1) {
            candidate.first = 1;
            break;
        }
    }
    m_oklabBlackpointL = candidate.first;
    candidate.first = 1;
    while (!AbsoluteColor::isLchInSRgbGamut(candidate, LchSpace::Oklch)) {
        candidate.first -= gamutPrecisionOklab;
        if (candidate.first <= m_oklabBlackpointL) {
            candidate.first = m_oklabBlackpointL;
            break;
        }
    }
    m_oklabWhitepointL = candidate.first;

    // Now, calculate the properties who’s calculation depends on a fully
    // initialized object.
    QList<QColor> chromaticityBoundaryQColor;
    chromaticityBoundaryQColor.reserve(256 * 6);
    for (int value = 0; value <= 255; ++value) {
        // This will create six duplicate values (at the borders between
        // the six value groups). These will be filtered out later
        // automatically, because std::map does not allow duplicates.

        // Red = 255
        chromaticityBoundaryQColor.append(QColor(255, value, 0)); // Vary green
        chromaticityBoundaryQColor.append(QColor(255, 0, value)); // Vary blue

        // Green = 255
        chromaticityBoundaryQColor.append(QColor(value, 255, 0)); // Vary red
        chromaticityBoundaryQColor.append(QColor(0, 255, value)); // Vary blue

        // Blue = 255
        chromaticityBoundaryQColor.append(QColor(value, 0, 255)); // Vary red
        chromaticityBoundaryQColor.append(QColor(0, value, 255)); // Vary green
    }

    m_profileMaximumCielchD50Chroma = 0;
    m_profileMaximumOklchChroma = 0;
    for (auto &color : chromaticityBoundaryQColor) {
        const auto rgb_1 = GenericColor( //
            static_cast<double>(color.redF()), //
            static_cast<double>(color.greenF()), //
            static_cast<double>(color.blueF()));
        const auto linearSRgb_1 = AbsoluteColor::fromSRgbToLinearSRgb(rgb_1);
        const auto xyzD65 = AbsoluteColor::fromLinearSRgbToXyzD65(linearSRgb_1);
        const auto oklab = AbsoluteColor::fromXyzD65ToOklab(xyzD65);
        const auto oklch = AbsoluteColor::fromCartesianToPolar(oklab);
        const auto xyzD50 = AbsoluteColor::fromXyzD65ToXyzD50(xyzD65);
        const auto cielabD50 = AbsoluteColor::fromXyzD50ToCielabD50(xyzD50);
        const auto cielchD50 = AbsoluteColor::fromCartesianToPolar(cielabD50);
        m_profileMaximumCielchD50Chroma = qMax( //
            m_profileMaximumCielchD50Chroma, //
            cielchD50.second);
        m_chromaticityBoundaryByCielchD50Hue360[cielchD50.third] = color;

        m_profileMaximumOklchChroma = qMax( //
            m_profileMaximumOklchChroma, //
            oklch.second);
        m_chromaticityBoundaryByOklabHue360[oklch.third] = color;
    }

    auto addDuplicates = [](auto &boundaryMap) {
        const auto firstKey = boundaryMap.begin()->first;
        const auto firstValue = boundaryMap.begin()->second;
        const auto lastKey = boundaryMap.rbegin()->first;
        const auto lastValue = boundaryMap.rbegin()->second;
        // In our circle, we create duplicates for the lowest and highest
        // angles beyond the [0, 360] boundary on the opposite side of the
        // circle. For example, the lowest original key is 2° and its duplicate
        // is placed at 362°, while the highest original key might be 357°,
        // with its duplicate at -3°.
        boundaryMap[firstKey + 360] = firstValue;
        boundaryMap[lastKey - 360] = lastValue;
    };
    addDuplicates(m_chromaticityBoundaryByCielchD50Hue360);
    addDuplicates(m_chromaticityBoundaryByOklabHue360);

    m_profileMaximumCielchD50Chroma = std::min<double>( //
        m_profileMaximumCielchD50Chroma, //
        cielchD50Values.maximumChroma);

    m_profileMaximumOklchChroma = std::min<double>( //
        m_profileMaximumOklchChroma, //
        oklchValues.maximumChroma);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param hue360 hue in the range [0, 360]
 *
 * @returns the most chromatic color for the given hue in the sRGB gamut.
 */
QColor ChromaInfo::maxChromaColorByOklabHue360(double hue360)
{
    return maxChromaColorByHue360( //
        hue360, //
        LchSpace::Oklch);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param hue360 hue in the range [0, 360]
 *
 * @returns the most chromatic color for the given hue in the sRGB gamut.
 */
QColor ChromaInfo::maxChromaColorByCielchD50Hue360(double hue360)
{
    return maxChromaColorByHue360( //
        hue360, //
        LchSpace::CielchD50);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param hue360 Oklab hue in the range [0, 360]
 * @param type The type of Lch color space.
 *
 * @returns the most chromatic color for the given hue in the sRGB gamut.
 */
QColor ChromaInfo::maxChromaColorByHue360(double hue360, PerceptualColor::LchSpace type)
{
    const auto &table = (type == LchSpace::CielchD50) //
        ? instance().m_chromaticityBoundaryByCielchD50Hue360 //
        : instance().m_chromaticityBoundaryByOklabHue360;

    // begin() points to the actual first key-value pair.
    // end() points to a virtual key-value pair after the last actual
    // key-value pair. Dereferencing is not allowed.

    // lower_bound: Returns an iterator pointing to the first element that
    // is not less than (i.e. greater or equal to) key.
    auto greaterOrEqual = //
        table.lower_bound(hue360);

    if (greaterOrEqual == table.begin()) {
        // All available keys are greater than the search key. So the key
        // at the begin is the closest match:
        return greaterOrEqual->second; // "second" returns the value of
                                       // the key-value pair.
        // NOTE If the map is empty, begin() == end(), so we would get a
        // crash. Therefore, we have to make sure that the map is initialized
        // in the constructor.
    }

    auto lower = --greaterOrEqual; // Move to the lower key

    if (greaterOrEqual == table.end()) {
        // We are at the end of the map. greaterOrEqual is not a valid
        // key-value pair. Return the value of the previous key-value pair.
        return lower->second;
    }

    // Compare distances to find the closest key
    const auto distanceToLower = qAbs(hue360 - lower->first);
    const auto distanceToHigher = qAbs(hue360 - greaterOrEqual->first);
    if (distanceToLower <= distanceToHigher) {
        return lower->second;
    } else {
        return greaterOrEqual->second;
    }
}

/**
 * @brief The darkest in-gamut point on the L* axis.
 *
 * @returns The darkest in-gamut point on the L* axis.
 *
 * @sa @ref m_cielabD50WhitepointL()
 */
double ChromaInfo::cielabD50BlackpointL()
{
    return instance().m_cielabD50BlackpointL;
}

/**
 * @brief The lightest in-gamut point on the L* axis.
 *
 * @returns The lightest in-gamut point on the L* axis.
 *
 * @sa @ref m_cielabD50BlackpointL()
 */
double ChromaInfo::cielabD50WhitepointL()
{
    return instance().m_cielabD50WhitepointL;
}

/**
 * @brief The darkest in-gamut point on the L* axis.
 *
 * @returns The darkest in-gamut point on the L* axis.
 *
 * @sa @ref m_oklabWhitepointL()
 */
double ChromaInfo::oklabBlackpointL()
{
    return instance().m_oklabBlackpointL;
}

/**
 * @brief The lightest in-gamut point on the L* axis.
 *
 * @returns The lightest in-gamut point on the L* axis.
 *
 * @sa @ref m_oklabBlackpointL()
 */
double ChromaInfo::oklabWhitepointL()
{
    return instance().m_oklabWhitepointL;
}

} // namespace PerceptualColor
