// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORWHEEL_P_H
#define PERCEPTUALCOLOR_COLORWHEEL_P_H

// Include the header of the public class of this private implementation.
// #include "colorwheel.h"

#include "colorwheelimageparameters.h"
#include "constpropagatingrawpointer.h"
#include "perceptualcolornamespace.h"
#include "polarpointf.h"
#include <qglobal.h>
#include <qpoint.h>

namespace PerceptualColor
{
class ColorWheel;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 *  \brief Private implementation within the \e {Pointer to
 *  implementation} idiom. */
class ColorWheelPrivate
{
public:
    ColorWheelPrivate(ColorWheel *backLink, const PerceptualColor::LchSpace projectionSpace);
    virtual ~ColorWheelPrivate() noexcept;

    /*!
     * \brief Internal storage of the \l ColorWheel::hue() property */
    qreal m_hue;
    /*!
     * \brief Holds if currently a mouse event is active or not.
     *
     * Default value is \c {false}.
     *
     * \list
     * \li A mouse event gets typically activated on a
     *   \l ColorWheel::mousePressEvent() done within the gamut diagram.
     *   The value is set to \c {true}.
     * \li While active, all \l ColorWheel::mouseMoveEvent() will move the
     *   diagram’s color handle.
     * \li Once a \l ColorWheel::mouseReleaseEvent() occurs, the value is set
     *   to \c {false}. Further mouse movements will not move the handle
     *   anymore.
     * \endlist
     *
     * This is done because Qt’s default mouse tracking reacts on all clicks
     * within the whole widget. However, \e {this} widget is meant as a
     * circular widget, only reacting on mouse events within the circle;
     * this requires this custom implementation. */
    bool m_isMouseEventActive = false;
    /*!
     * \brief The color space into which the gamut will be projected.
     */
    const LchSpace m_projectionSpace;
    /*!
     * \brief The image of the wheel itself. */
    ColorWheelImageParameters m_wheelImage;

    [[nodiscard]] int border() const;
    [[nodiscard]] QPointF fromWheelToWidgetCoordinates(const PolarPointF wheelCoordinates) const;
    [[nodiscard]] PolarPointF fromWidgetPixelPositionToWheelCoordinates(const QPoint position) const;
    [[nodiscard]] qreal innerDiameter() const;
    void setHueNormalized(const qreal newHue);

private:
    Q_DISABLE_COPY(ColorWheelPrivate)

    /*!
     * \brief Pointer to the object from which \e {this} object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorWheel> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORWHEEL_P_H
