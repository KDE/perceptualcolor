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
#include "genericcolor.h"
#include "helper.h"
#include "helpermath.h"
#include "initializetranslation.h"
#include "rgbcolorspace.h"
#include <algorithm>
#include <optional>
#include <qapplication.h>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qfontmetrics.h>
#include <qlabel.h>
#include <qline.h>
#include <qlist.h>
#include <qmargins.h>
#include <qmenu.h>
#include <qmetatype.h>
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
    const QFontMetricsF myFontMetrics(q_pointer->font());
    auto validateWithFont = [&myFontMetrics](const QString &string) -> QString {
        // Test if all characters of the translated string are actually
        // available in the given font.
        auto ucs4 = string.toUcs4();
        bool okay = true;
        for (int i = 0; okay && (i < ucs4.count()); ++i) {
            okay = myFontMetrics.inFontUcs4(ucs4.at(i));
        }
        if (okay) {
            return string;
        }
        return QString();
    };

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

    /*: @item Indicate the selected color in the swatch book. This symbol
    should be translated to whatever symbol is most appropriate for “selected”
    in the translation language. Example symbols: ✓ U+2713 CHECK MARK.
    ✗ U+2717 BALLOT X. ✘ U+2718 HEAVY BALLOT X. ○ U+25CB WHITE CIRCLE.
    ◯ U+25EF LARGE CIRCLE. Do not use emoji characters as they may render
    colorful on some systems, so they will ignore the automatically chosen
    color which is used get best contrast with the background. (Also
    U+FE0E VARIATION SELECTOR-15 does not prevent colorful rendering.) */
    m_selectionMark = validateWithFont(tr("✓"));

    /*: @item Indicate that you can click on this empty patch to add a new
    color to it. This symbol should be translated to whatever symbol is most
    appropriate for “add” in the translation language. Do not use emoji
    characters as they may render colorful on some systems, so they will ignore
    the automatically chosen color which is used get best contrast with the
    background. (Also U+FE0E VARIATION SELECTOR-15 does not prevent colorful
    rendering.) */
    m_addMark = validateWithFont(tr("+"));

    // Schedule a paint event to make the changes visible.
    q_pointer->update();
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

    const auto logicalColumnRow = d_pointer->logicalColumnRowFromPosition(event->pos());
    const auto logicalColumn = logicalColumnRow.first;
    const auto logicalRow = logicalColumnRow.second;
    if ((logicalColumn < 0) || (logicalRow < 0)) {
        return;
    }

    // If we reached here, the click must have been within a patch
    // and we have valid indexes.

    const bool swatchIsEmpty = //
        !d_pointer->m_swatchGrid.value(logicalColumn, logicalRow).isValid();

    if (event->button() == Qt::MouseButton::RightButton) {
        if (d_pointer->m_isEditable && (!swatchIsEmpty)) {
            QMenu *menu = new QMenu(this);
            // Ensure proper cleanup when menu is closed
            menu->setAttribute(Qt::WA_DeleteOnClose);
            /*: action:inmenu Appears in the context menu of swatches within
            the swatch book and provides the option to remove a swatch from the
            swatch book. */
            QAction *deleteAction = menu->addAction(tr("Delete"));
            connect(deleteAction, //
                    &QAction::triggered, //
                    this, //
                    [this, logicalColumn, logicalRow]() {
                        d_pointer->m_swatchGrid.setValue(logicalColumn, //
                                                         logicalRow, //
                                                         QColor());
                        // If the deleted swatch was the currently selected
                        // swatch, the selection mark needs an update:
                        d_pointer->selectSwatchFromCurrentColor();
                        Q_EMIT swatchGridChanged(d_pointer->m_swatchGrid);
                    });
            menu->popup(mapToGlobal(event->pos())); // Display asynchronously
            return;
        }
        return;
    }

    if (event->button() == Qt::MouseButton::LeftButton) {
        if (d_pointer->m_isEditable && swatchIsEmpty) {
            d_pointer->m_swatchGrid.setValue(logicalColumn, //
                                             logicalRow, //
                                             d_pointer->m_currentColor);
            d_pointer->selectSwatchByLogicalCoordinates(logicalColumn, //
                                                        logicalRow);
            Q_EMIT swatchGridChanged(d_pointer->m_swatchGrid);
        } else {
            d_pointer->selectSwatchByLogicalCoordinates(logicalColumn, //
                                                        logicalRow);
        }
        return;
    }
}

/** @brief Constructor
 *
 * @param colorSpace The color space of the swatches.
 * @param swatchGrid Initial value for property @ref swatchGrid
 * @param wideSpacing Set of axis where the spacing should be wider than
 *        normal. This is useful to give some visual structure: When your
 *        swatches are organized logically in columns, set
 *        <tt>Qt::Orientation::Horizontal</tt> here. To use normal spacing
 *        everywhere, simply set this parameter to <tt>{}</tt>.
 * @param parent The parent of the widget, if any */
SwatchBook::SwatchBook(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                       const PerceptualColor::QColorArray2D &swatchGrid,
                       Qt::Orientations wideSpacing,
                       QWidget *parent)
    : AbstractDiagram(parent)
    , d_pointer(new SwatchBookPrivate(this, swatchGrid, wideSpacing))
{
    qRegisterMetaType<QColorArray2D>();

    d_pointer->m_rgbColorSpace = colorSpace;

    setFocusPolicy(Qt::FocusPolicy::StrongFocus);

    setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);

    // Trigger paint events whenever the mouse enters or leaves the widget.
    // (Important on some QStyle who might paint widgets different then.)
    setAttribute(Qt::WA_Hover);

    initializeTranslation(QCoreApplication::instance(),
                          // An empty std::optional means: If in initialization
                          // had been done yet, repeat this initialization.
                          // If not, do a new initialization now with default
                          // values.
                          std::optional<QStringList>());
    d_pointer->retranslateUi();

    d_pointer->updateColorSchemeCache();
}

/** @brief Destructor */
SwatchBook::~SwatchBook() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation.
 * @param swatchGrid The swatches.
 * @param wideSpacing Set of axis using @ref widePatchSpacing instead
 *        of @ref normalPatchSpacing. */
SwatchBookPrivate::SwatchBookPrivate(SwatchBook *backLink, const PerceptualColor::QColorArray2D &swatchGrid, Qt::Orientations wideSpacing)
    : m_swatchGrid(swatchGrid)
    , m_wideSpacing(wideSpacing)
    , q_pointer(backLink)
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
    const auto contentSize = d_pointer->colorPatchesSizeWithMargin();
    const auto styleSize = style()->sizeFromContents(QStyle::CT_LineEdit, //
                                                     &myOption,
                                                     contentSize,
                                                     this);

    // On some style like (for example the MacOS style), sizeFromContents()
    // for line edits returns a value that is less height than the content
    // size. Therefore, here comes some safety code:
    const auto lineWidth = myOption.lineWidth;
    QMargins margins{lineWidth, lineWidth, lineWidth, lineWidth};
    const QSize minimumSize = contentSize.grownBy(margins);

    return minimumSize.expandedTo(styleSize);
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

    d_pointer->selectSwatchFromCurrentColor();

    Q_EMIT currentColorChanged(temp);

    update();
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColorArray2D SwatchBook::swatchGrid() const
{
    return d_pointer->m_swatchGrid;
}

/** @brief Setter for the @ref swatchGrid property.
 *
 * @pre The colors are fully opaque (alpha = 100%).
 *
 * @param newSwatchGrid the new value */
void SwatchBook::setSwatchGrid(const PerceptualColor::QColorArray2D &newSwatchGrid)
{
    if (newSwatchGrid == d_pointer->m_swatchGrid) {
        return;
    }

    d_pointer->m_swatchGrid = newSwatchGrid;

    d_pointer->selectSwatchFromCurrentColor();

    Q_EMIT swatchGridChanged(newSwatchGrid);

    // As of Qt documentation:
    //     “Notifies the layout system that this widget has changed and may
    //      need to change geometry.”
    updateGeometry();

    update();
}

/** @brief Selects a swatch from the book.
 *
 * @pre Both parameters must be valid indexes within @ref m_swatchGrid.
 *      Providing invalid indexes may result in a crash.
 *
 * @param newCurrentColumn Index of the column, corresponding to the first
 *        index in @ref m_swatchGrid.
 *
 * @param newCurrentRow Index of the row, corresponding to the second
 *        index in @ref m_swatchGrid.
 *
 * @note The coordinates are <em>logical</em>, not physically visible
 *       coordinates. In left-to-right (LTR) layouts, the logical
 *       coordinate (0, 0) represents the top-left swatch. In right-to-left
 *       (RTL) layouts, the logical coordinate (0, 0) corresponds to the
 *       top-right swatch, though the physical screen representation is
 *       mirrored.
 *
 * @post If the specified swatch is empty, no action is taken. Otherwise, the
 *       swatch is selected, the selection mark becomes visible, and
 *       @ref SwatchBook::currentColor is updated to reflect the selected color.
 */
void SwatchBookPrivate::selectSwatchByLogicalCoordinates(QListSizeType newCurrentColumn, QListSizeType newCurrentRow)
{
    const auto newColor = m_swatchGrid.value(newCurrentColumn, newCurrentRow);
    if (!newColor.isValid()) {
        return;
    }
    m_selectedColumn = newCurrentColumn;
    m_selectedRow = newCurrentRow;
    if (newColor != m_currentColor) {
        m_currentColor = newColor;
        Q_EMIT q_pointer->currentColorChanged(newColor);
    }
    q_pointer->update();
}

/** @brief Selects a swatch from the grid.
 *
 * @post If the currently selected swatch corresponds to
 * @ref SwatchBook::currentColor nothing happens. Otherwise, a swatch if
 * selected if there is one that corresponds to @ref SwatchBook::currentColor,
 * or none if there is no corresponding swatch. */
void SwatchBookPrivate::selectSwatchFromCurrentColor()
{
    if ((m_selectedColumn >= 0) && (m_selectedRow >= 0)) {
        if (m_swatchGrid.value(m_selectedColumn, m_selectedRow) == m_currentColor) {
            return;
        }
    }

    bool colorFound = false;
    const qsizetype myColumnCount = m_swatchGrid.iCount();
    const qsizetype myRowCount = m_swatchGrid.jCount();
    int columnIndex = 0;
    int rowIndex = 0;
    for (columnIndex = 0; //
         columnIndex < myColumnCount; //
         ++columnIndex) {
        for (rowIndex = 0; rowIndex < myRowCount; ++rowIndex) {
            if (m_swatchGrid.value(columnIndex, rowIndex) == m_currentColor) {
                colorFound = true;
                break;
            }
        }
        if (colorFound) {
            break;
        }
    }
    if (colorFound) {
        m_selectedColumn = columnIndex;
        m_selectedRow = rowIndex;
    } else {
        m_selectedColumn = -1;
        m_selectedRow = -1;
    }
}

/** @brief Horizontal spacing between color patches.
 *
 * @returns Horizontal spacing between color patches, measured in
 * device-independent pixel. The value depends on the
 * current <tt>QStyle</tt>.
 *
 * @sa @ref verticalPatchSpacing */
int SwatchBookPrivate::horizontalPatchSpacing() const
{
    if (m_wideSpacing.testFlag(Qt::Orientation::Horizontal)) {
        return widePatchSpacing();
    }
    return normalPatchSpacing();
}

/** @brief Value for a wide spacing between swatches.
 *
 * @returns Wide spacing between color patches, measured in
 * device-independent pixel. The value depends on the
 * current <tt>QStyle</tt>.
 *
 * @sa @ref normalPatchSpacing */
int SwatchBookPrivate::widePatchSpacing() const
{
    // NOTE The value is derived from the current QStyle’s values for some
    // horizontal spacings. This seems reasonable because some styles might
    // have tighter metrics for vertical spacing, which might not look good
    // here. The derived value is actually useful for both, horizontal and
    // vertical metrics.

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
    return qMax(temp, 5);
}

/** @brief Vertical spacing between color patches.
 *
 * @returns Vertical spacing between color patches, measured in
 * device-independent pixel. The value is typically smaller than
 * @ref horizontalPatchSpacing(), to symbolize that the binding
 * between patches is vertically stronger than horizontally. */
int SwatchBookPrivate::verticalPatchSpacing() const
{
    if (m_wideSpacing.testFlag(Qt::Orientation::Vertical)) {
        return widePatchSpacing();
    }
    return normalPatchSpacing();
}

/** @brief Normal spacing between color swatches.
 *
 * @returns Normal spacing between color patches, measured in
 * device-independent pixel. The value is typically smaller than
 * @ref widePatchSpacing(), to symbolize that the binding
 * between patches is stronger. */
int SwatchBookPrivate::normalPatchSpacing() const
{
    return qMax(widePatchSpacing() / 2, // ½ looks nice
                3 // minimal useful value for a line visible as all scales
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

/**
 * @brief Calculates the logical column and row based on a position relative to
 * the widget.
 *
 * The logical column and row may differ from the visual column and row
 * in environments with right-to-left text direction, where color patches are
 * mirrored.
 *
 * @param position The position in logical pixels relative to the widget.
 *
 * @returns The logical column and row corresponding to the given position,
 * or <tt>(-1, -1)</tt> if the position does not correspond to any color patch.
 * Empty color patches are treated as active color patches.
 */
std::pair<QListSizeType, QListSizeType> SwatchBookPrivate::logicalColumnRowFromPosition(const QPoint position) const
{
    constexpr std::pair<QListSizeType, QListSizeType> invalid(-1, -1);

    const QSize myColorPatchSize = patchSizeOuter();
    const int myPatchWidth = myColorPatchSize.width();
    const int myPatchHeight = myColorPatchSize.height();
    QStyleOptionFrame myFrameStyleOption;
    initStyleOption(&myFrameStyleOption);
    const QPoint temp = position - offset(myFrameStyleOption);

    if ((temp.x() < 0) || (temp.y() < 0)) {
        return invalid; // Click position too much leftwards or upwards.
    }

    const auto columnWidth = myPatchWidth + horizontalPatchSpacing();
    const int xWithinPatch = temp.x() % columnWidth;
    if (xWithinPatch >= myPatchWidth) {
        return invalid; // Click position horizontally between two patch columns
    }

    const auto rowHeight = myPatchHeight + verticalPatchSpacing();
    const int yWithinPatch = temp.y() % rowHeight;
    if (yWithinPatch >= myPatchHeight) {
        return invalid; // Click position vertically between two patch rows
    }

    const int rowIndex = temp.y() / rowHeight;
    if (!isInRange<qsizetype>(0, rowIndex, m_swatchGrid.jCount() - 1)) {
        // The index is out of range. This might happen when the user
        // clicks very near to the border, where is no other patch
        // anymore, but which is still part of the widget.
        return invalid;
    }

    const int visualColumnIndex = temp.x() / columnWidth;
    QListSizeType columnIndex;
    if (q_pointer->layoutDirection() == Qt::LayoutDirection::LeftToRight) {
        columnIndex = visualColumnIndex;
    } else {
        columnIndex = m_swatchGrid.iCount() - 1 - visualColumnIndex;
    }
    if (!isInRange<qsizetype>(0, columnIndex, m_swatchGrid.iCount() - 1)) {
        // The index is out of range. This might happen when the user
        // clicks very near to the border, where is no other patch
        // anymore, but which is still part of the widget.
        return invalid;
    }

    return std::pair<QListSizeType, QListSizeType>(columnIndex, rowIndex);
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
    const QSize myInnerSize = patchSizeInner();
    QStyleOptionToolButton myOptions;
    myOptions.initFrom(q_pointer.toPointerToConstObject());
    myOptions.rect.setSize(myInnerSize);
    const auto myStyledOuterSize = q_pointer->style()->sizeFromContents( //
        QStyle::CT_ToolButton,
        &myOptions,
        myInnerSize,
        q_pointer.toPointerToConstObject());
    // Ensure that the difference between patchInnerSize and patchOuterSize is
    // large enough to accommodate twice the cornerRadius. This accounts for
    // one cornerRadius on each border of the rectangle.
    const int extra = 2 * cornerRadius();
    return myStyledOuterSize.expandedTo(myInnerSize + QSize(extra, extra));
}

/** @brief Size of the inner space of a color patch.
 *
 * This is typically smaller than @ref patchSizeOuter().
 *
 * @returns Size of the inner space of a color patch, measured in
 * device-independent pixel. */
QSize SwatchBookPrivate::patchSizeInner() const
{
    const int metric = q_pointer->style()->pixelMetric( //
        QStyle::PM_ButtonIconSize,
        nullptr,
        q_pointer.toPointerToConstObject());
    const int size = std::max({metric, //
                               horizontalPatchSpacing(), //
                               verticalPatchSpacing()});
    return QSize(size, size);
}

/** @brief Corner radius for drawing rounded color patch rectangles
 *
 * Tries to guess a radius that matches well with the current QStyle.
 *
 * @returns Corner radius for drawing rounded color patch rectangles.
 * Guarantied to be ≥ 0. */
int SwatchBookPrivate::cornerRadius() const
{
    const auto defaultFrameWidth = //
        q_pointer->style()->pixelMetric(QStyle::PM_DefaultFrameWidth);
    return qMax(defaultFrameWidth, 0);
}

/**
 * @brief Draws some mark somewhere on a paint surface.
 *
 * Meant for @ref SwatchBook::paintEvent()
 *
 * @param offset Offset from the origin of the coordinate system
 * @param widgetPainter Pointer to a painter that will paint on the surface.
 *        The state of the painter will be saved before modifying it, and
 *        restored before this function returns.
 * @param color Color used to draw the mark
 * @param markSymbol Which mark to draw.
 * @param row in @ref m_swatchGrid
 * @param column in @ref m_swatchGrid
 */
void SwatchBookPrivate::drawMark(const QPoint offset,
                                 QPainter *widgetPainter,
                                 const QColor color,
                                 const SwatchBookPrivate::Mark markSymbol,
                                 const QListSizeType row,
                                 const QListSizeType column) const
{
    widgetPainter->save(); // We'll do a restore() at the end of this function.

    // Draw the selection mark (if any)
    const QListSizeType visualSelectedColumnIndex = //
        (q_pointer->layoutDirection() == Qt::LayoutDirection::LeftToRight) //
        ? column //
        : m_swatchGrid.iCount() - 1 - column;
    const int patchWidthOuter = patchSizeOuter().width();
    const int patchHeightOuter = patchSizeOuter().height();

    const QPointF selectedPatchOffset = QPointF( //
        offset.x() //
            + (static_cast<int>(visualSelectedColumnIndex) //
               * (patchWidthOuter + horizontalPatchSpacing())), //
        offset.y() //
            + (static_cast<int>(row) //
               * (patchHeightOuter + verticalPatchSpacing())));
    const int patchWidthInner = patchSizeInner().width();
    const int patchHeightInner = patchSizeInner().height();

    QString myMark;
    switch (markSymbol) {
    case Mark::Selection:
        myMark = m_selectionMark;
        break;
    case Mark::Add:
        myMark = m_addMark;
        break;
    default:
        break;
    }
    if (myMark.isEmpty()) {
        // If no selection mark is available for the current translation in
        // the current font, we will draw a hard-coded fallback mark.
        const QSize sizeDifference = patchSizeOuter() - patchSizeInner();
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
        pen.setColor(color);
        pen.setCapStyle(Qt::PenCapStyle::RoundCap);
        pen.setWidthF(penWidth);
        widgetPainter->setPen(pen);

        switch (markSymbol) {
        case Mark::Selection: {
            QPointF point1 = QPointF(penWidth, //
                                     0.7 * effectiveSquareSize);
            point1 += selectedPatchOffset + selectionMarkOffset;
            QPointF point2(0.35 * effectiveSquareSize, //
                           1 * effectiveSquareSize - penWidth);
            point2 += selectedPatchOffset + selectionMarkOffset;
            QPointF point3(1 * effectiveSquareSize - penWidth, //
                           penWidth);
            point3 += selectedPatchOffset + selectionMarkOffset;
            widgetPainter->drawLine(QLineF(point1, point2));
            widgetPainter->drawLine(QLineF(point2, point3));
        } break;
        case Mark::Add: {
            QPointF point1 = QPointF(penWidth, //
                                     0.5 * effectiveSquareSize);
            point1 += selectedPatchOffset + selectionMarkOffset;
            QPointF point2 = QPointF(1 * effectiveSquareSize - penWidth, //
                                     0.5 * effectiveSquareSize);
            point2 += selectedPatchOffset + selectionMarkOffset;
            QPointF point3(0.5 * effectiveSquareSize, //
                           penWidth);
            point3 += selectedPatchOffset + selectionMarkOffset;
            QPointF point4(0.5 * effectiveSquareSize, //
                           1 * effectiveSquareSize - penWidth);
            point4 += selectedPatchOffset + selectionMarkOffset;
            widgetPainter->drawLine(QLineF(point1, point2));
            widgetPainter->drawLine(QLineF(point3, point4));
        } break;
        default:
            break;
        }
    } else {
        QPainterPath textPath;
        // Render the selection mark string in the path
        textPath.addText(0, 0, q_pointer->font(), myMark);
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
                selectedPatchOffset.x() //
                    + (patchWidthOuter - patchWidthInner) / 2,
                // y:
                selectedPatchOffset.y() //
                    + (patchHeightOuter - patchHeightInner) / 2);

            // Scale to maximum and center within the margins
            const qreal scaleFactor = qMin(
                // Horizontal scale factor:
                patchWidthInner / boundingRectangleSize.width(),
                // Vertical scale factor:
                patchHeightInner / boundingRectangleSize.height());
            QSizeF scaledSelectionMarkSize = //
                boundingRectangleSize * scaleFactor;
            const QSizeF temp = //
                (patchSizeInner() - scaledSelectionMarkSize) / 2;
            textTransform.translate(temp.width(), temp.height());
            textTransform.scale(scaleFactor, scaleFactor);

            // Draw
            widgetPainter->setTransform(textTransform);
            widgetPainter->setPen(Qt::NoPen);
            widgetPainter->setBrush(color);
            widgetPainter->drawPath(textPath);
        }
    }

    widgetPainter->restore(); // restore from initial save() function.
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
    const int patchWidthOuter = d_pointer->patchSizeOuter().width();
    const int patchHeightOuter = d_pointer->patchSizeOuter().height();

    // Draw the background
    {
        // We draw the frame slightly shrunk on windowsvista style. Otherwise,
        // when the windowsvista style is used on 125% scale factor and with
        // a multi-monitor setup, the frame would sometimes not render on some
        // of the borders on some of the screens.
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
        // As ukui has many, many graphical glitches and bugs (up to crashs not
        // having been fixed for years), we assume that this is a problem of
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
    const QListSizeType columnCount = d_pointer->m_swatchGrid.iCount();
    const int myCornerRadius = d_pointer->cornerRadius();
    QListSizeType visualColumn;
    const auto currentScheme = d_pointer->m_colorSchemeCache;
    const QColor addMarkColor = (currentScheme == ColorSchemeType::Dark) //
        ? Qt::white
        : Qt::black;
    for (int columnIndex = 0; columnIndex < columnCount; ++columnIndex) {
        for (int row = 0; //
             row < d_pointer->m_swatchGrid.jCount(); //
             ++row //
        ) {
            const auto swatchColor = //
                d_pointer->m_swatchGrid.value(columnIndex, row);
            if (swatchColor.isValid()) {
                widgetPainter.setBrush(swatchColor);
                widgetPainter.setPen(Qt::NoPen);
                if (layoutDirection() == Qt::LayoutDirection::LeftToRight) {
                    visualColumn = columnIndex;
                } else {
                    visualColumn = columnCount - 1 - columnIndex;
                }
                widgetPainter.drawRoundedRect( //
                    offset.x() //
                        + (static_cast<int>(visualColumn) //
                           * (patchWidthOuter + d_pointer->horizontalPatchSpacing())),
                    offset.y() //
                        + row * (patchHeightOuter + d_pointer->verticalPatchSpacing()),
                    patchWidthOuter,
                    patchHeightOuter,
                    myCornerRadius,
                    myCornerRadius);
            } else {
                if (d_pointer->m_isEditable) {
                    d_pointer->drawMark(offset, //
                                        &widgetPainter, //
                                        addMarkColor, //
                                        SwatchBookPrivate::Mark::Add, //
                                        row, //
                                        columnIndex);
                }
            }
        }
    }

    // If there is no selection mark to draw, nothing more to do: Return!
    if (d_pointer->m_selectedColumn < 0 || d_pointer->m_selectedRow < 0) {
        return;
    }

    // Draw the selection mark (if any)
    const auto selectedColor = d_pointer->m_swatchGrid.value( //
        d_pointer->m_selectedColumn,
        d_pointer->m_selectedRow);
    // TODO Use Oklab instead of CielchD50
    const auto colorCielchD50 = d_pointer->m_rgbColorSpace->toCielchD50( //
        selectedColor.rgba64());
    const QColor selectionMarkColor = //
        handleColorFromBackgroundLightness(colorCielchD50.first);
    d_pointer->drawMark(offset, //
                        &widgetPainter, //
                        selectionMarkColor, //
                        SwatchBookPrivate::Mark::Selection, //
                        d_pointer->m_selectedRow, //
                        d_pointer->m_selectedColumn);
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
    QListSizeType steps = 0;
    const QListSizeType stepWidth = //
        (event->modifiers().testFlag(Qt::ControlModifier)) //
        ? 2 //
        : 1;
    QListSizeType shiftColumn = 0;
    QListSizeType shiftRow = 0;
    switch (event->key()) {
    case Qt::Key_Up:
        steps = stepWidth;
        shiftRow = -1;
        break;
    case Qt::Key_Down:
        steps = stepWidth;
        shiftRow = 1;
        break;
    case Qt::Key_Left:
        steps = stepWidth;
        shiftColumn = (layoutDirection() == Qt::LeftToRight) //
            ? -1 //
            : 1;
        break;
    case Qt::Key_Right:
        steps = stepWidth;
        shiftColumn = (layoutDirection() == Qt::LeftToRight) //
            ? 1
            : -1;
        break;
    case Qt::Key_PageUp:
        steps = d_pointer->m_swatchGrid.jCount() - 1;
        shiftRow = -1;
        break;
    case Qt::Key_PageDown:
        steps = d_pointer->m_swatchGrid.jCount() - 1;
        shiftRow = 1;
        break;
    case Qt::Key_Home:
        steps = d_pointer->m_swatchGrid.iCount() - 1;
        shiftColumn = -1;
        break;
    case Qt::Key_End:
        steps = d_pointer->m_swatchGrid.iCount() - 1;
        shiftColumn = 1;
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

    // If currently no color of the swatch book is selected, select the
    // first color as default, then return.
    if ((d_pointer->m_selectedColumn < 0) || (d_pointer->m_selectedRow < 0)) {
        for (int j = 0; j < d_pointer->m_swatchGrid.iCount(); ++j) {
            for (int i = 0; i < d_pointer->m_swatchGrid.jCount(); ++i) {
                if (d_pointer->m_swatchGrid.value(i, j).isValid()) {
                    d_pointer->selectSwatchByLogicalCoordinates(i, j);
                    return;
                }
            }
        }
    }

    // At this point, we can assume that currently a valid swatch is yet
    // selected.

    QListSizeType newLogicalColumn = d_pointer->m_selectedColumn;
    QListSizeType newLogicalRow = d_pointer->m_selectedRow;
    QListSizeType tempLogicalColumn = newLogicalColumn;
    QListSizeType tempLogicalRow = newLogicalRow;
    bool isTempLogicalPositionInRange = true;
    QListSizeType completedSteps = 0;
    while (isTempLogicalPositionInRange && (completedSteps < steps)) {
        tempLogicalColumn += shiftColumn;
        tempLogicalRow += shiftRow;
        isTempLogicalPositionInRange = //
            d_pointer->m_swatchGrid.isInRange(tempLogicalColumn, //
                                              tempLogicalRow);
        if (isTempLogicalPositionInRange) {
            const auto swatch = //
                d_pointer->m_swatchGrid.value(tempLogicalColumn, //
                                              tempLogicalRow);
            if (swatch.isValid()) {
                newLogicalColumn = tempLogicalColumn;
                newLogicalRow = tempLogicalRow;
                ++completedSteps;
            }
        }
    }
    d_pointer->selectSwatchByLogicalCoordinates(newLogicalColumn, //
                                                newLogicalRow);
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
    const auto type = event->type();

    if (type == QEvent::LanguageChange) {
        // From QCoreApplication documentation:
        //     “Installing or removing a QTranslator, or changing an installed
        //      QTranslator generates a LanguageChange event for the
        //      QCoreApplication instance. A QApplication instance will
        //      propagate the event to all toplevel widgets […].
        // Retranslate this widget itself:
        d_pointer->retranslateUi();
    }

    if ((type == QEvent::PaletteChange) || (type == QEvent::StyleChange)) {
        d_pointer->updateColorSchemeCache();
        update();
    }

    QWidget::changeEvent(event);
}

/**
 * @brief Updates @ref m_colorSchemeCache
 */
void SwatchBookPrivate::updateColorSchemeCache()
{
    m_colorSchemeCache = guessColorSchemeTypeFromWidget(q_pointer);
}

/** @brief Size necessary to render the color patches, including a margin.
 *
 * @returns Size necessary to render the color patches, including a margin.
 * Measured in device-independent pixels. */
QSize SwatchBookPrivate::colorPatchesSizeWithMargin() const
{
    q_pointer->ensurePolished();
    const QSize patchSize = patchSizeOuter();
    const int columnCount = static_cast<int>(m_swatchGrid.iCount());
    const int rowCount = static_cast<int>(m_swatchGrid.jCount());
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

bool SwatchBook::isEditable() const
{
    return d_pointer->m_isEditable;
}

/** @brief Setter for the @ref editable property.
 *
 * @param newEditable the new value */
void SwatchBook::setEditable(const bool newEditable)
{
    d_pointer->m_isEditable = newEditable;
    update(); // Schedule a paint event to make the changes visible.
    Q_EMIT editableChanged(newEditable);
}

} // namespace PerceptualColor
