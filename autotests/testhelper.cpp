// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helper.h"

#include <qevent.h>
#include <qglobal.h>
#include <qimage.h>
#include <qlocale.h>
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

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

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
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelper)
// The following “include” is necessary because we do not use a header file:
#include "testhelper.moc"
