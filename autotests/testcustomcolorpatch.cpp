// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "customcolorpatch.h"
// Second, the private implementation.
#include "customcolorpatch_p.h" // IWYU pragma: keep

#include "colorpatch.h"
#include "completecolor.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "rgbcolor.h"
#include "sink.h"
#include <memory>
#include <qbytearray.h>
#include <qbytearrayview.h>
#include <qcolor.h>
#include <qmimedata.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qstringview.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>
#include <qvariant.h>

namespace PerceptualColor
{
class TestCustomColorPatch : public QObject
{
    Q_OBJECT

public:
    explicit TestCustomColorPatch(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QColor m_color;

    void helperReceiveSignals(QColor color)
    {
        m_color = color;
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
        CustomColorPatch myCustomColorPatch;
        sink(&myCustomColorPatch);
    }

    void testCreateMimeData()
    {
        {
            constexpr auto color1 = QColor(0x00, 0x12, 0xFF);
            std::unique_ptr<QMimeData> data1 //
                {CustomColorPatchPrivate::createMimeData(color1)};
            QCOMPARE(QStringLiteral("#0012FF"), data1->text());
            QVERIFY(data1->colorData().isValid());
            QCOMPARE(data1->colorData(), color1);
        }

        {
            constexpr auto color2 = QColor(0x00, 0x12, 0xFF, 0x56);
            std::unique_ptr<QMimeData> data2 //
                {CustomColorPatchPrivate::createMimeData(color2)};
            QCOMPARE(data2->text(), QStringLiteral("#0012FF56"));
            QVERIFY(data2->colorData().isValid());
            QCOMPARE(data2->colorData(), color2);
        }

        // Test correct rounding for hex representation

        {
            const auto color3 = QColor::fromRgbF(0x00 / 255.f, //
                                                 0x12 / 255.f, //
                                                 255 / 255.f, //
                                                 0x56 / 255.f);
            std::unique_ptr<QMimeData> data3 //
                {CustomColorPatchPrivate::createMimeData(color3)};
            QCOMPARE(data3->text(), QStringLiteral("#0012FF56"));
        }

        {
            const auto color4 = QColor::fromRgbF(0x00 / 255.f, //
                                                 0x12 / 255.f, //
                                                 254.6f / 255.f, //
                                                 0x56 / 255.f);
            std::unique_ptr<QMimeData> data4 //
                {CustomColorPatchPrivate::createMimeData(color4)};
            QCOMPARE(data4->text(), QStringLiteral("#0012FF56"));
        }

        {
            const auto color5 = QColor::fromRgbF(0x00 / 255.f, //
                                                 0x12 / 255.f, //
                                                 254.4f / 255.f, //
                                                 0x56 / 255.f);
            std::unique_ptr<QMimeData> data5 //
                {CustomColorPatchPrivate::createMimeData(color5)};
            QCOMPARE(data5->text(), QStringLiteral("#0012FE56"));
        }
    }

    void testFromMimeToCompleteColor()
    {
        const QMimeData myMimeData1;
        const auto myColorResult1 = //
            CustomColorPatchPrivate::fromMimeToCompleteColor(&myMimeData1);
        QVERIFY(!myColorResult1.isValid);
        QVERIFY(!myColorResult1.opaqueColorSRgb.rgb_QColor.isValid());

        QMimeData myMimeData2;
        myMimeData2.setColorData(QVariant());
        const auto myColorResult2 = //
            CustomColorPatchPrivate::fromMimeToCompleteColor(&myMimeData2);
        QVERIFY(!myColorResult2.isValid);
        QVERIFY(!myColorResult2.opaqueColorSRgb.rgb_QColor.isValid());

        QMimeData myMimeData3;
        myMimeData3.setColorData(QColor());
        const auto myColorResult3 = //
            CustomColorPatchPrivate::fromMimeToCompleteColor(&myMimeData3);
        QVERIFY(!myColorResult3.isValid);
        QVERIFY(!myColorResult3.opaqueColorSRgb.rgb_QColor.isValid());

        const QColor myColor4 = QColor::fromHsvF(0.5f, 0.6f, 0.7f);
        QMimeData myMimeData4;
        myMimeData4.setColorData(myColor4);
        const auto myColorResult4 = //
            CustomColorPatchPrivate::fromMimeToCompleteColor(&myMimeData4);
        QVERIFY(myColorResult4.isValid);
        QCOMPARE(myColorResult4.opaqueColorSRgb.rgb_QColor, myColor4.toRgb());

        QMimeData myMimeData5;
        myMimeData5.setColorData(QStringLiteral("test"));
        const auto myColorResult5 = //
            CustomColorPatchPrivate::fromMimeToCompleteColor(&myMimeData5);
        QVERIFY(!myColorResult5.isValid);
        QVERIFY(!myColorResult5.opaqueColorSRgb.rgb_QColor.isValid());

        const QColor myColor6 = QColor::fromHsvF(0.5f, 0.6f, 0.7f);
        QMimeData myMimeData6;
        myMimeData6.setColorData(myColor6);
        myMimeData6.setText(QStringLiteral("test"));
        const auto myColorResult6 = //
            CustomColorPatchPrivate::fromMimeToCompleteColor(&myMimeData6);
        QVERIFY(myColorResult6.isValid);
        QCOMPARE(myColorResult6.opaqueColorSRgb.rgb_QColor, myColor6.toRgb());

        QMimeData myMimeData7;
        myMimeData7.setText(QStringLiteral("rgb(20% 40% 60%)"));
        const auto myColorResult7 = //
            CustomColorPatchPrivate::fromMimeToCompleteColor(&myMimeData7);
        QCOMPARE(myColorResult7.opaqueColorSRgb.rgb_1, //
                 GenericColor(0.2, 0.4, 0.6));
    }

    void testGegl()
    {
        // roundtrip test

        const auto mimeData = CustomColorPatchPrivate::createMimeData(Qt::red);

        const auto myMimeTypeGegl = //
            CustomColorPatchPrivate::mimeTypeGegl.toString();
        QByteArray data;
        if (mimeData->hasFormat(myMimeTypeGegl)) {
            data = mimeData->data(myMimeTypeGegl);
        }
        const auto geglCompelteColor = //
            CustomColorPatchPrivate::fromGegelToCompleteColor( //
                QByteArrayView(data));

        const auto resultingQColor = geglCompelteColor.opaqueColorSRgb.rgb_QColor;

        QCOMPARE(resultingQColor, Qt::red);
    }

    void testSetColor()
    {
        // Initialization
        PerceptualColor::ColorPatch thePatch;

        thePatch.setColor(Qt::red);
        QCOMPARE(thePatch.color(), Qt::red);

        thePatch.setColor(QColor());
        QCOMPARE(thePatch.color(), QColor());

        thePatch.setColor(Qt::green);
        QCOMPARE(thePatch.color(), Qt::green);

        constexpr QColor semitransparentColor = QColor(250, 200, 150, 50);
        thePatch.setColor(semitransparentColor);
        QCOMPARE(thePatch.color(), semitransparentColor);
        QCOMPARE(thePatch.color().alpha(), 50);
    }

    void testColorProperty()
    {
        // Initialization
        CustomColorPatch thePatch;
        connect(&thePatch, //
                &PerceptualColor::CustomColorPatch::colorChanged, //
                this, //
                &TestCustomColorPatch::helperReceiveSignals);

        m_color = Qt::red;
        thePatch.setColor(QColor()); // invalid like initial value
        // We expect that no signal was emitted
        QCOMPARE(m_color, Qt::red);

        m_color = Qt::red;
        thePatch.setColor(Qt::blue); // new value
        // We expect that a signal was emitted
        QCOMPARE(m_color, Qt::blue);

        m_color = Qt::red;
        thePatch.setColor(Qt::blue); // is yet blue, set again to blue…
        // We expect that no signal was emitted
        QCOMPARE(m_color, Qt::red);

        m_color = Qt::red;
        thePatch.setColor(QColor()); // new: invalid color
        // We expect that a signal was emitted
        QCOMPARE(m_color, QColor());
    }

    void testIsTransparencyAllowedSetColor()
    {
        CustomColorPatch thePatch;
        QVERIFY(thePatch.d_pointer->m_isTransparencyAllowed);

        constexpr QColor semitransparentColor = QColor(250, 200, 150, 50);
        thePatch.setColor(semitransparentColor);
        QCOMPARE(thePatch.color(), semitransparentColor);
        QCOMPARE(thePatch.color().alpha(), 50);

        thePatch.setTransparencyAllowed(false);
        QCOMPARE(thePatch.color(), QColor(250, 200, 150, 255));
        QCOMPARE(thePatch.color().alpha(), 255);

        thePatch.setColor(QColor(240, 190, 140, 90));
        QCOMPARE(thePatch.color(), QColor(240, 190, 140, 255));
        QCOMPARE(thePatch.color().alpha(), 255);

        thePatch.setTransparencyAllowed(true);
        thePatch.setColor(QColor(230, 180, 130, 80));
        QCOMPARE(thePatch.color(), QColor(230, 180, 130, 80));
        QCOMPARE(thePatch.color().alpha(), 80);

        thePatch.setTransparencyAllowed(false);
        QCOMPARE(thePatch.color(), QColor(230, 180, 130, 255));
        QCOMPARE(thePatch.color().alpha(), 255);
    }

    void testIsTransparencyAllowedSetColorData()
    {
        CustomColorPatch thePatch;
        QVERIFY(thePatch.d_pointer->m_isTransparencyAllowed);

        {
            constexpr QColor semitransparentQColor = QColor(250, 200, 150, 50);
            const CompleteColor semitransparentCompleteColor = //
                CompleteColor::fromSRgbColor( //
                    RgbColor::fromRgb_QColor(semitransparentQColor), //
                    50 / 255.);
            const CompleteColor opaqueCompleteColor = //
                CompleteColor::fromSRgbColor( //
                    RgbColor::fromRgb_QColor(semitransparentQColor), //
                    255 / 255.);

            thePatch.setColorData(semitransparentCompleteColor);

            QCOMPARE(thePatch.colorData(), semitransparentCompleteColor);
            QCOMPARE(thePatch.color().alpha(), 50);
            QCOMPARE(thePatch.color(), semitransparentQColor);

            thePatch.setTransparencyAllowed(false);
            QCOMPARE(thePatch.color(), QColor(250, 200, 150, 255));
            QCOMPARE(thePatch.color().alpha(), 255);
            QCOMPARE(thePatch.colorData(), opaqueCompleteColor);
        }

        {
            constexpr QColor semitransparentQColor = QColor(240, 190, 140, 90);
            const CompleteColor semitransparentCompleteColor = //
                CompleteColor::fromSRgbColor( //
                    RgbColor::fromRgb_QColor(semitransparentQColor), //
                    50 / 255.);
            const CompleteColor opaqueCompleteColor = //
                CompleteColor::fromSRgbColor( //
                    RgbColor::fromRgb_QColor(semitransparentQColor), //
                    255 / 255.);

            thePatch.setColorData(semitransparentCompleteColor);
            QCOMPARE(thePatch.color(), QColor(240, 190, 140, 255));
            QCOMPARE(thePatch.color().alpha(), 255);
            QCOMPARE(thePatch.colorData(), opaqueCompleteColor);

            thePatch.setTransparencyAllowed(true);
            QCOMPARE(thePatch.color(), QColor(240, 190, 140, 255));
            QCOMPARE(thePatch.color().alpha(), 255);
            QCOMPARE(thePatch.colorData(), opaqueCompleteColor);
        }
    }

    void testSetExtraPrecision()
    {
        CustomColorPatch patch;
        patch.setExtraPrecision(3);
        QCOMPARE(patch.d_pointer->m_extraPrecision, 3);
        patch.setExtraPrecision(-1);
        QCOMPARE(patch.d_pointer->m_extraPrecision, 0);
        patch.setExtraPrecision(10);
        QCOMPARE(patch.d_pointer->m_extraPrecision, 9);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestCustomColorPatch)
#include "testcustomcolorpatch.moc" // necessary because we do not use a header file
