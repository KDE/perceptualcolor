// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "gradientimageparameters.h"

#include "asyncimagerenderthread.h"
#include "colorenginefactory.h"
#include "genericcolor.h"
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
        exampleParameters.colorEngine = //
            PerceptualColor::createSrgbColorEngine();
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
        PerceptualColor::GenericColor firstColor;
        firstColor.third = 10;
        firstColor.first = 20;
        firstColor.second = 30;
        firstColor.fourth = 0.4;
        exampleParameters.setFirstColorCieLchD50A(firstColor);
        PerceptualColor::GenericColor secondColor;
        secondColor.third = 50;
        secondColor.first = 60;
        secondColor.second = 25;
        secondColor.fourth = 0.9;
        exampleParameters.setSecondColorCieLchD50A( //
            secondColor);
        exampleParameters.setDevicePixelRatioF( //
            devicePixelRatioF());
        //! [GradientImage HiDPI usage]
    }
};

namespace PerceptualColor
{
class ColorEngine;

class TestGradientImageParameters : public QObject
{
    Q_OBJECT

public:
    explicit TestGradientImageParameters(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::ColorEngine> m_colorEngine = createSrgbColorEngine();

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
        myGradient.colorEngine = m_colorEngine;
        GenericColor lchaTestValue;

        // Test values that are too high
        lchaTestValue.first = 500;
        lchaTestValue.second = 20;
        lchaTestValue.third = 361;
        lchaTestValue.fourth = 5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).first, 100);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).second, 20);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).third, 1);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).fourth, 1);

        // Test value that are too low
        lchaTestValue.first = -500;
        lchaTestValue.second = -20;
        lchaTestValue.third = -1;
        lchaTestValue.fourth = -5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).first, 0);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).second,
                 20 // Normalised to positive value (hue is changed by 180°)
        );
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).third,
                 179 // Changed by 180° because of the negative chroma value
        );
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).fourth, 0);

        // Test value that much too low
        lchaTestValue.first = 50;
        lchaTestValue.second = 20;
        lchaTestValue.third = -361;
        lchaTestValue.fourth = 0.5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).first, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).second, 20);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).third, 359);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).fourth, 0.5);

        // Test that hue is preserved also if chroma is zero
        lchaTestValue.first = 50;
        lchaTestValue.second = 0;
        lchaTestValue.third = 50;
        lchaTestValue.fourth = 0.5;
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).first, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).second, 0);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).third, 50);
        QCOMPARE(myGradient.completlyNormalizedAndBounded(lchaTestValue).fourth, 0.5);
    }
    void testUpdateSecondColor()
    {
        GradientImageParameters myGradient;
        myGradient.colorEngine = m_colorEngine;
        myGradient.m_firstColorCorrected = GenericColor{50, 0, 30, 0.5};
        myGradient.m_secondColorCorrectedAndAltered = GenericColor{50, 0, 40, 0.5};
        myGradient.updateSecondColor();
        qreal absoluteDifference = qAbs(myGradient.m_firstColorCorrected.third - myGradient.m_secondColorCorrectedAndAltered.third);
        QVERIFY2(absoluteDifference <= 180, "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered = GenericColor{50, 0, 240, 0.5};
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.third - myGradient.m_secondColorCorrectedAndAltered.third) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered = GenericColor{50, 0, 540, 0.5};
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.third - myGradient.m_secondColorCorrectedAndAltered.third) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
        myGradient.m_secondColorCorrectedAndAltered = GenericColor{50, 0, -240, 0.5};
        myGradient.updateSecondColor();
        QVERIFY2(qAbs(myGradient.m_firstColorCorrected.third - myGradient.m_secondColorCorrectedAndAltered.third) <= 180,
                 "Verify that the hue difference is 0° ≤ difference ≤ 180°.");
    }

    void testGetImage()
    {
        GradientImageParameters myGradient;
        myGradient.colorEngine = m_colorEngine;
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
        myGradient.colorEngine = m_colorEngine;
        myGradient.m_firstColorCorrected = GenericColor{50, 0, 30, 0.5};
        myGradient.m_secondColorCorrectedAndAltered = GenericColor{60, 10, 20, 0.4};
        GenericColor middleColor = myGradient.colorFromValue(0.5);
        QCOMPARE(middleColor.first, 55);
        QCOMPARE(middleColor.second, 5);
        QCOMPARE(middleColor.third, 25);
        QCOMPARE(middleColor.fourth, 0.45);
    }

    void testSetDevicelPixelRatioF()
    {
        GradientImageParameters myGradient;
        myGradient.colorEngine = m_colorEngine;
        myGradient.setGradientLength(20);
        myGradient.setGradientThickness(10);
        // Should not crash:
        myGradient.setDevicePixelRatioF(1.25);
        myGradient.setDevicePixelRatioF(1.5);
    }

    void testSetGradientLength()
    {
        GradientImageParameters myGradient;
        myGradient.colorEngine = m_colorEngine;
        // Should not crash:
        myGradient.setGradientLength(20);
    }

    void testSetGradientThickness()
    {
        GradientImageParameters myGradient;
        myGradient.colorEngine = m_colorEngine;
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
