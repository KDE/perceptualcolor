// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_P_H
#define PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_P_H

// Include the header of the public class of this private implementation.
// #include "chromalightnessdiagram.h"

#include "asyncimageprovider.h"
#include "chromalightnessimageparameters.h"
#include "constpropagatingrawpointer.h"
#include "genericcolor.h"
#include "helperconversion.h"
#include "lchvalues.h"
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <qglobal.h>
#include <qsharedpointer.h>
#include <qsize.h>
class QRect;
class QPoint;

namespace PerceptualColor
{
class ChromaLightnessDiagram;
class ColorEngine;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ChromaLightnessDiagramPrivate
{
public:
    explicit ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink, const LchSpace projectionSpace);
    virtual ~ChromaLightnessDiagramPrivate() noexcept;

    // Member variables
    /** @brief The image of the chroma-lightness diagram itself. */
    AsyncImageProvider<ChromaLightnessImageParameters> m_chromaLightnessImage;
    /** @brief Properties for @ref m_chromaLightnessImage. */
    ChromaLightnessImageParameters m_chromaLightnessImageParameters;
    /** @brief Internal storage of
     * the @ref ChromaLightnessDiagram::currentColorLch property */
    GenericColor m_currentColorLch;
    /** @brief Pointer to ColorEngine() object */
    QSharedPointer<ColorEngine> m_colorEngine;
    /**
     * @brief Geometry of the current Lch color space.
     * */
    const LchValues m_lchValues;
    /**
     * @brief The color space into which the gamut will be projected.
     */
    const LchSpace m_projectionSpace;

    // Member functions
    [[nodiscard]] QSize calculateImageSizePhysical() const;
    [[nodiscard]] int defaultBorderDeviceIndependant() const;
    [[nodiscard]] int defaultBorderPhysical() const;
    /** @internal
     *
     * @brief Calculate how far a value is from a given range.
     * @pre <tt>low</tt> ≤ <tt>high</tt>
     * @param low the lower limit
     * @param x the value that will be tested
     * @param high the higher limit
     * @returns <tt>0</tt> if the value is within the range. The distance
     * to the nearest border of the range otherwise. */
    template<typename T>
    [[nodiscard]] static constexpr T distanceFromRange(const T &low, const T &x, const T &high)
    {
        if (x < low) {
            return low - x;
        }
        if (x > high) {
            return x - high;
        }
        if constexpr ( //
            std::numeric_limits<T>::has_quiet_NaN //
            || std::numeric_limits<T>::has_signaling_NaN //
        ) {
            if (std::isnan(low) || std::isnan(x) || std::isnan(high)) {
                return std::numeric_limits<T>::quiet_NaN();
            }
        }
        return 0;
    }
    [[nodiscard]] GenericColor fromWidgetPixelPositionToLch(const QPoint widgetPixelPosition) const;
    [[nodiscard]] bool isWidgetPixelPositionInGamut(const QPoint widgetPixelPosition) const;
    [[nodiscard]] int leftBorderDeviceIndependent() const;
    [[nodiscard]] int leftBorderPhysical() const;
    [[nodiscard]] GenericColor nearestInGamutLchByAdjustingChromaLightness(const double chroma, const double lightness);
    [[nodiscard]] std::optional<QPoint> nearestInGamutPixelPosition(const QPoint originalPixelPosition);
    [[nodiscard]] static std::optional<QPoint>
    nearestNeighborSearch(const QPoint point, const QRect boundingBox, const std::function<bool(const QPoint)> &doesPointExist);
    void setCurrentColorFromWidgetPixelPosition(const QPoint widgetPixelPosition);
    void updateImageDimensions();

private:
    Q_DISABLE_COPY(ChromaLightnessDiagramPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaLightnessDiagram> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_P_H
