// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromahueimageparameters.h"

#include "asyncimagerendercallback.h"
#include "cielchd50values.h"
#include "helperconstants.h"
#include "helperimage.h"
#include "helpermath.h"
#include "helperqttypes.h"
#include "interlacingpass.h"
#include "rgbcolorspace.h"
#include <lcms2.h>
#include <qcolor.h>
#include <qimage.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qrgb.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <type_traits>

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
    cmsCIELab cielabD50;
    cielabD50.L = parameters.lightness;
    QRgb tempColor;
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

    const auto shift = pixelOffset - parameters.borderPhysical;

    // The reference size (assumed to be a typical/common size) for the image:
    constexpr double referenceSizePhysical = 343;
    const auto factor = parameters.imageSizePhysical / referenceSizePhysical;
    // The number of appropriate interlacing passes at the reference size:
    constexpr int numberOfPassesAtReferenceSize = 5;
    static_assert(isOdd(numberOfPassesAtReferenceSize));
    // The number of actual passes
    const auto numberOfPasses = //
        numberOfPassesAtReferenceSize //
        // qMax makes sure std::log2() is never called with a parameter ≤ 0
        + 2 * std::log2(qMax(0.01, factor));
    InterlacingPass currentPass(numberOfPasses);

    QPainter myPainter(&myImage);
    myPainter.setRenderHint(QPainter::Antialiasing, false);
    while (true) {
        for (int y = currentPass.lineOffset; //
             y < parameters.imageSizePhysical; //
             y += currentPass.lineFrequency) //
        {
            if (callbackObject.shouldAbort()) {
                return;
            }
            cielabD50.b = chromaRange //
                - (y + shift) * scaleFactor;
            for (int x = currentPass.columnOffset; //
                 x < parameters.imageSizePhysical; //
                 x += currentPass.columnFrequency //
            ) {
                cielabD50.a = //
                    (x + shift) * scaleFactor //
                    - chromaRange;
                if ( //
                    (qPow(cielabD50.a, 2) + qPow(cielabD50.b, 2)) //
                    <= (qPow(chromaRange + overlap, 2)) //
                ) {
                    tempColor = //
                        parameters
                            .rgbColorSpace //
                            ->fromCielabD50ToQRgbOrTransparent(cielabD50);
                    if (qAlpha(tempColor) != 0) {
                        // The pixel is within the gamut!
                        myPainter.fillRect(
                            //
                            x, //
                            y, //
                            currentPass.rectangleSize.width(), //
                            currentPass.rectangleSize.height(), //
                            QColor(tempColor));
                    } else {
                        myPainter.save();
                        myPainter.setCompositionMode(
                            // Allow making the background transparent.
                            QPainter::CompositionMode_Clear);
                        myPainter.fillRect(
                            //
                            x, //
                            y, //
                            currentPass.rectangleSize.width(), //
                            currentPass.rectangleSize.height(), //
                            Qt::transparent);
                        myPainter.restore();
                    }
                }
            }
        }

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

    const auto myColorFunction = [parameters, shift, scaleFactor, chromaRange](const double x, const double y) -> QRgb {
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
