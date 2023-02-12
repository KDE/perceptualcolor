// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef GRADIENTSLIDER_P_H
#define GRADIENTSLIDER_P_H

// Include the header of the public class of this private implementation.
// #include "gradientslider.h"

#include "asyncimageprovider.h"
#include "constpropagatingrawpointer.h"
#include "gradientimageparameters.h"
#include "lchadouble.h"
#include <QtCore/qsharedpointer.h>
#include <qglobal.h>
#include <qnamespace.h>
class QPoint;

namespace PerceptualColor
{
class GradientSlider;
class RgbColorSpace;
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class GradientSliderPrivate final
{
public:
    explicit GradientSliderPrivate(GradientSlider *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~GradientSliderPrivate() noexcept = default;

    // Methods
    [[nodiscard]] qreal fromWidgetPixelPositionToValue(QPoint pixelPosition);
    void initialize(const QSharedPointer<RgbColorSpace> &colorSpace, Qt::Orientation orientation);
    void setOrientationWithoutSignalAndForceNewSizePolicy(Qt::Orientation newOrientation);
    [[nodiscard]] int physicalPixelLength() const;
    [[nodiscard]] int physicalPixelThickness() const;

    // Data members
    /** @brief Internal storage for property @ref GradientSlider::firstColor */
    LchaDouble m_firstColor;
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
     * @ref GradientSlider::secondColor */
    LchaDouble m_secondColor;
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

#endif // GRADIENTSLIDER_P_H
