// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef COLORPATCH_P_H
#define COLORPATCH_P_H

// Include the header of the public class of this private implementation.
// #include "colorpatch.h"

#include "constpropagatingrawpointer.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qpixmap.h>
#include <qpoint.h>
class QLabel;

namespace PerceptualColor
{

class ColorPatch;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ColorPatchPrivate final
{
public:
    /** @brief Constructor */
    explicit ColorPatchPrivate(ColorPatch *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ColorPatchPrivate() noexcept = default;

    /** @brief Initial cursor position (at the time of the left-click)
     * where a drag-and-drop action starts.
     *
     * @sa @ref ColorPatch::mousePressEvent
     * @sa @ref ColorPatch::mouseMoveEvent */
    QPoint dragStartPosition;

    /** @brief Internal storage for property @ref ColorPatch::color
     *
     * QColor automatically initializes with an invalid color, just like it
     * should be for the property @ref ColorPatch::color, so no need to
     * initialize here explicitly. */
    QColor m_color;
    /** @brief The QLabel widget that is used to display the color. */
    QLabel *m_label;

    [[nodiscard]] static QImage renderImage(const int width,
                                            const int height,
                                            const qreal devicePixelRatioF,
                                            const QColor color,
                                            const int lineWidth,
                                            const QColor lineColor,
                                            const Qt::LayoutDirection layoutDirection);
    [[nodiscard]] QPixmap renderPixmap(const int width, const int height);
    void updatePixmap();

private:
    Q_DISABLE_COPY(ColorPatchPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorPatch> q_pointer;
};

} // namespace PerceptualColor

#endif // COLORPATCH_P_H
