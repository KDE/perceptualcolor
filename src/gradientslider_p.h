// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_GRADIENTSLIDER_P_H
#define PERCEPTUALCOLOR_GRADIENTSLIDER_P_H

// Include the header of the public class of this private implementation.
// #include "gradientslider.h"

#include "asyncimageprovider.h"
#include "constpropagatingrawpointer.h"
#include "genericcolor.h"
#include "gradientimageparameters.h"
#include <qglobal.h>
#include <qnamespace.h>
#include <qsharedpointer.h>
class QPoint;

namespace PerceptualColor
{
class GradientSlider;
class ColorEngine;
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class GradientSliderPrivate
{
public:
    explicit GradientSliderPrivate(GradientSlider *backLink);
    virtual ~GradientSliderPrivate() noexcept;

    // Methods
    [[nodiscard]] qreal fromWidgetPixelPositionToValue(QPoint pixelPosition);
    void initialize(const QSharedPointer<ColorEngine> &colorEngine, Qt::Orientation orientation);
    void setOrientationWithoutSignalAndForceNewSizePolicy(Qt::Orientation newOrientation);
    [[nodiscard]] int physicalPixelLength() const;
    [[nodiscard]] int physicalPixelThickness() const;

    // Data members
    /** @brief Internal storage for property @ref GradientSlider::firstColorCieLchD50A */
    GenericColor m_firstColorCieLchD50A;
    /** @brief The gradient image (without the handle).
     *
     * Always at the left is the first color, always at the right
     * is the second color. This is independent from the actual
     * @ref GradientSlider::orientation and the actual LTR or RTL
     * layout. So when painting, it might be necessary to rotate
     * and/or mirror the image. */
    AsyncImageProvider<GradientImageParameters> m_gradientImage;
    /** @brief Properties for @ref m_gradientImage. */
    GradientImageParameters m_gradientImageParameters;
    /** @brief Internal storage for property
     * @ref GradientSlider::orientation */
    Qt::Orientation m_orientation;
    /** @brief Internal storage for property
     * @ref GradientSlider::pageStep */
    qreal m_pageStep = 0.1;
    /** @brief Internal storage for property
     * @ref GradientSlider::secondColorCieLchD50A */
    GenericColor m_secondColorCieLchD50A;
    /** @brief Internal storage for property
     * @ref GradientSlider::singleStep */
    qreal m_singleStep = 0.01;
    /** @brief Internal storage for property
     * @ref GradientSlider::value */
    qreal m_value = 0.5;

private:
    Q_DISABLE_COPY(GradientSliderPrivate)
    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<GradientSlider> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_GRADIENTSLIDER_P_H
