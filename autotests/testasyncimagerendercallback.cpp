// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "asyncimagerendercallback.h"

#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

class QImage;

namespace PerceptualColor
{
class Mockup : public AsyncImageRenderCallback
{
public:
    virtual bool shouldAbort() const override;
    virtual void deliverInterlacingPass(const QImage &image, const QVariant &parameters, const InterlacingState state) override;
};

bool Mockup::shouldAbort() const
{
    return false;
}

void Mockup::deliverInterlacingPass(const QImage &image, const QVariant &parameters, const InterlacingState state)
{
    Q_UNUSED(image)
    Q_UNUSED(parameters)
    Q_UNUSED(state)
}

class TestAsyncImageRenderCallback : public QObject
{
    Q_OBJECT

public:
    explicit TestAsyncImageRenderCallback(QObject *parent = nullptr)
        : QObject(parent)
    {
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
        Mockup test;
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestAsyncImageRenderCallback)

// The following “include” is necessary because we do not use a header file:
#include "testasyncimagerendercallback.moc"
