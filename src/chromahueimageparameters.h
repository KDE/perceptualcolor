// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMAHUEIMAGEPARAMETERS_H
#define PERCEPTUALCOLOR_CHROMAHUEIMAGEPARAMETERS_H

#include "interlacingpass.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qmetatype.h>
#include <qvariant.h>

class QSize;

namespace PerceptualColor
{
class AsyncImageRenderCallback;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Parameters for an image of a chroma hue plane.
 *
 * For usage with \l AsyncImageProvider.
 *
 * This is a cut through the gamut body. The cut is orthogonal to
 * the L axis, so it shows the a‑b diagram (speaking in terms of
 * LAB color model) respectively chroma‑hue diagram (speaking in terms
 * of LCH color model). The center of the coordinate system is in
 * the center of the image (floating point precision).
 *
 * Each pixel has the color that corresponds to the coordinate point \e {at
 * the middle} of the pixel for in-gamut coordinate points, and
 * a solid background color for out-of-gamut coordinate points.
 *
 * The \c {QImage} that is provided by this class has the
 * size \c {QSize( \l ChromaHueImageParameters::imageSizePhysical,
 * \l ChromaHueImageParameters::imageSizePhysical)}. There is an
 * imaginary circle in the center of the \c {QImage} with a distance
 * of \l ChromaHueImageParameters::borderPhysical to the border of
 * the \c {QImage}. All pixels within this imaginary circle, plus an
 * overlap for safety, are calculated correctly. All other pixels
 * have arbitrary values. Therefore, when you paint this
 * image somewhere, you have to clip the painting to the imaginary circle.
 * Thanks to the overlap, there will be no rendering artefacts, regardless
 * of whether you render the circle with or without antialiasing.
 *
 * This type is declared as type to Qt’s type system via
 * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
 * example if you want to use for \e {queued} signal-slot connections),
 * you might consider calling \c {qRegisterMetaType()} for
 * this type, once you have a QApplication object.
 */
struct ChromaHueImageParameters {
public:
    /*!
     * \brief The border size, measured in physical pixels. */
    qreal borderPhysical = 0;
    /*!
     * \brief The device pixel ratio as floating point. */
    qreal devicePixelRatioF = 1;
    /*!
     * \brief Image size, measured in physical pixels. */
    int imageSizePhysical = 0;
    /*!
     * \brief Lightness.
     *
     * This is the lightness (L) value in the LCH color model.
     *
     * Range: \c {[0, 100]} */
    qreal lightness = 50;
    /*!
     * \brief The color space into which the working space is projected.
     */
    LchSpace projectionSpace = LchSpace::CielchD50;

    /*!
     * \brief Equal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if equal, \c {false} otherwise.
     */
    [[nodiscard]] bool operator==(const ChromaHueImageParameters &other) const = default;

    /*!
     * \brief Unequal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if unequal, \c {false} otherwise.
     */
    [[nodiscard]] bool operator!=(const ChromaHueImageParameters &other) const = default;

    static void render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject);

private:
    [[nodiscard]] static InterlacingPass createInterlacingPassObject(const QSize imageSizePhysical);

    static void renderByRow(uchar *const bytesPtr,
                            const qsizetype bytesPerLine,
                            // cppcheck-suppress passedByValue
                            const ChromaHueImageParameters parameters,
                            const qreal shift,
                            const qreal scaleFactor,
                            const double chromaRange,
                            int firstRow,
                            int lastRow);

    static void renderByRowInterlaced(uchar *const bytesPtr,
                                      const qsizetype bytesPerLine,
                                      // cppcheck-suppress passedByValue
                                      const ChromaHueImageParameters parameters,
                                      const qreal shift,
                                      const qreal scaleFactor,
                                      const double chromaRange,
                                      const InterlacingPass currentPass,
                                      int firstRow,
                                      int lastRow);
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ChromaHueImageParameters)

#endif // PERCEPTUALCOLOR_CHROMAHUEIMAGEPARAMETERS_H
