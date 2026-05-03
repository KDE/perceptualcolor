// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromalightnessimageparameters.h"

#include "absolutecolor.h"
#include "asyncimagerendercallback.h"
#include "genericcolor.h"
#include "helper.h"
#include "helperimage.h"
#include "helpermath.h"
#include "lchvalues.h"
#include <atomic>
#include <functional>
#include <qimage.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qpoint.h>
#include <qrgb.h>
#include <qrunnable.h>
#include <qsemaphore.h>
#include <qthreadpool.h>
#include <type_traits>
#include <utility>

namespace PerceptualColor
{

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
void ChromaLightnessImageParameters::renderByRow( //
    uchar *const bytesPtr,
    const qsizetype bytesPerLine,
    const ChromaLightnessImageParameters parameters, // clazy:exclude=function-args-by-ref
    const int firstRow,
    const int lastRow)
{
    const LchValues ranges = //
        (parameters.projectionSpace == LchSpace::Oklch) //
        ? oklchValues //
        : cielchD50Values;
    QRgb rgbColor;
    GenericColor lch;
    lch.third = normalizedAngle360(parameters.hue);
    for (int y = firstRow; y <= lastRow; ++y) {
        QRgb *line = //
            reinterpret_cast<QRgb *>(bytesPtr + y * bytesPerLine);
        lch.first = ranges.maximumLightness - (y + 0.5) * ranges.maximumLightness / parameters.imageSizePhysical.height();
        for (int x = 0; x < parameters.imageSizePhysical.width(); ++x) {
            // Using the same scale as on the y axis. floating point
            lch.second = (x + 0.5) * ranges.maximumLightness / parameters.imageSizePhysical.height();
            const auto lab = AbsoluteColor::fromPolarToCartesian(lch);
            if (parameters.projectionSpace == LchSpace::Oklch) {
                rgbColor = AbsoluteColor::fastFromOklabToSRgbOrTransparent(lab);
            } else {
                rgbColor = //
                    AbsoluteColor::fromCielabD50ToSRgbOrTransparent(lab);
            }
            if (qAlpha(rgbColor) != 0) {
                line[x] = rgbColor;
                // If color is out-of-gamut: We have chroma on the
                // x axis and lightness on the y axis. We are drawing
                // the pixmap line per line, so we go for given
                // lightness from low chroma to high chroma. Because of
                // the nature of many gamuts, if once in a line we have
                // an out-of-gamut value, often all other pixels that
                // are more at the right will be out-of-gamut also. So
                // we could optimize our code and break here. But as we
                // are not sure about this: It’s just likely, but not
                // always correct. We do not know the gamut at compile
                // time, so for the moment we do not optimize the code.
            }
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
 *
 * @todo SHOULDHAVE Interlacing support (for faster reactions)
 */
void ChromaLightnessImageParameters::render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)
{
    if (!variantParameters.canConvert<ChromaLightnessImageParameters>()) {
        return;
    }
    const ChromaLightnessImageParameters parameters = //
        variantParameters.value<ChromaLightnessImageParameters>();

    // Create a new QImage with correct image size.
    QImage myImage(QSize(parameters.imageSizePhysical), //
                   QImage::Format_ARGB32_Premultiplied);
    // Test if image size is empty.
    if (myImage.size().isEmpty()) {
        // The image must be non-empty (otherwise, our algorithm would
        // crash because of a division by 0).
        callbackObject.deliverInterlacingPass( //
            myImage, //
            QImage(), //
            QVariant::fromValue(parameters), //
            AsyncImageRenderCallback::InterlacingState::Final);
        return;
    }

    myImage.fill(Qt::transparent); // Initialize background color

    // Initialization
    const int imageHeight = parameters.imageSizePhysical.height();
    auto &poolReference = getLibraryQThreadPoolInstance();
    const auto threadCount = qMax(1, poolReference.maxThreadCount());

    // Paint the gamut.
    const double normalizedHue = normalizedAngle360(parameters.hue);
    uchar *const bytesPtr = myImage.bits();
    const qsizetype bytesPerLine = myImage.bytesPerLine();

    {
        const auto segments = splitElements(imageHeight, threadCount);
        // The narrowing static_cast<int>() is okay because parts.size() is a
        // result of threadCount, which is also int.
        static_assert( //
            std::is_same_v<std::remove_cv_t<decltype(threadCount)>, int>);
        const int segmentsCount = static_cast<int>(segments.size());
        QSemaphore semaphore(0);
        if (callbackObject.shouldAbort()) {
            return;
        }
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

    // A 1-bit mask for the gamut.
    // transparent = white
    // opaque = black
    const auto myMask = myImage.createAlphaMask();

    callbackObject.deliverInterlacingPass( //
        myImage, //
        myMask, //
        QVariant::fromValue(parameters), //
        AsyncImageRenderCallback::InterlacingState::Intermediate);

    // From Qt Example’s documentation:
    //
    //     “If we discover […] that restart has been set
    //      to true (by render()), we break out […] immediately […].
    //      Similarly, if we discover that abort has been set
    //      to true (by the […] destructor), we return from the
    //      function immediately […].”
    //
    // Strategic Abort Handling for Enhanced UI Responsivity:
    // We intentionally check for restart/abort only *after* the first
    // interlacing pass. This guarantees that at least one image is
    // rendered and shown in the widget, so the UI appears responsive
    // even while the user is interacting (e.g. dragging the hue slider).
    // If we allowed abort earlier, rapid user input could prevent any
    // image from ever being displayed. While the resulting image may be
    // slightly outdated, it maintains the perception of a fluid, reactive
    // interface.
    //
    // After the first pass we may skip the remaining work (such as
    // anti‑aliasing) while the user is still changing the slider, because
    // those steps are comparatively expensive and not critical for
    // immediate feedback. Once the user stops interacting, the remaining
    // passes (including full anti‑aliasing) will be completed and the
    // final image delivered.
    if (callbackObject.shouldAbort()) {
        return;
    }

    // Anti-aliasing
    QList<QPoint> antiAliasCoordinates = findBoundary(myImage);

    // cppcheck-suppress knownConditionTrueFalse // false positive
    if (callbackObject.shouldAbort()) {
        return;
    }

    std::function<QRgb(const double x, const double y)> myColorFunction;
    if (parameters.projectionSpace == LchSpace::Oklch) {
        myColorFunction = [normalizedHue, imageHeight] //
            (const double colorFunctionX, const double colorFunctionY) -> QRgb {
            GenericColor oklch;
            oklch.third = normalizedHue;
            constexpr double maxLight = oklchValues.maximumLightness;
            oklch.first = //
                maxLight - (colorFunctionY + 0.5) * maxLight / imageHeight;
            oklch.second = (colorFunctionX + 0.5) * maxLight / imageHeight;
            return AbsoluteColor::fastFromOklabToSRgbOrTransparent( //
                AbsoluteColor::fromPolarToCartesian(oklch));
        };
    } else {
        myColorFunction = [normalizedHue, imageHeight] //
            (const double colorFunctionX, const double colorFunctionY) -> QRgb {
            GenericColor myCielchD50;
            myCielchD50.third = normalizedHue;
            constexpr double maxLight = cielchD50Values.maximumLightness;
            myCielchD50.first = //
                maxLight - (colorFunctionY + 0.5) * maxLight / imageHeight;
            myCielchD50.second = //
                (colorFunctionX + 0.5) * maxLight / imageHeight;
            return AbsoluteColor::fromCielabD50ToSRgbOrTransparent( //
                AbsoluteColor::fromPolarToCartesian(myCielchD50));
        };
    }
    doAntialias(myImage, antiAliasCoordinates, myColorFunction);

    if (callbackObject.shouldAbort()) {
        return;
    }

    callbackObject.deliverInterlacingPass( //
        myImage, //
        myMask, //
        QVariant::fromValue(parameters), //
        AsyncImageRenderCallback::InterlacingState::Final);
}

} // namespace PerceptualColor
