// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromalightnessdiagram.h"
// Second, the private implementation.
#include "chromalightnessdiagram_p.h" // IWYU pragma: associated

#include "perceptualcolor-0/abstractdiagram.h"
#include "perceptualcolor-0/constpropagatinguniquepointer.h"
#include "perceptualcolor-0/lchdouble.h"
#include "cielchvalues.h"
#include "constpropagatingrawpointer.h"
#include "helperconstants.h"
#include "rgbcolorspace.h"
#include <memory>
#include <optional>
#include <qcolor.h>
#include <qevent.h>
#include <qimage.h>
#include <qlist.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpen.h>
#include <qpoint.h>
#include <qrect.h>
#include <qrgb.h>
#include <qsizepolicy.h>
#include <qwidget.h>

namespace PerceptualColor
{
/** @brief The constructor.
 *
 * @param colorSpace The color space within which the widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 *
 * @param parent Passed to the QWidget base class constructor */
ChromaLightnessDiagram::ChromaLightnessDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new ChromaLightnessDiagramPrivate(this))
{
    // Setup the color space must be the first thing to do because
    // other operations rely on a working color space.
    d_pointer->m_rgbColorSpace = colorSpace;

    // Initialization
    d_pointer->m_currentColor = CielchValues::srgbVersatileInitialColor;
    setFocusPolicy(Qt::FocusPolicy::StrongFocus);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    d_pointer->m_chromaLightnessImageParameters.imageSizePhysical = //
        d_pointer->calculateImageSizePhysical();
    d_pointer->m_chromaLightnessImageParameters.rgbColorSpace = colorSpace;
    d_pointer->m_chromaLightnessImage.setImageParameters( //
        d_pointer->m_chromaLightnessImageParameters);

    // Connections
    connect(&d_pointer->m_chromaLightnessImage, //
            &AsyncImageProvider<ChromaLightnessImageParameters>::interlacingPassCompleted, //
            this, //
            &ChromaLightnessDiagram::callUpdate);
}

/** @brief Default destructor */
ChromaLightnessDiagram::~ChromaLightnessDiagram() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
ChromaLightnessDiagramPrivate::ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink)
    : q_pointer(backLink)
{
}

/** @brief Updates @ref ChromaLightnessDiagram::currentColor corresponding
 * to the given widget pixel position.
 *
 * @param widgetPixelPosition The position of a pixel within the widget’s
 * coordinate system. This does not necessarily need to intersect with the
 * actually displayed diagram or the gamut. It might even be negative or
 * outside the widget.
 *
 * @post If the pixel position is within the gamut, then the corresponding
 * @ref ChromaLightnessDiagram::currentColor is set. If the pixel position
 * is outside the gamut, than a nearby in-gamut color is set (hue is
 * preserved, chroma and lightness are adjusted). Exception: If the
 * widget is so small that no diagram is displayed, nothing will happen. */
void ChromaLightnessDiagramPrivate::setCurrentColorFromWidgetPixelPosition(const QPoint widgetPixelPosition)
{
    const LchDouble color = fromWidgetPixelPositionToColor(widgetPixelPosition);
    q_pointer->setCurrentColor(
        // Search for the nearest color without changing the hue:
        nearestInGamutColorByAdjustingChromaLightness(color.c, color.l));
}

/** @brief The border between the widget outer top, right and bottom
 * border and the diagram itself.
 *
 * The diagram is not painted on the whole extend of the widget.
 * A border is left to allow that the selection handle can be painted
 * completely even when a pixel on the border of the diagram is
 * selected.
 *
 * This is the value for the top, right and bottom border. For the left
 * border, see @ref leftBorderPhysical() instead.
 *
 * Measured in <em>physical pixels</em>. */
int ChromaLightnessDiagramPrivate::defaultBorderPhysical() const
{
    const qreal border = q_pointer->handleRadius() //
        + q_pointer->handleOutlineThickness() / 2.0;
    return qCeil(border * q_pointer->devicePixelRatioF());
}

/** @brief The left border between the widget outer left border and the
 * diagram itself.
 *
 * The diagram is not painted on the whole extend of the widget.
 * A border is left to allow that the selection handle can be painted
 * completely even when a pixel on the border of the diagram is
 * selected. Also, there is space left for the focus indicator.
 *
 * This is the value for the left border. For the other three borders,
 * see @ref defaultBorderPhysical() instead.
 *
 * Measured in <em>physical pixels</em>. */
int ChromaLightnessDiagramPrivate::leftBorderPhysical() const
{
    const int focusIndicatorThickness = qCeil( //
        q_pointer->handleOutlineThickness() * q_pointer->devicePixelRatioF());

    // Candidate 1:
    const int candidateOne = defaultBorderPhysical() + focusIndicatorThickness;

    // Candidate 2: Generally recommended value for focus indicator:
    const int candidateTwo = qCeil( //
        q_pointer->spaceForFocusIndicator() * q_pointer->devicePixelRatioF());

    return qMax(candidateOne, candidateTwo);
}

/** @brief Calculate a size for @ref m_chromaLightnessImage that corresponds
 * to the current widget size.
 *
 * @returns The size for @ref m_chromaLightnessImage that corresponds
 * to the current widget size. Measured in <em>physical pixels</em>. */
QSize ChromaLightnessDiagramPrivate::calculateImageSizePhysical() const
{
    const QSize borderSizePhysical(
        // Borders:
        leftBorderPhysical() + defaultBorderPhysical(), // left + right
        2 * defaultBorderPhysical() // top + bottom
    );
    return q_pointer->physicalPixelSize() - borderSizePhysical;
}

/** @brief Converts widget pixel positions to color.
 *
 * @param widgetPixelPosition The position of a pixel of the widget
 * coordinate system. The given value  does not necessarily need to
 * be within the actual displayed widget. It might even be negative.
 *
 * @returns The corresponding color for the (center of the) given
 * widget pixel position. (The value is not normalized. It might have
 * a negative C value if the position is on the left of the diagram,
 * or an L value smaller than 0 or bigger than 100…) Exception: If
 * the widget is too small to show a diagram, a default color is
 * returned.
 *
 * @sa @ref measurementdetails */
LchDouble ChromaLightnessDiagramPrivate::fromWidgetPixelPositionToColor(const QPoint widgetPixelPosition) const
{
    const QPointF offset(leftBorderPhysical(), defaultBorderPhysical());
    const QPointF imageCoordinatePoint = widgetPixelPosition
        // Offset to pass from widget reference system
        // to image reference system:
        - offset / q_pointer->devicePixelRatioF()
        // Offset to pass from pixel positions to coordinate points:
        + QPointF(0.5, 0.5);
    LchDouble color;
    color.h = m_currentColor.h;
    const qreal diagramHeight = //
        calculateImageSizePhysical().height() / q_pointer->devicePixelRatioF();
    if (diagramHeight > 0) {
        color.l = imageCoordinatePoint.y() * 100.0 / diagramHeight * (-1.0) + 100.0;
        color.c = imageCoordinatePoint.x() * 100.0 / diagramHeight;
    } else {
        color.l = 50;
        color.c = 0;
    }
    return color;
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * Does not differentiate between left, middle and right mouse click.
 *
 * If the mouse moves inside the <em>displayed</em> gamut, the handle
 * is displaced there. If the mouse moves outside the <em>displayed</em>
 * gamut, the handle is displaced to a nearby in-gamut color.
 *
 * @param event The corresponding mouse event
 *
 * @internal
 *
 * @todo This widget reacts on mouse press events also when they occur
 * within the border. It might be nice if it would not. On the other
 * hand: The border is small. Would it really be worth the pain to
 * implement this? */
void ChromaLightnessDiagram::mousePressEvent(QMouseEvent *event)
{
    d_pointer->m_isMouseEventActive = true;
    d_pointer->setCurrentColorFromWidgetPixelPosition(event->pos());
    if (d_pointer->isWidgetPixelPositionInGamut(event->pos())) {
        setCursor(Qt::BlankCursor);
    } else {
        unsetCursor();
    }
}

/** @brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * If the mouse moves inside the <em>displayed</em> gamut, the handle
 * is displaced there. If the mouse moves outside the <em>displayed</em>
 * gamut, the handle is displaced to a nearby in-gamut color.
 *
 * @param event The corresponding mouse event */
void ChromaLightnessDiagram::mouseMoveEvent(QMouseEvent *event)
{
    d_pointer->setCurrentColorFromWidgetPixelPosition(event->pos());
    if (d_pointer->isWidgetPixelPositionInGamut(event->pos())) {
        setCursor(Qt::BlankCursor);
    } else {
        unsetCursor();
    }
}

/** @brief React on a mouse release event.
 *
 * Reimplemented from base class. Does not differentiate between left,
 * middle and right mouse click.
 *
 * If the mouse moves inside the <em>displayed</em> gamut, the handle
 * is displaced there. If the mouse moves outside the <em>displayed</em>
 * gamut, the handle is displaced to a nearby in-gamut color.
 *
 * @param event The corresponding mouse event */
void ChromaLightnessDiagram::mouseReleaseEvent(QMouseEvent *event)
{
    d_pointer->setCurrentColorFromWidgetPixelPosition(event->pos());
    unsetCursor();
}

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * @param event the paint event */
void ChromaLightnessDiagram::paintEvent(QPaintEvent *event)
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
    QImage paintBuffer(physicalPixelSize(), //
                       QImage::Format_ARGB32_Premultiplied);
    paintBuffer.fill(Qt::transparent);
    QPainter painter(&paintBuffer);
    QPen pen;
    painter.setRenderHint(QPainter::Antialiasing, false);

    // Paint the diagram itself.
    // Request image update. If the cache is not up-to-date, this
    // will trigger a new paint event, once the cache has been updated.
    d_pointer->m_chromaLightnessImage.refreshAsync();
    const QColor myNeutralGray = //
        d_pointer->m_rgbColorSpace->toQRgbBound(CielchValues::neutralGray);
    painter.setPen(Qt::NoPen);
    painter.setBrush(myNeutralGray);
    const auto imageSize = //
        d_pointer->m_chromaLightnessImage.imageParameters().imageSizePhysical;
    painter.drawRect( // Paint diagram background
                      // Operating in physical pixels:
        d_pointer->leftBorderPhysical(), // x position (top-left)
        d_pointer->defaultBorderPhysical(), // y position (top-left));
        imageSize.width(),
        imageSize.height());
    painter.drawImage( // Paint the diagram itself as available in the cache.
                       // Operating in physical pixels:
        d_pointer->leftBorderPhysical(), // x position (top-left)
        d_pointer->defaultBorderPhysical(), // y position (top-left)
        d_pointer->m_chromaLightnessImage.getCache() // image
    );

    // Paint a focus indicator.
    //
    // We could paint a focus indicator (round or rectangular) around the
    // handle. Depending on the currently selected hue for the diagram,
    // it looks ugly because the colors of focus indicator and diagram
    // do not harmonize, or it is mostly invisible the the colors are
    // similar. So this approach does not work well.
    //
    // It seems better to paint a focus indicator for the whole widget.
    // We could use the style primitives to paint a rectangular focus
    // indicator around the whole widget:
    //
    // style()->drawPrimitive(
    //     QStyle::PE_FrameFocusRect,
    //     &option,
    //     &painter,
    //     this
    // );
    //
    // However, this does not work well because the chroma-lightness
    // diagram has usually a triangular shape. The style primitive, however,
    // often paints just a line at the bottom of the widget. That does not
    // look good. An alternative approach is that we paint ourselves a focus
    // indicator only on the left of the diagram (which is the place of
    // black/gray/white, so the won't be any problems with non-harmonic
    // colors).
    //
    // Then we have to design the line that we want to display. It is better
    // to do that ourselves instead of relying on generic QStyle::PE_Frame or
    // similar solutions as their result seems to be quite unpredictable across
    // various styles. So we use handleOutlineThickness as line width and
    // paint it at the left-most possible position.
    if (hasFocus()) {
        pen = QPen();
        pen.setWidthF(handleOutlineThickness() * devicePixelRatioF());
        pen.setColor(focusIndicatorColor());
        pen.setCapStyle(Qt::PenCapStyle::FlatCap);
        painter.setPen(pen);
        painter.setRenderHint(QPainter::Antialiasing, true);
        const QPointF pointOne(
            // x:
            handleOutlineThickness() * devicePixelRatioF() / 2.0,
            // y:
            0 + d_pointer->defaultBorderPhysical());
        const QPointF pointTwo(
            // x:
            handleOutlineThickness() * devicePixelRatioF() / 2.0,
            // y:
            physicalPixelSize().height() - d_pointer->defaultBorderPhysical());
        painter.drawLine(pointOne, pointTwo);
    }

    // Paint the handle on-the-fly.
    const int diagramHeight = d_pointer->calculateImageSizePhysical().height();
    QPointF colorCoordinatePoint = QPointF(
        // x:
        d_pointer->m_currentColor.c * diagramHeight / 100.0,
        // y:
        d_pointer->m_currentColor.l * diagramHeight / 100.0 * (-1) + diagramHeight);
    colorCoordinatePoint += QPointF(
        // horizontal offset:
        d_pointer->leftBorderPhysical(),
        // vertical offset:
        d_pointer->defaultBorderPhysical());
    pen = QPen();
    pen.setWidthF(handleOutlineThickness() * devicePixelRatioF());
    pen.setColor(handleColorFromBackgroundLightness(d_pointer->m_currentColor.l));
    painter.setPen(pen);
    painter.setBrush(Qt::NoBrush);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.drawEllipse(colorCoordinatePoint, // center
                        handleRadius() * devicePixelRatioF(), // x radius
                        handleRadius() * devicePixelRatioF() // y radius
    );

    // Paint the buffer to the actual widget
    paintBuffer.setDevicePixelRatio(devicePixelRatioF());
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(QPainter::Antialiasing, true);
    widgetPainter.drawImage(0, 0, paintBuffer);
}

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 *  When the arrow keys are pressed, it moves the
 * handle a small step into the desired direction.
 * When <tt>Qt::Key_PageUp</tt>, <tt>Qt::Key_PageDown</tt>,
 * <tt>Qt::Key_Home</tt> or <tt>Qt::Key_End</tt> are pressed, it moves the
 * handle a big step into the desired direction.
 *
 * Other key events are forwarded to the base class.
 *
 * @param event the event
 *
 * @internal
 *
 * @todo Is the current behaviour (when pressing right arrow while yet
 * at the right border of the gamut, also the lightness is adjusted to
 * allow moving actually to the right) really a good idea? Anyway, it
 * has a bug, and arrow-down does not work on blue hues because the
 * gamut has some sort of corner, and there, the curser blocks. */
void ChromaLightnessDiagram::keyPressEvent(QKeyEvent *event)
{
    LchDouble temp = d_pointer->m_currentColor;
    switch (event->key()) {
    case Qt::Key_Up:
        temp.l += singleStepLightness;
        break;
    case Qt::Key_Down:
        temp.l -= singleStepLightness;
        break;
    case Qt::Key_Left:
        temp.c = qMax<double>(0, temp.c - singleStepChroma);
        break;
    case Qt::Key_Right:
        temp.c += singleStepChroma;
        temp = d_pointer->m_rgbColorSpace->reduceChromaToFitIntoGamut(temp);
        break;
    case Qt::Key_PageUp:
        temp.l += pageStepLightness;
        break;
    case Qt::Key_PageDown:
        temp.l -= pageStepLightness;
        break;
    case Qt::Key_Home:
        temp.c += pageStepChroma;
        temp = d_pointer->m_rgbColorSpace->reduceChromaToFitIntoGamut(temp);
        break;
    case Qt::Key_End:
        temp.c = qMax<double>(0, temp.c - pageStepChroma);
        break;
    default:
        // Quote from Qt documentation:
        //
        //     “If you reimplement this handler, it is very important that
        //      you call the base class implementation if you do not act
        //      upon the key.
        //
        //      The default implementation closes popup widgets if the
        //      user presses the key sequence for QKeySequence::Cancel
        //      (typically the Escape key). Otherwise the event is
        //      ignored, so that the widget’s parent can interpret it.“
        QWidget::keyPressEvent(event);
        return;
    }
    // Here we reach only if the key has been recognized. If not, in the
    // default branch of the switch statement, we would have passed the
    // keyPressEvent yet to the parent and returned.

    // Set the new color (only takes effect when the color is indeed different).
    setCurrentColor(
        // Search for the nearest color without changing the hue:
        d_pointer->m_rgbColorSpace->reduceChromaToFitIntoGamut(temp));
    // TODO Instead of this, simply do setCurrentColor(temp); but guarantee
    // for up, down, page-up and page-down that the lightness is raised
    // or reduced until fitting into the gamut. Maybe find a way to share
    // code with reduceChromaToFitIntoGamut ?
}

/** @brief Tests if a given widget pixel position is within
 * the <em>displayed</em> gamut.
 *
 * @param widgetPixelPosition The position of a pixel of the widget coordinate
 * system. The given value  does not necessarily need to be within the
 * actual displayed diagram or even the gamut itself. It might even be
 * negative.
 *
 * @returns <tt>true</tt> if the widget pixel position is within the
 * <em>currently displayed gamut</em>. Otherwise <tt>false</tt>.
 *
 * @internal
 *
 * @todo How does isInGamut() react? Does it also control valid chroma
 * and lightness ranges? */
bool ChromaLightnessDiagramPrivate::isWidgetPixelPositionInGamut(const QPoint widgetPixelPosition) const
{
    if (calculateImageSizePhysical().isEmpty()) {
        // If there is no displayed gamut, the answer must be false.
        // But fromWidgetPixelPositionToColor() would return an in-gamut
        // fallback color nevertheless. Therefore, we have to catch
        // the special case with an empty diagram here manually.
        return false;
    }

    const LchDouble color = fromWidgetPixelPositionToColor(widgetPixelPosition);

    // Test if C is in range. This is important because a negative C value
    // can be in-gamut, but is not in the _displayed_ gamut.
    if (color.c < 0) {
        return false;
    }

    // Actually for in-gamut color:
    return m_rgbColorSpace->isInGamut(color);
}

/** @brief Setter for the @ref currentColor() property.
 *
 * @param newCurrentColor the new @ref currentColor
 *
 * @todo When an out-of-gamut color is given, both lightness and chroma
 * are adjusted. But does this really make sense? In @ref WheelColorPicker,
 * when using the hue wheel, also <em>both</em>, lightness <em>and</em> chroma
 * will change. Isn’t that confusing? */
void ChromaLightnessDiagram::setCurrentColor(const PerceptualColor::LchDouble &newCurrentColor)
{
    if (newCurrentColor.hasSameCoordinates(d_pointer->m_currentColor)) {
        return;
    }

    double oldHue = d_pointer->m_currentColor.h;
    d_pointer->m_currentColor = newCurrentColor;
    if (d_pointer->m_currentColor.h != oldHue) {
        // Update the diagram (only if the hue has changed):
        d_pointer->m_chromaLightnessImageParameters.hue = //
            d_pointer->m_currentColor.h;
        d_pointer->m_chromaLightnessImage.setImageParameters( //
            d_pointer->m_chromaLightnessImageParameters);
    }
    update(); // Schedule a paint event
    Q_EMIT currentColorChanged(newCurrentColor);
}

/** @brief React on a resize event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding event */
void ChromaLightnessDiagram::resizeEvent(QResizeEvent *event)
{
    Q_UNUSED(event)
    d_pointer->m_chromaLightnessImageParameters.imageSizePhysical = //
        d_pointer->calculateImageSizePhysical();
    d_pointer->m_chromaLightnessImage.setImageParameters( //
        d_pointer->m_chromaLightnessImageParameters);
    // As by Qt documentation:
    //     “The widget will be erased and receive a paint event
    //      immediately after processing the resize event. No drawing
    //      need be (or should be) done inside this handler.”
}

/** @brief Recommended size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize ChromaLightnessDiagram::sizeHint() const
{
    return minimumSizeHint() * scaleFromMinumumSizeHintToSizeHint;
}

/** @brief Recommended minimum size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref sizeHint() */
QSize ChromaLightnessDiagram::minimumSizeHint() const
{
    const int minimumHeight = qRound(
        // Top border and bottom border:
        2.0 * d_pointer->defaultBorderPhysical() / devicePixelRatioF()
        // Add the height for the diagram:
        + gradientMinimumLength());
    const int minimumWidth = qRound(
        // Left border and right border:
        (d_pointer->leftBorderPhysical() + d_pointer->defaultBorderPhysical()) / devicePixelRatioF()
        // Add the gradient minimum length from y axis, multiplied with
        // the factor to allow at correct scaling showing up the whole
        // chroma range of the gamut.
        + gradientMinimumLength() * d_pointer->m_rgbColorSpace->profileMaximumCielchChroma() / 100.0);
    // Expand to the global minimum size for GUI elements
    return QSize(minimumWidth, minimumHeight);
}

// No documentation here (documentation of properties
// and its getters are in the header)
LchDouble PerceptualColor::ChromaLightnessDiagram::currentColor() const
{
    return d_pointer->m_currentColor;
}

/** @brief An abstract Nearest-neighbor-search algorithm.
 *
 * There are many different solutions for
 * <a href="https://en.wikipedia.org/wiki/Nearest_neighbor_search">
 * Nearest-neighbor-searches</a>. This one is not naive, but still quite easy
 * to implement. It is based on <a href="https://stackoverflow.com/a/307523">
 * this Stackoverflow answer</a>.
 *
 * @param point The point to which the nearest neighbor is searched.
 * @param searchRectangle The rectangle within which the algorithm searches
 *        for a nearest neighbor. All points outside this rectangle are
 *        ignored.
 * @param doesPointExist A callback function that must return <tt>true</tt>
 *        for points that are considered to exist, and <tt>false</tt> for
 *        points that are considered to no exist. This callback function will
 *        never be called with points outside the search rectangle.
 * @returns The nearest neighbor, if any. An empty value otherwise. If there
 *          are multiple non-transparent pixels at the same distance, it is
 *          indeterminate which one is returned.  Note that the point itself is
 *          considered to be itself its nearest neighbor if it is within the
 *          search rectangle and considered by the test function to exist. */
std::optional<QPoint>
ChromaLightnessDiagramPrivate::nearestNeighborSearch(const QPoint point, const QRect searchRectangle, const std::function<bool(const QPoint)> &doesPointExist)
{
    if (!searchRectangle.isValid()) {
        return std::nullopt;
    }
    // A valid QRect is non-empty, as described by QRect documentation…

    // Test for special case:
    // originalPixelPosition itself is within the image and non-transparent
    if (searchRectangle.contains(point)) {
        if (doesPointExist(point)) {
            return point;
        }
    }

    // We search the perimeter of a square that we keep moving out one pixel
    // at a time from the original point (“offset”).

    const auto hDistanceFromRect = distanceFromRange(searchRectangle.left(), //
                                                     point.x(),
                                                     searchRectangle.right());
    const auto vDistanceFromRect = distanceFromRange(searchRectangle.top(), //
                                                     point.y(),
                                                     searchRectangle.bottom());
    // As described at https://stackoverflow.com/a/307523:
    // An offset of “0” means that only the original point itself is searched
    // for. This is inefficient, because all eight search points will be
    // identical for an offset of “0”. And because we test yet for the
    // original point itself as a special case above, we can start here with
    // an offset ≥ 0.
    const auto initialOffset = qMax(1, //
                                    qMax(hDistanceFromRect, vDistanceFromRect));
    const auto hMaxDistance = qMax(qAbs(point.x() - searchRectangle.left()), //
                                   qAbs(point.x() - searchRectangle.right()));
    const auto vMaxDistance = qMax(qAbs(point.y() - searchRectangle.top()), //
                                   qAbs(point.y() - searchRectangle.bottom()));
    const auto maximumOffset = qMax(hMaxDistance, vMaxDistance);
    std::optional<QPoint> nearestPointTillNow;
    int nearestPointTillNowDistanceSquare = 0;
    qreal nearestPointTillNowDistance = 0.0;
    QPoint searchPoint;
    auto searchPointOffsets = [](int i, int j) -> QList<QPoint> {
        return QList<QPoint>({
            QPoint(i, j), // right
            QPoint(i, -j), // right
            QPoint(-i, j), // left
            QPoint(-i, -j), // left
            QPoint(j, i), // bottom
            QPoint(-j, i), // bottom
            QPoint(j, -i), // top
            QPoint(-j, -i) // top
        });
    };
    int i;
    int j;
    // As described at https://stackoverflow.com/a/307523:
    // The search starts at the four points that intersect the axes and moves
    // one pixel at a time towards the corners. (We have have 8 moving search
    // points). As soon as we locate an existing point, there is no need to
    // continue towards the corners, as the remaining points are all further
    // from the original point.
    for (i = initialOffset; //
         (i <= maximumOffset) && (!nearestPointTillNow.has_value()); //
         ++i //
    ) {
        for (j = 0; (j <= i) && (!nearestPointTillNow.has_value()); ++j) {
            for (QPoint &temp : searchPointOffsets(i, j)) {
                // TODO A possible optimization might be to not always use all
                // eight search points. Imagine you have an original point
                // that is outside the image, at its left side. The search
                // point on the left line of the search perimeter rectangle
                // will always be out-of-boundary, so there is no need
                // to calculate the search points, just to find out later
                // that these points are outside the searchRectangle. But
                // how could an elegant implementation look like?
                searchPoint = point + temp;
                if (searchRectangle.contains(searchPoint)) {
                    if (doesPointExist(searchPoint)) {
                        nearestPointTillNow = searchPoint;
                        nearestPointTillNowDistanceSquare = //
                            temp.rx() * temp.rx() + temp.ry() * temp.ry();
                        nearestPointTillNowDistance = qSqrt( //
                            nearestPointTillNowDistanceSquare);
                        break;
                    }
                }
            }
        }
    }

    if (!nearestPointTillNow.has_value()) {
        // There is not one single pixel that is valid in the
        // whole searchRectangle.
        return nearestPointTillNow;
    }

    i += 1;
    qreal maximumJ;
    // After the initial search for the nearest-neighbor-point, we must
    // continue to search the perimeter of wider squares until we reach an
    // offset of “nearestPointTillNowDistance”. However, the search points
    // no longer have to travel ("j") all the way to the corners: They can
    // stop when they reach a pixel that is farther away from the original
    // point than the current "nearest-neighbor-point" candidate."
    for (; i < nearestPointTillNowDistance; ++i) {
        maximumJ = qSqrt(nearestPointTillNowDistanceSquare - i * i);
        for (j = 0; j < maximumJ; ++j) {
            for (QPoint &temp : searchPointOffsets(i, j)) {
                searchPoint = point + temp;
                if (searchRectangle.contains(searchPoint)) {
                    if (doesPointExist(searchPoint)) {
                        nearestPointTillNow = searchPoint;
                        nearestPointTillNowDistanceSquare = //
                            temp.rx() * temp.rx() + temp.ry() * temp.ry();
                        nearestPointTillNowDistance = qSqrt( //
                            nearestPointTillNowDistanceSquare);
                        maximumJ = qSqrt( //
                            nearestPointTillNowDistanceSquare - i * i);
                        break;
                    }
                }
            }
        }
    }

    return nearestPointTillNow;
}

/** @brief Search the nearest in-gamut neighbor pixel.
 *
 * @param originalPixelPosition The pixel for which you search the nearest
 * neighbor, expressed in the coordinate system of the image. This pixel may
 * be inside or outside the image.
 * @returns The nearest non-transparent pixel of @ref m_chromaLightnessImage,
 *          if any. An empty value otherwise. If there are multiple
 *          non-transparent pixels at the same distance, it is
 *          indeterminate which one is returned. Note that the point itself
 *          is considered to be itself its nearest neighbor if it is within
 *          the image and non-transparent.
 *
 * @note This function waits until a full-quality @ref m_chromaLightnessImage
 * is available, which might take some time.
 *
 * @todo A possible optimization might be to search initially, after a new
 * image is available, entire columns, starting from the right, until we hit
 * the first column that has a non-transparent pixel. This information can be
 * used to reduce the search rectangle significantly. */
std::optional<QPoint> ChromaLightnessDiagramPrivate::nearestInGamutPixelPosition(const QPoint originalPixelPosition)
{
    m_chromaLightnessImage.refreshSync();
    const auto upToDateImage = m_chromaLightnessImage.getCache();

    auto isOpaqueFunction = [&upToDateImage](const QPoint point) -> bool {
        return (qAlpha(upToDateImage.pixel(point)) != 0);
    };
    return nearestNeighborSearch(originalPixelPosition, //
                                 QRect(QPoint(0, 0), upToDateImage.size()), //
                                 isOpaqueFunction);
}

/** @brief Find the nearest in-gamut pixel.
 *
 * The hue is assumed to be the current hue at @ref m_currentColor.
 * Chroma and lightness are sacrificed, but the hue is preserved. This function
 * works at the precision of the current @ref m_chromaLightnessImage.
 *
 * @param chroma Chroma of the original color.
 *
 * @param lightness Lightness of the original color.
 *
 * @note This function waits until a full-quality @ref m_chromaLightnessImage
 * is available, which might take some time.
 *
 * @returns The nearest in-gamut pixel with the same hue as the original
 * color. */
PerceptualColor::LchDouble ChromaLightnessDiagramPrivate::nearestInGamutColorByAdjustingChromaLightness(const double chroma, const double lightness)
{
    // Initialization
    LchDouble temp;
    temp.l = lightness;
    temp.c = chroma;
    temp.h = m_currentColor.h;
    if (temp.c < 0) {
        temp.c = 0;
    }

    // Return is we are within the gamut.
    // NOTE Calling isInGamut() is slower than simply testing for the pixel,
    // it is more exact.
    if (m_rgbColorSpace->isInGamut(temp)) {
        return temp;
    }

    const auto imageHeight = calculateImageSizePhysical().height();
    QPoint myPixelPosition( //
        qRound(temp.c * (imageHeight - 1) / 100.0),
        qRound(imageHeight - 1 - temp.l * (imageHeight - 1) / 100.0));

    myPixelPosition = //
        nearestInGamutPixelPosition(myPixelPosition).value_or(QPoint(0, 0));
    LchDouble result = temp;
    result.c = myPixelPosition.x() * 100.0 / (imageHeight - 1);
    result.l = 100 - myPixelPosition.y() * 100.0 / (imageHeight - 1);
    return result;
}

} // namespace PerceptualColor
