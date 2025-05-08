// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SWATCHBOOK_P_H
#define SWATCHBOOK_P_H

// Include the header of the public class of this private implementation.
// #include "swatchbook.h"

#include "constpropagatingrawpointer.h"
#include "helper.h"
#include "helperqttypes.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qstring.h>
#include <qstyleoption.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

namespace PerceptualColor
{
class SwatchBook;
class RgbColorSpace;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class SwatchBookPrivate final : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Marks (symbols) for use in @ref SwatchBook.
     */
    enum class Mark {
        Selection, /**< A mark that indicates the a given patch is the selected
            one. */
        Add /**< A mark that indicated that by clicking on the given empty
            patch, a new color is added to this patch. */
    };

    explicit SwatchBookPrivate(SwatchBook *backLink, const PerceptualColor::QColorArray2D &swatchGrid, Qt::Orientations wideSpacing);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~SwatchBookPrivate() noexcept override = default;

    [[nodiscard]] QSize colorPatchesSizeWithMargin() const;
    [[nodiscard]] int cornerRadius() const;
    void drawMark(const QPoint offset,
                  QPainter *widgetPainter,
                  const QColor color,
                  const SwatchBookPrivate::Mark markSymbol,
                  const QListSizeType row,
                  const QListSizeType column) const;
    [[nodiscard]] int horizontalPatchSpacing() const;
    void initStyleOption(QStyleOptionFrame *option) const;
    [[nodiscard]] std::pair<QListSizeType, QListSizeType> logicalColumnRowFromPosition(const QPoint position) const;
    [[nodiscard]] int normalPatchSpacing() const;
    [[nodiscard]] QPoint offset(const QStyleOptionFrame &styleOptionFrame) const;
    [[nodiscard]] QSize patchSizeInner() const;
    [[nodiscard]] QSize patchSizeOuter() const;
    void retranslateUi();
    void selectSwatchByLogicalCoordinates(QListSizeType newCurrentColumn, QListSizeType newCurrentRow);
    void selectSwatchFromCurrentColor();
    void updateColorSchemeCache();
    [[nodiscard]] int verticalPatchSpacing() const;
    [[nodiscard]] int widePatchSpacing() const;

    /** @brief The add mark to use, or an empty string if no
     * add mark is available.
     *
     * The add mark is drawn above an empty patch.
     *
     * This variable contains the localized add mark string (if all its
     * characters are available in the default font of this widget). An
     * empty string otherwise.
     *
     * The value is set by @ref retranslateUi(). */
    QString m_addMark;
    /** @brief Internal storage for property @ref SwatchBook::currentColor
     *
     * QColor automatically initializes with an invalid color, just like it
     * should be for the property @ref SwatchBook::currentColor, so no
     * need to initialize here explicitly. */
    QColor m_currentColor;
    /**
     * @brief Cache for the current color scheme of this widget.
     *
     * @sa @ref updateColorSchemeCache()
     */
    ColorSchemeType m_colorSchemeCache = ColorSchemeType::Light;
    /** @brief Internal storage for property @ref SwatchBook::editable */
    bool m_isEditable = false;
    /** @brief Pointer to the RgbColorSpace object. */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief Selected column.
     *
     * If one of the swatches in the book is selected, this is
     * the index of the column.
     * Otherwise, its <tt>-1</tt>.
     *
     * @note The coordinates are <em>logical</em>, not physically visible
     *       coordinates. In left-to-right (LTR) layouts, the logical
     *       coordinate (0, 0) represents the top-left swatch. In right-to-left
     *       (RTL) layouts, the logical coordinate (0, 0) corresponds to the
     *       top-right swatch, though the physical screen representation is
     *       mirrored.
     *
     * @sa @ref m_selectedRow
     */
    QListSizeType m_selectedColumn = -1;
    /** @brief Selected row.
     *
     * If one of the swatches in the book is selected, this is
     * the index of the row.
     * Otherwise, its <tt>-1</tt>.
     *
     * @note The coordinates are <em>logical</em>, not physically visible
     *       coordinates. In left-to-right (LTR) layouts, the logical
     *       coordinate (0, 0) represents the top-left swatch. In right-to-left
     *       (RTL) layouts, the logical coordinate (0, 0) corresponds to the
     *       top-right swatch, though the physical screen representation is
     *       mirrored.
     *
     * @sa @ref m_selectedColumn
     */
    QListSizeType m_selectedRow = -1;
    /** @brief The selection mark to use, or an empty string if no
     * selection mark is available.
     *
     * The selection mark is drawn above the patch that is currently selected.
     *
     * This variable contains the localized selection mark string (if all its
     * characters are available in the default font of this widget). An
     * empty string otherwise.
     *
     * The value is set by @ref retranslateUi(). */
    QString m_selectionMark;
    /** @brief Internal storage for property @ref SwatchBook::swatchGrid */
    QColorArray2D m_swatchGrid;
    /** @brief List of axis where @ref widePatchSpacing should be used. */
    const Qt::Orientations m_wideSpacing;

private:
    Q_DISABLE_COPY(SwatchBookPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<SwatchBook> q_pointer;
};

} // namespace PerceptualColor

#endif // SWATCHBOOK_P_H
