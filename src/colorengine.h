// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORENGINE_H
#define PERCEPTUALCOLOR_COLORENGINE_H

#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "helperconversion.h"
#include <lcms2.h>
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

Q_DECLARE_METATYPE(cmsColorSpaceSignature)
Q_DECLARE_METATYPE(cmsProfileClassSignature)

namespace PerceptualColor
{
class ColorEnginePrivate;

/** @internal
 *
 * @brief Provides access to color management functionality.
 *
 * This internal utility class centralizes color-profile-specific functionality
 * required by the widgets of this library. Its interface may evolve over time
 * as needed by the widget classes and can change at any moment. Therefore,
 * this class is private and not part of the public API. If necessary, the
 * factory function @ref ColorEngine::createSrgb(), defined in another header
 * file that only forward-declares this class, can be used to provide a public
 * API for creating objects without exposing this class’s declaration.
 *
 * This class has no public constructor. Objects can only be created
 * using the static factory functions.
 *
 * @todo SHOULDHAVE
 *       In the API of this class, clarify the precision. If more than
 *       8 bit per channel, we have to switch from QRgb to QRgb64. But
 *       probably all OS APIs only accept 8 bit anyway? Is it worth the
 *       pain just because @ref ColorDialog can return <tt>QColor</tt>
 *       which provides 16 bit support?
 *
 * @todo SHOULDHAVE To determine whether a specific value is in-gamut or
 * out-of-gamut, this class converts Oklch or CIELch values to RGB using an
 * RGB ICC color profile. If the resulting RGB values are out of range, the
 * original color is considered out-of-gamut. However, we know that this
 * behavior in LittleCMS works with some RGB memory formats, but fails with
 * others. Is it also possible that this entire approach may not work with
 * certain (unusual) ICC profiles? Could we test this during profile creation?
 * For example, by converting a clearly out-of-gamut Oklch or CIELch value and
 * rejecting the profile creation if the resulting RGB value appears in-gamut?
 */
class ColorEngine : public QObject
{
    Q_OBJECT

public: // Static factory functions
    [[nodiscard]] Q_INVOKABLE static QSharedPointer<PerceptualColor::ColorEngine> createSrgb();

public:
    virtual ~ColorEngine() noexcept override;

    [[nodiscard]] Q_INVOKABLE virtual bool isCielabD50InGamut(const cmsCIELab &lab) const;
    [[nodiscard]] Q_INVOKABLE virtual bool isCielchD50InGamut(const PerceptualColor::GenericColor &lch) const;
    [[nodiscard]] Q_INVOKABLE virtual bool isOklabInGamut(const PerceptualColor::GenericColor &oklab) const;
    [[nodiscard]] Q_INVOKABLE virtual bool isOklchInGamut(const PerceptualColor::GenericColor &lch) const;
    [[nodiscard]] Q_INVOKABLE QColor maxChromaColorByCielchD50Hue360(double hue360) const;
    [[nodiscard]] Q_INVOKABLE QColor maxChromaColorByOklabHue360(double hue360) const;
    /** @brief Getter for property @ref profileMaximumCielchD50Chroma
     *  @returns the property @ref profileMaximumCielchD50Chroma */
    [[nodiscard]] double profileMaximumCielchD50Chroma() const;
    /** @brief Getter for property @ref profileMaximumOklchChroma
     *  @returns the property @ref profileMaximumOklchChroma */
    [[nodiscard]] double profileMaximumOklchChroma() const;
    // The function declaration is kept on a single line to prevent issues
    // with Doxygen parsing.
    // clang-format is disabled here to prevent automatic line breaks.
    // clang-format off
    [[nodiscard]] Q_INVOKABLE virtual PerceptualColor::GenericColor reduceCielchD50ChromaToFitIntoGamut(const PerceptualColor::GenericColor &cielchD50color) const;
    // clang-format on
    [[nodiscard]] Q_INVOKABLE virtual PerceptualColor::GenericColor reduceOklchChromaToFitIntoGamut(const PerceptualColor::GenericColor &oklchColor) const;
    [[nodiscard]] Q_INVOKABLE virtual cmsCIELab toCielabD50(const QRgba64 rgbColor) const;
    [[nodiscard]] Q_INVOKABLE static cmsCIELab fromLchToCmsCIELab(const PerceptualColor::GenericColor &lch);
    [[nodiscard]] Q_INVOKABLE virtual QRgb fromCielchD50ToQRgbBound(const PerceptualColor::GenericColor &cielchD50) const;
    [[nodiscard]] Q_INVOKABLE virtual QRgb fromCielabD50ToQRgbOrTransparent(const cmsCIELab &lab) const;

private:
    Q_DISABLE_COPY(ColorEngine)

    explicit ColorEngine(QObject *parent = nullptr);

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ColorEnginePrivate;

    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ColorEnginePrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestColorEngine;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORENGINE_H
