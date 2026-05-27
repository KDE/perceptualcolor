// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "colorspaceinfo.h"

#include "helpermath.h"
#include <qglobal.h>
#include <qobject.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
class TestColorSpaceInfo : public QObject
{
    Q_OBJECT

public:
    explicit TestColorSpaceInfo(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
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
        const ColorSpaceInfo temp;
        Q_UNUSED(temp);
    }

    void testMaxOklchChroma()
    {
        QVERIFY(ColorSpaceInfo::maxOklchChroma() > 0.3);
        QVERIFY(ColorSpaceInfo::maxOklchChroma() < 0.4);
    }

    void testMaxCielchD50Chroma()
    {
        QVERIFY(ColorSpaceInfo::maxCielchD50Chroma() > 130);
        QVERIFY(ColorSpaceInfo::maxCielchD50Chroma() < 140);
    }

    void testMaxChromaColorByCielchD50Hue360()
    {
        const auto temp = ColorSpaceInfo::maxChromaColorByCielchD50Hue360(42);
        Q_UNUSED(temp);
    }

    void testMaxChromaColorByOklabHue360()
    {
        const auto temp = ColorSpaceInfo::maxChromaColorByOklabHue360(42);
        Q_UNUSED(temp);
    }

    void testCielabD50BlackpointL()
    {
        QVERIFY(isInRange<double>(0, ColorSpaceInfo::cielabD50BlackpointL(), 1));
    }

    void testCielabD50WhitepointL()
    {
        QVERIFY(isInRange<double>(99, ColorSpaceInfo::cielabD50WhitepointL(), 100));
    }

    void testOklabBlackpointL()
    {
        QVERIFY(isInRange<double>(0, ColorSpaceInfo::oklabBlackpointL(), 0.01));
    }

    void testOklabWhitepointL()
    {
        QVERIFY(isInRange<double>(0.99, ColorSpaceInfo::oklabWhitepointL(), 1));
    }

    void testUnusualHueCielchD50()
    {
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::CielchD50, 0));
        QVERIFY(ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::CielchD50, 99));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::CielchD50, 110));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::CielchD50, 359));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::CielchD50, 360));
        constexpr auto temp = ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::CielchD50, 0);
        Q_UNUSED(temp)
    }

    void testUnusualHueOklch()
    {
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::Oklch, 0));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::Oklch, 264));
        QVERIFY(ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::Oklch, 264.2));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::Oklch, 264.3));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::Oklch, 359));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::Oklch, 360));
        constexpr auto temp = ColorSpaceInfo::isUnusualShapeAtHue(LchSpace::Oklch, 0);
        Q_UNUSED(temp)
    }

    void testUnusualLightnessCielchD50()
    {
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::CielchD50, 0));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::CielchD50, 92));
        QVERIFY(ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::CielchD50, 94));
        QVERIFY(ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::CielchD50, 98));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::CielchD50, 100));
        constexpr auto temp = ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::CielchD50, 0);
        Q_UNUSED(temp)
    }

    void testUnusualLightnessOklch()
    {
        QVERIFY(ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::Oklch, 0.2));
        QVERIFY(ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::Oklch, 0.52));
        QVERIFY(ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::Oklch, 0.53));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::Oklch, 0.54));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::Oklch, 0.99));
        QVERIFY(!ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::Oklch, 1));
        constexpr auto temp = ColorSpaceInfo::isUnusualShapeAtLightness(LchSpace::Oklch, 0);
        Q_UNUSED(temp)
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorSpaceInfo)

// The following “include” is necessary because we do not use a header file:
#include "testcolorspaceinfo.moc"
