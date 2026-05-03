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

/**
 * @internal
 *
 * @brief Complete wheel-based color picker widget
 *
 * It is composed of a @ref ColorWheel and, in the middle of the wheel,
 * a chroma-lightness diagram.
 *
 * @image html WheelColorPicker.png "WheelColorPicker"
 *
 * @internal
 *
 * @todo NICETOHAVE Is the (double) focus indicator actually good design? Are
 * there better solutions?
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT WheelColorPicker : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * @sa READ @ref currentColorLch() const
     * @sa WRITE @ref setCurrentColorLch()
     * @sa NOTIFY @ref currentColorLchChanged() */
    // The Q_PROPERTY macro must be on a single line for correct compilation.
    // clang-format is disabled here to prevent automatic line breaks.
    // clang-format off
    Q_PROPERTY(PerceptualColor::GenericColor currentColorLch READ currentColorLch WRITE setCurrentColorLch NOTIFY currentColorLchChanged USER true)
    // clang-format on

public:
    Q_INVOKABLE explicit WheelColorPicker(const PerceptualColor::LchSpace projectionSpace, QWidget *parent = nullptr);
    virtual ~WheelColorPicker() noexcept override;
    /** @brief Getter for property @ref currentColorLch
     *  @returns the property @ref currentColorLch */
    [[nodiscard]] PerceptualColor::GenericColor currentColorLch() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    void setCurrentColorLch(const PerceptualColor::GenericColor &newCurrentColorLch);
    [[nodiscard]] virtual QSize sizeHint() const override;

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColorLch.
     *  @param newCurrentColorLch the new current color */
    void currentColorLchChanged(const PerceptualColor::GenericColor &newCurrentColorLch);

protected:
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(WheelColorPicker)

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class WheelColorPickerPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<WheelColorPickerPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestWheelColorPicker;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_WHEELCOLORPICKER_H
