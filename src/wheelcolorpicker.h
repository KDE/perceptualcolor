// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef WHEELCOLORPICKER_H
#define WHEELCOLORPICKER_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include "lchdouble.h"
#include <qglobal.h>
#include <qsharedpointer.h>
#include <qsize.h>
class QResizeEvent;
class QWidget;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
class QObject;
#endif

namespace PerceptualColor
{
class RgbColorSpace;

class WheelColorPickerPrivate;

/** @brief Complete wheel-based color picker widget
 *
 * It is composed of a @ref ColorWheel and, in the middle of the wheel,
 * a chroma-lightness diagram.
 *
 * @image html WheelColorPicker.png "WheelColorPicker" width=200
 *
 * @internal
 *
 * @todo BUG: When the hue changes and the gamut gets smaller than the
 * current chroma-lightness value, then the marker is not moved into the
 * new gamut. But it should!
 *
 * @todo This class is a friend class of @ref ChromaLightnessDiagram
 * and of @ref ColorWheel. Would it be possible to not rely on this
 * friendship in the code of this class (or at least, to rely less
 * on it)?
 *
 * @todo Is the (double) focus indicator actually good design? Are there
 * better solutions?
 *
 * @todo Add <tt>QToolTip</tt> value explaining the accepted keys and mouse
 * movements (and also to other widgets). */
class PERCEPTUALCOLOR_IMPORTEXPORT WheelColorPicker : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged() */
    Q_PROPERTY(PerceptualColor::LchDouble currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged USER true)

public:
    Q_INVOKABLE explicit WheelColorPicker(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    virtual ~WheelColorPicker() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    [[nodiscard]] PerceptualColor::LchDouble currentColor() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    void setCurrentColor(const PerceptualColor::LchDouble &newCurrentColor);
    [[nodiscard]] virtual QSize sizeHint() const override;

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColor.
     *  @param newCurrentColor the new current color */
    void currentColorChanged(const PerceptualColor::LchDouble &newCurrentColor);

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

#endif // WHEELCOLORPICKER_H
