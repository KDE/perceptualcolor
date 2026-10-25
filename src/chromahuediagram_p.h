// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_P_H
#define PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_P_H

// Include the header of the public class of this private implementation.
// #include "chromahuediagram.h"

#include "asyncimageprovider.h"
#include "chromahueimageparameters.h"
#include "colorwheelimageparameters.h"
#include "constpropagatingrawpointer.h"
#include "genericcolor.h"
#include "lchvalues.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qpoint.h>

namespace PerceptualColor
{
class ChromaHueDiagram;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 *  \brief Private implementation within the \e {Pointer to
 *  implementation} idiom.
 *
 * \target ChromaHueMeasurement
 *
 * \b {Measurement details:} The
 * general \l {measurementdetails.html} {notes about measurement} apply
 * also to this widget.
 * Furthermore, in this widget, when painting a pixel of the gamut, the color
 * of the pixel will be the color of the coordinate point at the center of the
 * pixel. So the pixel at position \c {QPoint(x, y)} gets the color that
 * corresponds to the coordinate point \c {QPoint(x+0.5, y+0.5)}.
 * Also, mouse events work with pixel position; so when reacting on mouse
 * events than it’s the center of the given mouse event pixel position that is
 * considered when processing the mouse event. */
class ChromaHueDiagramPrivate
{
public:
    ChromaHueDiagramPrivate(ChromaHueDiagram *backLink, const LchSpace projectionSpace);

    virtual ~ChromaHueDiagramPrivate() noexcept;

    // Member variables
    /*!
     * \brief The image of the chroma-hue diagram itself. */
    AsyncImageProvider<ChromaHueImageParameters> m_chromaHueImage;

    /*!
     * \brief Properties for \l m_chromaHueImage. */
    ChromaHueImageParameters m_chromaHueImageParameters;

    /*!
     * \brief Internal storage of the \l ChromaHueDiagram::currentColorLch()
     * property */
    GenericColor m_currentColorLch;

    /*!
     * \brief Holds if currently a mouse event is active or not.
     *
     * Default value is \c {false}.
     *
     * \list
     * \li A mouse event gets typically activated on
     *   a \l ChromaHueDiagram::mousePressEvent()
     *   done within the gamut diagram. The value is set to \c {true}.
     * \li While active, all \l ChromaHueDiagram::mouseMoveEvent() will
     *   move the diagram’s color handle.
     * \li Once a \l ChromaHueDiagram::mouseReleaseEvent() occurs, the value
     *   is set to \c {false}. Further mouse movements will not move the
     *   handle anymore.
     * \endlist
     *
     * This is done because Qt’s default mouse tracking reacts on all clicks
     * within the whole widget. However, \e {this} widget is meant as a
     * circular widget, only reacting on mouse events within the circle;
     * this requires this custom implementation. */
    bool m_isMouseEventActive = false;

    /*!
     * \brief The details corresponding to \l m_projectionSpace.
     */
    const LchValues m_lchValues;

    /*!
     * \brief The color space into which the gamut will be projected.
     *
     * Get more details about the chosen projection space
     * through \l m_lchValues.
     */
    const LchSpace m_projectionSpace;

    /*!
     * \brief The image of the color wheel. */
    ColorWheelImageParameters m_wheelImage;

    // Member functions
    [[nodiscard]] int gamutBorder() const;
    [[nodiscard]] QPointF diagramCenter() const;
    [[nodiscard]] qreal diagramOffset() const;
    [[nodiscard]] GenericColor fromWidgetPixelPositionToLab(const QPoint position) const;
    [[nodiscard]] bool isWidgetPixelPositionWithinGamutCircle(const QPoint position) const;
    [[nodiscard]] bool isWidgetPixelPositionWithinMouseSensibleCircle(const QPoint widgetCoordinates) const;
    void setColorFromWidgetPixelPosition(const QPoint position);
    [[nodiscard]] QPointF widgetCoordinatesFromCurrentColorLch() const;

private:
    Q_DISABLE_COPY(ChromaHueDiagramPrivate)

    /*!
     * \brief Pointer to the object from which \e {this} object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaHueDiagram> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_P_H
