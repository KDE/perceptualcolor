// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "chromalightnessimageparameters.h"

#include "asyncimageprovider.h"
#include "helper.h"
#include "rgbcolorspacefactory.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qimage.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>

namespace PerceptualColor
{
class RgbColorSpace;

class TestChromaLightnessImageParameters : public QObject
{
    Q_OBJECT

public:
    TestChromaLightnessImageParameters(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace = RgbColorSpaceFactory::createSrgb();

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
        ChromaLightnessImageParameters test;
    }

    void testGetImage0()
    {
        ChromaLightnessImageParameters m_imageParameters;
        m_imageParameters.rgbColorSpace = m_rgbColorSpace;
        m_imageParameters.hue = 0;

        // Testing extremely small images

        AsyncImageProvider<ChromaLightnessImageParameters> m_imageProvider;
        m_imageParameters.imageSizePhysical = QSize(0, 0);
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        delayedEventProcessing();
        const QImage m_image = m_imageProvider.getCache();
        QCOMPARE(m_image.size(), QSize(0, 0));
    }

    void testGetImage1()
    {
        ChromaLightnessImageParameters m_imageParameters;
        m_imageParameters.rgbColorSpace = m_rgbColorSpace;
        m_imageParameters.hue = 0;

        // Testing extremely small images

        AsyncImageProvider<ChromaLightnessImageParameters> m_imageProvider;
        m_imageParameters.imageSizePhysical = QSize(1, 1);
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        delayedEventProcessing();
        const QImage m_image = m_imageProvider.getCache();
        QCOMPARE(m_image.size(), QSize(1, 1));
    }

    void testGetImage2()
    {
        ChromaLightnessImageParameters m_imageParameters;
        m_imageParameters.rgbColorSpace = m_rgbColorSpace;
        m_imageParameters.hue = 0;

        // Testing extremely small images

        AsyncImageProvider<ChromaLightnessImageParameters> m_imageProvider;
        m_imageParameters.imageSizePhysical = QSize(2, 2);
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        delayedEventProcessing();
        const QImage m_image = m_imageProvider.getCache();
        QCOMPARE(m_image.size(), QSize(2, 2));
    }

    void testGetImageInvalid()
    {
        ChromaLightnessImageParameters m_imageParameters;
        m_imageParameters.rgbColorSpace = m_rgbColorSpace;
        m_imageParameters.hue = 0;

        // Testing extremely small images

        AsyncImageProvider<ChromaLightnessImageParameters> m_imageProvider;
        m_imageParameters.imageSizePhysical = QSize(-1, -1);
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        delayedEventProcessing();
        const QImage m_image = m_imageProvider.getCache();
        QCOMPARE(m_image.size(), QSize(0, 0));
    }

    void testGetImageNormalSize()
    {
        ChromaLightnessImageParameters m_imageParameters;
        m_imageParameters.rgbColorSpace = m_rgbColorSpace;
        m_imageParameters.hue = 0;

        AsyncImageProvider<ChromaLightnessImageParameters> m_imageProvider;

        // Start testing for a normal size image
        m_imageParameters.imageSizePhysical = QSize(201, 101);
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        delayedEventProcessing();
        const QImage m_image = m_imageProvider.getCache();
        QCOMPARE(m_image.height(), 101);
        QCOMPARE(m_image.width(), 201);

        // Test if position within the QImage is valid:
        QCOMPARE(m_image.pixelColor(0, 0).isValid(), true);

        // Test if position within the QImage is valid:
        QCOMPARE(m_image.pixelColor(0, 100).isValid(), true);

        // Test if position within the QImage is invalid:
        QTest::ignoreMessage( //
            QtWarningMsg, //
            "QImage::pixelColor: coordinate (0,101) out of range");
        QCOMPARE(m_image.pixelColor(0, 101).isValid(), false);
    }

    void testSetHue_data()
    {
        QTest::addColumn<qreal>("hue");
        QTest::newRow("0") << 0.;
        QTest::newRow("20") << 20.;
        QTest::newRow("30") << 30.;
        QTest::newRow("40") << 40.;
        QTest::newRow("50") << 50.;
        QTest::newRow("60") << 60.;
        QTest::newRow("70") << 70.;
        QTest::newRow("80") << 80.;
        QTest::newRow("290") << 290.;
    }

    void testSetHueInvalid()
    {
        // Make sure that setting an invalid values does not crash.
        ChromaLightnessImageParameters m_imageParameters;
        m_imageParameters.rgbColorSpace = m_rgbColorSpace;

        // Set a non-zero image size:
        m_imageParameters.imageSizePhysical = QSize(20, 10);

        AsyncImageProvider<ChromaLightnessImageParameters> m_imageProvider;

        m_imageParameters.hue = 0;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 1;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 2;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = -10;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = -1000;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 100;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 110;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 250;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 360;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 361;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())

        m_imageParameters.hue = 721;
        m_imageProvider.setImageParameters(m_imageParameters);
        m_imageProvider.refreshSync();
        Q_UNUSED(m_imageProvider.getCache())
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaLightnessImageParameters)

// The following “include” is necessary because we do not use a header file:
#include "testchromalightnessimageparameters.moc"
