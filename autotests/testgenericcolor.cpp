// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "genericcolor.h"

#include "helpermath.h"
#include <lcms2.h>
#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestGenericColor : public QObject
{
    Q_OBJECT

public:
    explicit TestGenericColor(QObject *parent = nullptr)
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

    void testConstructorWithoutArguments()
    {
        constexpr GenericColor color{};
        QCOMPARE(color.first, 0.0);
        QCOMPARE(color.second, 0.0);
        QCOMPARE(color.third, 0.0);
        QCOMPARE(color.fourth, 0.0);
    }

    void testConstructorWithTrio()
    {
        const Trio testValue = createMatrix<1, 3, double>(10., 20., 30.);
        // NOTE Not using constexpr here because Trio is derived from
        // QGenericMatrix which cannot be constructed as constexpr.
        GenericColor color(testValue);
        QCOMPARE(color.first, 10.0);
        QCOMPARE(color.second, 20.0);
        QCOMPARE(color.third, 30.0);
        QCOMPARE(color.fourth, 0.0);
    }

    void testConstructorWithCmsCIELab()
    {
        constexpr cmsCIELab testValue = {50, 20, 30};
        constexpr GenericColor color(testValue);
        QCOMPARE(color.first, 50.0);
        QCOMPARE(color.second, 20.0);
        QCOMPARE(color.third, 30.0);
        QCOMPARE(color.fourth, 0.0);
    }

    void testConstructorWithCmsCIEXYZ()
    {
        constexpr cmsCIEXYZ testValue = {0.1, 0.2, 0.3};
        constexpr GenericColor color(testValue);
        QCOMPARE(color.first, 0.1);
        QCOMPARE(color.second, 0.2);
        QCOMPARE(color.third, 0.3);
        QCOMPARE(color.fourth, 0.0);
    }

    void testConstructorWith3Args()
    {
        constexpr GenericColor color(10, 20, 30);
        QCOMPARE(color.first, 10.0);
        QCOMPARE(color.second, 20.0);
        QCOMPARE(color.third, 30.0);
        QCOMPARE(color.fourth, 0);
    }

    void testConstructorWith4Args()
    {
        constexpr GenericColor color(10, 20, 30, 40);
        QCOMPARE(color.first, 10.0);
        QCOMPARE(color.second, 20.0);
        QCOMPARE(color.third, 30.0);
        QCOMPARE(color.fourth, 40.0);
    }

    void testToTrio()
    {
        GenericColor color(10, 20, 30);
        Trio trio = color.toTrio();
        QCOMPARE(trio(0, 0), 10.0);
        QCOMPARE(trio(1, 0), 20.0);
        QCOMPARE(trio(2, 0), 30.0);
    }

    void testToCmsCIEXYZ()
    {
        GenericColor color(0.1, 0.2, 0.3);
        cmsCIEXYZ ciexyz = color.reinterpretAsXyzToCmsciexyz();
        QCOMPARE(ciexyz.X, 0.1);
        QCOMPARE(ciexyz.Y, 0.2);
        QCOMPARE(ciexyz.Z, 0.3);
    }

    void testToCmsCIELab()
    {
        GenericColor color(50, 20, 30);
        cmsCIELab cielab = color.reinterpretAsLabToCmscielab();
        QCOMPARE(cielab.L, 50.0);
        QCOMPARE(cielab.a, 20.0);
        QCOMPARE(cielab.b, 30.0);
    }

    void testEqualityOperatorsEqual1()
    {
        // Create two GenericColor objects with the same values
        GenericColor color1(1.0, 2.0, 3.0);
        GenericColor color2(1.0, 2.0, 3.0);

        // Test equality
        QVERIFY(color1 == color2);
        QVERIFY(!(color1 != color2));
    }

    void testEqualityOperatorsEqual2()
    {
        // Create two GenericColor objects with the same values
        GenericColor color1(0.0, 0.0, 0.0);
        GenericColor color2;

        // Test equality
        QVERIFY(color1 == color2);
        QVERIFY(!(color1 != color2));
    }

    void testEqualityOperatorsUnequal1()
    {
        // Create two GenericColor objects with different values
        GenericColor color1(1.0, 2.0, 3.0);
        GenericColor color2(4.0, 5.0, 6.0);

        // Test inequality
        QVERIFY(!(color1 == color2));
        QVERIFY(color1 != color2);
    }

    void testEqualityOperatorsUnequal2()
    {
        // Create two GenericColor objects with different values
        GenericColor color1(1.0, 2.0, 3.0);
        GenericColor color2;

        // Test inequality
        QVERIFY(!(color1 == color2));
        QVERIFY(color1 != color2);
    }

    void testEqualityOperatorsUnequal3()
    {
        // Create two GenericColor objects with different values
        GenericColor color1(0.0, 0.0, 0.1);
        GenericColor color2;

        // Test inequality
        QVERIFY(!(color1 == color2));
        QVERIFY(color1 != color2);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestGenericColor)

// The following “include” is necessary because we do not use a header file:
#include "testgenericcolor.moc"
