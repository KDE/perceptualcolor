﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef RGBCOLORSPACE_P_H
#define RGBCOLORSPACE_P_H

// Include the header of the public class of this private implementation.
// #include "rgbcolorspace.h"

#include "cielchd50values.h"
#include "constpropagatingrawpointer.h"
#include "oklchvalues.h"
#include "rgbcolorspace.h"
#include <lcms2.h>
#include <map>
#include <optional>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qglobal.h>
#include <qlist.h>
#include <qstring.h>
#include <qversionnumber.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class RgbColorSpacePrivate final
{
public:
    /**
     * @brief Enum class representing possible color spaces in the Lch color
     * models.
     */
    enum class LchSpace {
        Oklch, /**< The Oklch color space, which uses by definition
            always a D65 whitepoint. */
        CielchD50 /**< The CielchD50 color space, assuming a chromatic
            adaption to the D50 whitepoint. */
    };

    explicit RgbColorSpacePrivate(RgbColorSpace *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~RgbColorSpacePrivate() noexcept = default;

    // Data members:
    /**
     * @brief All RGB colors located on the chromaticity boundary, ordered by
     * CielchD50 hue.
     *
     * The chromaticity boundary consists of the spectral locus and the purple
     * line.
     *
     * This dataset includes all integer RGB values (i.e., values representable
     * with three one-byte channels) that lie on the chromaticity boundary.
     * To ensure smooth cyclic transitions, the dataset includes duplicates
     * of the lowest and highest hues positioned outside the [0, 360] boundary.
     * For instance, the lowest original angle of 2° has a duplicate at 362°,
     * and the highest original angle of 357° has a duplicate at -3°.
     *
     * The hue is normalized to the range [0, 360].
     */
    std::map<double, QColor> m_chromaticityBoundaryByCielchD50Hue360;
    /**
     * @brief All RGB colors located on the chromaticity boundary, ordered by
     * Oklab hue.
     *
     * The chromaticity boundary consists of the spectral locus and the purple
     * line.
     *
     * This dataset includes all integer RGB values (i.e., values representable
     * with three one-byte channels) that lie on the chromaticity boundary.
     * To ensure smooth cyclic transitions, the dataset includes duplicates
     * of the lowest and highest hues positioned outside the [0, 360] boundary.
     * For instance, the lowest original angle of 2° has a duplicate at 362°,
     * and the highest original angle of 357° has a duplicate at -3°.
     *
     * The hue is normalized to the range [0, 360].
     */
    std::map<double, QColor> m_chromaticityBoundaryByOklabHue360;
    /** @brief The darkest in-gamut point on the L* axis.
     * @sa whitepointL
     *
     * @internal
     *
     * @todo Use cmsDetectBlackPoint? But “our” “blackpoint” is always on
     * the grey axis, but the real blackpoint not? Document this? */
    qreal m_cielabD50BlackpointL = 0;
    /** @brief The lightest in-gamut point on the L* axis.
     * @sa blackpointL() */
    qreal m_cielabD50WhitepointL = 100;
    /** @brief The darkest in-gamut point on the L* axis.
     * @sa whitepointL
     *
     * @internal
     *
     * @todo Use cmsDetectBlackPoint? But “our” “blackpoint” is always on
     * the grey axis, but the real blackpoint not? Document this? */
    qreal m_oklabBlackpointL = 0;
    /** @brief The lightest in-gamut point on the L* axis.
     * @sa blackpointL() */
    qreal m_oklabWhitepointL = 1;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileAbsoluteFilePath */
    QString m_profileAbsoluteFilePath;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileClass */
    cmsProfileClassSignature m_profileClass;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileColorModel */
    cmsColorSpaceSignature m_profileColorModel;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileCopyright */
    QString m_profileCopyright;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileCreationDateTime */
    QDateTime m_profileCreationDateTime;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileFileSize */
    qint64 m_profileFileSize = -1;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileHasClut */
    bool m_profileHasClut = false;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileHasMatrixShaper */
    bool m_profileHasMatrixShaper = false;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileRenderingIntentDirections */
    RgbColorSpace::RenderingIntentDirections m_profileRenderingIntentDirections;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileIccVersion */
    QVersionNumber m_profileIccVersion;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileManufacturer */
    QString m_profileManufacturer;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileMaximumCielchD50Chroma */
    double m_profileMaximumCielchD50Chroma = CielchD50Values::maximumChroma;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileMaximumOklchChroma */
    double m_profileMaximumOklchChroma = OklchValues::maximumChroma;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileModel */
    QString m_profileModel;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileName */
    QString m_profileName;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profilePcsColorModel */
    cmsColorSpaceSignature m_profilePcsColorModel;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileTagBlackpoint */
    std::optional<cmsCIEXYZ> m_profileTagBlackpoint;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileTagBluePrimary */
    std::optional<cmsCIEXYZ> m_profileTagBluePrimary;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileTagGreenPrimary */
    std::optional<cmsCIEXYZ> m_profileTagGreenPrimary;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileTagRedPrimary */
    std::optional<cmsCIEXYZ> m_profileTagRedPrimary;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileTagSignatures */
    QStringList m_profileTagSignatures;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileTagWhitepoint */
    std::optional<cmsCIEXYZ> m_profileTagWhitepoint;
    /** @brief A handle to a LittleCMS transform. */
    cmsHTRANSFORM m_transformCielabD50ToRgb16Handle = nullptr;
    /** @brief A handle to a LittleCMS transform. */
    cmsHTRANSFORM m_transformCielabD50ToRgbHandle = nullptr;
    /** @brief A handle to a LittleCMS transform. */
    cmsHTRANSFORM m_transformRgbToCielabD50Handle = nullptr;

    // Functions:
    static void deleteTransform(cmsHTRANSFORM *transformHandle);
    void initializeChromaticityBoundaries();
    [[nodiscard]] bool initialize(cmsHPROFILE rgbProfileHandle);
    [[nodiscard]] Q_INVOKABLE QColor maxChromaColorByHue360(double oklabHue360, RgbColorSpacePrivate::LchSpace type) const;
    [[nodiscard]] static QDateTime profileCreationDateTime(cmsHPROFILE profileHandle);
    [[nodiscard]] static QVersionNumber profileIccVersion(cmsHPROFILE profileHandle);
    [[nodiscard]] static QString profileInformation(cmsHPROFILE profileHandle, cmsInfoType infoType, const QString &languageTerritory);
    [[nodiscard]] static std::optional<cmsCIEXYZ> profileReadCmsciexyzTag(cmsHPROFILE profileHandle, cmsTagSignature signature);
    [[nodiscard]] static QStringList profileTagSignatures(cmsHPROFILE profileHandle);

    /** @brief Increment factor for the maximum-chroma detection.
     *
     * The maximum-chroma detection, regardless of the precision, might
     * always return a value that is a bit too small. However, we want
     * to have @ref RgbColorSpace::profileMaximumCielchD50Chroma and
     * @ref RgbColorSpace::profileMaximumOklchChroma values that
     * are equal or slightly bigger than the actual maximum-chroma,
     * to make sure to not exclude valid values. Therefore,
     * @ref initializeChromaticityBoundaries() uses this increment factor
     * to slightly increment the outcome of the chroma detection relative
     * to the original value, as a safety margin. Note that additionally,
     * an absolute increment should also be added, because of limited
     * precision in floating point operations. */
    static constexpr double chromaDetectionIncrementFactor = 1.02;
    /** @brief For detecting CIELab in-gamut or out-of-gamut colors.
     *
     * For gamut detection, a roundtrip conversion is performed: Lab values
     * are converted to an RGB color space and backwards. If the distance
     * in euclidean space between the the original Lab value and the result
     * of the roundtrip is smaller than a certain value, it is considered
     * as an in-gamut value.
     *
     * This deviation limit should be as small as possible for a more correct
     * gamut boundary. But it must unfortunately also be big enough to ignore
     * rounding errors. The current value was chosen by trial-and-error. */
    static constexpr qreal cielabDeviationLimit = 0.5;
    /** @brief For detecting Oklab in-gamut or out-of-gamut colors.
     *
     * For gamut detection, a roundtrip conversion is performed: Lab values
     * are converted to an RGB color space and backwards. If the distance
     * in euclidean space between the the original Lab value and the result
     * of the roundtrip is smaller than a certain value, it is considered
     * as an in-gamut value.
     *
     * This deviation limit should be as small as possible for a more correct
     * gamut boundary. But it must unfortunately also be big enough to ignore
     * rounding errors. The current value was chosen by trial-and-error. */
    static constexpr qreal oklabDeviationLimit = 0.001;

private:
    Q_DISABLE_COPY(RgbColorSpacePrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<RgbColorSpace> q_pointer;
};

} // namespace PerceptualColor

#endif // RGBCOLORSPACE_P_H
