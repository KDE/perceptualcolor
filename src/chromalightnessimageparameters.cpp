// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromalightnessimageparameters.h"

#include "asyncimagerendercallback.h"
#include "helper.h"
#include "helperconversion.h"
#include "helperimage.h"
#include "helpermath.h"
#include "rgbcolorspace.h"
#include <lcms2.h>
#include <qbitarray.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qrgb.h>
#include <qsemaphore.h>

namespace PerceptualColor
{

/** @brief Equal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if equal, <tt>false</tt> otherwise. */
bool ChromaLightnessImageParameters::operator==(const ChromaLightnessImageParameters &other) const
{
    return ( //
        (hue == other.hue) //
        && (imageSizePhysical == other.imageSizePhysical) //
        && (rgbColorSpace == other.rgbColorSpace) //
    );
}

/** @brief Unequal operator
 *
 * @param other The object to compare with.
 *
 * @returns <tt>true</tt> if unequal, <tt>false</tt> otherwise. */
bool ChromaLightnessImageParameters::operator!=(const ChromaLightnessImageParameters &other) const
{
    return !(*this == other);
}

/**
 * @brief Render some rows of the image directly to the buffer.
 *
 * @param callbackObject Used to stop rendering when an abort is requested
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
    const AsyncImageRenderCallback &callbackObject,
    uchar *const bytesPtr,
    const qsizetype bytesPerLine,
    const ChromaLightnessImageParameters parameters, // clazy:exclude=function-args-by-ref
    int firstRow,
    int lastRow)
{
    QRgb rgbColor;
    cmsCIELCh cielchD50;
    cielchD50.h = normalizedAngle360(parameters.hue);
    for (int y = firstRow; y <= lastRow; ++y) {
        if (callbackObject.shouldAbort()) {
            return;
        }
        QRgb *line = //
            reinterpret_cast<QRgb *>(bytesPtr + y * bytesPerLine);
        cielchD50.L = 100 - (y + 0.5) * 100.0 / parameters.imageSizePhysical.height();
        for (int x = 0; x < parameters.imageSizePhysical.width(); ++x) {
            // Using the same scale as on the y axis. floating point
            // division thanks to 100 which is a "cmsFloat64Number"
            cielchD50.C = (x + 0.5) * 100.0 / parameters.imageSizePhysical.height();
            rgbColor = //
                parameters.rgbColorSpace->fromCielabD50ToQRgbOrTransparent( //
                    toCmsLab(cielchD50));
            if (qAlpha(rgbColor) != 0) {
                // The pixel is within the gamut
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
 * @todo Interlacing support.
 *
 * @todo Could we get better performance? Even online tools like
 * https://bottosson.github.io/misc/colorpicker/#ff2a00 or
 * https://oklch.evilmartians.io/#65.4,0.136,146.7,100 get quite good
 * performance. How do they do that? */
void ChromaLightnessImageParameters::render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)
{
    if (!variantParameters.canConvert<ChromaLightnessImageParameters>()) {
        return;
    }
    const ChromaLightnessImageParameters parameters = //
        variantParameters.value<ChromaLightnessImageParameters>();

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
    const auto colorSpace = parameters.rgbColorSpace;
    auto &poolReference = getLibraryQThreadPoolInstance();
    const auto threadCount = qMax(1, poolReference.maxThreadCount());

    // Paint the gamut.
    const double normalizedHue = normalizedAngle360(parameters.hue);
    uchar *const bytesPtr = myImage.bits();
    const qsizetype bytesPerLine = myImage.bytesPerLine();

    {
        const auto segments = splitElements(imageHeight, threadCount);
        // The narrowing static_cast<int>() is okay because parts.count() is a
        // result of threadCount, which is also int.
        static_assert( //
            std::is_same_v<std::remove_cv_t<decltype(threadCount)>, int>);
        const int segmentsCount = static_cast<int>(segments.count());
        QSemaphore semaphore(0);
        if (callbackObject.shouldAbort()) {
            return;
        }
        for (const auto &segment : segments) {
            const auto myLambda = [&callbackObject, //
                                   bytesPtr,
                                   bytesPerLine,
                                   parameters,
                                   segment,
                                   &semaphore]() {
                renderByRow(callbackObject, //
                            bytesPtr,
                            bytesPerLine,
                            parameters,
                            segment.first,
                            segment.second);
                semaphore.release();
            };
            const auto myRunnablePtr = QRunnable::create(myLambda);
            poolReference.start(myRunnablePtr, imageThreadPriority);
        }
        // Intentionally acquiring segments.count() and not
        // treadCount,  because they might differ and
        // segments.count() is mandatory for thread execution.
        semaphore.acquire(segmentsCount); // Wait for all threads to finish.
    }

    if (callbackObject.shouldAbort()) {
        return;
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

    // cppcheck-suppress knownConditionTrueFalse // false positive
    if (callbackObject.shouldAbort()) {
        return;
    }

    // Anti-aliasing
    QList<QPoint> antiAliasCoordinates = findBoundary(myImage);
    // cppcheck-suppress knownConditionTrueFalse // false positive
    if (callbackObject.shouldAbort()) {
        return;
    }
    const auto myColorFunction = [normalizedHue, //
                                  imageHeight,
                                  parameters] //
        (const double colorFunctionX, const double colorFunctionY) -> QRgb {
        cmsCIELCh myCielchD50;
        myCielchD50.h = normalizedHue;
        myCielchD50.L = 100 - (colorFunctionY + 0.5) * 100.0 / imageHeight;
        myCielchD50.C = (colorFunctionX + 0.5) * 100.0 / imageHeight;
        return parameters.rgbColorSpace->fromCielabD50ToQRgbOrTransparent( //
            toCmsLab(myCielchD50));
    };
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
