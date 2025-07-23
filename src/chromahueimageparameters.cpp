// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromahueimageparameters.h"

#include "asyncimagerendercallback.h"
#include "helper.h"
#include "helperconstants.h"
#include "helperimage.h"
#include "helpermath.h"
#include "interlacingpass.h"
#include "rgbcolorspace.h"
#include <atomic>
#include <cmath>
#include <lcms2.h>
#include <qimage.h>
#include <qlist.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qpoint.h>
#include <qrect.h>
#include <qrgb.h>
#include <qrunnable.h>
#include <qsemaphore.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qthreadpool.h>
#include <type_traits>
#include <utility>

namespace PerceptualColor
{
/** @brief Equal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if equal, <tt>false</tt> otherwise. */
bool ChromaHueImageParameters::operator==(const ChromaHueImageParameters &other) const
{
    return ( //
        (borderPhysical == other.borderPhysical) //
        && (devicePixelRatioF == other.devicePixelRatioF) //
        && (imageSizePhysical == other.imageSizePhysical) //
        && (lightness == other.lightness) //
        && (rgbColorSpace == other.rgbColorSpace) //
    );
}

/** @brief Unequal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if unequal, <tt>false</tt> otherwise. */
bool ChromaHueImageParameters::operator!=(const ChromaHueImageParameters &other) const
{
    return !(*this == other);
}

/**
 * @brief A new interlacing object with an appropriate number of interlacing
 * steps.
 *
 * @param imageSizePhysical The size of the image, measured in physical pixels.
 *
 * @returns A new interlacing object with an appropriate number of interlacing
 * steps.
 */
InterlacingPass ChromaHueImageParameters::createInterlacingPassObject(const QSize imageSizePhysical)
{
    // The reference size
    constexpr auto size = 2000;
    constexpr QSize referenceSizePhysical = QSize(size, size);
    constexpr int numberOfPassesAtReferenceSize = 5;

    // Calculate
    static_assert(isOdd(numberOfPassesAtReferenceSize));

    const auto pixelCountImage = //
        imageSizePhysical.width() * imageSizePhysical.height();
    constexpr double pixelCountReference = //
        referenceSizePhysical.width() * referenceSizePhysical.height();
    const double factor = pixelCountImage / pixelCountReference;
    // The number of actual passes
    const auto numberOfPasses = //
        numberOfPassesAtReferenceSize //
        // qMax makes sure std::log2() is never called with a parameter ≤ 0
        + std::log2(qMax(0.01, factor));
    return InterlacingPass(numberOfPasses);
}

/**
 * @brief Render some rows of the image directly to the buffer.
 *
 * @param callbackObject Used to stop rendering when an abort is requested
 * @param bytesPtr Pointer to the image data.
 * @param bytesPerLine Bytes per line of the image data (can be obtained by
 *        QImage)
 * @param parameters The parameters
 * @param shift Shift value
 * @param scaleFactor Scale factor
 * @param currentPass The object providing metrics for the current interlacing
 *        pass
 * @param firstRow Index of the first row to render. Must be a valid index.
 * @param lastRow Index of the last row to render. Must be a valid index.
 *
 * @pre The parameters must be valid within the image. As this function
 * operates directly on the image data, out-of-bound values will cause
 * undefined behaviour.
 *
 * @pre The parameter firstRow must be aligned to the interlacing pass steps.
 * If the interlacing starts for example with 8 x 8 pixels, valid values for
 * the firstRow index are: 0, 8, 16, 32 etc.
 */
// Disable Clazy checks for passing large objects by value. In this function,
// designed for threaded execution, we avoid passing by reference whenever
// possible to prevent potential pitfalls, even though copying by value may
// introduce slight overhead.
void ChromaHueImageParameters::renderByRow( //
    const AsyncImageRenderCallback &callbackObject,
    uchar *const bytesPtr,
    const qsizetype bytesPerLine,
    const ChromaHueImageParameters parameters, // clazy:exclude=function-args-by-ref
    const qreal shift,
    const qreal scaleFactor,
    const InterlacingPass currentPass, // clazy:exclude=function-args-by-ref
    int firstRow,
    int lastRow)
{
    const auto chromaRange = //
        parameters.rgbColorSpace->profileMaximumCielchD50Chroma();
    cmsCIELab cielabD50;
    cielabD50.L = parameters.lightness;
    QRgb tempColor;
    const auto threshold = qPow(chromaRange + overlap, 2);
    for (int y = firstRow + currentPass.lineOffset; //
         y <= lastRow; //
         y += currentPass.lineFrequency) //
    {
        if (callbackObject.shouldAbort()) {
            return;
        }
        cielabD50.b = chromaRange //
            - (y + shift) * scaleFactor;
        const auto rectangleHeight = // Make sure to stay within the image
            qMin(currentPass.rectangleSize.height(), //
                 lastRow + 1 - y);
        for (int x = currentPass.columnOffset; //
             x < parameters.imageSizePhysical; //
             x += currentPass.columnFrequency //
        ) {
            cielabD50.a = (x + shift) * scaleFactor - chromaRange;
            if (qPow(cielabD50.a, 2) + qPow(cielabD50.b, 2) <= threshold) {
                tempColor = //
                    parameters
                        .rgbColorSpace //
                        ->fromCielabD50ToQRgbOrTransparent(cielabD50);
                const auto rectangleWidth =
                    // Make sure to stay within the image
                    qMin(currentPass.rectangleSize.width(), //
                         parameters.imageSizePhysical - x);
                const QRect rect{x, y, rectangleWidth, rectangleHeight};
                if (qAlpha(tempColor) != 0) {
                    // The pixel is within the gamut!
                    fillRect(bytesPtr, bytesPerLine, rect, tempColor);
                } else {
                    fillRect(bytesPtr, bytesPerLine, rect, qRgbTransparent);
                }
            }
        }
    }
}

/** @brief Render an image.
 *
 * The function will render the image with the given parameters,
 * and deliver the result of each interlacing pass and also the final
 * result by means of <tt>callbackObject</tt>.
 *
 * This function is thread-safe as long as each call of this function
 * uses different <tt>variantParameters</tt> and <tt>callbackObject</tt>.
 *
 * @param variantParameters A <tt>QVariant</tt> that contains the
 *        image parameters.
 * @param callbackObject Pointer to the object for the callbacks.
 *
 * @todo Could we get better performance? Even online tools like
 * https://bottosson.github.io/misc/colorpicker/#ff2a00 or
 * https://oklch.evilmartians.io/#65.4,0.136,146.7,100 get quite good
 * performance. How do they do that? */
void ChromaHueImageParameters::render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)
{
    if (!variantParameters.canConvert<ChromaHueImageParameters>()) {
        return;
    }
    const ChromaHueImageParameters parameters = //
        variantParameters.value<ChromaHueImageParameters>();

    // From Qt Example’s documentation:
    //
    //     “If we discover […] that restart has been set
    //      to true (by render()), we break out […] immediately […].
    //      Similarly, if we discover that abort has been set
    //      to true (by the […] destructor), we return from the
    //      function immediately […].”
    if (callbackObject.shouldAbort()) {
        return;
    }
    // Create a new QImage with correct image size.
    QImage myImage(
        // size:
        QSize(parameters.imageSizePhysical, parameters.imageSizePhysical),
        // format:
        QImage::Format_ARGB32_Premultiplied);
    // Calculate the radius of the circle we want to paint (and which will
    // finally have the background color, while everything around will be
    // transparent).
    const qreal circleRadius = //
        (parameters.imageSizePhysical - 2 * parameters.borderPhysical) / 2.;
    if ((circleRadius <= 0) || parameters.rgbColorSpace.isNull()) {
        // The border is too big the and image size too small: The size
        // of the circle is zero. Or: There is no color space with which
        // we can work (and dereferencing parameters.rgbColorSpace will
        // crash).
        // In either case: The image will therefore be transparent.
        // Initialize the image as completely transparent and return.
        myImage.fill(Qt::transparent);
        // Set the correct scaling information for the image and return
        myImage.setDevicePixelRatio(parameters.devicePixelRatioF);
        callbackObject.deliverInterlacingPass( //
            myImage, //
            QImage(), //
            variantParameters, //
            AsyncImageRenderCallback::InterlacingState::Final);
        return;
    }

    // If we continue, the circle will at least be visible.

    // Initialize the hole image background:
    myImage.fill(Qt::transparent);

    // Prepare for gamut painting
    const auto chromaRange = //
        parameters.rgbColorSpace->profileMaximumCielchD50Chroma();
    const qreal scaleFactor = static_cast<qreal>(2 * chromaRange)
        // The following line will never be 0 because we have have
        // tested above that circleRadius is > 0, so this line will
        // we > 0 also.
        / (parameters.imageSizePhysical - 2 * parameters.borderPhysical);

    // Paint the gamut.

    // The pixel at position QPoint(x, y) is the square with the top-left
    // edge at coordinate point QPoint(x, y) and the bottom-right edge at
    // coordinate point QPoint(x+1, y+1). This pixel is supposed to have
    // the color from coordinate point QPoint(x+0.5, y+0.5), which is
    // the middle of this pixel. Therefore, with an offset of 0.5 we
    // can convert from the pixel position to the point in the middle of
    // the pixel.
    constexpr qreal pixelOffset = 0.5;

    const qreal shift = pixelOffset - parameters.borderPhysical;

    InterlacingPass currentPass = createInterlacingPassObject( //
        QSize(parameters.imageSizePhysical, parameters.imageSizePhysical));
    const auto interlacingMaxRasterSize = currentPass.columnFrequency;

    auto &poolReference = getLibraryQThreadPoolInstance();
    const auto threadCount = qMax(1, poolReference.maxThreadCount());

    while (true) {
        if (callbackObject.shouldAbort()) {
            return;
        }

        // Get an up-to-date pointer to the raw image data. It is
        // mandatory to do this again in each loop run, because
        // delivering the intermediate image will likely create shallow
        // and later also deep copies, which may affect where the
        // actual image data is located. By running QImage::bits(), we
        // make sure that the implicit sharing of QImage is detached.
        uchar *const bytesPtr = myImage.bits();
        const qsizetype bytesPerLine = myImage.bytesPerLine();

        const auto segments = splitElementsTapered( //
            parameters.imageSizePhysical, //
            threadCount, //
            interlacingMaxRasterSize,
            0.5 // normalized position of the peak. 0.5 means: in the middle.
        );
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
            const auto myLambda = [&callbackObject, //
                                   bytesPtr,
                                   bytesPerLine,
                                   parameters,
                                   shift,
                                   scaleFactor,
                                   currentPass,
                                   segment,
                                   &semaphore]() {
                renderByRow(callbackObject,
                            bytesPtr,
                            bytesPerLine,
                            parameters, //
                            shift,
                            scaleFactor,
                            currentPass,
                            segment.first, // first row
                            segment.second // last row
                );
                semaphore.release();
            };
            const auto myRunnablePtr = QRunnable::create(myLambda);
            poolReference.start(myRunnablePtr, imageThreadPriority);
        }
        // Intentionally acquiring segments.size() and not
        // treadCount,  because they might differ and
        // segments.size() is mandatory for thread execution.
        semaphore.acquire(segmentsCount); // Wait for all threads to finish.

        myImage.setDevicePixelRatio(parameters.devicePixelRatioF);
        callbackObject.deliverInterlacingPass( //
            myImage, //
            QImage(), //
            variantParameters, //
            // We return the state “Intermediate” even when the final
            // interlacing step of the Adam-interlacing has finished.
            // This is because we will still to some antialiasing in a
            // final step, which is independent from the Adam-interlacing.
            AsyncImageRenderCallback::InterlacingState::Intermediate);
        myImage.setDevicePixelRatio(1);

        if (currentPass.countdown > 1) {
            currentPass.switchToNextPass();
        } else {
            break;
        }
    }

    // cppcheck-suppress knownConditionTrueFalse // false positive
    if (callbackObject.shouldAbort()) {
        return;
    }

    // Anti-aliasing

    // The drawn gamut body has a sharp, non-anti-aliased border against the
    // background, which looks unappealing. While recalculating the entire
    // image at a higher resolution and then downscaling would provide
    // anti-aliasing, this approach is computationally expensive. Instead, we
    // take an optimized approach: we detect all pixels located at the border
    // between the gamut body and the background (on both sides of the
    // boundary) and store their coordinates in a duplicate-free container.
    // Anti-aliased values are then computed exclusively for these pixels,
    // reducing overhead while improving visual quality.

    // NOTE: Outside the circle, artefacts from previous rendering steps may
    // persist, as subsequent steps clean up artefacts only within the circle
    // for performance reasons. When detecting boundary pixels, some artefact
    // pixels might be included in the search results. However, this does not
    // negatively impact the image, as it only affects pixels outside the
    // defined circle. While performing unnecessary rendering operations is
    // inefficient, filtering out these artefacts beforehand would be complex.
    // Thus, for now, we leave the code as-is.

    QList<QPoint> antiAliasCoordinates = findBoundary(myImage);

    // cppcheck-suppress knownConditionTrueFalse // false positive
    if (callbackObject.shouldAbort()) {
        return;
    }

    const auto myColorFunction = [parameters,
                                  shift,
                                  scaleFactor,
                                  chromaRange] //
        (const double x, const double y) -> QRgb {
        cmsCIELab myCielabD50;
        myCielabD50.L = parameters.lightness;
        myCielabD50.b = chromaRange - (y + shift) * scaleFactor;
        myCielabD50.a = (x + shift) * scaleFactor - chromaRange;
        return parameters.rgbColorSpace->fromCielabD50ToQRgbOrTransparent( //
            myCielabD50);
    };
    doAntialias(myImage, antiAliasCoordinates, myColorFunction);

    if (callbackObject.shouldAbort()) {
        return;
    }

    myImage.setDevicePixelRatio(parameters.devicePixelRatioF);
    callbackObject.deliverInterlacingPass( //
        myImage, //
        QImage(), //
        variantParameters, //
        AsyncImageRenderCallback::InterlacingState::Final);
}

static_assert(std::is_standard_layout_v<ChromaHueImageParameters>);

} // namespace PerceptualColor
