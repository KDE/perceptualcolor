// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "completecolor.h"

#include "absolutecolor.h"
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
class TestCompleteColor : public QObject
{
    Q_OBJECT

public:
    explicit TestCompleteColor(QObject *parent = nullptr)
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

    void testConstructor()
    {
        const CompleteColor myColor;
        QVERIFY(!myColor.isValid);
        QCOMPARE(myColor.alpha, 1);
    }

    void testFromAbsColor()
    {
        const auto originalSRgbColor = GenericColor(0.2, 0.3, 0.4);
        const auto absColor = AbsoluteColor::allConversions( //
            ColorModel::SRgb_1, //
            originalSRgbColor);
        const CompleteColor temp = CompleteColor::fromAbsColor(absColor, 0.5);

        QVERIFY(temp.isValid);
        QCOMPARE(temp.alpha, 0.5);
        QCOMPARE(temp.opaqueColorAbs[ColorModel::SRgb_1], originalSRgbColor);
        QCOMPARE(temp.opaqueColorSRgb.rgb_1, originalSRgbColor);
    }

    void testFromRgbColor()
    {
        const auto originalSRgbColor = GenericColor(0.2, 0.3, 0.4);
        const CompleteColor temp = CompleteColor::fromSRgbColor( //
            RgbColor::fromRgb_1(originalSRgbColor), //
            0.5);

        QVERIFY(temp.isValid);
        QCOMPARE(temp.alpha, 0.5);
        QCOMPARE(temp.opaqueColorAbs[ColorModel::SRgb_1], originalSRgbColor);
        QCOMPARE(temp.opaqueColorSRgb.rgb_1, originalSRgbColor);
    }

    void testQDebug()
    {
        const auto originalSRgbColor = GenericColor(0.2, 0.3, 0.4);
        const auto absColor = AbsoluteColor::allConversions( //
            ColorModel::SRgb_1, //
            originalSRgbColor);
        const CompleteColor temp = CompleteColor::fromAbsColor(absColor, 0.5);

        // Suppress warnings
        qInstallMessageHandler(voidMessageHandler);

        // Test that qDebug() does not crash
        qDebug() << CompleteColor();
        qDebug() << temp;

        // Do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestCompleteColor)

// The following “include” is necessary because we do not use a header file:
#include "testcompletecolor.moc"
