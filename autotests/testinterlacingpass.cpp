﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "interlacingpass.h"

#include <qglobal.h>
#include <qobject.h>
#include <qsize.h>
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
class TestInterlacingPass : public QObject
{
    Q_OBJECT

public:
    explicit TestInterlacingPass(QObject *parent = nullptr)
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

    void testConstructorDestructor()
    {
        QVERIFY_EXCEPTION_THROWN(InterlacingPass test{2}, int);
        QVERIFY_EXCEPTION_THROWN(InterlacingPass test{0}, int);
        QVERIFY_EXCEPTION_THROWN(InterlacingPass test{-0}, int);
        QVERIFY_EXCEPTION_THROWN(InterlacingPass test{-1}, int);
        QVERIFY_EXCEPTION_THROWN(InterlacingPass test{-2}, int);
        InterlacingPass testA{3};
        InterlacingPass testB{5};
        InterlacingPass testC{7};
        InterlacingPass testD{9};
        InterlacingPass testE{11};
    }

    void testMake()
    {
        InterlacingPass testA = InterlacingPass::make<3>();
        InterlacingPass testB = InterlacingPass::make<5>();
        InterlacingPass testC = InterlacingPass::make<7>();
        InterlacingPass testD = InterlacingPass::make<9>();
        InterlacingPass testE = InterlacingPass::make<11>();
        Q_UNUSED(testA);
        Q_UNUSED(testB);
        Q_UNUSED(testC);
        Q_UNUSED(testD);
        Q_UNUSED(testE);
    }

    void testAdam3()
    {
        InterlacingPass test = InterlacingPass::make<3>();
        QCOMPARE(test.countdown, 3);
        QCOMPARE(test.rectangleSize.width(), 2);
        QCOMPARE(test.rectangleSize.height(), 2);
        QCOMPARE(test.columnFrequency, 2);
        QCOMPARE(test.columnOffset, 0);
        QCOMPARE(test.lineFrequency, 2);
        QCOMPARE(test.lineOffset, 0);

        test.switchToNextPass();
        QCOMPARE(test.countdown, 2);
        QCOMPARE(test.rectangleSize.width(), 1);
        QCOMPARE(test.rectangleSize.height(), 2);
        QCOMPARE(test.columnFrequency, 2);
        QCOMPARE(test.columnOffset, 1);
        QCOMPARE(test.lineFrequency, 2);
        QCOMPARE(test.lineOffset, 0);

        test.switchToNextPass();
        QCOMPARE(test.countdown, 1);
        QCOMPARE(test.rectangleSize.width(), 1);
        QCOMPARE(test.rectangleSize.height(), 1);
        QCOMPARE(test.columnFrequency, 1);
        QCOMPARE(test.columnOffset, 0);
        QCOMPARE(test.lineFrequency, 2);
        QCOMPARE(test.lineOffset, 1);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestInterlacingPass)

// The following “include” is necessary because we do not use a header file:
#include "testinterlacingpass.moc"
