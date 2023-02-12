// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef ASYNCIMAGERENDERTHREAD_H
#define ASYNCIMAGERENDERTHREAD_H

#include "asyncimagerendercallback.h"
#include <atomic>
#include <functional>
#include <qglobal.h>
#include <qmutex.h>
#include <qthread.h>
#include <qvariant.h>
#include <qwaitcondition.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

class QImage;
class QObject;

namespace PerceptualColor
{
/** @internal
 *
 * @brief Provides threaded rendering for @ref AsyncImageProvider. */
class AsyncImageRenderThread : public QThread, public AsyncImageRenderCallback
{
    Q_OBJECT

public:
    /** @brief Function pointer to a render function.
     *
     * The function pointed to by this pointer has <tt>void</tt> as its
     * return value. It has the following parameters:
     *
     * @param variantParameters A <tt>QVariant</tt> that contains the
     *        image parameters.
     * @param callbackObject An object that provides the necessary
     *        callbacks.
     *
     * The function pointed to by this pointer is supposed to
     * render the image with the given parameters, and deliver the
     * result of each interlacing pass and also the final result by
     * callbacks. It also is supposed to check regularly via callbacks
     * if it should abort the rendering.
     *
     * The function pointed to by this pointer must be thread-safe.
     *
     * @internal
     *
     * The render function is meant to be used
     * by @ref AsyncImageRenderThread.
     *
     * @note It might be possible to use <tt>
     * <a href="https://en.cppreference.com/w/cpp/utility/any">std::any</a>
     * </tt> instead of <tt><a href="https://doc.qt.io/qt-6/qvariant.html">
     * QVariant</a></tt>. This might eliminate the need to register
     * types to the Qt meta-type system. On the other hand, it
     * probably will not integrate as well with other parts of Qt
     * (signals, slots…). So currently we are doing well by using <tt>
     * <a href="https://doc.qt.io/qt-6/qvariant.html"> QVariant</a></tt>. */
    using pointerToRenderFunction = std::function<void(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)>;

    explicit AsyncImageRenderThread(const pointerToRenderFunction &renderFunction, QObject *parent = nullptr);
    virtual ~AsyncImageRenderThread() override;

    virtual void deliverInterlacingPass(const QImage &image, const QVariant &parameters, const AsyncImageRenderCallback::InterlacingState state) override;
    void startRenderingAsync(const QVariant &parameters);
    [[nodiscard]] virtual bool shouldAbort() const override;
    void waitForIdle();

Q_SIGNALS:
    /** @brief Result of an interlacing pass of the <em>rendering</em>
     * operation.
     *
     * <em>Rendering</em> operations can be started
     * by @ref startRenderingAsync().
     *
     * @note <em>Rendering</em> operations might be stopped before emitting
     * this signal by calling again @ref startRenderingAsync(); therefore it
     * is <em>not</em> guaranteed that each call of @ref startRenderingAsync()
     * will finally emit this signal.
     *
     * @param image The image
     * @param parameters The parameters of the image
     * @param state The interlacing state of the image. A render function
     * must first return zero or more images with intermediate state. After
     * that, it must return exactly one image with final state (unless it
     * was aborted). After that, it must not return any more images.
     *
     * @warning This signal can be emitted by a thread other than the
     * thread in which this object itself lives. Therefore, use only
     * <tt>Qt::AutoConnection</tt> or <tt>Qt::QueuedConnection</tt>
     * when connecting to this signal. */
    void interlacingPassCompleted(const QImage &image, const QVariant &parameters, const PerceptualColor::AsyncImageRenderCallback::InterlacingState state);

protected:
    virtual void run() override;

private:
    Q_DISABLE_COPY(AsyncImageRenderThread)

    /** @internal @brief Only for unit tests. */
    friend class TestAsyncImageRenderThread;

    /** @brief Provide parameters for the next re(start) of @ref run().
     *
     * @ref run() is supposed to read these parameters on each round,
     * and to render a corresponding image.
     *
     * @note This data member has read and write access protected
     * by @ref m_loopMutex. */
    QVariant m_imageParameters;
    /** @brief Request @ref run() to abort.
     *
     * @ref run() is supposed to control regularly if this value
     * is <tt>true</tt>. If so, it should return as fast as possible.
     * This variable is used by the destructor to make sure that the
     * associated thread is stopped before destroying this object.
     *
     * @warning This is used with @ref m_loopCondition. See there for details.
     *
     * @note This data member has write access protected
     * by @ref m_loopMutex. */
    std::atomic_bool m_loopAbort = false;
    /** @brief Wait condition used between the rendering rounds.
     *
     * @warning @ref m_loopAbort and @ref m_loopRestart are used to control the
     * waiting. Changing them requires locking @ref m_loopMutex (otherwise,
     * this condition could become out-of-synchronization). Reading them
     * during the rendering to stop more immediately should be okay, as
     * both variables are atomic.
     *
     * @note See
     * <a href="https://www.heise.de/developer/artikel/C-Core-Guidelines-Sei-dir-der-Fallen-von-Bedingungsvariablen-bewusst-4063822.html">
     * this in-depth explication</a> or also
     * <a href="https://www.grimm-jaud.de/index.php/blog/bedingungsvariablen">
     * this other in-depth explication</a>, both of Rainer Grimm, for
     * more details about this synchronization pattern. */
    QWaitCondition m_loopCondition;
    /** @brief Mutex protection for @ref m_loopAbort and @ref m_loopRestart
     * and @ref m_imageParameters.
     *
     * @warning This is used with @ref m_loopCondition. See there for details. */
    QMutex m_loopMutex;
    /** @brief Request @ref run() to restart its outer loop.
     *
     * @ref run() is supposed to control regularly if this value is
     * <tt>true</tt>. If so, it should restart its outer loop as fast as
     * possible. This variable is set by @ref startRenderingAsync() to
     * <tt>true</tt> to make sure that the outer loop restarts, and it is set
     * by @ref run() to <tt>false</tt> once the restart of the outer loop
     * has happened.
     *
     * @warning This is used with @ref m_loopCondition. See there for details.
     *
     * @note This data member has write access protected
     * by @ref m_loopMutex. */
    std::atomic_bool m_loopRestart = false;
    /** @brief Function pointer to the function that does the
     * actual rendering. */
    const pointerToRenderFunction m_renderFunction;
    /** @brief Wait condition to wait until this thread goes to sleep. */
    QWaitCondition m_syncCondition;
    /** @brief Is <tt>true</tt> if the render thread is either sleeping
     * or not yet started at all. */
    std::atomic_bool m_syncIsIdle = true;
    /** @brief Mutex protection for @ref m_syncCondition */
    QMutex m_syncMutex;
};

} // namespace PerceptualColor

#endif // ASYNCIMAGERENDERTHREAD_H
