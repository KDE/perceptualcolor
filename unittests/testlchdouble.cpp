// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/lchdouble.h"

#include <qdebug.h>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvariant.h>

static void snippet01()
{
    //! [Use LchDouble]
    PerceptualColor::LchDouble myValue;
    myValue.l = 50; // Lightness: 50%
    myValue.c = 25; // Chroma: 25
    myValue.h = 5; // Hue: 5°
    //! [Use LchDouble]
    Q_UNUSED(myValue)
}

namespace PerceptualColor
{
class TestLchDouble : public QObject
{
    Q_OBJECT

public:
    TestLchDouble(QObject *parent = nullptr)
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

    void testConstructorDestructor()
    {
        // This should not crash.
        LchDouble test;
        test.l = 50;
        Q_UNUSED(test)
    }

    void testCopyConstructor()
    {
        // This should not crash.
        LchDouble test;
        test.l = 50;
        test.c = 25;
        test.h = 5;
        LchDouble copy(test);
        QCOMPARE(copy.l, 50);
        QCOMPARE(copy.c, 25);
        QCOMPARE(copy.h, 5);
    }

    void testHasSameCoordinates()
    {
        LchDouble a;
        a.l = 50;
        a.c = 20;
        a.h = 5;
        LchDouble b = a;
        QVERIFY(a.hasSameCoordinates(b));
        QVERIFY(b.hasSameCoordinates(a));
        QVERIFY(a.hasSameCoordinates(a));
        QVERIFY(b.hasSameCoordinates(b));
        b.h = 365;
        QVERIFY(!a.hasSameCoordinates(b));
        QVERIFY(!b.hasSameCoordinates(a));
        QVERIFY(a.hasSameCoordinates(a));
        QVERIFY(b.hasSameCoordinates(b));
        // When chroma is 0, hue becomes meaningless. Nevertheless, different
        // hues should be detected.
        a.c = 0;
        b.c = 0;
        QVERIFY(!a.hasSameCoordinates(b));
        QVERIFY(!b.hasSameCoordinates(a));
        QVERIFY(a.hasSameCoordinates(a));
        QVERIFY(b.hasSameCoordinates(b));
        // And when returning to the same hue, everything should be considered
        // as with same coordinates.
        b.h = 5;
        QVERIFY(a.hasSameCoordinates(b));
        QVERIFY(b.hasSameCoordinates(a));
        QVERIFY(a.hasSameCoordinates(a));
        QVERIFY(b.hasSameCoordinates(b));
    }

    void testQDebugSupport()
    {
        PerceptualColor::LchDouble test;
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);
        qDebug() << test;
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void testMetaTypeDeclaration()
    {
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(LchDouble());
    }

    void testConstExpr()
    {
        // Test if a constexpr can be instantiated:
        constexpr LchDouble temp = {1, 2, 3};

        // The aggregate initialization order should be l, c, h.
        // Test is this has actually been the case:
        static_assert(temp.l == 1);
        static_assert(temp.c == 2);
        static_assert(temp.h == 3);
    }

    void testSnippet01()
    {
        snippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestLchDouble)

// The following “include” is necessary because we do not use a header file:
#include "testlchdouble.moc"
