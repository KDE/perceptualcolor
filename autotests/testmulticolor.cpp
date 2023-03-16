// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "multicolor.h"

#include "lchdouble.h"
#include "multirgb.h"
#include "rgbcolorspacefactory.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscopedpointer.h>
#include <qtest.h>
#include <qtestcase.h>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestMultiColor : public QObject
{
    Q_OBJECT

public:
    explicit TestMultiColor(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    template<typename T>
    bool isAlmostEqual(T first, T second, T tolerance = 3)
    {
        return qAbs<T>(first - second) < tolerance;
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

    void testConstructorDestructorStatic()
    {
        MultiColor myMulticolor;
    }

    void testConstructorDestructorDynamic()
    {
        QScopedPointer<MultiColor> myPointer{new MultiColor()};
    }

    void testDefaultConstructor()
    {
        MultiColor myMulticolor;
        QCOMPARE(myMulticolor.ciehlc.size(), 0);
        QCOMPARE(myMulticolor.multiRgb.hwb.size(), 0);
        QCOMPARE(myMulticolor.multiRgb.hsl.size(), 0);
        QCOMPARE(myMulticolor.multiRgb.hsv.size(), 0);
        QCOMPARE(myMulticolor.oklch.size(), 0);
        QCOMPARE(myMulticolor.multiRgb.rgb.size(), 0);
        QCOMPARE(myMulticolor.ciehlc.size(), 0);
        QCOMPARE(myMulticolor.ciehlc.size(), 0);
        QCOMPARE(myMulticolor.multiRgb.rgbQColor.isValid(), false);
    }

    void testCopyConstructorUninitialized()
    {
        MultiColor myMulticolor1;
        MultiColor myMulticolor2(myMulticolor1);
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testCopyConstructor()
    {
        MultiColor myMulticolor1 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(),
            MultiRgb::fromRgb(QList<double>{1, 2, 3}));
        MultiColor myMulticolor2(myMulticolor1);
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testCopyAssignmentUninitialized()
    {
        MultiColor myMulticolor1;
        MultiColor myMulticolor2;
        myMulticolor2 = myMulticolor1;
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testCopyAssignment()
    {
        MultiColor myMulticolor1 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(),
            MultiRgb::fromRgb(QList<double>{4, 5, 6}));
        MultiColor myMulticolor2 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(),
            MultiRgb::fromRgb(QList<double>{7, 8, 9}));
        Q_UNUSED(myMulticolor2);
        myMulticolor2 = myMulticolor1;
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testMoveConstructorUninitialized()
    {
        MultiColor myReference;
        MultiColor myMulticolor1 = myReference;
        MultiColor myMulticolor2(std::move(myMulticolor1));
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testMoveConstructor()
    {
        MultiColor myReference = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(),
            MultiRgb::fromRgbQColor(Qt::yellow));
        MultiColor myMulticolor1 = myReference;
        MultiColor myMulticolor2(std::move(myMulticolor1));
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testMoveAssignmentUninitialized()
    {
        MultiColor myReference;
        MultiColor myMulticolor1 = myReference;
        MultiColor myMulticolor2;
        myMulticolor2 = std::move(myMulticolor1);
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testMoveAssignment()
    {
        MultiColor myReference = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(),
            MultiRgb::fromRgbQColor(Qt::red));
        MultiColor myMulticolor1 = myReference;
        MultiColor myMulticolor2;
        myMulticolor2 = std::move(myMulticolor1);
        QCOMPARE(myMulticolor2.ciehlc, myMulticolor1.ciehlc);
        QCOMPARE(myMulticolor2.cielch.l, myMulticolor1.cielch.l);
        QCOMPARE(myMulticolor2.cielch.c, myMulticolor1.cielch.c);
        QCOMPARE(myMulticolor2.cielch.h, myMulticolor1.cielch.h);
        QCOMPARE(myMulticolor2.multiRgb.hsl, myMulticolor1.multiRgb.hsl);
        QCOMPARE(myMulticolor2.multiRgb.hsv, myMulticolor1.multiRgb.hsv);
        QCOMPARE(myMulticolor2.multiRgb.hwb, myMulticolor1.multiRgb.hwb);
        QCOMPARE(myMulticolor2.oklch, myMulticolor1.oklch);
        QCOMPARE(myMulticolor2.multiRgb.rgb, myMulticolor1.multiRgb.rgb);
        QCOMPARE(myMulticolor2.multiRgb.rgbQColor, myMulticolor1.multiRgb.rgbQColor);
    }

    void testLch()
    {
        LchDouble myLch;
        myLch.l = 51;
        myLch.c = 21;
        myLch.h = 1;
        MultiColor myMulticolor1 = MultiColor::fromLch( //
            RgbColorSpaceFactory::createSrgb(),
            myLch);
        QCOMPARE(myMulticolor1.cielch.l, 51);
        QCOMPARE(myMulticolor1.cielch.c, 21);
        QCOMPARE(myMulticolor1.cielch.h, 1);
        QCOMPARE(myMulticolor1.ciehlc, (QList<double>{1, 51, 21}));
    }

    void testRgb()
    {
        MultiColor myMulticolor1 = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(),
                MultiRgb::fromRgbQColor(Qt::yellow));
        QCOMPARE(myMulticolor1.multiRgb.rgbQColor, Qt::yellow);
    }

    void testRgbHue()
    {
        // The hue of the RGB-based HSV, HSL and HBW is identical.
        MultiColor value;

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 40, 30})));
        QCOMPARE(value.multiRgb.hsl.at(0), 150);
        QCOMPARE(value.multiRgb.hsv.at(0), 150);

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({150, 40, 30})));
        QCOMPARE(value.multiRgb.hsl.at(0), 150);
        QCOMPARE(value.multiRgb.hsv.at(0), 150);
    }

    void testRgbHueOnGrayAxis()
    {
        // The hue of the RGB-based HSV, HSL and HBW is identical,
        // even when the value is on the gray axis.
        MultiColor value;

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 0, 50})));
        QCOMPARE(value.multiRgb.hsl.at(0), 150);
        QCOMPARE(value.multiRgb.hsv.at(0), 150);
        QCOMPARE(value.multiRgb.hwb.at(0), 150);

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({150, 0, 50})));
        QCOMPARE(value.multiRgb.hsl.at(0), 150);
        QCOMPARE(value.multiRgb.hsv.at(0), 150);
        QCOMPARE(value.multiRgb.hwb.at(0), 150);

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHwb(QList<double>({150, 50, 50})));
        // Sum of w and b is 100.
        QCOMPARE(value.multiRgb.hsl.at(0), 150);
        QCOMPARE(value.multiRgb.hsv.at(0), 150);
        QCOMPARE(value.multiRgb.hwb.at(0), 150);

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHwb(QList<double>({150, 70, 70})));
        // Sum of w and b is more than 100.
        QCOMPARE(value.multiRgb.hsl.at(0), 150);
        QCOMPARE(value.multiRgb.hsv.at(0), 150);
        QCOMPARE(value.multiRgb.hwb.at(0), 150);

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromRgb(QList<double>({120, 120, 120})));
        // An RGB value on the gray axis does not provide any information
        // about the hue. We can reasonably expect a standard value: 0°.
        QCOMPARE(value.multiRgb.hsl.at(0), 0);
        QCOMPARE(value.multiRgb.hsv.at(0), 0);
        QCOMPARE(value.multiRgb.hwb.at(0), 0);
    }

    void testHueFromLchToRgb()
    {
        // Hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values.

        MultiColor grayColor1 = MultiColor::fromLch( //
            RgbColorSpaceFactory::createSrgb(), //
            LchDouble{50, 0, 150});
        MultiColor slightlyColorfullColor1 = MultiColor::fromLch( //
            RgbColorSpaceFactory::createSrgb(), //
            LchDouble{50, 10, 150});
        QVERIFY( //
            isAlmostEqual(grayColor1.multiRgb.hsl.at(0), //
                          slightlyColorfullColor1.multiRgb.hsl.at(0)));

        MultiColor grayColor2 = MultiColor::fromLch( //
            RgbColorSpaceFactory::createSrgb(), //
            LchDouble{50, 0, 350});
        MultiColor slightlyColorfullColor2 = MultiColor::fromLch( //
            RgbColorSpaceFactory::createSrgb(), //
            LchDouble{50, 10, 350});
        QVERIFY( //
            isAlmostEqual(grayColor2.multiRgb.hsl.at(0), //
                          slightlyColorfullColor2.multiRgb.hsl.at(0)));

        QVERIFY(!isAlmostEqual(grayColor1.multiRgb.hsl.at(0), //
                               grayColor2.multiRgb.hsl.at(0)));
    }

    void testHueFromRgbToLch()
    {
        // Hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values. The synchronization of RGB-based hue values on the
        // gray axis is covered by a different test. This test covers the
        // conversion between RGB-hue and LCH-hue.

        MultiColor grayColor1 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 0, 50})));
        MultiColor slightlyColorfullColor1 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 10, 50})));
        QVERIFY( //
            isAlmostEqual(grayColor1.ciehlc.at(0), //
                          slightlyColorfullColor1.ciehlc.at(0)));

        MultiColor grayColor2 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({350, 0, 50})));
        MultiColor slightlyColorfullColor2 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({350, 10, 50})));
        QVERIFY( //
            isAlmostEqual(grayColor2.ciehlc.at(0), //
                          slightlyColorfullColor2.ciehlc.at(0)));

        QVERIFY(!isAlmostEqual(grayColor1.ciehlc.at(0), //
                               grayColor2.ciehlc.at(0)));
    }

    void testHueFromRgbToLchContinuity()
    {
        // LCH-hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values. For different HSL-lightness while keeping a
        // HSL-saturation of 0% and a constant HSL-hue, the LCH-hue should
        // not change very much.

        // Testing this explicitly for black and white as the edge cases,
        // which did show buggy behaviour in early stage of the code.

        const double referenceLchCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({186.35, 0, 50})) //
                )
                .ciehlc.at(0);

        const double blackHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({186.35, 0, 0})) //
                )
                .ciehlc.at(0);
        QVERIFY(isAlmostEqual(blackHue, referenceLchCieHue, 1.));

        const double whiteHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({186.35, 0, 100})) //
                )
                .ciehlc.at(0);
        QVERIFY(isAlmostEqual(whiteHue, referenceLchCieHue, 1.));
    }

    void testHueFromLchToRgbContinuity()
    {
        // LCH-hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values. For different HSL-lightness while keeping a
        // HSL-saturation of 0% and a constant HSL-hue, the LCH-hue should
        // not change very much.

        // Testing this explicitly for black and white as the edge cases,
        // which did show buggy behaviour in early stage of the code.

        const double referenceRgbHue = //
            MultiColor::fromLch( //
                RgbColorSpaceFactory::createSrgb(), //
                LchDouble{50, 0, 186.35} //
                )
                .multiRgb.hsl.at(0);

        const double blackHue = //
            MultiColor::fromLch( //
                RgbColorSpaceFactory::createSrgb(), //
                LchDouble{0, 0, 186.35} //
                )
                .multiRgb.hsl.at(0);
        QVERIFY(isAlmostEqual(blackHue, referenceRgbHue, 1.));

        const double whiteHue = //
            MultiColor::fromLch( //
                RgbColorSpaceFactory::createSrgb(), //
                LchDouble{100, 0, 186.35} //
                )
                .multiRgb.hsl.at(0);
        QVERIFY(isAlmostEqual(whiteHue, referenceRgbHue, 1.));
    }

    void testHueFromRgbToLchSaturationContinuityWhite()
    {
        // LCH-hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values.

        const double referenceLchCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 50, 100})) //
                )
                .ciehlc.at(0);

        const double lowestSaturationCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 0, 100})) //
                )
                .ciehlc.at(0);
        QVERIFY(isAlmostEqual(lowestSaturationCieHue, referenceLchCieHue, 1.));

        const double highestSaturationCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 100, 100})) //
                )
                .ciehlc.at(0);
        QVERIFY(isAlmostEqual(highestSaturationCieHue, referenceLchCieHue, 1.));
    }

    void testHueFromRgbToLchSaturationContinuityBlack()
    {
        // LCH-hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values.

        const double referenceLchCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 50, 0})) //
                )
                .ciehlc.at(0);

        const double lowestHslSaturationCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 0, 0})) //
                )
                .ciehlc.at(0);
        QVERIFY( //
            isAlmostEqual(lowestHslSaturationCieHue, referenceLchCieHue, 1.));

        const double highestHslSaturationCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 100, 0})) //
                )
                .ciehlc.at(0);
        QVERIFY( //
            isAlmostEqual(highestHslSaturationCieHue, referenceLchCieHue, 1.));

        const double lowestHsvSaturationCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 0, 0})) //
                )
                .ciehlc.at(0);
        QVERIFY( //
            isAlmostEqual(lowestHsvSaturationCieHue, referenceLchCieHue, 1.));

        const double middleHsvSaturationCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 50, 0})) //
                )
                .ciehlc.at(0);
        QVERIFY( //
            isAlmostEqual(middleHsvSaturationCieHue, referenceLchCieHue, 1.));

        const double highestHsvSaturationCieHue = //
            MultiColor::fromMultiRgb( //
                RgbColorSpaceFactory::createSrgb(), //
                MultiRgb::fromHsl(QList<double>({120, 100, 0})) //
                )
                .ciehlc.at(0);
        QVERIFY( //
            isAlmostEqual(highestHsvSaturationCieHue, referenceLchCieHue, 1.));
    }

    void testFromHsl()
    {
        const MultiColor value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({100, 60, 30})));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 60));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(2), 30));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 75));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(2), 48));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(1), 12));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(2), 52));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(0), 61));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(1), 122));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(2), 31));

        QVERIFY(isAlmostEqual<double>(value.cielch.l, 46));
        QVERIFY(isAlmostEqual<double>(value.cielch.c, 55));
        QVERIFY(isAlmostEqual<double>(value.cielch.h, 132));

        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(0), 132));
        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(1), 46));
        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(2), 55));
    }

    void testFromHsv()
    {
        const MultiColor value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({100, 60, 30})));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 43));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(2), 21));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 60));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(2), 30));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(1), 12));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(2), 70));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(0), 45));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(1), 76));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(2), 30));

        QVERIFY(isAlmostEqual<double>(value.cielch.l, 29));
        QVERIFY(isAlmostEqual<double>(value.cielch.c, 32));
        QVERIFY(isAlmostEqual<double>(value.cielch.h, 132));

        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(0), 132));
        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(1), 29));
        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(2), 32));
    }

    void testSaturationSynchronizationForBlackFromHsv()
    {
        // HSV-saturation and HSL-saturation are different. However, when
        // the color is black, changing any of these two saturation types
        // does not modify the color. But near to the blackpoint,
        // HSV-saturation and HSL-saturation behave very similar,
        // while they become more different the larger we get away
        // from the blackpoint. Therefore, it seems somewhat logical
        // that both are synchronized if (and only if) the color is black.

        MultiColor value;

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({150, 100, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 100));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({150, 60, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 60));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({150, 30, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 30));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({150, 0, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 0));
    }

    void testSaturationSynchronizationForBlackFromHsl()
    {
        // HSV-saturation and HSL-saturation are different. However, when
        // the color is black, changing any of these two saturation types
        // does not modify the color. But near to the blackpoint,
        // HSV-saturation and HSL-saturation behave very similar,
        // while they become more different the larger we get away
        // from the blackpoint. Therefore, it seems somewhat logical
        // that both are synchronized if (and only if) the color is black.

        MultiColor value;

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 100, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 100));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 60, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 60));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 30, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 30));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({150, 0, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 0));
    }

    void testSaturationSynchronizationForBlackFromOther()
    {
        // HSV-saturation and HSL-saturation are different. However, when
        // the color is black, changing any of these two saturation types
        // does not modify the color. When converting from a color format
        // different from HSV and HSL, there is no information about the
        // saturation, so the saturation could be anything within the valid
        // range. But for usability, it is better that for all conversions
        // of black, we get always the same saturation value. Given that
        // for white, we want always 0% for different reasons, and that all
        // the gray axis between black and white has also a saturation of 0%
        // for both saturation types, it seems natural to use also 0% for
        // black, so that the whole gray axis has a uniform saturation
        // value.

        MultiColor value;

        constexpr int saturationOfBlackColor = 0;

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromRgb(QList<double>({0, 0, 0})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), //
                                      saturationOfBlackColor));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), //
                                      saturationOfBlackColor));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHwb(QList<double>({320, 0, 100})));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), //
                                      saturationOfBlackColor));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), //
                                      saturationOfBlackColor));
    }

    void testHslSaturationForWhite()
    {
        // For white, the HSV-saturation is necessarily 0%, while the
        // HSL-saturation might have any valid values (0%–100%). It is better
        // for usability to get in any situation always the same value for
        // HSL-saturation for white color. It seems natural to choose 0% as
        // a standard value, because this is synchronized with HSV-saturation,
        // and especially because the whole gray axis between black and white
        // has 0% as saturation anyway, so it is nice to have a uniform
        // saturation value for the whole gray axis (including black and
        // white).

        MultiColor value;

        constexpr int saturationOfWhiteColor = 0;

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsl(QList<double>({320, 50, 100})));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 50));

        // All other original color formats should give the standard
        // HSL-saturation for white:

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromRgb(QList<double>({255, 255, 255})));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), //
                                      saturationOfWhiteColor));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHsv(QList<double>({320, 0, 100})));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), //
                                      saturationOfWhiteColor));

        value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHwb(QList<double>({320, 100, 0})));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), //
                                      saturationOfWhiteColor));
    }

    void testFromHwb()
    {
        const MultiColor value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHwb(QList<double>({100, 60, 30})));

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 14));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(2), 65)); //

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 15));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(2), 70)); //

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(1), 60));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(2), 30)); //

        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(0), 162));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(1), 179));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(2), 153)); //

        QVERIFY(isAlmostEqual<double>(value.cielch.l, 71));
        QVERIFY(isAlmostEqual<double>(value.cielch.c, 16));
        QVERIFY(isAlmostEqual<double>(value.cielch.h, 134));

        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(0), 134));
        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(1), 71));
        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(2), 16));
    }

    void testFromHwbDenormalized()
    {
        const MultiColor value = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(), //
            MultiRgb::fromHwb(QList<double>({100, 70, 70})));
        // The sum of w and b is greater than 100. This is denormalized.

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(1), 0));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsl.at(2), 50)); //

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(1), 0));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hsv.at(2), 50)); //

        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(1), 70));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.hwb.at(2), 70)); //

        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(0), 128));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(1), 128));
        QVERIFY(isAlmostEqual<double>(value.multiRgb.rgb.at(2), 128)); //

        // Test LCH and HLC, but without the hue. This is because the hue
        // requires special treatment which is part of another unit test,
        // and we do not want to test this unit test two unrelated
        // behaviours at the same time.
        QVERIFY(isAlmostEqual<double>(value.cielch.l, 54));
        QVERIFY(isAlmostEqual<double>(value.cielch.c, 0));

        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(1), 54));
        QVERIFY(isAlmostEqual<double>(value.ciehlc.at(2), 0));
    }

    void testEquality()
    {
        MultiColor myMulticolor1 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(),
            MultiRgb::fromRgb(QList<double>{1, 2, 3}));
        MultiColor myMulticolor2 = MultiColor::fromMultiRgb( //
            RgbColorSpaceFactory::createSrgb(),
            MultiRgb::fromRgb(QList<double>{1, 2, 3}));
        QVERIFY(myMulticolor1 == myMulticolor2);
        myMulticolor2.oklch[0] += 1;
        QVERIFY(!(myMulticolor1 == myMulticolor2));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestMultiColor)

// The following “include” is necessary because we do not use a header file:
#include "testmulticolor.moc"
