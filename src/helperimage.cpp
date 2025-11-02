// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helperimage.h"

#include "helper.h"
#include <qcolor.h>
#include <qimage.h>
#include <qpoint.h>
#include <qrect.h>
#include <qrgb.h>
#include <qrunnable.h>
#include <qsemaphore.h>
#include <qthreadpool.h>
#include <type_traits>

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Find boundaries between fully opaque and fully transparent pixels.
 *
 * @param image The image to be searched.
 *
 * @note There is no API guarantee regarding the handling of partially
 * transparent pixels — they may be treated as fully opaque or
 * fully transparent.
 *
 * @returns A list of all coordinate points on both sides of the boundary.
 *
 * @note This function is thread-save as long as there is no more than one
 * thread of this function operating on the same data on the same time.
 */
QList<QPoint> findBoundary(const QImage &image)
{
    QList<QPoint> coordinates;
    int width = image.width();
    int height = image.height();
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            if (image.pixelColor(x, y).alpha() != 0) { // gamut body
                // We process only the pixels of the gamut body. A gamut body
                // pixel is added if at least one of its neighbors is a
                // background pixel, along with all neighboring background
                // pixels. This eliminates the need for a second pass to test
                // background pixels.
                // NOTE: The background color may occasionally appear within
                // the gamut body, but such instances are rare and therefore
                // not computationally expensive to handle. In these cases,
                // anti-aliasing has no effect, making it inconsequential to
                // the final image.
                bool hasTransparentNeighbor = false;
                // Check 8 neighbors
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        if (dx == 0 && dy == 0) {
                            continue; // Skip the pixel itself
                        }
                        const auto xOutOfRange = //
                            ((x + dx < 0) || (x + dx >= image.width()));
                        const auto yOutOfRange = //
                            ((y + dy < 0) || (y + dy >= image.height()));
                        if (xOutOfRange || yOutOfRange) {
                            // Out of range
                            continue;
                        }
                        const auto myPixelColor = //
                            image.pixelColor(x + dx, y + dy);
                        if (myPixelColor.alpha() == 0) {
                            hasTransparentNeighbor = true;
                            const QPoint myNeighbor = QPoint(x + dx, y + dy);
                            if (!coordinates.contains(myNeighbor)) {
                                // Add transparent pixel
                                coordinates.append(myNeighbor);
                            }
                        }
                    }
                }
                if (hasTransparentNeighbor) {
                    const auto gamutPixel = QPoint(x, y);
                    if (!coordinates.contains(gamutPixel)) {
                        // Add the gamut body pixel itself
                        coordinates.append(gamutPixel);
                    }
                }
            }
        }
    }
    return coordinates;
}

/**
 * @internal
 *
 * @brief Helper function for @ref doAntialias().
 *
 * Not available outside this translation unit.
 *
 * @param bytesPtr A reference to the bytes of the image provides as
 *        QImage::Format_ARGB32_Premultiplied
 * @param bytesPerLine The number of bytes in a single row of pixels in the
 *        image.
 * @param antiAliasCoordinates A list of pixels for which anti-aliasing should
 *        be done.
 * @param colorFunction A pointer to a function that returns the opaque color
 *        for the given coordinates, or a transparent color if out-of-gamut.
 */
static void doAntialiasHelper(uchar *const bytesPtr,
                              const qsizetype bytesPerLine,
                              const QList<QPoint> &antiAliasCoordinates,
                              const std::function<QRgb(const double x, const double y)> &colorFunction)
{
    QList<QColor> opaqueColors;
    for (const QPoint point : antiAliasCoordinates) {
        // Iterating over a square grid of data points within the given pixel.
        // The side length of the square contains exactly “sideLength” data
        // points. Its square represents the total number of data points,
        // referred to here as “totalDataPoints”. The“ sideLength” is chosen so
        // that the total number of data points is 256, corresponding to the
        // number of possible alpha values in typical 4-byte colors
        // (RGB+Alpha), which is sufficient for this case."
        constexpr int sideLength = 16;
        constexpr int totalDataPoints = sideLength * sideLength;
        opaqueColors.clear();
        opaqueColors.reserve(totalDataPoints);
        constexpr double stepWidth = 1.0 / sideLength;
        {
            const double x = point.x() - 0.5 + stepWidth / 2;
            const double y = point.y() - 0.5 + stepWidth / 2;
            for (int i = 0; i < sideLength; ++i) {
                for (int j = 0; j < sideLength; ++j) {
                    const QRgb tempColor = colorFunction(x + i * stepWidth, //
                                                         y + j * stepWidth);
                    if (qAlpha(tempColor) != 0) {
                        opaqueColors.append(tempColor);
                    }
                }
            }
        }
        if (opaqueColors.size() > 0) {
            const float countF = //
                static_cast<float>(opaqueColors.size());
            QRgb &pixelRef = *( //
                reinterpret_cast<QRgb *>(bytesPtr + point.y() * bytesPerLine) //
                + point.x());
            QRgb opaqueColor = pixelRef;
            if (qAlpha(opaqueColor) == 0) {
                // If the pixel is transparent, the center of the pixel is
                // out-of-gamut, and no actual color is available. For
                // anti-aliasing, we need an actual color, so we calculate the
                // mean color of all other data points within the pixel that
                // actually are in-gamut.
                float r = 0;
                float g = 0;
                float b = 0;
                for (const QColor &myColor : opaqueColors) {
                    r += myColor.redF();
                    g += myColor.greenF();
                    b += myColor.blueF();
                }
                r /= countF;
                g /= countF;
                b /= countF;
                opaqueColor = qRgb(qRound(r * 255), //
                                   qRound(g * 255), //
                                   qRound(b * 255));
            }
            pixelRef = qPremultiply( //
                qRgba(qRed(opaqueColor), //
                      qGreen(opaqueColor), //
                      qBlue(opaqueColor), //
                      qRound(countF / totalDataPoints * 255)));
        }
    }
}

/**
 * @internal
 *
 * @brief Calculates anti-alias for gamut diagrams.
 *
 * Gamut images generated by this library typically exhibit sharp boundaries,
 * where a pixel is either within the gamut (opaque color) or outside it
 * (transparent color). The determination is based on the coordinates at the
 * center of the pixel's square surface.
 *
 * This function is designed to perform anti-aliasing by smoothing the sharp
 * gamut boundaries. To use this function, first obtain a list of candidate
 * pixels for anti-aliasing. These are the pixels surrounding the sharp gamut
 * border, which can be identified using @ref findBoundary(). This function
 * then calculates, within the 1 px × 1 px area of each candidate pixel,
 * multiple data points at a significantly higher resolution than the single
 * data point in the original image. By analyzing this detailed data, the
 * function applies anti-aliasing to smooth the boundary.
 *
 * @note Since this operation is computationally intensive, it is recommended
 * to apply it only to the pixels returned by @ref findBoundary(), rather than
 * the entire image.
 *
 * @param image A reference to he image that should be modified.
 *        Only <tt>QImage::Format_ARGB32_Premultiplied</tt> is supported.
 * @param antiAliasCoordinates A list of pixels for which anti-aliasing should
 *        be done.
 * @param colorFunction A pointer to a function that returns the opaque color
 *        for the given coordinates, or a transparent color if out-of-gamut.
 */
void doAntialias(QImage &image, const QList<QPoint> &antiAliasCoordinates, const std::function<QRgb(const double x, const double y)> &colorFunction)
{
    if (image.format() != QImage::Format_ARGB32_Premultiplied) {
        return;
    }
    uchar *const bytesPtr = image.bits();
    const qsizetype bytesPerLine = image.bytesPerLine();
    auto &poolReference = getLibraryQThreadPoolInstance();
    const int threadCount = qMax(1, poolReference.maxThreadCount());
    const auto parts = splitList(antiAliasCoordinates, threadCount);
    // The narrowing static_cast<int>() is okay because parts.size() is a
    // result of threadCount, which is also int.
    static_assert( //
        std::is_same_v<std::remove_cv_t<decltype(threadCount)>, int>);
    const int partsCount = static_cast<int>(parts.size());
    QSemaphore semaphore(0);
    for (const auto &part : parts) {
        const auto myLambda = [bytesPtr, bytesPerLine, part, colorFunction, &semaphore]() {
            doAntialiasHelper(bytesPtr, bytesPerLine, part, colorFunction);
            semaphore.release();
        };
        const auto myRunnablePtr = QRunnable::create(myLambda);
        poolReference.start(myRunnablePtr, imageThreadPriority);
    }
    // Intentionally acquiring parts.size() and not treadCount, because
    // they might differ and parts.size() is mandatory for thread
    // execution.
    semaphore.acquire(partsCount); // Wait for all threads to finish.
}

/**
 * @internal
 *
 * @brief Get a reference to the singleton instance.
 *
 * This library holds it's own thread pool instead of using the global
 * QThreadPool::globalInstance() because we want to avoid interference with
 * what the library use might do with the global thread pool.
 *
 * @returns A reference to the instance.
 *
 * To use it, assign the return value to a reference (not a normal variable):
 *
 * @snippet testperceptualsettings.cpp PerceptualSettings Instance */
QThreadPool &getLibraryQThreadPoolInstance()
{
    // Pattern: Meyer's singleton.
    static QThreadPool myInstance;
    return myInstance;
}

/**
 * @internal
 *
 * @brief Paints a rectangle.
 *
 * The function replaces the values in the given rectangle directly by the new
 * color value, without any blending.
 *
 * @param bytesPtr Pointer to the QRgb array with the image.
 * @param bytesPerLine Number of bytes per line of the image.
 * @param rectangle The rectangle to draw.
 * @param color The color to draw.
 *
 * @pre The given rectangle must be completely within the boundary of the
 * image.
 *
 * @pre The image raw data must be 32 bit QRgb data.
 */
void fillRect(uchar *const bytesPtr, const qsizetype bytesPerLine, const QRect rectangle, const QRgb color)
{
    const auto xFirst = rectangle.x();
    const auto xLast = xFirst + rectangle.width() - 1;
    const auto yFirst = rectangle.y();
    const auto yLast = yFirst + rectangle.height() - 1;
    for (int y = yFirst; y <= yLast; ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(bytesPtr + y * bytesPerLine);
        for (int x = xFirst; x <= xLast; ++x) {
            line[x] = color;
        }
    }
}

} // namespace PerceptualColor
