// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CHROMALIGHTNESSIMAGEPARAMETERS_H
#define CHROMALIGHTNESSIMAGEPARAMETERS_H

#include <qglobal.h>
#include <qmetatype.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qvariant.h>

namespace PerceptualColor
{

class AsyncImageRenderCallback;
class RgbColorSpace;

/** @internal
 *
 *  @brief An image of a chroma-lightness plane.
 *
 * This is a cut through the gamut body at a given hue.
 *
 * For the y axis, its height covers the lightness range [0, 100].
 * Coordinate point <tt>(0)</tt> corresponds to value 100.
 * Coordinate point <tt>height</tt> corresponds to value 0.
 * Its x axis uses always the same scale as the y axis. So if the size
 * is a square, both x range and y range are from 0 to 100. If the
 * width is larger than the height, the x range goes beyond 100. The
 * image paints all the LCH values that are within the gamut and x/y range.
 * Each pixel show the color of the coordinate point at its center. So
 * the pixel at pixel position <tt>(2, 3)</tt> shows the color corresponding
 * to coordinate point <tt>(2.5, 3.5)</tt>.
 *
 * @todo Solve the problem with nearestNeighborSearch to respond immediately,
 * without waiting for the rendering to complete, to avoid using things like
 * <a href="https://api.kde.org/frameworks/kwidgetsaddons/html/classKBusyIndicatorWidget.html">
 * KBusyIndicatorWidget</a>.
 *
 * @note Intentionally there is no anti-aliasing because this would be much
 * slower: As there is no mathematical description of the shape of the color
 * solid, the only easy way to get anti-aliasing would be to render at a
 * higher resolution (say two times higher, which would yet mean four times
 * more data), and then downscale it to the final resolution. This would be
 * too slow. */
class ChromaLightnessImageParameters final
{
public:
    [[nodiscard]] bool operator==(const ChromaLightnessImageParameters &other) const;
    [[nodiscard]] bool operator!=(const ChromaLightnessImageParameters &other) const;
    static void render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject);

    /** @brief The LCH-hue.
     *
     * Valid range: 0° ≤ value < 360° */
    qreal hue = 0;
    /** @brief Image size, measured in physical pixels. */
    QSize imageSizePhysical;
    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> rgbColorSpace;

private:
    /** @internal @brief Only for unit tests. */
    friend class TestChromaLightnessImageParameters;

    /** @brief Calculate one-dimensional index for given <tt>x</tt> and
     * <tt>y</tt> coordinates.
     *
     * @param x The <tt>x</tt> coordinate
     * @param y The <tt>y</tt> coordinate
     * @param imageSizePhysical The image size
     * @returns The corresponding index, assuming a one-dimensional array
     * that contains one element for each pixel, starting with the elements
     * <tt>(0, 0)</tt>, than <tt>(0, 1)</tt> and so on, line by line. */
    [[nodiscard]] static constexpr int maskIndex(const int x, const int y, const QSize imageSizePhysical)
    {
        return x + y * imageSizePhysical.width();
    }
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ChromaLightnessImageParameters)

#endif // CHROMALIGHTNESSIMAGEPARAMETERS_H
