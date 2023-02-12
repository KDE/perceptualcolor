// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "asyncimagerenderthread.h"

#include "asyncimagerendercallback.h"
#include <qglobal.h>
#include <qimage.h>
#include <qobject.h>
#include <qtest.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestAsyncImageRenderThread : public QObject
{
    Q_OBJECT

public:
    explicit TestAsyncImageRenderThread(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

    static void renderEmptyImage(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject)
    {
        callbackObject.deliverInterlacingPass( //
            QImage(),
            variantParameters,
            AsyncImageRenderCallback::InterlacingState::Intermediate);
        if (!callbackObject.shouldAbort()) {
            callbackObject.deliverInterlacingPass( //
                QImage(),
                variantParameters,
                AsyncImageRenderCallback::InterlacingState::Intermediate);
        }
        return;
    }

private Q_SLOTS:
    void initTestCase()
    {
        // Called before the first test function is executed
    }

    void cleanupTestCase()
    {
        // Called after the last test function was executed
    }

    void init()
    {
        // Called before each test function is executed
    }

    void cleanup()
    {
        // Called after every test function
    }

    void testConstructorDestructor()
    {
        AsyncImageRenderThread test( //
            &TestAsyncImageRenderThread::renderEmptyImage);
    }

    void testStartRenderingAsync()
    {
        AsyncImageRenderThread test( //
            &TestAsyncImageRenderThread::renderEmptyImage);
        test.startRenderingAsync(QVariant());
    }

    void testShouldAbort()
    {
        AsyncImageRenderThread test( //
            &TestAsyncImageRenderThread::renderEmptyImage);
        Q_UNUSED(test.shouldAbort());
    }

    void testDeliver()
    {
        AsyncImageRenderThread test( //
            &TestAsyncImageRenderThread::renderEmptyImage);
        test.deliverInterlacingPass( //
            QImage(), //
            QVariant(), //
            AsyncImageRenderCallback::InterlacingState::Intermediate);
    }

    void testWaitForIdle()
    {
        AsyncImageRenderThread test( //
            &TestAsyncImageRenderThread::renderEmptyImage);
        test.waitForIdle();
    }

    void testWaitForIdleAfterRenderingTriggered()
    {
        AsyncImageRenderThread test( //
            &TestAsyncImageRenderThread::renderEmptyImage);
        test.startRenderingAsync(QVariant());
        test.waitForIdle();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestAsyncImageRenderThread)

// The following “include” is necessary because we do not use a header file:
#include "testasyncimagerenderthread.moc"
