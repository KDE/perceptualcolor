// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_P_H
#define PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_P_H

// Include the header of the public class of this private implementation.
// #include "chromalightnessdiagram.h"

#include "asyncimageprovider.h"
#include "chromalightnessimageparameters.h"
#include "constpropagatingrawpointer.h"
#include "genericcolor.h"
#include "lchvalues.h"
#include "perceptualcolornamespace.h"
#include <cmath>
#include <functional>
#include <limits>
#include <optional>
#include <qglobal.h>
#include <qlabel.h>
#include <qlayout.h>
#include <qpointer.h>
#include <qsize.h>
#include <qtoolbutton.h>
class QRect;
class QPoint;

namespace PerceptualColor
{
class ChromaLightnessDiagram;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 *  \brief Private implementation within the \e {Pointer to
 *  implementation} idiom. */
class ChromaLightnessDiagramPrivate : public QObject
{
    Q_OBJECT

public:
    explicit ChromaLightnessDiagramPrivate(ChromaLightnessDiagram *backLink, const LchSpace projectionSpace);
    virtual ~ChromaLightnessDiagramPrivate() noexcept override;

    // Member variables
    /*!
     * \brief The image of the chroma-lightness diagram itself. */
    AsyncImageProvider<ChromaLightnessImageParameters> m_chromaLightnessImage;
    /*!
     * \brief Properties for \l m_chromaLightnessImage. */
    ChromaLightnessImageParameters m_chromaLightnessImageParameters;
    /*!
     * \brief Internal storage of
     * the \l ChromaLightnessDiagram::currentColorLch property */
    GenericColor m_currentColorLch;

    // NOTE This could be replaced by KWidgetsAddons’
    // https://api.kde.org/kcontextualhelpbutton.html
    // KContextualHelpButton, which is more sophisticated and the
    // QWidget-based alternative to
    // https://api.kde.org/qml-org-kde-kirigami-contextualhelpbutton.html
    // org.kde.kirigami.ContextualHelpButton QML Type. However,
    // this is a Tier-1 library and therefore cannot use them.
    /*!
     * \brief The info button for irregular gamut shapes.
     */
    QPointer<QToolButton> m_infoButton = nullptr;

    /*!
     * \brief Layout for positionning \l m_infoButton.
     */
    QPointer<QGridLayout> m_layout = nullptr;

    /*!
     * \brief Geometry of the current Lch color space.
     * */
    const LchValues m_lchValues;
    /*!
     * \brief The color space into which the gamut will be projected.
     */
    const LchSpace m_projectionSpace;

    // Member functions
    [[nodiscard]] QSize calculateImageSizePhysical() const;
    [[nodiscard]] int defaultBorderDeviceIndependant() const;
    [[nodiscard]] int defaultBorderPhysical() const;
    // NOTE QDoc unfortunately does not support documentation for function
    // templates that are members of non-template classes. Therefore, the
    // following documentation will be ignored by QDoc.
    /*!
     *
     *
     * \brief Calculate how far a value is from a given range.
     *
     * \b Precondition: \c {low} ≤ \c {high}
     *
     * \a low the lower limit
     *
     * \a x the value that will be tested
     *
     * \a high the higher limit
     *
     * Returns \c {0} if the value is within the range. The distance
     * to the nearest border of the range otherwise. */
    template<typename T>
    [[nodiscard]] static constexpr T distanceFromRange(const T &low, const T &x, const T &high)
    {
        if (x < low) {
            return low - x;
        }
        if (x > high) {
            return x - high;
        }
        if constexpr ( //
            std::numeric_limits<T>::has_quiet_NaN //
            || std::numeric_limits<T>::has_signaling_NaN //
        ) {
            if (std::isnan(low) || std::isnan(x) || std::isnan(high)) {
                return std::numeric_limits<T>::quiet_NaN();
            }
        }
        return 0;
    }
    [[nodiscard]] GenericColor fromWidgetPixelPositionToLch(const QPoint widgetPixelPosition) const;
    [[nodiscard]] bool isWidgetPixelPositionInGamut(const QPoint widgetPixelPosition) const;
    [[nodiscard]] int leftBorderDeviceIndependent() const;
    [[nodiscard]] int leftBorderPhysical() const;
    [[nodiscard]] GenericColor nearestInGamutLchByAdjustingChromaLightness(const double chroma, const double lightness);
    [[nodiscard]] std::optional<QPoint> nearestInGamutPixelPosition(const QPoint originalPixelPosition);
    [[nodiscard]] static std::optional<QPoint>
    nearestNeighborSearch(const QPoint point, const QRect boundingBox, const std::function<bool(const QPoint)> &doesPointExist);
    void reloadIcons();
    void retranslateUi();
    void setCurrentColorFromWidgetPixelPosition(const QPoint widgetPixelPosition);
    void updateImageDimensions();
    void updateInfoButtonVisibility();
    void updateLayoutMargins();

private:
    Q_DISABLE_COPY(ChromaLightnessDiagramPrivate)

    /*!
     * \brief Pointer to the object from which \e {this} object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ChromaLightnessDiagram> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMALIGHTNESSDIAGRAM_P_H
