// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_SWATCHBOOK_H
#define PERCEPTUALCOLOR_SWATCHBOOK_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "helper.h"
#include "internalimportexport.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qsize.h>
#include <qtmetamacros.h>
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QWidget;

namespace PerceptualColor
{

class SwatchBookPrivate;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Shows colors patches.
 *
 * \image SwatchBook.png "SwatchBook"
 *
 * The user can select a color either by mouse click or by using the keyboard.
 *
 * The marker used to mark the currently selected color depends
 * on the current translation; see \l setTranslation for details.
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT SwatchBook : public AbstractDiagram
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::SwatchBook::currentColor
     *
     * \brief The current color.
     *
     * This property can contain any color, including colors
     * that are not in the swatch book.
     *
     * If you set this property exactly to an RGB color that is in the
     * swatch book, this particular color patch will show a selection mark.
     * Otherwise, no selection mark will be visible.
     */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

    /*!
     * \property PerceptualColor::SwatchBook::editable
     *
     * \brief Indicates whether the user can add or remove colors in the swatch
     * book.
     *
     * When \l editable, the user can, for example, assign the \l currentColor
     * to empty color patches by a left mouse click or remove colors from
     * existing patches by a right mouse click.
     *
     * \image SwatchBookEditable.png "Editable \l SwatchBook"
     *
     * When not \l editable, the user can still select different colors, but
     * modifications to the color patches are disabled.
     */
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)

    /*!
     * \property PerceptualColor::SwatchBook::swatchGrid
     *
     * \brief The \l swatchGrid that is displayed in the swatch book.
     *
     * The colors are in the current color engine’s working gamut. The first
     * dimension \l Array2D::iCount() is interpreted as horizontal axis
     * from left to right on LTR layouts, and the other way around on RTL
     * layouts. The second dimension \l Array2D::jCount() of the array
     * is interpreted as vertical axis, from top to bottom.
     */
    Q_PROPERTY(QColorArray2D swatchGrid READ swatchGrid WRITE setSwatchGrid NOTIFY swatchGridChanged)

public:
    Q_INVOKABLE explicit SwatchBook(const PerceptualColor::QColorArray2D &swatchGrid, Qt::Orientations wideSpacing, QWidget *parent = nullptr);
    virtual ~SwatchBook() noexcept override;
    /*!
     * \brief Getter for property \l currentColor
     *
     * Returns the property value.
     */
    [[nodiscard]] QColor currentColor() const;
    /*!
     * \brief Getter for property \l editable
     *
     * Returns the property value.
     */
    [[nodiscard]] bool isEditable() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    /*!
     * \brief Getter for property \l swatchGrid
     *
     * Returns the property value.
     */
    [[nodiscard]] QColorArray2D swatchGrid() const;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColor(const QColor &newCurrentColor);
    void setEditable(const bool newEditable);
    void setSwatchGrid(const PerceptualColor::QColorArray2D &newSwatchGrid);

Q_SIGNALS:
    /*!
     * \brief Notify signal for property \l currentColor.
     *
     * \a newCurrentColor the new \l currentColor
     */
    void currentColorChanged(const QColor &newCurrentColor);
    /*!
     * \brief Notify signal for property \l editable.
     *
     * \a newEditable the new \l editable
     */
    void editableChanged(const bool newEditable);
    /*!
     * \brief Notify signal for property \l swatchGrid.
     *
     * \a newSwatchGrid the new \l swatchGrid
     */
    void swatchGridChanged(const PerceptualColor::QColorArray2D &newSwatchGrid);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Q_DISABLE_COPY(SwatchBook)

    friend class SwatchBookPrivate;
    /*!
     * \brief Pointer to implementation (pimpl)
     */
    ConstPropagatingUniquePointer<SwatchBookPrivate> d_pointer;

    friend class TestSwatchBook;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_SWATCHBOOK_H
