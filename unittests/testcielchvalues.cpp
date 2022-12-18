// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "cielchvalues.h"

#include "perceptualcolor-0/lchdouble.h"
#include "rgbcolorspace.h"
#include <QtCore/qsharedpointer.h>
#include <math.h>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>

namespace PerceptualColor
{
class TestCielchValues : public QObject
{
    Q_OBJECT

public:
    TestCielchValues(QObject *parent = nullptr)
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

    void testNeutralValues()
    {
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::CielchValues::neutralChroma), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::CielchValues::neutralHue), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::CielchValues::neutralLightness), 50);
    }

    void testCielchValues()
    {
        auto temp = RgbColorSpace::createSrgb();
        LchDouble color;
        qreal presicion = 0.1;
        const auto srgbMaximumChroma = temp->profileMaximumCielchChroma();
        color.c = srgbMaximumChroma;
        qreal hue = 0;
        color.c = srgbMaximumChroma - 1;
        bool inGamutValueFound = false;

        // Test if versatile is small enough
        qreal precisionVersatileSrgbChroma = //
            presicion / 360 * 2 * M_PI * PerceptualColor::CielchValues::srgbVersatileChroma;
        color.c = PerceptualColor::CielchValues::srgbVersatileChroma;
        color.l = 50;
        hue = 0;
        while (hue <= 360) {
            color.h = hue;
            QVERIFY2(temp->isInGamut(color), "Test if versatile is small enough");
            hue += precisionVersatileSrgbChroma;
        }

        // Test if versatile is as big as possible
        color.c = PerceptualColor::CielchValues::srgbVersatileChroma + 1;
        color.l = 50;
        inGamutValueFound = true;
        hue = 0;
        while (hue <= 360) {
            color.h = hue;
            if (!temp->isInGamut(color)) {
                inGamutValueFound = false;
                break;
            }
            hue += precisionVersatileSrgbChroma;
        }
        QVERIFY2(!inGamutValueFound, "Test if versatile is as big as possible");
    }

    void testNeutralGray()
    {
        // Test that the unified initialization is done in the correct order.
        QCOMPARE(CielchValues::neutralGray.l,
                 50 // Should be half the way between light and dark
        );
        QCOMPARE(CielchValues::neutralGray.c,
                 0 // Should have no chroma
        );
        QCOMPARE(CielchValues::neutralGray.h,
                 0 // Hue does not matter, but by convention should be 0
        );
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestCielchValues)
// The following “include” is necessary because we do not use a header file:
#include "testcielchvalues.moc"
