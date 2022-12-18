// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef CHROMAHUEDIAGRAM_H
#define CHROMAHUEDIAGRAM_H

#include <QtCore/qsharedpointer.h>
#include <qglobal.h>
#include <qobjectdefs.h>
#include <qsize.h>
#include <qstring.h>

#include "perceptualcolor-0/abstractdiagram.h"
#include "perceptualcolor-0/constpropagatinguniquepointer.h"
#include "perceptualcolor-0/importexport.h"
#include "perceptualcolor-0/lchdouble.h"

class QKeyEvent;
class QMouseEvent;
class QObject;
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
 * @headerfile chromahuediagram.h <perceptualcolor-0/chromahuediagram.h>
 *
 * This widget displays the plan of chroma and hue
 * (that means a diagram of the radial and the angle of the
 * LCH color space respectively the a axis and the b axis of the
 * <a href="https://en.wikipedia.org/wiki/CIELAB_color_space">
 * Lab color model</a>) at a given lightness.
 *
 * @image html ChromaHueDiagram.png "ChromaHueDiagram" width=250
 *
 * The widget allows the user to select a color (chroma and hue) within the
 * specified gamut at a given lightness. It reacts on mouse events and on
 * keyboard events (see @ref keyPressEvent() for details).
 *
 * The form of the selection handle (that always indicates the distance from
 * the center of the diagram) and the circular form of the widget, all this
 * helps the user to understand intuitively that he is moving within a
 * polar coordinate system and to capture easily the current radial
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
 * @todo BUG Left-click in the gray area inside the wheel but outside
 * the displayed gamut; maintain the click button and do not move the
 * mouse. Actual behavior: Mouse cursor is invisible. Expected behaviour:
 * Mouse cursor stays visible (as it would be anyway after moving the mouse).
 *
 * @todo BUG Click on the wheel. Actual behaviour: Nothing. Expected behavior:
 * The selected color follows the cursor.
 *
 * @todo BUG Wide gamut RGB: RGB 51 255 51. Chroma-Hue-Diagram: The handle
 * is drawn outside the circle. This should never happen! See also
 * @ref PerceptualColor::CielchValues::maximumChroma
 *
 * @todo The hue circle around chroma-hue diagram might be confusing because
 * it is colored, but it is not a usable slider like all other colored
 * elements. We could remove it. But on the other hand, it is also useful
 * to have it. Maybe make it look different than for @ref WheelColorPicker,
 * for instance make it thinner and make it touch the gray diagram area?
 * Maybe make it react on mouse events just like the inner part of the diagram.
 *
 * @todo Add a circular indicator to the handle, indicating the values
 * with identical chroma? Only during mouse dragging? Or always? Or never?
 *
 * @todo Example code: How to create the widget at a given
 * lightness.
 *
 * @todo Allow to touch the widget on the color wheel (and have a reaction).
 *
 * @todo Use a cross cursor for better usability: The cross cursor indicates
 * to the user that an area can be clicked in. Do it only within the gamut
 * (where the color handle can actually go) or in the hole gray circle,
 * which is the mouse sensitive area (but out of the gamut the color
 * handle cannot follow)?
 *
 * @todo Support additional mouse buttons. For example, “forward” and
 * “backward” could be used to increase or decrease the radial.
 *
 * @todo What if black or white are out of gamut on L=0.1 or L=99.9? Where
 * are the handles placed? Visible or invisible? How to react? Should
 * there be always a physical pixel in the middle that is visible (black
 * or white) even when out of gamut?
 *
 * @todo Optimization: It might be possible to <em>not</em> store
 * both, @ref ChromaHueDiagramPrivate::m_chromaHueImage and
 * a @ref ChromaHueDiagramPrivate::m_wheelImage. Instead, both could
 * be combined into one single image. As long as there is a (big enough)
 * safety margin between the color wheel and the inner (circular) diagram
 * surface, it should be possible to erase the original data and paint
 * new data above without rendering artefacts for each of these two elements
 * of the image. */
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
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged() */
    Q_PROPERTY(LchDouble currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

public:
    Q_INVOKABLE explicit ChromaHueDiagram(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~ChromaHueDiagram() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    [[nodiscard]] LchDouble currentColor() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColor(const PerceptualColor::LchDouble &newCurrentColor);

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColor.
     *  @param newCurrentColor the new current color */
    void currentColorChanged(const PerceptualColor::LchDouble &newCurrentColor);

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

#endif // CHROMAHUEDIAGRAM_H
