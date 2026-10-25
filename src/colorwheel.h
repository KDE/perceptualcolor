// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORWHEEL_H
#define PERCEPTUALCOLOR_COLORWHEEL_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "internalimportexport.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qsize.h>
#include <qtmetamacros.h>
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWheelEvent;
class QWidget;

namespace PerceptualColor
{
class ColorWheelPrivate;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A color wheel widget.
 *
 * This widget allows the user to choose the hue (as defined in the LCH
 * color space).
 *
 * \image ColorWheel.png "ColorWheel"
 *
 * See \l {Circular widgets} for some pecularities of this widget.
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT ColorWheel : public AbstractDiagram
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::ColorWheel::hue
     *
     * \brief The currently selected hue.
     *
     * This is the hue angle, as defined in the LCH color model.
     *
     *
     * Measured in degree.
     *
     * Valid range: [0°, 360°[. The widget accepts initially also
     * out-of-range values, but once a user interaction has taken
     * place, it will hold a normalized value. So
     *
     * \list
     *     \li 0 gets 0
     *     \li 359.9 gets 359.9
     *     \li 360 gets 0
     *     \li 361.2 gets 1.2
     *     \li 720 gets 0
     *     \li -1 gets 359
     *     \li -1.3 gets 358.7
     * \endlist
     */
    Q_PROPERTY(qreal hue READ hue WRITE setHue NOTIFY hueChanged USER true)

public:
    Q_INVOKABLE explicit ColorWheel(const PerceptualColor::LchSpace projectionSpace, QWidget *parent = nullptr);
    virtual ~ColorWheel() noexcept override;
    /*!
     * \brief Getter for property \l hue
     */
    [[nodiscard]] qreal hue() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

Q_SIGNALS:
    /*!
     * \brief Notify signal for property \l hue.
     *
     * \a newHue the new hue
     */
    void hueChanged(const qreal newHue);

public Q_SLOTS:
    void setHue(const qreal newHue);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    Q_DISABLE_COPY(ColorWheel)

    friend class ColorWheelPrivate;
    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ColorWheelPrivate> d_pointer;

    friend class TestColorWheel;

    // This class is used as child class in WheelColorPicker.
    // There is a tight collaboration.
    friend class WheelColorPicker;

    // This class is used as child class in WheelColorPicker.
    // There is a tight collaboration.
    friend class WheelColorPickerPrivate;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORWHEEL_H
