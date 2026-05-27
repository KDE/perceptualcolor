// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "colorwheelimageparameters.h"

#include <qcolor.h>
#include <qglobal.h>
#include <qimage.h>
#include <qobject.h>
#include <qsize.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>
#include <qwidget.h>

class TestColorWheelSnippetClass : public QWidget
{
    Q_OBJECT
public:
    // A constructor that is clazy-conform
    explicit TestColorWheelSnippetClass(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }
};

namespace PerceptualColor
{

class Mockup : public AsyncImageRenderCallback
{
public:
    virtual bool shouldAbort() const override;
    virtual void deliverInterlacingPass(const QImage &image, const QImage &mask, const QVariant &parameters, const InterlacingState state) override;
    QImage lastDeliveredImage() const;
    QImage lastDeliveredMask() const;
    QVariant lastDeliveredParameters() const;

private:
    QImage m_lastDeliveredImage;
    QImage m_lastDeliveredMask;
    QVariant m_lastDeliveredParameters;
};

bool Mockup::shouldAbort() const
{
    return false;
}

void Mockup::deliverInterlacingPass(const QImage &image, const QImage &mask, const QVariant &parameters, const InterlacingState state)
{
    Q_UNUSED(state)
    m_lastDeliveredImage = image;
    m_lastDeliveredMask = mask;
    m_lastDeliveredParameters = parameters;
}

QImage Mockup::lastDeliveredImage() const
{
    return m_lastDeliveredImage;
}

QImage Mockup::lastDeliveredMask() const
{
    return m_lastDeliveredMask;
}

QVariant Mockup::lastDeliveredParameters() const
{
    return m_lastDeliveredParameters;
}

class TestColorWheelImageParameters : public QObject
{
    Q_OBJECT

public:
    explicit TestColorWheelImageParameters(QObject *parent = nullptr)
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

    void testConstructor()
    {
        ColorWheelImageParameters test;
        Q_UNUSED(test)
    }

    void testSize()
    {
        ColorWheelImageParameters test;
        Mockup callback;

        test.imageSizePhysical = 0;
        test.render(QVariant::fromValue(test), callback);
        QCOMPARE(callback.lastDeliveredImage().size(), QSize(0, 0));

        test.imageSizePhysical = 1;
        test.render(QVariant::fromValue(test), callback);
        QCOMPARE(callback.lastDeliveredImage().size(), QSize(1, 1));

        test.imageSizePhysical = -1;
        test.render(QVariant::fromValue(test), callback);
        QCOMPARE(callback.lastDeliveredImage().size(), QSize(0, 0));
    }

    void benchmarkRender()
    {
        ColorWheelImageParameters testProperties;
        Mockup myMockup;
        testProperties.imageSizePhysical = 1000; // an even number
        QBENCHMARK {
            testProperties.render(QVariant::fromValue(testProperties), //
                                  myMockup);
        }
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorWheelImageParameters)

// The following “include” is necessary because we do not use a header file:
#include "testcolorwheelimageparameters.moc"
