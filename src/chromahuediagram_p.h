// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CHROMAHUEDIAGRAM_P_H
#define CHROMAHUEDIAGRAM_P_H

// Include the header of the public class of this private implementation.
// #include "chromahuediagram.h"

#include "asyncimageprovider.h"
#include "chromahueimageparameters.h"
#include "colorwheelimage.h"
#include "constpropagatingrawpointer.h"
#include "genericcolor.h"
#include "lcms2.h"
#include <qglobal.h>
#include <qpoint.h>
#include <qsharedpointer.h>

namespace PerceptualColor
{
class ChromaHueDiagram;
class RgbColorSpace;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom
 *
 * @anchor ChromaHueMeasurement <b>Measurement details:</b> The general
 * @ref measurementdetails "notes about measurement" apply also to this widget.
 * Furthermore, in this widget, when painting a pixel of the gamut, the color
 * of the pixel will be the color of the coordinate point at the center of the
 * pixel. So the pixel at position <tt>QPoint(x, y)</tt> gets the color that
 * corresponds to the coordinate point <tt>QPoint(x+0.5, y+0.5)</tt>.
 * Also, mouse events work with pixel position; so when reacting on mouse
 * events than it’s the center of the given mouse event pixel position that is
 * considered when processing the mouse event. */
class ChromaHueDiagramPrivate
{
public:
    ChromaHueDiagramPrivate(ChromaHueDiagram *backLink, const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ChromaHueDiagramPrivate() noexcept = default;

    // Member variables
    /** @brief The image of the chroma-hue diagram itself. */
    AsyncImageProvider<ChromaHueImageParameters> m_chromaHueImage;
    /** @brief Properties for @ref m_chromaHueImage. */
    ChromaHueImageParameters m_chromaHueImageParameters;
    /** @brief Internal storage of the @ref ChromaHueDiagram::currentColorCielchD50()
     * property */
    GenericColor m_currentColorCielchD50;
    /** @brief Holds if currently a mouse event is active or not.
     *
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on
     *   a @ref ChromaHueDiagram::mousePressEvent()
     *   done within the gamut diagram. The value is set to <tt>true</tt>.
     * - While active, all @ref ChromaHueDiagram::mouseMoveEvent() will
     *   move the diagram’s color handle.
     * - Once a @ref ChromaHueDiagram::mouseReleaseEvent() occurs, the value
     *   is set to <tt>false</tt>. Further mouse movements will not move the
     *   handle anymore.
     *
     * This is done because Qt’s default mouse tracking reacts on all clicks
     * within the whole widget. However, <em>this</em> widget is meant as a
     * circular widget, only reacting on mouse events within the circle;
     * this requires this custom implementation. */
    bool m_isMouseEventActive = false;
    /** @brief Pointer to @ref RgbColorSpace object used to describe the
     * color space. */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief The image of the color wheel. */
    ColorWheelImage m_wheelImage;

    // Member functions
    [[nodiscard]] int diagramBorder() const;
    [[nodiscard]] QPointF diagramCenter() const;
    [[nodiscard]] qreal diagramOffset() const;
    [[nodiscard]] cmsCIELab fromWidgetPixelPositionToLab(const QPoint position) const;
    [[nodiscard]] bool isWidgetPixelPositionWithinMouseSensibleCircle(const QPoint widgetCoordinates) const;
    void setColorFromWidgetPixelPosition(const QPoint position);
    [[nodiscard]] QPointF widgetCoordinatesFromCurrentColorCielchD50() const;

private:
    Q_DISABLE_COPY(ChromaHueDiagramPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaHueDiagram> q_pointer;
};

} // namespace PerceptualColor

#endif // CHROMAHUEDIAGRAM_P_H
