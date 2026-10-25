// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_ASYNCIMAGERENDERTHREAD_H
#define PERCEPTUALCOLOR_ASYNCIMAGERENDERTHREAD_H

#include "asyncimagerendercallback.h"
#include <atomic>
#include <functional>
#include <qglobal.h>
#include <qmutex.h>
#include <qthread.h>
#include <qtmetamacros.h>
#include <qvariant.h>
#include <qwaitcondition.h>
class QImage;
class QObject;

namespace PerceptualColor
{
/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Provides threaded rendering for \l AsyncImageProvider. */
class AsyncImageRenderThread : public QThread, public AsyncImageRenderCallback
{
    Q_OBJECT

public:
    /*!
     * \typealias PerceptualColor::AsyncImageRenderThread::pointerToRenderFunction
     *
     * \relates PerceptualColor::AsyncImageRenderThread
     *
     * \brief Function pointer to a render function.
     *
     * The function pointed to by this pointer has \c {void} as its
     * return value. It has the following parameters:
     *
     * \a variantParameters A \c {QVariant} that contains the
     *        image parameters.
     *
     * \a callbackObject An object that provides the necessary
     *        callbacks.
     *
     * The function pointed to by this pointer is supposed to
     * render the image with the given parameters, and deliver the
     * result of each interlacing pass and also the final result by
     * callbacks. It also is supposed to check regularly via callbacks
     * if it should abort the rendering.
     *
     * The function pointed to by this pointer must be thread-safe.
     *
     * The render function is meant to be used
     * by \l AsyncImageRenderThread.
     *
     * \note It might be possible to use
     * \l {https://en.cppreference.com/w/cpp/utility/any} {std::any}
     * instead of \l {https://doc.qt.io/qt-6/qvariant.html} {QVariant}.
     * This might eliminate the need to register
     * types to the Qt meta-type system. On the other hand, it
     * probably will not integrate as well with other parts of Qt
     * (signals, slots…). So currently we are doing well by using
     * \l {https://doc.qt.io/qt-6/qvariant.html} {QVariant}.
     */
    using pointerToRenderFunction = std::function<void(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)>;

    explicit AsyncImageRenderThread(const pointerToRenderFunction &renderFunction, QObject *parent = nullptr);
    virtual ~AsyncImageRenderThread() override;

    virtual void deliverInterlacingPass(const QImage &image,
                                        const QImage &mask,
                                        const QVariant &parameters,
                                        const AsyncImageRenderCallback::InterlacingState state) override;
    void startRenderingAsync(const QVariant &parameters);
    [[nodiscard]] virtual bool shouldAbort() const override;
    void waitForIdle();

Q_SIGNALS:
    /*!
     * \brief Result of an interlacing pass of the \e {rendering}
     * operation.
     *
     * \e {Rendering} operations can be started
     * by \l startRenderingAsync().
     *
     * \note \e {Rendering} operations might be stopped before emitting
     * this signal by calling again \l startRenderingAsync(); therefore it
     * is \e {not} guaranteed that each call of \l startRenderingAsync()
     * will finally emit this signal.
     *
     * \a image The image
     *
     * \a mask The alpha mask, if provided. Renderers may choose whether
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
     *
     * \a parameters The parameters of the image
     *
     * \a state The interlacing state of the image. A render function
     * must first return zero or more images with intermediate state. After
     * that, it must return exactly one image with final state (unless it
     * was aborted). After that, it must not return any more images.
     *
     * \warning This signal can be emitted by a thread other than the
     * thread in which this object itself lives. Therefore, use only
     * \c {Qt::AutoConnection} or \c {Qt::QueuedConnection}
     * when connecting to this signal. */
    void interlacingPassCompleted(const QImage &image,
                                  const QImage &mask,
                                  const QVariant &parameters,
                                  const PerceptualColor::AsyncImageRenderCallback::InterlacingState state);

protected:
    virtual void run() override;

private:
    Q_DISABLE_COPY(AsyncImageRenderThread)

    friend class TestAsyncImageRenderThread;

    /*!
     * \brief Provide parameters for the next re(start) of run().
     *
     * run() is supposed to read these parameters on each round,
     * and to render a corresponding image.
     *
     * \note This data member has read and write access protected
     * by \l m_loopMutex. */
    QVariant m_imageParameters;
    /*!
     * \brief Request run() to abort.
     *
     * run() is supposed to control regularly if this value
     * is \c {true}. If so, it should return as fast as possible.
     * This variable is used by the destructor to make sure that the
     * associated thread is stopped before destroying this object.
     *
     * \warning This is used with \l m_loopCondition. See there for details.
     *
     * \note This data member has write access protected
     * by \l m_loopMutex. */
    std::atomic_bool m_loopAbort = false;
    /*!
     * \brief Wait condition used between the rendering rounds.
     *
     * \warning \l m_loopAbort and \l m_loopRestart are used to control the
     * waiting. Changing them requires locking \l m_loopMutex (otherwise,
     * this condition could become out-of-synchronization). Reading them
     * during the rendering to stop more immediately should be okay, as
     * both variables are atomic.
     *
     * \note See
     * \l {https://www.heise.de/developer/artikel/C-Core-Guidelines-Sei-dir-der-Fallen-von-Bedingungsvariablen-bewusst-4063822.html}
     * {this in-depth explication} or also
     * \l {https://www.grimm-jaud.de/index.php/blog/bedingungsvariablen}
     * {this other in-depth explication}, both of Rainer Grimm, for
     * more details about this synchronization pattern. */
    QWaitCondition m_loopCondition;
    /*!
     * \brief Mutex protection for \l m_loopAbort and \l m_loopRestart
     * and \l m_imageParameters.
     *
     * \warning This is used with \l m_loopCondition. See there for details. */
    QMutex m_loopMutex;
    /*!
     * \brief Request run() to restart its outer loop.
     *
     * run() is supposed to control regularly if this value is
     * \c {true}. If so, it should restart its outer loop as fast as
     * possible. This variable is set by \l startRenderingAsync() to
     * \c {true} to make sure that the outer loop restarts, and it is set
     * by run() to \c {false} once the restart of the outer loop
     * has happened.
     *
     * \warning This is used with \l m_loopCondition. See there for details.
     *
     * \note This data member has write access protected
     * by \l m_loopMutex. */
    std::atomic_bool m_loopRestart = false;
    /*!
     * \brief Function pointer to the function that does the
     * actual rendering. */
    const pointerToRenderFunction m_renderFunction;
    /*!
     * \brief Wait condition to wait until this thread goes to sleep. */
    QWaitCondition m_syncCondition;
    /*!
     * \brief Is \c {true} if the render thread is either sleeping
     * or not yet started at all. */
    std::atomic_bool m_syncIsIdle = true;
    /*!
     * \brief Mutex protection for \l m_syncCondition */
    QMutex m_syncMutex;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_ASYNCIMAGERENDERTHREAD_H
