﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "rgbdouble.h"

#include <lcms2.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qobject.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

static cmsCIELab snippet01()
{
    //! [Use RgbDouble]
    cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(nullptr);
    cmsHPROFILE rgbProfileHandle = cmsCreate_sRGBProfile();
    cmsHTRANSFORM m_transformRgbToLabHandle = cmsCreateTransform(
        // Input profile handle:
        rgbProfileHandle,
        // Input buffer format:
        TYPE_RGB_DBL,
        // Output profile handle:
        labProfileHandle,
        // Output buffer format:
        TYPE_Lab_DBL,
        // Rendering intent:
        INTENT_ABSOLUTE_COLORIMETRIC,
        // Flags:
        0);
    cmsCloseProfile(labProfileHandle);
    cmsCloseProfile(rgbProfileHandle);
    PerceptualColor::RgbDouble rgb;
    rgb.red = 1;
    rgb.green = 0.5;
    rgb.blue = 0;
    cmsCIELab lab;
    // Convert exactly 1 value:
    cmsDoTransform(m_transformRgbToLabHandle, // transform handle
                   &rgb, // input buffer
                   &lab, // output buffer
                   1 // number of RGB values to convert
    );
    cmsDeleteTransform(m_transformRgbToLabHandle);
    //! [Use RgbDouble]
    return lab;
}

namespace PerceptualColor
{
class TestRgbDouble : public QObject
{
    Q_OBJECT

public:
    explicit TestRgbDouble(QObject *parent = nullptr)
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
        RgbDouble test;
        test.red = 0.5;
        Q_UNUSED(test)
    }

    void testCopyConstructor()
    {
        // This should not crash.
        RgbDouble test;
        test.red = 0.5;
        test.green = 0.6;
        test.blue = 0.7;
        RgbDouble copy(test);
        QCOMPARE(copy.red, 0.5);
        QCOMPARE(copy.green, 0.6);
        QCOMPARE(copy.blue, 0.7);
    }

    void testQDebugSupport()
    {
        PerceptualColor::RgbDouble test;
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);
        qDebug() << test;
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void testRgbDouble()
    {
        cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(nullptr);
        cmsHPROFILE rgbProfileHandle = cmsCreate_sRGBProfile();
        cmsHTRANSFORM m_transformLabToRgbHandle = cmsCreateTransform(labProfileHandle, // input profile handle
                                                                     TYPE_Lab_DBL, // input buffer format
                                                                     rgbProfileHandle, // output profile handle
                                                                     TYPE_RGB_DBL, // output buffer format
                                                                     INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
                                                                     0 // flags
        );
        cmsCloseProfile(labProfileHandle);
        cmsCloseProfile(rgbProfileHandle);
        PerceptualColor::RgbDouble rgb;
        cmsCIELab lab;
        lab.L = 50;
        lab.a = 0;
        lab.b = 0;
        // Test if the following line does not produce
        // a memory error on the heap.
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        // Test if the result is okay (so it has to be neutral gray: red,
        // green and blue should be roughly the same)
        QCOMPARE(qRound(rgb.red * 255), qRound(rgb.blue * 255));
        QCOMPARE(qRound(rgb.green * 255), qRound(rgb.blue * 255));
        // Test if Red, Green, Blue are at the correct position in memory
        lab.L = 53;
        lab.a = 80;
        lab.b = 67;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(rgb.red > 0.8, "Test if Red is at the correct position in memory");
        lab.L = 87;
        lab.a = -86;
        lab.b = 83;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(rgb.green > 0.8, "Test if Green is at the correct position in memory");
        lab.L = 32;
        lab.a = 79;
        lab.b = -107;
        // Convert exactly 1 value.
        cmsDoTransform(m_transformLabToRgbHandle, &lab, &rgb, 1);
        QVERIFY2(rgb.blue > 0.8, "Test if Blue is at the correct position in memory");

        // Clean up
        cmsDeleteTransform(m_transformLabToRgbHandle);
    }

    void testMetaTypeDeclaration()
    {
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(RgbDouble());
    }

    void testSnippet01()
    {
        cmsCIELab lab = snippet01();
        constexpr int tolerance = 5;
        constexpr int expectedL = 68;
        constexpr int expectedA = 46;
        constexpr int expectedB = 75;
        QVERIFY2((expectedL - tolerance) < lab.L, "Verify that hue is within tolerance.");
        QVERIFY2(lab.L < (expectedL + tolerance), "Verify that hue is within tolerance.");
        QVERIFY2((expectedA - tolerance) < lab.a, "Verify that lightness is within tolerance.");
        QVERIFY2(lab.a < (expectedA + tolerance), "Verify that lightness is within tolerance.");
        QVERIFY2((expectedB - tolerance) < lab.b, "Verify that chroma is within tolerance.");
        QVERIFY2(lab.b < (expectedB + tolerance), "Verify that chroma is within tolerance.");
    }
};

} // input profile

QTEST_MAIN(PerceptualColor::TestRgbDouble)

// The following “include” is necessary because we do not use a header file:
#include "testrgbdouble.moc"
