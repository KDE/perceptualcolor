// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CHROMAHUEIMAGEPARAMETERS_H
#define CHROMAHUEIMAGEPARAMETERS_H

#include <qglobal.h>
#include <qmetatype.h>
#include <qsharedpointer.h>
#include <qvariant.h>

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
 *
 * @internal
 *
 * @todo Why does @ref ChromaHueImageParameters::render() not make everything
 * outside the circle transparent? Because it would look ugly without
 * antialiasing. And when we use antialiasing various times to cut of
 * unwanted artefacts, half-opaque pixel become quarter-opaque and so on,
 * so this would be ugly, too. However, we could use a single image to
 * work on, and for each interlacing pass result, create a copy and
 * apply the antialiased circle only to the copy. This would of course
 * require more memory. On the other hand: When calling
 * @ref AsyncImageRenderThread::deliverInterlacingPass() a signal will
 * be emitted, which will create a copy anyway…
 *
 * @todo Make <tt>generatescreenshots.cpp</tt> work again:
 *
 * To provide information about the state of the asynchronous processing
 * that are necessary for <tt>generatescreenshots.cpp</tt> to get the
 * full-quality image screenshot, there might be two options:
 * - Change the name of all test classes to <em>one</em> single name: “Test”.
 *   This would unify all the friend declarations of all our classes. So
 *   <tt>generatescreenshots.cpp</tt> could also define a class named “Test”
 *   to get the necessary access…
 * - Add an API for example to @ref AsyncImageProvider that gives information
 *   about the current state of the rendering (empty, low-quality,
 *   full-quality), which would give <tt>generatescreenshots.cpp</tt>
 *   the possibility to wait until the full-quality image is available.
 *   This API could be available in <tt>libperceptualcolorinternal</tt> only,
 *   controlled by conditional compiling using <tt>\#if</tt>. */
struct ChromaHueImageParameters final {
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
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ChromaHueImageParameters)

#endif // CHROMAHUEIMAGEPARAMETERS_H
