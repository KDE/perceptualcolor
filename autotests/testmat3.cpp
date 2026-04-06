// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "mat3.h"

#include "blackhole.h"
#include <array>
#include <qgenericmatrix.h>
#include <qtest.h>
#include <qtestcase.h>
#include <type_traits>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestMat3 : public QObject
{
    Q_OBJECT

public:
    explicit TestMat3(QObject *parent = nullptr)
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

    void testDefaultConstructor()
    {
        const Mat3<float> v1;
        QCOMPARE(v1(0, 0), 0);
        QCOMPARE(v1(0, 1), 0);
        QCOMPARE(v1(0, 2), 0);
        QCOMPARE(v1(1, 0), 0);
        QCOMPARE(v1(1, 1), 0);
        QCOMPARE(v1(1, 2), 0);
        QCOMPARE(v1(2, 0), 0);
        QCOMPARE(v1(2, 1), 0);
        QCOMPARE(v1(2, 2), 0);
        const Mat3<double> v2;
        QCOMPARE(v2(0, 0), 0);
        QCOMPARE(v2(0, 1), 0);
        QCOMPARE(v2(0, 2), 0);
        QCOMPARE(v2(1, 0), 0);
        QCOMPARE(v2(1, 1), 0);
        QCOMPARE(v2(1, 2), 0);
        QCOMPARE(v2(2, 0), 0);
        QCOMPARE(v2(2, 1), 0);
        QCOMPARE(v2(2, 2), 0);
        const Mat3<long double> v3;
        QCOMPARE(v3(0, 0), 0);
        QCOMPARE(v3(0, 1), 0);
        QCOMPARE(v3(0, 2), 0);
        QCOMPARE(v3(1, 0), 0);
        QCOMPARE(v3(1, 1), 0);
        QCOMPARE(v3(1, 2), 0);
        QCOMPARE(v3(2, 0), 0);
        QCOMPARE(v3(2, 1), 0);
        QCOMPARE(v3(2, 2), 0);
        constexpr Mat3<float> v4;
        static_assert(v4(0, 0) == 0);
        static_assert(v4(0, 1) == 0);
        static_assert(v4(0, 2) == 0);
        static_assert(v4(1, 0) == 0);
        static_assert(v4(1, 1) == 0);
        static_assert(v4(1, 2) == 0);
        static_assert(v4(2, 0) == 0);
        static_assert(v4(2, 1) == 0);
        static_assert(v4(2, 2) == 0);
        constexpr Mat3<double> v5;
        static_assert(v5(0, 0) == 0);
        static_assert(v5(0, 1) == 0);
        static_assert(v5(0, 2) == 0);
        static_assert(v5(1, 0) == 0);
        static_assert(v5(1, 1) == 0);
        static_assert(v5(1, 2) == 0);
        static_assert(v5(2, 0) == 0);
        static_assert(v5(2, 1) == 0);
        static_assert(v5(2, 2) == 0);
        constexpr Mat3<long double> v6;
        static_assert(v6(0, 0) == 0);
        static_assert(v6(0, 1) == 0);
        static_assert(v6(0, 2) == 0);
        static_assert(v6(1, 0) == 0);
        static_assert(v6(1, 1) == 0);
        static_assert(v6(1, 2) == 0);
        static_assert(v6(2, 0) == 0);
        static_assert(v6(2, 1) == 0);
        static_assert(v6(2, 2) == 0);
    }

    void testConstructorAndCallOperator()
    {
        const Mat3<float> v1(1, 2, 3, 4, 5, 6, 7, 8, 9);
        QCOMPARE(v1(0, 0), 1);
        QCOMPARE(v1(0, 1), 2);
        QCOMPARE(v1(0, 2), 3);
        QCOMPARE(v1(1, 0), 4);
        QCOMPARE(v1(1, 1), 5);
        QCOMPARE(v1(1, 2), 6);
        QCOMPARE(v1(2, 0), 7);
        QCOMPARE(v1(2, 1), 8);
        QCOMPARE(v1(2, 2), 9);
        const Mat3<double> v2(1, 2, 3, 4, 5, 6, 7, 8, 9);
        QCOMPARE(v2(0, 0), 1);
        QCOMPARE(v2(0, 1), 2);
        QCOMPARE(v2(0, 2), 3);
        QCOMPARE(v2(1, 0), 4);
        QCOMPARE(v2(1, 1), 5);
        QCOMPARE(v2(1, 2), 6);
        QCOMPARE(v2(2, 0), 7);
        QCOMPARE(v2(2, 1), 8);
        QCOMPARE(v2(2, 2), 9);
        const Mat3<long double> v3(1, 2, 3, 4, 5, 6, 7, 8, 9);
        QCOMPARE(v3(0, 0), 1);
        QCOMPARE(v3(0, 1), 2);
        QCOMPARE(v3(0, 2), 3);
        QCOMPARE(v3(1, 0), 4);
        QCOMPARE(v3(1, 1), 5);
        QCOMPARE(v3(1, 2), 6);
        QCOMPARE(v3(2, 0), 7);
        QCOMPARE(v3(2, 1), 8);
        QCOMPARE(v3(2, 2), 9);
        constexpr Mat3<float> v4(1, 2, 3, 4, 5, 6, 7, 8, 9);
        QCOMPARE(v4(0, 0), 1);
        QCOMPARE(v4(0, 1), 2);
        QCOMPARE(v4(0, 2), 3);
        QCOMPARE(v4(1, 0), 4);
        QCOMPARE(v4(1, 1), 5);
        QCOMPARE(v4(1, 2), 6);
        QCOMPARE(v4(2, 0), 7);
        QCOMPARE(v4(2, 1), 8);
        QCOMPARE(v4(2, 2), 9);
        constexpr Mat3<double> v5(1, 2, 3, 4, 5, 6, 7, 8, 9);
        QCOMPARE(v5(0, 0), 1);
        QCOMPARE(v5(0, 1), 2);
        QCOMPARE(v5(0, 2), 3);
        QCOMPARE(v5(1, 0), 4);
        QCOMPARE(v5(1, 1), 5);
        QCOMPARE(v5(1, 2), 6);
        QCOMPARE(v5(2, 0), 7);
        QCOMPARE(v5(2, 1), 8);
        QCOMPARE(v5(2, 2), 9);
        constexpr Mat3<long double> v6(1, 2, 3, 4, 5, 6, 7, 8, 9);
        QCOMPARE(v6(0, 0), 1);
        QCOMPARE(v6(0, 1), 2);
        QCOMPARE(v6(0, 2), 3);
        QCOMPARE(v6(1, 0), 4);
        QCOMPARE(v6(1, 1), 5);
        QCOMPARE(v6(1, 2), 6);
        QCOMPARE(v6(2, 0), 7);
        QCOMPARE(v6(2, 1), 8);
        QCOMPARE(v6(2, 2), 9);

        Mat3<long double> v7(1, 2, 3, 4, 5, 6, 7, 8, 9);
        v7(2, 1) = 10;
        QCOMPARE(v7(2, 1), 10);
    }

    void testCast()
    {
        constexpr Mat3f myFloat(1, 2, 3, 4, 5, 6, 7, 8, 9);
        constexpr Mat3d myDouble(1, 2, 3, 4, 5, 6, 7, 8, 9);
        constexpr Mat3ld myLongDouble(1, 2, 3, 4, 5, 6, 7, 8, 9);

        constexpr Mat3d f_d = static_cast<Mat3d>(myFloat);
        constexpr Mat3ld f_ld = static_cast<Mat3ld>(myFloat);

        constexpr Mat3f d_f = static_cast<Mat3f>(myDouble);
        constexpr Mat3ld d_ld = static_cast<Mat3ld>(myDouble);

        constexpr Mat3f ld_f = static_cast<Mat3f>(myLongDouble);
        constexpr Mat3d ld_d = static_cast<Mat3d>(myLongDouble);

        Q_UNUSED(f_d)
        Q_UNUSED(f_ld)
        Q_UNUSED(d_f)
        Q_UNUSED(d_ld)
        Q_UNUSED(ld_f)
        Q_UNUSED(ld_d)
    }

    void testQDebug()
    {
        constexpr Mat3f myFloat( //
            1.00000f,
            2.00001f,
            3.f,
            1.f,
            2.000001f,
            355555.1f,
            1.f,
            32.23f,
            3.f);
        constexpr Mat3d myDouble( //
            1.00000,
            2.00001,
            3.,
            1.,
            2.000001,
            355555.1,
            1.,
            32.23,
            3.);
        constexpr Mat3ld myLongDouble( //
            1.00000,
            2.00001,
            3.,
            1.,
            2.000001,
            355555.1,
            1.,
            32.23,
            3.);

        // Suppress warnings
        qInstallMessageHandler(voidMessageHandler);

        qDebug() << myFloat;
        qDebug() << myDouble;
        qDebug() << myLongDouble;

        // Do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testMultiplicationWithMatrix()
    {
        constexpr Mat3d myMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const QGenericMatrix<3, 3, double> myQMatrix(std::array<double, 9>{{1, 2, 3, 4, 5, 6, 7, 8, 9}}.data());
        constexpr Mat3d myMatrix2(11, 12, 13, 14, 15, 16, 17, 18, 19);
        const QGenericMatrix<3, 3, double> myQMatrix2(std::array<double, 9>{{11, 12, 13, 14, 15, 16, 17, 18, 19}}.data());
        constexpr Mat3d multiplication = myMatrix * myMatrix2;
        const QGenericMatrix<3, 3, double> qMultiplication = myQMatrix * myQMatrix2;

        QCOMPARE(multiplication(0, 0), qMultiplication(0, 0));
        QCOMPARE(multiplication(0, 1), qMultiplication(0, 1));
        QCOMPARE(multiplication(0, 2), qMultiplication(0, 2));
        QCOMPARE(multiplication(1, 0), qMultiplication(1, 0));
        QCOMPARE(multiplication(1, 1), qMultiplication(1, 1));
        QCOMPARE(multiplication(1, 2), qMultiplication(1, 2));
        QCOMPARE(multiplication(2, 0), qMultiplication(2, 0));
        QCOMPARE(multiplication(2, 1), qMultiplication(2, 1));
        QCOMPARE(multiplication(2, 2), qMultiplication(2, 2));
    }

    void testMultiplicationWithVector()
    {
        constexpr Mat3d myMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
        const QGenericMatrix<3, 3, double> myQMatrix(std::array<double, 9>{{1, 2, 3, 4, 5, 6, 7, 8, 9}}.data());
        constexpr Vec3d myMatrix2(11, 12, 13);
        const QGenericMatrix<1, 3, double> myQMatrix2(std::array<double, 3>{{11, 12, 13}}.data());
        constexpr Vec3d multiplication = myMatrix * myMatrix2;
        const QGenericMatrix<1, 3, double> qMultiplication = myQMatrix * myQMatrix2;

        QCOMPARE(multiplication(0), qMultiplication(0, 0));
        QCOMPARE(multiplication(1), qMultiplication(1, 0));
        QCOMPARE(multiplication(2), qMultiplication(2, 0));
    }

    void testDeterminant()
    {
        constexpr Mat3d myMatrix(-2, -1, 2, 2, 1, 4, -3, 3, -1);
        QCOMPARE(myMatrix.determinant(), 54);
    }

    void testInverseMatrixFromNonInvertible()
    {
        // clang-format off
        constexpr Mat3d myMatrix(
            1, 2, 3, //
            4, 5, 6, //
            7, 8, 9);
        // clang-format on
        constexpr auto inverse = myMatrix.inverse();
        QVERIFY(!inverse.has_value());
    }

    void testInverseMatrixFromInvertible()
    {
        // clang-format off
        constexpr Mat3d myMatrix(
            1, 2, 1, //
            0, 1, 3, //
            -1, 0, 1);
        // clang-format on
        constexpr auto actualInverse = myMatrix.inverse().value();
        // clang-format off
        constexpr Mat3d expectedInverse (
            -0.25, 0.5, -1.25,
            0.75, -0.5, 0.75,
            -0.25, 0.5, -0.25);
        // clang-format on
        QCOMPARE(actualInverse(0, 0), expectedInverse(0, 0));
        QCOMPARE(actualInverse(0, 1), expectedInverse(0, 1));
        QCOMPARE(actualInverse(0, 2), expectedInverse(0, 2));
        QCOMPARE(actualInverse(1, 0), expectedInverse(1, 0));
        QCOMPARE(actualInverse(1, 1), expectedInverse(1, 1));
        QCOMPARE(actualInverse(1, 2), expectedInverse(1, 2));
        QCOMPARE(actualInverse(2, 0), expectedInverse(2, 0));
        QCOMPARE(actualInverse(2, 1), expectedInverse(2, 1));
        QCOMPARE(actualInverse(2, 2), expectedInverse(2, 2));
    }

    void testBenchmark()
    {
        constexpr Mat3d myMatrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
        Vec3d result(11, 12, 13);
        QBENCHMARK {
            for (long int i = 0; i < 100000000; ++i) {
                result = myMatrix * result;
            }
            blackhole(result);
        };
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestMat3)

// The following “include” is necessary because we do not use a header file:
#include "testmat3.moc"
