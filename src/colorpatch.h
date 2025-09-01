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
class QPaintEvent;
class QResizeEvent;
class QWidget;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
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
 * @image html ColorPatch.png "ColorPatch"
 *
 * This widget is also able to display transparency (the pattern will be
 * mirrored on right-to-left layouts):
 *
 * @image html ColorPatchSemiTransparent.png "ColorPatch with 50% transparency"
 * There is also a simple representation
 * for <tt>QColor::isValid() == false</tt>:
 *
 * @image html ColorPatchInvalid.png "ColorPatch with invalid color"
 *
 * The default minimum size of this widget is similar to a
 * <tt>QToolButton</tt>. Depending on your use case, you might
 * want to set a bigger minimum size:
 * @snippet testcolorpatch.cpp ColorPatch Bigger minimum size
 *
 * @note This class is API-compatible with the legacy
 * <a href="https://api.kde.org/legacy/4.12-api/kdelibs-apidocs/kdeui/html/classKColorPatch.html">
 * KColorPatch</a> class that was available in KDElibs4 (and still in KF5
 * through KDELibs4Support).
 *
 * @internal
 *
 * @note This class intentionally does not reimplement the paint event,
 * but uses a child QLabel to display the color. QLabel integrates by
 * default nicely with QStyle: Even round frames that are cutting slightly
 * the pixmap (like in some styles), are possible. So we rely entirely
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
    virtual void paintEvent(QPaintEvent *event) override;
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
