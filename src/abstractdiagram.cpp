// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "abstractdiagram.h"
// Second, the private implementation.
#include "abstractdiagram_p.h" // IWYU pragma: associated

#include "helper.h"
#include <qcolor.h>
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
/** @brief The constructor.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
AbstractDiagram::AbstractDiagram(QWidget *parent)
    : QWidget(parent)
    , d_pointer(new AbstractDiagramPrivate())
{
}

/** @brief Destructor */
AbstractDiagram::~AbstractDiagram() noexcept
{
}

/** @brief The color for painting focus indicators
 * @returns The color for painting focus indicators. This color is based on
 * the current widget style at the moment this function is called. The value
 * might therefore be different on the next function call, if the widget style
 * has been switched by the user in the meantime.
 * @note As there is no build-in support in Qt to get this information, we
 * have to do some best guess, which might go wrong on some styles. */
QColor AbstractDiagram::focusIndicatorColor() const
{
    return palette().color(QPalette::ColorGroup::Active, QPalette::ColorRole::Highlight);
}

/** @brief The rounded size of the widget measured in
 * <em>physical pixels</em>.
 *
 * @returns The rounded size of this widget,
 * measured in <em>physical pixels</em>, based on
 * <tt>QPaintDevice::devicePixelRatioF()</tt>. This is the recommended
 * image size for calling <tt>QPainter::drawImage()</tt> during a paint event.
 * Both, width and height are guaranteed to be ≥ 0.
 *
 * Example: You want to prepare a <tt>QImage</tt> of the hole widget to be
 * used in <tt>QWidget::paintEvent()</tt>. To make sure a crisp rendering,
 * you have to
 *
 * - Prepare an image with the size that this function returns.
 * - Set <tt>QImage::setDevicePixelRatio()</tt> of the image to the same
 *   value as <tt>QPaintDevice::devicePixelRatioF()</tt> of the widget.
 * - Actually paint the image on the widget at position <tt>(0, 0)</tt>
 *   <em>without</em> anti-aliasing.
 *
 * @note If <tt>QPaintDevice::devicePixelRatioF()</tt> is not an integer,
 * the result of this function is rounded down. Qt’s widget geometry code
 * has no documentation about how this is handled. However, Qt seems to
 * round up starting with 0.5, at least on Linux/X11. But there are a few
 * themes (for example the “Kvantum style engine” with the style
 * “MildGradientKvantum”) that seem to round down: This becomes visible, as
 * the corresponding last physical pixels are not automatically redrawn before
 * executing the <tt>paintEvent()</tt> code. To avoid relying on undocumented
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

/** @brief The maximum possible size of a square within the widget, measured
 * in <em>physical pixels</em>.
 *
 * This is the shorter value of width and height of the widget.
 *
 * @returns The maximum possible size of a square within the widget, measured
 * in <em>physical pixels</em>. Both, width and height are guaranteed
 * to be ≥ 0.
 *
 * @sa @ref maximumWidgetSquareSize */
int AbstractDiagram::maximumPhysicalSquareSize() const
{
    return qMin(physicalPixelSize().width(), physicalPixelSize().height());
}

/** @brief The maximum possible size of a square within the widget, measured
 * in <em>device-independent pixels</em>.
 *
 * This is the conversion of @ref maximumPhysicalSquareSize to the unit
 * <em>device-independent pixels</em>. It might be <em>smaller</em> than
 * the shortest value of <tt>QWidget::width()</tt> and
 * <tt>QWidget::height()</tt> because @ref maximumPhysicalSquareSize
 * might have rounded down.
 *
 * @returns The maximum possible size of a square within the widget, measured
 * in <em>device-independent pixels</em>. */
qreal AbstractDiagram::maximumWidgetSquareSize() const
{
    return (maximumPhysicalSquareSize() / devicePixelRatioF());
}

/** @brief Background for semi-transparent colors.
 *
 * When showing a semi-transparent color, there has to be a background
 * on which it is shown. This function provides a suitable background
 * for showcasing a color.
 *
 * Example code (to use within a class that inherits from
 * @ref PerceptualColor::AbstractDiagram):
 * @snippet testabstractdiagram.cpp useTransparencyBackground
 *
 * @returns An image of a mosaic of neutral gray rectangles of different
 * lightness. You can use this as tiles to paint a background.
 *
 * @note The image is considering QWidget::devicePixelRatioF() to deliver
 * crisp (correctly scaled) images also for high-DPI devices.
 * The painting does not use floating point drawing, but rounds
 * to full integers. Therefore, the result is always a sharp image.
 * This function takes care that each square has the same physical pixel
 * size, without scaling errors or anti-aliasing errors.
 *
 * @internal
 * @sa @ref transparencyBackground(qreal devicePixelRatioF)
 * @endinternal */
QImage AbstractDiagram::transparencyBackground() const
{
    return PerceptualColor::transparencyBackground(devicePixelRatioF());
}

/** @brief The outline thickness of a handle.
 *
 * @returns The outline thickness of a (either circular or linear) handle.
 * Measured in <em>device-independent pixels</em>. */
int AbstractDiagram::handleOutlineThickness() const
{
    /** @note The return value is constant. For a given object instance, this
     * function returns the same value every time it is called. This constant
     * value may be different for different instances of the object. */
    return 2;
}

/** @brief The radius of a circular handle.
 * @returns The radius of a circular handle, measured in
 * <em>device-independent pixels</em>. */
qreal AbstractDiagram::handleRadius() const
{
    /** @note The return value is constant. For a given object instance, this
     * function returns the same value every time it is called. This constant
     * value may be different for different instances of the object. */
    return handleOutlineThickness() * 2.5;
}

/** @brief The thickness of a color gradient.
 *
 * This is the thickness of a one-dimensional gradient, for example in
 * a slider or a color wheel.
 *
 * @returns The thickness of a slider or a color wheel, measured in
 * <em>device-independent pixels</em>.
 *
 * @sa @ref gradientMinimumLength() */
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

/** @brief The minimum length of a color gradient.
 *
 * This is the minimum length of a one-dimensional gradient, for example in
 * a slider or a color wheel. This is also the minimum width and minimum
 * height of two-dimensional gradients.
 *
 * @returns The length of a gradient, measured in
 * <em>device-independent pixels</em>.
 *
 * @sa @ref gradientThickness() */
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

/** @brief The empty space around diagrams reserved for the focus indicator.
 *
 * Measured in <em>device-independent pixels</em>.
 *
 * @returns The empty space around diagrams reserved for the focus
 * indicator. */
int AbstractDiagram::spaceForFocusIndicator() const
{
    // 1 × handleOutlineThickness() for the focus indicator itself.
    // 2 × handleOutlineThickness() for the space between the focus indicator
    // and the diagram.
    return 3 * handleOutlineThickness();
}

/** @brief An appropriate color for a handle, depending on the background
 * lightness.
 * @param lightness The background lightness. Valid range: <tt>[0, 100]</tt>.
 * @returns An appropriate color for a handle. This color will provide
 * contrast to the background. */
QColor AbstractDiagram::handleColorFromBackgroundLightness(qreal lightness) const
{
    if (lightness >= 50) {
        return Qt::black;
    }
    return Qt::white;
}

/** @brief If this widget is actually visible.
 *
 * Unlike <tt>QWidget::isVisible</tt>, minimized windows are <em>not</em>
 * considered visible.
 *
 * Changes can be observed with
 * @ref AbstractDiagram::actualVisibilityToggledEvent.
 *
 * @returns If this widget is actually visible.
 *
 * @internal
 *
 * This information is based on the last @ref AbstractDiagram::showEvent
 * or @ref AbstractDiagram::hideEvent that was received. */
bool AbstractDiagram::isActuallyVisible() const
{
    return d_pointer->m_isActuallyVisible;
}

/** @brief Event occurring after @ref isActuallyVisible has been toggled.
 *
 * This function is called if and only if @ref isActuallyVisible has
 * actually been toggled. */
void AbstractDiagram::actualVisibilityToggledEvent()
{
}

/** @brief React on a show event.
 *
 * Reimplemented from base class.
 *
 * @param event The show event.
 *
 * @internal
 *
 * @sa @ref AbstractDiagram::isActuallyVisible */
void AbstractDiagram::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    if (d_pointer->m_isActuallyVisible == false) {
        d_pointer->m_isActuallyVisible = true;
        actualVisibilityToggledEvent();
    }
}

/** @brief React on a hide event.
 *
 * Reimplemented from base class.
 *
 * @param event The hide event.
 *
 * @internal
 *
 * @sa @ref AbstractDiagram::isActuallyVisible */
void AbstractDiagram::hideEvent(QHideEvent *event)
{
    QWidget::hideEvent(event);
    if (d_pointer->m_isActuallyVisible == true) {
        d_pointer->m_isActuallyVisible = false;
        actualVisibilityToggledEvent();
    }
}

/** @brief An alternative to QWidget::update(). It’s a workaround
 * that avoids trouble with overload resolution.
 *
 * Connecting a signal to the slot <tt>
 * <a href="https://doc.qt.io/qt-6/qwidget.html#update">QWidget::update()</a>
 * </tt> is surprisingly difficult, at least if you want to use the functor
 * syntax (which provides compile-time checks) for the connection. A simple
 * connection fails to compile because it fails to do a correct  overload
 * resolution, as there is more than one slot called <tt>update</tt>. Now, <tt>
 * <a href="https://doc.qt.io/qt-6/qtglobal.html#qOverload">qOverload&lt;&gt;()
 * </a></tt> can be used to choose the correct overload, but in this special
 * case, <tt><a href="https://doc.qt.io/qt-6/qtglobal.html#qOverload">
 * qOverload&lt;&gt;()</a></tt> generates compiler warnings.
 *
 * Instead of connecting to <tt>
 * <a href="https://doc.qt.io/qt-6/qwidget.html#update">QWidget::update()
 * </a></tt> directly, simply connect to this slot instead. It calls
 * the actual <tt><a href="https://doc.qt.io/qt-6/qwidget.html#update">
 * QWidget::update()</a></tt>, but avoids the annoyance with the overload
 * resolution */
void AbstractDiagram::callUpdate()
{
    update();
}

} // namespace PerceptualColor
