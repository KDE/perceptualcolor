// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helperconversion.h"
#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qtmetamacros.h>

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

    void testFromFloatingToEightBit()
    {
        constexpr quint8 value = fromFloatingToEightBit(1.0);
        static_assert(value == 255);
    }

    void testToRgb()
    {
        QVERIFY(!toRgbExact(QColor()).isValid());
        QCOMPARE(toRgbExact(Qt::red).spec(), QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromHsl(1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromHslF(1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromHsv(1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromHsvF(1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromCmyk(1, 1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromCmykF(1, 1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromRgb(1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE(toRgbExact(QColor::fromRgbF(1, 1, 1)).spec(), //
                 QColor::Spec::Rgb);
        QCOMPARE( // QColor::Spec::ExtendedRgb
            toRgbExact(QColor::fromRgbF(2, 2, 2)).spec(),
            QColor::Spec::Rgb);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperConversion)
// The following “include” is necessary because we do not use a header file:
#include "testhelperconversion.moc"
