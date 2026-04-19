// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "chromahuediagram.h"
// Second, the private implementation.
#include "chromahuediagram_p.h" // IWYU pragma: keep

#include "colorengine.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "polarpointf.h"
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
    auto myColorEngine = PerceptualColor::ColorEngine::createSrgb();
    auto myDiagram = new PerceptualColor::ChromaHueDiagram(myColorEngine, PerceptualColor::LchSpace::CielchD50);
    PerceptualColor::GenericColor myColor; // Lch
    myColor.first = 50; // Lightness
    myColor.second = 25; // Chroma
    myColor.third = 270; // Hue
    myDiagram->setCurrentColorLch(myColor);
    myDiagram->show();
    //! [instantiate]
    delete myDiagram;
}

namespace PerceptualColor
{
class ColorEngine;

class TestChromaHueDiagram : public QObject
{
    Q_OBJECT

public:
    explicit TestChromaHueDiagram(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::ColorEngine> m_colorEngine = ColorEngine::createSrgb();

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
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
    }

    void testShow()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
        myDiagram.show();
    }

    void testKeyPressEvent()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
        GenericColor referenceColorLch;
        referenceColorLch.first = 50;
        referenceColorLch.second = 0;
        referenceColorLch.third = 180;
        myDiagram.setCurrentColorLch(referenceColorLch);
        if (myDiagram.currentColorLch().third != 180) {
            throw 0;
        }
        if (myDiagram.currentColorLch().second != 0) {
            throw 0;
        }
        GenericColor referenceColorChromaLch;
        referenceColorChromaLch.first = 50;
        referenceColorChromaLch.second = 10;
        referenceColorChromaLch.third = 180;
        myDiagram.setCurrentColorLch(referenceColorChromaLch);

        QScopedPointer<QKeyEvent> myEvent;
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, 0, Qt::NoModifier));

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Up, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().second > 0, "Test Key_Up");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageUp, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().second > 0, "Test Key_PageUp");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().second < 10, "Test Key_Down");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().second < 10, "Test Key_PageDown");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Down, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().second >= 0, "Test Key_Down never negative");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_PageDown, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().second >= 0, "Test Key_PageDown never negative");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Left, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().third > 180, "Test Key_Left");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Home, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().third > 180, "Test Key_Home");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_Right, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().third < 180, "Test Key_Right");

        myDiagram.setCurrentColorLch(referenceColorChromaLch);
        myEvent.reset(new QKeyEvent(QEvent::KeyPress, Qt::Key_End, Qt::NoModifier));
        myDiagram.keyPressEvent(myEvent.data());
        QVERIFY2(myDiagram.currentColorLch().third < 180, "Test Key_End");
    }

    void testMinimumSizeHint()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
        QVERIFY2(myDiagram.minimumSizeHint().width() > 0, "minimumSizeHint width is implemented.");
        QVERIFY2(myDiagram.minimumSizeHint().height() > 0, "minimumSizeHint height is implemented.");
        // Check that the hint is a square:
        QCOMPARE(myDiagram.minimumSizeHint().width(), myDiagram.minimumSizeHint().height());
    }

    void testSizeHint()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
        QVERIFY2(myDiagram.sizeHint().width() > myDiagram.minimumSizeHint().width(), "sizeHint width is bigger than minimumSizeHint width.");
        QVERIFY2(myDiagram.sizeHint().height() > myDiagram.minimumSizeHint().height(), "sizeHint height is bigger than minimumSizeHint height.");
        // Check that the hint is a square:
        QCOMPARE(myDiagram.minimumSizeHint().width(), myDiagram.minimumSizeHint().height());
    }

    void testColorProperty()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
        QSignalSpy mySpy(&myDiagram, &PerceptualColor::ChromaHueDiagram::currentColorLchChanged);
        GenericColor referenceColorLch;
        referenceColorLch.first = 50;
        referenceColorLch.second = 10;
        referenceColorLch.third = 180;

        // Test if signal for new color is emitted.
        myDiagram.setCurrentColorLch(referenceColorLch);
        QCOMPARE(mySpy.size(), 1);
        QVERIFY2(myDiagram.currentColorLch() == referenceColorLch, //
                 "Verify that the color is equal to the reference color.");

        // Test that no signal is emitted for old color.
        myDiagram.setCurrentColorLch(referenceColorLch);
        QCOMPARE(mySpy.size(), 1);
        QVERIFY2(myDiagram.currentColorLch() == referenceColorLch, //
                 "Verify that the color is equal to the reference color.");
    }

    void testDiagramOffset()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
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
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
        myDiagram.resize(100, 100);
        // Test conformance with diagramOffset()
        QCOMPARE(myDiagram.d_pointer->diagramCenter().x(), //
                 myDiagram.d_pointer->diagramOffset());
        QCOMPARE(myDiagram.d_pointer->diagramCenter().y(), //
                 myDiagram.d_pointer->diagramOffset());
    }

    void testConversions()
    {
        PerceptualColor::ChromaHueDiagram myDiagram(m_colorEngine, LchSpace::CielchD50);
        GenericColor myGrayColor;
        myGrayColor.third = 0;
        myGrayColor.first = 50;
        myGrayColor.second = 0;
        myDiagram.setCurrentColorLch(myGrayColor);
        myDiagram.show(); // Necessary to make sure resize events are processed
        constexpr int widgetSize = 300;
        myDiagram.resize(widgetSize, widgetSize);
        QCOMPARE(myDiagram.size(), QSize(widgetSize, widgetSize));
        // Chose a position near to, but different from the center.
        constexpr int testPosition = widgetSize / 2 + 10;
        constexpr auto testPoint = QPoint(testPosition, testPosition);
        const auto testLab = //
            myDiagram.d_pointer->fromWidgetPixelPositionToLab(testPoint);
        myDiagram.d_pointer->setColorFromWidgetPixelPosition( //
            testPoint);
        QCOMPARE( //
            myDiagram.d_pointer->m_currentColorLch.first, //
            testLab.first);
        QCOMPARE( //
            PolarPointF(myDiagram.d_pointer->m_currentColorLch.second, myDiagram.d_pointer->m_currentColorLch.third).toCartesian().x(), //
            testLab.second);
        QCOMPARE( //
            PolarPointF(myDiagram.d_pointer->m_currentColorLch.second, myDiagram.d_pointer->m_currentColorLch.third).toCartesian().y(),
            testLab.third);
        QCOMPARE(myDiagram.d_pointer->widgetCoordinatesFromCurrentColorLch(), //
                 testPoint + QPointF(0.5, 0.5));
    }

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in immediate
        // re-paint.
        ChromaHueDiagram myWidget{m_colorEngine, LchSpace::CielchD50};
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
        ChromaHueDiagram myWidget{m_colorEngine, LchSpace::CielchD50};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting out-of-gamut colors works

        const GenericColor myFirstColor{100, 150, 0};
        myWidget.setCurrentColorLch(myFirstColor);
        QVERIFY(myFirstColor == myWidget.currentColorLch());
        QVERIFY(myFirstColor == myWidget.d_pointer->m_currentColorLch);

        const GenericColor mySecondColor{0, 150, 0};
        myWidget.setCurrentColorLch(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorLch());
        QVERIFY(mySecondColor == myWidget.d_pointer->m_currentColorLch);
    }

    void testOutOfRange()
    {
        ChromaHueDiagram myWidget{m_colorEngine, LchSpace::CielchD50};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting colors, that are not only out-of-gamut colors
        // but also out of a reasonable range, works.

        const GenericColor myFirstColor{300, 550, -10};
        myWidget.setCurrentColorLch(myFirstColor);
        QVERIFY(myFirstColor == myWidget.currentColorLch());
        QVERIFY(myFirstColor == myWidget.d_pointer->m_currentColorLch);

        const GenericColor mySecondColor{-100, -150, 890};
        myWidget.setCurrentColorLch(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorLch());
        QVERIFY(mySecondColor == myWidget.d_pointer->m_currentColorLch);
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
