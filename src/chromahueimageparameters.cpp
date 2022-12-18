// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromahueimageparameters.h"

#include "asyncimagerendercallback.h"
#include "cielchvalues.h"
#include "helperconstants.h"
#include "helpermath.h"
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
            variantParameters, //
            AsyncImageRenderCallback::InterlacingState::Final);
        return;
    }

    // If we continue, the circle will at least be visible.

    const QColor myNeutralGray = //
        parameters.rgbColorSpace->toQRgbBound(CielchValues::neutralGray);

    // Initialize the hole image background to the background color
    // of the circle:
    myImage.fill(myNeutralGray);

    // Prepare for gamut painting
    cmsCIELab lab;
    lab.L = parameters.lightness;
    int x;
    int y;
    QRgb tempColor;
    const auto chromaRange = parameters.rgbColorSpace->profileMaximumCielchChroma();
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
    // TODO Could this be further optimized? For example not go from zero
    // up to imageSizePhysical, but exclude the border (and add the
    // tolerance)? Thought anyway the color transform (which is the heavy
    // work) is only done when within a given diameter, reducing loop runs
    // itself might also increase performance at least a little bit…
    constexpr auto numberOfPasses = 11;
    static_assert(isOdd(numberOfPasses));
    InterlacingPass currentPass{numberOfPasses};
    QPainter myPainter(&myImage);
    myPainter.setRenderHint(QPainter::Antialiasing, false);
    while (true) {
        for (y = currentPass.lineOffset; //
             y < parameters.imageSizePhysical; //
             y += currentPass.lineFrequency) //
        {
            if (callbackObject.shouldAbort()) {
                return;
            }
            lab.b = chromaRange //
                - (y + pixelOffset - parameters.borderPhysical) * scaleFactor;
            for (x = currentPass.columnOffset; //
                 x < parameters.imageSizePhysical; //
                 x += currentPass.columnFrequency //
            ) {
                lab.a = //
                    (x + pixelOffset - parameters.borderPhysical) * scaleFactor //
                    - chromaRange;
                if ( //
                    (qPow(lab.a, 2) + qPow(lab.b, 2)) //
                    <= (qPow(chromaRange + overlap, 2)) //
                ) {
                    tempColor = parameters //
                                    .rgbColorSpace //
                                    ->toQRgbOrTransparent(lab);
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
                        myPainter.fillRect(
                            //
                            x, //
                            y, //
                            currentPass.rectangleSize.width(), //
                            currentPass.rectangleSize.height(), //
                            myNeutralGray);
                    }
                }
            }
        }

        const AsyncImageRenderCallback::InterlacingState state = //
            (currentPass.countdown > 1) //
            ? AsyncImageRenderCallback::InterlacingState::Intermediate //
            : AsyncImageRenderCallback::InterlacingState::Final;

        myImage.setDevicePixelRatio(parameters.devicePixelRatioF);
        callbackObject.deliverInterlacingPass(myImage, variantParameters, state);
        myImage.setDevicePixelRatio(1);

        if (state == AsyncImageRenderCallback::InterlacingState::Intermediate) {
            currentPass.switchToNextPass();
        } else {
            return;
        }
    }
}

static_assert(std::is_standard_layout_v<ChromaHueImageParameters>);

} // namespace PerceptualColor
