// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef RGBCOLORSPACE_P_H
#define RGBCOLORSPACE_P_H

// Include the header of the public class of this private implementation.
// #include "rgbcolorspace.h"

#include "cielchvalues.h"
#include "constpropagatingrawpointer.h"
#include "helperconstants.h"
#include "oklchvalues.h"
#include <lcms2.h>
#include <qdatetime.h>
#include <qglobal.h>
#include <qmap.h>
#include <qstring.h>
#include <qversionnumber.h>

namespace PerceptualColor
{
class RgbColorSpace;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class RgbColorSpacePrivate final
{
public:
    explicit RgbColorSpacePrivate(RgbColorSpace *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~RgbColorSpacePrivate() noexcept = default;

    // Data members:
    /** @brief The darkest in-gamut point on the L* axis.
     * @sa whitepointL */
    qreal m_blackpointL = 0;
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
     * @ref RgbColorSpace::profileIccVersion */
    QVersionNumber m_profileIccVersion;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileManufacturer */
    QString m_profileManufacturer;
    /** @brief Internal storage for property
     * @ref RgbColorSpace::profileMaximumCielchChroma */
    double m_profileMaximumCielchChroma = CielchValues::maximumChroma;
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
    /** @brief A handle to a LittleCMS transform. */
    cmsHTRANSFORM m_transformLabToRgb16Handle = nullptr;
    /** @brief A handle to a LittleCMS transform. */
    cmsHTRANSFORM m_transformLabToRgbHandle = nullptr;
    /** @brief A handle to a LittleCMS transform. */
    cmsHTRANSFORM m_transformRgbToLabHandle = nullptr;
    /** @brief The lightest in-gamut point on the L* axis.
     * @sa blackpointL() */
    qreal m_whitepointL = 100;

    // Functions:
    static void deleteTransform(cmsHTRANSFORM *transformHandle);
    [[nodiscard]] double detectMaximumCielchChroma() const;
    [[nodiscard]] double detectMaximumOklchChroma() const;
    [[nodiscard]] static QDateTime getCreationDateTimeFromProfile(cmsHPROFILE profileHandle);
    [[nodiscard]] static QVersionNumber getIccVersionFromProfile(cmsHPROFILE profileHandle);
    [[nodiscard]] static QString getInformationFromProfile(cmsHPROFILE profileHandle, cmsInfoType infoType);
    [[nodiscard]] static QMap<cmsUInt32Number, QString> getIntentList();
    [[nodiscard]] bool initialize(cmsHPROFILE rgbProfileHandle);

    /** @brief The rendering intents supported by the LittleCMS library.
     *
     * Contains all rendering intents supported by the LittleCMS library
     * against which this we are currently linking. Each entry contains
     * the code and the (english-language) description just as provided
     * by LittleCMS.
     *
     * Note that LittleCMS supports as built-in intents the four official
     * ICC intents and also some other, non-ICC intents. Furthermore,
     * LittleCMS plugins can provide even more intents. As of LittleCMS 2.13
     * the following built-in intents are available:
     *
     * | Type    | Macro name                                    | Code |
     * | :------ | :-------------------------------------------- | ---: |
     * | ICC     | INTENT_PERCEPTUAL                             |    0 |
     * | ICC     | INTENT_RELATIVE_COLORIMETRIC                  |    1 |
     * | ICC     | INTENT_SATURATION                             |    2 |
     * | ICC     | INTENT_ABSOLUTE_COLORIMETRIC                  |    3 |
     * | Non-ICC | INTENT_PRESERVE_K_ONLY_PERCEPTUAL             |   10 |
     * | Non-ICC | INTENT_PRESERVE_K_ONLY_RELATIVE_COLORIMETRIC  |   11 |
     * | Non-ICC | INTENT_PRESERVE_K_ONLY_SATURATION             |   12 |
     * | Non-ICC | INTENT_PRESERVE_K_PLANE_PERCEPTUAL            |   13 |
     * | Non-ICC | INTENT_PRESERVE_K_PLANE_RELATIVE_COLORIMETRIC |   14 |
     * | Non-ICC | INTENT_PRESERVE_K_PLANE_SATURATION            |   15 | */
    static inline const QMap<cmsUInt32Number, QString> intentList = getIntentList();

    /** @brief Precision of HSV hue during maximum-chroma detection.
     *
     * @todo A value smaller than 0.001 does not make sense
     * currently, because QColor has only a limited precision for
     * HSV conversions. Furthermore, since Qt6 it’s floating point interface
     * has been defined with “float”. For a more exact solution, we would
     * have to implement our own HSV conversion first. */
    static constexpr double chromaDetectionPrecision = gamutPrecision;
    /** @brief Increment factor for the maximum-chroma detection.
     *
     * The maximum-chroma detection, regardless of the precision,
     * might always return a value that is a bit too small. However,
     * we want to have @ref RgbColorSpace::profileMaximumCielchChroma and
     * @ref RgbColorSpace::profileMaximumOklchChroma values that are equal
     * or slightly bigger than the actual maximum-chroma, to make sure to
     * not exclude valid values. Therefore, @ref detectMaximumCielchChroma()
     * and @ref detectMaximumOklchChroma use this increment factor to
     * slightly increment the outcome of the chroma detection relative to
     * the original value, as a safety margin. Note that additionally,
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
    static constexpr qreal oklabDeviationLimit = 0.005; // TODO Test this!

private:
    Q_DISABLE_COPY(RgbColorSpacePrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<RgbColorSpace> q_pointer;
};

} // namespace PerceptualColor

#endif // RGBCOLORSPACE_P_H
