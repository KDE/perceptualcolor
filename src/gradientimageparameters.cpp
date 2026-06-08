// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "gradientimageparameters.h"

#include "absolutecolor.h"
#include "asyncimagerendercallback.h"
#include "helper.h"
#include "lchvalues.h"
#include <cmath>
#include <qbrush.h>
#include <qcolor.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qpainter.h>

namespace PerceptualColor
{
/** @brief Constructor */
GradientImageParameters::GradientImageParameters()
{
    setFirstColorLchA(GenericColor{0, 0, 0}, 1);
    setSecondColorLchA(GenericColor{1000, 0, 0}, 1);
}

/**
 * @brief Normalizes the value and bounds it to the current LCH color space.
 *
 * @param color the color that should be treated.
 *
 * @returns A normalized and bounded version. If the chroma was negative,
 * it gets positive (which implies turning the hue by 180°). The hue is
 * normalized to the range <tt>[0°, 360°[</tt>. Lightness is bounded to the
 * range <tt>[0, 100]</tt>. Alpha is bounded to the range <tt>[0, 1]</tt>. */
GenericColor GradientImageParameters::completlyNormalizedAndBounded(const GenericColor &color) const
{
    GenericColor result;
    const LchValues myLchValues = makeLchValues(m_projectionSpace);

    // Lightness
    result.first = qBound<double>(0, color.first, myLchValues.maximumLightness);

    // Chroma und Hue
    if (color.second < 0) {
        result.second = color.second * (-1);
        result.third = fmod(color.third + 180, 360);
    } else {
        result.second = color.second;
        result.third = fmod(color.third, 360);
    }
    result.second = qBound<double>(0, result.second, myLchValues.maximumChroma);
    if (result.third < 0) {
        result.third += 360;
    }

    // Return
    return result;
}

/** @brief Setter for the first color property.
 *
 * @param newFirstColorLch The new first color Lch value
 * @param newFirstColorAlpha The new first color alpha value.
 *
 * @sa @ref m_firstColorLchCorrected
 * @sa @ref m_firstColorAlphaCorrected
 */
void GradientImageParameters::setFirstColorLchA(const GenericColor &newFirstColorLch, const double newFirstColorAlpha)
{
    const GenericColor correctedNewFirstColorLch = //
        completlyNormalizedAndBounded(newFirstColorLch);
    const auto correctedNewFirstColorAlpha = qBound<double>(0, newFirstColorAlpha, 1);
    if ((m_firstColorLchCorrected == correctedNewFirstColorLch) && (m_firstColorAlphaCorrected == correctedNewFirstColorAlpha)) {
        return;
    }

    m_firstColorLchCorrected = correctedNewFirstColorLch;
    updateSecondColor();
}

/**
 * @brief Setter for the second color.
 *
 * @param newSecondColorLch The new second color Lch value.
 * @param newSecondColorAlpha The new second color alpha value.
 *
 * @sa @ref m_secondColorLchCorrectedAndAltered
 * @sa @ref m_secondColorAlphaCorrected
 */
void GradientImageParameters::setSecondColorLchA(const GenericColor &newSecondColorLch, const double newSecondColorAlpha)
{
    GenericColor correctedNewSecondColor = //
        completlyNormalizedAndBounded(newSecondColorLch);
    const auto correctedNewSecondColorAlpha = //
        qBound<double>(0, newSecondColorAlpha, 1);
    if ((m_secondColorLchCorrectedAndAltered == correctedNewSecondColor) //
        && (m_secondColorAlphaCorrected == correctedNewSecondColorAlpha)) {
        return;
    }
    m_secondColorLchCorrectedAndAltered = correctedNewSecondColor;
    m_secondColorAlphaCorrected = correctedNewSecondColorAlpha;
    updateSecondColor();
}

/**
 * @brief Updates @ref m_secondColorLchCorrectedAndAltered
 *
 * This update takes into account the current values of
 * @ref m_firstColorLchCorrected and
 * @ref m_secondColorLchCorrectedAndAltered.
 */
void GradientImageParameters::updateSecondColor()
{
    m_secondColorLchCorrectedAndAltered = //
        completlyNormalizedAndBounded(m_secondColorLchCorrectedAndAltered);
    if (qAbs(m_firstColorLchCorrected.third - m_secondColorLchCorrectedAndAltered.third) > 180) {
        if (m_firstColorLchCorrected.third > m_secondColorLchCorrectedAndAltered.third) {
            m_secondColorLchCorrectedAndAltered.third += 360;
        } else {
            m_secondColorLchCorrectedAndAltered.third -= 360;
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
void GradientImageParameters::render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)
{
    if (!variantParameters.canConvert<GradientImageParameters>()) {
        return;
    }
    const GradientImageParameters parameters = //
        variantParameters.value<GradientImageParameters>();

    // First, create an image of the gradient with only one pixel thickness.
    // (Color management operations are expensive in CPU time; we try to
    // minimize this.)
    QImage onePixelLine(parameters.m_gradientLength, //
                        1, //
                        QImage::Format_ARGB32_Premultiplied);
    onePixelLine.fill(Qt::transparent); // Initialize image with transparency.
    GenericColor colorLch;
    QColor temp;
    for (int i = 0; i < parameters.m_gradientLength; ++i) {
        const auto value = //
            (i + 0.5) / static_cast<qreal>(parameters.m_gradientLength);
        colorLch = parameters.colorFromValue(value);
        if (parameters.m_projectionSpace == LchSpace::CielchD50) {
            temp = AbsoluteColor::fastFromCielchD50ToSRgbClamped(colorLch);
        } else {
            temp = AbsoluteColor::fastFromOklchToSRgbClamped(colorLch);
        }
        const auto alpha = parameters.m_firstColorAlphaCorrected //
            + (parameters.m_secondColorAlphaCorrected - parameters.m_firstColorAlphaCorrected) * value;
        temp.setAlphaF(static_cast<float>(alpha));
        onePixelLine.setPixelColor(i, 0, temp);
    }

    // Now, create a full image of the gradient
    QImage result = QImage(parameters.m_gradientLength, //
                           parameters.m_gradientThickness, //
                           QImage::Format_ARGB32_Premultiplied);
    if (result.isNull()) {
        // Make sure that no QPainter can be created on a null image
        // (because this would trigger warning messages on the command
        // line).
        return;
    }
    QPainter painter(&result);

    // Transparency background
    if ( //
        (parameters.m_firstColorAlphaCorrected != 1) //
        || (parameters.m_secondColorAlphaCorrected != 1) //
    ) {
        // Fill the image with tiles. (QBrush will ignore
        // the devicePixelRatioF of the image of the tile.)
        auto background = transparencyBackground( //
            parameters.m_devicePixelRatioF);
        background.setDevicePixelRatio(1);
        painter.fillRect(0, //
                         0, //
                         parameters.m_gradientLength, //
                         parameters.m_gradientThickness, //
                         QBrush(background));
    }

    // Paint the gradient itself.
    for (int i = 0; i < parameters.m_gradientThickness; ++i) {
        painter.drawImage(0, i, onePixelLine);
    }

    result.setDevicePixelRatio(parameters.m_devicePixelRatioF);

    callbackObject.deliverInterlacingPass( //
        result, //
        QImage(), //
        variantParameters, //
        AsyncImageRenderCallback::InterlacingState::Final);
}

/** @brief The color that the gradient has at a given position of the gradient.
 * @param value The position. Valid range: <tt>[0.0, 1.0]</tt>. <tt>0.0</tt>
 * means the first color, <tt>1.0</tt> means the second color, and everything
 * in between means a color in between.
 * @returns If the position is valid: The color at the given position and
 * its corresponding alpha value. If the position is out-of-range: An
 * arbitrary value. */
GenericColor GradientImageParameters::colorFromValue(qreal value) const
{
    GenericColor color;
    color.first = m_firstColorLchCorrected.first //
        + (m_secondColorLchCorrectedAndAltered.first - m_firstColorLchCorrected.first) * value;
    color.second = m_firstColorLchCorrected.second + //
        (m_secondColorLchCorrectedAndAltered.second - m_firstColorLchCorrected.second) * value;
    color.third = m_firstColorLchCorrected.third + //
        (m_secondColorLchCorrectedAndAltered.third - m_firstColorLchCorrected.third) * value;
    return color;
}

/** @brief Setter for the device pixel ratio (floating point).
 *
 * This value is set as device pixel ratio (floating point) in the
 * <tt>QImage</tt> that this class holds. It does <em>not</em> change
 * the <em>pixel</em> size of the image or the pixel size of wheel
 * thickness or border.
 *
 * This is for HiDPI support. You can set this to
 * <tt>QWidget::devicePixelRatioF()</tt> to get HiDPI images in the correct
 * resolution for your widgets. Within a method of a class derived
 * from <tt>QWidget</tt>, you could write:
 *
 * @snippet testgradientimageparameters.cpp GradientImage HiDPI usage
 *
 * The default value is <tt>1</tt> which means no special scaling.
 *
 * @param newDevicePixelRatioF the new device pixel ratio as a
 * floating point data type. (Values smaller than <tt>1.0</tt> will be
 * considered as <tt>1.0</tt>.) */
void GradientImageParameters::setDevicePixelRatioF(const qreal newDevicePixelRatioF)
{
    m_devicePixelRatioF = qMax<qreal>(1, newDevicePixelRatioF);
}

/** @brief Setter for the gradient length property.
 *
 * @param newGradientLength The new gradient length, measured
 * in <em>physical pixels</em>. */
void GradientImageParameters::setGradientLength(const int newGradientLength)
{
    m_gradientLength = qMax(0, newGradientLength);
}

/**
 * @brief Setter for the projection space property.
 *
 * @param newProjectionSpace The new projection space
 */
void GradientImageParameters::setProjectionSpace(const LchSpace newProjectionSpace)
{
    m_projectionSpace = newProjectionSpace;
}

/** @brief Setter for the gradient thickness property.
 *
 * @param newGradientThickness The new gradient thickness, measured
 * in <em>physical pixels</em>. */
void GradientImageParameters::setGradientThickness(const int newGradientThickness)
{
    m_gradientThickness = qMax(0, newGradientThickness);
}

} // namespace PerceptualColor
