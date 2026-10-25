// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "abstractdiagram.h"
// Second, the private implementation.
#include "abstractdiagram_p.h" // IWYU pragma: associated

#include "absolutecolor.h"
#include "helper.h"
#include "lchvalues.h"
#include <qapplication.h>
#include <qcolor.h>
#include <qevent.h>
#include <qglobal.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qpalette.h>
#include <qsize.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qwidget.h>
class QHideEvent;
class QShowEvent;

namespace PerceptualColor
{
/*!
 * \brief The constructor.
 *
 * \a parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
AbstractDiagram::AbstractDiagram(QWidget *parent)
    : QWidget(parent)
    , d_pointer(new AbstractDiagramPrivate())
{
}

/*!
 * \brief Destructor */
AbstractDiagram::~AbstractDiagram() noexcept
{
}

/*!
 * \brief Default destructor
 */
AbstractDiagramPrivate::~AbstractDiagramPrivate() noexcept = default;

/*!
 * \brief The color for painting focus indicators
 * Returns the color for painting focus indicators. This color is based on
 * the current widget style at the moment this function is called. The value
 * might therefore be different on the next function call, if the widget style
 * has been switched by the user in the meantime.
 * \note As there is no build-in support in Qt to get this information, we
 * have to do some best guess, which might go wrong on some styles. */
QColor AbstractDiagram::focusIndicatorColor() const
{
    return palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Highlight);
}

/*!
 * \brief The rounded size of the widget measured in
 * \e {physical pixels}.
 *
 * Returns the rounded size of this widget,
 * measured in \e {physical pixels}, based on
 * \c {QPaintDevice::devicePixelRatioF()}. This is the recommended
 * image size for calling \c {QPainter::drawImage()} during a paint event.
 * Both, width and height are guaranteed to be ≥ 0.
 *
 * Example: You want to prepare a \c {QImage} of the hole widget to be
 * used in \c {QWidget::paintEvent()}. To make sure a crisp rendering,
 * you have to
 *
 * \list
 *     \li Prepare an image with the size that this function returns.
 *     \li Set \c {QImage::setDevicePixelRatio()} of the image to the same
 *         value as \c {QPaintDevice::devicePixelRatioF()} of the widget.
 *     \li Actually paint the image on the widget at
 *         position \c {(0, 0)} \e {without} anti-aliasing.
 * \endlist
 *
 * \note If \c {QPaintDevice::devicePixelRatioF()} is not an integer,
 * the result of this function is rounded down. Qt’s widget geometry code
 * has no documentation about how this is handled. However, Qt seems to
 * round up starting with 0.5, at least on Linux/X11. But there are a few
 * themes (for example the “Kvantum style engine” with the style
 * “MildGradientKvantum”) that seem to round down: This becomes visible, as
 * the corresponding last physical pixels are not automatically redrawn before
 * executing the \c {paintEvent()} code. To avoid relying on undocumented
 * behaviour and to avoid known problems with some styles, this function
 * is conservative and always rounds down. */
QSize AbstractDiagram::physicalPixelSize() const
{
    // Assert that static_cast<int> always rounds down.
    static_assert(static_cast<int>(1.9) == 1);
    static_assert(static_cast<int>(1.5) == 1);
    static_assert(static_cast<int>(1.0) == 1);
    // Multiply the size with the (floating point) scale factor
    // and than round down (by using static_cast<int>).
    const int width = static_cast<int>(size().width() * devicePixelRatioF());
    const int height = static_cast<int>(size().height() * devicePixelRatioF());
    return QSize(qMax(width, 0), qMax(height, 0));
}

/*!
 * \brief The maximum possible size of a square within the widget, measured
 * in \e {physical pixels}.
 *
 * This is the shorter value of width and height of the widget.
 *
 * Returns the maximum possible size of a square within the widget, measured
 * in \e {physical pixels}. Both, width and height are guaranteed
 * to be ≥ 0.
 *
 * \sa maximumWidgetSquareSize
 */
int AbstractDiagram::maximumPhysicalSquareSize() const
{
    return qMin(physicalPixelSize().width(), physicalPixelSize().height());
}

/*!
 * \brief The maximum possible size of a square within the widget, measured
 * in \e {device-independent pixels}.
 *
 * This is the conversion of \l maximumPhysicalSquareSize to the unit
 * \e {device-independent pixels}. It might be \e {smaller} than
 * the shortest value of \c {QWidget::width()} and
 * \c {QWidget::height()} because \l maximumPhysicalSquareSize
 * might have rounded down.
 *
 * Returns the maximum possible size of a square within the widget, measured
 * in \e {device-independent pixels}. */
qreal AbstractDiagram::maximumWidgetSquareSize() const
{
    return (maximumPhysicalSquareSize() / devicePixelRatioF());
}

/*!
 * \brief The outline thickness of a handle.
 *
 * Returns the outline thickness of a (either circular or linear) handle.
 * Measured in \e {device-independent pixels}.
 *
 * \note The return value is constant. For a given object instance, this
 * function returns the same value every time it is called. This constant
 * value may be different for different instances of the object.
 */
int AbstractDiagram::handleOutlineThickness() const
{
    return 2;
}

/*!
 * \brief The radius of a circular handle.
 *
 * Returns the radius of a circular handle, measured in
 * \e {device-independent pixels}.
 *
 * \note The return value is constant. For a given object instance, this
 * function returns the same value every time it is called. This constant
 * value may be different for different instances of the object.
 */
qreal AbstractDiagram::handleRadius() const
{
    return handleOutlineThickness() * 2.5;
}

/*!
 * \brief The thickness of a color gradient.
 *
 * This is the thickness of a one-dimensional gradient, for example in
 * a slider or a color wheel.
 *
 * Returns the thickness of a slider or a color wheel, measured in
 * \e {device-independent pixels}.
 *
 * \sa gradientMinimumLength()
 */
int AbstractDiagram::gradientThickness() const
{
    ensurePolished();
    int result = 0;
    QStyleOptionSlider styleOption;
    styleOption.initFrom(this); // Sets also QStyle::State_MouseOver
    styleOption.orientation = Qt::Horizontal;
    result = qMax(result, style()->pixelMetric(QStyle::PM_SliderThickness, &styleOption, this));
    styleOption.orientation = Qt::Vertical;
    result = qMax(result, style()->pixelMetric(QStyle::PM_SliderThickness, &styleOption, this));
    result = qMax(result, qRound(handleRadius()));
    // No supplementary space for ticks is added.
    return result;
}

/*!
 * \brief The minimum length of a color gradient.
 *
 * This is the minimum length of a one-dimensional gradient, for example in
 * a slider or a color wheel. This is also the minimum width and minimum
 * height of two-dimensional gradients.
 *
 * Returns the length of a gradient, measured in
 * \e {device-independent pixels}.
 *
 * \sa gradientThickness()
 */
int AbstractDiagram::gradientMinimumLength() const
{
    ensurePolished();
    QStyleOptionSlider option;
    option.initFrom(this);
    return qMax(
        // Parameter: style-based value:
        qMax(
            // Similar to QSlider sizeHint():
            84,
            // Similar to QSlider::minimumSizeHint():
            style()->pixelMetric(QStyle::PM_SliderLength, &option, this)),
        // Parameter:
        gradientThickness());
}

/*!
 * \brief The empty space around diagrams reserved for the focus indicator.
 *
 * Measured in \e {device-independent pixels}.
 *
 * Returns the empty space around diagrams reserved for the focus
 * indicator. */
int AbstractDiagram::spaceForFocusIndicator() const
{
    // 1 × handleOutlineThickness() for the focus indicator itself.
    // 2 × handleOutlineThickness() for the space between the focus indicator
    // and the diagram.
    return 3 * handleOutlineThickness();
}

/*!
 * \brief An appropriate color for a handle, depending on the background
 * lightness.
 *
 * \a lightness The background lightness. Valid range: \c {[0, 100]}
 * for \l LchSpace::CielchD50 and \c {[0, 100]} for \l LchSpace::Oklch.
 *
 * \a projectionSpace The color space into which the gamut will be
 * projected.
 *
 * Returns an appropriate color for a handle. This color will provide
 * contrast to the background. */
QColor AbstractDiagram::handleColorFromBackgroundLightness(qreal lightness, PerceptualColor::LchSpace projectionSpace)
{
    const auto values = (projectionSpace == LchSpace::CielchD50) //
        ? cielchD50Values //
        : oklchValues;
    if (lightness >= (values.maximumLightness * 0.5)) {
        return Qt::black;
    }
    return Qt::white;
}

/*!
 * \brief If this widget is actually visible.
 *
 * Unlike \c {QWidget::isVisible}, minimized windows are \e {not}
 * considered visible by this function.
 *
 * Changes can be observed with
 * \l AbstractDiagram::actualVisibilityToggledEvent.
 *
 * Returns if this widget is actually visible.
 *
 *
 *
 * This information is based on the last \l AbstractDiagram::showEvent
 * or \l AbstractDiagram::hideEvent that was received. */
bool AbstractDiagram::isActuallyVisible() const
{
    return d_pointer->m_isActuallyVisible;
}

/*!
 * \brief Event occurring after \l isActuallyVisible has been toggled.
 *
 * This function is called if and only if \l isActuallyVisible has
 * actually been toggled. */
void AbstractDiagram::actualVisibilityToggledEvent()
{
}

/*!
 * \brief React on a show event.
 *
 * Reimplemented from base class.
 *
 * \a eventParameter The show event.
 *
 *
 *
 * \sa AbstractDiagram::isActuallyVisible
 */
void AbstractDiagram::showEvent(QShowEvent *eventParameter)
{
    QWidget::showEvent(eventParameter);
    if (d_pointer->m_isActuallyVisible == false) {
        d_pointer->m_isActuallyVisible = true;
        actualVisibilityToggledEvent();
    }
}

/*!
 * \brief React on a hide event.
 *
 * Reimplemented from base class.
 *
 * \a eventParameter The hide event.
 *
 *
 *
 * \sa AbstractDiagram::isActuallyVisible
 */
void AbstractDiagram::hideEvent(QHideEvent *eventParameter)
{
    QWidget::hideEvent(eventParameter);
    if (d_pointer->m_isActuallyVisible == true) {
        d_pointer->m_isActuallyVisible = false;
        actualVisibilityToggledEvent();
    }
}

/*!
 * \brief An alternative to QWidget::update(). It’s a workaround
 * that avoids trouble with overload resolution.
 *
 * Connecting a signal to the slot
 * \l {https://doc.qt.io/qt-6/qwidget.html#update} {QWidget::update()}
 * is surprisingly difficult, at least if you want to use the functor
 * syntax (which provides compile-time checks) for the connection. A simple
 * connection fails to compile because it fails to do a correct  overload
 * resolution, as there is more than one slot called \c {update}. Now,
 * \l {https://doc.qt.io/qt-6/qoverload.html#qOverload} {qOverload}
 * can be used to choose the correct overload, but in some
 * case, it might generate compiler warnings.
 *
 * Instead of connecting to
 * \l {https://doc.qt.io/qt-6/qwidget.html#update} {QWidget::update()}
 * directly, simply connect to this slot instead. It calls
 * the actual \l {https://doc.qt.io/qt-6/qwidget.html#update}
 * {QWidget::update()}, but avoids the annoyance with the overload
 * resolution.
 */
void AbstractDiagram::callUpdate()
{
    update();
}

/*!
 * \brief A neutral gray color for the diagram background, returned as an
 * sRGB value.
 *
 * Returns a neutral gray color for the diagram background, returned as an
 * sRGB value.
 *
 *
 *
 * \note This value corresponds to \l LchValues::neutralGray() from
 * \l oklchValues, converted to sRGB. Using a consistent neutral gray across
 * all widgets is desirable, regardless of which \l LchSpace they operate in.
 */
QColor AbstractDiagram::neutralGray()
{
    static const QColor v = AbsoluteColor::fastFromOklabToSRgbOrTransparent( //
        AbsoluteColor::fromPolarToCartesian(oklchValues.neutralGray()));
    return v;
}

/*!
 * \brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * \note When overriding this function in a subclass, make sure to
 * call the base class implementation (AbstractDiagram::mousePressEvent).
 *
 * \a eventParameter The corresponding mouse event
 */
void AbstractDiagram::mousePressEvent(QMouseEvent *eventParameter)
{
    if (eventParameter->button() == Qt::LeftButton)
        d_pointer->m_dragStartPosition = eventParameter->pos();
    QWidget::mousePressEvent(eventParameter);
}

/*!
 * \brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * \note When overriding this function in a subclass, make sure to
 * call the base class implementation (AbstractDiagram::mouseMoveEvent).
 *
 * \a eventParameter The corresponding mouse event
 */
void AbstractDiagram::mouseMoveEvent(QMouseEvent *eventParameter)
{
    if (eventParameter->buttons() & Qt::LeftButton) {
        // Distance since the left mouse buttons was originally clicked.
        const auto vector = eventParameter->pos() - d_pointer->m_dragStartPosition;
        const auto distanceSquare = //
            vector.x() * vector.x() + vector.y() * vector.y();
        const auto refSquare = QApplication::startDragDistance() //
            * QApplication::startDragDistance();
        if (distanceSquare >= refSquare) {
            // NOTE Calling execDrag() here does not trigger a new drag
            // operation on every minor mouse movement. Once QDrag::exec()
            // has been invoked, Qt suspends further mouse move events until
            // the drag-and-drop sequence has finished. Only after the drag
            // completes will normal mouse events resume.
            execDrag(d_pointer->m_dragStartPosition);
        }
    }
    // NOTE Intentionally not calling the parent’s class’ implementation to
    // avoid that on Breeze style, instead of drag-and-drop, sometimes
    // the window gets moved.
}

/*!
 * \brief Initiates an outgoing drag operation.
 *
 * This virtual function simplifies support for drag-and-drop.
 * It is called when the user presses the left mouse button and
 * moves the cursor beyond the platform-defined drag threshold.
 *
 * To implement drag support, reimplement this function in a subclass:
 *
 * \list
 *     \li Create a QDrag object on the heap, using the widget as parent.
 *     \li Configure the drag object (e.g. set mime data, pixmap).
 *     \li Call QDrag::exec() to start the drag. Qt will take ownership
 *         of the QDrag object; do not delete it manually.
 * \endlist
 *
 * \a startPosition The position where the drag gesture began.
 */
void AbstractDiagram::execDrag(QPoint startPosition)
{
    Q_UNUSED(startPosition)
}

/*!
 * \brief Main event handler.
 *
 * Reimplemented from base class.
 *
 * \a eventParameter The event to be processed.
 *
 * Returns true if the event was recognized, otherwise
 * it returns false.
 *
 *
 *
 * \note This is a dummy reimplementation, provided to ensure future
 * extensibility while preserving binary compatibility, as
 * \l {https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#You_should...}
 * {recommended by the KDE binary compatibility policy}.
 * The policy states that
 * \l {https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#Adding_a_reimplemented_virtual_function}
 * {“you can safely reimplement a virtual function defined in one of the base
 * classes only if it is safe that the programs linked with the prior version
 * call the implementation in the base class rather than the derived one.”}.
 * Consistent behavior across versions would be difficult to guarantee.
 * By reimplementing the main event handler now,
 * we retain the option to extend it later to process new event types
 * without introducing binary compatibility issues. Since this is the
 * central event dispatcher, it can eventually handle all event types,
 * eliminating the need for dummy overrides of each specialized handler.
 */
bool AbstractDiagram::event(QEvent *eventParameter)
{
    return QWidget::event(eventParameter);
}

/*!
 * \brief Handle state changes.
 *
 * Reimplemented from base class.
 *
 * \a eventParameter The event.
 */
void AbstractDiagram::changeEvent(QEvent *eventParameter)
{
    const auto type = eventParameter->type();

    if (isThemeChange(type)) {
        update();
    }

    QWidget::changeEvent(eventParameter);
}

} // namespace PerceptualColor
