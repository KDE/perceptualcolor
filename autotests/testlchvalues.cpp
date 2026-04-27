// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "lchvalues.h"

#include "chromainfo.h"
#include "genericcolor.h"
#include "helperposixmath.h"
#include <qglobal.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
class TestLchValue : public QObject
{
    Q_OBJECT

public:
    explicit TestLchValue(QObject *parent = nullptr)
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
        QCOMPARE(static_cast<qreal>(PerceptualColor::cielchD50Values.neutralChroma), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::cielchD50Values.neutralHue), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::cielchD50Values.neutralLightness), 50);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::oklchValues.neutralChroma), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::oklchValues.neutralHue), 0);
        // Is the value as documented?
        QCOMPARE(static_cast<qreal>(PerceptualColor::oklchValues.neutralLightness), 0.5f);
    }

    void testNeutralGray()
    {
        // Test that the unified initialization is done in the correct order.
        QCOMPARE(cielchD50Values.neutralGray().first,
                 50 // Should be half the way between light and dark
        );
        QCOMPARE(cielchD50Values.neutralGray().second,
                 0 // Should have no chroma
        );
        QCOMPARE(cielchD50Values.neutralGray().third,
                 0 // Hue does not matter, but by convention should be 0
        );
        // Test that the unified initialization is done in the correct order.
        QCOMPARE(oklchValues.neutralGray().first,
                 0.5f // Should be half the way between light and dark
        );
        QCOMPARE(oklchValues.neutralGray().second,
                 0 // Should have no chroma
        );
        QCOMPARE(oklchValues.neutralGray().third,
                 0 // Hue does not matter, but by convention should be 0
        );

        // Test if neutralGray() can return constexpr results.
        constexpr auto myValue1 = PerceptualColor::cielchD50Values.neutralGray();
        constexpr auto myValue2 = PerceptualColor::oklchValues.neutralGray();
        constexpr LchValues temp{};
        constexpr auto myValue3 = temp.neutralGray();
        Q_UNUSED(myValue1)
        Q_UNUSED(myValue2)
        Q_UNUSED(myValue3)
    }

    void testCielchMaximumChroma()
    {
        QVERIFY(cielchD50Values.maximumChroma > 0);

        const auto maxOkChroma = ChromaInfo::maxCielchD50Chroma();
        QVERIFY(cielchD50Values.maximumChroma > maxOkChroma);
    }

    void testOklchMaximumChroma()
    {
        QVERIFY(oklchValues.maximumChroma > 0);

        const auto maxOkChroma = ChromaInfo::maxOklchChroma();
        QVERIFY(oklchValues.maximumChroma > maxOkChroma);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestLchValue)
// The following “include” is necessary because we do not use a header file:
#include "testlchvalues.moc"
