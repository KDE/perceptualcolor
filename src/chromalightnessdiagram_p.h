// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CHROMALIGHTNESSDIAGRAM_P_H
#define CHROMALIGHTNESSDIAGRAM_P_H

// Include the header of the public class of this private implementation.
// #include "chromalightnessdiagram.h"

#include "asyncimageprovider.h"
#include "chromalightnessimageparameters.h"
#include "constpropagatingrawpointer.h"
#include "lchdouble.h"
#include <QtCore/qsharedpointer.h>
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <qglobal.h>
#include <qsize.h>
class QRect;
class QPoint;

namespace PerceptualColor
{
class ChromaLightnessDiagram;
class RgbColorSpace;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ChromaLightnessDiagramPrivate final
{
public:
    ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ChromaLightnessDiagramPrivate() noexcept = default;

    // Member variables
    /** @brief The image of the chroma-lightness diagram itself. */
    AsyncImageProvider<ChromaLightnessImageParameters> m_chromaLightnessImage;
    /** @brief Properties for @ref m_chromaLightnessImage. */
    ChromaLightnessImageParameters m_chromaLightnessImageParameters;
    /** @brief Internal storage of
     * the @ref ChromaLightnessDiagram::currentColor property */
    LchDouble m_currentColor;
    /** @brief Holds if currently a mouse event is active or not.
     *
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on a
     *   @ref ChromaLightnessDiagram::mousePressEvent()
     *   done within the gamut diagram. The value is set to <tt>true</tt>.
     * - While active, all @ref ChromaLightnessDiagram::mouseMoveEvent() will
     *   move the diagram’s color handle.
     * - Once a @ref ChromaLightnessDiagram::mouseReleaseEvent() occurs, the
     *   value is set to <tt>false</tt>. Further mouse movements will not
     *   move the handle anymore.
     *
     * This is done because Qt’s default mouse tracking reacts on all clicks
     * within the (rectangular) widget. However, <em>this</em> widget is meant
     * as a circular widget, only reacting on mouse events within the circle;
     * this requires this custom implementation. */
    bool m_isMouseEventActive = false; // TODO Remove me!
    /** @brief Pointer to RgbColorSpace() object */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;

    // Member functions
    [[nodiscard]] QSize calculateImageSizePhysical() const;
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
    [[nodiscard]] LchDouble fromWidgetPixelPositionToColor(const QPoint widgetPixelPosition) const;
    [[nodiscard]] bool isWidgetPixelPositionInGamut(const QPoint widgetPixelPosition) const;
    [[nodiscard]] int leftBorderPhysical() const;
    [[nodiscard]] PerceptualColor::LchDouble nearestInGamutColorByAdjustingChromaLightness(const double chroma, const double lightness);
    [[nodiscard]] std::optional<QPoint> nearestInGamutPixelPosition(const QPoint originalPixelPosition);
    [[nodiscard]] static std::optional<QPoint>
    nearestNeighborSearch(const QPoint point, const QRect searchRectangle, const std::function<bool(const QPoint)> &doesPointExist);
    void setCurrentColorFromWidgetPixelPosition(const QPoint widgetPixelPosition);

private:
    Q_DISABLE_COPY(ChromaLightnessDiagramPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaLightnessDiagram> q_pointer;
};

} // namespace PerceptualColor

#endif // CHROMALIGHTNESSDIAGRAM_P_H
