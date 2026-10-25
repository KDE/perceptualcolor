// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_WHEELCOLORPICKER_H
#define PERCEPTUALCOLOR_WHEELCOLORPICKER_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "internalimportexport.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qsize.h>
#include <qtmetamacros.h>
class QResizeEvent;
class QWidget;

namespace PerceptualColor
{

class WheelColorPickerPrivate;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Complete wheel-based color picker widget.
 *
 * It is composed of a \l ColorWheel and, in the middle of the wheel,
 * a chroma-lightness diagram.
 *
 * \image WheelColorPicker.png "WheelColorPicker"
 *
 * See \l {Circular widgets} for some pecularities of this widget.
 *
 *
 *
 * \note It may seem appealing to use the entire inner circle
 * as the background for the chroma‑lightness diagram, since this
 * would allow a slightly larger display area. However, this design
 * is less intuitive: a circular background suggests a polar
 * coordinate system (as in \l ChromaHueDiagram), while the chroma‑lightness
 * diagram is based on a Cartesian system. By keeping the background
 * rectangular, users immediately recognize the hard limits at the
 * left, top, and bottom edges (with the right edge potentially
 * extending toward infinity). The rectangular form makes the
 * Cartesian nature of the diagram clear and avoids confusion with
 * polar coordinates.
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT WheelColorPicker : public AbstractDiagram
{
    Q_OBJECT

    // The Q_PROPERTY macro must be on a single line for correct compilation.
    // clang-format is disabled here to prevent automatic line breaks.
    // clang-format off
    /*!
     * \property PerceptualColor::WheelColorPicker::currentColorLch
     *
     * \brief Currently selected color
     */
    Q_PROPERTY(PerceptualColor::GenericColor currentColorLch READ currentColorLch WRITE setCurrentColorLch NOTIFY currentColorLchChanged USER true)
    // clang-format on

public:
    Q_INVOKABLE explicit WheelColorPicker(const PerceptualColor::LchSpace projectionSpace, QWidget *parent = nullptr);
    virtual ~WheelColorPicker() noexcept override;
    /*!
     * \brief Getter for property \l currentColorLch
     */
    [[nodiscard]] PerceptualColor::GenericColor currentColorLch() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    void setCurrentColorLch(const PerceptualColor::GenericColor &newCurrentColorLch);
    [[nodiscard]] virtual QSize sizeHint() const override;

Q_SIGNALS:
    /*!
     * \brief Notify signal for property \l currentColorLch.
     *
     *  \a newCurrentColorLch the new current color
     */
    void currentColorLchChanged(const PerceptualColor::GenericColor &newCurrentColorLch);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(WheelColorPicker)

    friend class WheelColorPickerPrivate;
    /*!
     *\brief Pointer to implementation (pimpl)
     */
    ConstPropagatingUniquePointer<WheelColorPickerPrivate> d_pointer;

    friend class TestWheelColorPicker;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_WHEELCOLORPICKER_H
