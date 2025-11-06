// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef ASYNCIMAGEPROVIDER_H
#define ASYNCIMAGEPROVIDER_H

#include "asyncimageproviderbase.h"
#include "asyncimagerenderthread.h"
#include <optional>
#include <qglobal.h>
#include <qimage.h>
#include <qmetatype.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

class QObject;

namespace PerceptualColor
{
/** @internal
 *
 * @brief Support for image caching and asynchronous rendering.
 *
 * This class template is intended for images whose calculation is expensive.
 * You need a (thread-safe) rendering function, and this class template will
 * provide automatically thread-support and image caching.
 *
 * @note This class template requires a running event loop.
 *
 * @tparam T The data type which will be used to parameterize
 * the image.
 *
 * @section asyncimageproviderfeatures Features
 *
 * - Asynchronous API: The image calculation is done in background
 *   thread(s). Results are communicated by means of the
 *   signal @ref interlacingPassCompleted as soon as they are available.
 * - Optional interlacing support: The rendering function can
 *   provide a low-quality image first, and then progressively
 *   better images until the final full-quality image. Since today’s
 *   high-DPI screens have more and more pixels (4K screens, perhaps
 *   one day 8K screens?), interlacing becomes increasingly important,
 *   especially with complex image calculation. The @ref InterlacingPass
 *   helper class makes it easy to implement  Adam7-like interlacing.
 * - Cache: As the image calculation might be expensive, resulting image is
 *   cached for further usage.
 *
 * @section asyncimagecreate How to create an object
 *
 * @snippet testasyncimageprovider.cpp How to create
 *
 * @section asyncimageuse How to use an object
 *
 * The cache can be accessed with @ref getCache(). Note that the
 * cache is <em>not</em> refreshed implicitly after changing the
 * @ref imageParameters(); therefore the cache can be out-of-date.
 * Use @ref refreshAsync() to request explicitly a refresh.
 *
 * @section asyncimagefurther Further reading
 *
 * @sa @ref AsyncImageRenderThread::pointerToRenderFunction.
 *
 * @note This class template is reentrant, but <em>not</em> thread-safe!
 *
 * @internal
 *
 * @section asyncimageinternals Internals
 *
 * @note <a href="https://stackoverflow.com/a/63021891">The <tt>Q_OBJECT</tt>
 * macro and templates cannot be combined.</a> Therefore,
 * @ref AsyncImageProviderBase serves as a base class to provide
 * signals for @ref AsyncImageProvider.
 *
 * @todo SHOULDHAVE In @ref ChromaLightnessImageParameters::render(),
 * @ref ChromaHueImageParameters::render(),
 * @ref GradientImageParameters::render()  Do not cancel rendering
 * until the first (interlacing) result has been delivered to make sure that
 * slowly but continuously moving slider see at least sometimes updates… (and
 * it's more likely the current value is near to the last value than to the
 * old value still in the buffer before the user started moving the cursor
 * at all). The performance impact should be minimal when interlacing is
 * used. And if no interlacing is available, the impact should not be
 * too bad either.
 *
 * @note It would be nice to merge @ref AsyncImageProviderBase and
 * @ref AsyncImageProvider into one single class (that is <em>not</em> a
 * template, but image parameters are now given in form of a QVariant).
 * It would take @ref AsyncImageRenderThread::pointerToRenderFunction as
 * argument in the constructor to be able to call the constructor of
 * @ref AsyncImageRenderThread.
 * <br/>
 * <b>Advantage:</b>
 * <br/>
 * → Only one class is compiled, instead of a whole bunch of template classes.
 *   The binary will therefore be smaller.
 * <br/>
 * <b>Disadvantage:</b>
 * <br/>
 * → In the future, maybe we could add support within the template for a
 *   per-class inter-object cache, so that if two objects of the same class
 *   have the same @ref imageParameters then the rendering is done only once
 *   and the result is shared between these two instances. This would
 *   obviously be impossible if there are no longer different classes
 *   for different type of images. Or it would at least require a special
 *   solution…
 * <br/>
 * → Calling @ref setImageParameters would be done with a <tt>QVariant</tt>
 *   (or an <tt>std::any</tt>?), so there would be no compile-time error
 *   anymore if the data type of the parameters is wrong – but is this
 *   really a big issue in practice?  */
template<typename T>
class AsyncImageProvider : public AsyncImageProviderBase
{
    // Here is no Q_OBJECT macro because it cannot be combined with templates.
    // See https://stackoverflow.com/a/63021891 for more information.

public:
    explicit AsyncImageProvider(QObject *parent = nullptr);
    virtual ~AsyncImageProvider() noexcept override;

    [[nodiscard]] QImage getMaskCache() const;
    [[nodiscard]] QImage getCache() const;
    [[nodiscard]] T imageParameters() const;
    void refreshAsync();
    void refreshSync();
    void setImageParameters(const T &newImageParameters);

private:
    Q_DISABLE_COPY(AsyncImageProvider)

    /** @internal @brief Only for unit tests. */
    friend class TestAsyncImageProvider;

    void processInterlacingPassResult(const QImage &deliveredImage, const QImage &deliveredMask);

    /** @brief The mask cache. */
    QImage m_maskCache;
    /** @brief The image cache. */
    QImage m_cache;
    /** @brief Internal storage for the image parameters.
     *
     * @sa @ref imageParameters()
     * @sa @ref setImageParameters() */
    T m_imageParameters;
    /** @brief Information about deliverd images of the last rendering
     * request.
     *
     * Is <tt>true</tt> if the last rendering request has yet
     * delivered at least <em>one</em> image, regardless of the
     * @ref AsyncImageRenderCallback::InterlacingState of the
     * delivered image. Is <tt>false</tt> otherwise. */
    bool m_lastRenderingRequestHasYetDeliveredAnImage = false;
    /** @brief The parameters of the last rendering that has been started
     * (if any). */
    std::optional<T> m_lastRenderingRequestImageParameters;
    /** @brief Provides a render thread. */
    AsyncImageRenderThread m_renderThread;
};

/** @brief Constructor
 * @param parent The object’s parent object. This parameter will be passed
 * to the base class’s constructor. */
template<typename T>
AsyncImageProvider<T>::AsyncImageProvider(QObject *parent)
    : AsyncImageProviderBase(parent)
    , m_renderThread(&T::render)
{
    // Calling qRegisterMetaType is safe even if a given type has yet
    // been registered before.
    qRegisterMetaType<T>();
    connect( //
        &m_renderThread, //
        &AsyncImageRenderThread::interlacingPassCompleted, //
        this, //
        &AsyncImageProvider<T>::processInterlacingPassResult);
}

/** @brief Destructor */
template<typename T>
AsyncImageProvider<T>::~AsyncImageProvider() noexcept
{
}

/** @brief Provides the content of the image cache.
 *
 * @returns The content of the image cache. Note that a cached image
 * might be out-of-date. The cache might also be empty, which is represented
 * by a null image. */
template<typename T>
QImage AsyncImageProvider<T>::getCache() const
{
    // m_cache is supposed to be a null image if the cache is empty.
    return m_cache;
}

/** @brief Provides the content of the alpha mask cache.
 *
 * @returns The content of the alpha mask cache. Note that a cached alpha mask
 * might be out-of-date. The cache might also be empty, which is represented
 * by a null image. */
template<typename T>
QImage AsyncImageProvider<T>::getMaskCache() const
{
    // m_maskCache is supposed to be a null image if the cache is empty.
    return m_maskCache;
}

/** @brief Setter for the image parameters.
 *
 * @param newImageParameters The new image parameters.
 *
 * @note This function does <em>not</em> trigger a new image calculation.
 * Only @ref refreshAsync() can trigger a new image calculation.
 *
 * @sa @ref imageParameters()
 *
 * @internal
 *
 * @sa @ref m_imageParameters */
// NOTE This cannot be a Q_PROPERTY as its type depends on the template
// parameter, and Q_PROPERTY is based on Q_OBJECT which cannot be used
// within templates.
template<typename T>
void AsyncImageProvider<T>::setImageParameters(const T &newImageParameters)
{
    m_imageParameters = newImageParameters;
}

/** @brief Getter for the image parameters.
 *
 * @returns The current image parameters.
 *
 * @sa @ref setImageParameters()
 *
 * @internal
 *
 * @sa @ref m_imageParameters */
// NOTE This cannot be a Q_PROPERTY as its type depends on the template
// parameter, and Q_PROPERTY is based on Q_OBJECT which cannot be used
// within templates. */
template<typename T>
T AsyncImageProvider<T>::imageParameters() const
{
    return m_imageParameters;
}

/** @brief Receives and processes newly rendered images that are
 * delivered from the background render process.
 *
 * @param deliveredImage The image (either interlaced or full-quality)
 * @param deliveredMask The alpha mask, if provided. Renderers may choose
 * whether to supply an alpha mask. Alpha masks are 1-bit images where white
 * represents transparency and black represents opacity, defining the
 * transparency state <i>before</i> any anti-aliasing is applied. This
 * differs from the potentially anti-aliased image itself, which may
 * contain partial transparency, making it difficult to determine the
 * original transparency before anti-aliasing. Typically, fully transparent
 * pixels will have an alpha value greater than 50% after anti-aliasing,
 * but in some cases, they may fall below this threshold. The alpha mask,
 * however, provides a clear and definitive indication of each pixel’s
 * validity.
 *
 * @post The new image, and if available the alpha mask, will be put into the
 * cache and the signal @ref interlacingPassCompleted() is emitted.
 *
 * This function is meant to be called by the background render process to
 * deliver more data. It <em>must</em> be called after each interlacing pass
 * exactly one time. (If the background process does not support interlacing,
 * it is called only once when the image rendering is done.)
 *
 * @note Like the whole class template, this function is not thread-safe.
 * You <em>must</em> call it from the thread within this object lives. It is
 * not declared as slot either (because templates and <em>Q_OBJECT</em> are
 * incompatible). To call it from a background thread, you can however use
 * the functor-based <tt>Qt::connect()</tt> syntax to connect to this function
 * as long as the connection type is not direct, but queued. */
template<typename T>
void AsyncImageProvider<T>::processInterlacingPassResult(const QImage &deliveredImage, const QImage &deliveredMask)
{
    m_cache = deliveredImage;
    if (!deliveredMask.isNull()) {
        m_maskCache = deliveredMask;
    }
    Q_EMIT interlacingPassCompleted();
}

/** @brief Asynchronously triggers a refresh of the image cache (if
 * necessary). */
template<typename T>
void AsyncImageProvider<T>::refreshAsync()
{
    if (imageParameters() == m_lastRenderingRequestImageParameters) {
        return;
    }
    m_renderThread.startRenderingAsync(QVariant::fromValue(imageParameters()));
    m_lastRenderingRequestImageParameters = imageParameters();
}

/** @brief Synchronously refreshes the image cache (if necessary). */
template<typename T>
void AsyncImageProvider<T>::refreshSync()
{
    refreshAsync();
    m_renderThread.waitForIdle();
}

} // namespace PerceptualColor

#endif // ASYNCIMAGEPROVIDER_H
