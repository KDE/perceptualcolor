// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "wheelcolorpicker.h"
// Second, the private implementation.
#include "wheelcolorpicker_p.h" // IWYU pragma: keep

#include "chromalightnessdiagram.h"
#include "colorwheel.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "rgbcolorspace.h"
#include "rgbcolorspacefactory.h"
#include <qglobal.h>
#include <qobject.h>
#include <qpointer.h>
#include <qsharedpointer.h>
#include <qsignalspy.h>
#include <qsize.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestWheelColorPicker : public QObject
{
    Q_OBJECT

public:
    explicit TestWheelColorPicker(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QSharedPointer<RgbColorSpace> m_rgbColorSpace = RgbColorSpaceFactory::createSrgb();

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
        // Test for crash in constructor or destructor
        WheelColorPicker test{m_rgbColorSpace};
    }

    void testCurrentColorCielchD50Property()
    {
        WheelColorPicker test{m_rgbColorSpace};
        GenericColor color;
        color.first = 50;
        color.second = 20;
        color.third = 10;
        test.setCurrentColorCielchD50(color);
        QSignalSpy spy(&test, &WheelColorPicker::currentColorCielchD50Changed);
        QCOMPARE(spy.count(), 0);

        // Change hue only:
        color.third += 1;
        test.setCurrentColorCielchD50(color);
        QCOMPARE(spy.count(), 1);
        QCOMPARE(test.d_pointer->m_chromaLightnessDiagram->currentColorCielchD50().third, color.third);
        QCOMPARE(test.d_pointer->m_colorWheel->hue(), color.third);

        // Change chroma only:
        color.second += 1;
        test.setCurrentColorCielchD50(color);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(test.d_pointer->m_chromaLightnessDiagram->currentColorCielchD50().second, color.second);
        QCOMPARE(test.d_pointer->m_colorWheel->hue(), color.third);

        // Not changing the color should not trigger the signal
        test.setCurrentColorCielchD50(color);
        QCOMPARE(spy.count(), 2);
        QCOMPARE(test.d_pointer->m_chromaLightnessDiagram->currentColorCielchD50().second, color.second);
        QCOMPARE(test.d_pointer->m_colorWheel->hue(), color.third);
    }

    void testSizeHints()
    {
        WheelColorPicker test{m_rgbColorSpace};
        QVERIFY(test.minimumSizeHint().width() <= test.sizeHint().width());
        QVERIFY(test.minimumSizeHint().height() <= test.sizeHint().height());
    }

    void testVerySmallWidgetSizes()
    {
        // Also very small widget sizes should not crash the widget.
        // This might happen because of divisions by 0, even when the widget
        // is bigger than 0 because of borders or offsets. We test this
        // here with various small sizes, always forcing in immediate
        // re-paint.
        WheelColorPicker myWidget{m_rgbColorSpace};
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

    void testSetOutOfGamutColors()
    {
        WheelColorPicker myWidget{m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting out-of-gamut colors works

        const GenericColor myFirstColor{100, 150, 0};
        myWidget.setCurrentColorCielchD50(myFirstColor);
        QVERIFY(myFirstColor == myWidget.currentColorCielchD50());
        QVERIFY(myFirstColor ==
                // This widget has no own storage of this property, but
                // relies on its child widget:
                myWidget.d_pointer->m_chromaLightnessDiagram->currentColorCielchD50());

        const GenericColor mySecondColor{0, 150, 0};
        myWidget.setCurrentColorCielchD50(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorCielchD50());
        QVERIFY(mySecondColor ==
                // This widget has no own storage of this property, but
                // relies on its child widget:
                myWidget.d_pointer->m_chromaLightnessDiagram->currentColorCielchD50());
    }

    void testSetOutOfRangeColors()
    {
        WheelColorPicker myWidget{m_rgbColorSpace};
        myWidget.show();
        myWidget.resize(QSize(400, 400));

        // Test that setting colors, that are not only out-of-gamut colors
        // but also out of a reasonable range, works.

        const GenericColor myFirstColor{300, 550, -10};
        myWidget.setCurrentColorCielchD50(myFirstColor);
        QVERIFY(myFirstColor == myWidget.currentColorCielchD50());
        QVERIFY(myFirstColor ==
                // This widget has no own storage of this property, but
                // relies on its child widget:
                myWidget.d_pointer->m_chromaLightnessDiagram->currentColorCielchD50());

        const GenericColor mySecondColor{-100, -150, 890};
        myWidget.setCurrentColorCielchD50(mySecondColor);
        QVERIFY(mySecondColor == myWidget.currentColorCielchD50());
        QVERIFY(mySecondColor ==
                // This widget has no own storage of this property, but
                // relies on its child widget:
                myWidget.d_pointer->m_chromaLightnessDiagram->currentColorCielchD50());
    }

    void testHueChanges()
    {
        WheelColorPicker myWidget{m_rgbColorSpace};
        myWidget.resize(QSize(400, 400));

        // Choose a color with an extreme, but still clearly in-gamut chroma
        // (at least for the build-in sRGB gamut, with which we are testing):
        const GenericColor myColor{32, 115, 300};
        myWidget.setCurrentColorCielchD50(myColor);

        // Move the wheel to a hue that allows much less chroma:
        myWidget.d_pointer->m_colorWheel->setHue(222);

        // Now, the chroma-lightness coordinates are out-of-gamut for
        // the new hue. Test if they have been corrected:
        QVERIFY(m_rgbColorSpace->isCielchD50InGamut(myWidget.currentColorCielchD50()));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestWheelColorPicker)

// The following “include” is necessary because we do not use a header file:
#include "testwheelcolorpicker.moc"
