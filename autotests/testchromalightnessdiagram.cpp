// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "chromalightnessdiagram.h"
// Second, the private implementation.
#include "chromalightnessdiagram_p.h" // IWYU pragma: keep

#include "colorenginefactory.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "helper.h"
#include "helpermath.h"
#include <cmath>
#include <limits>
#include <qglobal.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsharedpointer.h>
#include <qsignalspy.h>
#include <qsize.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestkeyboard.h>
#include <qtestmouse.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class ColorEngine;

class TestChromaLightnessDiagram : public QObject
{
    Q_OBJECT

public:
    explicit TestChromaLightnessDiagram(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::ColorEngine> m_colorEngine = createSrgbColorEngine();

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

    void testConstructorDestructor()
    {
        ChromaLightnessDiagram test(m_colorEngine);
    }

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in immediate
        // re-paint.
        ChromaLightnessDiagram myWidget{m_colorEngine};
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

    void testSetCurrentColorCielchD50FromWidgetPixelPosition1()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because if the widget is too small, there
        // is no place for a diagram, and some value conversions are
        // diagram-based..
        ChromaLightnessDiagram myWidget{m_colorEngine};
        const QPoint positive(10, 20);
        const QPoint negative(-10, -20);
        myWidget.resize(QSize(1, 1));
        // Executing the following lines should not crash!
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(positive);
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(negative);
    }

    void testSetCurrentColorCielchD50FromWidgetPixelPosition2()
    {
        // Test this function for out-of-gamut positions
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        constexpr int size = 100;
        myWidget.resize(size, size);
        GenericColor color;

        // Test for top-left corner
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(
            // Same x and y spacing from top-left corner
            QPoint(size * (-1), size * (-1)));
        delayedEventProcessing();
        color = myWidget.currentColorCielchD50();
        QCOMPARE(color.first, 100);
        QCOMPARE(color.second, 0);

        // Test for bottom-left corner
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(
            // Same x and y spacing from bottom-left corner
            QPoint(size * (-1), size * 2));
        delayedEventProcessing();
        color = myWidget.currentColorCielchD50();
        QCOMPARE(color.first, 0);
        QCOMPARE(color.second, 0);

        // Test for middle-right position
        myWidget.d_pointer->setCurrentColorFromWidgetPixelPosition(
            // x position far from diagram boundaries, y position in the middle
            QPoint(size * 10, size * 50 / 100));
        delayedEventProcessing();
        color = myWidget.currentColorCielchD50();
        // Lightness should be somewhere in the middle.
        QVERIFY(color.first > 10);
        QVERIFY(color.first < 90);
        // At least 25 should be possible on all hues.
        QVERIFY(color.second > 25);
    }

    void testDefaultBorderPhysical()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        QVERIFY(myWidget.d_pointer->defaultBorderPhysical() >= 0);
    }

    void testLeftBorderPhysical()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        QVERIFY(myWidget.d_pointer->defaultBorderPhysical() >= 0);
        QVERIFY(myWidget.d_pointer->defaultBorderPhysical() >= myWidget.d_pointer->defaultBorderPhysical());
    }

    void testCalculateImageSizePhysical()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes.
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.resize(QSize());
        Q_UNUSED(myWidget.d_pointer->calculateImageSizePhysical()); // Should not crash
        myWidget.resize(QSize(-1, -1));
        Q_UNUSED(myWidget.d_pointer->calculateImageSizePhysical()); // Should not crash
        myWidget.resize(QSize(-1, 0));
        Q_UNUSED(myWidget.d_pointer->calculateImageSizePhysical()); // Should not crash
        myWidget.resize(QSize(0, -1));
        Q_UNUSED(myWidget.d_pointer->calculateImageSizePhysical()); // Should not crash
        myWidget.resize(QSize(0, 1));
        Q_UNUSED(myWidget.d_pointer->calculateImageSizePhysical()); // Should not crash
        myWidget.resize(QSize(1, 0));
        Q_UNUSED(myWidget.d_pointer->calculateImageSizePhysical()); // Should not crash
        myWidget.resize(QSize(1, 1));
        Q_UNUSED(myWidget.d_pointer->calculateImageSizePhysical()); // Should not crash
    }

    void testFromWidgetPixelPositionToColor()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes.
        ChromaLightnessDiagram myWidget{m_colorEngine};
        const QPoint positive(10, 20);
        const QPoint negative(-10, -20);
        myWidget.resize(QSize());
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(-1, -1));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(-1, 0));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(0, -1));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(0, 1));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(1, 0));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(1, 1));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(2, 2));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(3, 3));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(4, 4));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(5, 5));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(6, 6));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(7, 7));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(8, 8));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(9, 9));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(10, 10));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(11, 11));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(12, 12));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(13, 13));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
        myWidget.resize(QSize(14, 14));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(positive));
        Q_UNUSED(myWidget.d_pointer->fromWidgetPixelPositionToCielchD50(negative));
    }

    void testMouseSupport1()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        myWidget.resize(2, 2);
        // Mouse movements should not crash when the size of the widget is
        // too small to show a diagram:
        QTest::mousePress(&myWidget, //
                          Qt::MouseButton::LeftButton, //
                          Qt::KeyboardModifier::NoModifier, //
                          QPoint(0, 0));
        // Alternative: Maybe this catches more bugs?…:
        // QTest::mouseMove(&myWidget, QPoint(1, 1));
        QTest::mouseRelease(&myWidget, //
                            Qt::MouseButton::LeftButton, //
                            Qt::KeyboardModifier::NoModifier, //
                            QPoint(1, 1));
    }

    void testMouseSupport2()
    {
        // Test reactions to mouse events when moving out-of-gamut
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        constexpr int size = 100;
        myWidget.resize(size, size);
        GenericColor color;

        // Test for top-left corner
        QTest::mousePress(&myWidget,
                          Qt::MouseButton::LeftButton,
                          Qt::KeyboardModifier::NoModifier,
                          // Press the mouse at a point with some chroma
                          // (10%) and a medium lightness (50%). This makes
                          // sure to get a point within the gamut.
                          QPoint(size * 10 / 100, size * 50 / 100));
        QTest::mouseRelease(&myWidget,
                            Qt::MouseButton::LeftButton,
                            Qt::KeyboardModifier::NoModifier,
                            // Press the mouse at a point with some chroma
                            // (10%) and a medium lightness (50%). This makes
                            // sure to get a point within the gamut.
                            QPoint(size * (-1), size * (-1)));
        // Test if the widget value is really the nearest in-gamut color
        delayedEventProcessing();
        color = myWidget.currentColorCielchD50();
        QCOMPARE(color.first, 100);
        QCOMPARE(color.second, 0);

        // Test for bottom-left corner
        QTest::mousePress(&myWidget,
                          Qt::MouseButton::LeftButton,
                          Qt::KeyboardModifier::NoModifier,
                          // Press the mouse at a point with some chroma
                          // (10%) and a medium lightness (50%). This makes
                          // sure to get a point within the gamut.
                          QPoint(size * 10 / 100, size * 50 / 100));
        QTest::mouseRelease(&myWidget,
                            Qt::MouseButton::LeftButton,
                            Qt::KeyboardModifier::NoModifier,
                            // Press the mouse at a point with some chroma
                            // (10%) and a medium lightness (50%). This makes
                            // sure to get a point within the gamut.
                            QPoint(size * (-1), size * 2));
        // Test if the widget value is really the nearest in-gamut color
        color = myWidget.currentColorCielchD50();
        QCOMPARE(color.first, 0);
        QCOMPARE(color.second, 0);

        // Test for middle-right position
        QTest::mousePress(&myWidget,
                          Qt::MouseButton::LeftButton,
                          Qt::KeyboardModifier::NoModifier,
                          // Press the mouse at a point with some chroma
                          // (10%) and a medium lightness (50%). This makes
                          // sure to get a point within the gamut.
                          QPoint(size * 10 / 100, size * 50 / 100));
        QTest::mouseRelease(&myWidget,
                            Qt::MouseButton::LeftButton,
                            Qt::KeyboardModifier::NoModifier,
                            // Press the mouse at a point with some chroma
                            // (10%) and a medium lightness (50%). This makes
                            // sure to get a point within the gamut.
                            QPoint(size * 10, size * 50 / 100));
        // Test if the widget value is really the nearest in-gamut color
        color = myWidget.currentColorCielchD50();
        // Lightness should be somewhere in the middle.
        QVERIFY(color.first > 10);
        QVERIFY(color.first < 90);
        // At least 25 should be possible on all hues.
        QVERIFY(color.second > 25);
    }

    void testPaintEventNormalSize()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        // Test normal size
        myWidget.resize(100, 100);
        myWidget.repaint();
    }

    void testPaintEventTooSmallSize()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        // Test small size (too small to show a diagram)
        myWidget.resize(2, 2);
        myWidget.repaint();
    }

    void testPaintEventEmptySize()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        // Test empty size
        myWidget.resize(0, 0);
        myWidget.repaint();
    }

    void testKeyPressEvent()
    {
        ChromaLightnessDiagram myDiagram(m_colorEngine);
        GenericColor referenceColorLch;
        referenceColorLch.first = 50;
        referenceColorLch.second = 20;
        referenceColorLch.third = 180;
        myDiagram.setCurrentColorCielchD50(referenceColorLch);

        // Assert pre-conditions:

        QCOMPARE(myDiagram.currentColorCielchD50().first, 50);
        QCOMPARE(myDiagram.currentColorCielchD50().second, 20);
        QCOMPARE(myDiagram.currentColorCielchD50().third, 180);

        // Actual test:

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Left);
        QVERIFY(myDiagram.currentColorCielchD50().first == referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second < referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Right);
        QVERIFY(myDiagram.currentColorCielchD50().first == referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second > referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Up);
        QVERIFY(myDiagram.currentColorCielchD50().first > referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Down);
        QVERIFY(myDiagram.currentColorCielchD50().first < referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Home);
        QVERIFY(myDiagram.currentColorCielchD50().first == referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second < referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_End);
        QVERIFY(myDiagram.currentColorCielchD50().first == referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second > referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageUp);
        QVERIFY(myDiagram.currentColorCielchD50().first > referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageDown);
        QVERIFY(myDiagram.currentColorCielchD50().first < referenceColorLch.first);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        referenceColorLch.second = 0;

        // Chroma should never become negative

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Left);
        QCOMPARE(myDiagram.currentColorCielchD50().first, referenceColorLch.first);
        QCOMPARE(myDiagram.currentColorCielchD50().second, referenceColorLch.second);
        QCOMPARE(myDiagram.currentColorCielchD50().third, referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Home);
        QCOMPARE(myDiagram.currentColorCielchD50().first, referenceColorLch.first);
        QCOMPARE(myDiagram.currentColorCielchD50().second, referenceColorLch.second);
        QCOMPARE(myDiagram.currentColorCielchD50().third, referenceColorLch.third);

        referenceColorLch.first = 0;

        // Lightness should never be smaller than 0.

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Down);
        QVERIFY(myDiagram.currentColorCielchD50().first >= 0);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageDown);
        QVERIFY(myDiagram.currentColorCielchD50().first >= 0);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        referenceColorLch.first = 100;

        // Lightness should never be bigger than 100.

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_Up);
        QVERIFY(myDiagram.currentColorCielchD50().first <= 100);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);

        myDiagram.setCurrentColorCielchD50(referenceColorLch);
        QTest::keyClick(&myDiagram, Qt::Key_PageUp);
        QVERIFY(myDiagram.currentColorCielchD50().first <= 100);
        QVERIFY(myDiagram.currentColorCielchD50().second == referenceColorLch.second);
        QVERIFY(myDiagram.currentColorCielchD50().third == referenceColorLch.third);
    }

    void testIsWidgetPixelPositionInGamut()
    {
        ChromaLightnessDiagram myDiagram(m_colorEngine);
        myDiagram.show();
        myDiagram.resize(QSize(2, 2));
        // On very small widget sizes, no diagram is visible. Therefore,
        // no pixel should be in-gamut.
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(0, 0)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(0, 1)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(0, 2)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(1, 0)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(1, 1)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(1, 2)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(2, 0)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(2, 1)));
        QVERIFY(!myDiagram.d_pointer->isWidgetPixelPositionInGamut(QPoint(2, 2)));
    }

#ifndef MSVC_DLL
    // The automatic export of otherwise private symbols on MSVC
    // shared libraries via CMake's WINDOWS_EXPORT_ALL_SYMBOLS property
    // does not work well for Qt meta objects, resulting in non-functional
    // signals. Since the following unit tests require signals, it cannot be
    // built for MSVC shared libraries.

    void testCurrentColorCielchD50Property()
    {
        ChromaLightnessDiagram test{m_colorEngine};
        GenericColor color;
        color.first = 50;
        color.second = 20;
        color.third = 10;
        test.setCurrentColorCielchD50(color);
        QVERIFY(test.currentColorCielchD50() == color);
        QSignalSpy spy(&test, &ChromaLightnessDiagram::currentColorCielchD50Changed);
        QCOMPARE(spy.size(), 0);

        // Change hue only:
        color.third += 1;
        test.setCurrentColorCielchD50(color);
        QCOMPARE(spy.size(), 1);
        QVERIFY(test.currentColorCielchD50() == color);

        // Change chroma only:
        color.second += 1;
        test.setCurrentColorCielchD50(color);
        QCOMPARE(spy.size(), 2);
        QVERIFY(test.currentColorCielchD50() == color);

        // Change lightness only:
        color.first += 1;
        test.setCurrentColorCielchD50(color);
        QCOMPARE(spy.size(), 3);
        QVERIFY(test.currentColorCielchD50() == color);

        // Not changing the color should not trigger the signal
        test.setCurrentColorCielchD50(color);
        QCOMPARE(spy.size(), 3);
        QVERIFY(test.currentColorCielchD50() == color);
    }

#endif

    void testResizeEvent()
    {
        ChromaLightnessDiagram test{m_colorEngine};
        test.show();
        // Resize events should not crash!
        test.resize(QSize(100, 100)); // normal size
        test.resize(QSize(2, 2)); // very small size
        test.resize(QSize(0, 0)); // empty size
        test.resize(QSize(-1, -1)); // invalid size
    }

    void testSizeHintAndMinimumSizeHint()
    {
        ChromaLightnessDiagram test{m_colorEngine};
        test.show();
        QVERIFY(test.minimumSizeHint().width() >= 0);
        QVERIFY(test.minimumSizeHint().height() >= 0);
        QVERIFY(test.sizeHint().width() >= test.minimumSizeHint().width());
        QVERIFY(test.sizeHint().height() >= test.minimumSizeHint().height());
    }

    void testOutOfGamutColors()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting out-of-gamut colors works

        const GenericColor myFirstColor{100, 150, 0};
        myWidget.setCurrentColorCielchD50(myFirstColor);
        QVERIFY(myFirstColor == myWidget.currentColorCielchD50());
        QVERIFY( //
            myFirstColor == myWidget.d_pointer->m_currentColorCielchD50);

        const GenericColor mySecondColor{0, 150, 0};
        myWidget.setCurrentColorCielchD50(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorCielchD50());
        QVERIFY( //
            mySecondColor == myWidget.d_pointer->m_currentColorCielchD50);
    }

    void testOutOfRange()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting colors, that are not only out-of-gamut colors
        // but also out of a reasonable range, works.

        const GenericColor myFirstColor{300, 550, -10};
        myWidget.setCurrentColorCielchD50(myFirstColor);
        QVERIFY( //
            myFirstColor == myWidget.currentColorCielchD50());
        QVERIFY( //
            myFirstColor == myWidget.d_pointer->m_currentColorCielchD50);

        const GenericColor mySecondColor{-100, -150, 890};
        myWidget.setCurrentColorCielchD50(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorCielchD50());
        QVERIFY(mySecondColor == myWidget.d_pointer->m_currentColorCielchD50);
    }

    void testNearestInGamutColorByAdjustingChromaLightness()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};

        // Variables
        GenericColor color;
        GenericColor nearestInGamutColor;

        // In-gamut colors should not be changed.
        color.first = 50;
        color.second = 20;
        color.third = 10;
        myWidget.setCurrentColorCielchD50(color);
        nearestInGamutColor = //
            myWidget.d_pointer->nearestInGamutCielchD50ByAdjustingChromaLightness(color.second, color.first);
        QVERIFY(nearestInGamutColor == color);

        // A negative chroma value should not be normalized (this would
        // mean to change the hue), but just put to 0.
        color.first = 50;
        color.second = -20;
        color.third = 10;
        myWidget.setCurrentColorCielchD50(color);
        nearestInGamutColor = //
            myWidget.d_pointer->nearestInGamutCielchD50ByAdjustingChromaLightness(color.second, color.first);
        QCOMPARE(nearestInGamutColor.first, 50);
        QCOMPARE(nearestInGamutColor.second, 0);
        QCOMPARE(nearestInGamutColor.third, 10);
    }

    void testNearestInGamutColorByAdjustingChromaLightnessSmallSize()
    {
        ChromaLightnessDiagram myWidget{m_colorEngine};

        // Variables
        GenericColor color;
        GenericColor nearestInGamutColor;

        // In-gamut colors should not be changed.
        color.first = 50;
        color.second = 20;
        color.third = 10;
        myWidget.setCurrentColorCielchD50(color);

        // nearestInGamutCielchD50ByAdjustingChromaLightness() is only
        // guaranteed to work correctly for an image size of at least
        // two pixel width and two pixel height. Test here if at least
        // we can call the function without crash, even if the result
        // does not make sense.
        myWidget.resize(1, 1);
        nearestInGamutColor = //
            myWidget.d_pointer->nearestInGamutCielchD50ByAdjustingChromaLightness(color.second, color.first);
    }

    void testDistanceFromRange()
    {
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(1, 2, 3), 0);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(-5, -4, -3), 0);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(-1, 0, 1), 0);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 6, 7), 0);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 5, 7), 0);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 7, 7), 0);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 4, 7), 1);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 3, 7), 2);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 8, 7), 1);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 9, 7), 2);

        // Special case: low == hight
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 5, 5), 0);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 4, 5), 1);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 3, 5), 2);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 6, 5), 1);
        QCOMPARE(ChromaLightnessDiagramPrivate::distanceFromRange(5, 7, 5), 2);

        // Special cases for floating point operations
        if constexpr (std::numeric_limits<double>::has_infinity //
                      && std::numeric_limits<double>::has_signaling_NaN //
                      && std::numeric_limits<double>::has_quiet_NaN //
        ) {
            constexpr auto inf = std::numeric_limits<double>::infinity();
            constexpr auto qnan = std::numeric_limits<double>::quiet_NaN();
            constexpr auto snan = std::numeric_limits<double>::signaling_NaN();

            // Infinity
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(-inf, 7., 5.),
                2);
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(-inf, 5., 5.),
                0);
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(-inf, 3., 5.),
                0);
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(3., -inf, 5.),
                inf);
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(3., inf, 5.),
                inf);
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(3., 5., inf),
                0);
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(3., 3., inf),
                0);
            QCOMPARE( //
                ChromaLightnessDiagramPrivate::distanceFromRange(3., 1., inf),
                2);

            // Nan
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(qnan, 2., 3.)));
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(1., qnan, 3.)));
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(1., 2., qnan)));
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(qnan, qnan, 3.)));
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(qnan, 2., qnan)));
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(1., qnan, qnan)));
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(qnan, qnan, qnan)));
            QVERIFY(std::isnan( //
                ChromaLightnessDiagramPrivate::distanceFromRange(snan, snan, snan)));
        }
    }

    void testDistanceFromRangeIsConstexpr()
    {
        constexpr auto value = //
            ChromaLightnessDiagramPrivate::distanceFromRange(1, 2, 3);
        static_assert(value == 0);
    }

    void testNearestNeighborSearch()
    {
        // Setup
        const auto doesExist = [](const QPoint point) -> bool {
            // Our valid search rectangle is from (2, 2) to (8, 8).
            if (isInRange(-2, point.x(), 8) && isInRange(-2, point.y(), 8)) {
                QList<QPoint> existingPoints({//
                                              QPoint(-2, -2),
                                              QPoint(5, 5),
                                              QPoint(8, 8)});
                return existingPoints.contains(point);
            }
            // A correct implementation of nearestNeighborSearch should never
            // call the callback function with values outside the valid range,
            // so we should never get here:
            return true;
        };
        constexpr auto searchRectangle = QRect(QPoint(-2, -2), QSize(11, 11));
        QVERIFY(searchRectangle.contains(QPoint(-3, -3)) == false); // assert
        QVERIFY(searchRectangle.contains(QPoint(-2, -2))); // assert
        QVERIFY(searchRectangle.contains(QPoint(8, 8))); // assert
        QVERIFY(searchRectangle.contains(QPoint(9, 9)) == false); // assert

        // Actual tests
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-2, -2), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-1, -2), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-2, -1), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-3, -2), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-2, -3), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-3, -3), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(1, 1), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(4, 4), searchRectangle, doesExist), //
                 QPoint(5, 5));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(5, 5), searchRectangle, doesExist), //
                 QPoint(5, 5));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-100, 5), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-100, -100), searchRectangle, doesExist), //
                 QPoint(-2, -2));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(100, 100), searchRectangle, doesExist), //
                 QPoint(8, 8));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(7, 100), searchRectangle, doesExist), //
                 QPoint(8, 8));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(100, 7), searchRectangle, doesExist), //
                 QPoint(8, 8));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(-2, 8), searchRectangle, doesExist), //
                 QPoint(5, 5));
        QCOMPARE(ChromaLightnessDiagramPrivate::nearestNeighborSearch(QPoint(8, -2), searchRectangle, doesExist), //
                 QPoint(5, 5));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaLightnessDiagram)

// The following “include” is necessary because we do not use a header file:
#include "testchromalightnessdiagram.moc"
