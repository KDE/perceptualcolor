﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef COLORWHEELIMAGE_H
#define COLORWHEELIMAGE_H

#include <qglobal.h>
#include <qimage.h>
#include <qsharedpointer.h>

namespace PerceptualColor
{
class RgbColorSpace;

/** @internal
 *
 *  @brief An image of a color wheel.
 *
 * The image has properties that can be accessed by the corresponding setters
 * and getters.
 *
 * This class has a cache. The data is cached because it is expensive to
 * calculate it again and again on the fly.
 *
 * When changing one of the properties, the image is <em>not</em> calculated
 * immediately. But the old image in the cache is deleted, so that this
 * memory becomes immediately available. Once you use @ref getImage() the next
 * time, a new image is calculated and cached. As long as you do not change
 * the properties, the next call of @ref getImage() will be very fast, as
 * it returns just the cache.
 *
 * This class is intended for usage in widgets that need to display a
 * color wheel. It is recommended to update the properties of this class as
 * early as possible: If your widget is resized, use immediately also
 * @ref setImageSize to update this object. (This will reduce your memory
 * usage, as no memory will be hold for data that will not be
 * needed again.)
 *
 * This class supports HiDPI via its @ref setDevicePixelRatioF function.
 *
 * @note Resetting a property to its very same value does not trigger an
 * image calculation. So, if the border is 5, and you call @ref setBorder
 * <tt>(5)</tt>, than this will not trigger an image calculation, but the
 * cache stays valid and available.
 *
 * @note This class is not based on <tt>QCache</tt> or <tt>QPixmapCache</tt>
 * because the semantic is different.
 *
 * @note This class is not part of the public API, but just for internal
 * usage. Therefore, its interface is incomplete and contains only the
 * functions that are really used in the rest of the source code (property
 * setters are available, but getters might be missing), and it does not use
 * the pimpl idiom either.
 *
 * @internal
 *
 * @note We could port this class to @ref AsyncImageProvider with support for
 * interlacing. Alternatively, we could implement a singleton that provides a
 * cached color wheel image shared by both @ref ChromaHueDiagram and
 * @ref ChromaLightnessDiagram (effectively, @ref WheelColorPicker).
 * The cache should ensure that the requested wheel size (defined by inner and
 * outer radius) is fully covered. A single cached image can accommodate
 * multiple wheel sizes, even if they intersect. Colors would be computed only
 * once, and new pixels would be calculated only if they haven't been generated
 * previously. However, this approach introduces significant complexity. Its
 * benefits are limited to scenarios where the image size changes frequently
 * (which is unlikely, as users rarely resize the window) or where multiple
 * widgets use the wheel image simultaneously (also unlikely, as we currently
 * have only two such widgets). Given the limited advantages, this may not
 * justify the added cost.
 */
class ColorWheelImage final
{
public:
    explicit ColorWheelImage(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    [[nodiscard]] QImage getImage();
    void setBorder(const qreal newBorder);
    void setDevicePixelRatioF(const qreal newDevicePixelRatioF);
    void setImageSize(const int newImageSize);
    void setWheelThickness(const qreal newWheelThickness);

private:
    Q_DISABLE_COPY(ColorWheelImage)

    /** @internal @brief Only for unit tests. */
    friend class TestColorWheelImage;

    /** @brief Internal store for the border size, measured in physical pixels.
     *
     * @sa @ref setBorder() */
    qreal m_borderPhysical = 0;
    /** @brief Internal storage of the device pixel ratio as floating point.
     *
     * @sa @ref setDevicePixelRatioF() */
    qreal m_devicePixelRatioF = 1;
    /** @brief Internal storage of the image (cache).
     *
     * - If <tt>m_image.isNull()</tt> than either no cache is available
     *   or @ref m_imageSizePhysical is <tt>0</tt>. Before using it,
     *   a new image has to be rendered. (If @ref m_imageSizePhysical
     *   is <tt>0</tt>, this will be extremely fast.)
     * - If <tt>m_image.isNull()</tt> is <tt>false</tt>, than the cache
     *   is valid and can be used directly. */
    QImage m_image;
    /** @brief Internal store for the image size, measured in physical pixels.
     *
     * @sa @ref setImageSize() */
    int m_imageSizePhysical = 0;
    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief Internal store for the image size, measured in physical pixels.
     *
     * @sa @ref setWheelThickness() */
    qreal m_wheelThicknessPhysical = 0;
};

} // namespace PerceptualColor

#endif // COLORWHEELIMAGE_H
