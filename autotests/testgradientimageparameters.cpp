// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "gradientimageparameters.h"

#include "asyncimagerenderthread.h"
#include "lchadouble.h"
#include "rgbcolorspacefactory.h"
#include <qglobal.h>
#include <qobject.h>
#include <qscopedpointer.h>
#include <qsharedpointer.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvariant.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

class TestGradientSnippetClass : public QWidget
{
    Q_OBJECT
public:
    // A constructor that is clazy-conform
    explicit TestGradientSnippetClass(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }
    void testSnippet01()
    {
        //! [GradientImage HiDPI usage]
        PerceptualColor::GradientImageParameters exampleParameters;
        exampleParameters.rgbColorSpace = //
            PerceptualColor::RgbColorSpaceFactory::createSrgb();
        // These functions expects an int
        // value. static_cast<int> will round
        // down, which is the desired behaviour
        // here. (Rounding up would mean one
        // more physical pixel, and on some Qt
        // styles this would fail.)
        exampleParameters.setGradientLength( //
            static_cast<int>(100 * devicePixelRatioF()));
        exampleParameters.setGradientThickness( //
            static_cast<int>(100 * devicePixelRatioF()));
        PerceptualColor::LchaDouble firstColor;
        firstColor.h = 10;
        firstColor.l = 20;
        firstColor.c = 30;
        firstColor.a = 0.4;
        exampleParameters.setFirstColor(firstColor);
        PerceptualColor::LchaDouble secondColor;
        secondColor.h = 50;
        secondColor.l = 60;
        secondColor.c = 25;
        secondColor.a = 0.9;
        exampleParameters.setSecondColor( //
            secondColor);
        exampleParameters.setDevicePixelRatioF( //
            devicePixelRatioF());
        //! [GradientImage HiDPI usage]
    }
};

namespace PerceptualColor
{
class RgbColorSpace;

class TestGradientImageParameters : public QObject
{
    Q_OBJECT

public:
    explicit TestGradientImageParameters(QObject *parent = nullptr)
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
        // Constructor and destructor do not crash.
        GradientImageParameters myGradient;
    }

    void testCompletlyNormalizedAndBounded()
    {
        GradientImageParameters myGradient;
        myGradient.rgbColorSpace = m_rgbColorSpace;
        LchaDouble lchaTestValue;

        // Test values that are too high
        lchaTestValue.l = 500;
        lchaTestValue.c = 20;
        lchaTestValue.h = 361;
        lchaTestValue.a = 5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l, 100);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c, 20);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h, 1);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a, 1);

        // Test value that are too low
        lchaTestValue.l = -500;
        lchaTestValue.c = -20;
        lchaTestValue.h = -1;
        lchaTestValue.a = -5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l, 0);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c,
                 20 // Normalised to positive value (hue is changed by 180°)
        );
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h,
                 179 // Changed by 180° because of the negative chroma value
        );
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a, 0);

        // Test value that much too low
        lchaTestValue.l = 50;
        lchaTestValue.c = 20;
        lchaTestValue.h = -361;
        lchaTestValue.a = 0.5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c, 20);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h, 359);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a, 0.5);

        // Test that hue is preserved also if chroma is zero
        lchaTestValue.l = 50;
        lchaTestValue.c = 0;
        lchaTestValue.h = 50;
        lchaTestValue.a = 0.5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).l, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).c, 0);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).h, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).a, 0.5);
    }
    void testUpdateSecondColor()
    {
        GradientImageParameters myGradient;
        myGradient.rgbColorSpace = m_rgbColorSpace;
        myGradient.m_firstColorCorrected = LchaDouble{50, 0, 30, 0.5};
        myGradient.m_secondColorCorrectedAndAltered = LchaDouble{50, 0, 40, 0.5};
        myGradient.updateSecondColor();
        qreal absoluteDifference = qAbs(myGradient.m_firstColorCorrected.h - myGradient.m_secondColorCorrectedAndAltered.h);
        QVERIFY2(absoluteDifference <= 180, "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered = LchaDouble{50, 0, 240, 0.5};
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.h - myGradient.m_secondColorCorrectedAndAltered.h) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered = LchaDouble{50, 0, 540, 0.5};
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.h - myGradient.m_secondColorCorrectedAndAltered.h) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered = LchaDouble{50, 0, -240, 0.5};
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.h - myGradient.m_secondColorCorrectedAndAltered.h) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
    }

    void testGetImage()
    {
        GradientImageParameters myGradient;
        myGradient.rgbColorSpace = m_rgbColorSpace;
        QScopedPointer<AsyncImageRenderThread> callbackObject{
            //
            new AsyncImageRenderThread(GradientImageParameters::render) //
        };
        // Should not crash also when values are not initialized.
        myGradient.render(QVariant::fromValue(myGradient), //
                          *callbackObject);
    }

    void testColorFromValue()
    {
        GradientImageParameters myGradient;
        myGradient.rgbColorSpace = m_rgbColorSpace;
        myGradient.m_firstColorCorrected = LchaDouble{50, 0, 30, 0.5};
        myGradient.m_secondColorCorrectedAndAltered = LchaDouble{60, 10, 20, 0.4};
        LchaDouble middleColor = myGradient.colorFromValue(0.5);
        QCOMPARE(middleColor.l, 55);
        QCOMPARE(middleColor.c, 5);
        QCOMPARE(middleColor.h, 25);
        QCOMPARE(middleColor.a, 0.45);
    }

    void testSetDevicelPixelRatioF()
    {
        GradientImageParameters myGradient;
        myGradient.rgbColorSpace = m_rgbColorSpace;
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        // Should not crash:
        myGradient.setDevicePixelRatioF(1.25);
        myGradient.setDevicePixelRatioF(1.5);
    }

    void testSetGradientLength()
    {
        GradientImageParameters myGradient;
        myGradient.rgbColorSpace = m_rgbColorSpace;
        // Should not crash:
        myGradient.setGradientLength(20);
    }

    void testSetGradientThickness()
    {
        GradientImageParameters myGradient;
        myGradient.rgbColorSpace = m_rgbColorSpace;
        // Should not crash:
        myGradient.setGradientThickness(10);
    }

    void testSnippet01()
    {
        TestGradientSnippetClass mySnippets;
        mySnippets.testSnippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestGradientImageParameters)

// The following “include” is necessary because we do not use a header file:
#include "testgradientimageparameters.moc"
