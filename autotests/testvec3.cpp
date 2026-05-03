// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "vec3.h"

#include <qdebug.h>
#include <qglobal.h>
#include <qobject.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
class TestVec3 : public QObject
{
    Q_OBJECT

public:
    explicit TestVec3(QObject *parent = nullptr)
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
        const Vec3<float> v1;
        QCOMPARE(v1(0), 0);
        QCOMPARE(v1(1), 0);
        QCOMPARE(v1(2), 0);
        const Vec3<double> v2;
        QCOMPARE(v2(0), 0);
        QCOMPARE(v2(1), 0);
        QCOMPARE(v2(2), 0);
        const Vec3<long double> v3;
        QCOMPARE(v3(0), 0);
        QCOMPARE(v3(1), 0);
        QCOMPARE(v3(2), 0);
        constexpr Vec3<float> v4;
        static_assert(v4(0) == 0);
        static_assert(v4(1) == 0);
        static_assert(v4(2) == 0);
        constexpr Vec3<double> v5;
        static_assert(v5(0) == 0);
        static_assert(v5(1) == 0);
        static_assert(v5(2) == 0);
        constexpr Vec3<long double> v6;
        static_assert(v6(0) == 0);
        static_assert(v6(1) == 0);
        static_assert(v6(2) == 0);
    }

    void testConstructorAndCallOperator()
    {
        const Vec3<float> v1(1, 2, 3);
        QCOMPARE(v1(0), 1);
        QCOMPARE(v1(1), 2);
        QCOMPARE(v1(2), 3);
        const Vec3<double> v2(1, 2, 3);
        QCOMPARE(v2(0), 1);
        QCOMPARE(v2(1), 2);
        QCOMPARE(v2(2), 3);
        const Vec3<long double> v3(1, 2, 3);
        QCOMPARE(v3(0), 1);
        QCOMPARE(v3(1), 2);
        QCOMPARE(v3(2), 3);
        constexpr Vec3<float> v4(1, 2, 3);
        QCOMPARE(v4(0), 1);
        QCOMPARE(v4(1), 2);
        QCOMPARE(v4(2), 3);
        constexpr Vec3<double> v5(1, 2, 3);
        QCOMPARE(v5(0), 1);
        QCOMPARE(v5(1), 2);
        QCOMPARE(v5(2), 3);
        constexpr Vec3<long double> v6(1, 2, 3);
        QCOMPARE(v6(0), 1);
        QCOMPARE(v6(1), 2);
        QCOMPARE(v6(2), 3);

        Vec3<long double> v7(1, 2, 3);
        v7(2) = 5;
        QCOMPARE(v7(2), 5);
    }

    void testCast()
    {
        constexpr Vec3f myFloat(1, 2, 3);
        constexpr Vec3d myDouble(1, 2, 3);
        constexpr Vec3ld myLongDouble(1, 2, 3);

        constexpr Vec3d f_d = static_cast<Vec3d>(myFloat);
        constexpr Vec3ld f_ld = static_cast<Vec3ld>(myFloat);

        constexpr Vec3f d_f = static_cast<Vec3f>(myDouble);
        constexpr Vec3ld d_ld = static_cast<Vec3ld>(myDouble);

        constexpr Vec3f ld_f = static_cast<Vec3f>(myLongDouble);
        constexpr Vec3d ld_d = static_cast<Vec3d>(myLongDouble);

        Q_UNUSED(f_d)
        Q_UNUSED(f_ld)
        Q_UNUSED(d_f)
        Q_UNUSED(d_ld)
        Q_UNUSED(ld_f)
        Q_UNUSED(ld_d)
    }

    void testQDebug()
    {
        constexpr Vec3f myFloat(1.00000f, 2.00001f, 3.f);
        constexpr Vec3d myDouble(-1, 0, 355555.1);
        constexpr Vec3ld myLongDouble(1, 32.23, 3);

        // Suppress warnings
        qInstallMessageHandler(voidMessageHandler);

        qDebug() << myFloat;
        qDebug() << myDouble;
        qDebug() << myLongDouble;

        // Do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestVec3)

// The following “include” is necessary because we do not use a header file:
#include "testvec3.moc"
