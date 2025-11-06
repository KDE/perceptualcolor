// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef ASYNCIMAGERENDERCALLBACK_H
#define ASYNCIMAGERENDERCALLBACK_H

#include <qglobal.h>
#include <qmetatype.h>

class QImage;
class QVariant;

namespace PerceptualColor
{
/** @internal
 *
 * @brief Interface for @ref AsyncImageRenderThread::pointerToRenderFunction
 * to make callbacks. */
class AsyncImageRenderCallback
{
public:
    virtual ~AsyncImageRenderCallback() noexcept;

    /** @brief Describes the interlacing state of an image.
     *
     * This enum is <em>not</em> declared to the meta-object system.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    // Q_ENUM can only be used within classes that have the Q_OBJECT macro,
    // which this class template does not have, because we want to avoid
    // conflicts when doing multiple inheritance with this class.
    // Tough the Qt documentation allows using Q_ENUM with Q_GADGET instead
    // of Q_OBJECT, in practice it does not work: It compiles without warnings
    // or errors, but the meta type is nevertheless not registered and
    // not available for signals and slots. Therefore, we do not use Q_ENUM
    // here.
    enum class InterlacingState {
        Intermediate, /**< The image represents
            an intermediate interlacing result. */
        Final /**< The image represents the final image in full quality.
            No further interlacing passes will happen. */
    };

    /** @brief Deliver the result of an interlacing pass of
     * the <em>rendering</em> operation.
     *
     * This function is thread-safe.
     *
     * @param image The image
     * @param mask The alpha mask, if provided. Renderers may choose whether
     * to supply an alpha mask. Alpha masks are 1-bit images where white
     * represents transparency and black represents opacity, defining the
     * transparency state <i>before</i> any anti-aliasing is applied. This
     * differs from the potentially anti-aliased image itself, which may
     * contain partial transparency, making it difficult to determine the
     * original transparency before anti-aliasing. Typically, fully transparent
     * pixels will have an alpha value greater than 50% after anti-aliasing,
     * but in some cases, they may fall below this threshold. The alpha mask,
     * however, provides a clear and definitive indication of each pixel’s
     * validity.
     * @param parameters The parameters of the image
     * @param state The interlacing state of the image. A render function
     * must first return zero or more images with intermediate state. After
     * that, it must return exactly one image with final state (unless it
     * was aborted). After that, it must not return any more images. */
    virtual void deliverInterlacingPass(const QImage &image, const QImage &mask, const QVariant &parameters, const InterlacingState state) = 0;

    /** @brief If the render function should abort.
     *
     * This function is thread-safe.
     *
     * @returns <tt>true</tt> if the render function should abort and
     * return. <tt>false</tt> otherwise. */
    [[nodiscard]] virtual bool shouldAbort() const = 0;

protected:
    AsyncImageRenderCallback() = default;

private:
    Q_DISABLE_COPY(AsyncImageRenderCallback)
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::AsyncImageRenderCallback::InterlacingState)

#endif // ASYNCIMAGERENDERCALLBACK_H
