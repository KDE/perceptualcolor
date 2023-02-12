// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "palettewidget.h"
// Second, the private implementation.
#include "palettewidget_p.h" // IWYU pragma: associated

#include "abstractdiagram.h"
#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "helpermath.h"
#include "initializetranslation.h"
#include "lchdouble.h"
#include "rgbcolorspace.h"
#include <memory>
#include <optional>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qfontmetrics.h>
#include <qline.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsizepolicy.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qtransform.h>
#include <qwidget.h>

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
void PaletteWidgetPrivate::retranslateUi()
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
PaletteWidget::PaletteWidget(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new PaletteWidgetPrivate(this))
{
    d_pointer->m_rgbColorSpace = colorSpace;

    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Trigger paint events whenever the mouse enters or leaves the widget.
    // (Important on some QStyle who might paint widgets different then.)
    setAttribute(Qt::WA_Hover);

    QList<QColor> red; // Same as in GTK’s color dialog
    red.append(QColor(246, 97, 81));
    red.append(QColor(237, 51, 59));
    red.append(QColor(224, 27, 36));
    red.append(QColor(192, 28, 40));
    red.append(QColor(165, 29, 45));
    QList<QColor> orange; // Same as in GTK’s color dialog
    orange.append(QColor(255, 190, 111));
    orange.append(QColor(255, 163, 72));
    orange.append(QColor(255, 120, 0));
    orange.append(QColor(230, 97, 0));
    orange.append(QColor(198, 70, 0));
    QList<QColor> yellow; // Same as in GTK’s color dialog
    yellow.append(QColor(249, 240, 107));
    yellow.append(QColor(248, 228, 92));
    yellow.append(QColor(246, 211, 45));
    yellow.append(QColor(245, 194, 17));
    yellow.append(QColor(229, 165, 10));
    QList<QColor> green; // Same as in GTK’s color dialog
    green.append(QColor(143, 240, 164));
    green.append(QColor(87, 227, 137));
    green.append(QColor(51, 209, 122));
    green.append(QColor(46, 194, 126));
    green.append(QColor(38, 162, 105));
    QList<QColor> blue; // Same as in GTK’s color dialog
    blue.append(QColor(153, 193, 241));
    blue.append(QColor(98, 160, 234));
    blue.append(QColor(53, 132, 228));
    blue.append(QColor(28, 113, 216));
    blue.append(QColor(26, 95, 180));
    QList<QColor> purple; // Same as in GTK’s color dialog
    purple.append(QColor(220, 138, 221));
    purple.append(QColor(192, 97, 203));
    purple.append(QColor(145, 65, 172));
    purple.append(QColor(129, 61, 156));
    purple.append(QColor(97, 53, 131));
    QList<QColor> pink; // Not is GTK’s color dialog
    pink.append(QColor(252, 193, 213));
    pink.append(QColor(255, 158, 192));
    pink.append(QColor(234, 99, 150));
    pink.append(QColor(207, 81, 128));
    pink.append(QColor(180, 62, 105));
    QList<QColor> brown; // Same as in GTK’s color dialog
    brown.append(QColor(205, 171, 143));
    brown.append(QColor(181, 131, 90));
    brown.append(QColor(152, 106, 68));
    brown.append(QColor(134, 94, 60));
    brown.append(QColor(99, 69, 44));
    QList<QColor> achromatic; // Subset of what’s in GTK’s color dialog
    achromatic.append(QColor(255, 255, 255));
    achromatic.append(QColor(222, 221, 218));
    achromatic.append(QColor(154, 153, 150));
    achromatic.append(QColor(94, 92, 100));
    achromatic.append(QColor(0, 0, 0));
    d_pointer->m_paletteColors //
        << red //
        << orange //
        << yellow //
        << green //
        << blue //
        << purple //
        << pink //
        << brown //
        << achromatic;

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
PaletteWidget::~PaletteWidget() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
PaletteWidgetPrivate::PaletteWidgetPrivate(PaletteWidget *backLink)
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
QSize PaletteWidget::sizeHint() const
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
QSize PaletteWidget::minimumSizeHint() const
{
    ensurePolished();

    const QSize myPatchSize = d_pointer->patchSizeOuter();
    const int myColumnCount = //
        static_cast<int>(d_pointer->m_paletteColors.count());
    const int myRowCount = static_cast<int>( //
        d_pointer->m_paletteColors.at(0).count());
    QSize myContentSize;
    myContentSize.setWidth( //
        style()->pixelMetric(QStyle::PM_LayoutLeftMargin) //
        + myColumnCount * myPatchSize.width() //
        + (myColumnCount - 1) * d_pointer->horizontalPatchSpacing() //
        + style()->pixelMetric(QStyle::PM_LayoutRightMargin));
    myContentSize.setHeight( //
        style()->pixelMetric(QStyle::PM_LayoutTopMargin) //
        + myRowCount * myPatchSize.height() //
        + (myRowCount - 1) * d_pointer->verticalPatchSpacing() //
        + style()->pixelMetric(QStyle::PM_LayoutBottomMargin));

    QStyleOptionFrame myOption;
    d_pointer->initStyleOption(&myOption);

    return style()->sizeFromContents(QStyle::CT_LineEdit, //
                                     &myOption,
                                     myContentSize,
                                     this);
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColor PaletteWidget::currentColor() const
{
    return d_pointer->m_currentColor;
}

/** @brief Setter for the @ref currentColor property.
 *
 * @param newCurrentColor the new color */
void PaletteWidget::setCurrentColor(const QColor &newCurrentColor)
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
 * @ref PaletteWidget::currentColor has the value of this color. */
void PaletteWidgetPrivate::selectColorFromPalette(QListSizeType newCurrentBasicColor, QListSizeType newCurrentRow)
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
int PaletteWidgetPrivate::horizontalPatchSpacing() const
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
int PaletteWidgetPrivate::verticalPatchSpacing() const
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
void PaletteWidgetPrivate::initStyleOption(QStyleOptionFrame *option) const
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
QPoint PaletteWidgetPrivate::offset(const QStyleOptionFrame &styleOptionFrame) const
{
    const QPoint innerMarginOffset = QPoint( //
        q_pointer->style()->pixelMetric(QStyle::PM_LayoutLeftMargin),
        q_pointer->style()->pixelMetric(QStyle::PM_LayoutTopMargin));

    QStyleOptionFrame temp = styleOptionFrame; // safety copy
    const QRect myContentRectangle = q_pointer->style()->subElementRect( //
        QStyle::SE_LineEditContents,
        &temp, // Risk of changes, therefore using the safety copy
        q_pointer.toPointerToConstObject());

    QPoint frameOffset = myContentRectangle.topLeft();
    // In the Kvantum style in version 0.18, there was a bug
    // https://github.com/tsujan/Kvantum/issues/676 that returned
    // negative values here. This Kvantum bug broke this widget here.
    // Therefore, it is well possible that other QStyle subclasses have
    // the same bug. While the Kvantum is has been fixed in the meantime,
    // to be sure we use this workaround, which isn’t perfect, but
    // better than nothing:
    frameOffset.rx() = qMax(frameOffset.x(), 0);
    frameOffset.ry() = qMax(frameOffset.y(), 0);

    return frameOffset + innerMarginOffset;
}

/** @brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * @param event The corresponding mouse event */
void PaletteWidget::mousePressEvent(QMouseEvent *event)
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
QSize PaletteWidgetPrivate::patchSizeOuter() const
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
QSize PaletteWidgetPrivate::patchSizeInner() const
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
void PaletteWidget::paintEvent(QPaintEvent *event)
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
    style()->drawPrimitive(QStyle::PE_PanelLineEdit, //
                           &frameStyleOption,
                           &widgetPainter,
                           this);

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
    const LchDouble colorLch = d_pointer->m_rgbColorSpace->toCielchDouble( //
        d_pointer //
            ->m_paletteColors //
            .at(d_pointer->m_selectedBasicColor) //
            .at(d_pointer->m_selectedTintShade) //
            .rgba64() //
    );
    const QColor selectionMarkColor = //
        handleColorFromBackgroundLightness(colorLch.l);
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
void PaletteWidget::keyPressEvent(QKeyEvent *event)
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
void PaletteWidget::changeEvent(QEvent *event)
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

} // namespace PerceptualColor
