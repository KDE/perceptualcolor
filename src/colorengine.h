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
 */
class ColorEngine : public QObject
{
    Q_OBJECT

public: // Static factory functions
    [[nodiscard]] Q_INVOKABLE static QSharedPointer<PerceptualColor::ColorEngine> createSrgb();

public:
    virtual ~ColorEngine() noexcept override;

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

    [[nodiscard]] Q_INVOKABLE QColor maxChromaColorByCielchD50Hue360(double hue360) const;
    [[nodiscard]] Q_INVOKABLE QColor maxChromaColorByOklabHue360(double hue360) const;
    /** @brief Getter for property @ref profileMaximumCielchD50Chroma
     *  @returns the property @ref profileMaximumCielchD50Chroma */
    [[nodiscard]] double profileMaximumCielchD50Chroma() const;
    /** @brief Getter for property @ref profileMaximumOklchChroma
     *  @returns the property @ref profileMaximumOklchChroma */
    [[nodiscard]] double profileMaximumOklchChroma() const;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORENGINE_H
