// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "wheelcolorpicker.h"
// Second, the private implementation.
#include "wheelcolorpicker_p.h" // IWYU pragma: associated

#include "abstractdiagram.h"
#include "chromalightnessdiagram.h"
#include "chromalightnessdiagram_p.h" // IWYU pragma: keep
#include "cielchd50values.h"
#include "colorwheel.h"
#include "colorwheel_p.h" // IWYU pragma: keep
#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "helperconstants.h"
#include "rgbcolorspace.h"
#include <math.h>
#include <qapplication.h>
#include <qmath.h>
#include <qobject.h>
#include <qpoint.h>
#include <qpointer.h>
#include <qrect.h>
#include <qsharedpointer.h>
#include <utility>
class QResizeEvent;
class QWidget;

namespace PerceptualColor
{
/** @brief Constructor
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
WheelColorPicker::WheelColorPicker(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new WheelColorPickerPrivate(this))
{
    d_pointer->m_rgbColorSpace = colorSpace;
    d_pointer->m_colorWheel = new ColorWheel(colorSpace, this);
    d_pointer->m_chromaLightnessDiagram = new ChromaLightnessDiagram(
        // Same color space for this widget:
        colorSpace,
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
            GenericColor lch = d_pointer->m_chromaLightnessDiagram->currentColorCielchD50();
            lch.third = newHue;
            // We have to be sure that the color is in-gamut also for the
            // new hue. If it is not, we adjust it:
            lch = d_pointer->m_rgbColorSpace->reduceCielchD50ChromaToFitIntoGamut(lch);
            d_pointer->m_chromaLightnessDiagram->setCurrentColorCielchD50(lch);
        });
    connect(d_pointer->m_chromaLightnessDiagram,
            &ChromaLightnessDiagram::currentColorCielchD50Changed,
            this,
            // As value is stored anyway within ChromaLightnessDiagram member,
            // it’s enough to just emit the corresponding signal of this class:
            &WheelColorPicker::currentColorCielchD50Changed);
    connect(
        // QWidget’s constructor requires a QApplication object. As this
        // is a class derived from QWidget, calling qApp is safe here.
        qApp,
        &QApplication::focusChanged,
        d_pointer.get(), // Without .get() apparently connect() won’t work…
        &WheelColorPickerPrivate::handleFocusChanged);

    // Initial color
    setCurrentColorCielchD50(
        // Though CielchD50Values::srgbVersatileInitialColor() is expected to
        // be in-gamut, its more secure to guarantee this explicitly:
        d_pointer->m_rgbColorSpace->reduceCielchD50ChromaToFitIntoGamut(
            // Default sRGB initial color:
            CielchD50Values::srgbVersatileInitialColor));
}

/** @brief Default destructor */
WheelColorPicker::~WheelColorPicker() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
WheelColorPickerPrivate::WheelColorPickerPrivate(WheelColorPicker *backLink)
    : q_pointer(backLink)
{
}

/** Repaint @ref m_colorWheel when focus changes
 * on @ref m_chromaLightnessDiagram
 *
 * @ref m_chromaLightnessDiagram is the focus proxy of @ref m_colorWheel.
 * Both show a focus indicator when keyboard focus is active. But
 * apparently @ref m_colorWheel does not always repaint when focus
 * changes. Therefore, this slot can be connected to the <tt>qApp</tt>’s
 * <tt>focusChanged()</tt> signal to make sure that the repaint works.
 *
 * @note It might be an alternative to write an event filter
 * for @ref m_chromaLightnessDiagram to do the same work. The event
 * filter could be either @ref WheelColorPicker or
 * @ref WheelColorPickerPrivate (the last case means that
 * @ref WheelColorPickerPrivate would still have to inherit from
 * <tt>QObject</tt>). But that would probably be more complicate… */
void WheelColorPickerPrivate::handleFocusChanged(QWidget *old, QWidget *now)
{
    if ((old == m_chromaLightnessDiagram) || (now == m_chromaLightnessDiagram)) {
        m_colorWheel->update();
    }
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event */
void WheelColorPicker::resizeEvent(QResizeEvent *event)
{
    AbstractDiagram::resizeEvent(event);
    d_pointer->resizeChildWidgets();
}

/** @brief Calculate the optimal size for the inner diagram.
 *
 * @returns The maximum possible size of the diagram within the
 * inner part of the color wheel. With floating point precision.
 * Measured in <em>device-independent pixels</em>. */
QSizeF WheelColorPickerPrivate::optimalChromaLightnessDiagramSize() const
{
    /** The outer dimensions of the widget are a rectangle within a
     * circumscribed circled, which is the inner border of the color wheel.
     *
     * The widget size is composed by the size of the diagram itself and
     * the size of the borders. The border size is fixed; only the diagram
     * size can vary.
     *
     * Known variables:
     * | variable     | comment                          | value                              |
     * | :----------- | :------------------------------- | :--------------------------------- |
     * | r            | relation b ÷ a                   | maximum lightness ÷ maximum chroma |
     * | h            | horizontal shift                 | left + right diagram border        |
     * | v            | vertical shift                   | top + bottom diagram border        |
     * | d            | diameter of circumscribed circle | inner diameter of the color wheel  |
     * | b            | diagram height                   | a × r                              |
     * | widgetWidth  | widget width                     | a + h                              |
     * | widgetHeight | widget height                    | b + v                              |
     * | a            | diagram width                    | ?                                  |
     */
    const qreal r = 100.0 / m_rgbColorSpace->profileMaximumCielchD50Chroma();
    const qreal h = m_chromaLightnessDiagram->d_pointer->leftBorderPhysical() //
        + m_chromaLightnessDiagram->d_pointer->defaultBorderPhysical();
    const qreal v = 2 * m_chromaLightnessDiagram->d_pointer->defaultBorderPhysical();
    const qreal d = m_colorWheel->d_pointer->innerDiameter();

    /** We can calculate <em>a</em> because right-angled triangle
     * with <em>a</em> and with <em>b</em> as legs/catheti will have
     * has hypotenuse the diameter of the circumscribed circle:
     *
     * <em>[The following formula requires a working Internet connection
     * to be displayed.]</em>
     *
     * @f[
        \begin{align}
            widgetWidth²
            + widgetHeight²
            = & d²
        \\
            (a+h)²
            + (b+v)²
            = & d²
        \\
            (a+h)²
            + (ra+v)²
            = & d²
        \\
            a²
            + 2ah
            + h²
            + r²a²
            + 2rav
            + v²
            = & d²
        \\
            a²
            + r²a²
            + 2ah
            + 2rav
            + h²
            + v²
            = & d²
        \\
            (1+r²)a²
            + 2a(h+rv)
            + (h²+v²)
            = & d²
        \\
            a²
            + 2a\frac{h+rv}{1+r²}
            + \frac{h²+v²}{1+r²}
            = & \frac{d²}{1+r²}
        \\
            a²
            + 2a\frac{h+rv}{1+r²}
            + \left(\frac{h+rv}{1+r²}\right)^{2}
            - \left(\frac{h+rv}{1+r²}\right)^{2}
            + \frac{h²+v²}{1+r²}
            = &  \frac{d²}{1+r²}
        \\
            \left(a+\frac{h+rv}{1+r²}\right)^{2}
            - \left(\frac{h+rv}{1+r²}\right)^{2}
            + \frac{h²+v²}{1+r²}
            = & \frac{d²}{1+r²}
        \\
            \left(a+\frac{h+rv}{1+r²}\right)^{2}
            = & \frac{d²}{1+r²}
            + \left(\frac{h+rv}{1+r²}\right)^{2}
            - \frac{h²+v²}{1+r²}
        \\
            a
            + \frac{h+rv}{1+r²}
            = & \sqrt{
                \frac{d²}{1+r²}
                + \left(\frac{h+rv}{1+r²}\right)^{2}
                -\frac{h²+v²}{1+r²}
            }
        \\
            a
            = & \sqrt{
                \frac{d²}{1+r²}
                + \left(\frac{h+rv}{1+r²}\right)^{2}
                - \frac{h²+v²}{1+r²}
            }
            - \frac{h+rv}{1+r²}
        \end{align}
     * @f] */
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

/** @brief Update the geometry of the child widgets.
 *
 * This widget does <em>not</em> use layout management for its child widgets.
 * Therefore, this function should be called on all resize events of this
 * widget.
 *
 * @post The geometry (size and the position) of the child widgets are
 * adapted according to the current size of <em>this</em> widget itself. */
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
GenericColor WheelColorPicker::currentColorCielchD50() const
{
    return d_pointer->m_chromaLightnessDiagram->currentColorCielchD50();
}

/** @brief Setter for the @ref currentColorCielchD50() property.
 *
 * @param newCurrentColorCielchD50 the new color */
void WheelColorPicker::setCurrentColorCielchD50(const GenericColor &newCurrentColorCielchD50)
{
    // The following line will also emit the signal of this class:
    d_pointer->m_chromaLightnessDiagram->setCurrentColorCielchD50(newCurrentColorCielchD50);

    // Avoid that setting the new hue will move the color into gamut.
    // (As documented, this function accepts happily out-of-gamut colors.)
    QSignalBlocker myBlocker(d_pointer->m_colorWheel);
    d_pointer->m_colorWheel->setHue(d_pointer->m_chromaLightnessDiagram->currentColorCielchD50().third);
}

/** @brief Recommended size for the widget
 *
 * Reimplemented from base class.
 *wh
 * @returns Recommended size for the widget.
 *
 * @sa @ref sizeHint() */
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

/** @brief Recommended minimum size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize WheelColorPicker::sizeHint() const
{
    return minimumSizeHint() * scaleFromMinumumSizeHintToSizeHint;
}

} // namespace PerceptualColor
