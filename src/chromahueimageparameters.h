// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CHROMAHUEIMAGEPARAMETERS_H
#define CHROMAHUEIMAGEPARAMETERS_H

#include "interlacingpass.h"
#include <qglobal.h>
#include <qmetatype.h>
#include <qsharedpointer.h>
#include <qvariant.h>

class QSize;

namespace PerceptualColor
{
class AsyncImageRenderCallback;
class RgbColorSpace;

/** @internal
 *
 * @brief Parameters for an image of a chroma hue plane.
 *
 * For usage with @ref AsyncImageProvider.
 *
 * @warning The default constructor constructs an object with an empty
 * @ref rgbColorSpace. Before using this object, you should initialize
 * @ref rgbColorSpace.
 *
 * This is a cut through the gamut body. The cut is orthogonal to
 * the L axis, so it shows the a‑b diagram (speaking in terms of
 * LAB color model) respectively chroma‑hue diagram (speaking in terms
 * of LCH color model). The center of the coordinate system is in
 * the center of the image (floating point precision).
 *
 * Each pixel has the color that corresponds to the coordinate point <em>at
 * the middle</em> of the pixel for in-gamut coordinate points, and
 * a solid background color for out-of-gamut coordinate points.
 *
 * The <tt>QImage</tt> that is provided by this class has the
 * size <tt>QSize(@ref ChromaHueImageParameters::imageSizePhysical,
 * @ref ChromaHueImageParameters::imageSizePhysical)</tt>. There is an
 * imaginary circle in the center of the <tt>QImage</tt> with a distance
 * of @ref ChromaHueImageParameters::borderPhysical to the border of
 * the <tt>QImage</tt>. All pixels within this imaginary circle, plus an
 * overlap for safety, are calculated correctly. All other pixels
 * have arbitrary values. Therefore, when you paint this
 * image somewhere, you have to clip the painting to the imaginary circle.
 * Thanks to the overlap, there will be no rendering artefacts, regardless
 * of whether you render the circle with or without antialiasing.
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 */
struct ChromaHueImageParameters {
public:
    /** @brief The border size, measured in physical pixels. */
    qreal borderPhysical = 0;
    /** @brief The device pixel ratio as floating point. */
    qreal devicePixelRatioF = 1;
    /** @brief Image size, measured in physical pixels. */
    int imageSizePhysical = 0;
    /** @brief Lightness.
     *
     * This is the lightness (L) value in the LCH color model.
     *
     * Range: <tt>[0, 100]</tt> */
    qreal lightness = 50;
    /** @brief Pointer to @ref RgbColorSpace object
     *
     * @warning The default constructor constructs an object with an empty
     * @ref rgbColorSpace. Before using this object, you must initialize
     * @ref rgbColorSpace. */
    QSharedPointer<PerceptualColor::RgbColorSpace> rgbColorSpace = nullptr;
    [[nodiscard]] bool operator==(const ChromaHueImageParameters &other) const;
    [[nodiscard]] bool operator!=(const ChromaHueImageParameters &other) const;

    static void render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject);

private:
    static InterlacingPass createInterlacingPassObject(const QSize imageSizePhysical);

    static void renderByRow(const AsyncImageRenderCallback &callbackObject,
                            uchar *const bytesPtr,
                            const qsizetype bytesPerLine,
                            const ChromaHueImageParameters parameters,
                            const qreal shift,
                            const qreal scaleFactor,
                            const InterlacingPass currentPass,
                            int firstRow,
                            int lastRow);
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ChromaHueImageParameters)

#endif // CHROMAHUEIMAGEPARAMETERS_H
