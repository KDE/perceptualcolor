// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helper.h"

#include <lcms2.h>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qevent.h>
#include <qglobal.h>
#include <qicon.h>
#include <qimage.h>
#include <qlist.h>
#include <qlocale.h>
#include <qmap.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpair.h>
#include <qpoint.h>
#include <qsize.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qwidget.h>
#include <rgbcolorspacefactory.h>
#include <type_traits>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

// From Qt documentation:
//     “Note: This function is not declared in any of Qt's header files. To
//      use it in your application, declare the function prototype before
//      calling it.”
void qt_set_sequence_auto_mnemonic(bool b);

static void snippet01()
{
    int test = 5;
    //! [isInUsage]
    // Having a variable “int test”:
    const bool result = PerceptualColor::isIn(test, 1, 2, 8);
    //! [isInUsage]
    Q_UNUSED(result)
}

static void snippet02()
{
    //! [percentTraditional]
    QSpinBox box1;
    box1.setSuffix(QStringLiteral("%"));
    //! [percentTraditional]
    //! [percentImproved]
    QSpinBox box2;
    QLocale locale;
    box2.setSuffix(locale.percent());
    //! [percentImproved]
    //! [percentFullyInternationalized]
    QSpinBox box3;
    const QPair<QString, QString> temp = //
        PerceptualColor::getPrefixSuffix(QObject::tr("%1%"));
    box3.setPrefix(temp.first);
    box3.setSuffix(temp.second);
    //! [percentFullyInternationalized]
    if (box3.prefix() != QString()) {
        throw 0;
    }
    if (box3.suffix() != QStringLiteral("%")) {
        throw 0;
    }
}

class EnumTestClass : QObject
{
    Q_OBJECT
public:
    enum class Normal { value0, value1, value2 };
    Q_ENUM(Normal);

    enum class Defined { value0 = 0, value1 = 1, value2 = 2 };
    Q_ENUM(Defined);

    enum class Shifted { value0 = 10, value1 = 11, value2 = 12 };
    Q_ENUM(Shifted);

    enum class Missing { value0 = 00, value1 = 10, value2 = 20 };
    Q_ENUM(Missing);

    enum class DoubledValue { value0 = 0, value1 = 1, value2 = 1 };
    Q_ENUM(DoubledValue);

    enum class DoubledKey { value0 = 0, value1 = 1, value2 = value1 };
    Q_ENUM(DoubledKey);
};

namespace MyNamespace
{
Q_NAMESPACE

enum class NamespaceEnum { value0, value1, value2 };
Q_ENUM_NS(NamespaceEnum);
}

namespace PerceptualColor
{

class TestHelper : public QObject
{
    Q_OBJECT

public:
    explicit TestHelper(QObject *parent = nullptr)
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

    void testTransparencyBackground()
    {
        QImage temp = transparencyBackground(1);
        QVERIFY2(temp.size().width() > 0, "Width of image is bigger than 0.");
        QVERIFY2(temp.size().height() > 0, "Height of image is bigger than 0.");
        QVERIFY2(temp.allGray(), "Image is neutral gray.");
    }

    void testStandardWheelSteps()
    {
        QWheelEvent temp( //
            QPointF(), // pos
            QPointF(), // globalPos
            QPoint(), // pixelDelta
            QPoint(200, 120), // angleDelta
            Qt::MouseButton::MiddleButton, // buttons
            Qt::KeyboardModifier::NoModifier, // modifiers
            Qt::ScrollUpdate, // phase
            false // inverted
        );
        QCOMPARE(PerceptualColor::standardWheelStepCount(&temp), 1);
    }

    void testSnippet01()
    {
        snippet01();
    }

    void testSnippet02()
    {
        snippet02();
    }

    void testIsIn()
    {
        QCOMPARE(isIn(5, 6), false);
        QCOMPARE(isIn(5), false);
        QCOMPARE(isIn(0), false);
        QCOMPARE(isIn(0, 1), false);
        QCOMPARE(isIn(5, 6, 5), true);
        QCOMPARE(isIn(5, 6, 5, 5), true);
        QCOMPARE(isIn(5, 6, 6, 5, 5), true);
        QCOMPARE(isIn(0, 6, 6, 5, 5), false);
        QCOMPARE(isIn(0, 6, 6, 5, 0, 5), true);
        QCOMPARE(isIn(QStringLiteral("0"), //
                      QStringLiteral("0"),
                      QStringLiteral("1")),
                 true);
        QCOMPARE(isIn(QStringLiteral("0"), //
                      QStringLiteral("4"),
                      QStringLiteral("1")),
                 false);
    }

    void testEnumeratorToFullStringNormal()
    {
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Normal>(-1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::Normal::value0), //
                 QStringLiteral("EnumTestClass::Normal::value0(0)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::Normal::value1), //
                 QStringLiteral("EnumTestClass::Normal::value1(1)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::Normal::value2), //
                 QStringLiteral("EnumTestClass::Normal::value2(2)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Normal>(3)), //
                 QString());
    }

    void testEnumeratorToFullStringDefined()
    {
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Defined>(-1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::Defined::value0), //
                 QStringLiteral("EnumTestClass::Defined::value0(0)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::Defined::value1), //
                 QStringLiteral("EnumTestClass::Defined::value1(1)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::Defined::value2), //
                 QStringLiteral("EnumTestClass::Defined::value2(2)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Defined>(3)), //
                 QString());
    }

    void testEnumeratorToFullStringShifted()
    {
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Shifted>(-1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Shifted>(0)), //
                 QString());
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Shifted>(1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Shifted>(9)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::Shifted::value0), //
                 QStringLiteral("EnumTestClass::Shifted::value0(10)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::Shifted::value1), //
                 QStringLiteral("EnumTestClass::Shifted::value1(11)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::Shifted::value2), //
                 QStringLiteral("EnumTestClass::Shifted::value2(12)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Shifted>(13)), //
                 QString());
    }

    void testEnumeratorToFullStringMissing()
    {
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Missing>(-1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::Missing::value0), //
                 QStringLiteral("EnumTestClass::Missing::value0(0)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Missing>(1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Missing>(9)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::Missing::value1), //
                 QStringLiteral("EnumTestClass::Missing::value1(10)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Missing>(11)), //
                 QString());
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Missing>(19)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::Missing::value2), //
                 QStringLiteral("EnumTestClass::Missing::value2(20)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::Missing>(21)), //
                 QString());
    }

    void testEnumeratorToFullStringDoubledValue()
    {
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::DoubledValue>(-1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::DoubledValue::value0), //
                 QStringLiteral("EnumTestClass::DoubledValue::value0(0)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::DoubledValue::value1), //
                 QStringLiteral("EnumTestClass::DoubledValue::value1|value2(1)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::DoubledValue::value2), //
                 QStringLiteral("EnumTestClass::DoubledValue::value1|value2(1)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::DoubledValue>(2)), //
                 QString());
    }

    void testEnumeratorToFullStringDoubledKey()
    {
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::DoubledKey>(-1)), //
                 QString());
        QCOMPARE(enumeratorToFullString(EnumTestClass::DoubledKey::value0), //
                 QStringLiteral("EnumTestClass::DoubledKey::value0(0)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::DoubledKey::value1), //
                 QStringLiteral("EnumTestClass::DoubledKey::value1|value2(1)"));
        QCOMPARE(enumeratorToFullString(EnumTestClass::DoubledKey::value2), //
                 QStringLiteral("EnumTestClass::DoubledKey::value1|value2(1)"));
        QCOMPARE(enumeratorToFullString(static_cast<EnumTestClass::DoubledKey>(2)), //
                 QString());
    }

    void testEnumerationToFullString()
    {
        QCOMPARE(enumerationToFullString<EnumTestClass::DoubledKey>(), //
                 QStringLiteral("EnumTestClass::DoubledKey"));
        QCOMPARE(enumerationToFullString<MyNamespace::NamespaceEnum>(), //
                 QStringLiteral("MyNamespace::NamespaceEnum"));
    }

    void testEnumeratorToStringNormal()
    {
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Normal>(-1)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::Normal::value0), //
                 QStringLiteral("value0(0)"));
        QCOMPARE(enumeratorToString(EnumTestClass::Normal::value1), //
                 QStringLiteral("value1(1)"));
        QCOMPARE(enumeratorToString(EnumTestClass::Normal::value2), //
                 QStringLiteral("value2(2)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Normal>(3)), //
                 QString());
    }

    void testEnumeratorToStringDefined()
    {
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Defined>(-1)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::Defined::value0), //
                 QStringLiteral("value0(0)"));
        QCOMPARE(enumeratorToString(EnumTestClass::Defined::value1), //
                 QStringLiteral("value1(1)"));
        QCOMPARE(enumeratorToString(EnumTestClass::Defined::value2), //
                 QStringLiteral("value2(2)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Defined>(3)), //
                 QString());
    }

    void testEnumeratorToStringShifted()
    {
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Shifted>(-1)), //
                 QString());
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Shifted>(0)), //
                 QString());
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Shifted>(1)), //
                 QString());
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Shifted>(9)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::Shifted::value0), //
                 QStringLiteral("value0(10)"));
        QCOMPARE(enumeratorToString(EnumTestClass::Shifted::value1), //
                 QStringLiteral("value1(11)"));
        QCOMPARE(enumeratorToString(EnumTestClass::Shifted::value2), //
                 QStringLiteral("value2(12)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Shifted>(13)), //
                 QString());
    }

    void testEnumeratorToStringMissing()
    {
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Missing>(-1)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::Missing::value0), //
                 QStringLiteral("value0(0)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Missing>(1)), //
                 QString());
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Missing>(9)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::Missing::value1), //
                 QStringLiteral("value1(10)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Missing>(11)), //
                 QString());
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Missing>(19)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::Missing::value2), //
                 QStringLiteral("value2(20)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::Missing>(21)), //
                 QString());
    }

    void testEnumeratorToStringDoubledValue()
    {
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::DoubledValue>(-1)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::DoubledValue::value0), //
                 QStringLiteral("value0(0)"));
        QCOMPARE(enumeratorToString(EnumTestClass::DoubledValue::value1), //
                 QStringLiteral("value1|value2(1)"));
        QCOMPARE(enumeratorToString(EnumTestClass::DoubledValue::value2), //
                 QStringLiteral("value1|value2(1)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::DoubledValue>(2)), //
                 QString());
    }

    void testEnumeratorToStringDoubledKey()
    {
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::DoubledKey>(-1)), //
                 QString());
        QCOMPARE(enumeratorToString(EnumTestClass::DoubledKey::value0), //
                 QStringLiteral("value0(0)"));
        QCOMPARE(enumeratorToString(EnumTestClass::DoubledKey::value1), //
                 QStringLiteral("value1|value2(1)"));
        QCOMPARE(enumeratorToString(EnumTestClass::DoubledKey::value2), //
                 QStringLiteral("value1|value2(1)"));
        QCOMPARE(enumeratorToString(static_cast<EnumTestClass::DoubledKey>(2)), //
                 QString());
    }

    void testQIconFromTheme()
    {
        // Test that this does not crash.
        const QIcon icon = qIconFromTheme(QStringList(), //
                                          QStringLiteral("eye-exclamation"),
                                          ColorSchemeType::Light);
        Q_UNUSED(icon)
    }

    void testFromMnemonicToRichTextWithAutoMnemonic()
    {
        // Make sure to have mnemonics (like Qt::ALT+Qt::Key_X for "E&xit")
        // enabled, also on platforms that disable it by default.
        qt_set_sequence_auto_mnemonic(true);
        QCOMPARE(fromMnemonicToRichText(QString()), //
                 QString());
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("A")), //
                 QStringLiteral("A"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&A")), //
                 QStringLiteral("<u>A</u>"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("x&A")), //
                 QStringLiteral("x<u>A</u>"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&Ax")), //
                 QStringLiteral("<u>A</u>x"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("& ")), //
                 QStringLiteral("<u> </u>"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&Ax&A")), //
                 QStringLiteral("<u>A</u>x<u>A</u>"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&&")), //
                 QStringLiteral("&"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&A&&x")), //
                 QStringLiteral("<u>A</u>&x"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&A&&x")), //
                 QStringLiteral("<u>A</u>&x"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("T&est && T&es&t")), //
                 QStringLiteral("T<u>e</u>st & T<u>e</u>s<u>t</u>"));
    }

    void testFromMnemonicToRichTextWithoutAutoMnemonic()
    {
        // Make sure to have mnemonics (like Qt::ALT+Qt::Key_X for "E&xit")
        // disabled, also on platforms that enable it by default.
        qt_set_sequence_auto_mnemonic(false);
        QCOMPARE(fromMnemonicToRichText(QString()), //
                 QString());
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("A")), //
                 QStringLiteral("A"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&A")), //
                 QStringLiteral("A"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("x&A")), //
                 QStringLiteral("xA"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&Ax")), //
                 QStringLiteral("Ax"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("& ")), //
                 QStringLiteral(" "));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&Ax&A")), //
                 QStringLiteral("AxA"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&&")), //
                 QStringLiteral("&"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&A&&x")), //
                 QStringLiteral("A&x"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("&A&&x")), //
                 QStringLiteral("A&x"));
        QCOMPARE(fromMnemonicToRichText(QStringLiteral("T&est && T&es&t")), //
                 QStringLiteral("Test & Test"));
    }

    void testGuessColorSchemeTypeFromWidget()
    {
        QWidget myWidget;

        // Should not crash on nullptr
        guessColorSchemeTypeFromWidget(nullptr);

        // Should not crash on normal widget.
        guessColorSchemeTypeFromWidget(&myWidget);
    }

    void testWcsBasicColors()
    {
        const auto colors = wcsBasicColors( //
            RgbColorSpaceFactory::createSrgb());
        QCOMPARE(
            // For test, choose a value that is not in the center neither
            // horizontally nor vertically; this might detect mirroring bugs.
            colors.value(3, 3),
            QColor(0, 112, 50));
    }

    void testArray2DConstructor()
    {
        Array2D<int> test01(1, 2);
        Array2D<int> test02(0, 2);
        Array2D<int> test03(0, 0);
    }

    void testArray2DConstructorInit()
    {
        Array2D<int> test(2,
                          3,
                          {
                              // clang-format off
                              1, 2,
                              3, 4,
                              5, 6,
                              7 // excess element
                                // clang-format on
                          });
        QCOMPARE(test.iCount(), 2);
        QCOMPARE(test.jCount(), 3);
        QCOMPARE(test.value(0, 0), 1);
        QCOMPARE(test.value(1, 0), 2);
        QCOMPARE(test.value(0, 1), 3);
        QCOMPARE(test.value(1, 1), 4);
        QCOMPARE(test.value(0, 2), 5);
        QCOMPARE(test.value(1, 2), 6);
        // Invalid indices should return default-constructed values, and
        // not the excess element.
        QCOMPARE(test.value(2, 2), 0);
        QCOMPARE(test.value(1, 3), 0);

        Array2D<int> testIncompleteInit(2,
                                        3,
                                        {
                                            // clang-format off
                                            1, 2,
                                            3, 4,
                                            5
                                            // clang-format on
                                        });
        // Non-initialized indices should return default-constructed values.
        QCOMPARE(testIncompleteInit.value(1, 2), 0);
    }

    void testArray2DSetValue()
    {
        Array2D<int> test(2, 3);
        QCOMPARE(test.value(1, 2), 0);
        test.setValue(1, 2, 10);
        QCOMPARE(test.value(1, 2), 10);
        test.setValue(1, 2, 20);
        QCOMPARE(test.value(1, 2), 20);
    }

    void testArray2DIsInRange()
    {
        Array2D<int> test(2, 3);
        QCOMPARE(test.isInRange(-1, -1), false);
        QCOMPARE(test.isInRange(-1, 0), false);
        QCOMPARE(test.isInRange(0, -1), false);
        QCOMPARE(test.isInRange(0, 0), true);
        QCOMPARE(test.isInRange(1, 2), true);
        QCOMPARE(test.isInRange(1, 3), false);
        QCOMPARE(test.isInRange(2, 2), false);
    }

    void testLcmsIntentList()
    {
        QVERIFY(lcmsIntentList().contains(INTENT_PERCEPTUAL));
        QVERIFY(lcmsIntentList().contains(INTENT_RELATIVE_COLORIMETRIC));
        QVERIFY(lcmsIntentList().contains(INTENT_SATURATION));
        QVERIFY(lcmsIntentList().contains(INTENT_ABSOLUTE_COLORIMETRIC));
    }

    void testToOpaqueColorValidColor()
    {
        const QColor red = Qt::red;
        QColorArray2D myArray(1, 1);

        myArray.setValue(0, 0, red);
        QCOMPARE(toOpaque(myArray).value(0, 0), red);

        QColor temp = red;
        temp.setAlphaF(0.5);
        myArray.setValue(0, 0, temp);
        QCOMPARE(toOpaque(myArray).value(0, 0), red);
    }

    void testToOpaqueColorInvalidColor()
    {
        QColorArray2D myArray(1, 1);

        myArray.setValue(0, 0, QColor());
        QVERIFY2(!toOpaque(myArray).value(0, 0).isValid(), //
                 "Invalid input colors must stay invalid after processing.");
    }

    void testSplitElements()
    {
        // -2 elements
        // If there are no elements, the list of segments is also empty.
        auto result = splitElements(-2, -2);
        QCOMPARE(result.size(), 0);
        result = splitElements(-2, -1);
        QCOMPARE(result.size(), 0);
        result = splitElements(-2, 0);
        QCOMPARE(result.size(), 0);
        result = splitElements(-2, 1);
        QCOMPARE(result.size(), 0);
        result = splitElements(-2, 2);
        QCOMPARE(result.size(), 0);
        result = splitElements(-2, 3);
        QCOMPARE(result.size(), 0);

        // -1 element
        // If there are no elements, the list of segments is also empty.
        result = splitElements(-1, -2);
        QCOMPARE(result.size(), 0);
        result = splitElements(-1, -1);
        QCOMPARE(result.size(), 0);
        result = splitElements(-1, 0);
        QCOMPARE(result.size(), 0);
        result = splitElements(-1, 1);
        QCOMPARE(result.size(), 0);
        result = splitElements(-1, 2);
        QCOMPARE(result.size(), 0);
        result = splitElements(-1, 3);
        QCOMPARE(result.size(), 0);

        // 0 elements
        // If there are no elements, the list of segments is also empty.
        result = splitElements(0, -2);
        QCOMPARE(result.size(), 0);
        result = splitElements(0, -1);
        QCOMPARE(result.size(), 0);
        result = splitElements(0, 0);
        QCOMPARE(result.size(), 0);
        result = splitElements(0, 1);
        QCOMPARE(result.size(), 0);
        result = splitElements(0, 2);
        QCOMPARE(result.size(), 0);
        result = splitElements(0, 3);
        QCOMPARE(result.size(), 0);

        // 1 element
        result = splitElements(1, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        result = splitElements(1, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        result = splitElements(1, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        result = splitElements(1, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        result = splitElements(1, 2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        result = splitElements(1, 3);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);

        // 2 elements
        result = splitElements(2, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 1);
        result = splitElements(2, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 1);
        result = splitElements(2, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 1);
        result = splitElements(2, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 1);
        result = splitElements(2, 2);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        QCOMPARE(result.at(1).first, 1);
        QCOMPARE(result.at(1).second, 1);
        result = splitElements(2, 3);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        QCOMPARE(result.at(1).first, 1);
        QCOMPARE(result.at(1).second, 1);

        // 3 elements
        result = splitElements(3, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 2);
        result = splitElements(3, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 2);
        result = splitElements(3, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 2);
        result = splitElements(3, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 2);
        result = splitElements(3, 2);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 1);
        QCOMPARE(result.at(1).first, 2);
        QCOMPARE(result.at(1).second, 2);
        result = splitElements(3, 3);
        QCOMPARE(result.size(), 3);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 0);
        QCOMPARE(result.at(1).first, 1);
        QCOMPARE(result.at(1).second, 1);
        QCOMPARE(result.at(2).first, 2);
        QCOMPARE(result.at(2).second, 2);

        // 4 elements
        result = splitElements(4, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 3);
        result = splitElements(4, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 3);
        result = splitElements(4, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 3);
        result = splitElements(4, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 3);
        result = splitElements(4, 2);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 1);
        QCOMPARE(result.at(1).first, 2);
        QCOMPARE(result.at(1).second, 3);
        result = splitElements(4, 3);
        QCOMPARE(result.size(), 3);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 1);
        QCOMPARE(result.at(1).first, 2);
        QCOMPARE(result.at(1).second, 2);
        QCOMPARE(result.at(2).first, 3);
        QCOMPARE(result.at(2).second, 3);

        // further tests
        result = splitElements(15, 3);
        QCOMPARE(result.at(0).first, 0);
        QCOMPARE(result.at(0).second, 4);
        QCOMPARE(result.at(1).first, 5);
        QCOMPARE(result.at(1).second, 9);
        QCOMPARE(result.at(2).first, 10);
        QCOMPARE(result.at(2).second, 14);
    }

    void testSplitList()
    {
        QList<int> myList;

        // If the original list is empty, the list of parts is also empty.
        auto result = splitList(myList, -2);
        QCOMPARE(result.size(), 0);
        result = splitList(myList, -1);
        QCOMPARE(result.size(), 0);
        result = splitList(myList, 0);
        QCOMPARE(result.size(), 0);
        result = splitList(myList, 1);
        QCOMPARE(result.size(), 0);
        result = splitList(myList, 2);
        QCOMPARE(result.size(), 0);

        myList.append(100);
        result = splitList(myList, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 2);
        // Splitting 1 element into 2 parts not possible, resulting in only
        // one element.
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0).at(0), 100);

        myList.append(101);
        result = splitList(myList, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 2);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).size(), 1);
        QCOMPARE(result.at(0).at(0), 100);
        QCOMPARE(result.at(1).size(), 1);
        QCOMPARE(result.at(1).at(0), 101);

        myList.append(102);
        result = splitList(myList, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 2);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).size(), 2);
        QCOMPARE(result.at(0).at(0), 100);
        QCOMPARE(result.at(0).at(1), 101);
        QCOMPARE(result.at(1).size(), 1);
        QCOMPARE(result.at(1).at(0), 102);

        myList.append(103);
        result = splitList(myList, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 2);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).size(), 2);
        QCOMPARE(result.at(0).at(0), 100);
        QCOMPARE(result.at(0).at(1), 101);
        QCOMPARE(result.at(1).size(), 2);
        QCOMPARE(result.at(1).at(0), 102);
        QCOMPARE(result.at(1).at(1), 103);

        myList.append(104);
        result = splitList(myList, -2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, -1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 0);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 1);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.at(0), myList);
        result = splitList(myList, 2);
        QCOMPARE(result.size(), 2);
        QCOMPARE(result.at(0).size(), 3);
        QCOMPARE(result.at(0).at(0), 100);
        QCOMPARE(result.at(0).at(1), 101);
        QCOMPARE(result.at(0).at(2), 102);
        QCOMPARE(result.at(1).size(), 2);
        QCOMPARE(result.at(1).at(0), 103);
        QCOMPARE(result.at(1).at(1), 104);
    }

    void testSplitElementsTaperedValidInput()
    {
        QList<QPair<int, int>> result = splitElementsTapered(100, 5, 1, 0.1);
        QCOMPARE(result.size(), 5);

        int totalElements = 0;
        for (const auto &segment : std::as_const(result)) {
            // cppcheck-suppress useStlAlgorithm
            totalElements += segment.second - segment.first + 1;
        }
        QCOMPARE(totalElements, 100); // Ensure all elements are used
    }

    void testSplitElementsTaperedSingleSegment()
    {
        QList<QPair<int, int>> result = splitElementsTapered(100, 1, 1, 0.2);
        QCOMPARE(result.size(), 1);
        QCOMPARE(result.first().first, 0);
        QCOMPARE(result.first().second, 99); // Should span entire range
    }

    void testSplitElementsTaperedAlignmentEnforcement()
    {
        QList<QPair<int, int>> result = splitElementsTapered(100, 7, 10, 0.3);

        for (const auto &segment : std::as_const(result)) {
            QCOMPARE(segment.first % 10, 0); // Start index must be a multiple of alignment
        }
    }

    void testSplitElementsTaperedEdgeCases()
    {
        // Zero elements
        QList<QPair<int, int>> result = splitElementsTapered(0, 5, 1, 0.4);
        QVERIFY(result.isEmpty());
        result = splitElementsTapered(0, 0, 0, 0.5);
        QVERIFY(result.isEmpty());

        // Zero segments
        result = splitElementsTapered(100, 0, 1, 0.6);
        QCOMPARE(result.size(), 1); // Should default to one segment
        QCOMPARE(result.first().first, 0);
        QCOMPARE(result.first().second, 99); // Should span entire range

        // Zero alignment
        result = splitElementsTapered(100, 5, 0, 0.7);
        QCOMPARE(result.size(), 5); // Should default to alignment of 1
        QCOMPARE(result.first().first, 0);
        QCOMPARE(result.last().second, 99); // Should span entire range
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelper)
// The following “include” is necessary because we do not use a header file:
#include "testhelper.moc"
