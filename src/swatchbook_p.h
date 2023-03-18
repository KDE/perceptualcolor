// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SWATCHBOOK_P_H
#define SWATCHBOOK_P_H

// Include the header of the public class of this private implementation.
// #include "swatchbook.h"

#include "constpropagatingrawpointer.h"
#include "helperqttypes.h"
#include <QtCore/qsharedpointer.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qlist.h>
#include <qobject.h>
#include <qpoint.h>
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
    explicit SwatchBookPrivate(SwatchBook *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~SwatchBookPrivate() noexcept override = default;

    [[nodiscard]] QSize patchSizeOuter() const;
    [[nodiscard]] QSize patchSizeInner() const;
    [[nodiscard]] int horizontalPatchSpacing() const;
    void initStyleOption(QStyleOptionFrame *option) const;
    [[nodiscard]] QPoint offset(const QStyleOptionFrame &styleOptionFrame) const;
    void retranslateUi();
    void selectColorFromPalette(QListSizeType newCurrentBasicColor, QListSizeType newCurrentRow);
    [[nodiscard]] int verticalPatchSpacing() const;

    /** @brief The colors of the palette.
     *
     * This is a two-dimensional array. At the first level the
     * basic color (red, green…). At the second level, a particular
     * tint/shade of this basic color.
     *
     * @sa @ref m_selectedBasicColor
     * @sa @ref m_selectedTintShade */
    QList<QList<QColor>> m_paletteColors;
    /** @brief Selected basic color.
     *
     * If one of the colors in the palette is selected, this is
     * the index of the basic color in @ref m_paletteColors.
     * Otherwise, its <tt>-1</tt>. */
    int m_selectedBasicColor = -1;
    /** @brief The selection mark to use, or an empty string if no
     * selection mark is available.
     *
     * The selection mark is drawn above the patch that is currently selected.
     *
     * This variable contains the localized selection mark string (if all its
     * characters are) available in the default font of this widget. An
     * empty string otherwise.
     *
     * The value is set by @ref retranslateUi(). */
    QString m_selectionMark;
    /** @brief Internal storage for property @ref SwatchBook::currentColor
     *
     * QColor automatically initializes with an invalid color, just like it
     * should be for the property @ref SwatchBook::currentColor, so no
     * need to initialize here explicitly. */
    QColor m_currentColor;
    /** @brief Selected tint/shade.
     *
     * If one of the colors in the palette is selected, this is
     * the index of the tint/shade in @ref m_paletteColors.
     * Otherwise, its <tt>-1</tt>. */
    int m_selectedTintShade = -1;
    /** @brief Pointer to the RgbColorSpace object. */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;

private:
    Q_DISABLE_COPY(SwatchBookPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<SwatchBook> q_pointer;
};

} // namespace PerceptualColor

#endif // SWATCHBOOK_P_H
