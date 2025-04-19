// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "asyncimagerenderthread.h"

#include <qglobal.h>
#include <qmetatype.h>

class QObject;

namespace PerceptualColor
{
/** @brief The constructor.
 *
 * @param renderFunction Pointer to the render function that will be used.
 * @param parent The widget’s parent widget. This parameter will be passed
 * to the base class’s constructor. */
AsyncImageRenderThread::AsyncImageRenderThread(const pointerToRenderFunction &renderFunction, QObject *parent)
    : QThread(parent)
    , m_renderFunction(renderFunction)
{
    qRegisterMetaType<PerceptualColor::AsyncImageRenderCallback::InterlacingState>();
}

/** @brief The destructor.
 *
 * This destructor might takes a little while because he has to
 * stop the associated thread before destroying it: Possibly running
 * rendering operations are aborted. */
AsyncImageRenderThread::~AsyncImageRenderThread()
{
    m_loopMutex.lock();
    m_loopAbort = true;
    m_loopCondition.wakeOne();
    m_loopMutex.unlock();

    wait(); // Wait for the thread to terminate.

    // We make sure no thread will stay blocked when this object is
    // destroyed. However, given that this class itself is NOT thread-safe,
    // anyway it isn’t allowed to execute the destructor and waitForIdle()
    // in parallel. Therefore, this should be a no-operation. We stays
    // here just to feel safe.
    m_syncCondition.wakeAll();
}

/** @brief Asynchronously start rendering.
 *
 * As this function is asynchronous, it will return very fast.
 *
 * @param parameters The parameters of the requested rendering.
 *
 * @post If the <tt>parameters</tt> are different from those at the last
 * call, a new rendering of the new parameters will be started. (If there
 * is currently a rendering of other parameters in progress, this rendering
 * will be requested to stop as soon as possible.) If the <tt>parameters</tt>
 * are identical to those at the last call, nothing happens.
 *
 * The rendering will emit the signal @ref interlacingPassCompleted(). */
void AsyncImageRenderThread::startRenderingAsync(const QVariant &parameters)
{
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QMutexLocker<QMutex> loopLocker(&m_loopMutex);
#else
    QMutexLocker loopLocker(&m_loopMutex);
#endif

    if (m_imageParameters == parameters) {
        // Nothing to do here.
        return;
    }

    m_imageParameters = parameters;

    {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
        QMutexLocker<QMutex> syncLocker(&m_syncMutex);
#else
        QMutexLocker syncLocker(&m_syncMutex);
#endif
        m_syncIsIdle = false;
    }
    if (!isRunning()) {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 9, 0))
        // The scheduler should run this thread to a high-performance CPU core
        // to get the image as fast as possible.
        setServiceLevel(QThread::QualityOfService::High);
#endif
        // But avoid blocking other threads:
        start(LowPriority); // One priority level lower than normal priority.
    } else {
        m_loopRestart = true;
        m_loopCondition.wakeOne();
    }
}

/** @brief The code that will run within the thread.
 *
 * Reimplemented from base class.
 *
 * This is a wrapper that provides the thread-control (loops and so on).
 * The actual rendering is done by calling @ref m_renderFunction. */
void AsyncImageRenderThread::run()
{
    Q_FOREVER {
        m_loopMutex.lock();
        const QVariant parameters = m_imageParameters;
        m_loopMutex.unlock();

        if (m_loopAbort) {
            return;
        }

        // From Qt Example’s documentation:
        //
        //     “If we discover inside […] [this function call] that restart
        //      has been set to true (by render()), this function will return
        //      immediately, so that the control quickly returns to the very
        //      top of […] the forever loop […] and we fetch the new rendering
        //      parameters. Similarly, if we discover that abort has been set
        //      to true (by the RenderThread destructor), we return from the
        //      function immediately, terminating the thread.”
        //
        // Here, this is done by passing m_abortRun and m_restart (in form of
        // shouldAbort())to the render function, which is supposed to return
        // as fast as possible if indicated.
        m_renderFunction(parameters, *this);

        // cppcheck-suppress identicalConditionAfterEarlyExit // false positive
        if (m_loopAbort) {
            return;
        }

        // From Qt’s examples:
        //     “Once we're done with all the iterations, we call
        //      QWaitCondition::wait() to put the thread to sleep, unless
        //      restart is true. There's no use in keeping a worker thread
        //      looping indefinitely while there's nothing to do.”
        m_loopMutex.lock();
        if (!m_loopRestart && !m_loopAbort) {
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
            QMutexLocker<QMutex> syncLocker(&m_syncMutex);
#else
            QMutexLocker syncLocker(&m_syncMutex);
#endif
            m_syncIsIdle = true;
            m_syncCondition.wakeOne();
        }
        while (!m_loopRestart && !m_loopAbort) {
            // QWaitCondition::wait() does the following things:
            // 1.) Unlock mutex.
            // 2.) Wait until another thread calls QWaitCondition::wakeOne()
            //     or QWaitCondition::wakeAll().
            // 3.) Lock mutex again.
            //
            // As explained on the StackOverflow webpage at
            // https://stackoverflow.com/questions/40445629
            // using QWaitCondition::wait() alone can do spurious
            // wake-up (wake-up without a reason). To prevent the
            // rendering from continuing in this case, we put the
            // QWaitCondition::wait() call into a while loop. This way
            // we can go back to sleep if the wake-up was without
            // reason (this is checked by the condition in the while loop).
            m_loopCondition.wait(&m_loopMutex);
        }
        m_loopRestart = false;
        m_loopMutex.unlock();
    }
}

/** @brief Deliver the result of an interlacing pass of
 * the <em>rendering</em> operation.
 *
 * This function is thread-safe.
 *
 * @param image The image
 * @param mask The alpha mask, if provided. Renderers may choose
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
 * @param parameters The parameters of the image
 * @param state The interlacing state of the image. A render function
 * must first return zero or more images with intermediate state. After
 * that, it must return exactly one image with final state (unless it
 * was aborted). After that, it must not return any more images. */
void AsyncImageRenderThread::deliverInterlacingPass(const QImage &image,
                                                    const QImage &mask,
                                                    const QVariant &parameters,
                                                    const AsyncImageRenderCallback::InterlacingState state)
{
    // interlacingPassCompleted() is documented as being possibly emitted
    // by different threads, so this call is thread-safe within the
    // restrictions mentioned in the documentation.
    Q_EMIT interlacingPassCompleted(image, mask, parameters, state);
}

/** @brief If the render function should abort.
 *
 * This function is thread-safe.
 *
 * @returns <tt>true</tt> if the render function should abort (and
 * return). <tt>false</tt> otherwise.
 *
 * @internal
 *
 * @sa @ref m_renderFunction */
bool AsyncImageRenderThread::shouldAbort() const
{
    // m_abortRun and m_restart are atomic, so this call is thread-safe.
    return (m_loopAbort || m_loopRestart);
}

/** @brief Wait until the render thread is idle. */
void AsyncImageRenderThread::waitForIdle()
{
    m_syncMutex.lock();
    while (!m_syncIsIdle) {
        // QWaitCondition::wait() does the following things:
        // 1.) Unlock mutex.
        // 2.) Wait until another thread calls QWaitCondition::wakeOne()
        //     or QWaitCondition::wakeAll().
        // 3.) Lock mutex again.
        //
        // As explained on the StackOverflow webpage at
        // https://stackoverflow.com/questions/40445629
        // using QWaitCondition::wait() alone can do spurious
        // wake-up (wake-up without a reason). To prevent the
        // rendering from continuing in this case, we put the
        // QWaitCondition::wait() call into a while loop. This way
        // we can go back to sleep if the wake-up was without
        // reason (this is checked by the condition in the while loop).
        m_syncCondition.wait(&m_syncMutex);
    }
    m_syncMutex.unlock();
}

} // namespace PerceptualColor
