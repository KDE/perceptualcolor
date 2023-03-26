// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "chromahueimageparameters.h"

#include "asyncimagerendercallback.h"
#include "helpermath.h"
#include "lchdouble.h"
#include "rgbcolorspacefactory.h"
#include <qbenchmark.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qimage.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>
#include <qvariant.h>
#include <rgbcolorspace.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class Mockup : public AsyncImageRenderCallback
{
public:
    virtual bool shouldAbort() const override;
    virtual void deliverInterlacingPass(const QImage &image, const QVariant &parameters, const InterlacingState state) override;
    QImage lastDeliveredImage() const;
    QVariant lastDeliveredParameters() const;

private:
    QImage m_lastDeliveredImage;
    QVariant m_lastDeliveredParameters;
};

bool Mockup::shouldAbort() const
{
    return false;
}

void Mockup::deliverInterlacingPass(const QImage &image, const QVariant &parameters, const InterlacingState state)
{
    Q_UNUSED(state)
    m_lastDeliveredImage = image;
    m_lastDeliveredParameters = parameters;
}

QImage Mockup::lastDeliveredImage() const
{
    return m_lastDeliveredImage;
}

QVariant Mockup::lastDeliveredParameters() const
{
    return m_lastDeliveredParameters;
}

class TestChromaHueImageParameters : public QObject
{
    Q_OBJECT

public:
    explicit TestChromaHueImageParameters(QObject *parent = nullptr)
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
        ChromaHueImageParameters test;
    }

    void testCopyConstructorAndEqualUnequal()
    {
        ChromaHueImageParameters test;
        test.borderPhysical = 1;
        test.devicePixelRatioF = 3;
        test.imageSizePhysical = 4;
        test.lightness = 5;
        test.rgbColorSpace = RgbColorSpaceFactory::createSrgb();

        auto copy = test;

        QCOMPARE(copy, test);
        QVERIFY(!(test != copy));
        QVERIFY(test == copy);

        copy.lightness = 30;

        QVERIFY(test != copy);
        QVERIFY(!(test == copy));
    }

    void testImageSizeNew()
    {
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;

        // Test especially small values, that might make special
        // problems in the algorithm (division by zero, offset by 1…)
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(0, 0));

        testProperties.imageSizePhysical = 1;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(1, 1));

        testProperties.imageSizePhysical = 2;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(2, 2));

        testProperties.imageSizePhysical = 3;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(3, 3));

        testProperties.imageSizePhysical = 4;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(4, 4));

        testProperties.imageSizePhysical = 5;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(5, 5));

        // Test a normal size value
        testProperties.imageSizePhysical = 500;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(500, 500));
    }

    void testDevicePixelRatioF()
    {
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        testProperties.imageSizePhysical = 100;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        // Image size is as described.
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(100, 100));
        // Default devicePixelRatioF is 1
        QCOMPARE(myMockup.lastDeliveredImage().devicePixelRatio(), 1);
        // Testing with a (non-integer) scale factor
        testProperties.devicePixelRatioF = 1.5;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        // Image size remains unchanged.
        QCOMPARE(myMockup.lastDeliveredImage().size(), QSize(100, 100));
        // Default devicePixelRatioF is 1.5
        QCOMPARE(myMockup.lastDeliveredImage().devicePixelRatio(), 1.5);
    }

    void testCornerCases()
    {
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        // Set a non-zero image size:
        testProperties.imageSizePhysical = 50;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.borderPhysical = -10;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.borderPhysical = 10;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.borderPhysical = 25;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.borderPhysical = 100;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.borderPhysical = 5;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.lightness = -10;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.lightness = 0;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.lightness = 50;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.lightness = 100;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
        testProperties.lightness = 150;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QVERIFY2(!myMockup.lastDeliveredImage().isNull(),
                 "Verify that there is no crash "
                 "and that the returned image is not null.");
    }

    void testVeryBigBorder()
    {
        Mockup myMockup;
        constexpr int myImageSize = 51;
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        // Set a non-zero image size:
        testProperties.imageSizePhysical = myImageSize;
        // Set a border that is bigger than half of the image size:
        testProperties.borderPhysical = myImageSize / 2 + 1;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        // The border is so big that the hole image should be transparent.
        for (int x = 0; x < myImageSize; ++x) {
            for (int y = 0; y < myImageSize; ++y) {
                QCOMPARE( //
                    myMockup.lastDeliveredImage().pixelColor(x, y).alpha(), //
                    0);
            }
        }
    }

    void testSetLightness_data()
    {
        QTest::addColumn<qreal>("lightness");
        QTest::newRow("10") << 10.;
        QTest::newRow("20") << 20.;
        QTest::newRow("30") << 30.;
        QTest::newRow("40") << 40.;
        QTest::newRow("50") << 50.;
        QTest::newRow("60") << 60.;
        QTest::newRow("70") << 70.;
        QTest::newRow("80") << 80.;
        QTest::newRow("90") << 90.;
    }

    void testSetLightness()
    {
        Mockup myMockup;
        QFETCH(qreal, lightness);
        constexpr int imageSize = 20;
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        // Set a non-zero image size:
        testProperties.imageSizePhysical = imageSize;
        testProperties.lightness = lightness;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        // Test the lightness. We are using QColor’s simple (non-color-managed)
        // lightness property. Therefore, we allow a tolerance up to 10%.
        const double gamutImageLightnessInPercent = //
            myMockup //
                .lastDeliveredImage() //
                .pixelColor(imageSize / 2, imageSize / 2) //
                .lightnessF() //
            * 100;
        constexpr double tolerance = 2;
        const bool lightnessIsCorrect = PerceptualColor::isInRange( //
            lightness - tolerance, //
            gamutImageLightnessInPercent, //
            lightness + tolerance);
        QVERIFY2(lightnessIsCorrect, //
                 "Verify that the correct lightness is applied. "
                 "(10% tolerance is allowed.)");
    }

    void testSetLightnessInvalid()
    {
        // Make sure that calling setLightness with invalid values
        // does not crash.
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        testProperties.imageSizePhysical = 20; // Set a non-zero image size
        testProperties.lightness = 0;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
        testProperties.lightness = 1;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
        testProperties.lightness = 2;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
        testProperties.lightness = -10;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
        testProperties.lightness = -1000;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
        testProperties.lightness = 100;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
        testProperties.lightness = 110;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
        testProperties.lightness = 250;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
    }

    void testSizeBorderCombinations()
    {
        // Make sure this code does not crash.
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        // Set a non-zero image size:
        testProperties.imageSizePhysical = 20;
        // Set exactly the half of image size as border:
        testProperties.borderPhysical = 10;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        Q_UNUSED(myMockup.lastDeliveredImage())
    }

    void testDevicePixelRatioFForExtremeCases()
    {
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        // Testing with a (non-integer) scale factor
        testProperties.devicePixelRatioF = 1.5;
        // Test with fully transparent image (here, the border is too big
        // for the given image size)
        testProperties.imageSizePhysical = 20;
        testProperties.borderPhysical = 30;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QCOMPARE(myMockup.lastDeliveredImage().devicePixelRatio(), 1.5);
    }

    void testIfGamutIsCenteredCorrectlyOnOddSize()
    {
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        testProperties.borderPhysical = 0;
        testProperties.lightness = 50;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        constexpr int oddSize = 101;
        testProperties.imageSizePhysical = oddSize; // an odd number
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        constexpr int positionAtCenter = (oddSize - 1) / 2;
        const qreal chromaAtCenter = //
            testProperties //
                .rgbColorSpace //
                ->toCielchD50Double( //
                    myMockup
                        .lastDeliveredImage() //
                        .pixelColor(positionAtCenter, positionAtCenter) //
                        .rgba64()) //
                .c;
        for (int x = positionAtCenter - 2; x <= positionAtCenter + 2; ++x) {
            for (int y = positionAtCenter - 2; y <= positionAtCenter + 2; ++y) {
                if ((x == positionAtCenter) && (y == positionAtCenter)) {
                    continue;
                }
                const qreal chromaAround = //
                    testProperties //
                        .rgbColorSpace //
                        ->toCielchD50Double(myMockup //
                                                .lastDeliveredImage() //
                                                .pixelColor(x, y) //
                                                .rgba64()) //
                        .c;
                QVERIFY2(chromaAtCenter < chromaAround,
                         "The chroma of the pixel at the center of the image "
                         "is lower than the chroma of any of the pixels "
                         "around.");
            }
        }
    }

    void testIfGamutIsCenteredCorrectlyOnEvenSize()
    {
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        testProperties.borderPhysical = 0;
        testProperties.lightness = 50;
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        constexpr int evenSize = 100;
        testProperties.imageSizePhysical = evenSize; // an even number
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        constexpr int positionAtCenter2 = evenSize / 2;
        constexpr int positionAtCenter1 = positionAtCenter2 - 1;
        const qreal chromaAtCenterA = //
            testProperties //
                .rgbColorSpace //
                ->toCielchD50Double( //
                    myMockup //
                        .lastDeliveredImage() //
                        .pixelColor(positionAtCenter1, positionAtCenter1) //
                        .rgba64())
                .c;
        const qreal chromaAtCenterB = //
            testProperties //
                .rgbColorSpace //
                ->toCielchD50Double( //
                    myMockup //
                        .lastDeliveredImage() //
                        .pixelColor(positionAtCenter1, positionAtCenter2) //
                        .rgba64())
                .c;
        const qreal chromaAtCenterC = //
            testProperties //
                .rgbColorSpace //
                ->toCielchD50Double( //
                    myMockup //
                        .lastDeliveredImage() //
                        .pixelColor(positionAtCenter2, positionAtCenter1) //
                        .rgba64())
                .c;
        const qreal chromaAtCenterD = //
            testProperties //
                .rgbColorSpace //
                ->toCielchD50Double( //
                    myMockup //
                        .lastDeliveredImage() //
                        .pixelColor(positionAtCenter2, positionAtCenter2) //
                        .rgba64())
                .c;
        const qreal maximumChromaAtCenter = qMax( //
            qMax(chromaAtCenterA, chromaAtCenterB), //
            qMax(chromaAtCenterC, chromaAtCenterD) //
        );
        for (int x = positionAtCenter1 - 2; x <= positionAtCenter2 + 2; ++x) {
            for (int y = positionAtCenter1 - 2; //
                 y <= positionAtCenter2 + 2; //
                 ++y //
            ) {
                if (isInRange(positionAtCenter1, x, positionAtCenter2) //
                    && isInRange(positionAtCenter1, y, positionAtCenter2)) {
                    continue;
                }
                const qreal chromaAround = //
                    testProperties //
                        .rgbColorSpace //
                        ->toCielchD50Double(myMockup //
                                                .lastDeliveredImage() //
                                                .pixelColor(x, y) //
                                                .rgba64()) //
                        .c;
                QVERIFY2(maximumChromaAtCenter < chromaAround,
                         "The chroma of the pixels at the center of the image "
                         "is lower than the chroma of any of the pixels "
                         "around.");
            }
        }
    }

    void benchmarkGetImage()
    {
        ChromaHueImageParameters testProperties;
        testProperties.rgbColorSpace = RgbColorSpaceFactory::createSrgb();
        Mockup myMockup;
        testProperties.borderPhysical = 0;
        testProperties.lightness = 50;
        testProperties.imageSizePhysical = 1000; // an even number
        testProperties.render(QVariant::fromValue(testProperties), myMockup);
        QBENCHMARK {
            testProperties.lightness = 51;
            testProperties.render(QVariant::fromValue(testProperties), //
                                  myMockup);
            testProperties.lightness = 50;
            testProperties.render(QVariant::fromValue(testProperties), //
                                  myMockup);
        }
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaHueImageParameters)

// The following “include” is necessary because we do not use a header file:
#include "testchromahueimageparameters.moc"
