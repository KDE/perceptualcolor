// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "csscolor.h"
// Second, the private implementation.
#include "csscolor_p.h" // IWYU pragma: keep

#include "genericcolor.h"
#include "perceptualcolornamespace.h"
#include <absolutecolor.h>
#include <numbers>
#include <optional>
#include <qcontainerfwd.h>
#include <qdebug.h>
#include <qhash.h>
#include <qlist.h>
#include <qobject.h>
#include <qrgb.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>
#include <qtmetamacros.h>

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
            CssColorPrivate::parseHexColor(QString()).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#1")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#12")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#12345")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#1234567")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#123456789")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("123")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("1234")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("123456")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("12345678")).has_value(),
            false);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#123")).value_or(42),
            0xff112233);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#1234")).value_or(42),
            0x44112233);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#123456")).value_or(42),
            0xff123456);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#12345678")).value_or(42),
            0x78123456);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#abcdef78")).value_or(42),
            0x78abcdef);
        QCOMPARE( //
            CssColorPrivate::parseHexColor(QStringLiteral("#AbCdeF78")).value_or(42),
            0x78abcdef);
    }

    void testParseNamedColor()
    {
        QCOMPARE(CssColorPrivate::parseNamedColor(QString()).has_value(), false);
        QCOMPARE(CssColorPrivate::parseNamedColor( //
                     QStringLiteral("invalid"))
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseNamedColor( //
                     QStringLiteral("aliceblue"))
                     .value_or(42),
                 0xfff0f8ff);
        QCOMPARE(CssColorPrivate::parseNamedColor( //
                     QStringLiteral("AliceBlue"))
                     .value_or(42),
                 0xfff0f8ff);
        QCOMPARE(CssColorPrivate::parseNamedColor( //
                     QStringLiteral("AliceBlueX"))
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseNamedColor( //
                     QStringLiteral("XAliceBlue"))
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseNamedColor( //
                     QStringLiteral("transparent"))
                     .value_or(42),
                 0x00000000);
        QCOMPARE(CssColorPrivate::parseNamedColor( //
                     QStringLiteral("TransParent"))
                     .value_or(42),
                 0x00000000);
    }

    void testParseArgumentPercentNumberNone()
    {
        // Empty value
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QString(),
                     255,
                     0)
                     .has_value(),
                 false);

        // none
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     255,
                     0)
                     .value_or(42),
                 0);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     255,
                     1)
                     .value_or(42),
                 1);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     255,
                     255)
                     .value_or(42),
                 255);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral(" none"),
                     255,
                     255)
                     .value_or(42),
                 255);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("none "),
                     255,
                     255)
                     .value_or(42),
                 255);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     0,
                     0)
                     .value_or(42),
                 0);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("none"),
                     -1,
                     0)
                     .value_or(42),
                 0);

        // Number
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56"),
                     255,
                     0)
                     .value_or(42),
                 56);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56"),
                     2,
                     -1)
                     .value_or(42),
                 56);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral(" 56"),
                     255,
                     0)
                     .value_or(42),
                 56);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56 "),
                     255,
                     0)
                     .value_or(42),
                 56);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0"),
                     2,
                     -1)
                     .value_or(42),
                 56);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56,0"),
                     255,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56e1"),
                     2,
                     -1)
                     .value_or(42),
                 560);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0e1"),
                     2,
                     -1)
                     .value_or(42),
                 560);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0e-1"),
                     2,
                     -1)
                     .value_or(42),
                 5.6);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("56.0E-1"),
                     2,
                     -1)
                     .value_or(42),
                 5.6);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("-56.0e-1"),
                     2,
                     -1)
                     .value_or(42),
                 -5.6);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("-56 e1"),
                     255,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("-56"),
                     255,
                     0)
                     .value_or(42),
                 -56);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("- 56"),
                     255,
                     0)
                     .has_value(),
                 false);

        // Percent
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50 %"),
                     300,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50%"),
                     300,
                     0)
                     .value_or(42),
                 150);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50%"),
                     400,
                     -1)
                     .value_or(42),
                 200);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral(" 50%"),
                     200,
                     0)
                     .value_or(42),
                 100);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50% "),
                     200,
                     0)
                     .value_or(42),
                 100);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0%"),
                     200,
                     -1)
                     .value_or(42),
                 100);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50,0%"),
                     200,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50e1%"),
                     200,
                     -1)
                     .value_or(42),
                 1000);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0e1%"),
                     200,
                     -1)
                     .value_or(42),
                 1000);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0e-1%"),
                     200,
                     -1)
                     .value_or(42),
                 10);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("50.0E-1%"),
                     200,
                     -1)
                     .value_or(42),
                 10);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("-50.0e-1%"),
                     200,
                     -1)
                     .value_or(42),
                 -10);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("-50 e1%"),
                     255,
                     0)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("-50%"),
                     200,
                     0)
                     .value_or(42),
                 -100);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNumberNone( //
                     QStringLiteral("- 50%"),
                     200,
                     0)
                     .has_value(),
                 false);
    }

    void testValidateArguments()
    {
        QCOMPARE(CssColorPrivate::validateArguments( //
                     QStringList({QStringLiteral("1"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .value(),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3")}));
        QCOMPARE(CssColorPrivate::validateArguments( //
                     QStringList({QStringLiteral(" 1"), //
                                  QStringLiteral("2 "),
                                  QStringLiteral(" 3 ")}))
                     .value(),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3")}));
        QCOMPARE(CssColorPrivate::validateArguments( //
                     QStringList({QStringLiteral("1 1"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::validateArguments( //
                     QStringList({QStringLiteral("1,"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::validateArguments( //
                     QStringList({QStringLiteral("1/"), //
                                  QStringLiteral("2"),
                                  QStringLiteral("3")}))
                     .has_value(),
                 false);
    }

    void testParseAllFunctionArguments()
    {
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColorPrivate::FunctionSyntax::BothSyntaxes,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColorPrivate::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     3)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     3)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3"),
                     CssColorPrivate::FunctionSyntax::LegacySyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3"),
                     CssColorPrivate::FunctionSyntax::BothSyntaxes,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("none")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 / 4"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("4")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 / 4"),
                     CssColorPrivate::FunctionSyntax::BothSyntaxes,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("4")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 / 4"),
                     CssColorPrivate::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3 / 4"),
                     CssColorPrivate::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 / 3"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 / 3 4"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 4"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1 2 3 4 5"),
                     CssColorPrivate::FunctionSyntax::StandardSyntax,
                     4)
                     .has_value(),
                 false);
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3, 4"),
                     CssColorPrivate::FunctionSyntax::LegacySyntax,
                     4)
                     .value_or(QStringList()),
                 QStringList({QStringLiteral("1"), //
                              QStringLiteral("2"),
                              QStringLiteral("3"),
                              QStringLiteral("4")}));
        QCOMPARE(CssColorPrivate::parseAllFunctionArguments( //
                     QStringLiteral("1, 2, 3, 4, 5"),
                     CssColorPrivate::FunctionSyntax::LegacySyntax,
                     4)
                     .has_value(),
                 false);
    }

#ifndef MSVC_DLL
    void testParseAbsoluteColorFunction_data()
    {
        QTest::addColumn<QString>("function");
        QTest::addColumn<CssColorPrivate::CssColorModel>("model");
        QTest::addColumn<CssColorPrivate::CssPredefinedRgbColorSpace>("rgbColorSpace");
        QTest::addColumn<double>("first");
        QTest::addColumn<double>("second");
        QTest::addColumn<double>("third");
        QTest::addColumn<double>("alpha1");

        QTest::newRow("rgb(1 2 3)") //
            << QStringLiteral("rgb(1 2 3)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 1.;
        QTest::newRow("rgba(1 2 3)") //
            << QStringLiteral("rgba(1 2 3)") //
            << CssColorPrivate::CssColorModel::Invalid //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(1 2 3 / 0.5)") //
            << QStringLiteral("rgb(1 2 3 / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 0.5;
        QTest::newRow("rgb(1 2 / 3)") //
            << QStringLiteral("rgb(1 2 / 3)") //
            << CssColorPrivate::CssColorModel::Invalid //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(1, 2, 3, 0.5)") //
            << QStringLiteral("rgb(1, 2, 3, 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 0.5;
        QTest::newRow("rgba(1, 2, 3, 0.5)") //
            << QStringLiteral("rgba(1, 2, 3, 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 0.5;
        QTest::newRow("rgb(100% 50% 0% / 0.5)") //
            << QStringLiteral("rgb(100% 50% 0% / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. //
            << 0.5;
        QTest::newRow("rgb(100% 50% 0% / 50%)") //
            << QStringLiteral("rgb(100% 50% 0% / 50%)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. //
            << 0.5;
        QTest::newRow("rgb(100% 50% 0% / none)") //
            << QStringLiteral("rgb(100% 50% 0% / none)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. //
            << 1.;
        QTest::newRow("rgb(100% 50% 0% / )") //
            << QStringLiteral("rgb(100% 50% 0% / )") //
            << CssColorPrivate::CssColorModel::Invalid //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(100% 50% 0%)") //
            << QStringLiteral("rgb(100% 50% 0%)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. //
            << 1.;
        QTest::newRow("rgb(none 50% 0%)") //
            << QStringLiteral("rgb(none 50% 0%)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 0. << 0.5 << 0. //
            << 1.;

        QTest::newRow("color(srgb 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(srgb 100% 50% 0% / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. << 0.5 << 0. //
            << 0.5;
        QTest::newRow("color(srgb 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(srgb 0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;

        QTest::newRow("color(srgb-linear 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(srgb-linear 100% 50% 0% / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::SrgbLinear //
            << 1. << 0.5 << 0. //
            << 0.5;

        QTest::newRow("color(display-p3 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(display-p3 100% 50% 0% / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::DisplayP3 //
            << 1. << 0.5 << 0. //
            << 0.5;

        QTest::newRow("color(a98-rgb 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(a98-rgb 100% 50% 0% / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::A98Rgb //
            << 1. << 0.5 << 0. //
            << 0.5;

        QTest::newRow("color(prophoto-rgb 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(prophoto-rgb 100% 50% 0% / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::ProphotoRgb //
            << 1. << 0.5 << 0. //
            << 0.5;

        QTest::newRow("color(rec2020 100% 50% 0% / 0.5)") //
            << QStringLiteral("color(rec2020 100% 50% 0% / 0.5)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Rec2020 //
            << 1. << 0.5 << 0. //
            << 0.5;

        QTest::newRow("color(xyz 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(xyz 0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::XyzD65_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;
        QTest::newRow("color(xyz 10% 20% 30% / 50%)") //
            << QStringLiteral("color(xyz 10% 20% 30% / 50%)") //
            << CssColorPrivate::CssColorModel::XyzD65_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;
        QTest::newRow("color(xyz-d65 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(xyz-d65 0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::XyzD65_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;
        QTest::newRow("color(xyz-d50 0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("color(xyz-d50 0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::XyzD50_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;

        QTest::newRow("hsl(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("hsl(0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::Invalid //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("hsl(3 10% 20% / 0.5)") //
            << QStringLiteral("hsl(3 10% 20% / 0.5)") //
            << CssColorPrivate::CssColorModel::Hsl_360_1_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0.1 << 0.2 //
            << 0.5;
        QTest::newRow("hsla(3, 10%, 20%, 0.5)") //
            << QStringLiteral("hsla(3, 10%, 20%, 0.5)") //
            << CssColorPrivate::CssColorModel::Hsl_360_1_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0.1 << 0.2 //
            << 0.5;
        QTest::newRow("hwb(3 10% 20% / 0.5)") //
            << QStringLiteral("hwb(3 10% 20% / 0.5)") //
            << CssColorPrivate::CssColorModel::Hwb_360_1_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0.1 << 0.2 //
            << 0.5;
        QTest::newRow("hwb(3 none 20% / 0.5)") //
            << QStringLiteral("hwb(3 none 20% / 0.5)") //
            << CssColorPrivate::CssColorModel::Hwb_360_1_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 3. << 0. << 0.2 //
            << 0.5;
        QTest::newRow("hwb(none 10% 20% / 0.5)") //
            << QStringLiteral("hwb(none 10% 20% / 0.5)") //
            << CssColorPrivate::CssColorModel::Hwb_360_1_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 0. << 0.1 << 0.2 //
            << 0.5;

        QTest::newRow("oklab(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("oklab(0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::OklabD65 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;
        QTest::newRow("oklab(30% 50% 150% / 0.5)") //
            << QStringLiteral("oklab(30% 50% 150% / 0.5)") //
            << CssColorPrivate::CssColorModel::OklabD65 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.3 << 0.2 << 0.6 //
            << 0.5;

        QTest::newRow("lab(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("lab(0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::CielabD50 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;
        QTest::newRow("lab(30% 50% 150% / 0.5)") //
            << QStringLiteral("lab(30% 50% 150% / 0.5)") //
            << CssColorPrivate::CssColorModel::CielabD50 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 30. << 62.5 << 187.5 //
            << 0.5;

        QTest::newRow("oklch(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("oklch(0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::OklchD65 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;
        QTest::newRow("oklch(0.1 0.2 0.75turn / 0.5)") //
            << QStringLiteral("oklch(0.1 0.2 0.75turn / 0.5)") //
            << CssColorPrivate::CssColorModel::OklchD65 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 270. //
            << 0.5;
        QTest::newRow("oklch(30% 50% 150 / 0.5)") //
            << QStringLiteral("oklch(30% 50% 150 / 0.5)") //
            << CssColorPrivate::CssColorModel::OklchD65 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.3 << 0.2 << 150. //
            << 0.5;

        QTest::newRow("lch(0.1 0.2 0.3 / 0.5)") //
            << QStringLiteral("lch(0.1 0.2 0.3 / 0.5)") //
            << CssColorPrivate::CssColorModel::CielchD50 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 0.3 //
            << 0.5;

        QTest::newRow("lch(0.1 0.2 0.75turn / 0.5)") //
            << QStringLiteral("lch(0.1 0.2 0.75turn / 0.5)") //
            << CssColorPrivate::CssColorModel::CielchD50 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0.1 << 0.2 << 270. //
            << 0.5;
        QTest::newRow("lch(30% 50% 0.8 / 0.5)") //
            << QStringLiteral("lch(30% 50% 0.8 / 0.5)") //
            << CssColorPrivate::CssColorModel::CielchD50 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 30. << 75. << 0.8 //
            << 0.5;
        QTest::newRow("lch(30% 50% 0.8% / 0.5)") //
            << QStringLiteral("lch(30% 50% 0.8% / 0.5)") //
            << CssColorPrivate::CssColorModel::Invalid //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. //
            << 0.;
    }

    void testParseAbsoluteColorFunction()
    {
        QFETCH(QString, function);
        QFETCH(CssColorPrivate::CssColorModel, model);
        QFETCH(CssColorPrivate::CssPredefinedRgbColorSpace, rgbColorSpace);
        QFETCH(double, first);
        QFETCH(double, second);
        QFETCH(double, third);
        QFETCH(double, alpha1);

        const auto temp = CssColorPrivate::parseAbsoluteColorFunction(function);
        QCOMPARE(temp.model, model);
        QCOMPARE(temp.rgbColorSpace, rgbColorSpace);
        QCOMPARE(temp.color.first, first);
        QCOMPARE(temp.color.second, second);
        QCOMPARE(temp.color.third, third);
        QCOMPARE(temp.alpha1, alpha1);
    }
#endif

    void testParseArgumentPercentNoneTo1()
    {
        QVERIFY(!CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QString())
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("0"))
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1"))
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1."))
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1 %"))
                     .has_value());
        QCOMPARE(CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("1%"))
                     .value(),
                 0.01);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("100%"))
                     .value(),
                 1);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("200%"))
                     .value(),
                 2);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("-200%"))
                     .value(),
                 -2);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("0%"))
                     .value(),
                 0);
        QCOMPARE(CssColorPrivate::parseArgumentPercentNoneTo1( //
                     QStringLiteral("none"))
                     .value(),
                 0);
    }

    void testParseArgumentHueNoneTo360()
    {
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("none"))
                     .value_or(42),
                 0.);

        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QString())
                     .has_value());

        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("0"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("-5"))
                     .value_or(42),
                 355.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("5"))
                     .value_or(42),
                 5.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("500"))
                     .value_or(42),
                 140.);

        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 deg"))
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("deg"))
                     .has_value());
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("0deg"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("-5deg"))
                     .value_or(42),
                 355.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("5deg"))
                     .value_or(42),
                 5.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("500deg"))
                     .value_or(42),
                 140.);

        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 grad"))
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("grad"))
                     .has_value());
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("0grad"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("-5grad"))
                     .value_or(42),
                 355.5);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("5grad"))
                     .value_or(42),
                 4.5);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("500grad"))
                     .value_or(42),
                 90.);

        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 rad"))
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("rad"))
                     .has_value());
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("0rad"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("-1rad"))
                     .value_or(42),
                 -1. / (2 * std::numbers::pi) * 360 + 360);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1rad"))
                     .value_or(42),
                 1. / (2 * std::numbers::pi) * 360);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("9rad"))
                     .value_or(42),
                 9. / (2 * std::numbers::pi) * 360 - 360);

        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1 turn"))
                     .has_value());
        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("turn"))
                     .has_value());
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("0turn"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("-0.5turn"))
                     .value_or(42),
                 180.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("-1turn"))
                     .value_or(42),
                 0.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("0.25turn"))
                     .value_or(42),
                 90.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("0.5turn"))
                     .value_or(42),
                 180.);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1turn"))
                     .value_or(42),
                 0);
        QCOMPARE(CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1.25turn"))
                     .value_or(42),
                 90);

        QVERIFY(!CssColorPrivate::parseArgumentHueNoneTo360( //
                     QStringLiteral("1%"))
                     .has_value());
    }

#ifndef MSVC_DLL
    void testParseToStructuredValue_data()
    {
        QTest::addColumn<QString>("function");
        QTest::addColumn<CssColorPrivate::CssColorModel>("model");
        QTest::addColumn<CssColorPrivate::CssPredefinedRgbColorSpace>("rgbColorSpace");
        QTest::addColumn<double>("first");
        QTest::addColumn<double>("second");
        QTest::addColumn<double>("third");
        QTest::addColumn<double>("alpha1");

        QTest::newRow("") //
            << QString() //
            << CssColorPrivate::CssColorModel::Invalid //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Invalid //
            << 0. << 0. << 0. //
            << 0.;
        QTest::newRow("rgb(1 2 3)") //
            << QStringLiteral("rgb(1 2 3)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 1.;
        QTest::newRow("rgb(1 2 3);") //
            << QStringLiteral("rgb(1 2 3)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 1.;
        QTest::newRow("color(srgb 0.1 0.2 0.3)") //
            << QStringLiteral("color(srgb 0.1 0.2 0.3)") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 0.1 << 0.2 << 0.3 //
            << 1.;
        QTest::newRow("color(srgb-linear 0.4 0.6 0.8 / 0.5);") //
            << QStringLiteral("color(srgb-linear 0.4 0.6 0.8 / 0.5);") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::SrgbLinear //
            << 0.4 << 0.6 << 0.8 //
            << 0.5;
        QTest::newRow("#010203") //
            << QStringLiteral("#010203") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 1.;
        QTest::newRow(" #010203 ") //
            << QStringLiteral(" #010203 ") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 1. / 255. << 2. / 255. << 3. / 255. //
            << 1.;
        QTest::newRow("aliceblue") //
            << QStringLiteral("aliceblue") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 240. / 255. << 248. / 255. << 1. //
            << 1.;
        QTest::newRow(" aliceblue ") //
            << QStringLiteral(" aliceblue ") //
            << CssColorPrivate::CssColorModel::Rgb_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 240. / 255. << 248. / 255. << 1. //
            << 1.;
        QTest::newRow(" hsl(210 50% 40% / 80%) ") //
            << QStringLiteral(" hsl(210 50% 40% / 80%) ") //
            << CssColorPrivate::CssColorModel::Hsl_360_1_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 210. << 50. / 100. << 40. / 100. //
            << 80. / 100.;
        QTest::newRow(" hwb(210 20% 40% / 80%) ") //
            << QStringLiteral(" hwb(210 20% 40% / 80%) ") //
            << CssColorPrivate::CssColorModel::Hwb_360_1_1 //
            << CssColorPrivate::CssPredefinedRgbColorSpace::Srgb //
            << 210. << 20. / 100. << 40. / 100. //
            << 80. / 100.;
    }

    void testParseToStructuredValue()
    {
        QFETCH(QString, function);
        QFETCH(CssColorPrivate::CssColorModel, model);
        QFETCH(CssColorPrivate::CssPredefinedRgbColorSpace, rgbColorSpace);
        QFETCH(double, first);
        QFETCH(double, second);
        QFETCH(double, third);
        QFETCH(double, alpha1);

        const auto temp = CssColorPrivate::parseToStructuredValue(function);
        QCOMPARE(temp.model, model);
        QCOMPARE(temp.rgbColorSpace, rgbColorSpace);
        QCOMPARE(temp.color.first, first);
        QCOMPARE(temp.color.second, second);
        QCOMPARE(temp.color.third, third);
        QCOMPARE(temp.alpha1, alpha1);
    }

    void testParseToCompleteColor()
    {
        {
            const QString testValue = QStringLiteral("rgb(20% 40% 60%)");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorSRgb.rgb_1;
            QCOMPARE(result.first, 0.2);
            QCOMPARE(result.second, 0.4);
            QCOMPARE(result.third, 0.6);
        }

        {
            const QString testValue = QStringLiteral("rgb(20 40 60)");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorSRgb.rgb_1;
            QCOMPARE(result.first, 20 / 255.);
            QCOMPARE(result.second, 40 / 255.);
            QCOMPARE(result.third, 60 / 255.);
        }

        {
            const QString testValue = QStringLiteral("hsl(210 50% 40% / 80%)");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorSRgb.hsl_360_100_100;
            QCOMPARE(result.first, 210);
            QCOMPARE(result.second, 50);
            QCOMPARE(result.third, 40);
        }

        {
            const QString testValue = QStringLiteral("hwb(210 20% 40% / 80%)");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorSRgb.hwb_360_100_100;
            QCOMPARE(result.first, 210);
            QCOMPARE(result.second, 20);
            QCOMPARE(result.third, 40);
        }

        {
            const QString testValue = //
                QStringLiteral("oklab(0.6 0.1 -0.15 / 50%)");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorAbs[ColorModel::OklabD65];
            QCOMPARE(result.first, 0.6);
            QCOMPARE(result.second, 0.1);
            QCOMPARE(result.third, -0.15);
        }

        {
            const QString testValue = //
                QStringLiteral("oklch(0.7 0.15 180 / 80%)");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorAbs[ColorModel::OklchD65];
            QCOMPARE(result.first, 0.7);
            QCOMPARE(result.second, 0.15);
            QCOMPARE(result.third, 180);
        }

        {
            const QString testValue = //
                QStringLiteral("color(xyz-d65 0.2 0.3 0.4 / 100%)");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorAbs[ColorModel::XyzD65_1];
            QCOMPARE(result.first, 0.2);
            QCOMPARE(result.second, 0.3);
            QCOMPARE(result.third, 0.4);
        }

        {
            const QString testValue = //
                QStringLiteral("color(srgb-linear 0.4 0.6 0.8 / 0.5);");
            const auto parsed = CssColor::parseToCompleteColor(testValue);
            const auto result = parsed.opaqueColorAbs[ColorModel::LinearSRgb_1];
            QCOMPARE(result.first, 0.4);
            QCOMPARE(result.second, 0.6);
            QCOMPARE(result.third, 0.8);
        }
    }

    void testRoundtrip_data()
    {
        QTest::addColumn<QString>("cssColorString");
        QTest::addColumn<quint8>("precision");

        // Generate CSS strings with a given precision
        constexpr quint8 precision = 0;
        const auto temp = CompleteColor::fromSRgbColor( //
            RgbColor::fromRgb_1(GenericColor(0.2, 0.4, 0.6)), //
            0.8);
        const auto cssStrings = CssColor::generateCss(temp, precision);

        // Now make a roundtrip test for every of those strings
        for (const auto &cssString : std::as_const(cssStrings)) {
            QTest::newRow(cssString.toUtf8().constData()) //
                << cssString //
                << precision;
        }
    }

    void testRoundtrip()
    {
        QFETCH(QString, cssColorString);
        QFETCH(quint8, precision);

        const auto parsedColor = CssColor::parseToCompleteColor(cssColorString);
        const auto roundtripResult = //
            CssColor::generateCss(parsedColor, precision);

        QVERIFY(roundtripResult.contains(cssColorString));
    }

    void testGenerateCss()
    {
        const auto abs = AbsoluteColor::allConversions( //
            ColorModel::OklchD65, //
            GenericColor(0.5, 0.2, 10));
        const auto css1 = CssColor::generateCss( //
            CompleteColor::fromAbsColor(abs, 1), //
            0);
        QCOMPARE(css1.value(0), //
                 QStringLiteral("oklch(0.50 0.20 10)"));
        const auto css2 = CssColor::generateCss( //
            CompleteColor::fromAbsColor(abs, 0.5), //
            0);
        QCOMPARE(css2.value(0), //
                 QStringLiteral("oklch(0.50 0.20 10 / 50%)"));
        const auto roundtrip = CssColorPrivate::parseToStructuredValue( //
            CssColor::generateCss( //
                CompleteColor::fromAbsColor(abs, 0.5),
                0)
                .value(0));
        QCOMPARE(roundtrip.alpha1, 0.5);
        QCOMPARE(roundtrip.model, CssColorPrivate::CssColorModel::OklchD65);
        QCOMPARE(roundtrip.rgbColorSpace, //
                 CssColorPrivate::CssPredefinedRgbColorSpace::Invalid);
        QCOMPARE(roundtrip.color.first, 0.5);
        QCOMPARE(roundtrip.color.second, 0.2);
        QCOMPARE(roundtrip.color.third, 10);

        // Test correct #hex transparency, making shure that the data
        // does not come from the OPAQUE color data, which would loose
        // this information.
        const auto abs2 = AbsoluteColor::allConversions( //
            ColorModel::SRgb_1, //
            GenericColor(0x55 / 255., 0xAA / 255., 0xDD / 255.));
        const auto css4 = CssColor::generateCss( //
            CompleteColor::fromAbsColor(abs2, 0x77 / 255.), //
            0);
        QCOMPARE(css4.value(8), //
                 QStringLiteral("#55AADD77"));
    }
#endif

    void testCssRgbHex()
    {
        QCOMPARE(CssColor::cssRgbHex(0., 0., 0.), //
                 QStringLiteral("#000000"));
        QCOMPARE(CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 0xFF * 1.), //
                 QStringLiteral("#1234FF"));
        QCOMPARE(CssColor::cssRgbHex(0xFF * 1., 0xFF * 1., 0xFF * 1.), //
                 QStringLiteral("#FFFFFF"));

        QCOMPARE(CssColor::cssRgbHex(-1., 0x34 * 1., 0xFF * 1.), //
                 QStringLiteral("#0034FF"));

        QCOMPARE(CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 253.4 * 1.), //
                 QStringLiteral("#1234FD"));
        QCOMPARE(CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 253.6 * 1.), //
                 QStringLiteral("#1234FE"));
        QCOMPARE(CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 254.4 * 1.), //
                 QStringLiteral("#1234FE"));
        QCOMPARE(CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 254.6 * 1.), //
                 QStringLiteral("#1234FF"));

        QCOMPARE(CssColor::cssRgbHex(0., 0., 0., 0.), //
                 QStringLiteral("#00000000"));
        QCOMPARE( //
            CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 0x56 * 1., 0xFF * 1.), //
            QStringLiteral("#123456FF"));
        QCOMPARE( //
            CssColor::cssRgbHex(0xFF * 1., 0xFF * 1., 0xFF * 1., 0xFF * 1.), //
            QStringLiteral("#FFFFFFFF"));

        QCOMPARE(CssColor::cssRgbHex(-1., 0x34 * 1., 0x56 * 1., 0xFF * 1.), //
                 QStringLiteral("#003456FF"));

        QCOMPARE( //
            CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 0x56 * 1., 253.4 * 1.), //
            QStringLiteral("#123456FD"));
        QCOMPARE( //
            CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 0x56 * 1., 253.6 * 1.), //
            QStringLiteral("#123456FE"));
        QCOMPARE( //
            CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 0x56 * 1., 254.4 * 1.), //
            QStringLiteral("#123456FE"));
        QCOMPARE( //
            CssColor::cssRgbHex(0x12 * 1., 0x34 * 1., 0x56 * 1., 254.6 * 1.), //
            QStringLiteral("#123456FF"));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestCssColor)

// The following “include” is necessary because we do not use a header file:
#include "testcsscolor.moc"
