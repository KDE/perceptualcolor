// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "csscolor.h"

#include "genericcolor.h"
#include "helperconversion.h"
#include <helperposixmath.h>
#include <optional>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qrgb.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestCssColor : public QObject
{
    Q_OBJECT

public:
    explicit TestCssColor(QObject *parent = nullptr)
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

    void testParseHexColor()
    {
        QCOMPARE( //
            CssColor::parseHexColor(QString()).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#1")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#12")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#12345")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#1234567")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#123456789")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("123")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("1234")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("123456")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("12345678")).has_value(),
            false);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#123")).value_or(42),
            0xff112233);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#1234")).value_or(42),
            0x44112233);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#123456")).value_or(42),
            0xff123456);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#12345678")).value_or(42),
            0x78123456);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#abcdef78")).value_or(42),
            0x78abcdef);
        QCOMPARE( //
            CssColor::parseHexColor(QStringLiteral("#AbCdeF78")).value_or(42),
            0x78abcdef);
    }

    void testParseNamedColor()
    {
        QCOMPARE(CssColor::parseNamedColor(QString()).has_value(), false);
        QCOMPARE(CssColor::parseNamedColor( //
                     QStringLiteral("invalid"))
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseNamedColor( //
                     QStringLiteral("aliceblue"))
                     .value_or(42),
                 0xfff0f8ff);
        QCOMPARE(CssColor::parseNamedColor( //
                     QStringLiteral("AliceBlue"))
                     .value_or(42),
                 0xfff0f8ff);
        QCOMPARE(CssColor::parseNamedColor( //
                     QStringLiteral("AliceBlueX"))
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseNamedColor( //
                     QStringLiteral("XAliceBlue"))
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseNamedColor( //
                     QStringLiteral("transparent"))
                     .value_or(42),
                 0x00000000);
        QCOMPARE(CssColor::parseNamedColor( //
                     QStringLiteral("TransParent"))
                     .value_or(42),
                 0x00000000);
    }

    void testParseArgumentPercentNumberNone()
    {
        // Empty value
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QString(),
                     255,
                     0)
                     .has_value(),
                 false);

        // none
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     255,
                     0)
                     .value_or(42),
                 0);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     255,
                     1)
                     .value_or(42),
                 1);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     255,
                     255)
                     .value_or(42),
                 255);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral(" none"),
                     255,
                     255)
                     .value_or(42),
                 255);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("none "),
                     255,
                     255)
                     .value_or(42),
                 255);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     0,
                     0)
                     .value_or(42),
                 0);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     -1,
                     0)
                     .value_or(42),
                 0);

        // Number
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56"),
                     255,
                     0)
                     .value_or(42),
                 56);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56"),
                     2,
                     -1)
                     .value_or(42),
                 56);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral(" 56"),
                     255,
                     0)
                     .value_or(42),
                 56);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56 "),
                     255,
                     0)
                     .value_or(42),
                 56);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0"),
                     2,
                     -1)
                     .value_or(42),
                 56);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56,0"),
                     255,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56e1"),
                     2,
                     -1)
                     .value_or(42),
                 560);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0e1"),
                     2,
                     -1)
                     .value_or(42),
                 560);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0e-1"),
                     2,
                     -1)
                     .value_or(42),
                 5.6);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0E-1"),
                     2,
                     -1)
                     .value_or(42),
                 5.6);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("-56.0e-1"),
                     2,
                     -1)
                     .value_or(42),
                 -5.6);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("-56 e1"),
                     255,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("-56"),
                     255,
                     0)
                     .value_or(42),
                 -56);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("- 56"),
                     255,
                     0)
                     .has_value(),
                 false);

        // Percent
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50 %"),
                     300,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50%"),
                     300,
                     0)
                     .value_or(42),
                 150);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50%"),
                     400,
                     -1)
                     .value_or(42),
                 200);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral(" 50%"),
                     200,
                     0)
                     .value_or(42),
                 100);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50% "),
                     200,
                     0)
                     .value_or(42),
                 100);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0%"),
                     200,
                     -1)
                     .value_or(42),
                 100);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50,0%"),
                     200,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50e1%"),
                     200,
                     -1)
                     .value_or(42),
                 1000);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0e1%"),
                     200,
                     -1)
                     .value_or(42),
                 1000);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0e-1%"),
                     200,
                     -1)
                     .value_or(42),
                 10);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0E-1%"),
                     200,
                     -1)
                     .value_or(42),
                 10);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("-50.0e-1%"),
                     200,
                     -1)
                     .value_or(42),
                 -10);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("-50 e1%"),
                     255,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("-50%"),
                     200,
                     0)
                     .value_or(42),
                 -100);
        QCOMPARE(CssColor::parseArgumentPercentNumberNone( //
                     QStringLiteral("- 50%"),
                     200,
                     0)
                     .has_value(),
                 false);
    }

    void testValidateArguments()
    {
        QCOMPARE(CssColor::validateArguments( //
                     QStringList({QStringLiteral("1"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .value(),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3")}));
        QCOMPARE(CssColor::validateArguments( //
                     QStringList({QStringLiteral(" 1"), //
                                  QStringLiteral("2 "),
                                  QStringLiteral(" 3 ")}))
                     .value(),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3")}));
        QCOMPARE(CssColor::validateArguments( //
                     QStringList({QStringLiteral("1 1"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .has_value(),
                 false);
        QCOMPARE(CssColor::validateArguments( //
                     QStringList({QStringLiteral("1,"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .has_value(),
                 false);
        QCOMPARE(CssColor::validateArguments( //
                     QStringList({QStringLiteral("1/"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .has_value(),
                 false);
    }

    void testParseAllFunctionArguments()
    {
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColor::FunctionSyntax::BothSyntaxes,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColor::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     3)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     3)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3"),
                     CssColor::FunctionSyntax::LegacySyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3"),
                     CssColor::FunctionSyntax::BothSyntaxes,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 / 4"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("4")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 / 4"),
                     CssColor::FunctionSyntax::BothSyntaxes,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("4")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 / 4"),
                     CssColor::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3 / 4"),
                     CssColor::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 / 3"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 / 3 4"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 4"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 4 5"),
                     CssColor::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3, 4"),
                     CssColor::FunctionSyntax::LegacySyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("4")}));
        QCOMPARE(CssColor::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3, 4, 5"),
                     CssColor::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
    }

#ifndef MSVC_DLL
    void testParseAbsoluteColorFunction_data()
    {
        QTest::addColumn<QString>("function");
        QTest::addColumn<ColorModel>("model");
        QTest::addColumn<CssColor::CssPredefinedRgbColorSpace>("rgbColorSpace");
        QTest::addColumn<double>("first");
        QTest::addColumn<double>("second");
        QTest::addColumn<double>("third");
        QTest::addColumn<double>("forth");
        QTest::addColumn<double>("alpha1");

        QTest::newRow("rgb(1 2 3)") //
            << QStringLiteral("rgb(1 2 3)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 1.;
        QTest::newRow("rgba(1 2 3)") //
            << QStringLiteral("rgba(1 2 3)") //
            << ColorModel::Invalid //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(1 2 3 / 0.5)") //
            << QStringLiteral("rgb(1 2 3 / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 0.5;
        QTest::newRow("rgb(1 2 / 3)") //
            << QStringLiteral("rgb(1 2 / 3)") //
            << ColorModel::Invalid //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(1, 2, 3, 0.5)") //
            << QStringLiteral("rgb(1, 2, 3, 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 0.5;
        QTest::newRow("rgba(1, 2, 3, 0.5)") //
            << QStringLiteral("rgba(1, 2, 3, 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 0.5;
        QTest::newRow("rgb(100% 50% 0% / 0.5)") //
            << QStringLiteral("rgb(100% 50% 0% / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;
        QTest::newRow("rgb(100% 50% 0% / 50%)") //
            << QStringLiteral("rgb(100% 50% 0% / 50%)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;
        QTest::newRow("rgb(100% 50% 0% / none)") //
            << QStringLiteral("rgb(100% 50% 0% / none)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. << 0. //
            << 1.;
        QTest::newRow("rgb(100% 50% 0% / )") //
            << QStringLiteral("rgb(100% 50% 0% / )") //
            << ColorModel::Invalid //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(100% 50% 0%)") //
            << QStringLiteral("rgb(100% 50% 0%)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. << 0. //
            << 1.;
        QTest::newRow("rgb(none 50% 0%)") //
            << QStringLiteral("rgb(none 50% 0%)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 0. << 0.5 << 0. << 0. //
            << 1.;

        QTest::newRow("color(srgb 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(srgb 100% 50% 0% / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;
        QTest::newRow("color(srgb 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(srgb 0.1 0.2 0.3 / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;

        QTest::newRow("color(srgb-linear 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(srgb-linear 100% 50% 0% / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::SrgbLinear //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;

        QTest::newRow("color(display-p3 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(display-p3 100% 50% 0% / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::DisplayP3 //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;

        QTest::newRow("color(a98-rgb 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(a98-rgb 100% 50% 0% / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::A98Rgb //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;

        QTest::newRow("color(prophoto-rgb 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(prophoto-rgb 100% 50% 0% / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::ProphotoRgb //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;

        QTest::newRow("color(rec2020 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(rec2020 100% 50% 0% / 0.5)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Rec2020 //
            << 1. << 0.5 << 0. << 0. //
            << 0.5;

        QTest::newRow("color(xyz 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(xyz 0.1 0.2 0.3 / 0.5)") //
            << ColorModel::XyzD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;
        QTest::newRow("color(xyz 10% 20% 30% / 50%)") //
            << QStringLiteral("color(xyz 10% 20% 30% / 50%)") //
            << ColorModel::XyzD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;
        QTest::newRow("color(xyz-d65 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(xyz-d65 0.1 0.2 0.3 / 0.5)") //
            << ColorModel::XyzD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;
        QTest::newRow("color(xyz-d50 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(xyz-d50 0.1 0.2 0.3 / 0.5)") //
            << ColorModel::XyzD50 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;

        QTest::newRow("hsl(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("hsl(0.1 0.2 0.3 / 0.5)") //
            << ColorModel::Invalid //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("hsl(3 10% 20% / 0.5)") //
            << QStringLiteral("hsl(3 10% 20% / 0.5)") //
            << ColorModel::Hsl360_1_1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0.1 << 0.2 << 0. //
            << 0.5;
        QTest::newRow("hsla(3, 10%, 20%, 0.5)") //
            << QStringLiteral("hsla(3, 10%, 20%, 0.5)") //
            << ColorModel::Hsl360_1_1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0.1 << 0.2 << 0. //
            << 0.5;
        QTest::newRow("hwb(3 10% 20% / 0.5)") //
            << QStringLiteral("hwb(3 10% 20% / 0.5)") //
            << ColorModel::Hwb360_1_1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0.1 << 0.2 << 0. //
            << 0.5;
        QTest::newRow("hwb(3 none 20% / 0.5)") //
            << QStringLiteral("hwb(3 none 20% / 0.5)") //
            << ColorModel::Hwb360_1_1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0. << 0.2 << 0. //
            << 0.5;
        QTest::newRow("hwb(none 10% 20% / 0.5)") //
            << QStringLiteral("hwb(none 10% 20% / 0.5)") //
            << ColorModel::Hwb360_1_1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 0. << 0.1 << 0.2 << 0. //
            << 0.5;

        QTest::newRow("oklab(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("oklab(0.1 0.2 0.3 / 0.5)") //
            << ColorModel::OklabD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;
        QTest::newRow("oklab(30% 50% 150% / 0.5)") //
            << QStringLiteral("oklab(30% 50% 150% / 0.5)") //
            << ColorModel::OklabD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.3 << 0.2 << 0.6 << 0. //
            << 0.5;

        QTest::newRow("lab(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("lab(0.1 0.2 0.3 / 0.5)") //
            << ColorModel::CielabD50 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;
        QTest::newRow("lab(30% 50% 150% / 0.5)") //
            << QStringLiteral("lab(30% 50% 150% / 0.5)") //
            << ColorModel::CielabD50 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 30. << 62.5 << 187.5 << 0. //
            << 0.5;

        QTest::newRow("oklch(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("oklch(0.1 0.2 0.3 / 0.5)") //
            << ColorModel::OklchD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;
        QTest::newRow("oklch(0.1 0.2 0.75turn / 0.5)") //
            << QStringLiteral("oklch(0.1 0.2 0.75turn / 0.5)") //
            << ColorModel::OklchD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 270. << 0. //
            << 0.5;
        QTest::newRow("oklch(30% 50% 150 / 0.5)") //
            << QStringLiteral("oklch(30% 50% 150 / 0.5)") //
            << ColorModel::OklchD65 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.3 << 0.2 << 150. << 0. //
            << 0.5;

        QTest::newRow("lch(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("lch(0.1 0.2 0.3 / 0.5)") //
            << ColorModel::CielchD50 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 << 0. //
            << 0.5;

        QTest::newRow("lch(0.1 0.2 0.75turn / 0.5)") //
            << QStringLiteral("lch(0.1 0.2 0.75turn / 0.5)") //
            << ColorModel::CielchD50 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 270. << 0. //
            << 0.5;
        QTest::newRow("lch(30% 50% 0.8 / 0.5)") //
            << QStringLiteral("lch(30% 50% 0.8 / 0.5)") //
            << ColorModel::CielchD50 //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 30. << 75. << 0.8 << 0. //
            << 0.5;
        QTest::newRow("lch(30% 50% 0.8% / 0.5)") //
            << QStringLiteral("lch(30% 50% 0.8% / 0.5)") //
            << ColorModel::Invalid //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. << 0. //
            << 0.;
    }

    void testParseAbsoluteColorFunction()
    {
        QFETCH(QString, function);
        QFETCH(ColorModel, model);
        QFETCH(CssColor::CssPredefinedRgbColorSpace, rgbColorSpace);
        QFETCH(double, first);
        QFETCH(double, second);
        QFETCH(double, third);
        QFETCH(double, forth);
        QFETCH(double, alpha1);

        const auto temp = CssColor::parseAbsoluteColorFunction(function);
        QCOMPARE(temp.model, model);
        QCOMPARE(temp.rgbColorSpace, rgbColorSpace);
        QCOMPARE(temp.color.first, first);
        QCOMPARE(temp.color.second, second);
        QCOMPARE(temp.color.third, third);
        QCOMPARE(temp.color.fourth, forth);
        QCOMPARE(temp.alpha1, alpha1);
    }
#endif

    void testParseArgumentPercentNoneTo1()
    {
        QVERIFY(!CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral(""))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("0"))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1"))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1."))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1 %"))
                     .has_value());
        QCOMPARE(CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1%"))
                     .value(),
                 0.01);
        QCOMPARE(CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("100%"))
                     .value(),
                 1);
        QCOMPARE(CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("200%"))
                     .value(),
                 2);
        QCOMPARE(CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("-200%"))
                     .value(),
                 -2);
        QCOMPARE(CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("0%"))
                     .value(),
                 0);
        QCOMPARE(CssColor::parseArgumentPercentNoneTo1( //
                     QStringLiteral("none"))
                     .value(),
                 0);
    }

    void testParseArgumentHueNoneTo360()
    {
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("none"))
                     .value_or(42),
                 0.);

        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral(""))
                     .has_value());

        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("0"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("-5"))
                     .value_or(42),
                 355.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("5"))
                     .value_or(42),
                 5.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("500"))
                     .value_or(42),
                 140.);

        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 deg"))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("deg"))
                     .has_value());
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("0deg"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("-5deg"))
                     .value_or(42),
                 355.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("5deg"))
                     .value_or(42),
                 5.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("500deg"))
                     .value_or(42),
                 140.);

        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 grad"))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("grad"))
                     .has_value());
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("0grad"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("-5grad"))
                     .value_or(42),
                 355.5);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("5grad"))
                     .value_or(42),
                 4.5);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("500grad"))
                     .value_or(42),
                 90.);

        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 rad"))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("rad"))
                     .has_value());
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("0rad"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("-1rad"))
                     .value_or(42),
                 -1. / (2 * pi) * 360 + 360);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1rad"))
                     .value_or(42),
                 1. / (2 * pi) * 360);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("9rad"))
                     .value_or(42),
                 9. / (2 * pi) * 360 - 360);

        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 turn"))
                     .has_value());
        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("turn"))
                     .has_value());
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("0turn"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("-0.5turn"))
                     .value_or(42),
                 180.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("-1turn"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("0.25turn"))
                     .value_or(42),
                 90.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("0.5turn"))
                     .value_or(42),
                 180.);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1turn"))
                     .value_or(42),
                 0);
        QCOMPARE(CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1.25turn"))
                     .value_or(42),
                 90);

        QVERIFY(!CssColor::parseArgumentHueNoneTo360( //
                     QStringLiteral("1%"))
                     .has_value());
    }

#ifndef MSVC_DLL
    void testParse_data()
    {
        QTest::addColumn<QString>("function");
        QTest::addColumn<ColorModel>("model");
        QTest::addColumn<CssColor::CssPredefinedRgbColorSpace>("rgbColorSpace");
        QTest::addColumn<double>("first");
        QTest::addColumn<double>("second");
        QTest::addColumn<double>("third");
        QTest::addColumn<double>("forth");
        QTest::addColumn<double>("alpha1");

        QTest::newRow("") //
            << QStringLiteral("") //
            << ColorModel::Invalid //
            << CssColor::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(1 2 3)") //
            << QStringLiteral("rgb(1 2 3)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 1.;
        QTest::newRow("rgb(1 2 3);") //
            << QStringLiteral("rgb(1 2 3)") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 1.;
        QTest::newRow("#010203") //
            << QStringLiteral("#010203") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 1.;
        QTest::newRow(" #010203 ") //
            << QStringLiteral(" #010203 ") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. << 0. //
            << 1.;
        QTest::newRow("aliceblue") //
            << QStringLiteral("aliceblue") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 240. / 255. << 248. / 255. << 1. << 0. //
            << 1.;
        QTest::newRow(" aliceblue ") //
            << QStringLiteral(" aliceblue ") //
            << ColorModel::Rgb1 //
            << CssColor::CssPredefinedRgbColorSpace::Srgb //
            << 240. / 255. << 248. / 255. << 1. << 0. //
            << 1.;
    }

    void testParse()
    {
        QFETCH(QString, function);
        QFETCH(ColorModel, model);
        QFETCH(CssColor::CssPredefinedRgbColorSpace, rgbColorSpace);
        QFETCH(double, first);
        QFETCH(double, second);
        QFETCH(double, third);
        QFETCH(double, forth);
        QFETCH(double, alpha1);

        const auto temp = CssColor::parse(function);
        QCOMPARE(temp.model, model);
        QCOMPARE(temp.rgbColorSpace, rgbColorSpace);
        QCOMPARE(temp.color.first, first);
        QCOMPARE(temp.color.second, second);
        QCOMPARE(temp.color.third, third);
        QCOMPARE(temp.color.fourth, forth);
        QCOMPARE(temp.alpha1, alpha1);
    }
#endif

#ifndef MSVC_DLL
    void testGenerateCss()
    {
        QHash<ColorModel, GenericColor> hash;
        hash.insert(ColorModel::OklchD65, GenericColor(0.5, 0.2, 10));
        QCOMPARE(CssColor::generateCss(hash, 1, 3).value(0), //
                 QStringLiteral("oklch(0.50 0.20 10)"));
        QCOMPARE(CssColor::generateCss(hash, 0.5, 3).value(0), //
                 QStringLiteral("oklch(0.50 0.20 10 / 50%)"));
        const auto roundtrip = CssColor::parse( //
            CssColor::generateCss(hash, 0.5, 3).value(0));
        QCOMPARE(roundtrip.alpha1, 0.5);
        QCOMPARE(roundtrip.model, ColorModel::OklchD65);
        QCOMPARE(roundtrip.rgbColorSpace, //
                 CssColor::CssPredefinedRgbColorSpace::Invalid);
        QCOMPARE(roundtrip.color.first, 0.5);
        QCOMPARE(roundtrip.color.second, 0.2);
        QCOMPARE(roundtrip.color.third, 10);
        QCOMPARE(roundtrip.color.fourth, 0);
    }
#endif
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestCssColor)

// The following “include” is necessary because we do not use a header file:
#include "testcsscolor.moc"
