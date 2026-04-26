// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_H
#define PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "helperconversion.h"
#include "internalimportexport.h"
#include <qglobal.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qtmetamacros.h>
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWidget;

namespace PerceptualColor
{
class ChromaLightnessDiagramPrivate;

class ColorEngine;

/** @internal
 *
 * @brief A widget that displays a chroma-lightness diagram for a given hue.
 *
 * @image html ChromaLightnessDiagram.png "ChromaLightnessDiagram"
 *
 * The widget renders the chroma–lightness diagram across its entire area.
 * - Vertically, it shows lightness from 0% (bottom) to 100% (top).
 * - Horizontally, it shows chroma from 0 (left) to a higher value (right).
 *
 * The horizontal and vertical axes use the same scale. For example:
 * - If the widget is square and the lightness range is 0 to 1, then both
 *   chroma and lightness span 0 to 1.
 * - If the widget is twice as wide as it is tall, lightness still
 *   spans 0 to 1, but chroma spans 0 to 2.
 *
 * @internal
 *
 * @note This class is not part of the public API because its interface
 * is not polished enough. Notably it does not automatically scale the
 * diagram to fit a given gamut (means: to fit up to a given maximum
 * chroma). Even if we would fix this: We would need a public API
 * that is widthForHeight-dependent to allow the library user to
 * comfortably make use of this!
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT ChromaLightnessDiagram : public AbstractDiagram
{
    Q_OBJECT

    /** @brief Currently selected color
     *
     * This property represents the color in LCH form. The specific
     * @ref LchSpace used is determined in the constructor of this class.
     *
     * The widget allows the user to change the LCH chroma and the
     * LCH lightness components. However, the LCH hue component cannot be
     * changed by the user; it can only be modified programmatically through
     * this property.
     *
     * @sa READ @ref currentColorLch() const
     * @sa WRITE @ref setCurrentColorLch()
     * @sa NOTIFY @ref currentColorLchChanged() */
    // The Q_PROPERTY macro must be on a single line for correct compilation.
    // clang-format is disabled here to prevent automatic line breaks.
    // clang-format off
    Q_PROPERTY(PerceptualColor::GenericColor currentColorLch READ currentColorLch WRITE setCurrentColorLch NOTIFY currentColorLchChanged)
    // clang-format on

public:
    Q_INVOKABLE explicit ChromaLightnessDiagram(const QSharedPointer<PerceptualColor::ColorEngine> &colorEngine,
                                                const PerceptualColor::LchSpace projectionSpace,
                                                QWidget *parent = nullptr);
    virtual ~ChromaLightnessDiagram() noexcept override;
    /**
     * @brief Getter for property @ref currentColorLch
     *
     * @returns the property @ref currentColorLch
     */
    [[nodiscard]] PerceptualColor::GenericColor currentColorLch() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColorLch(const PerceptualColor::GenericColor &newCurrentColorLch);

Q_SIGNALS:
    /**
     * @brief Notify signal for property @ref currentColorLch.
     *  @param newCurrentColorLch the new current color
     */
    void currentColorLchChanged(const PerceptualColor::GenericColor &newCurrentColorLch);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(ChromaLightnessDiagram)

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ChromaLightnessDiagramPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ChromaLightnessDiagramPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestChromaLightnessDiagram;

    /** @internal
     * @brief Internal friend declaration.
     *
     * This class is used as child class in @ref WheelColorPicker.
     * There is a tight collaboration. */
    friend class WheelColorPicker;
    /** @internal
     * @brief Internal friend declaration.
     *
     * This class is used as child class in @ref WheelColorPicker.
     * There is a tight collaboration. */
    friend class WheelColorPickerPrivate;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_H
