// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helpermath.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <optional>
#include <qgenericmatrix.h>
#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{

class TestHelperMath : public QObject
{
    Q_OBJECT

public:
    explicit TestHelperMath(QObject *parent = nullptr)
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

    void testInRangeInt()
    {
        QCOMPARE(PerceptualColor::isInRange<int>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::isInRange<int>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::isInRange<int>(-3, 0, -1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 2, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 0, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 4, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(3, 2, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 1, 2), true);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 0, 2), true);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 2, 2), true);
        QCOMPARE(PerceptualColor::isInRange<double>(0, 3, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(0, -1, 2), false);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 2, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 1, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 3, 3), true);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 0, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(1, 4, 3), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, 0, 1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, -1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, 1, 1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, 2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-1, -2, 1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, -1, 0), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, -2, 0), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, 0, 0), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, -3, 0), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-2, 1, 0), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -2, -1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -3, -1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -1, -1), true);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, -4, -1), false);
        QCOMPARE(PerceptualColor::isInRange<double>(-3, 0, -1), false);

        QCOMPARE(PerceptualColor::isInRange<double>(-3.1, 0.2, -1.3), false);

        if constexpr (std::numeric_limits<double>::has_infinity) {
            constexpr auto inf = std::numeric_limits<double>::infinity();
            QCOMPARE(PerceptualColor::isInRange<double>(-inf, 0, inf), //
                     true);
            QCOMPARE(PerceptualColor::isInRange<double>(-5, 0, inf), //
                     true);
            QCOMPARE(PerceptualColor::isInRange<double>(-inf, 0, 5), //
                     true);
            QCOMPARE(PerceptualColor::isInRange<double>(inf, 0, -inf), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(5, 0, -inf), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(inf, 0, -5), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(-inf, -inf, inf), //
                     true);
            QCOMPARE(PerceptualColor::isInRange<double>(-inf, inf, inf), //
                     true);
            QCOMPARE(PerceptualColor::isInRange<double>(-5, inf, inf), //
                     true);
            QCOMPARE(PerceptualColor::isInRange<double>(-inf, -inf, 5), //
                     true);
        }

        if constexpr (std::numeric_limits<double>::has_quiet_NaN) {
            constexpr auto nan = std::numeric_limits<double>::quiet_NaN();
            QCOMPARE(PerceptualColor::isInRange<double>(3, 4, 5), //
                     true);
            QCOMPARE(PerceptualColor::isInRange<double>(nan, 4, 5), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(3, nan, 5), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(3, 4, nan), //
                     false);

            QCOMPARE(PerceptualColor::isInRange<double>(5, 4, 3), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(nan, 4, 3), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(5, nan, 3), //
                     false);
            QCOMPARE(PerceptualColor::isInRange<double>(5, 4, nan), //
                     false);
        }
    }

    void testRounding()
    {
        QCOMPARE(roundToDigits(12.3456, 6), 12.345600);
        QCOMPARE(roundToDigits(12.3456, 5), 12.34560);
        QCOMPARE(roundToDigits(12.3456, 4), 12.3456);
        QCOMPARE(roundToDigits(12.3456, 3), 12.346);
        QCOMPARE(roundToDigits(12.3456, 2), 12.35);
        QCOMPARE(roundToDigits(12.3456, 1), 12.3);
        QCOMPARE(roundToDigits(12.3456, 0), 12.);
        QCOMPARE(roundToDigits(12.3456, -1), 10.);
        QCOMPARE(roundToDigits(12.3456, -2), 0.);
        QCOMPARE(roundToDigits(92.3456, -2), 100.);

        QCOMPARE(roundToDigits(-12.3456, 6), -12.345600);
        QCOMPARE(roundToDigits(-12.3456, 5), -12.34560);
        QCOMPARE(roundToDigits(-12.3456, 4), -12.3456);
        QCOMPARE(roundToDigits(-12.3456, 3), -12.346);
        QCOMPARE(roundToDigits(-12.3456, 2), -12.35);
        QCOMPARE(roundToDigits(-12.3456, 1), -12.3);
        QCOMPARE(roundToDigits(-12.3456, 0), -12.);
        QCOMPARE(roundToDigits(-12.3456, -1), -10.);
        QCOMPARE(roundToDigits(-12.3456, -2), -0.);
        QCOMPARE(roundToDigits(-92.3456, -2), -100.);

        if constexpr (std::numeric_limits<double>::has_infinity) {
            QCOMPARE( //
                roundToDigits(std::numeric_limits<double>::infinity(), 6), //
                std::numeric_limits<double>::infinity());
            QCOMPARE( //
                roundToDigits(-std::numeric_limits<double>::infinity(), 6), //
                -std::numeric_limits<double>::infinity());
        }

        if constexpr (std::numeric_limits<double>::has_quiet_NaN) {
            constexpr auto quietNan = std::numeric_limits<double>::quiet_NaN();
            QVERIFY(std::isnan(roundToDigits(quietNan, 6)));
        }

        if constexpr (std::numeric_limits<double>::has_signaling_NaN) {
            constexpr auto signalingNan = //
                std::numeric_limits<double>::signaling_NaN();
            QVERIFY(std::isnan(roundToDigits(signalingNan, 6)));
        }
    }

    void testIsOdd()
    {
        QCOMPARE(isOdd(-2), false);
        QCOMPARE(isOdd(-1), true);
        QCOMPARE(isOdd(-0), false);
        QCOMPARE(isOdd(0), false);
        QCOMPARE(isOdd(1), true);
        QCOMPARE(isOdd(2), false);
    }

    void testIsNearlyEqual()
    {
        QVERIFY(isNearlyEqual(2., 2.));
        QVERIFY(isNearlyEqual(1.5, 1.5));
        QVERIFY(isNearlyEqual(1., 1.));
        QVERIFY(isNearlyEqual(0.5, 0.5));
        QVERIFY(isNearlyEqual(0., 0.));
        QVERIFY(isNearlyEqual(-0., 0.));
        QVERIFY(isNearlyEqual(0., -0.));
        QVERIFY(isNearlyEqual(-0., -0.));
        QVERIFY(isNearlyEqual(-0.5, -0.5));

        QVERIFY(!isNearlyEqual(0., 1.));

        if constexpr (std::numeric_limits<double>::has_infinity) {
            QVERIFY(isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                  std::numeric_limits<double>::infinity()));
            QVERIFY(isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                  -std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   -std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   5.));
            QVERIFY(!isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                   5.));
        }

        if constexpr (std::numeric_limits<double>::has_quiet_NaN) {
            // NaN should never compare equal to itself or any other value.
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   5.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   0.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   -std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   std::numeric_limits<double>::quiet_NaN()));

            // NaN should never compare equal to itself or any other value.
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               5.));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               0.));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::infinity()));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               -std::numeric_limits<double>::infinity()));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::signaling_NaN()));
        }

        if constexpr (std::numeric_limits<double>::has_signaling_NaN //
                      && std::numeric_limits<double>::has_quiet_NaN //
        ) {
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::quiet_NaN()));
        }
    }

    void testIsNearlyEqualEpsilon()
    {
        QVERIFY(isNearlyEqual(2., 2., 0.1));
        QVERIFY(isNearlyEqual(1.5, 1.5, 0.1));
        QVERIFY(isNearlyEqual(1., 1., 0.1));
        QVERIFY(isNearlyEqual(0.5, 0.5, 0.1));
        QVERIFY(isNearlyEqual(0., 0., 0.1));
        QVERIFY(isNearlyEqual(-0., 0., 0.1));
        QVERIFY(isNearlyEqual(0., -0., 0.1));
        QVERIFY(isNearlyEqual(-0., -0., 0.1));
        QVERIFY(isNearlyEqual(-0.5, -0.5, 0.1));

        QVERIFY(isNearlyEqual(2., 2., 0.));
        QVERIFY(isNearlyEqual(2., 2., -0.));
        QVERIFY(isNearlyEqual(2., 2., -1.));

        QVERIFY(isNearlyEqual(5., 6., 2.));

        QVERIFY(!isNearlyEqual(0., 1., 0.));
        QVERIFY(!isNearlyEqual(0., 1., -0.));
        QVERIFY(!isNearlyEqual(0., 1., -1.));

        if constexpr (std::numeric_limits<double>::has_infinity) {
            QVERIFY(isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                  std::numeric_limits<double>::infinity(),
                                  2.));
            QVERIFY(isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                  -std::numeric_limits<double>::infinity(),
                                  2.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   -std::numeric_limits<double>::infinity(),
                                   2.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   5.,
                                   2.));
            QVERIFY(!isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                   5.,
                                   2.));

            QVERIFY(isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                  std::numeric_limits<double>::infinity(),
                                  std::numeric_limits<double>::infinity()));
            QVERIFY(isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                  -std::numeric_limits<double>::infinity(),
                                  std::numeric_limits<double>::infinity()));

            QVERIFY(isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                  std::numeric_limits<double>::infinity(),
                                  -std::numeric_limits<double>::infinity()));
            QVERIFY(isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                  -std::numeric_limits<double>::infinity(),
                                  -std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   -std::numeric_limits<double>::infinity(),
                                   -std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   5.,
                                   -std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                   5.,
                                   -std::numeric_limits<double>::infinity()));

            if constexpr (std::numeric_limits<double>::has_quiet_NaN) {
                QVERIFY( //
                    !isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   std::numeric_limits<double>::infinity(),
                                   std::numeric_limits<double>::quiet_NaN()));
                QVERIFY( //
                    !isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                   -std::numeric_limits<double>::infinity(),
                                   std::numeric_limits<double>::quiet_NaN()));
                QVERIFY( //
                    !isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   -std::numeric_limits<double>::infinity(),
                                   std::numeric_limits<double>::quiet_NaN()));
                QVERIFY( //
                    !isNearlyEqual(std::numeric_limits<double>::infinity(), //
                                   5.,
                                   std::numeric_limits<double>::quiet_NaN()));
                QVERIFY( //
                    !isNearlyEqual(-std::numeric_limits<double>::infinity(), //
                                   5.,
                                   std::numeric_limits<double>::quiet_NaN()));
            }
        }

        if constexpr (std::numeric_limits<double>::has_quiet_NaN) {
            // NaN should never compare equal to itself or any other value.
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   5.,
                                   2.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   0.,
                                   2.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   std::numeric_limits<double>::infinity(),
                                   2.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   -std::numeric_limits<double>::infinity(),
                                   2.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   std::numeric_limits<double>::quiet_NaN(),
                                   2.));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   5.,
                                   std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   0.,
                                   std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   std::numeric_limits<double>::infinity(),
                                   std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   -std::numeric_limits<double>::infinity(),
                                   std::numeric_limits<double>::infinity()));
            QVERIFY(!isNearlyEqual(std::numeric_limits<double>::quiet_NaN(), //
                                   std::numeric_limits<double>::quiet_NaN(),
                                   std::numeric_limits<double>::infinity()));

            // NaN should never compare equal to itself or any other value.
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               5.,
                               2.));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               0.,
                               2.));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::infinity(),
                               2.));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               -std::numeric_limits<double>::infinity(),
                               2.));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::signaling_NaN(),
                               2.));

            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               5.,
                               std::numeric_limits<double>::infinity()));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               0.,
                               std::numeric_limits<double>::infinity()));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::infinity(),
                               std::numeric_limits<double>::infinity()));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               -std::numeric_limits<double>::infinity(),
                               std::numeric_limits<double>::infinity()));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::signaling_NaN(),
                               std::numeric_limits<double>::infinity()));
        }

        if constexpr (std::numeric_limits<double>::has_signaling_NaN //
                      && std::numeric_limits<double>::has_quiet_NaN //
        ) {
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::quiet_NaN(),
                               2.));
            QVERIFY( //
                !isNearlyEqual(std::numeric_limits<double>::signaling_NaN(), //
                               std::numeric_limits<double>::quiet_NaN(),
                               std::numeric_limits<double>::infinity()));
        }
    }

    void testNormalizeAngle()
    {
        QCOMPARE(normalizedAngle360(0.), 0);
        QCOMPARE(normalizedAngle360(359.9), 359.9);
        QCOMPARE(normalizedAngle360(360.), 0);
        QCOMPARE(normalizedAngle360(720.), 0);
        QCOMPARE(normalizedAngle360(-1.), 359);
        QCOMPARE(normalizedAngle360(-1.3), 358.7);
    }

    void testCreateSquareMatrix3()
    {
        // clang-format off
        const auto temp = createSquareMatrix3(
            1, 2, 3,
            4, 5, 6,
            7, 8, 9);
        // clang-format on
        QCOMPARE(temp(0, 0), 1);
        QCOMPARE(temp(0, 1), 2);
        QCOMPARE(temp(0, 2), 3);
        QCOMPARE(temp(1, 0), 4);
        QCOMPARE(temp(1, 1), 5);
        QCOMPARE(temp(1, 2), 6);
        QCOMPARE(temp(2, 0), 7);
        QCOMPARE(temp(2, 1), 8);
        QCOMPARE(temp(2, 2), 9);
    }

    void testInverseMatrixFromNonInvertible()
    {
        // clang-format off
        const auto temp = createSquareMatrix3(
            1, 2, 3, //
            4, 5, 6, //
            7, 8, 9);
        // clang-format on
        const auto inverse = inverseMatrix(temp);
        QVERIFY(!inverse.has_value());
    }

    void testInverseMatrixFromInvertible()
    {
        // clang-format off
        const auto temp = createSquareMatrix3(
            1, 2, 1, //
            0, 1, 3, //
            -1, 0, 1);
        // clang-format on
        const auto actualInverse = inverseMatrix(temp);
        // clang-format off
        const auto expectedInverse = createSquareMatrix3(
            -0.25, 0.5, -1.25,
            0.75, -0.5, 0.75,
            -0.25, 0.5, -0.25);
        // clang-format on
        QCOMPARE(actualInverse, expectedInverse);
    }

    void testCreateTrio()
    {
        const Trio temp = createTrio(7, 6, 5);
        QCOMPARE(temp(0, 0), 7);
        QCOMPARE(temp(1, 0), 6);
        QCOMPARE(temp(2, 0), 5);
    }

    void testCreateMatrix()
    {
        const auto temp = createMatrix<2, 2, int>(7, 6, 5, 4);
        QCOMPARE(temp(0, 0), 7);
        QCOMPARE(temp(0, 1), 6);
        QCOMPARE(temp(1, 0), 5);
        QCOMPARE(temp(1, 1), 4);
    }

    void testDecimalPlaces_data()
    {
        QTest::addColumn<int>("maxRange");
        QTest::addColumn<int>("significantFigures");
        QTest::addColumn<int>("expected");

        QTest::newRow("0 << -1 << 0") << 0 << -1 << 0;
        QTest::newRow("0 << 0 << 0") << 0 << 0 << 0;
        QTest::newRow("0 << 1 << 0") << 0 << 1 << 0;
        QTest::newRow("0 << 2 << 1") << 0 << 2 << 1;
        QTest::newRow("0 << 3 << 2") << 0 << 3 << 2;
        QTest::newRow("0 << 4 << 3") << 0 << 4 << 3;

        QTest::newRow("1 << -1 << 0") << 1 << -1 << 0;
        QTest::newRow("1 << 0 << 0") << 1 << 0 << 0;
        QTest::newRow("1 << 1 << 0") << 1 << 1 << 0;
        QTest::newRow("1 << 2 << 1") << 1 << 2 << 1;
        QTest::newRow("1 << 3 << 2") << 1 << 3 << 2;
        QTest::newRow("1 << 4 << 3") << 1 << 4 << 3;

        QTest::newRow("2 << -1 << 0") << 2 << -1 << 0;
        QTest::newRow("2 << 0 << 0") << 2 << 0 << 0;
        QTest::newRow("2 << 1 << 0") << 2 << 1 << 0;
        QTest::newRow("2 << 2 << 1") << 2 << 2 << 1;
        QTest::newRow("2 << 3 << 2") << 2 << 3 << 2;
        QTest::newRow("2 << 4 << 3") << 2 << 4 << 3;

        QTest::newRow("100 << -1 << 0") << 100 << -1 << 0;
        QTest::newRow("100 << 0 << 0") << 100 << 0 << 0;
        QTest::newRow("100 << 1 << 0") << 100 << 1 << 0;
        QTest::newRow("100 << 2 << 0") << 100 << 2 << 0;
        QTest::newRow("100 << 3 << 0") << 100 << 3 << 0;
        QTest::newRow("100 << 4 << 1") << 100 << 4 << 1;

        QTest::newRow("255 << -1 << 0") << 255 << -1 << 0;
        QTest::newRow("255 << 0 << 0") << 255 << 0 << 0;
        QTest::newRow("255 << 1 << 0") << 255 << 1 << 0;
        QTest::newRow("255 << 2 << 0") << 255 << 2 << 0;
        QTest::newRow("255 << 3 << 0") << 255 << 3 << 0;
        QTest::newRow("255 << 4 << 1") << 255 << 4 << 1;

        QTest::newRow("-255 << -1 << 0") << -255 << -1 << 0;
        QTest::newRow("-255 << 0 << 0") << -255 << 0 << 0;
        QTest::newRow("-255 << 1 << 0") << -255 << 1 << 0;
        QTest::newRow("-255 << 2 << 0") << -255 << 2 << 0;
        QTest::newRow("-255 << 3 << 0") << -255 << 3 << 0;
        QTest::newRow("-255 << 4 << 1") << -255 << 4 << 1;

        QTest::newRow("360 << -1 << 0") << 360 << -1 << 0;
        QTest::newRow("360 << 0 << 0") << 360 << 0 << 0;
        QTest::newRow("360 << 1 << 0") << 360 << 1 << 0;
        QTest::newRow("360 << 2 << 0") << 360 << 2 << 0;
        QTest::newRow("360 << 3 << 0") << 360 << 3 << 0;
        QTest::newRow("360 << 4 << 1") << 360 << 4 << 1;
    }

    void testDecimalPlaces()
    {
        QFETCH(int, maxRange);
        QFETCH(int, significantFigures);
        QFETCH(int, expected);

        QCOMPARE(decimalPlaces(maxRange, significantFigures), expected);
    }

    void testNormalizePolar360()
    {
        double radius;
        double angleDegree;

        // Value of 0, 0°
        radius = 0;
        angleDegree = 0;
        normalizePolar360(radius, angleDegree);
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 0);

        // Same for initialization with -0, 0
        radius = -0;
        angleDegree = 0;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 0);

        // Yet normalized values are taken as-is
        radius = 2;
        angleDegree = 3;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 3);

        // Negative radii are normalized (180° shift for angle)
        radius = -2;
        angleDegree = 183;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 3);

        // Out-of-range angle is normalized
        radius = 2;
        angleDegree = 363;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 3);

        radius = 2;
        angleDegree = -357;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 3);

        // Also when both radius and angle are
        // out-of-range, normalization works
        radius = -2;
        angleDegree = -357;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 183);
        radius = -2;
        angleDegree = -717;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 183);
        radius = -2;
        angleDegree = 363;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 183);
        radius = -2;
        angleDegree = 723;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 2);
        QCOMPARE(angleDegree, 183);

        // When radius is 0, angle (while meaningless) is
        // preserved (but normalized)
        radius = 0;
        angleDegree = 150;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 150);
        radius = 0;
        angleDegree = 370;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 10);

        // When radius is -0, angle (while meaningless) is
        // preserved (but normalized)
        radius = -0;
        angleDegree = 150;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 150);
        radius = -0;
        angleDegree = 370;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 10);

        // Edge case: 360°
        radius = -0;
        angleDegree = 360;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 0);
        radius = 0;
        angleDegree = 360;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 0);
        QCOMPARE(angleDegree, 0);
        radius = 5;
        angleDegree = 360;
        normalizePolar360(radius, angleDegree);
        QCOMPARE(radius, 5);
        QCOMPARE(angleDegree, 0);
    }

    void testRoundToNearestMultiple()
    {
        QCOMPARE(roundToNearestMultiple(-4.6, 3), -6);
        QCOMPARE(roundToNearestMultiple(-4.4, 3), -3);
        QCOMPARE(roundToNearestMultiple(-1.6, 3), -3);
        QCOMPARE(roundToNearestMultiple(-1.4, 3), 0);
        QCOMPARE(roundToNearestMultiple(1.4, 3), 0);
        QCOMPARE(roundToNearestMultiple(1.6, 3), 3);
        QCOMPARE(roundToNearestMultiple(4.4, 3), 3);
        QCOMPARE(roundToNearestMultiple(4.6, 3), 6);

        QCOMPARE(roundToNearestMultiple(-4.6, 1), -5);
        QCOMPARE(roundToNearestMultiple(-4.4, 1), -4);
        QCOMPARE(roundToNearestMultiple(-1.6, 1), -2);
        QCOMPARE(roundToNearestMultiple(-1.4, 1), -1);
        QCOMPARE(roundToNearestMultiple(1.4, 1), 1);
        QCOMPARE(roundToNearestMultiple(1.6, 1), 2);
        QCOMPARE(roundToNearestMultiple(4.4, 1), 4);
        QCOMPARE(roundToNearestMultiple(4.6, 1), 5);

        QCOMPARE(roundToNearestMultiple(4.6, 0), 5);

        QCOMPARE(roundToNearestMultiple(-4.6, -3), -6);
        QCOMPARE(roundToNearestMultiple(-4.4, -3), -3);
        QCOMPARE(roundToNearestMultiple(-1.6, -3), -3);
        QCOMPARE(roundToNearestMultiple(-1.4, -3), 0);
        QCOMPARE(roundToNearestMultiple(1.4, -3), 0);
        QCOMPARE(roundToNearestMultiple(1.6, -3), 3);
        QCOMPARE(roundToNearestMultiple(4.4, -3), 3);
        QCOMPARE(roundToNearestMultiple(4.6, -3), 6);

        QCOMPARE(roundToNearestMultiple(-4.6, -1), -5);
        QCOMPARE(roundToNearestMultiple(-4.4, -1), -4);
        QCOMPARE(roundToNearestMultiple(-1.6, -1), -2);
        QCOMPARE(roundToNearestMultiple(-1.4, -1), -1);
        QCOMPARE(roundToNearestMultiple(1.4, -1), 1);
        QCOMPARE(roundToNearestMultiple(1.6, -1), 2);
        QCOMPARE(roundToNearestMultiple(4.4, -1), 4);
        QCOMPARE(roundToNearestMultiple(4.6, -1), 5);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperMath)
// The following “include” is necessary because we do not use a header file:
#include "testhelpermath.moc"
