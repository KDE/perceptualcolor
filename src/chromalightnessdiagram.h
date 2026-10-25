// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_H
#define PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "internalimportexport.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
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

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A widget that displays a chroma-lightness diagram for a given hue.
 *
 * \image ChromaLightnessDiagram.png "ChromaLightnessDiagram"
 *
 * The widget renders the chroma–lightness diagram across its entire area.
 *
 * \list
 *     \li Vertically, it shows lightness from 0% (bottom) to 100% (top).
 *     \li Horizontally, it shows chroma from 0 (left) to a
 *         higher value (right).
 * \endlist
 *
 * The horizontal and vertical axes use the same scale. For example:
 *
 * \list
 *     \li If the widget is square and the lightness range is 0 to 1, then both
 *         chroma and lightness span 0 to 1.
 *     \li If the widget is twice as wide as it is tall, lightness still
 *         spans 0 to 1, but chroma spans 0 to 2.
 * \endlist
 *
 * \warning Ensure that the widget’s width is sufficiently large relative
 * to its height so that the maximum‑chroma color of the current gamut
 * remains fully visible within the diagram. If the diagram surface is too
 * narrow, the rightmost portion of the gamut will be clipped, and user
 * interaction in that area will not function correctly.
 *
 *
 *
 * \note This class is not part of the public API because its interface
 * is not polished enough. Notably it does not automatically scale the
 * diagram to fit a given gamut (means: to fit up to a given maximum
 * chroma). Even if we would fix this: We would need a public API
 * that is widthForHeight-dependent to allow the library user to
 * comfortably make use of this!
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT ChromaLightnessDiagram : public AbstractDiagram
{
    Q_OBJECT

    // The Q_PROPERTY macro must be on a single line for correct compilation.
    // clang-format is disabled here to prevent automatic line breaks.
    // clang-format off
    /*!
     * \property PerceptualColor::ChromaLightnessDiagram::currentColorLch
     *
     * \brief Currently selected color
     *
     * This property represents the color in LCH form. The specific
     * \l LchSpace used is determined in the constructor of this class.
     *
     * The widget allows the user to change the LCH chroma and the
     * LCH lightness components. However, the LCH hue component cannot be
     * changed by the user; it can only be modified programmatically through
     * this property.
     */
    Q_PROPERTY(PerceptualColor::GenericColor currentColorLch READ currentColorLch WRITE setCurrentColorLch NOTIFY currentColorLchChanged)
    // clang-format on

public:
    Q_INVOKABLE explicit ChromaLightnessDiagram(const PerceptualColor::LchSpace projectionSpace, QWidget *parent = nullptr);
    virtual ~ChromaLightnessDiagram() noexcept override;
    /*!
     * \brief Getter for property \l currentColorLch
     *
     * Returns the property \l currentColorLch
     */
    [[nodiscard]] PerceptualColor::GenericColor currentColorLch() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColorLch(const PerceptualColor::GenericColor &newCurrentColorLch);

Q_SIGNALS:
    /*!
     * \brief Notify signal for property \l currentColorLch.
     *
     *  \a newCurrentColorLch the new current color
     */
    void currentColorLchChanged(const PerceptualColor::GenericColor &newCurrentColorLch);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(ChromaLightnessDiagram)

    friend class ChromaLightnessDiagramPrivate;

    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ChromaLightnessDiagramPrivate> d_pointer;

    friend class TestChromaLightnessDiagram;

    // This class is used as child class in WheelColorPicker.
    // There is a tight collaboration.
    friend class WheelColorPicker;

    // This class is used as child class in WheelColorPicker.
    // There is a tight collaboration.
    friend class WheelColorPickerPrivate;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_H
