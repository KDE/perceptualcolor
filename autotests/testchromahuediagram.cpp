// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "chromahuediagram.h"
// Second, the private implementation.
#include "chromahuediagram_p.h" // IWYU pragma: keep

#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "polarpointf.h"
#include "rgbcolorspacefactory.h"
#include <lcms2.h>
#include <qcoreevent.h>
#include <qevent.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qscopedpointer.h>
#include <qsharedpointer.h>
#include <qsignalspy.h>
#include <qsize.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

static void snippet01()
{
    //! [instantiate]
    auto myColorSpace = PerceptualColor::RgbColorSpaceFactory::createSrgb();
    PerceptualColor::ChromaHueDiagram *myDiagram = new PerceptualColor::ChromaHueDiagram(myColorSpace);
    PerceptualColor::GenericColor myColor;
    myColor.third = 270;
    myColor.first = 50;
    myColor.second = 25;
    myDiagram->setCurrentColorCielchD50(myColor);
    myDiagram->show();
    //! [instantiate]
    delete myDiagram;
}

namespace PerceptualColor
{
class RgbColorSpace;

class TestChromaHueDiagram : public QObject
{
    Q_OBJECT

public:
    explicit TestChromaHueDiagram(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace = RgbColorSpaceFactory::createSrgb();

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

    void testConstructorAndDestructor()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
    }

    void testShow()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.show();
    }

    void testKeyPressEvent()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        GenericColor referenceColorLch;
        referenceColorLch.first = 50;
        referenceColorLch.second = 0;
        referenceColorLch.third = 180;
        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        if (myDiagram.currentColorCielchD50().third != 180) {
            throw 0;
        }
        if (myDiagram.currentColorCielchD50().second != 0) {
            throw 0;
        }
        GenericColor referenceColorChromaLch;
        referenceColorChromaLch.first = 50;
        referenceColorChromaLch.second = 10;
        referenceColorChromaLch.third = 180;
        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);

        QScopedPointer<QKeyEvent> myEvent; // TODO Use QTest::keyClick() instead!
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier));

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().second > 0, "Test Key_Up");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().second > 0, "Test Key_PageUp");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().second < 10, "Test Key_Down");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().second < 10, "Test Key_PageDown");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().second >= 0, "Test Key_Down never negative");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().second >= 0, "Test Key_PageDown never negative");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().third > 180, "Test Key_Left");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().third > 180, "Test Key_Home");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().third < 180, "Test Key_Right");

        myDiagram.setCurrentColorCielchD50(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorCielchD50().third < 180, "Test Key_End");
    }

    void testMinimumSizeHint()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QVERIFY2(myDiagram.minimumSizeHint().width() > 0, "minimalSizeHint width is implemented.");
        QVERIFY2(myDiagram.minimumSizeHint().height() > 0, "minimalSizeHint height is implemented.");
        // Check that the hint is a square:
        QCOMPARE(myDiagram.minimumSizeHint().width(), myDiagram.minimumSizeHint().height());
    }

    void testSizeHint()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QVERIFY2(myDiagram.sizeHint().width() > myDiagram.minimumSizeHint().width(), "sizeHint width is bigger than minimalSizeHint width.");
        QVERIFY2(myDiagram.sizeHint().height() > myDiagram.minimumSizeHint().height(), "sizeHint height is bigger than minimalSizeHint height.");
        // Check that the hint is a square:
        QCOMPARE(myDiagram.minimumSizeHint().width(), myDiagram.minimumSizeHint().height());
    }

    void testColorProperty()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        QSignalSpy mySpy(&myDiagram, &PerceptualColor::ChromaHueDiagram::currentColorCielchD50Changed);
        GenericColor referenceColorLch;
        referenceColorLch.first = 50;
        referenceColorLch.second = 10;
        referenceColorLch.third = 180;

        // Test if signal for new color is emitted.
        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QCOMPARE(mySpy.count(), 1);
        QVERIFY2(myDiagram.currentColorCielchD50() == referenceColorLch, //
                 "Verify that the color is equal to the reference color.");

        // Test that no signal is emitted for old color.
        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QCOMPARE(mySpy.count(), 1);
        QVERIFY2(myDiagram.currentColorCielchD50() == referenceColorLch, //
                 "Verify that the color is equal to the reference color.");
    }

    void testDiagramOffset()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.show(); // Necessary to allow event processing
        myDiagram.resize(50, 50);
        qreal oldOffset = myDiagram.d_pointer->diagramOffset();
        myDiagram.resize(100, 100);
        QVERIFY2(myDiagram.d_pointer->diagramOffset() > oldOffset,
                 "Verify that the offset at widget size 150 is bigger "
                 "than at widget size 100.");
    }

    void testdiagramCenter()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        myDiagram.resize(100, 100);
        // Test conformance with diagramOffset()
        QCOMPARE(myDiagram.d_pointer->diagramCenter().x(), //
                 myDiagram.d_pointer->diagramOffset());
        QCOMPARE(myDiagram.d_pointer->diagramCenter().y(), //
                 myDiagram.d_pointer->diagramOffset());
    }

    void testConversions()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_rgbColorSpace);
        GenericColor myGrayColor;
        myGrayColor.third = 0;
        myGrayColor.first = 50;
        myGrayColor.second = 0;
        myDiagram.setCurrentColorCielchD50(myGrayColor);
        myDiagram.show(); // Necessary to make sure resize events are processed
        constexpr int widgetSize = 300;
        myDiagram.resize(widgetSize, widgetSize);
        QCOMPARE(myDiagram.size(), QSize(widgetSize, widgetSize));
        // Chose a position near to, but different from the center.
        constexpr int testPosition = widgetSize / 2 + 10;
        myDiagram.d_pointer->setColorFromWidgetPixelPosition( //
            QPoint(testPosition, testPosition));
        QCOMPARE( //
            myDiagram.d_pointer->m_currentColorCielchD50.first, //
            myDiagram.d_pointer->fromWidgetPixelPositionToLab(QPoint(testPosition, testPosition)).L);
        QCOMPARE( //
            PolarPointF(myDiagram.d_pointer->m_currentColorCielchD50.second, myDiagram.d_pointer->m_currentColorCielchD50.third).toCartesian().x(), //
            myDiagram.d_pointer->fromWidgetPixelPositionToLab(QPoint(testPosition, testPosition)).a);
        QCOMPARE( //
            PolarPointF(myDiagram.d_pointer->m_currentColorCielchD50.second, myDiagram.d_pointer->m_currentColorCielchD50.third).toCartesian().y(),
            myDiagram.d_pointer->fromWidgetPixelPositionToLab(QPoint(testPosition, testPosition)).b);
        QCOMPARE(myDiagram.d_pointer->widgetCoordinatesFromCurrentColorCielchD50(), //
                 QPoint(testPosition, testPosition) + QPointF(0.5, 0.5));
    }

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in immediate
        // re-paint.
        ChromaHueDiagram myWidget{m_rgbColorSpace};
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

    void testOutOfGamutColors()
    {
        ChromaHueDiagram myWidget{m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting out-of-gamut colors works

        const GenericColor myFirstColor{100, 150, 0};
        myWidget.setCurrentColorCielchD50(myFirstColor);
        QVERIFY(myFirstColor == myWidget.currentColorCielchD50());
        QVERIFY(myFirstColor == myWidget.d_pointer->m_currentColorCielchD50);

        const GenericColor mySecondColor{0, 150, 0};
        myWidget.setCurrentColorCielchD50(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorCielchD50());
        QVERIFY(mySecondColor == myWidget.d_pointer->m_currentColorCielchD50);
    }

    void testOutOfRange()
    {
        ChromaHueDiagram myWidget{m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting colors, that are not only out-of-gamut colors
        // but also out of a reasonable range, works.

        const GenericColor myFirstColor{300, 550, -10};
        myWidget.setCurrentColorCielchD50(myFirstColor);
        QVERIFY(myFirstColor == myWidget.currentColorCielchD50());
        QVERIFY(myFirstColor == myWidget.d_pointer->m_currentColorCielchD50);

        const GenericColor mySecondColor{-100, -150, 890};
        myWidget.setCurrentColorCielchD50(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorCielchD50());
        QVERIFY(mySecondColor == myWidget.d_pointer->m_currentColorCielchD50);
    }

    void testSnipped01()
    {
        snippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaHueDiagram)

// The following “include” is necessary because we do not use a header file:
#include "testchromahuediagram.moc"
