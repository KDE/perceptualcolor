// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SWATCHBOOK_H
#define SWATCHBOOK_H

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
class SwatchBookPrivate;

class RgbColorSpace;

/** @internal
 *
 * @brief Shows the colors of a palette.
 *
 * @image html SwatchBook.png "SwatchBook"
 *
 * The user can select a color by mouse click or keyboard.
 *
 * This used palette shows tints and shades of the basic colors.
 * Many other widgets of this library are about the perceptually
 * uniform color space. This one is different. How can we determine
 * the basic colors? The associations people have with individual
 * colors can be culturally determined. But the <em>basic color terms</em> in
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
 * LCH hue). Then follows brown, which is a special case
 * at its hue is quite the same as for orange — it’s
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
 * @section whatcolor What exact color to use?
 *
 * Currently, we use the colors of the Gtk color picker (except of pink). But
 * is there a more objective choice? What exactly is a typical “red” or a
 * typical “green”?
 *
 * We have the eleven <em>basic color terms</em> that we want
 * to use. But doesn’t every human have a slightly different
 * feeling what a “typical” red or a “typical” blue is? We
 * need a <em>focal color</em>. A definition according to
 * <a href="https://www.oxfordreference.com/display/10.1093/oi/authority.20110803095825870">
 * Oxford Reference</a>:
 *
 * > “A colour that is a prototypical instance of a particular colour name,
 * > such as a shade of red that a majority of viewers consider to be the
 * > best example of a red colour.”
 *
 * A big study about focal colors of speakers of different languages across
 * the world is the <a href="https://www1.icsi.berkeley.edu/wcs/">World
 * Color Survery</a> (WCS), who’s data is available online. Unfortunately,
 * it does not give a direct values for focal colors. Various studies have
 * however used this data, so we can find for some <em>basic color terms</em>
 * a focal color, and for some others at least a naming centroid.
 *
 * |Basic color term|WCS grid coordinates|Cielab³ L|Cielab³ a|Cielab³ b|
 * | :--------------| -----------------: | ------: | ------: | ------: |
 * | white¹         |                 A0 |   96.00 |   -0.06 |    0.06 |
 * | black¹         |                 J0 |   15.60 |   -0.02 |    0.02 |
 * | red¹           |                 G1 |   41.22 |   61.40 |   17.92 |
 * | yellow¹        |                 C9 |   81.35 |    7.28 |  109.12 |
 * | green¹         |                F17 |   51.57 |  -63.28 |   28.95 |
 * | blue¹          |                F29 |   51.57 |   -3.41 |  -48.08 |
 * | brown³         |                 G7 |   41.22 |   17.04 |   45.95 |
 * | purple³        |                G34 |   41.22 |   33.08 |  -30.50 |
 * | pink³          |                 E1 |   61.70 |   49.42 |   18.23 |
 * | orange³        |                 E6 |   61.70|    29.38 |   64.40 |
 * | gray           |      not available |         |         |         |
 *
 * ¹ Focal color as proposed by
 *   <a href="https://www.pnas.org/doi/10.1073/pnas.0503281102">Focal colors
 *   are universal after all</a>.
 *
 * ² Raw estimation of the naming centroid based on Fig. 4 of
 *   <a href="https://sites.socsci.uci.edu/~kjameson/ECST/Kay_Cook_WorldColorSurvey.pdf">
 *   this document</a>. (Fig. 5 would be the better choice, as it gives the
 *   focal color instead of the naming centroid, but unfortunately contains
 *   only red, yellow, green and blue, for which we have yet direct data.)
 *
 * ³ <a href="https://www1.icsi.berkeley.edu/wcs/data/cnum-maps/cnum-vhcm-lab-new.txt">
 *   Lookup table providing Lab values for WCS grid coordinates</a> and the
 *   <a href="https://www1.icsi.berkeley.edu/wcs/data/cnum-maps/cnum-vhcm-lab-new-README.txt">
 *   corresponding explanation</a>.
 *
 * @todo Maybe choose slightly different, but more systematic colors:
 * Same CIE-LCH or OKLCH hue for all tints and shade of a given color?
 *
 * @todo A design question: Should we draw margins around each individual
 * color patch? Maybe rely on @ref ColorPatch somehow?
 *
 * @todo A design question: Should the size of the individual color patches
 * be responsive, adopting the the widget size? */
class SwatchBook : public AbstractDiagram
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
    Q_INVOKABLE explicit SwatchBook(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~SwatchBook() noexcept override;
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
    Q_DISABLE_COPY(SwatchBook)

    /** @internal
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class SwatchBookPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<SwatchBookPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestSwatchBook;
};

} // namespace PerceptualColor

#endif // SWATCHBOOK_H
