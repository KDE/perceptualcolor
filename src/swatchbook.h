// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SWATCHBOOK_H
#define SWATCHBOOK_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
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
#else
#include <qobjectdefs.h>
#include <qstring.h>
class QObject;
#endif

namespace PerceptualColor
{

template<typename T>
class Array2D;
class RgbColorSpace;
class SwatchBookPrivate;

/** @internal
 *
 * @brief Shows colors swatches.
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
 * @todo A design question: Should we draw margins around each individual
 * color patch? Maybe rely on @ref ColorPatch somehow?
 *
 * @todo A design question: Should the size of the individual color patches
 * be responsive, adopting the the widget size? */
class SwatchBook : public AbstractDiagram
{
    Q_OBJECT

    /** @brief The current color.
     *
     * This property can contain any valid color, including colors
     * that are not in the swatch book.
     *
     * If you set this property exactly to an RGB color that is in the
     * swatch book, this particular swatch will show a selection mark.
     * Otherwise, no selection mark will be visible.
     *
     * @sa @ref currentColor() const
     * @sa @ref setCurrentColor()
     * @sa @ref currentColorChanged() */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged)

public:
    Q_INVOKABLE explicit SwatchBook(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                                    const Array2D<QColor> &swatches,
                                    Qt::Orientations wideSpacing,
                                    QWidget *parent = nullptr);
    virtual ~SwatchBook() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    [[nodiscard]] QColor currentColor() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setCurrentColor(const QColor &newCurrentColor);

Q_SIGNALS:
    /** @brief Notify signal for property @ref currentColor.
     *
     * @param newCurrentColor the new @ref currentColor */
    void currentColorChanged(const QColor &newCurrentColor);

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

#endif // SWATCHBOOK_H
