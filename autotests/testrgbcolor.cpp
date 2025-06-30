// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "rgbcolor.h"

#include "genericcolor.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscopedpointer.h>
#include <qtest.h>
#include <qtestcase.h>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestRgbColor : public QObject
{
    Q_OBJECT

public:
    explicit TestRgbColor(QObject *parent = nullptr)
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
        RgbColor myColor;
    }

    void testConstructorDestructorDynamic()
    {
        QScopedPointer<RgbColor> myPointer{new RgbColor()};
    }

    void testDefaultConstructor()
    {
        RgbColor myColor;
        QCOMPARE(myColor.rgbQColor.isValid(), false);
    }

    void testCopyConstructorUninitialized()
    {
        RgbColor myColor1;
        RgbColor myColor2(myColor1);
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testCopyConstructor()
    {
        RgbColor myColor1 = RgbColor::fromRgb255(GenericColor{1, 2, 3});
        RgbColor myColor2(myColor1);
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testCopyAssignmentUninitialized()
    {
        RgbColor myColor1;
        RgbColor myColor2;
        myColor2 = myColor1;
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testCopyAssignment()
    {
        RgbColor myColor1 = RgbColor::fromRgb255(GenericColor{4, 5, 6});
        RgbColor myColor2 = RgbColor::fromRgb255(GenericColor{7, 8, 9});
        Q_UNUSED(myColor2);
        myColor2 = myColor1;
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testMoveConstructorUninitialized()
    {
        RgbColor myReference;
        RgbColor myColor1 = myReference;
        RgbColor myColor2(std::move(myColor1));
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testMoveConstructor()
    {
        RgbColor myReference = RgbColor::fromRgbQColor(Qt::yellow);
        RgbColor myColor1 = myReference;
        RgbColor myColor2(std::move(myColor1));
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testMoveAssignmentUninitialized()
    {
        RgbColor myReference;
        RgbColor myColor1 = myReference;
        RgbColor myColor2;
        myColor2 = std::move(myColor1);
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testMoveAssignment()
    {
        RgbColor myReference = RgbColor::fromRgbQColor(Qt::red);
        RgbColor myColor1 = myReference;
        RgbColor myColor2;
        myColor2 = std::move(myColor1);
        QCOMPARE(myColor2.hsl, myColor1.hsl);
        QCOMPARE(myColor2.hsv, myColor1.hsv);
        QCOMPARE(myColor2.hwb, myColor1.hwb);
        QCOMPARE(myColor2.rgb255, myColor1.rgb255);
        QCOMPARE(myColor2.rgbQColor, myColor1.rgbQColor);
    }

    void testFromRgb()
    {
        RgbColor myColor1 = RgbColor::fromRgb255(GenericColor{113, 53, 23});
        QCOMPARE(myColor1.rgbQColor, QColor::fromRgb(113, 53, 23));
    }

    void testFromRgbQColor()
    {
        RgbColor myColor1 = RgbColor::fromRgbQColor(Qt::yellow);
        QCOMPARE(myColor1.rgbQColor, Qt::yellow);
    }

    void testFromRgbQColorAlpha()
    {
        QColor semiTransparend = QColor(127, 127, 127, 127);

        // It's supposed that the alpha channel is ignored.
        const auto myValue = RgbColor::fromRgbQColor(semiTransparend);

        QCOMPARE(myValue.rgbQColor.alphaF(), 1); // color is opaque.
        QCOMPARE(myValue.hsl.fourth, 0); // the fourth value is unused.
        QCOMPARE(myValue.hsv.fourth, 0); // the fourth value is unused.
        QCOMPARE(myValue.hwb.fourth, 0); // the fourth value is unused.
        QCOMPARE(myValue.rgb255.fourth, 0); // the fourth value is unused.
    }

    void testRgbHue()
    {
        // The hue of the RGB-based HSV, HSL and HBW is identical.
        RgbColor value;

        value = RgbColor::fromHsl(GenericColor{150, 40, 30});
        QCOMPARE(value.hsl.first, 150);
        QCOMPARE(value.hsv.first, 150);

        value = RgbColor::fromHsv(GenericColor{150, 40, 30});
        QCOMPARE(value.hsl.first, 150);
        QCOMPARE(value.hsv.first, 150);
    }

    void testRgbHueOnGrayAxis()
    {
        // The hue of the RGB-based HSV, HSL and HBW is identical,
        // even when the value is on the gray axis.
        RgbColor value;

        value = RgbColor::fromHsl(GenericColor{150, 0, 50});
        QCOMPARE(value.hsl.first, 150);
        QCOMPARE(value.hsv.first, 150);
        QCOMPARE(value.hwb.first, 150);

        value = RgbColor::fromHsv(GenericColor{150, 0, 50});
        QCOMPARE(value.hsl.first, 150);
        QCOMPARE(value.hsv.first, 150);
        QCOMPARE(value.hwb.first, 150);

        value = RgbColor::fromHwb(GenericColor{150, 50, 50});
        // Sum of w and b is 100.
        QCOMPARE(value.hsl.first, 150);
        QCOMPARE(value.hsv.first, 150);
        QCOMPARE(value.hwb.first, 150);

        value = RgbColor::fromHwb(GenericColor{150, 70, 70});
        // Sum of w and b is more than 100.
        QCOMPARE(value.hsl.first, 150);
        QCOMPARE(value.hsv.first, 150);
        QCOMPARE(value.hwb.first, 150);

        value = RgbColor::fromRgb255(GenericColor{120, 120, 120});
        // An RGB value on the gray axis does not provide any information
        // about the hue. We can reasonably expect a standard value: 0°.
        QCOMPARE(value.hsl.first, 0);
        QCOMPARE(value.hsv.first, 0);
        QCOMPARE(value.hwb.first, 0);
    }

    void testHueFromRgbToLchSaturationContinuityWhite()
    {
        // LCH-hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values.
    }

    void testFromHsl()
    {
        const RgbColor value = RgbColor::fromHsl(GenericColor{100, 60, 30});

        QVERIFY(isAlmostEqual<double>(value.hsl.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 60));
        QVERIFY(isAlmostEqual<double>(value.hsl.third, 30));

        QVERIFY(isAlmostEqual<double>(value.hsv.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 75));
        QVERIFY(isAlmostEqual<double>(value.hsv.third, 48));

        QVERIFY(isAlmostEqual<double>(value.hwb.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.second, 12));
        QVERIFY(isAlmostEqual<double>(value.hwb.third, 52));

        QVERIFY(isAlmostEqual<double>(value.rgb255.first, 61));
        QVERIFY(isAlmostEqual<double>(value.rgb255.second, 122));
        QVERIFY(isAlmostEqual<double>(value.rgb255.third, 31));
    }

    void testFromHsv()
    {
        const RgbColor value = RgbColor::fromHsv(GenericColor{100, 60, 30});

        QVERIFY(isAlmostEqual<double>(value.hsl.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 43));
        QVERIFY(isAlmostEqual<double>(value.hsl.third, 21));

        QVERIFY(isAlmostEqual<double>(value.hsv.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 60));
        QVERIFY(isAlmostEqual<double>(value.hsv.third, 30));

        QVERIFY(isAlmostEqual<double>(value.hwb.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.second, 12));
        QVERIFY(isAlmostEqual<double>(value.hwb.third, 70));

        QVERIFY(isAlmostEqual<double>(value.rgb255.first, 45));
        QVERIFY(isAlmostEqual<double>(value.rgb255.second, 76));
        QVERIFY(isAlmostEqual<double>(value.rgb255.third, 30));
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

        RgbColor value;

        value = RgbColor::fromHsv(GenericColor{150, 100, 0});
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 100));

        value = RgbColor::fromHsv(GenericColor{150, 60, 0});
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 60));

        value = RgbColor::fromHsv(GenericColor{150, 30, 0});
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 30));

        value = RgbColor::fromHsv(GenericColor{150, 0, 0});
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 0));
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

        RgbColor value;

        value = RgbColor::fromHsl(GenericColor{150, 100, 0});
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 100));

        value = RgbColor::fromHsl(GenericColor{150, 60, 0});
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 60));

        value = RgbColor::fromHsl(GenericColor{150, 30, 0});
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 30));

        value = RgbColor::fromHsl(GenericColor{150, 0, 0});
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 0));
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

        RgbColor value;

        constexpr int saturationOfBlackColor = 0;

        value = RgbColor::fromRgb255(GenericColor{0, 0, 0});
        QVERIFY(isAlmostEqual<double>(value.hsv.second, saturationOfBlackColor));
        QVERIFY(isAlmostEqual<double>(value.hsl.second, saturationOfBlackColor));

        value = RgbColor::fromHwb(GenericColor{320, 0, 100});
        QVERIFY(isAlmostEqual<double>(value.hsv.second, saturationOfBlackColor));
        QVERIFY(isAlmostEqual<double>(value.hsl.second, saturationOfBlackColor));
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

        RgbColor value;

        constexpr int saturationOfWhiteColor = 0;

        value = RgbColor::fromHsl(GenericColor{320, 50, 100});
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 50));

        // All other original color formats should give the standard
        // HSL-saturation for white:

        value = RgbColor::fromRgb255(GenericColor{255, 255, 255});
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.second, saturationOfWhiteColor));

        value = RgbColor::fromHsv(GenericColor{320, 0, 100});
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.second, saturationOfWhiteColor));

        value = RgbColor::fromHwb(GenericColor{320, 100, 0});
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.second, saturationOfWhiteColor));
    }

    void testFromHwb()
    {
        const RgbColor value = RgbColor::fromHwb(GenericColor{100, 60, 30});

        QVERIFY(isAlmostEqual<double>(value.hsl.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 14));
        QVERIFY(isAlmostEqual<double>(value.hsl.third, 65)); //

        QVERIFY(isAlmostEqual<double>(value.hsv.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 15));
        QVERIFY(isAlmostEqual<double>(value.hsv.third, 70)); //

        QVERIFY(isAlmostEqual<double>(value.hwb.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.second, 60));
        QVERIFY(isAlmostEqual<double>(value.hwb.third, 30)); //

        QVERIFY(isAlmostEqual<double>(value.rgb255.first, 162));
        QVERIFY(isAlmostEqual<double>(value.rgb255.second, 179));
        QVERIFY(isAlmostEqual<double>(value.rgb255.third, 153)); //
    }

    void testFromHwbDenormalized()
    {
        const RgbColor value = RgbColor::fromHwb(GenericColor{100, 70, 70});
        // The sum of w and b is greater than 100. This is denormalized.

        QVERIFY(isAlmostEqual<double>(value.hsl.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.second, 0));
        QVERIFY(isAlmostEqual<double>(value.hsl.third, 50)); //

        QVERIFY(isAlmostEqual<double>(value.hsv.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.second, 0));
        QVERIFY(isAlmostEqual<double>(value.hsv.third, 50)); //

        QVERIFY(isAlmostEqual<double>(value.hwb.first, 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.second, 70));
        QVERIFY(isAlmostEqual<double>(value.hwb.third, 70)); //

        QVERIFY(isAlmostEqual<double>(value.rgb255.first, 128));
        QVERIFY(isAlmostEqual<double>(value.rgb255.second, 128));
        QVERIFY(isAlmostEqual<double>(value.rgb255.third, 128)); //
    }

    void testEquality()
    {
        RgbColor myColor1 = RgbColor::fromRgb255(GenericColor{1, 2, 3});
        RgbColor myColor2 = RgbColor::fromRgb255(GenericColor{1, 2, 3});
        QVERIFY(myColor1 == myColor2);
        myColor2.rgb255.first += 1;
        QVERIFY(!(myColor1 == myColor2));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestRgbColor)

// The following “include” is necessary because we do not use a header file:
#include "testrgbcolor.moc"
