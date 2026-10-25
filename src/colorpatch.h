// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORPATCH_H
#define PERCEPTUALCOLOR_COLORPATCH_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "internalimportexport.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qsize.h>
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWidget;
#include <qtmetamacros.h>

namespace PerceptualColor
{
class ColorPatchPrivate;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A color display widget.
 *
 * This widget simply displays a color. And it provides drag-and-drop support;
 * it emits also colorChanged on drop events if the color has changed.
 * Useful for showing a given color. The instantiation and usage is simple.
 * Example:
 *
 * \snippet testcolorpatch.cpp ColorPatch Create widget
 *
 * \image ColorPatch.png "ColorPatch"
 *
 * This widget is also able to display transparency (the pattern will be
 * mirrored on right-to-left layouts):
 *
 * \image ColorPatchSemiTransparent.png "ColorPatch with 50% transparency"
 * There is also a simple representation
 * for \c {QColor::isValid() == false}:
 *
 * \image ColorPatchInvalid.png "ColorPatch with invalid color"
 *
 * The default minimum size of this widget is similar to a
 * \c {QToolButton}. Depending on your use case, you might
 * want to set a bigger minimum size:
 *
 * \snippet testcolorpatch.cpp ColorPatch Bigger minimum size
 *
 * This widget supports drag-and-drop operations.
 * Outgoing drag events are always enabled.
 * Whether incoming drop events are accepted can be controlled
 * via QWidget::setAcceptDrops(bool).
 *
 * \note This class is API-compatible with the legacy
 * \l {https://api.kde.org/legacy/4.12-api/kdelibs-apidocs/kdeui/html/classKColorPatch.html}
 * {KColorPatch} class that was available in KDElibs4 (and still in KF5
 * through KDELibs4Support).
 *
 *
 *
 * \note This class intentionally does not reimplement the paint event,
 * but uses a child QLabel to display the color. QLabel integrates by
 * default nicely with QStyle: Even round frames that are cutting slightly
 * the pixmap (like in some styles), are possible. So we rely entirely
 * on QLabel for the actual display, and only implement \l sizeHint() and
 * minimumSizeHint() ourselves. */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT ColorPatch : public AbstractDiagram
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::ColorPatch::color
     *
     * \brief The color that is displayed
     *
     * Default value is an invalid color.
     *
     * \list
     * \li If the color is valid, the widget frame is filled with this color.
     *   If this color is not fully opaque, the background behind the color
     *   will be a special background pattern (and \e {not} the default
     *   widget background).
     * \li If the color is invalid, this is represented by a a special
     *   appearance of the widget.
     * \endlist
     *
     * \note No color management is applied. The color is used as-is to
     * paint on the canvas provided by the operation system.
     */
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    Q_INVOKABLE explicit ColorPatch(QWidget *parent = nullptr);
    virtual ~ColorPatch() noexcept override;
    /*!
     * \brief Getter for property color
     */
    [[nodiscard]] QColor color() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setColor(const QColor &newColor);

Q_SIGNALS:
    /*!
     * \brief Notify signal for property color.
     *
     * \a color the new color
     */
    void colorChanged(const QColor &color);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *eventParameter) override;
    virtual void dropEvent(QDropEvent *eventParameter) override;
    virtual bool event(QEvent *eventParameter) override;
    virtual void execDrag(QPoint startPosition) override;
    virtual void paintEvent(QPaintEvent *eventParameter) override;
    virtual void resizeEvent(QResizeEvent *eventParameter) override;

private:
    Q_DISABLE_COPY(ColorPatch)

    friend class ColorPatchPrivate;
    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ColorPatchPrivate> d_pointer;

    friend class TestColorPatch;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORPATCH_H
