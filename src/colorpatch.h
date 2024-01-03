// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef COLORPATCH_H
#define COLORPATCH_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qsize.h>
class QDragEnterEvent;
class QDropEvent;
class QMouseEvent;
class QResizeEvent;
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
class ColorPatchPrivate;

/** @brief A color display widget.
 *
 * This widget simply displays a color. And it provides drag-and-drop support;
 * it emits also @ref colorChanged on drop events if the color has changed.
 * Useful for showing a given color. The instantiation and usage is simple.
 * Example:
 * @snippet testcolorpatch.cpp ColorPatch Create widget
 *
 * @image html ColorPatch.png "ColorPatch" width=50
 *
 * This widget is also able to display transparency (the pattern will be
 * mirrored on right-to-left layouts):
 *
 * @image html ColorPatchSemiTransparent.png "ColorPatch with 50% transparency" width=50
 *
 * There is also a simple representation
 * for <tt>QColor::isValid() == false</tt>:
 *
 * @image html ColorPatchInvalid.png "ColorPatch with invalid color" width=50
 *
 * The default minimum size of this widget is similar to a
 * <tt>QToolButton</tt>. Depending on your use case, you might
 * set a bigger minimum size:
 * @snippet testcolorpatch.cpp ColorPatch Bigger minimum size
 *
 * This class’s API is similar to KColorPatch’s API.
 *
 * @internal
 *
 * @note A similar functionality is available as KColorPatch, but this is
 * part of KDELibs4Support which is available in KF5 but should not be used
 * for new code, but only for legacy code. Also, depending on KDELibs4Support
 * would pull-in another dependency. Furthermore, KColorPatch has support for
 * drag-and-drop, which is not desirable for our use case. Therefore, with
 * @ref ColorPatch there is a lightweight alternative (that, by the way,
 * implements the full  API of KColorPatch).
 *
 * @note This class intentionally does not reimplement the paint event,
 * but uses a child QLabel to display the color. QLabel integrates by
 * default nicely with QStyle: Even round frames that are cutting slightly
 * the pixmap (like in the Breeze style), are possible. So we rely entirely
 * on QLabel for the actual display, and only implement @ref sizeHint() and
 * @ref minimumSizeHint() ourselves. */
class PERCEPTUALCOLOR_IMPORTEXPORT ColorPatch : public AbstractDiagram
{
    Q_OBJECT

    /** @brief The color that is displayed
     *
     * Default value is an invalid color.
     *
     * - If the color is valid, the widget frame is filled with this color.
     *   If this color is not fully opaque, the background behind the color
     *   will be a special background pattern (and <em>not</em> the default
     *   widget background).
     * - If the color is invalid, this is represented by a a special
     *   appearance of the widget.
     *
     * @note No color management is applied. The color is used as-is to
     * paint on the canvas provided by the operation system.
     *
     * @sa @ref color() const
     * @sa @ref setColor()
     * @sa @ref colorChanged() */
    Q_PROPERTY(QColor color READ color WRITE setColor NOTIFY colorChanged)

public:
    Q_INVOKABLE explicit ColorPatch(QWidget *parent = nullptr);
    virtual ~ColorPatch() noexcept override;
    /** @brief Getter for property @ref color
     *  @returns the property @ref color */
    [[nodiscard]] QColor color() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] virtual QSize sizeHint() const override;

public Q_SLOTS:
    void setColor(const QColor &newColor);

Q_SIGNALS:
    /** @brief Notify signal for property @ref color.
     *
     * @param color the new color */
    void colorChanged(const QColor &color);

protected:
    virtual void dragEnterEvent(QDragEnterEvent *event) override;
    virtual void dropEvent(QDropEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;

private:
    Q_DISABLE_COPY(ColorPatch)

    /** @internal
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ColorPatchPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ColorPatchPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestColorPatch;
};

} // namespace PerceptualColor

#endif // COLORPATCH_H
