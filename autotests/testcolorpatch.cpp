// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "colorpatch.h"
// Second, the private implementation.
#include "colorpatch_p.h" // IWYU pragma: keep

#include "constpropagatinguniquepointer.h"
#include <QtCore/qglobal.h>
#include <memory>
#include <qcolor.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsize.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

static void snippet01()
{
    //! [ColorPatch Create widget]
    PerceptualColor::ColorPatch *m_patch = new PerceptualColor::ColorPatch;
    m_patch->setColor(Qt::blue);
    //! [ColorPatch Create widget]
    //! [ColorPatch Bigger minimum size]
    m_patch->setMinimumSize(QSize(50, 50));
    //! [ColorPatch Bigger minimum size]
    QCOMPARE(m_patch->color(), QColor(Qt::blue));
    delete m_patch;
}

namespace PerceptualColor
{
class TestColorPatch : public QObject
{
    Q_OBJECT

public:
    explicit TestColorPatch(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
    }

    void helperProvideQColors()
    {
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);

        QTest::addColumn<QColor>("color");
        QTest::newRow("RGB 1 2 3") << QColor(1, 2, 3);
        QTest::newRow("RGBA 1 2 3 4") << QColor(1, 2, 3, 4);
        QTest::newRow("RGB 1 2 300") << QColor(1, 2, 300);
        QTest::newRow("RGBA 1 2 300 4") << QColor(1, 2, 300, 4);

        QTest::newRow("RGB 0.1 0.2 0.3") << QColor::fromRgbF(0.1f, 0.2f, 0.3f);
        QTest::newRow("RGBA 0.1 0.2 0.3 0.4") << QColor::fromRgbF(0.1f, 0.2f, 0.3f, 0.4f);
        QTest::newRow("RGB 0.1 6.2 0.300") << QColor::fromRgbF(0.1f, 6.2f, 0.300f);
        QTest::newRow("RGBA 0.1 6.2 0.300 0.4") << QColor::fromRgbF(0.1f, 6.2f, 0.300f, 0.4f);

        QTest::newRow("CMYK 1 2 3 4") << QColor::fromCmyk(1, 2, 3, 4);
        QTest::newRow("CMYK 1 2 3 4 5") << QColor::fromCmyk(1, 2, 3, 4, 5);
        QTest::newRow("CMYK 1 2 300 4") << QColor::fromCmyk(1, 2, 300, 4);
        QTest::newRow("CMYK 1 2 300 4 5") << QColor::fromCmyk(1, 2, 300, 4, 5);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4") << QColor::fromCmykF(0.1f, 0.2f, 0.300f, 0.4f);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4 0.6495217645f") << QColor::fromCmykF(0.1f, 0.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4") << QColor::fromCmykF(0.1f, 6.2f, 0.300f, 0.4f);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4 0.6495217645f") << QColor::fromCmykF(0.1f, 6.2f, 0.300f, 0.4f, 0.6495217645f);

        QTest::newRow("HSL 2 3 4") << QColor::fromHsl(2, 3, 4);
        QTest::newRow("HSL 2 3 4 5") << QColor::fromHsl(2, 3, 4, 5);
        QTest::newRow("HSL 2 300 4") << QColor::fromHsl(2, 300, 4);
        QTest::newRow("HSL 2 300 4 5") << QColor::fromHsl(2, 300, 4, 5);
        QTest::newRow("HSL 0.2 0.300 0.4") << QColor::fromHslF(0.2f, 0.300f, 0.4f);
        QTest::newRow("HSL 0.2 0.300 0.4 0.6495217645") << QColor::fromHslF(0.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("HSL 6.2 0.300 0.4") << QColor::fromHslF(6.2f, 0.300f, 0.4f);
        QTest::newRow("HSL 6.2 0.300 0.4 0.6495217645") << QColor::fromHslF(6.2f, 0.300f, 0.4f, 0.6495217645f);

        QTest::newRow("HSV 2 3 4") << QColor::fromHsv(2, 3, 4);
        QTest::newRow("HSV 2 3 4 5") << QColor::fromHsv(2, 3, 4, 5);
        QTest::newRow("HSV 2 300 4") << QColor::fromHsv(2, 300, 4);
        QTest::newRow("HSV 2 300 4 5") << QColor::fromHsv(2, 300, 4, 5);
        QTest::newRow("HSV 0.2 0.300 0.4") << QColor::fromHsvF(0.2f, 0.300f, 0.4f);
        QTest::newRow("HSV 0.2 0.300 0.4 0.6495217645") << QColor::fromHsvF(0.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("HSV 6.2 0.300 0.4") << QColor::fromHsvF(6.2f, 0.300f, 0.4f);
        QTest::newRow("HSV 6.2 0.300 0.4 0.6495217645") << QColor::fromHsvF(6.2f, 0.300f, 0.4f, 0.6495217645f);

        QTest::newRow("invalid") << QColor();

        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    QColor m_color;

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

    void testInitialazation()
    {
        PerceptualColor::ColorPatch thePatch;
        // Test initial value (an invalid color following the documentation)
        QCOMPARE(thePatch.color(), QColor());
    }

    void testM_color()
    {
        PerceptualColor::ColorPatch thePatch;
        thePatch.setColor(Qt::red);
        QCOMPARE(thePatch.color(), Qt::red);
        QCOMPARE(thePatch.d_pointer->m_color, Qt::red);
    }

    void testApplyColors()
    {
        PerceptualColor::ColorPatch thePatch;
        // Test initial value (an invalid color following the documentation)
        QCOMPARE(thePatch.color(), QColor());
    }

    void testColorProperty_data()
    {
        helperProvideQColors();
    }

    void testColorProperty()
    {
        QFETCH(QColor, color);
        PerceptualColor::ColorPatch thePatch;
        thePatch.setColor(color);
        QCOMPARE(thePatch.color(), color);
    }

    void helperReceiveSignals(QColor color)
    {
        m_color = color;
    }

    void testColorChanged()
    {
        // Initialization
        PerceptualColor::ColorPatch thePatch;
        connect(&thePatch, &PerceptualColor::ColorPatch::colorChanged, this, &TestColorPatch::helperReceiveSignals);

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

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in immediate
        // re-paint.
        ColorPatch myWidget;
        myWidget.show();
        myWidget.resize(QSize());
        myWidget.repaint();
        myWidget.resize(QSize(-1, -1));
        myWidget.repaint();
        myWidget.resize(QSize(-1, 0));
        myWidget.repaint();
        myWidget.resize(QSize(0, -1));
        myWidget.repaint();
        myWidget.resize(QSize(0, 1));
        myWidget.repaint();
        myWidget.resize(QSize(1, 0));
        myWidget.repaint();
        myWidget.resize(QSize(1, 1));
        myWidget.repaint();
        myWidget.resize(QSize(2, 2));
        myWidget.repaint();
        myWidget.resize(QSize(3, 3));
        myWidget.repaint();
        myWidget.resize(QSize(4, 4));
        myWidget.repaint();
        myWidget.resize(QSize(5, 5));
        myWidget.repaint();
        myWidget.resize(QSize(6, 6));
        myWidget.repaint();
        myWidget.resize(QSize(7, 7));
        myWidget.repaint();
        myWidget.resize(QSize(8, 8));
        myWidget.repaint();
        myWidget.resize(QSize(9, 9));
        myWidget.repaint();
        myWidget.resize(QSize(10, 10));
        myWidget.repaint();
        myWidget.resize(QSize(11, 11));
        myWidget.repaint();
        myWidget.resize(QSize(12, 12));
        myWidget.repaint();
        myWidget.resize(QSize(13, 13));
        myWidget.repaint();
        myWidget.resize(QSize(14, 14));
        myWidget.repaint();
    }

    void testSnippet01()
    {
        snippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorPatch)
#include "testcolorpatch.moc" // necessary because we do not use a header file
