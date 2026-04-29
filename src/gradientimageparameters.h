// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_GRADIENTIMAGEPARAMETERS_H
#define PERCEPTUALCOLOR_GRADIENTIMAGEPARAMETERS_H

#include "genericcolor.h"
#include "helperconversion.h"
#include <qglobal.h>
#include <qimage.h>
#include <qmetatype.h>
#include <qsharedpointer.h>
#include <qvariant.h>

namespace PerceptualColor
{
class AsyncImageRenderCallback;

/** @internal
 *
 *  @brief Parameters for image of a gradient.
 *
 * For usage with @ref AsyncImageProvider.
 *
 * As the hue is a circular property, there exists two ways to go from one hue
 * to another (clockwise or counter-clockwise). This gradient takes always the
 * shortest way.
 *
 * The image has properties that can be accessed by the corresponding setters
 * and getters or directly. You should explicitly set all values
 * <em>before</em> calling the first time @ref render().
 *
 * This class supports HiDPI via its @ref setDevicePixelRatioF function.
 */
struct GradientImageParameters {
public:
    explicit GradientImageParameters();

    /**
     * @brief Equal operator
     *
     * @param other The object to compare with.
     *
     * @returns <tt>true</tt> if equal, <tt>false</tt> otherwise.
     */
    [[nodiscard]] bool operator==(const GradientImageParameters &other) const = default;

    /**
     * @brief Unequal operator
     *
     * @param other The object to compare with.
     *
     * @returns <tt>true</tt> if unequal, <tt>false</tt> otherwise.
     */
    [[nodiscard]] bool operator!=(const GradientImageParameters &other) const = default;

    [[nodiscard]] GenericColor colorFromValue(qreal value) const;
    static void render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject);
    void setDevicePixelRatioF(const qreal newDevicePixelRatioF);
    void setFirstColorLchA(const GenericColor &newFirstColor);
    void setGradientLength(const int newGradientLength);
    void setGradientThickness(const int newGradientThickness);
    void setProjectionSpace(const LchSpace newProjectionSpace);
    void setSecondColorLchA(const GenericColor &newSecondColor);

private:
    /** @internal @brief Only for unit tests. */
    friend class TestGradientImageParameters;

    // Methods
    [[nodiscard]] GenericColor completlyNormalizedAndBounded(const GenericColor &color) const;
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
    /**
     * @brief The color space into which the gamut will be projected.
     */
    LchSpace m_projectionSpace = LchSpace::CielchD50;
    /** @brief Internal storage of the second color (corrected and altered
     * value).
     *
     * The color is normalized and bound to the LCH color space. In an
     * additional step, it has been altered (by increasing or decreasing the
     * hue component in steps of 360°) to minimize the distance in hue
     * from this color to @ref m_firstColorCorrected. This is necessary to
     * easily allow to calculate the intermediate colors of the gradient, so
     * that they take the shortest way through the color space.
     * @sa @ref setFirstColorLchA()
     * @sa @ref setSecondColorLchA()
     * @sa @ref completlyNormalizedAndBounded()
     * @sa @ref updateSecondColor() */
    GenericColor m_secondColorCorrectedAndAltered;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::GradientImageParameters)

#endif // PERCEPTUALCOLOR_GRADIENTIMAGEPARAMETERS_H
