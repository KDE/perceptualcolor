// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "colorwheelimageparameters.h"

#include "colorspaceinfo.h"
#include "helper.h"
#include "helperconstants.h"
#include "helperimage.h"
#include "helpermath.h"
#include "polarpointf.h"
#include <qbrush.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpainterpath.h>
#include <qpen.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsemaphore.h>
#include <qsize.h>
#include <qthreadpool.h>

namespace PerceptualColor
{
/** @brief Constructor
 */
ColorWheelImageParameters::ColorWheelImageParameters()
{
}

/**
 * @brief Render some rows of the image directly to the buffer.
 *
 * @param bytesPtr Pointer to the image data.
 * @param bytesPerLine Bytes per line of the image data (can be obtained by
 *        QImage)
 * @param parameters The parameters
 * @param firstRow Index of the first row to render. Must be a valid index.
 * @param lastRow Index of the last row to render. Must be a valid index.
 *
 * @pre The parameters must be valid within the image. As this function
 * operates directly on the image data, out-of-bound values will cause
 * undefined behaviour.
 */
// Disable Clazy checks for passing large objects by value. In this function,
// designed for threaded execution, we avoid passing by reference whenever
// possible to prevent potential pitfalls, even though copying by value may
// introduce slight overhead.
void ColorWheelImageParameters::renderByRow( //
    uchar *const bytesPtr,
    const qsizetype bytesPerLine,
    const ColorWheelImageParameters parameters, // clazy:exclude=function-args-by-ref
    const int firstRow,
    const int lastRow)
{
    const qreal center = (parameters.imageSizePhysical - 1) / static_cast<qreal>(2);
    for (int y = firstRow; y <= lastRow; ++y) {
        QRgb *line = reinterpret_cast<QRgb *>(bytesPtr + y * bytesPerLine);
        for (int x = 0; x < parameters.imageSizePhysical; ++x) {
            const PolarPointF polarCoordinates = //
                PolarPointF(QPointF(x - center, center - y));
            const auto hue = polarCoordinates.angleDegree();
            const auto rgbColor = //
                (parameters.projectionSpace == LchSpace::CielchD50) //
                ? ColorSpaceInfo::maxChromaColorByCielchD50Hue360(hue)
                : ColorSpaceInfo::maxChromaColorByOklabHue360(hue);
            line[x] = rgbColor.rgb();
        }
    }
}

/** @brief Render an image.
 *
 * The function will render the image with the given parameters,
 * and deliver the result by means of <tt>callbackObject</tt>.
 *
 * This function is thread-safe as long as each call of this function
 * uses different <tt>variantParameters</tt> and <tt>callbackObject</tt>.
 *
 * @param variantParameters A <tt>QVariant</tt> that contains the
 *        image parameters.
 * @param callbackObject Pointer to the object for the callbacks.
 */
void ColorWheelImageParameters::render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)
{
    if (!variantParameters.canConvert<ColorWheelImageParameters>()) {
        return;
    }
    const ColorWheelImageParameters parameters = //
        variantParameters.value<ColorWheelImageParameters>();

    QImage image = QImage(QSize(parameters.imageSizePhysical, parameters.imageSizePhysical), //
                          QImage::Format_ARGB32_Premultiplied);
    image.fill(Qt::transparent);

    {
        uchar *const bytesPtr = image.bits();
        const qsizetype bytesPerLine = image.bytesPerLine();
        auto &poolReference = getLibraryQThreadPoolInstance();
        const auto threadCount = qMax(1, poolReference.maxThreadCount());
        const auto segments = splitElements(parameters.imageSizePhysical, threadCount);
        // The narrowing static_cast<int>() is okay because parts.size() is a
        // result of threadCount, which is also int.
        static_assert( //
            std::is_same_v<std::remove_cv_t<decltype(threadCount)>, int>);
        const int segmentsCount = static_cast<int>(segments.size());
        QSemaphore semaphore(0);
        std::atomic_thread_fence(std::memory_order_seq_cst); // memory barrier
        for (const auto &segment : segments) {
            const auto myLambda = [bytesPtr, //
                                   bytesPerLine,
                                   parameters,
                                   segment,
                                   &semaphore]() {
                renderByRow(bytesPtr, //
                            bytesPerLine,
                            parameters,
                            segment.first,
                            segment.second);
                semaphore.release();
            };
            const auto myRunnablePtr = QRunnable::create(myLambda);
            poolReference.start(myRunnablePtr, imageThreadPriority);
        }
        // Intentionally acquiring segments.size() and not
        // treadCount,  because they might differ and
        // segments.size() is mandatory for thread execution.
        semaphore.acquire(segmentsCount); // Wait for all threads to finish.
    }

    callbackObject.deliverInterlacingPass( //
        image, //
        QImage(), //
        variantParameters, //
        AsyncImageRenderCallback::InterlacingState::Final);
}

static_assert(std::is_standard_layout_v<ColorWheelImageParameters>);

} // namespace PerceptualColor
