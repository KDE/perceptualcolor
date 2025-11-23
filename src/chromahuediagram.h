// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_H
#define PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "importexport.h"
#include <qglobal.h>
#include <qsharedpointer.h>
#include <qsize.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWheelEvent;
class QWidget;

namespace PerceptualColor
{
class ChromaHueDiagramPrivate;
}
namespace PerceptualColor
{
class RgbColorSpace;
}

namespace PerceptualColor
{
/** @brief A widget for selecting chroma and hue in LCH color space
 *
 * This widget displays the plan of chroma and hue
 * (that means a diagram of the radius and the angle of the
 * LCH color space respectively the a axis and the b axis of the
 * <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * Lab color model</a>) at a given lightness.
 *
 * @image html ChromaHueDiagram.png "ChromaHueDiagram"
 *
 * The widget allows the user to select a color (chroma and hue) within the
 * specified gamut at a given lightness. It reacts on mouse events and on
 * keyboard events (see @ref keyPressEvent() for details).
 *
 * The form of the selection handle (that always indicates the distance from
 * the center of the diagram) and the circular form of the widget, all this
 * helps the user to understand intuitively that he is moving within a
 * polar coordinate system and to capture easily the current radius
 * and angle.
 *
 * Usage example: @snippet testchromahuediagram.cpp instantiate
 *
 * @note This widget <em>always</em> accepts focus by a mouse click within
 * the circle. This happens regardless of the <tt>QWidget::focusPolicy</tt>
 * property:
 * - If you set the <tt>QWidget::focusPolicy</tt> property to a
 *   value that does not accept focus by mouse click, the focus
 *   will nevertheless be accepted for clicks within the actual circle.
 *   (This is the default behavior.)
 * - If you set the <tt>QWidget::focusPolicy</tt> property to a
 *   value that accepts focus by mouse click, the focus will not only be
 *   accepted for clicks within the actual circle, but also for clicks
 *   anywhere within the (rectangular) widget.
 *
 * @internal
 *
 * @todo NICETOHAVE The hue circle around
 * chroma-hue diagram might be confusing because
 * it is colored, but it is not a usable slider like all other colored
 * elements. We could remove it. But on the other hand, it is also useful
 * to have it. Maybe make it look different than for @ref WheelColorPicker,
 * for instance make it thinner and make it touch the gray diagram area?
 *
 * @todo NICETOHAVE Questin: Use a cross cursor when the mouse cursor hovers
 * above the diagrams, for better usability? The cross cursor indicates
 * to the user that an area can be clicked in. Do it only within the gamut
 * (where the color handle can actually go) or in the hole gray circle,
 * which is the mouse sensitive area.
 *
 * @todo SHOULDHAVE Support more mouse buttons. For example, “forward” and
 * “backward” could be used to increase or decrease the radius.
 *
 * @todo SHOULDHAVE What if black or white are out of gamut on L=0.1 or L=99.9?
 * Where are the handles placed? Visible or invisible? How to react (for mouse
 * input, keyboard input, but also API functions like setColor()? Should
 * there be always a physical pixel in the middle that is visible (black
 * or white) even when out of gamut?
 * Make sure that @ref ChromaHueDiagram always shows at least at the
 * central physical pixel with an in-gamut color? Solution: Limit the range
 * of the lightness selector?
 *
 * @todo SHOULDHAVE Accept Key + like currently yet Key Left, and Key - like
 * currently yet Key Right, for consistency with @ref WheelColorPicker.
 */
class PERCEPTUALCOLOR_IMPORTEXPORT ChromaHueDiagram : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * The widget allows the user to change the LCH chroma and the LCH hue
     * values. However, the LCH lightness value cannot be changed by the
     * user, but only by the programmer through this property.
     *
     * The programmer can set this property to out-of-gamut values; the
     * user cannot.
     *
     * @sa READ @ref currentColorCielchD50() const
     * @sa WRITE @ref setCurrentColorCielchD50()
     * @sa NOTIFY @ref currentColorCielchD50Changed() */
    Q_PROPERTY(GenericColor currentColorCielchD50 READ currentColorCielchD50 WRITE setCurrentColorCielchD50 NOTIFY currentColorCielchD50Changed)

public:
    Q_INVOKABLE explicit ChromaHueDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~ChromaHueDiagram() noexcept override;
    /** @brief Getter for property @ref currentColorCielchD50
     *  @returns the property @ref currentColorCielchD50 */
    [[nodiscard]] GenericColor currentColorCielchD50() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColorCielchD50(const PerceptualColor::GenericColor &newCurrentColorCielchD50);

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColorCielchD50.
     *  @param newCurrentColorCielchD50 the new current color */
    void currentColorCielchD50Changed(const PerceptualColor::GenericColor &newCurrentColorCielchD50);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    Q_DISABLE_COPY(ChromaHueDiagram)

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ChromaHueDiagramPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ChromaHueDiagramPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestChromaHueDiagram;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_H
