// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "wheelcolorpicker.h"
// Second, the private implementation.
#include "wheelcolorpicker_p.h" // IWYU pragma: associated

#include "absolutecolor.h"
#include "abstractdiagram.h"
#include "chromalightnessdiagram.h"
#include "chromalightnessdiagram_p.h" // IWYU pragma: keep
#include "colorspaceinfo.h"
#include "colorwheel.h"
#include "colorwheel_p.h" // IWYU pragma: keep
#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "helperconstants.h"
#include "lchvalues.h"
#include <math.h>
#include <qapplication.h>
#include <qmath.h>
#include <qobject.h>
#include <qpoint.h>
#include <qpointer.h>
#include <qrect.h>
#include <utility>
class QResizeEvent;
class QWidget;

namespace PerceptualColor
{
/*!
 *
 *
 * \brief Constructor
 *
 * \a projectionSpace The color space into which the gamut will be
 * projected.
 *
 * \a parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor.
 */
WheelColorPicker::WheelColorPicker(const PerceptualColor::LchSpace projectionSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new WheelColorPickerPrivate(this, projectionSpace))
{
    d_pointer->m_colorWheel = new ColorWheel(projectionSpace, this);
    d_pointer->m_chromaLightnessDiagram = new ChromaLightnessDiagram(
        // Projection into which Lch space
        projectionSpace,
        // This widget is smaller than the color wheel. It will be a child
        // of the color wheel, so that missed mouse or key events will be
        // forwarded to the parent widget (color wheel).
        d_pointer->m_colorWheel);
    d_pointer->m_colorWheel->setFocusProxy(d_pointer->m_chromaLightnessDiagram);
    d_pointer->resizeChildWidgets();

    connect(
        // changes on the color wheel trigger a change in the inner diagram
        d_pointer->m_colorWheel,
        &ColorWheel::hueChanged,
        this,
        [this](const qreal newHue) {
            GenericColor lch = d_pointer->m_chromaLightnessDiagram->currentColorLch();
            lch.third = newHue;
            // We have to be sure that the color is in-gamut also for the
            // new hue. If it is not, we adjust it:
            lch = AbsoluteColor::reduceChromaToFitIntoGamut( //
                lch, //
                d_pointer->m_projectionSpace);
            d_pointer->m_chromaLightnessDiagram->setCurrentColorLch(lch);
        });
    connect(d_pointer->m_chromaLightnessDiagram,
            &ChromaLightnessDiagram::currentColorLchChanged,
            this,
            // As value is stored anyway within ChromaLightnessDiagram member,
            // it’s enough to just emit the corresponding signal of this class:
            &WheelColorPicker::currentColorLchChanged);
    connect(
        // QWidget’s constructor requires a QApplication object. As this
        // is a class derived from QWidget, calling qApp is safe here.
        qApp,
        &QApplication::focusChanged,
        d_pointer.get(), // Without .get() apparently connect() won’t work…
        &WheelColorPickerPrivate::handleFocusChanged);

    // Initial color
    // Though LchValues.srgbVersatileInitialColor() is expected to
    // be in-gamut, its more secure to guarantee this explicitly:
    // Default sRGB initial color:
    const auto versatileColor = d_pointer->m_lchValues.neutralGray();
    const auto reducedVersatileColor = //
        AbsoluteColor::reduceChromaToFitIntoGamut(versatileColor, //
                                                  d_pointer->m_projectionSpace);
    setCurrentColorLch(reducedVersatileColor);
}

/*!
 *\brief Default destructor
 */
WheelColorPicker::~WheelColorPicker() noexcept
{
}

/*!
 * \brief Constructor
 *
 * \a backLink Pointer to the object from which \e {this} object
 * is the private implementation.
 *
 * \a projectionSpace The color space into which the gamut will be
 * projected.
 */
WheelColorPickerPrivate::WheelColorPickerPrivate(WheelColorPicker *backLink, const LchSpace projectionSpace)
    : m_lchValues((projectionSpace == LchSpace::CielchD50) ? cielchD50Values : oklchValues)
    , m_projectionSpace(projectionSpace)
    , q_pointer(backLink)
{
}

/*!
 * \brief Repaint \l m_colorWheel when focus changes
 * on \l m_chromaLightnessDiagram
 *
 * \a old The widget that had the focus before the focus change.
 *
 * \a now The widget that has the focus now, after the focus change.
 *
 * \l m_chromaLightnessDiagram is the focus proxy of \l m_colorWheel.
 * Both show a focus indicator when keyboard focus is active. But
 * apparently \l m_colorWheel does not always repaint when focus
 * changes. Therefore, this slot can be connected to the \c {qApp}’s
 * \c {focusChanged()} signal to make sure that the repaint works.
 *
 * \note It might be an alternative to write an event filter
 * for \l m_chromaLightnessDiagram to do the same work. The event
 * filter could be either \l WheelColorPicker or
 * \l WheelColorPickerPrivate (the last case means that
 * \l WheelColorPickerPrivate would still have to inherit from
 * \c {QObject}). But that would probably be more complicate…
 */
void WheelColorPickerPrivate::handleFocusChanged(const QWidget *old, const QWidget *now)
{
    if ((old == m_chromaLightnessDiagram) || (now == m_chromaLightnessDiagram)) {
        m_colorWheel->update();
    }
}

/*!
 * \brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * \a event The corresponding resize event
 */
void WheelColorPicker::resizeEvent(QResizeEvent *event)
{
    AbstractDiagram::resizeEvent(event);
    d_pointer->resizeChildWidgets();
}

/*!
 * \brief Handle state changes.
 *
 * Implements reaction on \c {QEvent::LanguageChange}.
 *
 * Reimplemented from base class.
 *
 * \a event The event.
 */
void WheelColorPicker::changeEvent(QEvent *event)
{
    const auto type = event->type();

    if (type == QEvent::LanguageChange) {
        // Retranslate all child widgets that actually need to be retranslated:
        {
            QEvent eventForChromaLightnessDiagram(QEvent::LanguageChange);
            QApplication::sendEvent(d_pointer->m_chromaLightnessDiagram, //
                                    &eventForChromaLightnessDiagram);
        }
    }

    AbstractDiagram::changeEvent(event);
}

/*!
 * \brief Calculate the optimal size for the inner diagram.
 *
 * Returns the maximum possible size of the diagram within the
 * inner part of the color wheel. With floating point precision.
 * Measured in \e {device-independent pixels}.
 *
 *  The outer dimensions of the widget form a rectangle inscribed within a
 * circumscribed circle, which corresponds to the inner border of the
 * color wheel.
 *
 * The widget size is composed of the size of the diagram itself and
 * the size of the borders. The border size is fixed; only the diagram
 * size can vary.
 *
 * \section1 Mathematical Model
 *
 * The following variables are used for the calculation:
 *
 *  \table
 *      \header
 *          \li variable
 *          \li comment
 *          \li value
 *      \row
 *          \li r
 *          \li relation b ÷ a
 *          \li maximum lightness ÷ maximum chroma
 *      \row
 *          \li h
 *          \li horizontal shift
 *          \li left + right diagram border
 *      \row
 *          \li v
 *          \li vertical shift
 *          \li top + bottom diagram border
 *      \row
 *          \li d
 *          \li diameter of circumscribed circle
 *          \li inner diameter of the color wheel
 *      \row
 *          \li b
 *          \li diagram height
 *          \li a × r
 *      \row
 *          \li widgetWidth
 *          \li widget width
 *          \li a + h
 *      \row
 *          \li widgetHeight
 *          \li widget height
 *          \li b + v
 *      \row
 *          \li a
 *          \li diagram width
 *          \li ?
 *  \endtable
 *
 * We can calculate \c a because a right-angled triangle with \c a and \c b
 * as its legs (catheti) has the diameter of the circumscribed circle as its
 * hypotenuse:
 *
 * \image innersize.svg "Formula"
 */
QSizeF WheelColorPickerPrivate::optimalChromaLightnessDiagramSize() const
{
    const auto maximumChroma = //
        (m_projectionSpace == LchSpace::CielchD50) //
        ? ColorSpaceInfo::maxCielchD50Chroma() //
        : ColorSpaceInfo::maxOklchChroma();
    const qreal r = m_lchValues.maximumLightness / maximumChroma;
    const qreal h =
        // The left border
        m_chromaLightnessDiagram->d_pointer->leftBorderDeviceIndependent()
        // The right border consists of two parts:
        // 1. The actual border area, which is excluded from the diagram
        //    when painting. No diagram background is drawn here.
        // 2. An additional margin inside the diagram, rendered as part of the
        //    diagram background. This margin ensures that, even at maximum
        //    chroma, the handle remains fully contained within the diagram
        //    area. It also guarantees a small gap between the handle’s edge
        //    and the diagram’s right boundary.
        + m_chromaLightnessDiagram->d_pointer->defaultBorderDeviceIndependant() + m_chromaLightnessDiagram->handleRadius()
        + m_chromaLightnessDiagram->handleOutlineThickness() * 1.5;
    const qreal v = //
        2 * m_chromaLightnessDiagram->d_pointer->defaultBorderDeviceIndependant();
    const qreal d = m_colorWheel->d_pointer->innerDiameter();

    const qreal x = (1 + qPow(r, 2)); // x = 1 + r²
    const qreal a =
        // The square root:
        qSqrt(
            // First fraction:
            d * d / x
            // Second fraction:
            + qPow((h + r * v) / x, 2)
            // Thierd fraction:
            - (h * h + v * v) / x)
        // The part after the square root:
        - (h + r * v) / x;
    const qreal b = r * a;

    return QSizeF(a + h, // width
                  b + v // height
    );
}

/*!
 * \brief Update the geometry of the child widgets.
 *
 * This widget does \e {not} use layout management for its child widgets.
 * Therefore, this function should be called on all resize events of this
 * widget.
 *
 *  The geometry (size and the position) of the child widgets are
 * adapted according to the current size of \e {this} widget itself. */
void WheelColorPickerPrivate::resizeChildWidgets()
{
    // Set new geometry of color wheel. Only the size changes, while the
    // position (which is 0, 0) remains always unchanged.
    m_colorWheel->resize(q_pointer->size());

    // Calculate new size for chroma-lightness-diagram
    const QSizeF widgetSize = optimalChromaLightnessDiagramSize();

    // Calculate new top-left corner position for chroma-lightness-diagram
    // (relative to parent widget)
    const qreal radius = m_colorWheel->maximumWidgetSquareSize() / 2.0;
    const QPointF widgetTopLeftPos(
        // x position
        radius - widgetSize.width() / 2.0,
        // y position:
        radius - widgetSize.height() / 2.0);

    // Correct the new geometry of chroma-lightness-diagram to fit into
    // an integer raster.
    QRectF diagramGeometry(widgetTopLeftPos, widgetSize);
    // We have to round to full integers
    diagramGeometry.setLeft(floor(diagramGeometry.left()));
    diagramGeometry.setTop(ceil(diagramGeometry.top()));
    // Round to smaller coordinates for bottom-right corner:
    diagramGeometry.setRight(ceil(diagramGeometry.right()));
    diagramGeometry.setBottom(floor(diagramGeometry.bottom()));
    // NOTE The rounding might change the ratio (b ÷ a) of the
    // diagram itself. If horizontially not wide enough, there may be a little
    // bit of gamut is not visible at the right of the diagram. There
    // is a simple solution: When rounding, top and bottom are rounded so that
    // the heigh is rounded down, but left and right are rounded so that the
    // width is rounded up. That makes sure that the whole gamut is visible.

    // Apply new geometry
    m_chromaLightnessDiagram->setGeometry(diagramGeometry.toRect());
}

// No documentation here (documentation of properties
// and its getters are in the header)
GenericColor WheelColorPicker::currentColorLch() const
{
    return d_pointer->m_chromaLightnessDiagram->currentColorLch();
}

/*!
 * \brief Setter for the \l currentColorLch() property.
 *
 * \a newCurrentColorLch the new color
 */
void WheelColorPicker::setCurrentColorLch(const GenericColor &newCurrentColorLch)
{
    // The following line will also emit the signal of this class:
    d_pointer->m_chromaLightnessDiagram->setCurrentColorLch(newCurrentColorLch);

    // Avoid that setting the new hue will move the color into gamut.
    // (As documented, this function accepts happily out-of-gamut colors.)
    QSignalBlocker myBlocker(d_pointer->m_colorWheel);
    d_pointer->m_colorWheel->setHue(d_pointer->m_chromaLightnessDiagram->currentColorLch().third);
}

/*!
 * \brief Recommended size for the widget
 *
 * Reimplemented from base class.
 *
 * \sa sizeHint()
 */
QSize WheelColorPicker::minimumSizeHint() const
{
    const QSizeF minimumDiagramSize =
        // Get the minimum size of the chroma-lightness widget.
        d_pointer->m_chromaLightnessDiagram->minimumSizeHint()
        // We have to fit this in a widget pixel raster. But the perfect
        // position might be between two integer coordinates. We might
        // have to shift up to 1 pixel at each of the four margins.
        + QSize(2, 2);
    const int diameterForMinimumDiagramSize =
        // The minimum inner diameter of the color wheel has
        // to be equal (or a little bit bigger) than the
        // diagonal through the chroma-lightness widget.
        qCeil(
            // c = √(a² + b²)
            qSqrt(qPow(minimumDiagramSize.width(), 2) + qPow(minimumDiagramSize.height(), 2)))
        // Add size for the color wheel gradient
        + d_pointer->m_colorWheel->gradientThickness()
        // Add size for the border around the color wheel gradient
        + d_pointer->m_colorWheel->d_pointer->border();
    // Necessary size for this widget so that the diagram fits:
    const QSize sizeForMinimumDiagramSize(diameterForMinimumDiagramSize, // x
                                          diameterForMinimumDiagramSize // y
    );

    return sizeForMinimumDiagramSize
        // Expand to the minimumSizeHint() of the color wheel itself
        .expandedTo(d_pointer->m_colorWheel->minimumSizeHint());
}

/*!
 * \brief Recommended minimum size for the widget.
 *
 * Reimplemented from base class.
 *
 * \sa minimumSizeHint()
 */
QSize WheelColorPicker::sizeHint() const
{
    return minimumSizeHint() * scaleFromMinumumSizeHintToSizeHint;
}

} // namespace PerceptualColor
