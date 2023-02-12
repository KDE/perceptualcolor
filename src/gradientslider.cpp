// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "gradientslider.h"
// Second, the private implementation.
#include "gradientslider_p.h" // IWYU pragma: associated

#include "abstractdiagram.h"
#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "gradientimageparameters.h"
#include "helperconstants.h"
#include "lchadouble.h"
#include <helper.h>
#include <memory>
#include <qevent.h>
#include <qguiapplication.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpoint.h>
#include <qsizepolicy.h>
#include <qtransform.h>
#include <qwidget.h>

namespace PerceptualColor
{
/** @brief Constructs a vertical slider.
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 * Can be created with @ref RgbColorSpaceFactory.
 * @param parent parent widget (if any) */
GradientSlider::GradientSlider(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new GradientSliderPrivate(this))
{
    d_pointer->initialize(colorSpace, Qt::Orientation::Vertical);
}

/** @brief Constructs a slider.
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 * @param orientation The orientation parameter determines whether
 * the slider is horizontal or vertical; the valid values
 * are <tt>Qt::Vertical</tt> and <tt>Qt::Horizontal</tt>.
 * @param parent parent widget (if any) */
GradientSlider::GradientSlider(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, Qt::Orientation orientation, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new GradientSliderPrivate(this))
{
    d_pointer->initialize(colorSpace, orientation);
}

/** @brief Default destructor */
GradientSlider::~GradientSlider() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
GradientSliderPrivate::GradientSliderPrivate(GradientSlider *backLink)
    : q_pointer(backLink)
{
}

/** @brief Basic initialization.
 *
 * Code that is shared between the various overloaded constructors
 * of @ref GradientSlider.
 *
 * @note This function requires that @ref q_pointer points to a completely
 * initialized object. Therefore, this function may <em>not</em> be called
 * within the constructor of @ref GradientSliderPrivate because in this
 * moment the @ref GradientSlider object is still not fully initialized.
 * However, a call from the <em>function body</em> of a constructor of
 * @ref GradientSlider should be okay.
 *
 * @param colorSpace the color space
 * @param orientation determines whether the slider is horizontal or
 * vertical */
void GradientSliderPrivate::initialize(const QSharedPointer<RgbColorSpace> &colorSpace, Qt::Orientation orientation)
{
    q_pointer->setFocusPolicy(Qt::StrongFocus);
    m_gradientImageParameters.rgbColorSpace = colorSpace;
    setOrientationWithoutSignalAndForceNewSizePolicy(orientation);
    LchaDouble first;
    first.l = 75;
    first.c = 65;
    first.h = 90;
    first.a = 1;
    LchaDouble second;
    second.l = 50;
    second.c = 75;
    second.h = 45;
    second.a = 1;
    q_pointer->setColors(first, second);

    // Connections
    q_pointer->connect( //
        &m_gradientImage, //
        &AsyncImageProvider<GradientImageParameters>::interlacingPassCompleted, //
        q_pointer,
        &GradientSlider::callUpdate);
}

// No documentation here (documentation of properties
// and its getters are in the header)
LchaDouble GradientSlider::firstColor() const
{
    return d_pointer->m_firstColor;
}

/** @brief Setter for @ref firstColor property.
 *
 * @param newFirstColor the new @ref firstColor */
void GradientSlider::setFirstColor(const PerceptualColor::LchaDouble &newFirstColor)
{
    if (!d_pointer->m_firstColor.hasSameCoordinates(newFirstColor)) {
        d_pointer->m_firstColor = newFirstColor;
        d_pointer->m_gradientImageParameters.setFirstColor(newFirstColor);
        d_pointer->m_gradientImage.setImageParameters( //
            d_pointer->m_gradientImageParameters);
        update();
        Q_EMIT firstColorChanged(newFirstColor);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
LchaDouble GradientSlider::secondColor() const
{
    return d_pointer->m_secondColor;
}

/** @brief Setter for @ref secondColor property.
 *
 * @param newSecondColor the new @ref secondColor */
void GradientSlider::setSecondColor(const PerceptualColor::LchaDouble &newSecondColor)
{
    if (!d_pointer->m_secondColor.hasSameCoordinates(newSecondColor)) {
        d_pointer->m_secondColor = newSecondColor;
        d_pointer->m_gradientImageParameters.setSecondColor(newSecondColor);
        d_pointer->m_gradientImage.setImageParameters( //
            d_pointer->m_gradientImageParameters);
        update();
        Q_EMIT secondColorChanged(newSecondColor);
    }
}

/** @brief Setter for both, @ref firstColor property and @ref secondColor
 * property.
 *
 * @param newFirstColor the new @ref firstColor
 * @param newSecondColor the new @ref secondColor */
void GradientSlider::setColors(const PerceptualColor::LchaDouble &newFirstColor, const PerceptualColor::LchaDouble &newSecondColor)
{
    setFirstColor(newFirstColor);
    setSecondColor(newSecondColor);
    update();
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding resize event */
void GradientSlider::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    d_pointer->m_gradientImageParameters.setGradientLength( //
        d_pointer->physicalPixelLength());
    d_pointer->m_gradientImageParameters.setGradientThickness(
        // Normally, this should not change, but maybe on Hight-DPI
        // devices there might be some differences.
        d_pointer->physicalPixelThickness());
    d_pointer->m_gradientImage.setImageParameters( //
        d_pointer->m_gradientImageParameters);
    update();
}

/** @brief Recommended size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns Recommended size for the widget.
 *
 * @sa @ref sizeHint() */
QSize GradientSlider::sizeHint() const
{
    QSize result = minimumSizeHint();
    if (d_pointer->m_orientation == Qt::Orientation::Horizontal) {
        result.setWidth( //
            qRound(result.width() * scaleFromMinumumSizeHintToSizeHint));
    } else {
        result.setHeight( //
            qRound(result.height() * scaleFromMinumumSizeHintToSizeHint));
    }
    return result;
}

/** @brief Recommended minimum size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize GradientSlider::minimumSizeHint() const
{
    QSize result;
    if (d_pointer->m_orientation == Qt::Orientation::Horizontal) {
        result.setWidth(gradientMinimumLength());
        result.setHeight(gradientThickness());
    } else {
        result.setWidth(gradientThickness());
        result.setHeight(gradientMinimumLength());
    }
    return result;
}

// No documentation here (documentation of properties
// and its getters are in the header)
qreal GradientSlider::singleStep() const
{
    return d_pointer->m_singleStep;
}

/** @brief Setter for @ref singleStep property.
 *
 * @param newSingleStep the new @ref singleStep. Is bound to the valid
 * range of the property. */
void GradientSlider::setSingleStep(qreal newSingleStep)
{
    // Do not use negative value
    const qreal boundedSingleStep = qBound<qreal>(0.0, newSingleStep, 1.0);
    if (boundedSingleStep != d_pointer->m_singleStep) {
        d_pointer->m_singleStep = boundedSingleStep;
        Q_EMIT singleStepChanged(d_pointer->m_singleStep);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
qreal GradientSlider::pageStep() const
{
    return d_pointer->m_pageStep;
}

/** @brief Setter for @ref pageStep property.
 *
 * @param newPageStep the new @ref pageStep. Is bound to the valid
 * range of the property. */
void GradientSlider::setPageStep(qreal newPageStep)
{
    // Do not use negative altkluge
    const qreal boundedNewPageStep = qBound<qreal>(0.0, newPageStep, 1.0);
    if (boundedNewPageStep != d_pointer->m_pageStep) {
        d_pointer->m_pageStep = boundedNewPageStep;
        Q_EMIT pageStepChanged(d_pointer->m_pageStep);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
qreal GradientSlider::value() const
{
    return d_pointer->m_value;
}

/** @brief Setter for @ref value property.
 *
 * @param newValue the new @ref value. Is bound to the valid
 * range of the property. */
void GradientSlider::setValue(qreal newValue)
{
    qreal temp = qBound<qreal>(0, newValue, 1);
    if (d_pointer->m_value != temp) {
        d_pointer->m_value = temp;
        update();
        Q_EMIT valueChanged(temp);
    }
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding mouse event */
void GradientSlider::mousePressEvent(QMouseEvent *event)
{
    setValue(d_pointer->fromWidgetPixelPositionToValue(event->pos()));
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding mouse event */
void GradientSlider::mouseReleaseEvent(QMouseEvent *event)
{
    setValue(d_pointer->fromWidgetPixelPositionToValue(event->pos()));
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding mouse event */
void GradientSlider::mouseMoveEvent(QMouseEvent *event)
{
    setValue(d_pointer->fromWidgetPixelPositionToValue(event->pos()));
}

/** @brief React on a mouse wheel event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding mouse event */
void GradientSlider::wheelEvent(QWheelEvent *event)
{
    qreal steps = standardWheelStepCount(event);
    //  Only react on good old vertical wheels, and not on horizontal wheels
    if (steps != 0) {
        qreal stepSize;
        if ( //
            QGuiApplication::keyboardModifiers().testFlag(Qt::ControlModifier) //
            || QGuiApplication::keyboardModifiers().testFlag(Qt::ShiftModifier) //
        ) {
            stepSize = pageStep();
        } else {
            stepSize = singleStep();
        }
        setValue(d_pointer->m_value + steps * stepSize);
    } else {
        // Don’t accept the event and let it up to the default treatment:
        event->ignore();
    }
}

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 * The user can change the @ref value of this widget by the following
 * key strokes:
 *
 * - Qt::Key_Up and Qt::Key_Plus increments a @ref singleStep.
 * - Qt::Key_Down and Qt::Key_Minus decrements a @ref singleStep.
 * - Qt::Key_Left increments and Qt::Key_Right increment or decrement
 *   a @ref singleStep, depending on the layout direction (LTR or RTL).
 * - Qt::Key_PageUp increments a @ref pageStep
 * - Qt::Key_PageDown decrements a @ref pageStep
 * - Qt::Key_Home increments to the maximum @ref value
 * - Qt::Key_End decrements to the minimum @ref value
 *
 * @param event the event  */
void GradientSlider::keyPressEvent(QKeyEvent *event)
{
    switch (event->key()) {
    case Qt::Key_Up:
    case Qt::Key_Plus:
        setValue(d_pointer->m_value + d_pointer->m_singleStep);
        break;
    case Qt::Key_Down:
    case Qt::Key_Minus:
        setValue(d_pointer->m_value - d_pointer->m_singleStep);
        break;
    case Qt::Key_Left:
        if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
            setValue(d_pointer->m_value - d_pointer->m_singleStep);
        } else {
            setValue(d_pointer->m_value + d_pointer->m_singleStep);
        }
        break;
    case Qt::Key_Right:
        if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
            setValue(d_pointer->m_value + d_pointer->m_singleStep);
        } else {
            setValue(d_pointer->m_value - d_pointer->m_singleStep);
        }
        break;
    case Qt::Key_PageUp:
        setValue(d_pointer->m_value + d_pointer->m_pageStep);
        break;
    case Qt::Key_PageDown:
        setValue(d_pointer->m_value - d_pointer->m_pageStep);
        break;
    case Qt::Key_Home:
        setValue(0);
        break;
    case Qt::Key_End:
        setValue(1);
        break;
    default:
        /* Quote from Qt documentation:
         *
         *     “If you reimplement this handler, it is very important that
         *      you call the base class implementation if you do not act
         *      upon the key.
         *
         *      The default implementation closes popup widgets if the
         *      user presses the key sequence for QKeySequence::Cancel
         *      (typically the Escape key). Otherwise the event is
         *      ignored, so that the widget’s parent can interpret it.“ */
        QWidget::keyPressEvent(event);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
Qt::Orientation GradientSlider::orientation() const
{
    return d_pointer->m_orientation;
}

/** @brief Forces a new orientation and a corresponding size policy.
 *
 * @param newOrientation The new orientation for the widget.
 *
 * @post The new orientation is stored. The signal
 * @ref GradientSlider::orientationChanged is <em>not</em> emitted.
 * The <tt>sizePolicy</tt> property is updated corresponding to the
 * <em>new</em> orientation; this happens even if the new
 * orientation is identical to the old @ref m_orientation! */
void GradientSliderPrivate::setOrientationWithoutSignalAndForceNewSizePolicy(Qt::Orientation newOrientation)
{
    if (newOrientation == Qt::Orientation::Vertical) {
        q_pointer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    } else {
        q_pointer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }
    m_orientation = newOrientation;
    m_gradientImageParameters.setGradientLength(physicalPixelLength());
    m_gradientImageParameters.setGradientThickness(
        // Normally, this should not change, but maybe on Hight-DPI
        // devices there are some differences.
        physicalPixelThickness());
    m_gradientImage.setImageParameters(m_gradientImageParameters);
    // Notify the layout system the the geometry has changed
    q_pointer->updateGeometry();
    q_pointer->update();
}

/** @brief Setter for @ref orientation property.
 *
 * @param newOrientation the new @ref orientation. */
void GradientSlider::setOrientation(Qt::Orientation newOrientation)
{
    if (newOrientation != d_pointer->m_orientation) {
        d_pointer->setOrientationWithoutSignalAndForceNewSizePolicy( //
            newOrientation);
        Q_EMIT orientationChanged(d_pointer->m_orientation);
    }
}

/** @brief The rounded length of the widget
 * measured in <em>physical pixels</em>.
 *
 * @returns The rounded length of the widget
 * measured in <em>physical pixels</em>.
 *
 * This is a convenience function to access
 * @ref GradientSlider::physicalPixelSize(). The length is the
 * size of the widget in the direction of the gradient.
 *
 * @sa @ref physicalPixelThickness() */
int GradientSliderPrivate::physicalPixelLength() const
{
    if (m_orientation == Qt::Orientation::Vertical) {
        return q_pointer->physicalPixelSize().height();
    } else {
        return q_pointer->physicalPixelSize().width();
    }
}

/** @brief The rounded thickness of the widget
 * measured in <em>physical pixels</em>.
 *
 * @returns The rounded thickness of the widget
 * measured in <em>physical pixels</em>.
 *
 * This is a convenience function to access
 * @ref GradientSlider::physicalPixelSize(). The thickness is the
 * size of the widget orthogonal to the direction of the gradient.
 *
 * @sa @ref physicalPixelLength() */
int GradientSliderPrivate::physicalPixelThickness() const
{
    if (m_orientation == Qt::Orientation::Horizontal) {
        return q_pointer->physicalPixelSize().height();
    } else {
        return q_pointer->physicalPixelSize().width();
    }
}

/** @brief Converts widget pixel positions to @ref GradientSlider::value
 * @param pixelPosition The position of a pixel of the widget coordinate
 * system. The given value  does not necessarily need to
 * be within the actual displayed widget. It might even be negative.
 * @returns The corresponding @ref GradientSlider::value for the (center of
 * the) given widget pixel position.
 * @sa @ref measurementdetails */
qreal GradientSliderPrivate::fromWidgetPixelPositionToValue(QPoint pixelPosition)
{
    // We are interested in the point in the middle of the given pixel.
    const QPointF coordinatePoint = pixelPosition + QPointF(0.5, 0.5);
    qreal temp;
    if (m_orientation == Qt::Orientation::Vertical) {
        temp = (q_pointer->size().height() - coordinatePoint.y()) //
            / static_cast<qreal>(q_pointer->size().height());
    } else {
        if (q_pointer->layoutDirection() == Qt::LayoutDirection::LeftToRight) {
            temp = coordinatePoint.x() //
                / static_cast<qreal>(q_pointer->size().width());
        } else {
            temp = (q_pointer->size().width() - coordinatePoint.x()) //
                / static_cast<qreal>(q_pointer->size().width());
        }
    }
    return qBound<qreal>(0, temp, 1);
}

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * @param event the paint event */
void GradientSlider::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)
    // We do not paint directly on the widget, but on a QImage buffer first:
    // Render anti-aliased looks better. But as Qt documentation says:
    //
    //      “Renderhints are used to specify flags to QPainter that may or
    //       may not be respected by any given engine.”
    //
    // Painting here directly on the widget might lead to different
    // anti-aliasing results depending on the underlying window system. This
    // is especially problematic as anti-aliasing might shift or not a pixel
    // to the left or to the right. So we paint on a QImage first. As QImage
    // (at difference to QPixmap and a QWidget) is independent of native
    // platform rendering, it guarantees identical anti-aliasing results on
    // all platforms. Here the quote from QPainter class documentation:
    //
    //      “To get the optimal rendering result using QPainter, you should
    //       use the platform independent QImage as paint device; i.e. using
    //       QImage will ensure that the result has an identical pixel
    //       representation on any platform.”
    QImage paintBuffer;

    // Paint the gradient itself.
    // Make sure the image will be correct. We set length and thickness,
    // just to be sure (we might have missed a resize event). Also,
    // the device pixel ratio float might have changed because the
    // window has been moved to another screen. We do not update the
    // first and the second color because we have complete control
    // about these values and are sure the any changes have yet been
    // applied.
    d_pointer->m_gradientImageParameters.setDevicePixelRatioF( //
        devicePixelRatioF());
    d_pointer->m_gradientImageParameters.setGradientLength( //
        d_pointer->physicalPixelLength());
    d_pointer->m_gradientImageParameters.setGradientThickness(
        // Normally, this should not change, but maybe on Hight-DPI
        // devices there are some differences.
        d_pointer->physicalPixelThickness());
    d_pointer->m_gradientImage.setImageParameters( //
        d_pointer->m_gradientImageParameters);
    d_pointer->m_gradientImage.refreshAsync();
    paintBuffer = d_pointer->m_gradientImage.getCache();
    if (paintBuffer.isNull()) {
        return;
    }

    // Draw slider handle
    QPainter bufferPainter(&paintBuffer);
    // We use antialiasing. As our current handle is just a horizontal or
    // vertical line, it might be slightly sharper without antialiasing.
    // But all other widgets of this library WILL USE antialiasing because
    // their handles are not perfectly horizontal or vertical and without
    // antialiasing they might look terrible. Now, when antialiasing is NOT
    // used, the line thickness is rounded. This would lead to a different
    // thickness in this widget compared to the other widgets. This is not
    // a good idea. Therefore, we USE antialiasing here. Anyway, in practical
    // tests, it seems almost as sharp as without antialiasing, and
    // additionally the position is more exact!
    bufferPainter.setRenderHint(QPainter::Antialiasing, true);
    QPen pen;
    const qreal handleCoordinatePoint = d_pointer->physicalPixelLength() //
        / devicePixelRatioF() //
        * d_pointer->m_value;
    if (hasFocus()) {
        pen.setWidthF(handleOutlineThickness() * 3);
        pen.setColor(focusIndicatorColor());
        bufferPainter.setPen(pen);
        bufferPainter.drawLine(QPointF(handleCoordinatePoint, 0), //
                               QPointF(handleCoordinatePoint, gradientThickness()));
    }
    pen.setWidthF(handleOutlineThickness());
    pen.setColor( //
        handleColorFromBackgroundLightness( //
            d_pointer->m_gradientImageParameters
                .colorFromValue( //
                    d_pointer->m_value)
                .l));
    bufferPainter.setPen(pen);
    bufferPainter.drawLine(QPointF(handleCoordinatePoint, 0), //
                           QPointF(handleCoordinatePoint, gradientThickness()));

    // Paint the buffer to the actual widget
    QTransform transform;
    // The m_gradientImageProvider contains the gradient always
    // in a default form, independent of the actual orientation
    // of this widget and independent of its actual layout direction:
    // In the default form, the first color is always on the left, and the
    // second color is always on the right. To paint it, we have to
    // rotate it if our actual orientation is vertical. And we have to
    // mirror it when our actual layout direction is RTL.
    if (d_pointer->m_orientation == Qt::Orientation::Vertical) {
        if (layoutDirection() == Qt::LayoutDirection::RightToLeft) {
            // Even on vertical gradients, we mirror the image, so that
            // the well-aligned edge of the transparency background is
            // always aligned according to the writing direction.
            transform.scale(-1, 1);
            transform.rotate(270);
            transform.translate(size().height() * (-1), size().width() * (-1));
        } else {
            transform.rotate(270);
            transform.translate(size().height() * (-1), 0);
        }
    } else {
        if (layoutDirection() == Qt::LayoutDirection::RightToLeft) {
            transform.scale(-1, 1);
            transform.translate(size().width() * (-1), 0);
        }
    }
    QPainter widgetPainter(this);
    widgetPainter.setTransform(transform);
    widgetPainter.drawImage(0, 0, paintBuffer);

    //     // TODO Draw a focus rectangle like this?:
    //     widgetPainter.setTransform(QTransform());
    //     if (hasFocus()) {
    //         QStyleOptionFocusRect opt;
    //         opt.palette = palette();
    //         opt.rect = rect();
    //         opt.state = QStyle::State_None |
    //         QStyle::State_KeyboardFocusChange; style()->drawPrimitive(
    //             QStyle::PE_FrameFocusRect,
    //             &opt,
    //             &widgetPainter,
    //             this
    //         );
    //     }
}

} // namespace PerceptualColor
