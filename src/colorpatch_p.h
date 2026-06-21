// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORPATCH_P_H
#define PERCEPTUALCOLOR_COLORPATCH_P_H

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
class ColorPatchPrivate
{
public:
    /**
     * @brief Parameters for the image.
     */
    struct ImageParameters {
        /**
         * @brief width of the requested image, measured in device-independent
         * pixels.
         */
        int width = 0;
        /**
         * @brief height of the requested image, measured in device-independent
         * pixels.
         */
        int height = 0;
        /**
         * @brief The device pixel ratio of the widget, with floating
         *        point precision
         */
        qreal devicePixelRatioF = 1;
        /**
         * @brief The color to be displayed
         */
        QColor color;
        /**
         * @brief The line width used to draw the mark that symbolized an
         * invalid color, measured in device-independent pixels.
         */
        int lineWidth = 1;
        /**
         * @brief The color used to draw the mark that symbolized an
         * invalid color.
         */
        QColor lineColor;
        /**
         * @brief The layout direction of the widget
         */
        Qt::LayoutDirection layoutDirection = Qt::LayoutDirectionAuto;

        bool operator==(const ImageParameters &other) const;
    };

    explicit ColorPatchPrivate(ColorPatch *backLink);
    virtual ~ColorPatchPrivate() noexcept;

    /** @brief Internal storage for property @ref ColorPatch::color
     *
     * QColor automatically initializes with an invalid color, just like it
     * should be for the property @ref ColorPatch::color, so no need to
     * initialize here explicitly. */
    QColor m_color;
    /** @brief The QLabel widget that is used to display the color. */
    QLabel *m_label;
    /**
     * @brief Cache for the last image parameters that have been shown
     * in @ref m_label.
     */
    ImageParameters m_lastImageParameters = ImageParameters();

    [[nodiscard]] ColorPatchPrivate::ImageParameters getImageParameters(const int width, const int height) const;
    [[nodiscard]] static QImage renderImage(const ImageParameters &parameters);
    [[nodiscard]] QPixmap renderPixmap(const ImageParameters &parameters);
    void updatePixmapIfNecessary();

private:
    Q_DISABLE_COPY(ColorPatchPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorPatch> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORPATCH_P_H
