// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORWHEEL_P_H
#define PERCEPTUALCOLOR_COLORWHEEL_P_H

// Include the header of the public class of this private implementation.
// #include "colorwheel.h"

#include "colorwheelimage.h"
#include "constpropagatingrawpointer.h"
#include "polarpointf.h"
#include <qglobal.h>
#include <qpoint.h>
#include <qsharedpointer.h>

namespace PerceptualColor
{
class ColorWheel;

class ColorEngine;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ColorWheelPrivate
{
public:
    ColorWheelPrivate(ColorWheel *backLink, const QSharedPointer<PerceptualColor::ColorEngine> &colorEngine);
    virtual ~ColorWheelPrivate() noexcept;

    /** @brief Internal storage of the @ref ColorWheel::hue() property */
    qreal m_hue;
    /** @brief Holds if currently a mouse event is active or not.
     *
     * Default value is <tt>false</tt>.
     * - A mouse event gets typically activated on a
     *   @ref ColorWheel::mousePressEvent() done within the gamut diagram.
     *   The value is set to <tt>true</tt>.
     * - While active, all @ref ColorWheel::mouseMoveEvent() will move the
     *   diagram’s color handle.
     * - Once a @ref ColorWheel::mouseReleaseEvent() occurs, the value is set
     *   to <tt>false</tt>. Further mouse movements will not move the handle
     *   anymore.
     *
     * This is done because Qt’s default mouse tracking reacts on all clicks
     * within the whole widget. However, <em>this</em> widget is meant as a
     * circular widget, only reacting on mouse events within the circle;
     * this requires this custom implementation. */
    bool m_isMouseEventActive = false;
    /** @brief Pointer to @ref ColorEngine object used to describe the
     * working gamut. */
    QSharedPointer<ColorEngine> m_colorEngine;
    /** @brief The image of the wheel itself. */
    ColorWheelImage m_wheelImage;

    [[nodiscard]] int border() const;
    [[nodiscard]] QPointF fromWheelToWidgetCoordinates(const PolarPointF wheelCoordinates) const;
    [[nodiscard]] PolarPointF fromWidgetPixelPositionToWheelCoordinates(const QPoint position) const;
    [[nodiscard]] qreal innerDiameter() const;
    void setHueNormalized(const qreal newHue);

private:
    Q_DISABLE_COPY(ColorWheelPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorWheel> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORWHEEL_P_H
