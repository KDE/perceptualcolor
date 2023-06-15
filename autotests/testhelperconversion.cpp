// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helperconversion.h"

#include "lchdouble.h"
#include <cmath>
#include <lcms2.h>
#include <qglobal.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

Q_DECLARE_METATYPE(cmsCIELab)

namespace PerceptualColor
{

class TestHelperConversion : public QObject
{
    Q_OBJECT

public:
    explicit TestHelperConversion(QObject *parent = nullptr)
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

    void testLchConvertion()
    {
        // Check if round-trips work fine.
        // One sense
        cmsCIELCh startValue;
        startValue.L = 50.1;
        startValue.C = 20.1;
        startValue.h = 80.1;
        QCOMPARE(toCmsLch(toLchDouble(startValue)).L, startValue.L);
        QCOMPARE(toCmsLch(toLchDouble(startValue)).C, startValue.C);
        QCOMPARE(toCmsLch(toLchDouble(startValue)).h, startValue.h);
        // The other sense
        LchDouble startValue2;
        startValue2.l = 50.1;
        startValue2.c = 20.1;
        startValue2.h = 80.1;
        QCOMPARE(toLchDouble(toCmsLch(startValue2)).l, startValue2.l);
        QCOMPARE(toLchDouble(toCmsLch(startValue2)).c, startValue2.c);
        QCOMPARE(toLchDouble(toCmsLch(startValue2)).h, startValue2.h);
    }

    void testFromXyzToOklabCubeRoot()
    {
        // The function fromXyzToOklab relies on the assumption that
        // std::cbrt() returns negative results for negative radicands,
        // and not simply “nan”. As std::cbrt() isn’t constexpr, we cannot
        // use a static assert within the function. Therefore, we have this
        // unit test:
        const auto actual = std::cbrt(-27);
        const decltype(actual) expected = -3;
        QCOMPARE(actual, expected);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperConversion)
// The following “include” is necessary because we do not use a header file:
#include "testhelperconversion.moc"
