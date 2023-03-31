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
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelper)
// The following “include” is necessary because we do not use a header file:
#include "testhelper.moc"
