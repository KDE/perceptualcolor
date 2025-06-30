// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "swatchbook.h"
// Second, the private implementation.
#include "swatchbook_p.h" // IWYU pragma: keep

#include "constpropagatinguniquepointer.h"
#include "helper.h"
#include "rgbcolorspacefactory.h"
#include <qboxlayout.h>
#include <qbytearray.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <qglobal.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpoint.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qstring.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qstyleoption.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>
#include <qtestkeyboard.h>
#include <qwidget.h>
#include <type_traits>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class RgbColorSpace;

class TestSwatchBook : public QObject
{
    Q_OBJECT

public:
    explicit TestSwatchBook(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace = RgbColorSpaceFactory::createSrgb();

    void provideStyleNamesAsData()
    {
        QTest::addColumn<QString>("styleName");
        const auto container = QStyleFactory::keys();
        for (const QString &currentStyleName : std::as_const(container)) {
            QTest::newRow(currentStyleName.toUtf8().constData()) //
                << currentStyleName;
        }
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

    void testConstructorDestructor()
    {
        SwatchBook testObject(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace),
                              {});
    }

    void testConstructorDefaultValues()
    {
        SwatchBook testObject(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace),
                              {});
        // Verify that initially by default no color is selected:
        QCOMPARE(testObject.d_pointer->m_selectedRow, -1);
        QCOMPARE(testObject.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testObject.d_pointer->m_selectedRow, -1);
    }

    void testMinimalSizeHint()
    {
        SwatchBook testWidget(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace),
                              {});
        QVERIFY2(testWidget.minimumSizeHint().width() > 0, //
                 "minimalSizeHint width is implemented.");
        QVERIFY2(testWidget.minimumSizeHint().height() > 0, //
                 "minimalSizeHint height is implemented.");
    }

    void testSizeHint()
    {
        SwatchBook testWidget(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace),
                              {});
        QVERIFY2( //
            testWidget.sizeHint().width() //
                >= testWidget.minimumSizeHint().width(), //
            "sizeHint width is bigger than or equal to minimalSizeHint width.");
        QVERIFY2( //
            testWidget.sizeHint().height() //
                >= testWidget.minimumSizeHint().height(),
            "sizeHint height is bigger than or equal to minimalSizeHint "
            "height.");
    }

#ifndef MSVC_DLL
    // The automatic export of otherwise private symbols on MSVC
    // shared libraries via CMake's WINDOWS_EXPORT_ALL_SYMBOLS property
    // does not work well for Qt meta objects, resulting in non-functional
    // signals. Since the following unit tests require signals, it cannot be
    // built for MSVC shared libraries.

    void testCurrentColor()
    {
        SwatchBook testWidget(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace),
                              {});
        // Prepare test
        QObject scopeMarker;
        QColor lastSignalColor;
        int signalCount = 0;
        connect( //
            &testWidget,
            &SwatchBook::currentColorChanged,
            &scopeMarker,
            [&lastSignalColor, &signalCount](const QColor &newCurrentColor) {
                lastSignalColor = newCurrentColor;
                ++signalCount;
            });
        // Initialize the swatch book widget and lastSignalColor to a
        // defined state
        testWidget.d_pointer->selectSwatchByLogicalCoordinates(0, 0);

        // Test
        const QColor oldColor = lastSignalColor;
        testWidget.d_pointer->selectSwatchByLogicalCoordinates(0, 1);
        QVERIFY(oldColor != lastSignalColor); // Signal has been emitted

        testWidget.setCurrentColor(Qt::red);
        QCOMPARE(testWidget.currentColor(), Qt::red);
        QCOMPARE(lastSignalColor, Qt::red);
        const int oldSignalCount = signalCount;
        testWidget.setCurrentColor(Qt::green);
        QCOMPARE(testWidget.currentColor(), Qt::green);
        QCOMPARE(signalCount, oldSignalCount + 1);
        QCOMPARE(lastSignalColor, Qt::green);
        // Setting it again should not trigger a new signal
        testWidget.setCurrentColor(Qt::green);
        QCOMPARE(testWidget.currentColor(), Qt::green);
        QCOMPARE(signalCount, oldSignalCount + 1);
        QCOMPARE(lastSignalColor, Qt::green);

        // Test conformance with QColorDialog when assigning invalid colors
        testWidget.setCurrentColor(Qt::blue);
        QColorDialog myQColorDialog;
        myQColorDialog.setCurrentColor(Qt::blue);
        testWidget.setCurrentColor(QColor());
        myQColorDialog.setCurrentColor(QColor());
        QCOMPARE(testWidget.currentColor(), myQColorDialog.currentColor());
        QCOMPARE(lastSignalColor, myQColorDialog.currentColor());
    }

#endif

    void testPatchSpacingH_data()
    {
        provideStyleNamesAsData();
    }

    void testPatchSpacingH()
    {
        QFETCH(QString, styleName);
        QStyle *style = QStyleFactory::create(styleName);
        {
            // Own block to make sure style will be deleted _after_ testWidget
            // has been destroyed.
            SwatchBook testWidget(m_rgbColorSpace, //
                                  wcsBasicColors(m_rgbColorSpace),
                                  Qt::Orientation::Horizontal);
            testWidget.setStyle(style);
            QVERIFY(testWidget.d_pointer->horizontalPatchSpacing() > 0);
            QVERIFY(testWidget.d_pointer->verticalPatchSpacing() > 0);
            QVERIFY( //
                testWidget.d_pointer->horizontalPatchSpacing() //
                > testWidget.d_pointer->verticalPatchSpacing());
        }
        delete style;
    }

    void testPatchSpacingV_data()
    {
        provideStyleNamesAsData();
    }

    void testPatchSpacingV()
    {
        QFETCH(QString, styleName);
        QStyle *style = QStyleFactory::create(styleName);
        {
            // Own block to make sure style will be deleted _after_ testWidget
            // has been destroyed.
            SwatchBook testWidget(m_rgbColorSpace, //
                                  wcsBasicColors(m_rgbColorSpace),
                                  Qt::Orientation::Vertical);
            testWidget.setStyle(style);
            QVERIFY(testWidget.d_pointer->horizontalPatchSpacing() > 0);
            QVERIFY(testWidget.d_pointer->verticalPatchSpacing() > 0);
            QVERIFY( //
                testWidget.d_pointer->horizontalPatchSpacing() //
                < testWidget.d_pointer->verticalPatchSpacing());
        }
        delete style;
    }

    void testPatchSpacingNone_data()
    {
        provideStyleNamesAsData();
    }

    void testPatchSpacingNone()
    {
        QFETCH(QString, styleName);
        QStyle *style = QStyleFactory::create(styleName);
        {
            // Own block to make sure style will be deleted _after_ testWidget
            // has been destroyed.
            SwatchBook testWidget(m_rgbColorSpace, //
                                  wcsBasicColors(m_rgbColorSpace),
                                  {});
            testWidget.setStyle(style);
            QVERIFY(testWidget.d_pointer->horizontalPatchSpacing() > 0);
            QVERIFY(testWidget.d_pointer->verticalPatchSpacing() > 0);
            QVERIFY( //
                testWidget.d_pointer->horizontalPatchSpacing() //
                == testWidget.d_pointer->verticalPatchSpacing());
        }
        delete style;
    }

    void testPatchSpacingBoth_data()
    {
        provideStyleNamesAsData();
    }

    void testPatchSpacingBoth()
    {
        QFETCH(QString, styleName);
        QStyle *style = QStyleFactory::create(styleName);
        {
            // Own block to make sure style will be deleted _after_ testWidget
            // has been destroyed.
            SwatchBook testWidget( //
                m_rgbColorSpace, //
                wcsBasicColors(m_rgbColorSpace), //
                Qt::Orientation::Horizontal | Qt::Orientation::Vertical);
            testWidget.setStyle(style);
            QVERIFY(testWidget.d_pointer->horizontalPatchSpacing() > 0);
            QVERIFY(testWidget.d_pointer->verticalPatchSpacing() > 0);
            QVERIFY( //
                testWidget.d_pointer->horizontalPatchSpacing() //
                == testWidget.d_pointer->verticalPatchSpacing());
        }
        delete style;
    }

    void testPatchSize_data()
    {
        provideStyleNamesAsData();
    }

    void testPatchSize()
    {
        QFETCH(QString, styleName);
        QStyle *style = QStyleFactory::create(styleName);
        {
            // Own block to make sure style will be deleted _after_ testWidget
            // has been destroyed.
            SwatchBook testWidget(m_rgbColorSpace, //
                                  wcsBasicColors(m_rgbColorSpace), //
                                  {});
            testWidget.setStyle(style);
            QVERIFY(!testWidget.d_pointer->patchSizeInner().isEmpty());
            QVERIFY(!testWidget.d_pointer->patchSizeOuter().isEmpty());
            QVERIFY( //
                testWidget.d_pointer->patchSizeOuter().width() //
                >= testWidget.d_pointer->patchSizeInner().width());
            QVERIFY( //
                testWidget.d_pointer->patchSizeOuter().height() //
                >= testWidget.d_pointer->patchSizeInner().height());

            // Test also some design properties:
            QVERIFY( //
                testWidget.d_pointer->patchSizeInner().width() //
                >= testWidget.d_pointer->horizontalPatchSpacing());
            QVERIFY( //
                testWidget.d_pointer->patchSizeInner().height() //
                >= testWidget.d_pointer->verticalPatchSpacing());
        }
        delete style;
    }

    void testRetranslateUI()
    {
        SwatchBook testWidget(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace), //
                              {});
        // Test that function call does not crash:
        testWidget.d_pointer->retranslateUi();
    }

    void testInitStyleOptions()
    {
        SwatchBook testWidget(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace), //
                              {});

        // Test that function call does not crash with regular object:
        QStyleOptionFrame temp;
        testWidget.d_pointer->initStyleOption(&temp);

        // Test that function does not crash with nullptr:
        testWidget.d_pointer->initStyleOption(nullptr);
    }

    void testOffset_data()
    {
        provideStyleNamesAsData();
    }

    void testOffset()
    {
        QFETCH(QString, styleName);
        QStyle *style = QStyleFactory::create(styleName);
        {
            // Own block to make sure style will be deleted _after_ testWidget
            // has been destroyed.

            // Encapsulating our widget within a parent widget that uses a
            // layout. This ensures proper handling of resize events, as some
            // styles struggle with such events. While this issue might cause
            // unit test failures, it does not pose a problem in real-world
            // usage scenarios.
            QWidget *mainWidget = new QWidget;
            QVBoxLayout *mainLayout = new QVBoxLayout;
            SwatchBook *testWidget = new SwatchBook( //
                m_rgbColorSpace, //
                wcsBasicColors(m_rgbColorSpace), //
                {},
                mainWidget);
            QHBoxLayout *topLayout = new QHBoxLayout;
            topLayout->addWidget(testWidget);
            topLayout->addStretch();
            mainLayout->addLayout(topLayout);
            mainLayout->addStretch();
            mainWidget->setLayout(mainLayout);
            mainWidget->setStyle(style);
            mainWidget->adjustSize();
            mainWidget->resize(400, 300);
            mainWidget->show();

            QStyleOptionFrame temp;
            testWidget->d_pointer->initStyleOption(&temp);
            QVERIFY(testWidget->d_pointer->offset(temp).x() >= 0);
            QVERIFY(testWidget->d_pointer->offset(temp).y() >= 0);

            delete mainWidget;
        }
        delete style;
    }

    void testSetCurrentColor()
    {
        QColorArray2D array = QColorArray2D(4, 1);
        array.setValue(0, 0, Qt::red);
        array.setValue(1, 0, Qt::green);
        array.setValue(2, 0, Qt::blue);
        array.setValue(3, 0, QColor()); // invalid color
        SwatchBook testWidget(m_rgbColorSpace, //
                              array,
                              {});
        testWidget.setLayoutDirection(Qt::LayoutDirection::LeftToRight);

        testWidget.setCurrentColor(Qt::red);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(Qt::green);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(Qt::blue);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 2);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(QColor());
        // Setting an invalid current color means: No color selected.
        // There might be individual color patches carrying the value of
        // an invalid color, but here it means that the color patch is empty.
        // So setting an invalid current color should never select an empty
        // swatch (if any).
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1); // And not 3.
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1); // And not 0.
    }

    void testKeyboardStartLTR()
    {
        // If no color patch is currently selected, but a key is pressed
        // to move the selection (e.g., left arrow, page up, etc.),
        // the first selected color patch should be (0, 0) in
        // left-to-right (LTR) layouts.

        const auto myBasicColors = wcsBasicColors(m_rgbColorSpace);
        SwatchBook testWidget(m_rgbColorSpace, //
                              myBasicColors,
                              {});
        testWidget.setLayoutDirection(Qt::LayoutDirection::LeftToRight);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Right);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Up);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Down);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_PageUp);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_PageDown);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Home);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_End);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        QColorArray2D array = QColorArray2D(3, 3);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, Qt::red);
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, Qt::red);
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 2);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, Qt::red);
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 1);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, Qt::red);
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 2);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 1);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
    }

    void testKeyboardStartRTL()
    {
        // If no color patch is currently selected, but a key is pressed
        // to move the selection (e.g., left arrow, page up, etc.),
        // the first selected color patch should be the top-right patch in
        // right-to-left (RTL) layouts.

        const auto myBasicColors = wcsBasicColors(m_rgbColorSpace);
        SwatchBook testWidget(m_rgbColorSpace, //
                              myBasicColors,
                              {});
        testWidget.setLayoutDirection(Qt::LayoutDirection::RightToLeft);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Right);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Up);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Down);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_PageUp);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_PageDown);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_Home);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QTest::keyClick(&testWidget, Qt::Key_End);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        QColorArray2D array = QColorArray2D(3, 3);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, Qt::red);
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, Qt::red);
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 2);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, Qt::red);
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 1);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, Qt::red);
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 2);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 1);

        // Set some invalid values
        array.setValue(0, 0, QColor());
        array.setValue(1, 0, QColor());
        array.setValue(2, 0, QColor());
        array.setValue(0, 1, QColor());
        array.setValue(1, 1, QColor());
        array.setValue(2, 1, QColor());
        array.setValue(0, 2, QColor());
        array.setValue(1, 2, QColor());
        array.setValue(2, 2, QColor());
        testWidget.setSwatchGrid(array);
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            QColor(1, 2, 3));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
        QTest::keyClick(&testWidget, Qt::Key_Left);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, -1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, -1);
    }

    void testKeyboardMove()
    {
        const auto myBasicColors = wcsBasicColors(m_rgbColorSpace);
        SwatchBook testWidget(m_rgbColorSpace, //
                              myBasicColors,
                              {});
        const auto count = //
            qMax(testWidget.d_pointer->m_swatchGrid.iCount(), //
                 testWidget.d_pointer->m_swatchGrid.jCount())
            // Add 1 to exceed the possible number of fields (crash test)
            + 1;

        // Starting point is (0, 0) on LTR layout
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            wcsBasicColors(m_rgbColorSpace).value(0, 0));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        // Test keys LTR
        testWidget.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Right);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Left);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_End);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_Home);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Left);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);

        // Key tests RTL
        testWidget.setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Left);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Right);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_End);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_Home);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);

        // Key tests vertical
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Down);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 testWidget.d_pointer->m_swatchGrid.jCount() - 1);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Up);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_PageDown);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 testWidget.d_pointer->m_swatchGrid.jCount() - 1);
        QTest::keyClick(&testWidget, Qt::Key_PageUp);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
    }

    void testKeyboardMoveWithHoles()
    {
        // There might be holes in the swatch book (patches that are empty).
        // The keyboard should react correctly nevertheless.
        const auto myBasicColors = wcsBasicColors(m_rgbColorSpace);
        QColorArray2D array = myBasicColors;
        // Set some invalid values
        array.setValue(1, 1, QColor());
        array.setValue(2, 2, QColor());
        array.setValue(3, 3, QColor());
        array.setValue(4, 4, QColor());
        array.setValue(9, 5, QColor());
        SwatchBook testWidget(m_rgbColorSpace, //
                              array,
                              {});
        const auto count = //
            qMax(testWidget.d_pointer->m_swatchGrid.iCount(), //
                 testWidget.d_pointer->m_swatchGrid.jCount())
            // Add 1 to exceed the possible number of fields (crash test)
            + 1;

        // Starting point is (0, 0) on LTR layout
        testWidget.setCurrentColor(
            // A color that is not in the swatch book:
            wcsBasicColors(m_rgbColorSpace).value(0, 0));
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, 0);

        // Test keys LTR
        testWidget.setLayoutDirection(Qt::LayoutDirection::LeftToRight);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Right);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Left);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_End);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_Home);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Left);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);

        // Key tests RTL
        testWidget.setLayoutDirection(Qt::LayoutDirection::RightToLeft);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Left);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Right);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_End);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 testWidget.d_pointer->m_swatchGrid.iCount() - 1);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_Home);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);

        // Key tests vertical
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Down);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 testWidget.d_pointer->m_swatchGrid.jCount() - 1);
        for (int i = 0; i < count; ++i) {
            QTest::keyClick(&testWidget, Qt::Key_Up);
        }
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
        QTest::keyClick(&testWidget, Qt::Key_PageDown);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 testWidget.d_pointer->m_swatchGrid.jCount() - 1);
        QTest::keyClick(&testWidget, Qt::Key_PageUp);
        QCOMPARE(testWidget.d_pointer->m_selectedColumn, //
                 0);
        QCOMPARE(testWidget.d_pointer->m_selectedRow, //
                 0);
    }

    void testCornerRadius_data()
    {
        provideStyleNamesAsData();
    }

    void testCornerRadius()
    {
        QFETCH(QString, styleName);
        QStyle *style = QStyleFactory::create(styleName);
        {
            // Own block to make sure style will be deleted _after_ testWidget
            // has been destroyed.
            SwatchBook testWidget(m_rgbColorSpace, //
                                  wcsBasicColors(m_rgbColorSpace), //
                                  {});
            testWidget.setStyle(style);
            QStyleOptionFrame temp;
            testWidget.d_pointer->initStyleOption(&temp);
            QVERIFY(testWidget.d_pointer->cornerRadius() >= 0);
        }
        delete style;
    }

    void testSwatchGrid()
    {
        SwatchBook testWidget(m_rgbColorSpace, //
                              wcsBasicColors(m_rgbColorSpace), //
                              {});
        QCOMPARE(testWidget.swatchGrid(), wcsBasicColors(m_rgbColorSpace));
        testWidget.setSwatchGrid(QColorArray2D());
        QCOMPARE(testWidget.swatchGrid(), QColorArray2D());
    }

    void testInitSwatchGridTransparency()
    {
        // The widget does not support transparency. When assigning a swatch
        // grid, all colors should bet treated as opaque, even if the
        // assigned values might contain transparency.
        QColorArray2D array = QColorArray2D(1, 1);
        const QColor myColor = QColor(50, 100, 150, 200);
        array.setValue(0, 0, myColor);
        SwatchBook testWidget(m_rgbColorSpace, //
                              array,
                              {});
        QCOMPARE(testWidget.swatchGrid().value(0, 0).alphaF(), 1);
    }

    void testSetSwatchGridTransparency()
    {
        // The widget does not support transparency. When assigning a swatch
        // grid, all colors should bet treated as opaque, even if the
        // assigned values might contain transparency.
        SwatchBook testWidget(m_rgbColorSpace, //
                              {},
                              {});
        QColorArray2D array = QColorArray2D(1, 1);
        const QColor myColor = QColor(50, 100, 150, 200);
        array.setValue(0, 0, myColor);
        testWidget.setSwatchGrid(array);
        QCOMPARE(testWidget.swatchGrid().value(0, 0).alphaF(), 1);
    }

    void testSetSwatchGridInvalid()
    {
        // An invalid color in the grid should be preserved and
        // means "empty swatch".
        QColorArray2D array = QColorArray2D(1, 1);
        array.setValue(0, 0, QColor());
        SwatchBook testWidget(m_rgbColorSpace, //
                              array,
                              {});
        QVERIFY(!testWidget.swatchGrid().value(0, 0).isValid());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestSwatchBook)

// The following “include” is necessary because we do not use a header file:
#include "testswatchbook.moc"
