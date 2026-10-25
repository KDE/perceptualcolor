// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_ASYNCIMAGEPROVIDER_H
#define PERCEPTUALCOLOR_ASYNCIMAGEPROVIDER_H

#include "asyncimageproviderbase.h"
#include "asyncimagerenderthread.h"
#include <optional>
#include <qglobal.h>
#include <qimage.h>
#include <qmetatype.h>
#include <qtmetamacros.h>
#include <qvariant.h>

class QObject;

namespace PerceptualColor
{
/*!
 * \inmodule ModulePrivateApi
 *
 * \class PerceptualColor::AsyncImageProvider
 *
 * \brief Support for image caching and asynchronous rendering.
 *
 * This class template is intended for images whose calculation is expensive.
 * You need a (thread-safe) rendering function, and this class template will
 * provide automatically thread-support and image caching.
 *
 * \note This class template requires a running event loop.
 *
 * \b T The data type which will be used to parameterize
 * the image.
 *
 * \section1 Features
 *
 * \list
 * \li Asynchronous API: The image calculation is done in background
 *   thread(s). Results are communicated by means of the
 *   signal \l interlacingPassCompleted as soon as they are available.
 * \li Optional interlacing support: The rendering function can
 *   provide a low-quality image first, and then progressively
 *   better images until the final full-quality image. Since today’s
 *   high-DPI screens have more and more pixels (4K screens, perhaps
 *   one day 8K screens?), interlacing becomes increasingly important,
 *   especially with complex image calculation. The \l InterlacingPass
 *   helper class makes it easy to implement  Adam7-like interlacing.
 * \li Cache: As the image calculation might be expensive, resulting image is
 *   cached for further usage.
 * \endlist
 *
 * \section1 How to create an object
 *
 * \snippet testasyncimageprovider.cpp How to create
 *
 * \section1 How to use an object
 *
 * The cache can be accessed with \l getCache(). Note that the
 * cache is \e {not} refreshed implicitly after changing the
 * \l imageParameters(); therefore the cache can be out-of-date.
 * Use \l refreshAsync() to request explicitly a refresh.
 *
 * \section1 Further reading
 *
 * \sa AsyncImageRenderThread::pointerToRenderFunction
 *
 * \note This class template is reentrant, but \e {not} thread-safe!
 *
 * \section1 Internals
 *
 * \note \l {https://stackoverflow.com/a/63021891} {The Q_OBJECT
 * macro and templates cannot be combined.} Therefore,
 * \l AsyncImageProviderBase serves as a base class to provide
 * signals for \l AsyncImageProvider.
 */
template<typename T>
class AsyncImageProvider : public AsyncImageProviderBase
{
    // Here is no Q_OBJECT macro because it cannot be combined with templates.
    // See https://stackoverflow.com/a/63021891 for more information.

public:
    /*!
     * \brief Constructor
     *
     * \a parent The object’s parent object. This parameter will be passed
     * to the base class’s constructor. */
    explicit AsyncImageProvider(QObject *parent = nullptr);

    /*!
     * \brief Destructor
     */
    virtual ~AsyncImageProvider() noexcept override;

    /*!
     * \brief Provides the content of the alpha mask cache.
     *
     * Returns the content of the alpha mask cache. Note that a cached alpha mask
     * might be out-of-date. The cache might also be empty, which is represented
     * by a null image. */
    [[nodiscard]] QImage getMaskCache() const;

    /*!
     * \brief Provides the content of the image cache.
     *
     * Returns the content of the image cache. Note that a cached image
     * might be out-of-date. The cache might also be empty, which is represented
     * by a null image. */
    [[nodiscard]] QImage getCache() const;

    // NOTE This cannot be a Q_PROPERTY as its type depends on the template
    // parameter, and Q_PROPERTY is based on Q_OBJECT which cannot be used
    // within templates. */
    /*!
     * \brief Getter for the image parameters.
     *
     * \sa setImageParameters()
     */
    [[nodiscard]] T imageParameters() const;

    /*!
     * \brief Asynchronously triggers a refresh of the image cache (if
     * necessary).
     */
    void refreshAsync();

    /*!
     * \brief Synchronously refreshes the image cache (if necessary).
     */
    void refreshSync();

    // NOTE This cannot be a Q_PROPERTY as its type depends on the template
    // parameter, and Q_PROPERTY is based on Q_OBJECT which cannot be used
    // within templates.
    /*!
     * \brief Setter for the image parameters.
     *
     * \a newImageParameters The new image parameters.
     *
     * \note This function does \e {not} trigger a new image calculation.
     * Only \l refreshAsync() can trigger a new image calculation.
     *
     * \sa imageParameters()
     */
    void setImageParameters(const T &newImageParameters);

private:
    Q_DISABLE_COPY(AsyncImageProvider)

    friend class TestAsyncImageProvider;

    /*!
     * \brief Receives and processes newly rendered images that are
     * delivered from the background render process.
     *
     * \a deliveredImage The image (either interlaced or full-quality)
     *
     * \a deliveredMask The alpha mask, if provided. Renderers may choose
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
     *  The new image, and if available the alpha mask, will be put into the
     * cache and the signal \l interlacingPassCompleted() is emitted.
     *
     * This function is meant to be called by the background render process to
     * deliver more data. It \e {must} be called after each interlacing pass
     * exactly one time. (If the background process does not support interlacing,
     * it is called only once when the image rendering is done.)
     *
     * \note Like the whole class template, this function is not thread-safe.
     * You \e {must} call it from the thread within this object lives. It is
     * not declared as slot either (because templates and \e {Q_OBJECT} are
     * incompatible). To call it from a background thread, you can however use
     * the functor-based \c {Qt::connect()} syntax to connect to this function
     * as long as the connection type is not direct, but queued. */
    void processInterlacingPassResult(const QImage &deliveredImage, const QImage &deliveredMask);

    /*!
     * \brief The mask cache. */
    QImage m_maskCache;

    /*!
     * \brief The image cache. */
    QImage m_cache;

    /*!
     * \brief Internal storage for the image parameters.
     *
     * \sa imageParameters()
     * \sa setImageParameters()
     */
    T m_imageParameters;

    /*!
     * \brief Information about deliverd images of the last rendering
     * request.
     *
     * Is \c {true} if the last rendering request has yet
     * delivered at least \e {one} image, regardless of the
     * \l AsyncImageRenderCallback::InterlacingState of the
     * delivered image. Is \c {false} otherwise. */
    bool m_lastRenderingRequestHasYetDeliveredAnImage = false;

    /*!
     * \brief The parameters of the last rendering that has been started
     * (if any). */
    std::optional<T> m_lastRenderingRequestImageParameters;

    /*!
     * \brief Provides a render thread. */
    AsyncImageRenderThread m_renderThread;
};

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

template<typename T>
AsyncImageProvider<T>::~AsyncImageProvider() noexcept
{
}

template<typename T>
QImage AsyncImageProvider<T>::getCache() const
{
    // m_cache is supposed to be a null image if the cache is empty.
    return m_cache;
}

template<typename T>
QImage AsyncImageProvider<T>::getMaskCache() const
{
    // m_maskCache is supposed to be a null image if the cache is empty.
    return m_maskCache;
}

template<typename T>
void AsyncImageProvider<T>::setImageParameters(const T &newImageParameters)
{
    m_imageParameters = newImageParameters;
}

template<typename T>
T AsyncImageProvider<T>::imageParameters() const
{
    return m_imageParameters;
}

template<typename T>
void AsyncImageProvider<T>::processInterlacingPassResult(const QImage &deliveredImage, const QImage &deliveredMask)
{
    m_cache = deliveredImage;
    if (!deliveredMask.isNull()) {
        m_maskCache = deliveredMask;
    }
    Q_EMIT interlacingPassCompleted();
}

template<typename T>
void AsyncImageProvider<T>::refreshAsync()
{
    if (imageParameters() == m_lastRenderingRequestImageParameters) {
        return;
    }
    m_renderThread.startRenderingAsync(QVariant::fromValue(imageParameters()));
    m_lastRenderingRequestImageParameters = imageParameters();
}

template<typename T>
void AsyncImageProvider<T>::refreshSync()
{
    refreshAsync();
    m_renderThread.waitForIdle();
}

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_ASYNCIMAGEPROVIDER_H
