// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "swatchbook.h"
// Second, the private implementation.
#include "swatchbook_p.h" // IWYU pragma: associated

#include "abstractdiagram.h"
#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "helperconversion.h"
#include "helpermath.h"
#include "initializetranslation.h"
#include "lchdouble.h"
#include "rgbcolorspace.h"
#include <array>
#include <lcms2.h>
#include <optional>
#include <qapplication.h>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qfontmetrics.h>
#include <qline.h>
#include <qlist.h>
#include <qmargins.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsharedpointer.h>
#include <qsizepolicy.h>
#include <qstringliteral.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qtransform.h>
#include <qwidget.h>
#include <type_traits>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#else
#include <qstringlist.h>
#include <qvector.h>
#endif

namespace PerceptualColor
{

/** @brief Retranslate the UI with all user-visible strings.
 *
 * This function updates all user-visible strings by using
 * <tt>Qt::tr()</tt> to get up-to-date translations.
 *
 * This function is meant to be called at the end of the constructor and
 * additionally after each <tt>QEvent::LanguageChange</tt> event.
 *
 * @note This is the same concept as
 * <a href="https://doc.qt.io/qt-5/designer-using-a-ui-file.html">
 * Qt Designer, which also provides a function of the same name in
 * uic-generated code</a>. */
void SwatchBookPrivate::retranslateUi()
{
    // Which symbol is appropriate as selection mark? This might depend on
    // culture and language. For more information, see also
    // https://en.wikipedia.org/w/index.php?title=Check_mark&oldid=1030853305#International_differences
    // Therefore, we provide translation support for the selection mark.

    // NOTE Some candidates for “translations” of this character might be
    // emoji characters that might render colorful on some systems and
    // some fonts. It would be great to disable color fonts and only
    // accept black fonts. However, this seems to be impossible with Qt.
    // There is a command-line option named “nocolorfonts”, documented at
    // https://doc.qt.io/qt-6/qguiapplication.html#QGuiApplication
    // However, this is only available for DirectWrite font rendering
    // on Windows. There does not seem to be a cross-platform solution
    // currently.
    /*: @item Used to indicate the selected color in the palette. This symbol
    should be translated to whatever symbol is most appropriate for “selected”
    in the translation language. Example symbols: ✓ U+2713 CHECK MARK.
    ✗ U+2717 BALLOT X. ✘ U+2718 HEAVY BALLOT X. ○ U+25CB WHITE CIRCLE.
    ◯ U+25EF LARGE CIRCLE. Do not use emoji characters as they may render
    colorful on some systems, so they will ignore the automatically chosen
    color which is used get best contrast with the background. (Also
    U+FE0E VARIATION SELECTOR-15 does not prevent colorful rendering.) */
    const QString translation = tr("✓");

    // Test if all characters of the translated string are actually
    // available in the given font.
    auto ucs4 = translation.toUcs4();
    bool okay = true;
    QFontMetricsF myFontMetrics(q_pointer->font());
    for (int i = 0; okay && (i < ucs4.count()); ++i) {
        okay = myFontMetrics.inFontUcs4(ucs4.at(i));
    }

    // Return
    if (okay) {
        m_selectionMark = translation;
    } else {
        m_selectionMark = QString();
    }

    // Schedule a paint event to make the changes visible.
    q_pointer->update();
}

/** @brief Constructor
 *
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 *
 * @param parent The parent of the widget, if any */
SwatchBook::SwatchBook(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new SwatchBookPrivate(this))
{
    d_pointer->m_rgbColorSpace = colorSpace;

    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Trigger paint events whenever the mouse enters or leaves the widget.
    // (Important on some QStyle who might paint widgets different then.)
    setAttribute(Qt::WA_Hover);

    d_pointer->m_paletteColors = d_pointer->wcsBasicColorPalette();

    // Initialize the selection (and implicitly the currentColor property):
    d_pointer->selectColorFromPalette(8, 0); // Same default as in QColorDialog

    initializeTranslation(QCoreApplication::instance(),
                          // An empty std::optional means: If in initialization
                          // had been done yet, repeat this initialization.
                          // If not, do a new initialization now with default
                          // values.
                          std::optional<QStringList>());
    d_pointer->retranslateUi();
}

/** @brief Destructor */
SwatchBook::~SwatchBook() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
SwatchBookPrivate::SwatchBookPrivate(SwatchBook *backLink)
    : q_pointer(backLink)
{
}

/** @brief Recommended size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended size for the widget.
 *
 * @sa @ref minimumSizeHint() */
QSize SwatchBook::sizeHint() const
{
    return minimumSizeHint();
}

/** @brief Recommended minimum size for the widget.
 *
 * Reimplemented from base class.
 *
 * @returns Recommended minimum size for the widget.
 *
 * @sa @ref sizeHint() */
QSize SwatchBook::minimumSizeHint() const
{
    ensurePolished();

    QStyleOptionFrame myOption;
    d_pointer->initStyleOption(&myOption);

    return style()->sizeFromContents(QStyle::CT_LineEdit, //
                                     &myOption,
                                     d_pointer->colorPatchesSizeWithMargin(),
                                     this);
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColor SwatchBook::currentColor() const
{
    return d_pointer->m_currentColor;
}

/** @brief Setter for the @ref currentColor property.
 *
 * @param newCurrentColor the new color */
void SwatchBook::setCurrentColor(const QColor &newCurrentColor)
{
    // Convert to RGB:
    QColor temp = newCurrentColor;
    if (!temp.isValid()) {
        temp = Qt::black; // Conformance with QColorDialog
    }
    if (temp.spec() != QColor::Spec::Rgb) {
        // Make sure that the QColor::spec() is QColor::Spec::Rgb.
        // QColorDialog apparently calls QColor.rgb() within its
        // setCurrentColor function; this will however round to 8 bit
        // per channel. We prefer a more exact conversion to RGB:
        temp = QColor::fromRgbF( //
            temp.redF(),
            temp.greenF(),
            temp.blueF(),
            temp.alphaF());
    }

    if (temp == d_pointer->m_currentColor) {
        return;
    }

    d_pointer->m_currentColor = temp;

    bool colorFound = false;
    const qsizetype myBasicColorCount = d_pointer->m_paletteColors.count();
    const qsizetype myRowCount = d_pointer->m_paletteColors.at(0).count();
    int basicColorIndex = 0;
    int rowIndex = 0;
    for (basicColorIndex = 0; //
         basicColorIndex < myBasicColorCount; //
         ++basicColorIndex) {
        for (rowIndex = 0; rowIndex < myRowCount; ++rowIndex) {
            if (d_pointer->m_paletteColors.at(basicColorIndex).at(rowIndex) == temp) {
                colorFound = true;
                break;
            }
        }
        if (colorFound) {
            break;
        }
    }
    if (colorFound) {
        d_pointer->m_selectedBasicColor = basicColorIndex;
        d_pointer->m_selectedTintShade = rowIndex;
    } else {
        d_pointer->m_selectedBasicColor = -1;
        d_pointer->m_selectedTintShade = -1;
    }

    Q_EMIT currentColorChanged(temp);

    update();
}

/** @brief Selects a color of the palette.
 *
 * @pre Both parameters are valid indexes within @ref m_paletteColors.
 * (Otherwise there will likely be a crash.)
 *
 * @pre There are no duplicates within @ref m_paletteColors.
 *
 * @param newCurrentBasicColor Index of basic color.
 *
 * @param newCurrentRow Index of the row (tint/shade).
 *
 * @post The given color is selected. The selection mark is visible.
 * @ref SwatchBook::currentColor has the value of this color. */
void SwatchBookPrivate::selectColorFromPalette(QListSizeType newCurrentBasicColor, QListSizeType newCurrentRow)
{
    // As we assume there are no duplicates in the palette, it’s safe
    // to let setCurrentColor do all the work: It will select the
    // (only) correct color entry.
    q_pointer->setCurrentColor( //
        m_paletteColors.at(newCurrentBasicColor).at(newCurrentRow));
}

/** @brief Horizontal spacing between color patches.
 *
 * @returns Horizontal spacing between color patches, measured in
 * device-independent pixel. The value depends on the
 * current <tt>QStyle</tt>.
 *
 * @sa @ref verticalPatchSpacing
 */
int SwatchBookPrivate::horizontalPatchSpacing() const
{
    int temp = q_pointer->style()->pixelMetric( //
        QStyle::PM_LayoutHorizontalSpacing,
        nullptr,
        q_pointer.toPointerToConstObject());
    if (temp <= 0) {
        // Some styles like Qt’s build-in “Plastique” style or the external
        // “QtCurve” style return 0 here. If so, we fall back to the left
        // margin. (We do not use qMax() because this workaround should
        // really only apply when the returned value is 0, because under
        // normal circumstances, it might be intentional that the left
        // margin is bigger than the horizontal spacing.)
        temp = q_pointer->style()->pixelMetric( //
            QStyle::PM_LayoutLeftMargin,
            nullptr,
            q_pointer.toPointerToConstObject());
    }
    // Another fallback (if also PM_LayoutLeftMargin fails):
    if (temp <= 0) {
        temp = q_pointer->style()->pixelMetric( //
            QStyle::PM_DefaultFrameWidth,
            nullptr,
            q_pointer.toPointerToConstObject());
    }
    // A last-resort fallback:
    return qMax(temp, 2);
}

/** @brief Vertical spacing between color patches.
 *
 * @returns Vertical spacing between color patches, measured in
 * device-independent pixel. The value is typically smaller than
 * @ref horizontalPatchSpacing(), to symbolize that the binding
 * between patches is vertically stronger than horizontally. */
int SwatchBookPrivate::verticalPatchSpacing() const
{
    return qMax(horizontalPatchSpacing() / 3, // ⅓ looks nice
                1 // minimal useful value for a line visible as all scales
    );
}

/** @brief Initializes a <tt>QStyleOptionFrame</tt> object for this widget
 * in its current state.
 *
 * This function is provided analogous to many Qt widgets that also
 * provide a function of that name with this purpose.
 *
 * @param option The object that will be initialized
 *
 * @note The value in QStyleOptionFrame::rect is not initialized. */
void SwatchBookPrivate::initStyleOption(QStyleOptionFrame *option) const
{
    if (option == nullptr) {
        return;
    }
    option->initFrom(q_pointer.toPointerToConstObject());
    option->lineWidth = q_pointer->style()->pixelMetric( //
        QStyle::PM_DefaultFrameWidth,
        option,
        q_pointer.toPointerToConstObject());
    option->midLineWidth = 0;
    option->state |= QStyle::State_Sunken;
    // The following option is not set because this widget
    // currently has no read-only mode:
    // option->state |= QStyle::State_ReadOnly;
    option->features = QStyleOptionFrame::None;
}

/** @brief Offset between top-left of the widget and top-left of the content.
 *
 * @param styleOptionFrame The options that will be passed to <tt>QStyle</tt>
 * to calculate correctly the offset.
 *
 * @returns The pixel position of the top-left pixel of the content area
 * which can be used for the color patches. */
QPoint SwatchBookPrivate::offset(const QStyleOptionFrame &styleOptionFrame) const
{
    const QPoint innerMarginOffset = QPoint( //
        q_pointer->style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
        q_pointer->style()->pixelMetric(QStyle::PM_LayoutTopMargin));

    QStyleOptionFrame temp = styleOptionFrame; // safety copy
    const QRectF frameContentRectangle = q_pointer->style()->subElementRect( //
        QStyle::SE_LineEditContents,
        &temp, // Risk of changes, therefore using the safety copy
        q_pointer.toPointerToConstObject());
    const QSizeF swatchbookContentSize = colorPatchesSizeWithMargin();

    // Some styles, such as the Fusion style, regularly return a slightly
    // larger rectangle through QStyle::subElementRect() than the one
    // requested in SwatchBook::minimumSizeHint(), which we need to draw
    // the color patches. In the case of the Kvantum style,
    // QStyle::subElementRect().height() is even greater than
    // SwatchBook::height(). It extends beyond the widget's own dimensions,
    // both at the top and bottom. QStyle::subElementRect().y() is
    // negative. Please see https://github.com/tsujan/Kvantum/issues/676
    // for more information. To ensure a visually pleasing rendering, we
    // implement centering within QStyle::subElementRect().
    QPointF frameOffset = frameContentRectangle.center();
    frameOffset.rx() -= swatchbookContentSize.width() / 2.;
    frameOffset.ry() -= swatchbookContentSize.height() / 2.;

    return (frameOffset + innerMarginOffset).toPoint();
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding mouse event */
void SwatchBook::mousePressEvent(QMouseEvent *event)
{
    // NOTE We will not actively ignore the event, even if we didn’t actually
    // react on it. Therefore, Breeze and other styles cannot move
    // the window when clicking in the middle between two patches.
    // This is intentional, because allowing it would be confusing:
    // - The space between the patches is quite limited anyway, so
    //   it’s not worth the pain and could be surprising because somebody
    //   can click there by mistake.
    // - We use the same background as QLineEdit, which at its turn also
    //   does not allow moving the window with a left-click within the
    //   field. We should be consistent with this behaviour.

    const QSize myColorPatchSize = d_pointer->patchSizeOuter();
    const int myPatchWidth = myColorPatchSize.width();
    const int myPatchHeight = myColorPatchSize.height();
    QStyleOptionFrame myFrameStyleOption;
    d_pointer->initStyleOption(&myFrameStyleOption);
    const QPoint temp = event->pos() - d_pointer->offset(myFrameStyleOption);

    if ((temp.x() < 0) || (temp.y() < 0)) {
        return; // Click position too much leftwards or upwards.
    }

    const auto columnWidth = myPatchWidth + d_pointer->horizontalPatchSpacing();
    const int xWithinPatch = temp.x() % columnWidth;
    if (xWithinPatch >= myPatchWidth) {
        return; // Click position horizontally between two patch columns
    }

    const auto rowHeight = myPatchHeight + d_pointer->verticalPatchSpacing();
    const int yWithinPatch = temp.y() % rowHeight;
    if (yWithinPatch >= myPatchHeight) {
        return; // Click position vertically between two patch rows
    }

    const int rowIndex = temp.y() / rowHeight;
    if (!isInRange<qsizetype>(0, rowIndex, d_pointer->m_paletteColors.at(0).count() - 1)) {
        // The index is out of range. This might happen when the user
        // clicks very near to the border, where is no other patch
        // anymore, but which is still part of the widget.
        return;
    }

    const int visualColumnIndex = temp.x() / columnWidth;
    QListSizeType basicColorIndex;
    if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
        basicColorIndex = visualColumnIndex;
    } else {
        basicColorIndex = //
            d_pointer->m_paletteColors.count() - 1 - visualColumnIndex;
    }
    if (!isInRange<qsizetype>(0, basicColorIndex, d_pointer->m_paletteColors.count() - 1)) {
        // The index is out of range. This might happen when the user
        // clicks very near to the border, where is no other patch
        // anymore, but which is still part of the widget.
        return;
    }

    // If we reached here, the click must have been within a patch
    // and we have valid indexes.
    d_pointer->selectColorFromPalette(basicColorIndex, rowIndex);
}

/** @brief The size of the color patches.
 *
 * This is the bounding box around the outer limit.
 *
 * @returns The size of the color patches, measured in device-independent
 * pixel.
 *
 * @sa @ref patchSizeInner */
QSize SwatchBookPrivate::patchSizeOuter() const
{
    q_pointer->ensurePolished();
    const QSize mySize = patchSizeInner();
    QStyleOptionToolButton myOptions;
    myOptions.initFrom(q_pointer.toPointerToConstObject());
    myOptions.rect.setSize(mySize);
    return q_pointer->style()->sizeFromContents( //
        QStyle::CT_ToolButton,
        &myOptions,
        mySize,
        q_pointer.toPointerToConstObject());
}

/** @brief Size of the inner space of a color patch.
 *
 * This is typically smaller than @ref patchSizeOuter.
 *
 * @returns Size of the inner space of a color patch, measured in
 * device-independent pixel. */
QSize SwatchBookPrivate::patchSizeInner() const
{
    const int temp = q_pointer->style()->pixelMetric( //
        QStyle::PM_ButtonIconSize,
        nullptr,
        q_pointer.toPointerToConstObject());
    return QSize(temp, temp);
}

/** @brief Paint the widget.
 *
 * Reimplemented from base class.
 *
 * @param event the paint event */
void SwatchBook::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    // Initialization
    QPainter widgetPainter(this);
    widgetPainter.setRenderHint(QPainter::Antialiasing);
    QStyleOptionFrame frameStyleOption;
    d_pointer->initStyleOption(&frameStyleOption);
    const int horizontalSpacing = d_pointer->horizontalPatchSpacing();
    const int verticalSpacing = d_pointer->verticalPatchSpacing();
    const QSize patchSizeOuter = d_pointer->patchSizeOuter();
    const int patchWidthOuter = patchSizeOuter.width();
    const int patchHeightOuter = patchSizeOuter.height();

    // Draw the background
    {
        // We draw the frame slightly shrunk on windowsvista style. Otherwise,
        // when the windowsvista style is used on 125% scale factor and with
        // a multi-monitor setup, the frame would sometimes not render on some
        // borders.
        const bool vistaStyle = QString::compare( //
                                    QApplication::style()->objectName(),
                                    QStringLiteral("windowsvista"),
                                    Qt::CaseInsensitive)
            == 0;
        const int shrink = vistaStyle ? 1 : 0;
        const QMargins margins(shrink, shrink, shrink, shrink);
        auto shrunkFrameStyleOption = frameStyleOption;
        shrunkFrameStyleOption.rect = frameStyleOption.rect - margins;
        // NOTE On ukui style, drawing this primitive results in strange
        // rendering on mouse hover. Actual behaviour: The whole panel
        // background is drawn blue. Expected behaviour: Only the frame is
        // drawn blue (as ukui actually does when rendering a QLineEdit).
        // Playing around with PE_FrameLineEdit instead of or additional to
        // PE_PanelLineEdit did not give better results either.
        // As ukui has many, many graphical glitches and bugs (up to
        // crashed unfixed for years), we assume that this is a problem of
        // ukui, and not of our code. Furthermore, while the behavior is
        // unexpected, the rendering doesn’t look completely terrible; we
        // can live with that.
        style()->drawPrimitive(QStyle::PE_PanelLineEdit, //
                               &shrunkFrameStyleOption,
                               &widgetPainter,
                               this);
    }

    // Draw the color patches
    const QPoint offset = d_pointer->offset(frameStyleOption);
    const QListSizeType basicColorCount = d_pointer->m_paletteColors.count();
    QListSizeType visualColumn;
    for (int basicColor = 0; basicColor < basicColorCount; ++basicColor) {
        for (int row = 0; //
             row < d_pointer->m_paletteColors.at(0).count(); //
             ++row //
        ) {
            widgetPainter.setBrush( //
                d_pointer->m_paletteColors.at(basicColor).at(row));
            widgetPainter.setPen(Qt::NoPen);
            if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
                visualColumn = basicColor;
            } else {
                visualColumn = basicColorCount - 1 - basicColor;
            }
            widgetPainter.drawRect( //
                offset.x() //
                    + static_cast<int>(visualColumn) * (patchWidthOuter + horizontalSpacing),
                offset.y() //
                    + row * (patchHeightOuter + verticalSpacing),
                patchWidthOuter,
                patchHeightOuter);
        }
    }

    // If there is no selection mark to draw, nothing more to do: Return!
    if (d_pointer->m_selectedBasicColor < 0 || d_pointer->m_selectedTintShade < 0) {
        return;
    }

    // Draw the selection mark (if any)
    const QListSizeType visualSelectedColumnIndex = //
        (layoutDirection() == Qt::LayoutDirection::LeftToRight) //
        ? d_pointer->m_selectedBasicColor //
        : d_pointer->m_paletteColors.count() - 1 - d_pointer->m_selectedBasicColor;
    const LchDouble colorCielchD50 = d_pointer->m_rgbColorSpace->toCielchD50Double( //
        d_pointer //
            ->m_paletteColors //
            .at(d_pointer->m_selectedBasicColor) //
            .at(d_pointer->m_selectedTintShade) //
            .rgba64() //
    );
    const QColor selectionMarkColor = //
        handleColorFromBackgroundLightness(colorCielchD50.l);
    const QPointF selectedPatchOffset = QPointF( //
        offset.x() //
            + static_cast<int>(visualSelectedColumnIndex) * (patchWidthOuter + horizontalSpacing), //
        offset.y() //
            + d_pointer->m_selectedTintShade * (patchHeightOuter + verticalSpacing));
    const QSize patchSizeInner = d_pointer->patchSizeInner();
    const int patchWidthInner = patchSizeInner.width();
    const int patchHeightInner = patchSizeInner.height();
    if (d_pointer->m_selectionMark.isEmpty()) {
        // If no selection mark is available for the current translation in
        // the current font, we will draw a hard-coded fallback mark.
        const QSize sizeDifference = patchSizeOuter - patchSizeInner;
        // Offset of the selection mark to the border of the patch:
        QPointF selectionMarkOffset = QPointF( //
            sizeDifference.width() / 2.0,
            sizeDifference.height() / 2.0);
        if (patchWidthInner > patchHeightInner) {
            selectionMarkOffset.rx() += //
                ((patchWidthInner - patchHeightInner) / 2.0);
        }
        if (patchHeightInner > patchWidthInner) {
            selectionMarkOffset.ry() += //
                ((patchHeightInner - patchWidthInner) / 2.0);
        }
        const int effectiveSquareSize = qMin( //
            patchHeightInner,
            patchWidthInner);
        qreal penWidth = effectiveSquareSize * 0.08;
        QPen pen;
        pen.setColor(selectionMarkColor);
        pen.setCapStyle(Qt::PenCapStyle::RoundCap);
        pen.setWidthF(penWidth);
        widgetPainter.setPen(pen);
        QPointF point1 = QPointF(penWidth, //
                                 0.7 * effectiveSquareSize);
        point1 += selectedPatchOffset + selectionMarkOffset;
        QPointF point2(0.35 * effectiveSquareSize, //
                       1 * effectiveSquareSize - penWidth);
        point2 += selectedPatchOffset + selectionMarkOffset;
        QPointF point3(1 * effectiveSquareSize - penWidth, //
                       penWidth);
        point3 += selectedPatchOffset + selectionMarkOffset;
        widgetPainter.drawLine(QLineF(point1, point2));
        widgetPainter.drawLine(QLineF(point2, point3));
    } else {
        QPainterPath textPath;
        // Render the selection mark string in the path
        textPath.addText(0, 0, font(), d_pointer->m_selectionMark);
        // Align the path top-left to the path’s virtual coordinate system
        textPath.translate(textPath.boundingRect().x() * (-1), //
                           textPath.boundingRect().y() * (-1));
        // QPainterPath::boundingRect() might be slow. Cache the result:
        const QSizeF boundingRectangleSize = textPath.boundingRect().size();

        if (!boundingRectangleSize.isEmpty()) { // Prevent division by 0
            QTransform textTransform;

            // Offset for the current patch
            textTransform.translate(
                // x:
                selectedPatchOffset.x() + (patchWidthOuter - patchWidthInner) / 2,
                // y:
                selectedPatchOffset.y() + (patchHeightOuter - patchHeightInner) / 2);

            // Scale to maximum and center within the margins
            const qreal scaleFactor = qMin(
                // Horizontal scale factor:
                patchWidthInner / boundingRectangleSize.width(),
                // Vertical scale factor:
                patchHeightInner / boundingRectangleSize.height());
            QSizeF scaledSelectionMarkSize = //
                boundingRectangleSize * scaleFactor;
            const QSizeF temp = //
                (patchSizeInner - scaledSelectionMarkSize) / 2;
            textTransform.translate(temp.width(), temp.height());
            textTransform.scale(scaleFactor, scaleFactor);

            // Draw
            widgetPainter.setTransform(textTransform);
            widgetPainter.setPen(Qt::NoPen);
            widgetPainter.setBrush(selectionMarkColor);
            widgetPainter.drawPath(textPath);
        }
    }
}

/** @brief React on key press events.
 *
 * Reimplemented from base class.
 *
 * When the arrow keys are pressed, it moves the selection mark
 * into the desired direction.
 * When <tt>Qt::Key_PageUp</tt>, <tt>Qt::Key_PageDown</tt>,
 * <tt>Qt::Key_Home</tt> or <tt>Qt::Key_End</tt> are pressed, it moves the
 * handle a big step into the desired direction.
 *
 * Other key events are forwarded to the base class.
 *
 * @param event the event */
void SwatchBook::keyPressEvent(QKeyEvent *event)
{
    QListSizeType basicColorShift = 0;
    QListSizeType rowShift = 0;
    const int writingDirection = //
        (layoutDirection() == Qt::LeftToRight) //
        ? 1 //
        : -1;
    switch (event->key()) {
    case Qt::Key_Up:
        rowShift = -1;
        break;
    case Qt::Key_Down:
        rowShift = 1;
        break;
    case Qt::Key_Left:
        basicColorShift = -1 * writingDirection;
        break;
    case Qt::Key_Right:
        basicColorShift = 1 * writingDirection;
        break;
    case Qt::Key_PageUp:
        rowShift = (-1) * d_pointer->m_paletteColors.at(0).count();
        break;
    case Qt::Key_PageDown:
        rowShift = d_pointer->m_paletteColors.at(0).count();
        break;
    case Qt::Key_Home:
        basicColorShift = (-1) * d_pointer->m_paletteColors.count();
        break;
    case Qt::Key_End:
        basicColorShift = d_pointer->m_paletteColors.count();
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

    // If currently no color of the palette is selected, select the
    // first color as default.
    if ((d_pointer->m_selectedBasicColor < 0) && (d_pointer->m_selectedTintShade < 0)) {
        d_pointer->selectColorFromPalette(0, 0);
        return;
    }

    const int accelerationFactor = 2;
    if (event->modifiers().testFlag(Qt::ControlModifier)) {
        basicColorShift *= accelerationFactor;
        rowShift *= accelerationFactor;
    }

    d_pointer->selectColorFromPalette( //
        qBound<QListSizeType>(0, //
                              d_pointer->m_selectedBasicColor + basicColorShift,
                              d_pointer->m_paletteColors.count() - 1),
        qBound<QListSizeType>(0, //
                              d_pointer->m_selectedTintShade + rowShift, //
                              d_pointer->m_paletteColors.at(0).count() - 1));
}

/** @brief Handle state changes.
 *
 * Implements reaction on <tt>QEvent::LanguageChange</tt>.
 *
 * Reimplemented from base class.
 *
 * @param event The event. */
void SwatchBook::changeEvent(QEvent *event)
{
    if (event->type() == QEvent::LanguageChange) {
        // From QCoreApplication documentation:
        //     “Installing or removing a QTranslator, or changing an installed
        //      QTranslator generates a LanguageChange event for the
        //      QCoreApplication instance. A QApplication instance will
        //      propagate the event to all toplevel widgets […].
        // Retranslate this widget itself:
        d_pointer->retranslateUi();
    }

    QWidget::changeEvent(event);
}

/** @brief Palette derived from the basic colors.
 *
 * The exact maximim-chroma basic colors come from the WCS. Tints and shades
 * are calculated with Oklab.
 *
 * @returns Palette derived from the basic colors. */
QList<QList<QColor>> SwatchBookPrivate::wcsBasicColorPalette() const
{
    constexpr std::array<double, 3> red{{41.22, 61.40, 17.92}};
    constexpr std::array<double, 3> orange{{61.70, 29.38, 64.40}};
    constexpr std::array<double, 3> yellow{{81.35, 07.28, 109.12}};
    constexpr std::array<double, 3> green{{51.57, -63.28, 28.95}};
    constexpr std::array<double, 3> blue{{51.57, -03.41, -48.08}};
    constexpr std::array<double, 3> purple{{41.22, 33.08, -30.50}};
    constexpr std::array<double, 3> pink{{61.70, 49.42, 18.23}};
    constexpr std::array<double, 3> brown{{41.22, 17.04, 45.95}};
    constexpr std::array<std::array<double, 3>, 8> chromaticCielabColors //
        {{red, orange, yellow, green, blue, purple, pink, brown}};

    QList<QList<QColor>> result;
    result.reserve(chromaticCielabColors.size() + 1); // + 1 for gray axis

    QList<QColor> rgbList;

    // Chromatic colors
    constexpr double strongTint = 0.46;
    constexpr double weakTint = 0.23;
    constexpr double weakShade = 0.18;
    constexpr double strongShade = 0.36;
    for (const auto &cielabList : std::as_const(chromaticCielabColors)) {
        const cmsCIELab myCmsCielab{cielabList.at(0), //
                                    cielabList.at(1), //
                                    cielabList.at(2)};
        const auto oklab = fromCmscielabD50ToOklab(myCmsCielab);
        cmsCIELCh oklch;
        cmsLab2LCh(&oklch, &oklab);
        QList<cmsCIELCh> tintsAndShades;
        tintsAndShades << cmsCIELCh //
            {oklch.L + (1 - oklch.L) * strongTint, //
             oklch.C * (1 - strongTint), //
             oklch.h};
        tintsAndShades << cmsCIELCh //
            {oklch.L + (1 - oklch.L) * weakTint, //
             oklch.C * (1 - weakTint), //
             oklch.h};
        tintsAndShades << oklch;
        tintsAndShades << cmsCIELCh //
            {oklch.L * (1 - weakShade), //
             oklch.C * (1 - weakShade), //
             oklch.h};
        tintsAndShades << cmsCIELCh //
            {oklch.L * (1 - strongShade), //
             oklch.C * (1 - strongShade), //
             oklch.h};
        rgbList.clear();
        rgbList.reserve(tintsAndShades.count());
        for (const auto &variation : std::as_const(tintsAndShades)) {
            cmsCIELab variationOklab;
            cmsLCh2Lab(&variationOklab, &variation);
            const auto cielabD50 = fromOklabToCmscielabD50(variationOklab);
            rgbList << m_rgbColorSpace->fromCielchD50ToQRgbBound( //
                toLchDouble(cielabD50));
        }
        result << rgbList;
    }

    // Gray axis
    QList<double> lightnesses{1, 0.75, 0.5, 0.25, 0};
    rgbList.clear();
    rgbList.reserve(lightnesses.count());
    for (const auto &lightness : lightnesses) {
        const cmsCIELab myOklab{lightness, 0, 0};
        const auto cielabD50 = fromOklabToCmscielabD50(myOklab);
        rgbList << m_rgbColorSpace->fromCielchD50ToQRgbBound( //
            toLchDouble(cielabD50));
    }
    result << rgbList;

    return result;
}

/** @brief Size necessary to render the color patches, including a margin.
 *
 * @returns Size necessary to render the color patches, including a margin.
 * Measured in device-independent pixels. */
QSize SwatchBookPrivate::colorPatchesSizeWithMargin() const
{
    q_pointer->ensurePolished();
    const QSize patchSize = patchSizeOuter();
    const int columnCount = static_cast<int>(m_paletteColors.count());
    const int rowCount = static_cast<int>(m_paletteColors.at(0).count());
    const int width = //
        q_pointer->style()->pixelMetric(QStyle::PM_LayoutLeftMargin) //
        + columnCount * patchSize.width() //
        + (columnCount - 1) * horizontalPatchSpacing() //
        + q_pointer->style()->pixelMetric(QStyle::PM_LayoutRightMargin);
    const int height = //
        q_pointer->style()->pixelMetric(QStyle::PM_LayoutTopMargin) //
        + rowCount * patchSize.height() //
        + (rowCount - 1) * verticalPatchSpacing() //
        + q_pointer->style()->pixelMetric(QStyle::PM_LayoutBottomMargin);
    return QSize(width, height);
}

} // namespace PerceptualColor
