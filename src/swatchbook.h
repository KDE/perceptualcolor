// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SWATCHBOOK_H
#define SWATCHBOOK_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qsharedpointer.h>
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
 * The user can select a color either by mouse click or by using the keyboard.
 *
 * The palette contains various tints and shades of the basic colors. While
 * many other widgets in this library use the perceptually uniform color space,
 * this one is different. The choice of the basic colors is based on the
 * <a href="https://en.wikipedia.org/wiki/Basic_Color_Terms:_Their_Universality_and_Evolution">
 * study by Brent Berlin and Paul Kay</a>, who suggest that the
 * basic color terms in almost all languages on earth follow a universal
 * pattern. They propose that there are eleven basic color terms that appear
 * in this order during the evolution of a language:
 *
 * 1. black, white
 * 2. red
 * 3. green, yellow
 * 4. blue
 * 5. brown
 * 6. purple, pink, orange, gray
 *
 * Additionally, people worldwide seem to agree quite well on the typical
 * values of each of these color terms. This theory is a fascinating one
 * and forms a good basis for choosing basic colors for this palette.
 *
 * This widget's colors have been arranged largely according to the color
 * wheel of the perceptually uniform color space. We start with the saturated
 * basic colors: red, orange, yellow, green, blue, and purple in order of
 * their hue angles. Next, we have pink and brown, which have roughly the
 * same hue as red or orange but are less saturated. These are simply the
 * less chromatic parts of this hue but are nevertheless perceived by humans
 * as independent colors.For each of these basic colors, there are five
 * variants in the order of <a href="https://en.wikipedia.org/wiki/Tints_and_shades">
 * tint, pure color, and shade</a>. Following the saturated colors and
 * eventually the less saturated ones, the gray axis comes in last place.
 *
 * The marker used to mark the currently selected color depends
 * on the current translation; see @ref setTranslation for details.
 *
 * What exact colors are used? What exactly is a typical “red” or a
 * “green”? Doesn’t every human have a slightly different
 * feeling what a “typical” red or a “typical” blue is? We
 * need a <em>focal color</em>, which is, according to
 * <a href="https://www.oxfordreference.com/display/10.1093/oi/authority.20110803095825870">
 * Oxford Reference</a>:
 *
 * > “A colour that is a prototypical instance of a particular colour name,
 * > such as a shade of red that a majority of viewers consider to be the
 * > best example of a red colour.”
 *
 * The <a href="https://www1.icsi.berkeley.edu/wcs/">World Color Survey</a>
 * (WCS) is a significant study about focal colors of speakers of different
 * languages across the world. The data from this survey is available online,
 * and while it does not provide direct values for focal colors, various
 * studies have used this data to determine focal colors for some
 * <em>basic color terms</em> and a naming centroid for others.
 *
 * The table below shows the WCS grid coordinates for the basic color terms
 * along with the corresponding Cielab values for the focal color (where
 * available) or the naming centroid (where focal color data is unavailable).
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
 * From this data, the colors in our palette have been derived as follows:
 * - The gray axis has been defined manually, ignoring the WCS data. Chroma
 *   is 0. The lightness is 100% for white, 0% for black, and 75%, 50%,
 *   and 25% for the intermediate grays.
 * - The other columns for chromatic colors use the WCS data for the swatch in
 *   the middle. Tints and shades are calculated by adding or reducing chroma
 *   and lightness within the Oklab color space. If the resulting color falls
 *   outside the widget's gamut, a nearby in-gamut color is chosen instead.
 *
 * @internal
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
