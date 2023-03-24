// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "colordialog.h"
// Second, the private implementation.
#include "colordialog_p.h" // IWYU pragma: keep

#include "chromahuediagram.h"
#include "colorpatch.h"
#include "constpropagatinguniquepointer.h"
#include "gradientslider.h"
#include "helperqttypes.h"
#include "initializetranslation.h"
#include "lchdouble.h"
#include "multicolor.h"
#include "multirgb.h"
#include "multispinbox.h"
#include "rgbcolorspace.h"
#include "rgbcolorspacefactory.h"
#include "settranslation.h"
#include "wheelcolorpicker.h"
#include <QtCore/qsharedpointer.h>
#include <qapplication.h>
#include <qbenchmark.h>
#include <qbytearray.h>
#include <qcolor.h>
#include <qcolordialog.h>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qglobal.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlocale.h>
#include <qmetaobject.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpair.h>
#include <qpointer.h>
#include <qpushbutton.h>
#include <qscopedpointer.h>
#include <qsignalspy.h>
#include <qspinbox.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qtabwidget.h>
#include <qtemporaryfile.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>
#include <qtestkeyboard.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qtmetamacros.h>
#include <utility>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qvector.h>
#endif

class TestColorDialogSnippetClass : public QWidget
{
    Q_OBJECT
public Q_SLOTS:
    void mySlot(QColor color)
    {
        Q_UNUSED(color)
    }

public:
    // A constructor that is clazy-conform
    explicit TestColorDialogSnippetClass(QWidget *parent = nullptr)
        : QWidget(parent)
    {
    }

    void testSnippet05()
    {
        //! [ColorDialog Open]
        PerceptualColor::ColorDialog *m_dialog = //
            new PerceptualColor::ColorDialog( //
                PerceptualColor::RgbColorSpaceFactory::createSrgb());
        m_dialog->open(this, SLOT(mySlot(QColor)));
        //! [ColorDialog Open]
        delete m_dialog;
    }
};

static void snippet01()
{
    // This function will not be called in the unit tests because getColor()
    // does not return without user interaction!
    //! [ColorDialog Get color with alpha channel]
    QColor myColor = PerceptualColor::ColorDialog::getColor(
        // Current color at widget startup:
        Qt::green,
        // Parent widget (or nullptr for no parent):
        nullptr,
        // Window title (or an empty string for default title):
        QStringLiteral("Window title"),
        // Options:
        PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel);
    //! [ColorDialog Get color with alpha channel]
    Q_UNUSED(myColor)
}

static void snippet02()
{
    //! [setOptionsWithLocalEnum]
    auto myDialog = new PerceptualColor::ColorDialog();
    myDialog->setOption( //
        PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel,
        false);
    //! [setOptionsWithLocalEnum]
    QCOMPARE( //
        myDialog->testOption( //
            PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel), //
        false);
    delete myDialog;
}

static void snippet03()
{
    //! [setOptionsWithQColorDialogEnum]
    auto myDialog = new PerceptualColor::ColorDialog();
    myDialog->setOption(QColorDialog::ShowAlphaChannel, false);
    //! [setOptionsWithQColorDialogEnum]
    QCOMPARE( //
        myDialog->testOption( //
            PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel), //
        false);
    delete myDialog;
}

static void snippet04()
{
    // This function will not be called in the unit tests because getColor()
    // does not return without user interaction!
    //! [ColorDialog Get color]
    // Show a modal color dialog and get the color that the user has chosen
    QColor myColor = PerceptualColor::ColorDialog::getColor();
    //! [ColorDialog Get color]
    Q_UNUSED(myColor)
}

namespace PerceptualColor
{
class TestColorDialog : public QObject
{
    Q_OBJECT

public:
    explicit TestColorDialog(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QScopedPointer<ColorDialog> m_perceptualDialog;
    QScopedPointer<ColorDialog> m_perceptualDialog2;
    QScopedPointer<QColorDialog> m_qDialog;
    QScopedPointer<QColorDialog> m_qDialog2;
    QColor m_color;
    QSharedPointer<RgbColorSpace> m_srgbBuildinColorSpace = //
        RgbColorSpaceFactory::createSrgb();

    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
    }

    void helperProvideQColors()
    {
        // suppress warning for generating invalid QColor
        qInstallMessageHandler(voidMessageHandler);

        QTest::addColumn<QColor>("color");

        QTest::newRow("RGB 1 2 3") //
            << QColor(1, 2, 3);
        QTest::newRow("RGBA 1 2 3 4") //
            << QColor(1, 2, 3, 4);
        QTest::newRow("RGB 1 2 300") //
            << QColor(1, 2, 300);
        QTest::newRow("RGB 1 2 -300") //
            << QColor(1, 2, -300);
        QTest::newRow("RGBA 1 2 300 4") //
            << QColor(1, 2, 300, 4);
        QTest::newRow("RGBA 1 2 3 400") //
            << QColor(1, 2, 3, 400);
        QTest::newRow("RGBA 1 2 3 -400") //
            << QColor(1, 2, 3, -400);

        QTest::newRow("RGB 0.1 0.2 0.3") //
            << QColor::fromRgbF(0.1f, 0.2f, 0.3f);
        QTest::newRow("RGBA 0.1 0.2 0.3 0.4") //
            << QColor::fromRgbF(0.1f, 0.2f, 0.3f, 0.4f);
        QTest::newRow("RGB 0.1 6.2 0.300") //
            << QColor::fromRgbF(0.1f, 6.2f, 0.300f);
        QTest::newRow("RGBA 0.1 6.2 0.300 0.4") //
            << QColor::fromRgbF(0.1f, 6.2f, 0.300f, 0.4f);
        QTest::newRow("RGBA 0.1 0.2 0.3 -0.4") //
            << QColor::fromRgbF(0.1f, 0.2f, 0.3f, -0.4f);
        QTest::newRow("RGBA 0.1 0.2 0.3 400") //
            << QColor::fromRgbF(0.1f, 0.2f, 0.3f, 400);

        QTest::newRow("CMYK 1 2 3 4") //
            << QColor::fromCmyk(1, 2, 3, 4);
        QTest::newRow("CMYK 1 2 3 4 5") //
            << QColor::fromCmyk(1, 2, 3, 4, 5);
        QTest::newRow("CMYK 1 2 300 4") //
            << QColor::fromCmyk(1, 2, 300, 4);
        QTest::newRow("CMYK 1 2 300 4 5") //
            << QColor::fromCmyk(1, 2, 300, 4, 5);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4") //
            << QColor::fromCmykF(0.1f, 0.2f, 0.300f, 0.4f);
        QTest::newRow("CMYK 0.1 0.2 0.300 0.4 0.6495217645") //
            << QColor::fromCmykF(0.1f, 0.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4") //
            << QColor::fromCmykF(0.1f, 6.2f, 0.300f, 0.4f);
        QTest::newRow("CMYK 0.1 -6.2 0.300 0.4") //
            << QColor::fromCmykF(0.1f, -6.2f, 0.300f, 0.4f);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4 0.6495217645") //
            << QColor::fromCmykF(0.1f, 6.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("CMYK 0.1 6.2 0.300 0.4 -0.6495217645") //
            << QColor::fromCmykF(0.1f, 6.2f, 0.300f, 0.4f, -0.6495217645f);

        QTest::newRow("HSL 2 3 4") //
            << QColor::fromHsl(2, 3, 4);
        QTest::newRow("HSL 2 3 4 5") //
            << QColor::fromHsl(2, 3, 4, 5);
        QTest::newRow("HSL 2 300 4") //
            << QColor::fromHsl(2, 300, 4);
        QTest::newRow("HSL 2 300 4 5") //
            << QColor::fromHsl(2, 300, 4, 5);
        QTest::newRow("HSL 0.2 0.300 0.4") //
            << QColor::fromHslF(0.2f, 0.300f, 0.4f);
        QTest::newRow("HSL 0.2 0.300 0.4 0.6495217645") //
            << QColor::fromHslF(0.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("HSL 6.2 0.300 0.4") //
            << QColor::fromHslF(6.2f, 0.300f, 0.4f);
        QTest::newRow("HSL -6.2 0.300 0.4") //
            << QColor::fromHslF(-6.2f, 0.300f, 0.4f);
        QTest::newRow("HSL 6.2 0.300 0.4 0.6495217645") //
            << QColor::fromHslF(6.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("HSL 6.2 0.300 0.4 -0.6495217645") //
            << QColor::fromHslF(6.2f, 0.300f, 0.4f, -0.6495217645f);
        QTest::newRow("HSL 6.2 0.300 0.4 1.6495217645") //
            << QColor::fromHslF(6.2f, 0.300f, 0.4f, 1.6495217645f);

        QTest::newRow("HSV 2 3 4") //
            << QColor::fromHsv(2, 3, 4);
        QTest::newRow("HSV 2 3 4 5") //
            << QColor::fromHsv(2, 3, 4, 5);
        QTest::newRow("HSV 2 300 4") //
            << QColor::fromHsv(2, 300, 4);
        QTest::newRow("HSV 2 300 4 5") //
            << QColor::fromHsv(2, 300, 4, 5);
        QTest::newRow("HSV 0.2 0.300 0.4") //
            << QColor::fromHsvF(0.2f, 0.300f, 0.4f);
        QTest::newRow("HSV 0.2 0.300 0.4 0.6495217645") //
            << QColor::fromHsvF(0.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("HSV 6.2 0.300 0.4") //
            << QColor::fromHsvF(6.2f, 0.300f, 0.4f);
        QTest::newRow("HSV -6.2 0.300 0.4") //
            << QColor::fromHsvF(-6.2f, 0.300f, 0.4f);
        QTest::newRow("HSV 6.2 0.300 0.4 0.6495217645") //
            << QColor::fromHsvF(6.2f, 0.300f, 0.4f, 0.6495217645f);
        QTest::newRow("HSV 6.2 0.300 0.4 -0.6495217645") //
            << QColor::fromHsvF(6.2f, 0.300f, 0.4f, -0.6495217645f);
        QTest::newRow("HSV 6.2 0.300 0.4 1.6495217645") //
            << QColor::fromHsvF(6.2f, 0.300f, 0.4f, 1.6495217645f);

        QTest::newRow("invalid") << QColor();

        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void helperCompareDialog(ColorDialog *perceptualDialog, QColorDialog *qColorDialog)
    {
        // Compare the state of perceptualDialog (actual)
        // to qColorDialog (expected)
        const QColor perceptualDialogSelected = //
            perceptualDialog->selectedColor();
        const QColor qColorDialogSelected = qColorDialog->selectedColor();
        QCOMPARE( //
            perceptualDialogSelected.name(), //
            qColorDialogSelected.name());
        QCOMPARE( //
            perceptualDialogSelected.alpha(), //
            qColorDialogSelected.alpha());
        QCOMPARE(perceptualDialogSelected.spec(), //
                 qColorDialogSelected.spec());
        const QColor perceptualDialogCurrent = perceptualDialog->currentColor();
        const QColor qColorDialogCurrent = qColorDialog->currentColor();
        QCOMPARE(perceptualDialogCurrent.name(), qColorDialogCurrent.name());
        QCOMPARE(perceptualDialogCurrent.alpha(), qColorDialogCurrent.alpha());
        QCOMPARE(perceptualDialogCurrent.spec(), qColorDialogCurrent.spec());
        QCOMPARE(perceptualDialog->testOption(QColorDialog::NoButtons), //
                 qColorDialog->testOption(QColorDialog::NoButtons));
        QCOMPARE( //
            perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            qColorDialog->testOption(QColorDialog::ShowAlphaChannel));
        QCOMPARE( //
            perceptualDialog->options().testFlag(QColorDialog::NoButtons), //
            qColorDialog->options().testFlag(QColorDialog::NoButtons));
        QCOMPARE( //
            perceptualDialog->options().testFlag(QColorDialog::ShowAlphaChannel),
            qColorDialog->options().testFlag(QColorDialog::ShowAlphaChannel));
        QCOMPARE(perceptualDialog->isVisible(), qColorDialog->isVisible());
        QCOMPARE(perceptualDialog->isModal(), qColorDialog->isModal());
        QCOMPARE(perceptualDialog->result(), qColorDialog->result());
        QCOMPARE(perceptualDialog->parent(), qColorDialog->parent());
        QCOMPARE(perceptualDialog->parentWidget(), //
                 qColorDialog->parentWidget());
    }

private Q_SLOTS:
    // This is just a helper slot, not an actual test.
    // It is neither called by QTest (because it has a parameter).
    void helperReceiveSignals(QColor color)
    {
        m_color = color;
    }

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

    void testDefaultConstructorAndDestructor1()
    {
        // This should not crash!
        ColorDialog test(m_srgbBuildinColorSpace);
        Q_UNUSED(test);
    }

    void testDefaultConstructorAndDestructor2()
    {
        // This should not crash!
        ColorDialog test;
        Q_UNUSED(test);
    }

    void testDefaultConstructorAndDestructor3()
    {
        QWidget myWidget;
        {
            // This should not crash!
            ColorDialog test(m_srgbBuildinColorSpace, &myWidget);
            Q_UNUSED(test);
        }
    }

    void testDefaultConstructorAndDestructor4()
    {
        QWidget myWidget;
        {
            // This should not crash!
            ColorDialog test(&myWidget);
            Q_UNUSED(test);
        }
    }

    void testConstructorQWidget()
    {
        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        QScopedPointer<QWidget> tempWidget{new QWidget()};
        QScopedPointer<ColorDialog> tempPerceptualDialog2;
        tempPerceptualDialog2.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, tempWidget.data()));
        QCOMPARE(tempPerceptualDialog2->parentWidget(), tempWidget.data());
        QCOMPARE(tempPerceptualDialog2->parent(), tempWidget.data());
    }

    void testConstructorQWidgetConformance()
    {
        // Test the constructor
        m_perceptualDialog.reset( //
            new ColorDialog( //
                m_srgbBuildinColorSpace, //
                QColor(Qt::white) //
                ) //
        );
        QScopedPointer<QWidget> tempWidget{new QWidget()};
        ColorDialog *tempPerceptualDialog2 = //
            new ColorDialog( //
                m_srgbBuildinColorSpace,
                QColor(Qt::white), //
                tempWidget.data() //
            );
        // Test if this coordinates is conform to QColorDialog
        m_qDialog.reset(new QColorDialog());
        QColorDialog *tempQDialog2 = new QColorDialog(tempWidget.data());
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        helperCompareDialog(tempPerceptualDialog2, tempQDialog2);
    }

    void testConstructorQColorQWidget_data()
    {
        helperProvideQColors();
    }

    void testConstructorQColorQWidget()
    {
        QFETCH(QColor, color);
        QColor colorOpaque;
        if (color.isValid()) {
            colorOpaque = color.toRgb();
            colorOpaque.setAlpha(255);
        } else {
            colorOpaque = Qt::black;
        }

        // Test the constructors
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, color));
        QScopedPointer<QWidget> tempWidget{new QWidget()};
        QScopedPointer<ColorDialog> tempPerceptualDialog2{
            //
            new ColorDialog( //
                m_srgbBuildinColorSpace, //
                color, //
                tempWidget.data() //
                ) //
        };

        // Test post-condition: currentColor() is color
        QCOMPARE(m_perceptualDialog->currentColor().name(), //
                 colorOpaque.name());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                 colorOpaque.alpha());
        QCOMPARE(m_perceptualDialog->currentColor().spec(), //
                 colorOpaque.spec());
        QCOMPARE(tempPerceptualDialog2->currentColor().name(), //
                 colorOpaque.name());
        QCOMPARE(tempPerceptualDialog2->currentColor().alpha(), //
                 colorOpaque.alpha());
        QCOMPARE(tempPerceptualDialog2->currentColor().spec(), //
                 colorOpaque.spec());
        QCOMPARE(tempPerceptualDialog2->parentWidget(), //
                 tempWidget.data());
        QCOMPARE(tempPerceptualDialog2->parent(), //
                 tempWidget.data());
    }

    void testConstructorQColorQWidgetConformance_data()
    {
        helperProvideQColors();
    }

    void testConstructorQColorQWidgetConformance()
    {
        QFETCH(QColor, color);

        // Test the constructor ColorDialog(QWidget * parent = nullptr)
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, color));
        QScopedPointer<QWidget> tempWidget{new QWidget()};
        ColorDialog *tempPerceptualDialog2 = new ColorDialog( //
            m_srgbBuildinColorSpace,
            color,
            tempWidget.data());
        // Test if this coordinates is conform to QColorDialog
        m_qDialog.reset(new QColorDialog(color));
        QColorDialog *tempQDialog2 = //
            new QColorDialog(color, tempWidget.data());
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        helperCompareDialog(tempPerceptualDialog2, tempQDialog2);
    }

    void testConformanceWithQColorDialog_data()
    {
        QTest::addColumn<QColor>("initialColor");
        QTest::addColumn<QColor>("secondColor");
        QTest::addColumn<bool>("showAlphaChannel");

        QVector<QPair<QByteArray, QColor>> colorList;

        colorList.append( //
            QPair<QByteArray, QColor>(QByteArrayLiteral("redOpaque"), //
                                      QColor(255, 0, 0)));

        colorList.append( //
            QPair<QByteArray, QColor>(QByteArrayLiteral("greenHalf"), //
                                      QColor(0, 255, 0, 128)));

        colorList.append( //
            QPair<QByteArray, QColor>(QByteArrayLiteral("greenTransparent"), //
                                      QColor(255, 0, 255, 0)));

        colorList.append( //
            QPair<QByteArray, QColor>(QByteArrayLiteral("invalid"), //
                                      QColor()));

        for (int i = 0; i < colorList.size(); ++i) {
            for (int j = 0; j < colorList.size(); ++j) {
                const QByteArray descriptionWithoutAlpha = //
                    colorList.at(i).first //
                    + QByteArrayLiteral("/") //
                    + colorList.at(j).first;
                const QByteArray descriptionWithAlpha = //
                    descriptionWithoutAlpha //
                    + QByteArrayLiteral("/ShowAlphaChannel"); //
                QTest::newRow(descriptionWithAlpha.constData()) //
                    << colorList.at(i).second //
                    << colorList.at(j).second //
                    << true;
                QTest::newRow(descriptionWithoutAlpha.constData()) //
                    << colorList.at(i).second //
                    << colorList.at(j).second //
                    << false;
            }
        }
    }

    void testConformanceWithQColorDialog()
    {
        // Some conformance tests (without a particular systematic approach)
        QFETCH(QColor, initialColor);
        QFETCH(QColor, secondColor);
        QFETCH(bool, showAlphaChannel);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, initialColor));
        m_qDialog.reset(new QColorDialog(initialColor));
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            showAlphaChannel);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel,
            showAlphaChannel);

        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
    }

    void testConformanceWithQColorDialogNoButtons_data()
    {
        QTest::addColumn<bool>("showAlphaChannel");
        QTest::addColumn<bool>("noButtons");

        QTest::newRow("/ShowAlphaChannel/NoButtons") << true << true;
        QTest::newRow("/ShowAlphaChannel") << true << false;
        QTest::newRow("/NoButtons") << false << true;
        QTest::newRow("") << false << false;
    }

    void testConformanceWithQColorDialogNoButtons()
    {
        // Some conformance tests (without a particular systematic approach)
        QColor initialColor = Qt::red;
        QColor secondColor = Qt::green;
        QFETCH(bool, showAlphaChannel);
        QFETCH(bool, noButtons);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, initialColor));
        m_qDialog.reset(new QColorDialog(initialColor));
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            showAlphaChannel);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel,
            showAlphaChannel);

        m_perceptualDialog->setOption(QColorDialog::NoButtons, noButtons);
        m_qDialog->setOption(QColorDialog::NoButtons, noButtons);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setCurrentColor(secondColor);
        m_qDialog->setCurrentColor(secondColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
    }

    void testColorSelectedSignal()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->show();
        m_qDialog.reset(new QColorDialog());
        m_qDialog->show();
        QSignalSpy spyPerceptualDialog( //
            m_perceptualDialog.data(), //
            &ColorDialog::colorSelected);
        QSignalSpy spyQDialog(m_qDialog.data(), &QColorDialog::colorSelected);
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        m_perceptualDialog->show();
        m_qDialog->show();
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
        m_perceptualDialog->show();
        m_qDialog->show();
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 2);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
    }

    void testPropertyConformance_data()
    {
        // We provide the property names as data. To get the property names,
        // this function is used:
        // const char *QMetaProperty::name() const
        // Now, the type “const char *” cannot be used with QTest::addColumn<>
        // because it is not known to Qt’s meta object system. The
        // meta object system requires copy constructors for its known types.
        // This might get wired with pointer types that might go out of scope.
        // Therefore, we create a QByteArray from the data, which can be passed
        // without problems through the meta object system. The good thing
        // is that for the conversion we do not need to know anything
        // about the actual encoding of “const char *”.
        QTest::addColumn<QByteArray>("propertyName");
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        for (int i = 0; i < referenceClass.propertyCount(); ++i) {
            QTest::newRow(referenceClass.property(i).name()) //
                << QByteArray(referenceClass.property(i).name());
        }
    }

    void testPropertyConformance()
    {
        QFETCH(QByteArray, propertyName);
        QMetaObject testClass = ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        int testClassIndex = //
            testClass.indexOfProperty(propertyName.constData());
        int referenceClassIndex = //
            referenceClass.indexOfProperty(propertyName.constData());
        QMetaProperty referenceClassProperty = //
            referenceClass.property(referenceClassIndex);
        // cppcheck-suppress unreadVariable // false positive
        QByteArray message = //
            QByteArrayLiteral("Test if property \"") //
            + QByteArray(referenceClassProperty.name()) //
            + QByteArrayLiteral("\" of class \"") //
            + QByteArray(referenceClass.className()) //
            + QByteArrayLiteral("\" is also available in \"") //
            + QByteArray(testClass.className()) //
            + QByteArrayLiteral("\".");
        QVERIFY2(testClassIndex >= 0, message.constData());
        QMetaProperty testClassProperty = testClass.property(testClassIndex);
        if (referenceClassProperty.hasNotifySignal()) {
            QVERIFY2(testClassProperty.hasNotifySignal(),
                     "If the reference class has a notify signal, "
                     "the test class must have also a notify signal.");
        }
        QCOMPARE(testClassProperty.isConstant(), //
                 referenceClassProperty.isConstant());
        QCOMPARE(testClassProperty.isDesignable(), //
                 referenceClassProperty.isDesignable());
        QCOMPARE(testClassProperty.isEnumType(), //
                 referenceClassProperty.isEnumType());
        if (referenceClassProperty.isEnumType()) {
            QCOMPARE(testClassProperty.enumerator().enumName(), //
                     referenceClassProperty.enumerator().enumName());
            QCOMPARE(testClassProperty.enumerator().isFlag(), //
                     referenceClassProperty.enumerator().isFlag());
            QCOMPARE(testClassProperty.enumerator().isScoped(), //
                     referenceClassProperty.enumerator().isScoped());
            QCOMPARE(testClassProperty.enumerator().isValid(), //
                     referenceClassProperty.enumerator().isValid());
            QCOMPARE(testClassProperty.enumerator().keyCount(), //
                     referenceClassProperty.enumerator().keyCount());
            QCOMPARE(testClassProperty.enumerator().name(), //
                     referenceClassProperty.enumerator().name());
            QCOMPARE(testClassProperty.enumerator().scope(), //
                     referenceClassProperty.enumerator().scope());
        }
        QCOMPARE(testClassProperty.isFinal(), //
                 referenceClassProperty.isFinal());
        QCOMPARE(testClassProperty.isFlagType(), //
                 referenceClassProperty.isFlagType());
        QCOMPARE(testClassProperty.isReadable(), //
                 referenceClassProperty.isReadable());
        QCOMPARE(testClassProperty.isResettable(), //
                 referenceClassProperty.isResettable());
        QCOMPARE(testClassProperty.isScriptable(), //
                 referenceClassProperty.isScriptable());
        QCOMPARE(testClassProperty.isStored(), //
                 referenceClassProperty.isStored());
        QCOMPARE(testClassProperty.isUser(), //
                 referenceClassProperty.isUser());
        QCOMPARE(testClassProperty.isValid(), //
                 referenceClassProperty.isValid());
        QCOMPARE(testClassProperty.isWritable(), //
                 referenceClassProperty.isWritable());
        QCOMPARE(testClassProperty.isWritable(), //
                 referenceClassProperty.isWritable());
        QCOMPARE(testClassProperty.name(), //
                 referenceClassProperty.name());
        if (referenceClassProperty.hasNotifySignal()) {
            QCOMPARE(testClassProperty.notifySignal().methodSignature(), //
                     referenceClassProperty.notifySignal().methodSignature());
            QCOMPARE(testClassProperty.notifySignal().methodType(), //
                     referenceClassProperty.notifySignal().methodType());
            QCOMPARE(testClassProperty.notifySignal().name(), //
                     referenceClassProperty.notifySignal().name());
            QCOMPARE(testClassProperty.notifySignal().parameterCount(), //
                     referenceClassProperty.notifySignal().parameterCount());
            QCOMPARE(testClassProperty.notifySignal().parameterNames(), //
                     referenceClassProperty.notifySignal().parameterNames());
            QCOMPARE(testClassProperty.notifySignal().parameterTypes(), //
                     referenceClassProperty.notifySignal().parameterTypes());
            QCOMPARE(testClassProperty.notifySignal().returnType(), //
                     referenceClassProperty.notifySignal().returnType());
            QCOMPARE(testClassProperty.notifySignal().revision(), //
                     referenceClassProperty.notifySignal().revision());
            QCOMPARE(testClassProperty.notifySignal().tag(), //
                     referenceClassProperty.notifySignal().tag());
            QCOMPARE(testClassProperty.notifySignal().typeName(), //
                     referenceClassProperty.notifySignal().typeName());
        }
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        QCOMPARE(testClassProperty.metaType(), //
                 referenceClassProperty.metaType());
#else
        QCOMPARE(testClassProperty.type(), referenceClassProperty.type());
#endif
        QCOMPARE(testClassProperty.typeName(), //
                 referenceClassProperty.typeName());
        QCOMPARE(testClassProperty.userType(), //
                 referenceClassProperty.userType());
    }

    void testMethodConformance_data()
    {
        QTest::addColumn<QByteArray>("methodSignature");
        QTest::addColumn<int>("referenceClassIndex");
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        for (int i = 0; i < referenceClass.methodCount(); ++i) {
            if (referenceClass.method(i).access() != QMetaMethod::Private) {
                // Exclude private methods from conformance check
                QTest::newRow(referenceClass.method(i).name().data()) //
                    << QMetaObject::normalizedSignature( //
                           referenceClass.method(i).methodSignature().data()) //
                    << i;
            }
        }
    }

    void testMethodConformance()
    {
        // We do only check QMetaObject::method() and
        // not QMetaObject::constructor because QColorDialog
        // does not provide its constructors to the
        // meta-object system.
        QFETCH(QByteArray, methodSignature);
        QFETCH(int, referenceClassIndex);
        QMetaObject testClass = ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        int testClassIndex = testClass.indexOfMethod(methodSignature.data());
        QMetaMethod referenceClassMethod = //
            referenceClass.method(referenceClassIndex);
        // cppcheck-suppress unreadVariable // false positive
        QByteArray message = //
            QByteArrayLiteral("Test if method \"") //
            + QByteArray(referenceClassMethod.methodSignature()) //
            + QByteArrayLiteral("\" of class \"") //
            + QByteArray(referenceClass.className()) //
            + QByteArrayLiteral("\" is also available in \"") //
            + QByteArray(testClass.className()) //
            + QByteArrayLiteral("\".");
        QVERIFY2(testClassIndex >= 0, message.constData());
        QMetaMethod testClassMethod = testClass.method(testClassIndex);
        QCOMPARE(testClassMethod.access(), //
                 referenceClassMethod.access());
        QCOMPARE(testClassMethod.isValid(), //
                 referenceClassMethod.isValid());
        QCOMPARE(testClassMethod.methodSignature(), //
                 referenceClassMethod.methodSignature());
        QCOMPARE(testClassMethod.methodType(), //
                 referenceClassMethod.methodType());
        QCOMPARE(testClassMethod.name(), //
                 referenceClassMethod.name());
        QCOMPARE(testClassMethod.parameterCount(), //
                 referenceClassMethod.parameterCount());
        QCOMPARE(testClassMethod.parameterNames(), //
                 referenceClassMethod.parameterNames());
        QCOMPARE(testClassMethod.parameterTypes(), //
                 referenceClassMethod.parameterTypes());
        QCOMPARE(testClassMethod.returnType(), //
                 referenceClassMethod.returnType());
        QCOMPARE(testClassMethod.revision(), //
                 referenceClassMethod.revision());
        QCOMPARE(testClassMethod.tag(), //
                 referenceClassMethod.tag());
        QCOMPARE(testClassMethod.typeName(), //
                 referenceClassMethod.typeName());
    }

    void testRttiConformance()
    {
        QMetaObject testClass = ColorDialog::staticMetaObject;
        QMetaObject referenceClass = QColorDialog::staticMetaObject;
        QVERIFY2(testClass.inherits(referenceClass.superClass()),
                 "Test that ColorDialog inherits "
                 "from QColorDialog’s superclass.");
    }

    void testCurrentColorChangedSignal()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_qDialog.reset(new QColorDialog());
        m_perceptualDialog->show();
        m_qDialog->show();
        QSignalSpy spyPerceptualDialog( //
            m_perceptualDialog.data(), //
            &ColorDialog::currentColorChanged);
        QSignalSpy spyQDialog(m_qDialog.data(), //
                              &QColorDialog::currentColorChanged);

        // Test that a simple “return key” click by the user
        // does not call this signal
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(spyPerceptualDialog.count(), 0);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());

        // A different color should call the signal
        m_perceptualDialog->setCurrentColor(QColor(1, 2, 3));
        m_qDialog->setCurrentColor(QColor(1, 2, 3));
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());

        // The same color again should not call again the signal
        m_perceptualDialog->setCurrentColor(QColor(1, 2, 3));
        m_qDialog->setCurrentColor(QColor(1, 2, 3));
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());

        // The same RGB values, but defined in another color model, should not
        // emit a signal either.
        m_perceptualDialog->setCurrentColor(QColor(1, 2, 3).toHsl());
        m_qDialog->setCurrentColor(QColor(1, 2, 3).toHsl());
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());

        // Changing QColorDialog::ShowAlphaChannel should
        // not emit a signal either
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        QCOMPARE(spyPerceptualDialog.count(), spyQDialog.count());
    }

    void testCurrentColorProperty_data()
    {
        helperProvideQColors();
    }

    void testCurrentColorProperty()
    {
        QFETCH(QColor, color);
        QColor correctedColor;
        if (color.isValid()) {
            correctedColor = color.toRgb();
        } else {
            correctedColor = Qt::black;
        }
        QColor opaqueColor = correctedColor;
        opaqueColor.setAlpha(255);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_qDialog.reset(new QColorDialog);

        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            true);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            true);
        m_perceptualDialog->setCurrentColor(color);
        m_qDialog->setCurrentColor(color);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), //
                 m_qDialog->currentColor().rgb());
        if (color.isValid()) {
            // Testing alpha value conformance only for valid QColor inputs,
            // because QColorDialog has here some surprising behaviour that
            // we won’t imitate.
            QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                     m_qDialog->currentColor().alpha());
        }
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), //
                 correctedColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                 correctedColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(correctedColor.spec()));

        // Test that changing QColorDialog::ShowAlphaChannel
        // alone does not change the currentColor property
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), //
                 m_qDialog->currentColor().rgb());
        if (color.isValid()) {
            // Testing alpha value conformance only for valid QColor inputs,
            // because QColorDialog has here some surprising behaviour that
            // we won’t imitate.
            QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                     m_qDialog->currentColor().alpha());
        }
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), //
                 correctedColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                 correctedColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(correctedColor.spec()));

        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, false);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        m_perceptualDialog->setCurrentColor(color);
        m_qDialog->setCurrentColor(color);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), //
                 m_qDialog->currentColor().rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                 m_qDialog->currentColor().alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), opaqueColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                 opaqueColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(opaqueColor.spec()));

        // Test that changing QColorDialog::ShowAlphaChannel
        // alone does not change the currentColor property
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            true);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            true);
        // Test conformance (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), //
                 m_qDialog->currentColor().rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                 m_qDialog->currentColor().alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(m_qDialog->currentColor().spec()));
        // Test post condition (but only integer precision)
        QCOMPARE(m_perceptualDialog->currentColor().rgb(), opaqueColor.rgb());
        QCOMPARE(m_perceptualDialog->currentColor().alpha(), //
                 opaqueColor.alpha());
        QCOMPARE(static_cast<int>(m_perceptualDialog->currentColor().spec()), //
                 static_cast<int>(opaqueColor.spec()));
    }

    void testSetCurrentColor()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->show();
        m_perceptualDialog->setCurrentColor(Qt::yellow);

        // Get internal LCH value
        const LchDouble color = //
            m_perceptualDialog->d_pointer->m_currentOpaqueColor.cielch;

        // The very same LCH value has to be found in all widgets using it.
        // (This is not trivial, because even coming from RGB, because of
        // rounding errors, you can get out-of-gamut LCH values when the
        // original RGB value was near to the border. And the child
        // widgets may change the LCH value that is given to them
        // to fit it into the gamut – each widget with a different
        // algorithm.)
        QVERIFY( //
            color.hasSameCoordinates( //
                m_perceptualDialog //
                    ->d_pointer //
                    ->m_wheelColorPicker //
                    ->currentColor()));
        QVERIFY( //
            color.hasSameCoordinates( //
                m_perceptualDialog //
                    ->d_pointer //
                    ->m_chromaHueDiagram //
                    ->currentColor()));
        // We do not also control this here for
        // m_perceptualDialog->d_pointer->m_ciehlcSpinBox because this
        // widget rounds the given value to the current decimal precision
        // it’s using. Therefore, it’s pointless to control here
        // for rounding errors.
    }

    void testOpen()
    {
        // Test our reference (QColorDialog)
        m_color = Qt::black;
        m_qDialog.reset(new QColorDialog);
        m_qDialog->setCurrentColor(Qt::white);
        m_qDialog->open(this, SLOT(helperReceiveSignals(QColor)));
        m_qDialog->setCurrentColor(Qt::red);
        // Changing the current color does not emit the signal
        QCOMPARE(m_color, Qt::black);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        // Return key really emits a signal
        QCOMPARE(m_color, Qt::red);
        m_qDialog->show();
        m_qDialog->setCurrentColor(Qt::green);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        // The signal is really disconnected after the dialog has been closed.
        QCOMPARE(m_color, Qt::red);

        // Now test if ColorDialog does the same
        // thing as our reference
        m_color = Qt::black;
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->setCurrentColor(Qt::white);
        m_perceptualDialog->open(this, SLOT(helperReceiveSignals(QColor)));
        m_perceptualDialog->setCurrentColor(Qt::red);
        // Changing the current color does not emit the signal
        QCOMPARE(m_color, Qt::black);
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        // Return key really emits a signal
        QCOMPARE(m_color, Qt::red);
        m_perceptualDialog->show();
        m_perceptualDialog->setCurrentColor(Qt::green);
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        // The signal is really disconnected after the dialog has been closed.
        QCOMPARE(m_color, Qt::red);
    }

    void testDefaultOptions()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_qDialog.reset(new QColorDialog);
        QCOMPARE( //
            m_perceptualDialog->testOption( //
                QColorDialog::DontUseNativeDialog), //
            true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            m_qDialog->testOption(QColorDialog::NoButtons));
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            m_qDialog->testOption(QColorDialog::ShowAlphaChannel));
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::DontUseNativeDialog), //
            true);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag(QColorDialog::NoButtons), //
            false);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag(QColorDialog::NoButtons), //
            m_qDialog->options().testFlag(QColorDialog::NoButtons));
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::ShowAlphaChannel), //
            false);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::ShowAlphaChannel), //
            m_qDialog->options().testFlag(QColorDialog::ShowAlphaChannel));
    }

    void testOptionDontUseNativeDialogAlwaysTrue()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->setOption(QColorDialog::DontUseNativeDialog);
        QCOMPARE( //
            m_perceptualDialog->testOption( //
                QColorDialog::DontUseNativeDialog), //
            true);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::DontUseNativeDialog), //
            true);
        m_perceptualDialog->setOptions(QColorDialog::DontUseNativeDialog);
        QCOMPARE( //
            m_perceptualDialog->testOption( //
                QColorDialog::DontUseNativeDialog), //
            true);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::DontUseNativeDialog),
            true);
        m_perceptualDialog->setOptions( //
            QColorDialog::DontUseNativeDialog | QColorDialog::NoButtons);
        QCOMPARE( //
            m_perceptualDialog->testOption( //
                QColorDialog::DontUseNativeDialog), //
            true);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::DontUseNativeDialog), //
            true);
        m_perceptualDialog->setOptions( //
            QColorDialog::DontUseNativeDialog | QColorDialog::ShowAlphaChannel);
        QCOMPARE( //
            m_perceptualDialog->testOption( //
                QColorDialog::DontUseNativeDialog), //
            true);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::DontUseNativeDialog), //
            true);
        m_perceptualDialog->setOptions( //
            QColorDialog::DontUseNativeDialog //
            | QColorDialog::ShowAlphaChannel //
            | QColorDialog::NoButtons);
        QCOMPARE( //
            m_perceptualDialog->testOption( //
                QColorDialog::DontUseNativeDialog), //
            true);
        QCOMPARE( //
            m_perceptualDialog->options().testFlag( //
                QColorDialog::DontUseNativeDialog), //
            true);
    }

    void testOptionShowAlpha()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace, QColor(Qt::white)));
        m_qDialog.reset(new QColorDialog);
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel);
        QVERIFY2( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            "ShowAlphaChannel successfully set.");
        m_qDialog->setOption(QColorDialog::ShowAlphaChannel);
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QColor tempColor = QColor(1, 101, 201, 155);
        m_perceptualDialog->setCurrentColor(tempColor);
        m_qDialog->setCurrentColor(tempColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        QVERIFY2( //
            !m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            "ShowAlphaChannel successfully set.");
        m_qDialog->setOption(QColorDialog::ShowAlphaChannel, false);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        tempColor = QColor(5, 105, 205, 133);
        m_perceptualDialog->setCurrentColor(tempColor);
        m_qDialog->setCurrentColor(tempColor);
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
    }

    void testOptionNoButtons()
    {
        m_perceptualDialog.reset( //
            new ColorDialog( //
                m_srgbBuildinColorSpace, //
                QColor(Qt::white) //
                ) //
        );
        m_qDialog.reset(new QColorDialog);
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons);
        QVERIFY2( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            "NoButtons successfully set to true.");
        m_qDialog->setOption(QColorDialog::NoButtons);
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(m_perceptualDialog->isVisible(), //
                 "Should still visible after Return key pressed.");
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(!m_perceptualDialog->isVisible(), //
                 "Should no longer be visible after Escape key pressed.");
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());

        m_perceptualDialog->setOption(QColorDialog::NoButtons, false);
        QVERIFY2(!m_perceptualDialog->testOption(QColorDialog::NoButtons), //
                 "NoButtons successfully set to false.");
        m_qDialog->setOption(QColorDialog::NoButtons, false);
        m_perceptualDialog->show();
        m_qDialog->show();
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->isVisible(), m_qDialog->isVisible());
        QVERIFY2(!m_perceptualDialog->isVisible(), //
                 "Should no longer be visible after Return key pressed.");
        helperCompareDialog(m_perceptualDialog.data(), m_qDialog.data());
    }

    void testSetOptionAndTestOptionInteraction()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // Test if the option changes as expected
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            true);
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            false);
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            true);
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            false);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // Test if the option changes as expected
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            false);
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            true);
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            false);
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel, //
                                      true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // Test if the option changes as expected
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            true);
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::NoButtons), //
                 false);
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            true);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::NoButtons), //
                 true);
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            false);
        QCOMPARE(m_perceptualDialog->testOption(QColorDialog::NoButtons), //
                 false);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // Test if the option changes as expected
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            false);
        m_perceptualDialog->setOption(QColorDialog::NoButtons, true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            true);
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            false);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            false);
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            true);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons), //
            true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // define an option
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            true);
        // change some other option
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            true);
        // test if first option is still unchanged
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // define an option
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        // change some other option
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            true);
        // test if first option is still unchanged
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            false);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // define an option
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            true);
        // change some other option
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            false);
        // test if first option is still unchanged
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            true);
        m_perceptualDialog.reset(nullptr);

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // define an option
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            false);
        // change some other option
        m_perceptualDialog->setOption( //
            QColorDialog::NoButtons, //
            false);
        // test if first option is still unchanged
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            false);
        m_perceptualDialog.reset(nullptr);
    }

    void testAlphaSpinbox()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->setOption( //
            QColorDialog::ShowAlphaChannel, //
            true);
        m_perceptualDialog->d_pointer->m_alphaGradientSlider->setValue(0.504);
        QCOMPARE( //
            m_perceptualDialog->d_pointer->m_alphaGradientSlider->value(), //
            0.504);
        QCOMPARE(m_perceptualDialog->d_pointer->m_alphaSpinBox->value(), 50);
        QTest::keyClick(m_perceptualDialog->d_pointer->m_alphaSpinBox, //
                        Qt::Key_Up);
        QCOMPARE( //
            m_perceptualDialog->d_pointer->m_alphaGradientSlider->value(), //
            0.51);
        QCOMPARE(m_perceptualDialog->d_pointer->m_alphaSpinBox->value(), 51);
    }

    void testSelectedColorAndSetVisible()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_qDialog.reset(new QColorDialog);
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->setCurrentColor(QColor(Qt::blue));
        m_qDialog->setCurrentColor(QColor(Qt::blue));
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        // Still no valid selectedColor() because the dialog still wasn't shown
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor(Qt::blue));
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Escape);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Escape);
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->setVisible(true);
        m_qDialog->setVisible(true);
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        QTest::keyClick(m_perceptualDialog.data(), Qt::Key_Return);
        QTest::keyClick(m_qDialog.data(), Qt::Key_Return);
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor(Qt::blue));
        m_perceptualDialog->show();
        m_qDialog->show();
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
        m_perceptualDialog->hide();
        m_qDialog->hide();
        QCOMPARE(m_perceptualDialog->selectedColor(), //
                 m_qDialog->selectedColor());
        QCOMPARE(m_perceptualDialog->selectedColor(), QColor());
    }

    void testAliases()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_qDialog.reset(new QColorDialog);

        // Test setting QColorDialog syntax
        m_perceptualDialog->setOption(QColorDialog::ShowAlphaChannel);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::ShowAlphaChannel), //
            true);
        QCOMPARE( //
            m_perceptualDialog->testOption //
            (ColorDialog::ColorDialogOption::ShowAlphaChannel), //
            true);
        m_qDialog->setOption( //
            QColorDialog::ShowAlphaChannel);
        QCOMPARE(m_qDialog->testOption(QColorDialog::ShowAlphaChannel), //
                 true);
        QCOMPARE( //
            m_qDialog->testOption( //
                ColorDialog::ColorDialogOption::ShowAlphaChannel), //
            true);

        // Test setting our alias syntax
        m_perceptualDialog->setOption( //
            ColorDialog::ColorDialogOption::NoButtons);
        QCOMPARE( //
            m_perceptualDialog->testOption(QColorDialog::NoButtons),
            true);
        QCOMPARE( //
            m_perceptualDialog->testOption( //
                ColorDialog::ColorDialogOption::NoButtons), //
            true);
        m_qDialog->setOption( //
            ColorDialog::ColorDialogOption::NoButtons);
        QCOMPARE(m_qDialog->testOption(QColorDialog::NoButtons), //
                 true);
        QCOMPARE( //
            m_qDialog->testOption(ColorDialog::ColorDialogOption::NoButtons), //
            true);

        // Test if ColorDialogOptions is compatible (at least for == operator)
        // Configure conformance with our dialog:
        m_qDialog->setOption(QColorDialog::DontUseNativeDialog);
        QCOMPARE(m_perceptualDialog->options(), m_qDialog->options());
    }

    void testReadLightnessValues()
    {
        QScopedPointer<ColorDialog> myDialog( //
            new ColorDialog(m_srgbBuildinColorSpace));
        myDialog->d_pointer->m_lchLightnessSelector->setValue(0.6);
        myDialog->d_pointer->readLightnessValue();
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.cielch.l, 60);
    }

    void testReadHlcNumericValues()
    {
        QScopedPointer<ColorDialog> myDialog( //
            new ColorDialog(m_srgbBuildinColorSpace));
        QList<double> myValues = //
            myDialog->d_pointer->m_ciehlcSpinBox->sectionValues();

        // Test with a normal value
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_ciehlcSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readHlcNumericValues();
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.cielch.h, 10);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.cielch.l, 11);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.cielch.c, 12);

        // Test with an out-of-gamut value.
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_ciehlcSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readHlcNumericValues();
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.cielch.h, 10);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.cielch.l, 11);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor.cielch.c, 12);
    }

    void testReadHsvNumericValues()
    {
        QScopedPointer<ColorDialog> myDialog( //
            new ColorDialog(m_srgbBuildinColorSpace));
        QList<double> myValues = //
            myDialog->d_pointer->m_hsvSpinBox->sectionValues();
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_hsvSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readHsvNumericValues();
        QCOMPARE(qRound(myDialog->currentColor().hueF() * 360), 10);
        QCOMPARE(qRound(myDialog->currentColor().saturationF() * 100), 11);
        QCOMPARE(qRound(myDialog->currentColor().valueF() * 100), 12);
    }

    void testReadRgbHexValues()
    {
        QScopedPointer<ColorDialog> myDialog( //
            new ColorDialog(m_srgbBuildinColorSpace));

        // Test some value
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#010203"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 1);
        QCOMPARE(myDialog->currentColor().green(), 2);
        QCOMPARE(myDialog->currentColor().blue(), 3);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), //
                 QStringLiteral("#010203"));

        // Test this value which is known to have triggered yet rounding errors!
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#ff0000"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 255);
        QCOMPARE(myDialog->currentColor().green(), 0);
        QCOMPARE(myDialog->currentColor().blue(), 0);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), //
                 QStringLiteral("#ff0000"));

        // Test this value which is known to have triggered yet rounding errors!
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#ef6c00"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 239);
        QCOMPARE(myDialog->currentColor().green(), 108);
        QCOMPARE(myDialog->currentColor().blue(), 0);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), //
                 QStringLiteral("#ef6c00"));

        // Test this value which is known to have triggered yet rounding errors!
        myDialog->d_pointer->m_rgbLineEdit->setText(QStringLiteral("#ffff00"));
        myDialog->d_pointer->readRgbHexValues();
        QCOMPARE(myDialog->currentColor().red(), 255);
        QCOMPARE(myDialog->currentColor().green(), 255);
        QCOMPARE(myDialog->currentColor().blue(), 0);
        QCOMPARE(myDialog->d_pointer->m_rgbLineEdit->text(), //
                 QStringLiteral("#ffff00"));
    }

    void testReadRgbNumericValues()
    {
        QScopedPointer<ColorDialog> myDialog( //
            new ColorDialog(m_srgbBuildinColorSpace));
        QList<double> myValues = //
            myDialog->d_pointer->m_rgbSpinBox->sectionValues();
        myValues[0] = 10;
        myValues[1] = 11;
        myValues[2] = 12;
        myDialog->d_pointer->m_rgbSpinBox->setSectionValues(myValues);
        myDialog->d_pointer->readRgbNumericValues();
        QCOMPARE(myDialog->currentColor().red(), 10);
        QCOMPARE(myDialog->currentColor().green(), 11);
        QCOMPARE(myDialog->currentColor().blue(), 12);
    }

    void testSetCurrentOpaqueColor()
    {
        QScopedPointer<ColorDialog> myDialog( //
            new ColorDialog(m_srgbBuildinColorSpace));
        LchDouble myOpaqueColor;
        myOpaqueColor.l = 30;
        myOpaqueColor.c = 40;
        myOpaqueColor.h = 50;
        const MultiColor myMultiColor = MultiColor::fromCielch( //
            myDialog->d_pointer->m_rgbColorSpace,
            myOpaqueColor);
        myDialog->d_pointer->setCurrentOpaqueColor(myMultiColor, nullptr);
        QCOMPARE(myDialog->d_pointer->m_currentOpaqueColor, myMultiColor);
        QList<double> myValues = //
            myDialog->d_pointer->m_rgbSpinBox->sectionValues();
        QCOMPARE(qRound(myValues.at(0)), 113);
        QCOMPARE(qRound(myValues.at(1)), 53);
        QCOMPARE(qRound(myValues.at(2)), 23);
    }

    void testUpdateColorPatch()
    {
        QScopedPointer<ColorDialog> myDialog( //
            new ColorDialog(m_srgbBuildinColorSpace));
        myDialog->d_pointer->m_currentOpaqueColor = //
            MultiColor::fromMultiRgb( //
                myDialog->d_pointer->m_rgbColorSpace,
                MultiRgb::fromRgbQColor(QColor(1, 2, 3)));
        myDialog->d_pointer->updateColorPatch();
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().red(), 1);
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().green(), 2);
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().blue(), 3);
        QCOMPARE(myDialog->d_pointer->m_colorPatch->color().alphaF(), //
                 myDialog->d_pointer->m_alphaGradientSlider->value());
    }

    void testSizeGrip()
    {
        // As this dialog can indeed be resized, the size grip should
        // be enabled. So, users can see the little triangle at the
        // right bottom of the dialog (or the left bottom on a
        // right-to-left layout). So, the user will be aware
        // that he can indeed resize this dialog, which is
        // important as the users are used to the default
        // platform dialog, which often does not allow resizing. Therefore,
        // by default, QDialog::isSizeGripEnabled() should be true.
        // NOTE: Some widget styles like Oxygen or Breeze leave the size grip
        // widget invisible; nevertheless it reacts on mouse events. Other
        // widget styles indeed show the size grip widget, like Fusion or
        // QtCurve.
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        QCOMPARE(m_perceptualDialog->isSizeGripEnabled(), true);
        m_perceptualDialog->show();
        QCOMPARE(m_perceptualDialog->isSizeGripEnabled(), true);
        m_perceptualDialog->hide();
        QCOMPARE(m_perceptualDialog->isSizeGripEnabled(), true);
    }

    void testLayoutDimensions()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // Test default value
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::Collapsed);

        // Test if values are correctly stored before showing
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Collapsed);
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::Collapsed);
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::Expanded);
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::ScreenSizeDependent);
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::ScreenSizeDependent);

        // Test if values are correctly stored after showing
        m_perceptualDialog->show();
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Collapsed);
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::Collapsed);
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::Expanded);
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::ScreenSizeDependent);
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::ScreenSizeDependent);
    }

    void testApplyLayoutDimensions()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        // Test default value
        QCOMPARE( //
            m_perceptualDialog->layoutDimensions(), //
            ColorDialog::DialogLayoutDimensions::Collapsed);

        m_perceptualDialog->d_pointer->m_layoutDimensions = //
            ColorDialog::DialogLayoutDimensions::Collapsed;
        m_perceptualDialog->d_pointer->applyLayoutDimensions();
        int collapsedWidth = m_perceptualDialog->width();

        m_perceptualDialog->d_pointer->m_layoutDimensions = //
            ColorDialog::DialogLayoutDimensions::Expanded;
        m_perceptualDialog->d_pointer->applyLayoutDimensions();
        int expandedWidth = m_perceptualDialog->width();

        QVERIFY2(collapsedWidth < expandedWidth,
                 "Verify that collapsed width of the dialog is smaller than "
                 "the expanded width.");
    }

    void testLayoutDimensionsChanged()
    {
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Collapsed);
        QSignalSpy spyPerceptualDialog(
            // QObject to spy:
            m_perceptualDialog.data(),
            // Signal to spy:
            &ColorDialog::layoutDimensionsChanged);
        // Setting a different DialogLayoutDimensions will emit a signal
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        QCOMPARE(spyPerceptualDialog.count(), 1);
        // Setting the same DialogLayoutDimensions will not emit a signal again
        m_perceptualDialog->setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        QCOMPARE(spyPerceptualDialog.count(), 1);
    }

    void testRoundingErrors_data()
    {
        QTest::addColumn<QColor>("color");
        QTest::newRow("Qt::yellow") << QColor(Qt::yellow);
        QColorFloatType red = 1;
        QColorFloatType green = 1;
        QColorFloatType blue = 0;
        while (blue < 1) {
            QTest::newRow( //
                QStringLiteral("RGB %1 %2 %3") //
                    .arg(red) //
                    .arg(green) //
                    .arg(blue) //
                    .toUtf8() //
                    .data()) //
                << QColor::fromRgbF(red, green, blue);
            blue += 0.1f;
        }
    }

    void testRoundingErrors()
    {
        QFETCH(QColor, color);

        // Moving around between the widgets with the Tab key should
        // never trigger a value change. (There could be a value
        // change because of rounding errors if the value gets updated
        // after the focus leaves, even though no editing has been
        // done. This would not be correct, and this test controls this.)

        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->setCurrentColor(color);
        m_perceptualDialog->show();
        QApplication::setActiveWindow(m_perceptualDialog.data());
        // Using a QList instead of a simple QWidget* to make sure
        // not to have infinite loops even is focus passing might be
        // broken and never returns to the original focus.
        QList<QWidget *> oldFocusWidgets;
        for (int i = 0; //
             i < m_perceptualDialog->d_pointer->m_tabWidget->count(); //
             i++ //
        ) {
            m_perceptualDialog->d_pointer->m_tabWidget->setCurrentIndex(i);
            oldFocusWidgets.clear();
            const QColor oldColor = m_perceptualDialog->currentColor();
            const MultiColor oldOpaqueLchColor = //
                m_perceptualDialog->d_pointer->m_currentOpaqueColor;
            bool focusPassingIsWorking = true;
            while ( //
                focusPassingIsWorking //
                && (!oldFocusWidgets.contains(QApplication::focusWidget())) //
            ) {
                oldFocusWidgets.append(QApplication::focusWidget());
                focusPassingIsWorking = m_perceptualDialog->focusNextChild();
                QCOMPARE(oldColor, m_perceptualDialog->currentColor());
                QVERIFY( //
                    oldOpaqueLchColor //
                    == m_perceptualDialog->d_pointer->m_currentOpaqueColor);
            }
        };
    }

    void testYellowRounding()
    {
        // During development was observed a particular bug for which
        // we test here.

        // As we expect rounding errors, we define a tolerance range,
        // which is used both for the assertions and for the actual test.
        // This is necessary to guarantee that this test does not produce
        // false-positives just because the rounding behaviour of the
        // library has changed.
        const int toleranceRange = 1;

        // Create a ColorDialog
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));

        // Start with Qt::yellow as initial color.
        // If this RGB value is interpreted in the sRGB (LittleCMS build-in)
        // profile, it has a particular property. Because of the irregular
        // shape of the sRGB color space at this position, thinking in
        // LCH values, when reducing (!) the chroma step-by-step, we run
        // out-of-gamut, before going again in-gamut at even lower chroma
        // values.
        m_perceptualDialog->setCurrentColor(QColor(Qt::yellow));
        // The value is also converted to HLC 100°, 98%, 95 (rounded)
        // visible in the HLC spin box.
        QList<double> hlc = //
            m_perceptualDialog->d_pointer->m_ciehlcSpinBox->sectionValues();
        QVERIFY(hlc.at(0) >= 100 - toleranceRange); // assertion
        QVERIFY(hlc.at(0) <= 100 + toleranceRange); // assertion
        QVERIFY(hlc.at(1) >= 98 - toleranceRange); // assertion
        QVERIFY(hlc.at(1) <= 98 + toleranceRange); // assertion
        QVERIFY(hlc.at(2) >= 95 - toleranceRange); // assertion
        QVERIFY(hlc.at(2) <= 95 + toleranceRange); // assertion
        // Now, the user clicks on the “Apply” button within the HLC spin box.
        // We simulate this by simply calling the slot that is connected
        // to this action:
        m_perceptualDialog->d_pointer->readHlcNumericValues();
        // Now, during development there was a bug observed: The buggy
        // behaviour was that the chroma value was changed from 95 to 24.
        // The expected result was that the chroma value only changes
        // slightly because of rounding (or ideally not at all).
        hlc = m_perceptualDialog->d_pointer->m_ciehlcSpinBox->sectionValues();
        QVERIFY(hlc.at(2) >= 95 - toleranceRange);
        QVERIFY(hlc.at(2) <= 95 + toleranceRange);
    }

    void testBlackHSV()
    {
        // In the HSV color space, if V is 0 then the color is black.
        // Both, H and S are meaningless. When converting from other
        // color spaces, they get probably a default value. However,
        // when the user is editing the HSV spin box, we does not expect
        // that H or S change when switching from one section to another
        // or when the focus leaves. Make sure that H and S are preserved
        // during editing even if V becomes 0:

        // Create a ColorDialog
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));

        const QList<double> hsvTestData{201, 33, 0};
        m_perceptualDialog->d_pointer->m_hsvSpinBox->setSectionValues( //
            hsvTestData);
        QCOMPARE( //
            m_perceptualDialog->d_pointer->m_hsvSpinBox->sectionValues(), //
            hsvTestData);
        m_perceptualDialog->d_pointer->readHsvNumericValues();
        QCOMPARE( //
            m_perceptualDialog->d_pointer->m_hsvSpinBox->sectionValues(), //
            hsvTestData);
    }

    void testRoundingMultipleError()
    {
        // This is a test for a bug discovered during development.

        // Create a ColorDialog:
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));

        // The user puts into the HLC spin box the value 100° 98% 94:
        m_perceptualDialog->d_pointer->m_ciehlcSpinBox->setSectionValues( //
            QList<double>{100, 98, 94});
        // This is an out-of-gamut color which is not corrected until
        // the focus will leave the widget or the Return key is pressed.
        // A nearby in-gamut color is around 100° 98% 24; this color
        // is used internally to perform the conversion to RGB and other
        // color spaces. (It is however still not visible in the HLC
        // spin box.)
        //
        // The RGB spin box becomes:
        const QList<double> expectedRgbValues{255, 251, 202};
        QCOMPARE( //
            m_perceptualDialog->d_pointer->m_rgbSpinBox->sectionValues(), //
            expectedRgbValues);
        // Now, the user finishes the editing process (the focus leaves
        // the widget or the Return key is pressed or the action button
        // is clicked):
        m_perceptualDialog->d_pointer->updateHlcButBlockSignals();
        // The buggy result during development phase was an RGB value
        // of 252 254 4. Why?
        // - The internal value was around 100° 97% 94, but not exactly.
        // - Now, the exact (!) value of 100° 97% 94 is applied, and this
        //   one, converted to RGB, triggers a different rounding.
        // The expected result is however still the very same RGB value
        // as above:
        QCOMPARE( //
            m_perceptualDialog->d_pointer->m_rgbSpinBox->sectionValues(), //
            expectedRgbValues);
    }

    void testRgbHexRounding()
    {
        // This is a test for a bug discovered during development.
        // QColor can produce a QString that contains a hexadecimal
        // (integer) representation of the color, just as used in
        // HTML. Example: #0000FF is blue. When rounding to
        // integers, apparently it does not use round(), but floor().
        // That is not useful and not inconsistent with the rest of our
        // dialog. We want correct rounding!

        // Create a ColorDialog:
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));

        // Set a color that triggers the rounding error:
        LchDouble testColor;
        testColor.h = 100;
        testColor.l = 97;
        testColor.c = 94;
        m_perceptualDialog->d_pointer->setCurrentOpaqueColor(
            // Color:
            MultiColor::fromCielch( //
                m_perceptualDialog->d_pointer->m_rgbColorSpace, //
                testColor),
            // Widget to ignore:
            nullptr);

        // Get the actual result
        QColor actualHex;
        actualHex.setNamedColor( //
            m_perceptualDialog->d_pointer->m_rgbLineEdit->text());

        // Get the expected result (We assume our own RGB spin box rounds
        // correctly.)
        const QList<double> rgbList =
            // The the values from the MultiSpinBox:
            m_perceptualDialog->d_pointer->m_rgbSpinBox->sectionValues();
        QColor expectedHex = QColor::fromRgb(
            // The MultiSpinBox might have decimal places, so we round
            // here again.
            qRound(rgbList.at(0)),
            qRound(rgbList.at(1)),
            qRound(rgbList.at(2)));

        // Compare
        QCOMPARE(actualHex, expectedHex);
    }

    void testBugMaximumLightness()
    {
        QScopedPointer<QTemporaryFile> wideGamutProfile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (wideGamutProfile.isNull()) {
            throw 0;
        }
        // This test looks for a bug that was seen during development
        // phase. When using WideGamutRGB and raising the lightness
        // slider up to 100%: Bug behaviour: the color switches
        // to 0% lightness. Expected behaviour: the color has almost
        // 100% lightness.
        auto myColorSpace = RgbColorSpace::createFromFile( //
            wideGamutProfile->fileName());
        QCOMPARE(myColorSpace.isNull(), false); // assertion
        m_perceptualDialog.reset( //
            new ColorDialog(myColorSpace));
        QTest::keyClick( //
            m_perceptualDialog->d_pointer->m_lchLightnessSelector, //
            Qt::Key_End);
        QVERIFY( //
            m_perceptualDialog->d_pointer->m_currentOpaqueColor.cielch.l > 95);
    }

    void testSnippet02()
    {
        snippet02();
    }

    void testSnippet03()
    {
        snippet03();
    }

    void testSnippet05()
    {
        TestColorDialogSnippetClass mySnippets;
        mySnippets.testSnippet05();
    }

    void benchmarkCreateAndShowPerceptualDialog()
    {
        m_perceptualDialog.reset(nullptr);
        QBENCHMARK {
            m_perceptualDialog.reset( //
                new ColorDialog(m_srgbBuildinColorSpace));
            m_perceptualDialog->show();
            m_perceptualDialog->repaint();
            m_perceptualDialog.reset(nullptr);
        }
    }

    void benchmarkCreateAndShowMaximizedPerceptualDialog()
    {
        m_perceptualDialog.reset(nullptr);
        QBENCHMARK {
            m_perceptualDialog.reset( //
                new ColorDialog(m_srgbBuildinColorSpace));
            m_perceptualDialog->showMaximized();
            m_perceptualDialog->repaint();
            m_perceptualDialog.reset(nullptr);
        }
    }

    void benchmarkCreateAndShowQColorDialog()
    {
        m_qDialog.reset(nullptr);
        QBENCHMARK {
            m_qDialog.reset(new QColorDialog);
            m_qDialog->show();
            m_qDialog->repaint();
            m_perceptualDialog.reset(nullptr);
        }
    }

    void benchmarkChangeColorPerceptualHueBased()
    {
        setTranslation(QCoreApplication::instance(), //
                       QLocale(QLocale::English).uiLanguages());
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->show();

        QTabWidget *theTabWidget = //
            m_perceptualDialog->findChild<QTabWidget *>();
        QVERIFY2(theTabWidget != nullptr, //
                 "Assert that theTabWidget has actually been found.");
        constexpr int myIndex = 1;
        // Assert that we got the correct tab widget:
        QCOMPARE(theTabWidget->tabText(myIndex), QStringLiteral("&Hue-based"));
        theTabWidget->setCurrentIndex(myIndex);

        QBENCHMARK {
            m_perceptualDialog->setCurrentColor(Qt::green);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::blue);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::yellow);
            m_perceptualDialog->repaint();
        }
    }

    void benchmarkChangeColorPerceptualLightnessBased()
    {
        setTranslation(QCoreApplication::instance(), //
                       QLocale(QLocale::English).uiLanguages());
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));
        m_perceptualDialog->show();

        QTabWidget *theTabWidget = //
            m_perceptualDialog->findChild<QTabWidget *>();
        QVERIFY2(theTabWidget != nullptr, //
                 "Assert that theTabWidget has actually been found.");
        constexpr int myIndex = 2;
        // Assert that we got the correct tab widget:
        QCOMPARE(theTabWidget->tabText(myIndex), //
                 QStringLiteral("&Lightness-based"));
        theTabWidget->setCurrentIndex(myIndex);

        QBENCHMARK {
            m_perceptualDialog->setCurrentColor(Qt::green);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::blue);
            m_perceptualDialog->repaint();
            m_perceptualDialog->setCurrentColor(Qt::yellow);
            m_perceptualDialog->repaint();
        }
    }

    void benchmarkChangeColorQColorDialog()
    {
        m_qDialog.reset(new QColorDialog);
        m_qDialog->show();
        QBENCHMARK {
            m_qDialog->setCurrentColor(Qt::green);
            m_qDialog->repaint();
            m_qDialog->setCurrentColor(Qt::blue);
            m_qDialog->repaint();
            m_qDialog->setCurrentColor(Qt::yellow);
            m_qDialog->repaint();
        }
    }

    // The last unit tests are those who need to change the locale.
    // To avoid side-effects, these unit tests are the last ones.

    void testChangeEventRetranslate()
    {
        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::English).uiLanguages());
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));

        // We test various translations, because we do not know the locale
        // of the system on which this unit test will run. As there might
        // be some coincidence between QLocale::system() and QLocale(),
        // we want to be sure and therefore test three translations.

        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::Dutch).uiLanguages());
        {
            QEvent temp(QEvent::LanguageChange);
            QCoreApplication::sendEvent(m_perceptualDialog.data(), &temp);
        }
        QCOMPARE(m_perceptualDialog->d_pointer->m_tabWidget->tabText(0), //
                 QStringLiteral("&Basiskleuren"));

        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::Catalan).uiLanguages());
        {
            QEvent temp(QEvent::LanguageChange);
            QCoreApplication::sendEvent(m_perceptualDialog.data(), &temp);
        }
        QCOMPARE(m_perceptualDialog->d_pointer->m_tabWidget->tabText(0), //
                 QStringLiteral("Colors &bàsics"));

        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::Spanish).uiLanguages());
        {
            QEvent temp(QEvent::LanguageChange);
            QCoreApplication::sendEvent(m_perceptualDialog.data(), &temp);
        }
        QCOMPARE(m_perceptualDialog->d_pointer->m_tabWidget->tabText(0), //
                 QStringLiteral("Colores &básicos"));
    }

    void testChangeEventRetranslateButtons()
    {
        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::English).uiLanguages());
        m_perceptualDialog.reset( //
            new ColorDialog(m_srgbBuildinColorSpace));

        // There is a particular reason for testing the translation of
        // the dialog buttons. We are using standard-buttons generated
        // by QDialogButtonBox. These standard-buttons apparently are
        // re-translated by QDialogButtonBox, which hooks into LanuageChange
        // events of its parent widgets. (LanuageChange events originally
        // only go to the top-level widgets.) This is a problem, because
        // it might change the text of the buttons from our own translation
        // to the Qt translation. Furthermore, QDialogButtonBox uses
        // QLocale::system() while we use QLocale(), which means we could
        // end up with two different languages after calling retranslate()
        // if the implementation would not prevent this problem. Therefore,
        // here we test if the implementation actually prevents this problem.

        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::Dutch).uiLanguages());
        {
            QEvent temp(QEvent::LanguageChange);
            QCoreApplication::sendEvent(m_perceptualDialog.data(), &temp);
        }
        QCOMPARE(m_perceptualDialog->d_pointer->m_buttonCancel->text(), //
                 QStringLiteral("&Annuleren"));

        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::Catalan).uiLanguages());
        {
            QEvent temp(QEvent::LanguageChange);
            QCoreApplication::sendEvent(m_perceptualDialog.data(), &temp);
        }
        QCOMPARE(m_perceptualDialog->d_pointer->m_buttonCancel->text(), //
                 QStringLiteral("&Cancel·la"));

        initializeTranslation(QCoreApplication::instance(), //
                              QLocale(QLocale::Spanish).uiLanguages());
        {
            QEvent temp(QEvent::LanguageChange);
            QCoreApplication::sendEvent(m_perceptualDialog.data(), &temp);
        }
        QCOMPARE(m_perceptualDialog->d_pointer->m_buttonCancel->text(), //
                 QStringLiteral("&Cancelar"));
    }

private:
    void unused()
    {
        // These will not be called in the unit tests because getColor()
        // does not return without user interaction!
        // They are noted here to avoid a warning about “unused function”.
        snippet01();
        snippet04();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorDialog)
// The following “include” is necessary because we do not use a header file:
#include "testcolordialog.moc"
