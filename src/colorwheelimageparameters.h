// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORWHEELIMAGEPARAMETERS_H
#define PERCEPTUALCOLOR_COLORWHEELIMAGEPARAMETERS_H

#include "asyncimageprovider.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qimage.h>

namespace PerceptualColor
{

/** @internal
 *
 *  @brief Parameters for generating a color wheel image.
 *
 * Intended for use with @ref AsyncImageProvider.
 *
 * The image is always square, with its center representing the origin
 * of a polar coordinate system. Each pixel corresponds to a specific
 * hue angle. The pixel encodes the color that has the maximum chroma
 * available in the selected gamut for that hue. This encoding is
 * independent of the radius, so only the hue determines the chosen color.
 *
 * To render an actual wheel, define a clipping path with QPainter and
 * draw the image within this path. Instead of using this class directly,
 * prefer @ref ColorWheelImageProvider, which offers
 * @ref ColorWheelImageProvider::drawColorWheel(). This helper takes care of
 * image alignment, devicePixelRatioF handling, and scaling when needed.
 * It also uses a single image cache across the library to avoid redundant
 * re‑rendering for multiple widgets.
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 */
class ColorWheelImageParameters
{
public:
    explicit ColorWheelImageParameters();

    static void render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject);

    /**
     * @brief Equal operator
     *
     * @param other The object to compare with.
     *
     * @returns <tt>true</tt> if equal, <tt>false</tt> otherwise.
     */
    [[nodiscard]] bool operator==(const ColorWheelImageParameters &other) const = default; // clazy:exclude=function-args-by-value

    /**
     * @brief Unequal operator
     *
     * @param other The object to compare with.
     *
     * @returns <tt>true</tt> if unequal, <tt>false</tt> otherwise.
     */
    [[nodiscard]] bool operator!=(const ColorWheelImageParameters &other) const = default; // clazy:exclude=function-args-by-value

    /**
     * @brief The image size (actual width and height measured in pixel).
     */
    int imageSizePhysical = 0;

    /**
     * @brief The color space into which the working space is projected.
     */
    PerceptualColor::LchSpace projectionSpace = PerceptualColor::LchSpace::CielchD50;

private:
    /** @internal @brief Only for unit tests. */
    friend class TestColorWheelImageParameters;

    static void
    renderByRow(uchar *const bytesPtr, const qsizetype bytesPerLine, const ColorWheelImageParameters parameters, const int firstRow, const int lastRow);
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ColorWheelImageParameters)

#endif // PERCEPTUALCOLOR_COLORWHEELIMAGEPARAMETERS_H
