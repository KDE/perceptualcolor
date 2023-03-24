// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "chromalightnessimageparameters.h"

#include "asyncimagerendercallback.h"
#include "helperconversion.h"
#include "helpermath.h"
#include "rgbcolorspace.h"
#include <lcms2.h>
#include <qbitarray.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qrgb.h>

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
    // A mask for the gamut.
    // In-gamut pixel are true, out-of-gamut pixel are false.
    QBitArray m_mask(parameters.imageSizePhysical.width() //
                         * parameters.imageSizePhysical.height(),
                     // Initial boolean value of all bits (true/false):
                     false);
    // Test if image size is empty.
    if (myImage.size().isEmpty()) {
        // The image must be non-empty (otherwise, our algorithm would
        // crash because of a division by 0).
        callbackObject.deliverInterlacingPass( //
            myImage, //
            QVariant::fromValue(parameters), //
            AsyncImageRenderCallback::InterlacingState::Final);
        return;
    }

    myImage.fill(Qt::transparent); // Initialize background color

    // Initialization
    cmsCIELCh LCh;
    QRgb rgbColor;
    int x;
    int y;
    const auto imageHeight = parameters.imageSizePhysical.height();
    const auto imageWidth = parameters.imageSizePhysical.width();

    // Paint the gamut.
    LCh.h = normalizedAngleDegree(parameters.hue);
    for (y = 0; y < imageHeight; ++y) {
        if (callbackObject.shouldAbort()) {
            return;
        }
        LCh.L = 100 - (y + 0.5) * 100.0 / imageHeight;
        for (x = 0; x < imageWidth; ++x) {
            // Using the same scale as on the y axis. floating point
            // division thanks to 100 which is a "cmsFloat64Number"
            LCh.C = (x + 0.5) * 100.0 / imageHeight;
            rgbColor = //
                parameters.rgbColorSpace->toQRgbOrTransparent( //
                    toCmsLab(LCh));
            if (qAlpha(rgbColor) != 0) {
                // The pixel is within the gamut
                myImage.setPixelColor(x, y, rgbColor);
                m_mask.setBit(maskIndex(x, y, parameters.imageSizePhysical), //
                              true);
                // If color is out-of-gamut: We have chroma on the x axis and
                // lightness on the y axis. We are drawing the pixmap line per
                // line, so we go for given lightness from low chroma to high
                // chroma. Because of the nature of many gamuts, if once in a
                // line we have an out-of-gamut value, often all other pixels
                // that are more at the right will be out-of-gamut also. So we
                // could optimize our code and break here. But as we are not
                // sure about this: It’s just likely, but not always correct.
                // We do not know the gamut at compile time, so
                // for the moment we do not optimize the code.
            }
        }
    }
    callbackObject.deliverInterlacingPass( //
        myImage, //
        QVariant::fromValue(parameters), //
        AsyncImageRenderCallback::InterlacingState::Final);
}

} // namespace PerceptualColor
