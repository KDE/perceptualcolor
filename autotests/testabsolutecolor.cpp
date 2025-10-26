// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "absolutecolor.h"

#include "genericcolor.h"
#include "helperconversion.h"
#include "helpermath.h"
#include <algorithm>
#include <cmath>
#include <lcms2.h>
#include <optional>
#include <qgenericmatrix.h>
#include <qglobal.h>
#include <qhash.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

Q_DECLARE_METATYPE(cmsCIELab)

namespace PerceptualColor
{
class TestAbsoluteColor : public QObject
{
    Q_OBJECT

public:
    explicit TestAbsoluteColor(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    void generateDataXyzd65Oklab()
    {
        qRegisterMetaType<Trio>();
        QTest::addColumn<double>("x");
        QTest::addColumn<double>("y");
        QTest::addColumn<double>("z");
        QTest::addColumn<Trio>("oklab");

        // The following reference values come from from the original paper:
        // https://bottosson.github.io/posts/oklab/#table-of-example-xyz-and-oklab-pairs

        constexpr double oklabHighXYZ[] = //
            {1.000, 0.000, 0.000};
        QTest::newRow("highXYZ 0.950	1.000	1.089") //
            << 0.950 << 1.000 << 1.089 //
            << Trio(oklabHighXYZ);

        constexpr double oklabHighX[] = //
            {0.450, 1.236, -0.019};
        QTest::newRow("highX 1.000 0.000 0.000") //
            << 1.000 << 0.000 << 0.000 //
            << Trio(oklabHighX);

        constexpr double oklabHighY[] = //
            {0.922, -0.671, 0.263};
        QTest::newRow("highY 0.000 1.000 0.000") //
            << 0.000 << 1.000 << 0.000 //
            << Trio(oklabHighY);

        constexpr double oklabHighZ[] = //
            {0.153, -1.415, -0.449};
        QTest::newRow("highZ 0.000 0.000 1.000") //
            << 0.000 << 0.000 << 1.000 //
            << Trio(oklabHighZ);

        // The following reference values have been calculated with the
        // online tool https://colorjs.io/apps/convert/

        constexpr double oklabWhite[] = //
            {0.9999999934735462, 8.095285553e-11, 3.727390762709e-8};
        QTest::newRow("white 0.9504559270516717 1. 1.0890577507598784") //
            << 0.9504559270516717 << 1. << 1.0890577507598784 //
            << Trio(oklabWhite);

        constexpr double oklabRed[] = //
            {0.6279553606145516, 0.22486306106597398, 0.1258462985307351};
        QTest::newRow("red 0.41239079926595934 0.21263900587151027 0.01933081871559182") //
            << 0.41239079926595934 << 0.21263900587151027 << 0.01933081871559182 //
            << Trio(oklabRed);

        constexpr double oklabGreen[] = //
            {0.519751827794842, -0.14030232755311015, 0.10767589774360209};
        QTest::newRow("green 0.0771883343323022 0.1543766686646044 0.02572944477743406") //
            << 0.0771883343323022 << 0.1543766686646044 << 0.02572944477743406 //
            << Trio(oklabGreen);

        constexpr double oklabBlue[] = //
            {0.4520137183853429, -0.03245698416876397, -0.3115281476783752};
        QTest::newRow("blue 0.1804807884018343 0.07219231536073371 0.9505321522496607") //
            << 0.1804807884018343 << 0.07219231536073371 << 0.9505321522496607 //
            << Trio(oklabBlue);

        constexpr double oklabCyan[] = //
            {0.9053992300557675, -0.14944393961066077, -0.03939815774426181};
        QTest::newRow("cyan 0.5380651277857122 0.7873609941284897 1.0697269320442866") //
            << 0.5380651277857122 << 0.7873609941284897 << 1.0697269320442866 //
            << Trio(oklabCyan);

        constexpr double oklabMagenta[] = //
            {0.7016738558717924, 0.27456629431932855, -0.16915605926294264};
        QTest::newRow("magenta 0.5928715876677937 0.284831321232244 0.9698629709652525") //
            << 0.5928715876677937 << 0.284831321232244 << 0.9698629709652525 //
            << Trio(oklabMagenta);

        constexpr double oklabYellow[] = //
            {0.9679827203267873, -0.07136908036816808, 0.19856975465179516};
        QTest::newRow("yellow 0.7699751386498374 0.9278076846392663 0.13852559851021778") //
            << 0.7699751386498374 << 0.9278076846392663 << 0.13852559851021778 //
            << Trio(oklabYellow);

        constexpr double oklabBlack[] = //
            {0., 0., 0.};
        QTest::newRow("black 0. 0. 0.") //
            << 0. << 0. << 0. //
            << Trio(oklabBlack);

        constexpr double oklabGray[] = //
            {0.5998708017071177, 4.856132163e-11, 2.235952889507e-8};
        QTest::newRow("gray 0.2051658917495936 0.21586050011389926 0.23508455073194565") //
            << 0.2051658917495936 << 0.21586050011389926 << 0.23508455073194565 //
            << Trio(oklabGray);
    }

    void generateDataCielabd50Oklab()
    {
        qRegisterMetaType<Trio>();
        qRegisterMetaType<cmsCIELab>();
        QTest::addColumn<cmsCIELab>("cmscielab");
        QTest::addColumn<Trio>("oklab");

        // The following reference values have been calculated with the
        // online tool https://colorjs.io/apps/convert/

        constexpr double oklabSpecialWhite[] = //
            {1.0000000010492212, -1.0775085046432764e-8, 5.03845311028428e-8};
        // NOTE The lightness value is out-of-bound! (Valid range: 0..100)
        QTest::newRow("special white 100., 0., 0.") //
            << cmsCIELab({100., 0., 0.}) //
            << Trio(oklabSpecialWhite);

        constexpr double oklabWhite[] = //
            {1.000000009791752, -3.3637913787742946e-8, 6.836016341882356e-8};
        // NOTE The lightness value is out-of-bound! (Valid range: 0..100)
        QTest::newRow("white 100.00000139649632, -0.000007807961277528364, 0.000006766250648659877") //
            << cmsCIELab({100.00000139649632, -0.000007807961277528364, 0.000006766250648659877}) //
            << Trio(oklabWhite);

        constexpr double oklabRed[] = //
            {0.627955380062011, 0.22486300104638587, 0.1258463407318262};
        QTest::newRow("red 54.29054294696968 80.80492033462421 69.89098825896275") //
            << cmsCIELab({54.29054294696968, 80.80492033462421, 69.89098825896275}) //
            << Trio(oklabRed);

        constexpr double oklabGreen[] = //
            {0.5197518404266431, -0.14030239549323664, 0.10767592658888475};
        QTest::newRow("green 46.27770902748027 -47.55240796497723 48.58629466423457") //
            << cmsCIELab({46.27770902748027, -47.55240796497723, 48.58629466423457}) //
            << Trio(oklabGreen);

        constexpr double oklabBlue[] = //
            {0.4520136952286447, -0.03245661282391282, -0.3115281896078159};
        QTest::newRow("blue 29.56829715344471 68.28740665215547 -112.02971798617645") //
            << cmsCIELab({29.56829715344471, 68.28740665215547, -112.02971798617645}) //
            << Trio(oklabBlue);

        constexpr double oklabCyan[] = //
            {0.9053992412363845, -0.14944395453880494, -0.03939813576103679};
        QTest::newRow("cyan 90.66601315791455 -50.65651077286893 -14.961666625736525") //
            << cmsCIELab({90.66601315791455, -50.65651077286893, -14.961666625736525}) //
            << Trio(oklabCyan);

        constexpr double oklabMagenta[] = //
            {0.7016738534591195, 0.2745663787537365, -0.16915605971312353};
        QTest::newRow("magenta 60.16894098715946 93.53959546199253 -60.50080231921204") //
            << cmsCIELab({60.16894098715946, 93.53959546199253, -60.50080231921204}) //
            << Trio(oklabMagenta);

        constexpr double oklabYellow[] = //
            {0.9679827459780366, -0.0713691921107204, 0.1985698110545745};
        QTest::newRow("yellow 97.60701009682253 -15.749846639252663 93.39361164266089") //
            << cmsCIELab({97.60701009682253, -15.749846639252663, 93.39361164266089}) //
            << Trio(oklabYellow);

        constexpr double oklabBlack[] = //
            {0., 0., 0.};
        QTest::newRow("black 0. 0. 0.") //
            << cmsCIELab({0., 0., 0.}) //
            << Trio(oklabBlack);

        constexpr double oklabGray[] = //
            {0.599870811495933, -2.0178402559967168e-8, 4.1007266304848855e-8};
        QTest::newRow("gray 53.5850142898864 -0.0000046837680400813 0.00000405887623511347") //
            << cmsCIELab({53.5850142898864, -0.0000046837680400813, 0.00000405887623511347}) //
            << Trio(oklabGray);
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

    void testLch()
    {
        const auto myLch = GenericColor(51, 21, 1);
        GenericColor myLchResult = //
            AbsoluteColor::allConversions(ColorModel::CielabD50, myLch) //
                .value(ColorModel::CielabD50);
        QCOMPARE(myLchResult.first, 51);
        QCOMPARE(myLchResult.second, 21);
        QCOMPARE(myLchResult.third, 1);
        myLchResult = AbsoluteColor::convert(ColorModel::CielabD50, //
                                             myLch, //
                                             ColorModel::CielabD50) //
                          .value();
        QCOMPARE(myLchResult.first, 51);
        QCOMPARE(myLchResult.second, 21);
        QCOMPARE(myLchResult.third, 1);
    }

    void testFromXyzd65ToOklab_data()
    {
        generateDataXyzd65Oklab();
    }

    void testFromXyzd65ToOklab()
    {
        // Test pre-requirements:
        // The code of the function fromXyzD65ToOklab relies on the assumption
        // that std::cbrt() returns negative results for negative radicands,
        // and not simply “nan”. As std::cbrt() isn’t constexpr, we cannot
        // use a static assert within the function. Therefore, we have this
        // small pre-requirement test:
        const auto actual = std::cbrt(-27);
        const decltype(actual) expected = -3;
        QCOMPARE(actual, expected);

        // Actual unit test:
        QFETCH(double, x);
        QFETCH(double, y);
        QFETCH(double, z);
        const GenericColor xyz{x, y, z};
        QFETCH(Trio, oklab);
        const auto actualResult = AbsoluteColor::fromXyzD65ToOklab(xyz);
        constexpr double epsilon = 0.001;
        QVERIFY(isNearlyEqual(actualResult.first, oklab(0, 0), epsilon));
        QVERIFY(isNearlyEqual(actualResult.second, oklab(1, 0), epsilon));
        QVERIFY(isNearlyEqual(actualResult.third, oklab(2, 0), epsilon));
    }

    void testFromOklabToXyzd65_data()
    {
        generateDataXyzd65Oklab();
    }

    void testFromOklabToXyzd65()
    {
        QFETCH(double, x);
        QFETCH(double, y);
        QFETCH(double, z);
        QFETCH(Trio, oklab);
        const auto actualXyzD65 = AbsoluteColor::fromOklabToXyzD65(GenericColor(oklab));
        constexpr double epsilon = 0.001;
        QVERIFY(isNearlyEqual(actualXyzD65.first, x, epsilon));
        QVERIFY(isNearlyEqual(actualXyzD65.second, y, epsilon));
        QVERIFY(isNearlyEqual(actualXyzD65.third, z, epsilon));
    }

    void testFromCmscielabD50ToOklab_data()
    {
        generateDataCielabd50Oklab();
    }

    void testFromCmscielabD50ToOklab()
    {
        QFETCH(cmsCIELab, cmscielab);
        QFETCH(Trio, oklab);
        const auto actualResult = AbsoluteColor::convert( //
                                      ColorModel::CielabD50, //
                                      GenericColor(cmscielab), //
                                      ColorModel::OklabD65) //
                                      .value();
        constexpr double epsilon = 0.001;
        QVERIFY(isNearlyEqual(actualResult.first, oklab(0, 0), epsilon));
        QVERIFY(isNearlyEqual(actualResult.second, oklab(1, 0), epsilon));
        QVERIFY(isNearlyEqual(actualResult.third, oklab(2, 0), epsilon));
    }

    void testFromOklabToCmscielabD50_data()
    {
        generateDataCielabd50Oklab();
    }

    void testFromOklabToCmscielabD50()
    {
        QFETCH(cmsCIELab, cmscielab);
        QFETCH(Trio, oklab);
        const auto actualCielabD50 = AbsoluteColor::convert( //
                                         ColorModel::OklabD65, //
                                         GenericColor(GenericColor(oklab)), //
                                         ColorModel::CielabD50) //
                                         .value();
        // NOTE The original test data has been calculated converting from
        // CIELab-D50 to Oklab (which is always D65). Because of the different
        // whitepoint, a round-trip conversion of pure white is not possible.
        // As we use the same data to check the inverse conversion, we have
        // to choose a higher epsilon:
        constexpr double epsilon = 0.05;
        QVERIFY(isNearlyEqual(actualCielabD50.first, cmscielab.L, epsilon));
        QVERIFY(isNearlyEqual(actualCielabD50.second, cmscielab.a, epsilon));
        QVERIFY(isNearlyEqual(actualCielabD50.third, cmscielab.b, epsilon));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestAbsoluteColor)

// The following “include” is necessary because we do not use a header file:
#include "testabsolutecolor.moc"
