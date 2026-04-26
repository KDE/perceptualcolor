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
#include <qtmetamacros.h>

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

    void testConstructorWithVec3d()
    {
        constexpr Vec3d testValue = {0.1, 0.2, 0.3};
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

    void testToVec3d()
    {
        constexpr GenericColor color(50, 20, 30);
        constexpr Vec3d vector = color.toVec3d();
        QCOMPARE(vector.v[0], 50.0);
        QCOMPARE(vector.v[1], 20.0);
        QCOMPARE(vector.v[2], 30.0);
    }

    void testToCmsCIEXYZ()
    {
        constexpr GenericColor color(0.1, 0.2, 0.3);
        constexpr cmsCIEXYZ ciexyz = color.reinterpretAsXyzToCmsciexyz();
        QCOMPARE(ciexyz.X, 0.1);
        QCOMPARE(ciexyz.Y, 0.2);
        QCOMPARE(ciexyz.Z, 0.3);
    }

    void testToCmsCIELab()
    {
        constexpr GenericColor color(50, 20, 30);
        constexpr cmsCIELab cielab = color.reinterpretAsLabToCmscielab();
        QCOMPARE(cielab.L, 50.0);
        QCOMPARE(cielab.a, 20.0);
        QCOMPARE(cielab.b, 30.0);
    }

    void testToCmsCIELCh()
    {
        constexpr GenericColor color(50, 20, 30);
        constexpr cmsCIELCh cielch = color.reinterpretAsLchToCmscielch();
        QCOMPARE(cielch.L, 50.0);
        QCOMPARE(cielch.C, 20.0);
        QCOMPARE(cielch.h, 30.0);
    }

    void testEqualityOperatorsEqual1()
    {
        // Create two GenericColor objects with the same values
        constexpr GenericColor color1(1.0, 2.0, 3.0);
        constexpr GenericColor color2(1.0, 2.0, 3.0);

        // Test equality
        constexpr bool equal = (color1 == color2);
        constexpr bool unequal = (color1 != color2);
        QVERIFY(equal);
        QVERIFY(!unequal);
    }

    void testEqualityOperatorsEqual2()
    {
        // Create two GenericColor objects with the same values
        constexpr GenericColor color1(0.0, 0.0, 0.0);
        constexpr GenericColor color2;

        // Test equality
        constexpr bool equal = (color1 == color2);
        constexpr bool unequal = (color1 != color2);
        QVERIFY(equal);
        QVERIFY(!unequal);
    }

    void testEqualityOperatorsUnequal1()
    {
        // Create two GenericColor objects with different values
        constexpr GenericColor color1(1.0, 2.0, 3.0);
        constexpr GenericColor color2(4.0, 5.0, 6.0);

        // Test unequality
        constexpr bool equal = (color1 == color2);
        constexpr bool unequal = (color1 != color2);
        QVERIFY(!equal);
        QVERIFY(unequal);
    }

    void testEqualityOperatorsUnequal2()
    {
        // Create two GenericColor objects with different values
        constexpr GenericColor color1(1.0, 2.0, 3.0);
        constexpr GenericColor color2;

        // Test unequality
        constexpr bool equal = (color1 == color2);
        constexpr bool unequal = (color1 != color2);
        QVERIFY(!equal);
        QVERIFY(unequal);
    }

    void testEqualityOperatorsUnequal3()
    {
        // Create two GenericColor objects with different values
        constexpr GenericColor color1(0.0, 0.0, 0.1);
        constexpr GenericColor color2;

        // Test unequality
        constexpr bool equal = (color1 == color2);
        constexpr bool unequal = (color1 != color2);
        QVERIFY(!equal);
        QVERIFY(unequal);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestGenericColor)

// The following “include” is necessary because we do not use a header file:
#include "testgenericcolor.moc"
