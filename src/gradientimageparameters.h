// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef GRADIENTIMAGEPARAMETERS_H
#define GRADIENTIMAGEPARAMETERS_H

#include "genericcolor.h"
#include <qglobal.h>
#include <qimage.h>
#include <qmetatype.h>
#include <qsharedpointer.h>
#include <qvariant.h>

namespace PerceptualColor
{
class AsyncImageRenderCallback;
class RgbColorSpace;

/** @internal
 *
 *  @brief Parameters for image of a gradient.
 *
 * For usage with @ref AsyncImageProvider.
 *
 * As the hue is a circular property, there exists two ways to go one hue to
 * another (clockwise or counter-clockwise). This gradient takes always the
 * shortest way.
 *
 * The image has properties that can be accessed by the corresponding setters
 * and getters or directly. You should explicitly set all values
 * <em>before</em> calling the first time @ref render().
 *
 * This class supports HiDPI via its @ref setDevicePixelRatioF function.
 *
 * @todo Instead of providing an image that has actually the size that
 * has been requested, we could provide just a tile. The user would have
 * to tile the surface. In many cases, we could get away with much smaller
 * images. Attention: Test if this approach works fine when the screen scale
 * factor is not an integer! */
struct GradientImageParameters final {
public:
    explicit GradientImageParameters();
    [[nodiscard]] bool operator==(const GradientImageParameters &other) const;
    [[nodiscard]] bool operator!=(const GradientImageParameters &other) const;

    /** @brief Pointer to @ref RgbColorSpace object */
    QSharedPointer<PerceptualColor::RgbColorSpace> rgbColorSpace = nullptr;

    [[nodiscard]] GenericColor colorFromValue(qreal value) const;
    static void render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject);
    void setDevicePixelRatioF(const qreal newDevicePixelRatioF);
    void setFirstColorCieLchD50A(const GenericColor &newFirstColor);
    void setGradientLength(const int newGradientLength);
    void setGradientThickness(const int newGradientThickness);
    void setSecondColorCieLchD50A(const GenericColor &newFirstColor);

private:
    /** @internal @brief Only for unit tests. */
    friend class TestGradientImageParameters;

    // Methods
    [[nodiscard]] static GenericColor completlyNormalizedAndBounded(const GenericColor &color);
    void updateSecondColor();

    // Data members
    /** @brief Internal storage of the device pixel ratio as floating point.
     *
     * @sa @ref setDevicePixelRatioF() */
    qreal m_devicePixelRatioF = 1;
    /** @brief Internal storage of the first color.
     *
     * The color is normalized and bound to the LCH color space.
     * @sa @ref completlyNormalizedAndBounded() */
    GenericColor m_firstColorCorrected;
    /** @brief Internal storage for the gradient length, measured in
     * physical pixels.
     *
     * @sa @ref setGradientLength() */
    int m_gradientLength = 0;
    /** @brief Internal storage for the gradient thickness, measured in
     * physical pixels.
     *
     * @sa @ref setGradientThickness() */
    int m_gradientThickness = 0;
    /** @brief Internal storage of the image (cache).
     *
     * - If <tt>m_image.isNull()</tt> than either no cache is available
     *   or @ref m_gradientLength or @ref m_gradientThickness is <tt>0</tt>.
     *   Before using it, a new image has to be rendered. (If
     *   @ref m_gradientLength or @ref m_gradientThickness is
     *   <tt>0</tt>, this will be extremly fast.)
     * - If <tt>m_image.isNull()</tt> is <tt>false</tt>, than the cache
     *   is valid and can be used directly. */
    QImage m_image;
    /** @brief Internal storage of the second color (corrected and altered
     * value).
     *
     * The color is normalized and bound to the LCH color space. In an
     * additional step, it has been altered (by increasing or decreasing the
     * hue component in steps of 360°) to minimize the distance in hue
     * from this color to @ref m_firstColorCorrected. This is necessary to
     * easily allow to calculate the intermediate colors of the gradient, so
     * that they take the shortest way through the color space.
     * @sa @ref setFirstColorCieLchD50A()
     * @sa @ref setSecondColorCieLchD50A()
     * @sa @ref completlyNormalizedAndBounded()
     * @sa @ref updateSecondColor() */
    GenericColor m_secondColorCorrectedAndAltered;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::GradientImageParameters)

#endif // GRADIENTIMAGEPARAMETERS_H
