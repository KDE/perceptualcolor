// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORWHEELIMAGEPROVIDER_H
#define PERCEPTUALCOLOR_COLORWHEELIMAGEPROVIDER_H

#include "abstractdiagram.h"
#include "asyncimageprovider.h"
#include "colorwheelimageparameters.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qimage.h>
#include <qpainter.h>
#include <qpainterpath.h>

namespace PerceptualColor
{

/*!
 * \inmodule ModulePrivateApi
 *
 * \class PerceptualColor::ColorWheelImageProvider
 *
 * \brief Provides cached color wheel images for rendering.
 *
 * This template class manages the generation, caching, and drawing of
 * color wheel images via \l ColorWheelImageParameters. It makes possible
 * that images are rendered efficiently and reused across multiple widgets,
 * avoiding redundant recalculations.
 *
 * The cache is shared across all widgets using this provider. Larger
 * images are never discarded once generated, so subsequent requests can
 * reuse them without recomputation.
 *
 * \a projectionSpace The projection space used for
 * generating the wheel image.
 *
 * Usage:
 *
 * \list
 * \li First, call \l connectPaintEvent to connect the provider’s asynchronous
 *   rendering updates to a diagram’s paint event.
 * \li Call \l drawColorWheel from your \c {paintEvent()} implementation
 *   to draw the wheel with proper clipping, scaling, and HiDPI handling.
 * \endlist
 *
 * The cache is cross‑widget and persists for the lifetime of the
 * application. Larger cached images are retained and reused; they are never
 * replaced by smaller ones.
 */
template<PerceptualColor::LchSpace projectionSpace>
class ColorWheelImageProvider
{
public:
    /*!
     * \brief Connects asynchronous rendering updates to a diagram.
     *
     * Whenever the underlying image provider completes an interlacing pass,
     * this connection triggers \c {AbstractDiagram::update()} on the
     * specified diagram.
     *
     * \a diagram Pointer to the diagram that should receive update
     * notifications. If \c {nullptr}, no connection is made.
     */
    static void connectPaintEvent(const AbstractDiagram *diagram)
    {
        if (!diagram) {
            return;
        }
        diagram->connect( //
            &getImageSingleton(), //
            &AsyncImageProvider<ColorWheelImageParameters>::interlacingPassCompleted, //
            diagram,
            qOverload<>(&AbstractDiagram::update));
    }

    /*!
     * \brief Draws the color wheel into the given QPainter.
     *
     * Intended for use inside the \c {paintEvent()} of classes derived
     * from \l AbstractDiagram.
     *
     * The function retrieves the currently cached wheel image. If the cached
     * image is larger than required, only the necessary portion is used. If
     * it is smaller, it is still drawn (scaled up), but a new asynchronous
     * rendering pass is triggered to generate a higher‑resolution version.
     * When the rendering completes, and if \l connectPaintEvent has been
     * used, the diagram will automatically receive an \c {update()}.
     *
     * \a painter Reference to the QPainter used for drawing.
     *
     * \a devicePixelRatioF The current device pixel ratio (for HiDPI
     * rendering).
     *
     * \a wheelCenterPoint Center of the wheel in painter coordinates.
     *
     * \a outerRadius Outer radius of the wheel.
     *
     * \a gradientThickness Thickness of the gradient ring (difference
     * between outer and inner radius).
     */
    static void
    drawColorWheel(QPainter &painter, const qreal devicePixelRatioF, const QPointF wheelCenterPoint, const double outerRadius, const int gradientThickness)
    {
        painter.save();

        painter.setRenderHint(QPainter::Antialiasing, true);

        const double innerRadius = outerRadius - gradientThickness;

        const int requiredWheelImageSize = //
            qCeil(outerRadius * 2 * devicePixelRatioF);

        QPainterPath ring;
        ring.addEllipse(wheelCenterPoint, outerRadius, outerRadius);
        if (innerRadius > 0) {
            ring.addEllipse(wheelCenterPoint, innerRadius, innerRadius);
        }
        painter.setClipPath(ring);

        ColorWheelImageParameters parameters = getImageSingleton().imageParameters();
        if (parameters.imageSizePhysical < requiredWheelImageSize) {
            parameters.imageSizePhysical = requiredWheelImageSize;
            getImageSingleton().setImageParameters(parameters);
            getImageSingleton().refreshAsync();
        }
        auto wheelImage = getImageSingleton().getCache();
        wheelImage.setDevicePixelRatio(devicePixelRatioF);
        const auto imageSize = wheelImage.width(); // width and height are equal
        QTransform transform;
        const auto oversizeShift = //
            (imageSize > requiredWheelImageSize) //
            ? (imageSize - requiredWheelImageSize) / (-2) / devicePixelRatioF //
            : 0;
        const auto xShift = wheelCenterPoint.x() - outerRadius + oversizeShift;
        const auto yShift = wheelCenterPoint.y() - outerRadius + oversizeShift;
        transform.translate(xShift, yShift);
        if ((imageSize < requiredWheelImageSize) && (imageSize > 0)) {
            wheelImage = wheelImage.scaled(requiredWheelImageSize, //
                                           requiredWheelImageSize, //
                                           Qt::IgnoreAspectRatio, //
                                           Qt::SmoothTransformation);
        }
        painter.setWorldTransform(transform, true);

        painter.drawImage(QPointF(0, 0), wheelImage);

        painter.restore();
    }

private:
    friend class TestColorWheelImageProvider;

    /*!
     *
     *
     * \brief Constructor is private.
     */
    explicit ColorWheelImageProvider() = default;

    /*!
     *
     *
     * \brief Destructor is private.
     */
    ~ColorWheelImageProvider() = default;

    /*!
     *
     *
     * \brief Initial size used for the first image calculation.
     *
     * This value is chosen to be as small as possible to ensure fast
     * rendering, while still large enough to provide a base image with
     * sufficient resolution. The goal is that scaling to typical screen sizes
     * with Qt::SmoothTransformation produces results without noticeable pixel
     * artifacts.
     */
    static constexpr int startSize = 50;

    /*!
     *
     *
     * \brief Returns the singleton instance of the underlying actual image
     * provider.
     *
     * Initializes the provider with default parameters on first use,
     * including the projection space and the initial image size.
     */
    [[nodiscard]] static AsyncImageProvider<ColorWheelImageParameters> &getImageSingleton()
    {
        static AsyncImageProvider<ColorWheelImageParameters> instance;
        static bool initialized = false;
        if (!initialized) {
            ColorWheelImageParameters parameters;
            parameters.projectionSpace = projectionSpace;
            parameters.imageSizePhysical = startSize;
            instance.setImageParameters(parameters);
            instance.refreshAsync();
            initialized = true;
        }
        return instance;
    }
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORWHEELIMAGEPROVIDER_H
