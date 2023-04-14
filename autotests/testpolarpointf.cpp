﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "polarpointf.h"

#include <cmath>
#include <qdebug.h>
#include <qglobal.h>
#include <qobject.h>
#include <qpoint.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qstring.h>
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestPolarPointF : public QObject
{
    Q_OBJECT

public:
    explicit TestPolarPointF(QObject *parent = nullptr)
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

    void testConstructorRadialAngle()
    {
        PerceptualColor::PolarPointF temp01;

        // Default value is 0, 0°
        temp01 = PerceptualColor::PolarPointF();
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 0);

        // Same for initialization with 0, 0
        temp01 = PerceptualColor::PolarPointF(0, 0);
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 0);

        // Yet normalized values are taken as-is
        temp01 = PerceptualColor::PolarPointF(2, 3);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 3);

        // Negative radials are normalized (180° shift for angle)
        temp01 = PerceptualColor::PolarPointF(-2, 183);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 3);

        // Out-of-range angle is normalized
        temp01 = PerceptualColor::PolarPointF(2, 363);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 3);

        temp01 = PerceptualColor::PolarPointF(2, -357);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 3);

        // Also when both radial and angle are
        // out-of-range, normalization works
        temp01 = PerceptualColor::PolarPointF(-2, -357);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 183);
        temp01 = PerceptualColor::PolarPointF(-2, -717);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 183);
        temp01 = PerceptualColor::PolarPointF(-2, 363);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 183);
        temp01 = PerceptualColor::PolarPointF(-2, 723);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 183);

        // When radial is 0, angle (while meaningless) is
        // preserved (but normalized)
        temp01 = PerceptualColor::PolarPointF(0, 150);
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 150);
        temp01 = PerceptualColor::PolarPointF(0, 370);
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 10);

        // Edge case: 360°
        temp01 = PerceptualColor::PolarPointF(0, 360);
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 0);
        temp01 = PerceptualColor::PolarPointF(5, 360);
        QCOMPARE(temp01.radius(), 5);
        QCOMPARE(temp01.angleDegree(), 0);
    }

    void testCopyAndAsignmentConstructor()
    {
        PerceptualColor::PolarPointF temp01;
        PerceptualColor::PolarPointF temp02;
        PerceptualColor::PolarPointF temp03;

        temp01 = PerceptualColor::PolarPointF();
        temp02 = temp01;
        temp03 = PerceptualColor::PolarPointF(temp01);
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp02.radius(), 0);
        QCOMPARE(temp03.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 0);
        QCOMPARE(temp02.angleDegree(), 0);
        QCOMPARE(temp03.angleDegree(), 0);

        temp01 = PerceptualColor::PolarPointF(0, 0);
        temp02 = temp01;
        temp03 = PerceptualColor::PolarPointF(temp01);
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp02.radius(), 0);
        QCOMPARE(temp03.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 0);
        QCOMPARE(temp02.angleDegree(), 0);
        QCOMPARE(temp03.angleDegree(), 0);

        temp01 = PerceptualColor::PolarPointF(-2, 723);
        temp02 = temp01;
        temp03 = PerceptualColor::PolarPointF(temp01);
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp02.radius(), 2);
        QCOMPARE(temp03.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 183);
        QCOMPARE(temp02.angleDegree(), 183);
        QCOMPARE(temp03.angleDegree(), 183);
    }

    void testConstructorCartesian()
    {
        PerceptualColor::PolarPointF temp01;
        temp01 = PerceptualColor::PolarPointF(QPointF(0, 0));
        QCOMPARE(temp01.radius(), 0);
        QCOMPARE(temp01.angleDegree(), 0);
        temp01 = PerceptualColor::PolarPointF(QPointF(2, 0));
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 0);
        temp01 = PerceptualColor::PolarPointF(QPointF(2, 2));
        QCOMPARE(temp01.radius(), sqrt(8));
        QCOMPARE(temp01.angleDegree(), static_cast<qreal>(45));
        temp01 = PerceptualColor::PolarPointF(QPointF(0, 2));
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 90);
        temp01 = PerceptualColor::PolarPointF(QPointF(-2, 2));
        QCOMPARE(temp01.radius(), sqrt(8));
        QCOMPARE(temp01.angleDegree(), static_cast<qreal>(135));
        temp01 = PerceptualColor::PolarPointF(QPointF(-2, 0));
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 180);
        temp01 = PerceptualColor::PolarPointF(QPointF(-2, -2));
        QCOMPARE(temp01.radius(), sqrt(8));
        QCOMPARE(temp01.angleDegree(), static_cast<qreal>(225));
        temp01 = PerceptualColor::PolarPointF(QPointF(0, -2));
        QCOMPARE(temp01.radius(), 2);
        QCOMPARE(temp01.angleDegree(), 270);
        temp01 = PerceptualColor::PolarPointF(QPointF(2, -2));
        QCOMPARE(temp01.radius(), sqrt(8));
        QCOMPARE(temp01.angleDegree(), static_cast<qreal>(315));
    }

    void testIsSamePoint()
    {
        QVERIFY(!PerceptualColor::PolarPointF(0, 0).isSamePoint(PerceptualColor::PolarPointF(1, 0)));

        // If radial is 0, different angle still means same point
        QVERIFY(PerceptualColor::PolarPointF(0, 0).isSamePoint(PerceptualColor::PolarPointF(0, 500)));

        QVERIFY(PerceptualColor::PolarPointF(0, 0).isSamePoint(PerceptualColor::PolarPointF(0, 300)));

        QVERIFY(PerceptualColor::PolarPointF(0, 0).isSamePoint(PerceptualColor::PolarPointF(0, -500)));

        QVERIFY(!PerceptualColor::PolarPointF(1, 320).isSamePoint(PerceptualColor::PolarPointF(1, 321)));

        QVERIFY(PerceptualColor::PolarPointF(5, 90).isSamePoint(PerceptualColor::PolarPointF(-5, 270)));

        QVERIFY(PerceptualColor::PolarPointF(5, 450).isSamePoint(PerceptualColor::PolarPointF(-5, -90)));
    }

    void testToCartesian()
    {
        QCOMPARE(PerceptualColor::PolarPointF(0, 0).toCartesian(), QPointF(0, 0));
        QCOMPARE(PerceptualColor::PolarPointF(0, 90).toCartesian(), QPointF(0, 0));
        QCOMPARE(PerceptualColor::PolarPointF(0, 361).toCartesian(), QPointF(0, 0));
        QCOMPARE(PerceptualColor::PolarPointF(0, -1).toCartesian(), QPointF(0, 0));
        QCOMPARE(PerceptualColor::PolarPointF(1, 0).toCartesian(), QPointF(1, 0));
        QCOMPARE(PerceptualColor::PolarPointF(1, 90).toCartesian(), QPointF(0, 1));
        QCOMPARE(PerceptualColor::PolarPointF(1, 180).toCartesian(), QPointF(-1, 0));
        QCOMPARE(PerceptualColor::PolarPointF(1, 270).toCartesian(), QPointF(0, -1));
        QCOMPARE(PerceptualColor::PolarPointF(1, 360).toCartesian(), QPointF(1, 0));
        QCOMPARE(PerceptualColor::PolarPointF(1, 720).toCartesian(), QPointF(1, 0));
        QCOMPARE(PerceptualColor::PolarPointF(1, -360).toCartesian(), QPointF(1, 0));
        QCOMPARE(PerceptualColor::PolarPointF(sqrt(8), 45).toCartesian(), QPointF(2, 2));
    }

    void testMetaType()
    {
        PerceptualColor::PolarPointF temp01(17, 18);
        QVariant var;
        // This following line should throw a compile-time error if meta-type
        // is not declared:
        var.setValue(temp01);
        QVERIFY(var.value<PerceptualColor::PolarPointF>().isSamePoint(temp01));
    }

    void testDebug()
    {
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Test if QDebug support does not make a crash.
        qDebug() << PolarPointF();
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void testMetaTypeDeclaration()
    {
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(PolarPointF());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestPolarPointF)
// The following “include” is necessary because we do not use a header file:
#include "testpolarpointf.moc"
