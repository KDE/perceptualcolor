// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMAINFO_H
#define PERCEPTUALCOLOR_CHROMAINFO_H

#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "helperconversion.h"
#include "lchvalues.h"
#include <optional>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qglobal.h>
#include <qmap.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qrgb.h>
#include <qsharedpointer.h>
#include <qstring.h>
#include <qversionnumber.h>
class QRgba64;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Access to chroma-related data.
 */
class ChromaInfo
{
public:
    [[nodiscard]] static double cielabD50BlackpointL();
    [[nodiscard]] static double cielabD50WhitepointL();
    [[nodiscard]] static QColor maxChromaColorByCielchD50Hue360(double hue360);
    [[nodiscard]] static QColor maxChromaColorByOklabHue360(double hue360);
    [[nodiscard]] static double maxCielchD50Chroma();
    [[nodiscard]] static double maxOklchChroma();
    [[nodiscard]] static double oklabBlackpointL();
    [[nodiscard]] static double oklabWhitepointL();

private:
    Q_DISABLE_COPY(ChromaInfo)

    /**
     *@internal
     * @brief Only for unit tests.
     */
    friend class TestChromaInfo;

    ChromaInfo();
    [[nodiscard]] static const ChromaInfo &instance();
    virtual ~ChromaInfo() noexcept;

    [[nodiscard]] static QColor maxChromaColorByHue360(double hue360, PerceptualColor::LchSpace type);

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
     *
     * @sa @ref m_cielabD50WhitepointL()
     */
    qreal m_cielabD50BlackpointL = 0;
    /** @brief The lightest in-gamut point on the L* axis.
     *
     * @sa @ref m_cielabD50BlackpointL()
     */
    qreal m_cielabD50WhitepointL = 100;
    /** @brief The darkest in-gamut point on the L* axis.
     *
     * @sa @ref m_oklabWhitepointL()
     */
    qreal m_oklabBlackpointL = 0;
    /** @brief The lightest in-gamut point on the L* axis.
     *
     * @sa @ref m_oklabBlackpointL()
     */
    qreal m_oklabWhitepointL = 1;
    /**
     * @brief Internal storage.
     */
    double m_profileMaximumCielchD50Chroma = cielchD50Values.maximumChroma;
    /**
     * @brief Internal storage.
     */
    double m_profileMaximumOklchChroma = oklchValues.maximumChroma;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CHROMAINFO_H
