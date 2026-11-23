// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// We actually delete the PIMPL's copy constructor in the private header using
// Q_DISABLE_COPY(ColorPatchPrivate)
// Nevertheless, cppcheck worries about a default copy constructor:
//     "Class 'ColorPatchPrivate' does not have a copy constructor which is
//     recommended since it has dynamic memory/resource allocation(s).
//     (CWE-398)"
// So we suppress this warning:
// cppcheck-suppress-file noCopyConstructor

// Own headers
// First the interface, which forces the header to be self-contained.
#include "colorpatch.h"
// Second, the private implementation.
#include "colorpatch_p.h" // IWYU pragma: associated

#include "constpropagatinguniquepointer.h"
#include "helper.h"
#include <algorithm>
#include <qapplication.h>
#include <qevent.h>
#include <qfont.h>
#include <qframe.h>
#include <qimage.h>
#include <qlabel.h>
#include <qmath.h>
#include <qminmax.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qstyle.h>
#include <qstyleoption.h>
class QWidget;

namespace PerceptualColor
{
/*!
 * \brief Constructor
 *
 * \a parent The parent of the widget, if any
 */
ColorPatch::ColorPatch(QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new ColorPatchPrivate(this))
{
    setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);

    // Trigger paint events whenever the mouse enters or leaves the widget.
    // This is important on some QStyle who might paint widgets different then.
    // For example, Breeze and Oxygen draw a focus indicator on mouse hover
    // even when the widget still not got the focus.
    setAttribute(Qt::WA_Hover);
}

/*!
 * \brief Destructor */
ColorPatch::~ColorPatch() noexcept
{
}

/*!
 * \brief Constructor
 *
 * \a backLink Pointer to the object from which \e {this} object
 * is the private implementation. */
ColorPatchPrivate::ColorPatchPrivate(ColorPatch *backLink)
    : q_pointer(backLink)
{
}

/*!
 * \brief Destructor
 */
ColorPatchPrivate::~ColorPatchPrivate() noexcept
{
}

/*!
 * \brief Provide the size hint.
 *
 * Reimplemented from base class.
 *
 * Returns the size hint
 *
 * \sa minimumSizeHint()
 */
QSize ColorPatch::sizeHint() const
{
    return minimumSizeHint();
}

/*!
 * \brief Returns the minimum size for color rendering.
 */
QSize ColorPatchPrivate::referenceSize() const
{
    // Use a size similar to a QToolButton with an icon (and without text)
    q_pointer->ensurePolished();
    QStyleOptionToolButton option;
    option.initFrom(q_pointer.toPointerToConstObject());
    option.font = q_pointer->font();
    const int iconSize = q_pointer->style()->pixelMetric( //
        QStyle::PM_ButtonIconSize,
        nullptr,
        q_pointer.toPointerToConstObject());
    option.iconSize = QSize(iconSize, iconSize);
    return q_pointer->style()->sizeFromContents( //
        QStyle::CT_ToolButton,
        &option,
        option.iconSize,
        q_pointer.toPointerToConstObject());
}

/*!
 * \brief Provide the minimum size hint.
 *
 * Reimplemented from base class.
 *
 * Returns the minimum size hint
 *
 * \sa sizeHint()
 */
QSize ColorPatch::minimumSizeHint() const
{
    const auto margins = d_pointer->contentMargins();
    const QSize marginSize = QSize(margins.left() + margins.right(), //
                                   margins.top() + margins.bottom());
    return d_pointer->referenceSize() + marginSize;
}

/*!
 * \brief Updates \l m_pixmap if necessary.
 *
 * \a size The new size of the pixmap.
 */
void ColorPatchPrivate::updatePixmapIfNecessary(const QSize size)
{
    const ImageParameters parameters = getImageParameters(size.width(), //
                                                          size.height());
    if (parameters == m_lastImageParameters) {
        return;
    }

    m_lastImageParameters = parameters;
    m_pixmap = renderPixmap(parameters);
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColor ColorPatch::color() const
{
    return d_pointer->m_color;
}

/*!
 * \brief Setter for the color property.
 *
 * \a newColor the new color
 */
void ColorPatch::setColor(const QColor &newColor)
{
    if (newColor != d_pointer->m_color) {
        d_pointer->m_color = newColor;
        update();
        Q_EMIT colorChanged(newColor);
    }
}

/*!
 * \brief Renders the image to be displayed.
 *
 * \a parameters The image parameters
 *
 * Returns an image containing the color of \l m_color. If the color is
 * transparent or semi-transparent, background with small gray squares is
 * visible. If \l ColorPatch has RTL layout, the image is mirrored.
 * If the color is invalid, a special mark is shown.
 *
 * The size of
 * the image is equal or (if rounding has to be done because of fractional
 * scale factors) slightly bigger than necessary to paint the whole
 * \l ColorPatch surface at the given device-pixel-ratio. */
QImage ColorPatchPrivate::renderImage(const ImageParameters &parameters)
{
    // Initialization
    // Round up to the next integer to be sure to have a big-enough image:
    const qreal imageWidthF = parameters.width * parameters.devicePixelRatioF;
    const int imageWidth = qCeil(imageWidthF);
    const qreal imageHeightF = parameters.height * parameters.devicePixelRatioF;
    const int imageHeight = qCeil(imageHeightF);
    QImage myImage(imageWidth, //
                   imageHeight, //
                   QImage::Format_ARGB32_Premultiplied);
    if ((imageWidth <= 0) || (imageHeight <= 0)) {
        // Initializing a QPainter on an image of zero size would print
        // errors. Therefore, returning immediately:
        myImage.setDevicePixelRatio(parameters.devicePixelRatioF);
        return QImage();
    }

    // Draw content of an invalid color (and return)
    if (!parameters.color.isValid()) {
        myImage.fill( //
            Qt::transparent
            // An alternative value might be:
            // q_pointer->palette().color(myColorGroup, QPalette::Window)
            // but this integrates less nice with styles like QtCurve who
            // might have background decorations that cover all widgets.
            // Ultimately, however, it is a matter of taste.
        );
        QPen pen(parameters.lineColor);

        const auto lineWidthF = //
            parameters.lineWidth * parameters.devicePixelRatioF;
        pen.setWidthF(lineWidthF);
        pen.setCapStyle(Qt::PenCapStyle::SquareCap);
        {
            QPainter painter{&myImage};
            // Because Qt::PenCapStyle::SquareCap will extends beyond the line
            // end  by half the line width, we can use an offset and the line
            // will still touch the corner pixels of the image. It is a good
            // idea to do so, because on widgets with an extreme aspect ratio
            // (for example width 400, height 40, which is a realistic value in
            // ColorDialog), the lines seem to “shift out of the image”. Using
            // an offset, it looks nicer. How big should the offset be? To keep
            // it simple, we use the same offset for both, x and y. The
            // distance from the offset point to the point where the line
            // touches the border depends on the angle of the line. The worst
            // case (that means, the biggest distance) is for 45°. With
            // Pythagoras, we have, for the offset “a” (identical for x and y):
            // a² + a² = (½ linewidth)²
            //    2 a² = ¼ linewidth²
            //      a² = ⅛ linewidth²
            //       a = 1 ÷ (√8) linewidth
            //       a ≈ 0.35 linewidth              (Rounding down to be safe)
            const qreal offset = static_cast<qreal>(lineWidthF * 0.35);
            const qreal &left = offset; // alias for “offset”
            const qreal &top = offset; // alias for “offset”
            const qreal bottom = imageHeightF - offset;
            const qreal right = imageWidthF - offset;
            painter.setPen(pen);
            painter.setRenderHint(QPainter::Antialiasing, true);
            painter.drawLine(QPointF(left, top), //
                             QPointF(right, bottom));
            painter.drawLine(QPointF(left, bottom), //
                             QPointF(right, top));
        }
        myImage.setDevicePixelRatio(parameters.devicePixelRatioF);
        return myImage;
    }

    // Draw content of a valid color
    if (parameters.color.alphaF() < 1) {
        // Prepare the image with (semi-)transparent color
        myImage = transparencyBackground(myImage.size(), //
                                         parameters.devicePixelRatioF);
        myImage.setDevicePixelRatio(1); // necessary for correct ratio
        // Paint the color above
        QPainter(&myImage).fillRect(myImage.rect(), parameters.color);
        if (parameters.layoutDirection == Qt::RightToLeft) {
            // Horizontally mirrored image for right-to-left layout,
            // so that the “nice” part is the first you see in reading
            // direction.
            myImage = myImage.mirrored(true, // horizontally mirrored
                                       false // vertically mirrored
            );
        }
    } else {
        // Prepare the image with plain color
        myImage.fill(parameters.color);
    }
    myImage.setDevicePixelRatio(parameters.devicePixelRatioF);
    return myImage;
}

/*!
 * \brief Get image parameters for the current widget state.
 *
 * \a width of the requested image, measured in logical pixels.
 *
 * \a height of the requested image, measured in logical pixels.
 *
 * Returns the image parameters, based on the current widget state (color
 * theme, device pixel ratio etc.).
 */
ColorPatchPrivate::ImageParameters ColorPatchPrivate::getImageParameters(const int width, const int height) const
{
    ImageParameters parameters;
    parameters.width = width;
    parameters.height = height;
    parameters.devicePixelRatioF = q_pointer->devicePixelRatioF();
    parameters.color = m_color;
    QStyleOptionFrame opt;
    // initFrom() also sets also QStyle::State_MouseOver if appropriate
    opt.initFrom(q_pointer.toPointerToConstObject());
    parameters.lineWidth = qMax( //
        q_pointer->style()->pixelMetric(QStyle::PM_DefaultFrameWidth, &opt), //
        1);
    const QPalette::ColorGroup myColorGroup = //
        (q_pointer->isEnabled()) //
        ? QPalette::ColorGroup::Normal //
        : QPalette::ColorGroup::Disabled;
    parameters.lineColor = q_pointer->palette().color(myColorGroup, //
                                                      QPalette::WindowText);
    parameters.layoutDirection = q_pointer->layoutDirection();
    return parameters;
}

/*!
 * \brief Renders the image to be displayed.
 *
 * \a parameters The image parameters
 *
 * Returns the same as \l renderImage but as QPixmap. */
QPixmap ColorPatchPrivate::renderPixmap(const ImageParameters &parameters) const
{
    const QImage image = renderImage(parameters);
    QPixmap pixmap = QPixmap::fromImage(image);
    pixmap.setDevicePixelRatio(q_pointer->devicePixelRatioF());
    return pixmap;
}

/*!
 * \brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * \a eventParameter the paint event
 */
void ColorPatch::paintEvent(QPaintEvent *eventParameter)
{
    AbstractDiagram::paintEvent(eventParameter);

    // Initialization
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(QPainter::Antialiasing);

    QStyleOptionFrame frameStyleOption;
    initQLineEditStyleOption(&frameStyleOption, this);

    drawQLineEditPanel(this, &widgetPainter, frameStyleOption);

    QRect contentRectangle;

    contentRectangle = frameStyleOption.rect.marginsRemoved( //
        d_pointer->contentMargins());

    // Ensure the image is up to date.
    // - devicePixelRatioF() might have changed if the window was moved — with
    //   more than half of its surface — to a screen with a different scale
    //   factor, or if the user manually adjusted the scale of the current
    //   screen. Since QWidget does not emit events or signals for scale factor
    //   changes, here is our only reliable point to apply the correct
    //   dimensions.
    // - Widget style, color scheme or layout direction might have changed,
    //   and currently we do not track it otherwhere.
    // - The widget might have been resized, though this is also tracked
    //   in the resizeEvent reimplementation.
    d_pointer->updatePixmapIfNecessary(contentRectangle.size());

    if (!d_pointer->m_pixmap.isNull()) {
        widgetPainter.drawPixmap(contentRectangle.topLeft(), //
                                 d_pointer->m_pixmap);
    }
}

/*!
 * \brief Returns the margins between the widgets rectangle and the actual
 * color content.
 *
 * These margins are the necessary space the frame and focus indicator.
 */
QMargins ColorPatchPrivate::contentMargins() const
{
    const ColorPatch *const widget = q_pointer.toPointerToConstObject();
    QStyleOptionFrame frameStyleOption;
    initQLineEditStyleOption(&frameStyleOption, //
                             q_pointer.toPointerToConstObject());

    const QRect styleContentRectangle = q_pointer->style()->subElementRect( //
        QStyle::SE_LineEditContents,
        &frameStyleOption,
        widget);
    const int hSubContentMargin = //
        (frameStyleOption.rect.width() - styleContentRectangle.width()) / 2;
    const int vSubContentMargin = //
        (frameStyleOption.rect.height() - styleContentRectangle.height()) / 2;

    const int frameLineWidth = frameStyleOption.lineWidth;

    const auto refContentSize =
        // Big enough to be beyond any reasonable minimum widget size
        QSize(1000, 1000);
    const auto sizeFromRefContentSize = //
        q_pointer->style()->sizeFromContents( //
            QStyle::CT_LineEdit, //
            &frameStyleOption,
            refContentSize,
            widget);
    const int hRefContentMargin = //
        (sizeFromRefContentSize.width() - refContentSize.width()) / 2;
    const int vRefContentMargin = //
        (sizeFromRefContentSize.height() - refContentSize.height()) / 2;

    const int focusFrameHMargin = q_pointer->style()->pixelMetric( //
        QStyle::PM_FocusFrameHMargin,
        &frameStyleOption,
        widget);
    const int focusFrameVMargin = q_pointer->style()->pixelMetric( //
        QStyle::PM_FocusFrameVMargin,
        &frameStyleOption,
        widget);

    const int minDistance = std::max({5, frameLineWidth});

    const int hMin = std::max( //
        {minDistance, hRefContentMargin, hSubContentMargin, focusFrameHMargin});
    const int vMin = std::max( //
        {minDistance, vRefContentMargin, vSubContentMargin, focusFrameVMargin});

    return QMargins(hMin, // left
                    vMin, // top
                    hMin, // right
                    vMin // bottom
    );
}

/*!
 * \brief Returns a pixmap suitable for representing the current \c color
 * in drag-and-drop operations.
 *
 * Suitable for QDrag::setPixmap().
 */
QPixmap ColorPatch::dragDropPixmap() const
{
    const auto size = d_pointer->referenceSize();
    const auto finalSize = std::max({30, size.width(), size.height()});
    const auto parameters = d_pointer->getImageParameters(finalSize, finalSize);
    return d_pointer->renderPixmap(parameters);
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
bool ColorPatch::event(QEvent *eventParameter)
{
    return AbstractDiagram::event(eventParameter);
}

} // namespace PerceptualColor
