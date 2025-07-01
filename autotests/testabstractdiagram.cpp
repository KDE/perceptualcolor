﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "abstractdiagram.h"

#include "helpermath.h"
#include <qbrush.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qsize.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

class QWidget;

class TestAbstractDiagramHelperClass : public PerceptualColor::AbstractDiagram
{
    Q_OBJECT
public:
    explicit TestAbstractDiagramHelperClass(QWidget *parent = nullptr)
        : AbstractDiagram(parent)
    {
        // This constructor exists only to satisfy Clazy code checker, which
        // expects constructors taking QWidget* as argument for all classes
        // that inherit from QWidget.
    }
    void testSnippet01()
    {
        //! [useTransparencyBackground]
        // Within a class derived from AbstractDiagram, you can use this code:

        QImage myImage(150, 200, QImage::Format_ARGB32_Premultiplied);

        QPainter myPainter(&myImage);

        // Fill the hole image with tiles made of transparencyBackground()
        myPainter.fillRect(0,
                           0,
                           150,
                           200,
                           // During painting, QBrush will ignore the
                           // device pixel ratio of the underlying
                           // transparencyBackground image!
                           QBrush(transparencyBackground()));

        // Paint semi-transparent red color above
        myPainter.fillRect(0, 0, 150, 200, QBrush(QColor(255, 0, 0, 128)));
        //! [useTransparencyBackground]
    }
};

namespace PerceptualColor
{
class TestAbstractDiagram : public QObject
{
    Q_OBJECT

public:
    explicit TestAbstractDiagram(QObject *parent = nullptr)
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

    void testConstructorDestructor()
    {
        PerceptualColor::AbstractDiagram myDiagram;
    }

    void testShow()
    {
        // Just test if instantiating does not crash:
        PerceptualColor::AbstractDiagram myDiagram;
        myDiagram.show();
        QVERIFY2(myDiagram.isVisible(), "Test is diagram was shown correctly.");
    }

    void testSnippet01()
    {
        TestAbstractDiagramHelperClass helper;
        helper.testSnippet01();
    }

    void testTransparencyBackground()
    {
        PerceptualColor::AbstractDiagram myDiagram;
        QImage temp = myDiagram.transparencyBackground();
        QVERIFY2(temp.size().width() > 0, "Width of image is bigger than 0.");
        QVERIFY2(temp.size().height() > 0, "Height of image is bigger than 0.");
        QVERIFY2(temp.allGray(), "Image is neutral gray.");
    }

    void testFocusIndicatorColor()
    {
        QVERIFY2(AbstractDiagram().focusIndicatorColor().isValid(), "focusIndicatorColor() returns a valid color.");
    }

    void testPhysicalPixelSize()
    {
        AbstractDiagram temp;
        temp.show();
        qreal widthError = (temp.width() * temp.devicePixelRatioF()) - temp.physicalPixelSize().width();
        QVERIFY2(qAbs(widthError) < 1, "Rounding width with error < 1.");
        qreal heightError = (temp.height() * temp.devicePixelRatioF()) - temp.physicalPixelSize().height();
        QVERIFY2(qAbs(heightError) < 1, "Rounding height with error < 1.");
    }

    void testDiagramOffset()
    {
        AbstractDiagram myDiagram;
        myDiagram.resize(50, 50);
        QVERIFY2(isInRange(49.0, myDiagram.maximumWidgetSquareSize(), 50.0),
                 "Verify that maximumWidgetSquareSize is within expected "
                 "rounding range.");
        // Next try: off by one.
        myDiagram.resize(51, 51);
        QVERIFY2(isInRange(50.0, myDiagram.maximumWidgetSquareSize(), 51.0),
                 "Verify that maximumWidgetSquareSize is within expected "
                 "rounding range.");
    }

    void testHandle()
    {
        AbstractDiagram temp;
        QVERIFY2(temp.handleRadius() > 0, "Radius is positive.");
        QVERIFY2(temp.handleOutlineThickness() > 0, "Thickness is positive.");
        QVERIFY2(temp.handleRadius() > temp.handleOutlineThickness(),
                 "Radius is bigger than thickness. "
                 "(Otherwise, there would be no hole in the middle.)");
    }

    void testGradientThickness()
    {
        AbstractDiagram temp;
        QVERIFY(temp.gradientThickness() > 0);
    }

    void testGradientMinimumLength()
    {
        AbstractDiagram temp;
        QVERIFY(temp.gradientMinimumLength() > temp.gradientThickness());
    }

    void testHandleColorFromBackgroundLightness()
    {
        AbstractDiagram temp;
        QCOMPARE(temp.handleColorFromBackgroundLightness(-1), QColor(Qt::white));
        QCOMPARE(temp.handleColorFromBackgroundLightness(0), QColor(Qt::white));
        QCOMPARE(temp.handleColorFromBackgroundLightness(49), QColor(Qt::white));
        QCOMPARE(temp.handleColorFromBackgroundLightness(51), QColor(Qt::black));
        QCOMPARE(temp.handleColorFromBackgroundLightness(100), QColor(Qt::black));
        QCOMPARE(temp.handleColorFromBackgroundLightness(101), QColor(Qt::black));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestAbstractDiagram)
// The following “include” is necessary because we do not use a header file:
#include "testabstractdiagram.moc"
