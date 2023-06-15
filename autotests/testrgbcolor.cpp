// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "rgbcolor.h"

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
        QCOMPARE(myColor.hwb.size(), 0);
        QCOMPARE(myColor.hsl.size(), 0);
        QCOMPARE(myColor.hsv.size(), 0);
        QCOMPARE(myColor.rgb255.size(), 0);
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
        RgbColor myColor1 = RgbColor::fromRgb255(QList<double>{1, 2, 3});
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
        RgbColor myColor1 = RgbColor::fromRgb255(QList<double>{4, 5, 6});
        RgbColor myColor2 = RgbColor::fromRgb255(QList<double>{7, 8, 9});
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
        RgbColor myColor1 = RgbColor::fromRgb255(QList<double>{113, 53, 23});
        QCOMPARE(myColor1.rgbQColor, QColor::fromRgb(113, 53, 23));
    }

    void testFromRgbQColor()
    {
        RgbColor myColor1 = RgbColor::fromRgbQColor(Qt::yellow);
        QCOMPARE(myColor1.rgbQColor, Qt::yellow);
    }

    void testRgbHue()
    {
        // The hue of the RGB-based HSV, HSL and HBW is identical.
        RgbColor value;

        value = RgbColor::fromHsl(QList<double>({150, 40, 30}));
        QCOMPARE(value.hsl.at(0), 150);
        QCOMPARE(value.hsv.at(0), 150);

        value = RgbColor::fromHsv(QList<double>({150, 40, 30}));
        QCOMPARE(value.hsl.at(0), 150);
        QCOMPARE(value.hsv.at(0), 150);
    }

    void testRgbHueOnGrayAxis()
    {
        // The hue of the RGB-based HSV, HSL and HBW is identical,
        // even when the value is on the gray axis.
        RgbColor value;

        value = RgbColor::fromHsl(QList<double>({150, 0, 50}));
        QCOMPARE(value.hsl.at(0), 150);
        QCOMPARE(value.hsv.at(0), 150);
        QCOMPARE(value.hwb.at(0), 150);

        value = RgbColor::fromHsv(QList<double>({150, 0, 50}));
        QCOMPARE(value.hsl.at(0), 150);
        QCOMPARE(value.hsv.at(0), 150);
        QCOMPARE(value.hwb.at(0), 150);

        value = RgbColor::fromHwb(QList<double>({150, 50, 50}));
        // Sum of w and b is 100.
        QCOMPARE(value.hsl.at(0), 150);
        QCOMPARE(value.hsv.at(0), 150);
        QCOMPARE(value.hwb.at(0), 150);

        value = RgbColor::fromHwb(QList<double>({150, 70, 70}));
        // Sum of w and b is more than 100.
        QCOMPARE(value.hsl.at(0), 150);
        QCOMPARE(value.hsv.at(0), 150);
        QCOMPARE(value.hwb.at(0), 150);

        value = RgbColor::fromRgb255(QList<double>({120, 120, 120}));
        // An RGB value on the gray axis does not provide any information
        // about the hue. We can reasonably expect a standard value: 0°.
        QCOMPARE(value.hsl.at(0), 0);
        QCOMPARE(value.hsv.at(0), 0);
        QCOMPARE(value.hwb.at(0), 0);
    }

    void testHueFromRgbToLchSaturationContinuityWhite()
    {
        // LCH-hue values can be arbitrary when the color is on the gray axis.
        // For usability reasons, we should have nevertheless meaningful
        // hue values.
    }

    void testFromHsl()
    {
        const RgbColor value = RgbColor::fromHsl(QList<double>({100, 60, 30}));

        QVERIFY(isAlmostEqual<double>(value.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 60));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(2), 30));

        QVERIFY(isAlmostEqual<double>(value.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 75));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(2), 48));

        QVERIFY(isAlmostEqual<double>(value.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(1), 12));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(2), 52));

        QVERIFY(isAlmostEqual<double>(value.rgb255.at(0), 61));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(1), 122));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(2), 31));
    }

    void testFromHsv()
    {
        const RgbColor value = RgbColor::fromHsv(QList<double>({100, 60, 30}));

        QVERIFY(isAlmostEqual<double>(value.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 43));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(2), 21));

        QVERIFY(isAlmostEqual<double>(value.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 60));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(2), 30));

        QVERIFY(isAlmostEqual<double>(value.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(1), 12));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(2), 70));

        QVERIFY(isAlmostEqual<double>(value.rgb255.at(0), 45));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(1), 76));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(2), 30));
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

        value = RgbColor::fromHsv(QList<double>({150, 100, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 100));

        value = RgbColor::fromHsv(QList<double>({150, 60, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 60));

        value = RgbColor::fromHsv(QList<double>({150, 30, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 30));

        value = RgbColor::fromHsv(QList<double>({150, 0, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 0));
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

        value = RgbColor::fromHsl(QList<double>({150, 100, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 100));

        value = RgbColor::fromHsl(QList<double>({150, 60, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 60));

        value = RgbColor::fromHsl(QList<double>({150, 30, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 30));

        value = RgbColor::fromHsl(QList<double>({150, 0, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 0));
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

        value = RgbColor::fromRgb255(QList<double>({0, 0, 0}));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), saturationOfBlackColor));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), saturationOfBlackColor));

        value = RgbColor::fromHwb(QList<double>({320, 0, 100}));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), saturationOfBlackColor));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), saturationOfBlackColor));
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

        value = RgbColor::fromHsl(QList<double>({320, 50, 100}));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 50));

        // All other original color formats should give the standard
        // HSL-saturation for white:

        value = RgbColor::fromRgb255(QList<double>({255, 255, 255}));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), saturationOfWhiteColor));

        value = RgbColor::fromHsv(QList<double>({320, 0, 100}));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), saturationOfWhiteColor));

        value = RgbColor::fromHwb(QList<double>({320, 100, 0}));
        // Expect a non-standard value because original values
        // should never be changed.
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), saturationOfWhiteColor));
    }

    void testFromHwb()
    {
        const RgbColor value = RgbColor::fromHwb(QList<double>({100, 60, 30}));

        QVERIFY(isAlmostEqual<double>(value.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 14));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(2), 65)); //

        QVERIFY(isAlmostEqual<double>(value.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 15));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(2), 70)); //

        QVERIFY(isAlmostEqual<double>(value.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(1), 60));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(2), 30)); //

        QVERIFY(isAlmostEqual<double>(value.rgb255.at(0), 162));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(1), 179));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(2), 153)); //
    }

    void testFromHwbDenormalized()
    {
        const RgbColor value = RgbColor::fromHwb(QList<double>({100, 70, 70}));
        // The sum of w and b is greater than 100. This is denormalized.

        QVERIFY(isAlmostEqual<double>(value.hsl.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(1), 0));
        QVERIFY(isAlmostEqual<double>(value.hsl.at(2), 50)); //

        QVERIFY(isAlmostEqual<double>(value.hsv.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(1), 0));
        QVERIFY(isAlmostEqual<double>(value.hsv.at(2), 50)); //

        QVERIFY(isAlmostEqual<double>(value.hwb.at(0), 100));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(1), 70));
        QVERIFY(isAlmostEqual<double>(value.hwb.at(2), 70)); //

        QVERIFY(isAlmostEqual<double>(value.rgb255.at(0), 128));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(1), 128));
        QVERIFY(isAlmostEqual<double>(value.rgb255.at(2), 128)); //
    }

    void testEquality()
    {
        RgbColor myColor1 = RgbColor::fromRgb255(QList<double>{1, 2, 3});
        RgbColor myColor2 = RgbColor::fromRgb255(QList<double>{1, 2, 3});
        QVERIFY(myColor1 == myColor2);
        myColor2.rgb255[0] += 1;
        QVERIFY(!(myColor1 == myColor2));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestRgbColor)

// The following “include” is necessary because we do not use a header file:
#include "testrgbcolor.moc"
