// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_SWATCHBOOK_H
#define PERCEPTUALCOLOR_SWATCHBOOK_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "helper.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qsharedpointer.h>
#include <qsize.h>
class QEvent;
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QWidget;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{

class RgbColorSpace;
class SwatchBookPrivate;

/** @internal
 *
 * @brief Shows colors patches.
 *
 * @image html SwatchBook.png "SwatchBook"
 *
 * The user can select a color either by mouse click or by using the keyboard.
 *
 * The marker used to mark the currently selected color depends
 * on the current translation; see @ref setTranslation for details.
 *
 * @internal
 *
 * @note Using Qt's use Model/View architecture instead of @ref QColorArray2D
 * would propably not be worth the added complexity.
 */
class SwatchBook : public AbstractDiagram
{
    Q_OBJECT

    /** @brief The current color.
     *
     * This property can contain any valid color, including colors
     * that are not in the swatch book.
     *
     * If you set this property exactly to an RGB color that is in the
     * swatch book, this particular color patch will show a selection mark.
     * Otherwise, no selection mark will be visible.
     *
     * @sa @ref currentColor() const
     * @sa @ref setCurrentColor()
     * @sa @ref currentColorChanged() */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

    /**
     * @brief Indicates whether the user can add or remove colors in the swatch
     * book.
     *
     * When editable, the user can, for example, assign the @ref currentColor
     * to empty color patches by a left mouse click or remove colors from
     * existing patches by a right mouse click.
     *
     * @image html SwatchBookEditable.png "Editable SwatchBook"
     *
     * When not editable, the user can still select different colors, but
     * modifications to the color patches are disabled.
     *
     * @sa @ref isEditable() const
     * @sa @ref setEditable()
     * @sa @ref editableChanged()
     */
    Q_PROPERTY(bool editable READ isEditable WRITE setEditable NOTIFY editableChanged)

    /** @brief The swatchGrid that is displayed in the swatch book.
     *
     * The colors are in the current color space. The first dimension
     * (@ref Array2D::iCount()) is interpreted as horizontal axis from
     * left to right on LTR layouts, and the other way around on RTL
     * layouts. The second dimension of the array (@ref Array2D::jCount())
     * is interpreted as vertical axis, from top to bottom.
     *
     * @sa @ref swatchGrid() const
     * @sa @ref setSwatchGrid()
     * @sa @ref swatchGridChanged() */
    Q_PROPERTY(QColorArray2D swatchGrid READ swatchGrid WRITE setSwatchGrid NOTIFY swatchGridChanged)

public:
    Q_INVOKABLE explicit SwatchBook(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                                    const PerceptualColor::QColorArray2D &swatchGrid,
                                    Qt::Orientations wideSpacing,
                                    QWidget *parent = nullptr);
    virtual ~SwatchBook() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    [[nodiscard]] QColor currentColor() const;
    /** @brief Getter for property @ref editable
     *  @returns the property @ref editable */
    [[nodiscard]] bool isEditable() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    /** @brief Getter for property @ref swatchGrid
     *  @returns the property @ref swatchGrid */
    [[nodiscard]] QColorArray2D swatchGrid() const;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColor(const QColor &newCurrentColor);
    void setEditable(const bool newEditable);
    void setSwatchGrid(const PerceptualColor::QColorArray2D &newSwatchGrid);

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColor.
     *
     * @param newCurrentColor the new @ref currentColor */
    void currentColorChanged(const QColor &newCurrentColor);
    /** @brief Notify signal for property @ref editable.
     *
     * @param newEditable the new @ref editable */
    void editableChanged(const bool newEditable);
    /** @brief Notify signal for property @ref swatchGrid.
     *
     * @param newSwatchGrid the new @ref swatchGrid */
    void swatchGridChanged(const PerceptualColor::QColorArray2D &newSwatchGrid);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;

private:
    Q_DISABLE_COPY(SwatchBook)

    /** @internal
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class SwatchBookPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<SwatchBookPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestSwatchBook;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_SWATCHBOOK_H
