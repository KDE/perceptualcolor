// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "cielchd50values.h"

#include "genericcolor.h"
#include "helperposixmath.h"
#include "rgbcolorspace.h"
#include <qglobal.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestCielchD50Values : public QObject
{
    Q_OBJECT

public:
    explicit TestCielchD50Values(QObject *parent = nullptr)
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
        QCOMPARE(static_cast<qreal>(PerceptualColor::CielchD50Values::neutralChroma), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::CielchD50Values::neutralHue), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::CielchD50Values::neutralLightness), 50);
    }

    void testCielchD50Values()
    {
        auto temp = RgbColorSpace::createSrgb();
        GenericColor color;
        qreal presicion = 0.1;
        qreal hue = 0;
        bool inGamutValueFound = false;

        // Test if versatile is small enough
        qreal precisionVersatileSrgbChroma = //
            presicion / 360 * 2 * pi * PerceptualColor::CielchD50Values::srgbVersatileChroma;
        color.second = PerceptualColor::CielchD50Values::srgbVersatileChroma;
        color.first = 50;
        hue = 0;
        while (hue <= 360) {
            color.third = hue;
            QVERIFY2(temp->isCielchD50InGamut(color), "Test if versatile is small enough");
            hue += precisionVersatileSrgbChroma;
        }

        // Test if versatile is as big as possible
        color.second = PerceptualColor::CielchD50Values::srgbVersatileChroma + 1;
        color.first = 50;
        inGamutValueFound = true;
        hue = 0;
        while (hue <= 360) {
            color.third = hue;
            if (!temp->isCielchD50InGamut(color)) {
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
        QCOMPARE(CielchD50Values::neutralGray.first,
                 50 // Should be half the way between light and dark
        );
        QCOMPARE(CielchD50Values::neutralGray.second,
                 0 // Should have no chroma
        );
        QCOMPARE(CielchD50Values::neutralGray.third,
                 0 // Hue does not matter, but by convention should be 0
        );
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestCielchD50Values)
// The following “include” is necessary because we do not use a header file:
#include "testcielchd50values.moc"
