// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PALETTEWIDGET_H
#define PALETTEWIDGET_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include <QtCore/qsharedpointer.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qsize.h>
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QWidget;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
class QObject;
#endif

namespace PerceptualColor
{
class PaletteWidgetPrivate;

class RgbColorSpace;

/** @internal
 *
 * @brief A palette widget.
 *
 * @image html PaletteWidget.png "PaletteWidget"
 *
 * This widget shows a palette of colors. The user can select
 * a color by mouse click or keyboard.
 *
 * This palette shows tints and shades of the basic colors.
 * Many other widgets of this library are about the perceptually
 * uniform color space. This one is different. How can we determine
 * the basic colors? The associations people have with individual
 * colors can be culturally determined. But the basic color terms in
 * almost all languages on earth might follow a universal pattern,
 * as Brent Berlin and Paul Kay suggest in their study
 * <a href="https://en.wikipedia.org/wiki/Basic_Color_Terms:_Their_Universality_and_Evolution">
 * Basic Colour Terms: Their Universality and Evolution</a>: There
 * are eleven basic color terms that appear in this order during
 * the evolution of a language:
 * 1. black, white
 * 2. red
 * 3. green, yellow
 * 4. blue
 * 5. brown
 * 6. purple, pink, orange, gray
 *
 * And also it seem that people worldwide agree quite well what each
 * of those color terms actually means (e.g. which HLC value has a
 * typical green or a typical read). That’s a fascinating theory. And
 * it’s a good base to chose basic colors for this palette.
 *
 * For the purpose of this widget, the colors are ordered, as exactly
 * as possible as in the perceptually uniform color space. We start
 * with the chromatic basic colors red, orange, yellow, green,
 * blue, purple, pink (this is the  order they appear on the
 * LCH hue). Then follows brown, which is
 * a special case at its hue is quite the same as for orange — it’s
 * just the less chromatic part of this hue, but nevertheless
 * perceived by humans as an independent color. For each of these
 * basic colors are provides five variants in the order
 * <a href="https://en.wikipedia.org/wiki/Tints_and_shades">
 * tint → pure color → shade</a>. After brown, which is yet less
 * chromatic than the previous colors, appear now five achromatic
 * variants in the order white → gray → black.
 *
 * The marker which is used to mark the currently selected color depends
 * on the current translation; see @ref setTranslation for details.
 *
 * @internal
 *
 * @note The chromatic color variants, with the exception of pink, come from
 * Gtk’s color dialog, as well as the achromatic color variants. The
 * variants of pink were created especially for this widget.
 *
 * @todo Maybe choose slightly different, but more systematic colors:
 * Same CIE-LCH or OKLCH hue for all tints and shade of a given color?
 *
 * @todo A design question: Should we draw margins around each individual
 * color patch? Maybe rely on @ref ColorPatch somehow?
 *
 * @todo A design question: Should the size of the individual color patches
 * be responsive, adopting the the widget size? */
class PaletteWidget : public AbstractDiagram
{
    Q_OBJECT

    /** @brief The current color.
     *
     * This property can contain any valid color, including colors
     * that are not in the palette.
     *
     * If you set this property exactly to an RGB color that is in the
     * palette, this palette entry will show a selection mark. Otherwise, no
     * selection mark will be visible.
     *
     * @sa @ref currentColor() const
     * @sa @ref setCurrentColor()
     * @sa @ref currentColorChanged() */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

public:
    Q_INVOKABLE explicit PaletteWidget(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~PaletteWidget() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    [[nodiscard]] QColor currentColor() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColor(const QColor &newCurrentColor);

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColor.
     *
     * @param newCurrentColor the new @ref currentColor */
    void currentColorChanged(const QColor &newCurrentColor);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Q_DISABLE_COPY(PaletteWidget)

    /** @internal
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class PaletteWidgetPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<PaletteWidgetPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestPaletteWidget;
};

} // namespace PerceptualColor

#endif // PALETTEWIDGET_H
