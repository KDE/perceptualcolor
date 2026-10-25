// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_H
#define PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_H

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
class QWheelEvent;
class QWidget;

namespace PerceptualColor
{

class ChromaHueDiagramPrivate;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A widget for selecting chroma and hue in LCH color space.
 *
 * This widget displays the plan of chroma and hue
 * (that means a diagram of the radius and the angle of the
 * LCH color space respectively the a axis and the b axis of the
 * \l {https://en.wikipedia.org/wiki/CIELAB_color_space}
 * {Lab color model}) at a given lightness.
 *
 * \image ChromaHueDiagram.png "ChromaHueDiagram"
 *
 * The widget allows the user to select a color (chroma and hue) within the
 * specified gamut at a given lightness. It reacts on mouse events and on
 * keyboard events (see \l keyPressEvent() for details).
 *
 * The form of the selection handle (that always indicates the distance from
 * the center of the diagram) and the circular form of the widget, all this
 * helps the user to understand intuitively that he is moving within a
 * polar coordinate system and to capture easily the current radius
 * and angle.
 *
 * Usage example:
 *
 * \snippet testchromahuediagram.cpp instantiate
 *
 * See \l {Circular widgets} for some pecularities of this widget.
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT ChromaHueDiagram : public AbstractDiagram
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::ChromaHueDiagram::currentColorLch
     *
     * \brief Currently selected color
     *
     * The widget allows the user to change the LCH chroma and the LCH hue
     * values. However, the LCH lightness value cannot be changed by the
     * user, but only by the programmer through this property.
     *
     * The programmer can set this property to out-of-gamut values; the
     * user cannot.
     */
    Q_PROPERTY(GenericColor currentColorLch READ currentColorLch WRITE setCurrentColorLch NOTIFY currentColorLchChanged)

public:
    Q_INVOKABLE explicit ChromaHueDiagram(const PerceptualColor::LchSpace projectionSpace, QWidget *parent = nullptr);
    virtual ~ChromaHueDiagram() noexcept override;
    /*!
     * \brief Getter for property \l currentColorLch
     */
    [[nodiscard]] GenericColor currentColorLch() const;
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
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    Q_DISABLE_COPY(ChromaHueDiagram)

    friend class ChromaHueDiagramPrivate;
    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ChromaHueDiagramPrivate> d_pointer;

    friend class TestChromaHueDiagram;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMAHUEDIAGRAM_H
