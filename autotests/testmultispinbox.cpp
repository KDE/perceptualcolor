// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "multispinbox.h"
// Second, the private implementation.
#include "multispinbox_p.h" // IWYU pragma: keep

#include "constpropagatinguniquepointer.h"
#include "multispinboxsection.h"
#include <qabstractspinbox.h>
#include <qaction.h>
#include <qapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlocale.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qscopedpointer.h>
#include <qsignalspy.h>
#include <qsize.h>
#include <qspinbox.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>
#include <qtestkeyboard.h>
#include <qtestsupport_widgets.h>
#include <qvalidator.h>
#include <qvariant.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

// From Qt documentation:
//     “Note: This function is not declared in any of Qt's header files. To
//      use it in your application, declare the function prototype before
//      calling it.”
void qt_set_sequence_auto_mnemonic(bool b);

static void snippet02()
{
    //! [MultiSpinBox Basic example]
    PerceptualColor::MultiSpinBox *myHsvSpinBox = //
        new PerceptualColor::MultiSpinBox();
    PerceptualColor::MultiSpinBoxSection myConfiguration;
    QList<PerceptualColor::MultiSpinBoxSection> hsvConfigurations;

    myConfiguration.setDecimals(1);

    myConfiguration.setMinimum(0);
    myConfiguration.setWrapping(true);
    myConfiguration.setMaximum(360);
    myConfiguration.setFormatString(QStringLiteral(u"%1° "));
    hsvConfigurations.append(myConfiguration);

    myConfiguration.setMinimum(0);
    myConfiguration.setMaximum(255);
    myConfiguration.setWrapping(false);
    myConfiguration.setFormatString(QStringLiteral(u" %1 "));
    hsvConfigurations.append(myConfiguration);

    myConfiguration.setFormatString(QStringLiteral("%1"));
    hsvConfigurations.append(myConfiguration);

    myHsvSpinBox->setSectionConfigurations(hsvConfigurations);

    myHsvSpinBox->setSectionValues(QList<double>{310, 200, 100});
    // Initial content is:  310,0°  200,0  100,0
    //! [MultiSpinBox Basic example]
    delete myHsvSpinBox;
}

class testSnippet02 : public QObject
{
    Q_OBJECT
    //! [MultiSpinBox Full-featured MultiSpinBoxSection]
    // API extension for MultiSpinBoxSection
    // for feature parity with QDoubleSpinBox

public:
    // range convenance function:
    void setRange(double newMinimum, double newMaximum); // convenance

    // specialValueText property (Note that QDateTimeEdit, different from
    // QDoubleSpinBox, does not provide this.)
    void setSpecialValueText(const QString &newSpecialValueText);
    QString specialValueText() const;

    // stepType property
    void setStepType(QAbstractSpinBox::StepType newStepType);
    QAbstractSpinBox::StepType stepType() const;
    //! [MultiSpinBox Full-featured MultiSpinBoxSection]

    //! [MultiSpinBox Full-featured MultiSpinBox]
    // API extension for MultiSpinBox
    // for feature parity with QDateTimeEdit

public: // (None of these functions is a Q_SLOTS in the mentioned Qt classes.)
    int currentSectionIndex() const;
    void setCurrentSectionIndex(int newIndex);
    void setSelectedSection(int newIndex);

    int sectionCount() const; // convenance for sectionConfigurations().size()

    QString sectionText(int index) const;
    //! [MultiSpinBox Full-featured MultiSpinBox]
};

namespace PerceptualColor
{
class TestMultiSpinBox : public QObject
{
    Q_OBJECT

public:
    explicit TestMultiSpinBox(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    QList<MultiSpinBoxSection> exampleConfigurations;
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
    }

private Q_SLOTS:
    void initTestCase()
    {
        // Called before the first test function is executed

        // Make sure to have mnemonics (like Qt::ALT+Qt::Key_X for "E&xit")
        // enabled, also on platforms that disable it by default.
        qt_set_sequence_auto_mnemonic(true);

        // Provide example configuration
        MultiSpinBoxSection mySection;
        mySection.setDecimals(0);
        mySection.setMinimum(0);
        mySection.setMaximum(360);
        mySection.setFormatString(QStringLiteral(u"%1°"));
        exampleConfigurations.append(mySection);
        mySection.setMaximum(100);
        mySection.setFormatString(QStringLiteral(u"  %1%"));
        exampleConfigurations.append(mySection);
        mySection.setMaximum(255);
        mySection.setFormatString(QStringLiteral(u"  %1"));
        exampleConfigurations.append(mySection);
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

    void testDefaultValues()
    {
        // The default values should be the same as for QDoubleSpinBox
        MultiSpinBox myMulti;
        QDoubleSpinBox myDoubleSpinBox;

        // Test default section values
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test default values of the configuration directly in the widget
        QCOMPARE(myMulti.sectionConfigurations().at(0).decimals(), //
                 myDoubleSpinBox.decimals());
        QCOMPARE(myMulti.sectionConfigurations().at(0).isWrapping(), //
                 myDoubleSpinBox.wrapping());
        QCOMPARE(myMulti.sectionConfigurations().at(0).maximum(), //
                 myDoubleSpinBox.maximum());
        QCOMPARE(myMulti.sectionConfigurations().at(0).minimum(), //
                 myDoubleSpinBox.minimum());
        QCOMPARE(myMulti.sectionConfigurations().at(0).prefix(), //
                 myDoubleSpinBox.prefix());
        QCOMPARE(myMulti.sectionConfigurations().at(0).singleStep(), //
                 myDoubleSpinBox.singleStep());
        QCOMPARE(myMulti.sectionConfigurations().at(0).suffix(), //
                 myDoubleSpinBox.suffix());

        // Whitebox tests
        QCOMPARE(myMulti.sectionValues(), QList<double>{0});
        QCOMPARE(myMulti.d_pointer->m_sectionValues, QList<double>{0});
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 0);
    }

    void testConstructor()
    {
        // Test the the constructor does not crash
        PerceptualColor::MultiSpinBox myMulti;
        // Test basic constructor results
        QVERIFY2(myMulti.d_pointer->m_sectionConfigurations.length() > 0, //
                 "Make sure the default configuration has at least 1 section.");
    }

    void testText()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        widget->setSectionConfigurations(exampleConfigurations);
        // Assert that the setup is okay.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  0%  0"));
    }

    void testInteraction()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        widget->setSectionConfigurations(exampleConfigurations);
        // Assert that the setup is okay.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  0%  0"));
        // Go to begin of the line edit
        for (int i = 0; i < 10; i++) {
            // NOTE Simply use 1 time Qt::Key_Home would be easier
            // than calling 10 times Qt::Key_Left, however Qt::Key_Home
            // does not work on MacOS.
            QTest::keyClick(widget.data(), Qt::Key_Left);
        }
        QCOMPARE(widget->lineEdit()->selectedText(), QString());
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  0%  0"));
        QCOMPARE(widget->lineEdit()->cursorPosition(), 0);
        // Select the first “0”:
        QTest::keyClick(widget.data(), Qt::Key_Right, Qt::ShiftModifier, 0);
        // The content shouldn’t have changed.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  0%  0"));
        // The selection should contain “0”.
        QCOMPARE(widget->lineEdit()->selectedText(), QStringLiteral(u"0"));
        // Write “45”
        QTest::keyClicks(widget.data(), QStringLiteral(u"45"));
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        // Select “45”
        QTest::keyClick(widget.data(), Qt::Key_Left, Qt::ShiftModifier, 0);
        QTest::keyClick(widget.data(), Qt::Key_Left, Qt::ShiftModifier, 0);
        // Copy to clipboard (The following line that copies to clipboard
        // is surprisingly extremly slow.)
        QTest::keyClick(widget.data(), Qt::Key_C, Qt::ControlModifier, 0);
        // Go to the left
        QTest::keyClick(widget.data(), Qt::Key_Left);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        // Go to second section
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  0%  0"));
        // Select second section:
        QTest::keyClick(widget.data(), Qt::Key_Right, Qt::ShiftModifier, 0);
        QCOMPARE(widget->lineEdit()->selectedText(), QStringLiteral(u"0"));
        // Take “45” from clipboard
        QTest::keyClick(widget.data(), Qt::Key_V, Qt::ControlModifier, 0);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
        QTest::keyClick(widget.data(), Qt::Key_Right);
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"45°  45%  0"));
    }

    void testCurrentSectionIndex()
    {
        MultiSpinBox test;
        // Test default index
        QCOMPARE(test.d_pointer->m_currentIndex, 0);

        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Test if setting negative value is ignored
        QVERIFY_EXCEPTION_THROWN( //
            test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(-1), //
            int);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        QVERIFY_EXCEPTION_THROWN( //
            test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(-100), //
            int);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        // Test setting too high values is ignored
        QVERIFY_EXCEPTION_THROWN( //
            test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(100), //
            int);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);

        // Test if correct sections are stored correctly
        QList<MultiSpinBoxSection> mySectionList;
        mySectionList.append(MultiSpinBoxSection());
        mySectionList.append(MultiSpinBoxSection());
        mySectionList.append(MultiSpinBoxSection());
        test.setSectionConfigurations(mySectionList);
        test.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(2);
        QCOMPARE(test.d_pointer->m_currentIndex, 2);
    }

    void testSetConfiguration()
    {
        // Correct configurations should be applied as-is.
        QList<MultiSpinBoxSection> myConfigurations;
        myConfigurations.append(MultiSpinBoxSection());
        myConfigurations.append(MultiSpinBoxSection());
        myConfigurations.append(MultiSpinBoxSection());
        MultiSpinBox test;
        QCOMPARE(test.sectionConfigurations().size(), 1);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);
        test.setSectionConfigurations(myConfigurations);
        QCOMPARE(test.sectionConfigurations().size(), 3);
        QCOMPARE(test.d_pointer->m_currentIndex, 0);

        // Empty configurations shall be ignored
        test.setSectionConfigurations(QList<MultiSpinBoxSection>());
        QCOMPARE(test.sectionConfigurations().size(), 3);

        // Invalid values should be adapted
        myConfigurations.clear();
        MultiSpinBoxSection myInvalidSection;
        myInvalidSection.setMinimum(50);
        myInvalidSection.setMaximum(30);
        myConfigurations.append(myInvalidSection);
        test.setSectionConfigurations(myConfigurations);
        QList<double> myValues;
        myValues.clear();
        myValues.append(40);
        test.setSectionValues(myValues);
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() //
                <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "minimum <= maximum");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() //
                <= test.d_pointer->m_sectionValues.at(0),
            // comment
            "minimum <= value");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionValues.at(0) //
                <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "value <= maximum");

        // Invalid values should be adapted
        myConfigurations.clear();
        myInvalidSection.setMinimum(-50);
        myInvalidSection.setMaximum(-70);
        myConfigurations.append(myInvalidSection);
        myValues.clear();
        myValues.append(-60);
        test.setSectionConfigurations(myConfigurations);
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() //
                <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "minimum <= maximum");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionConfigurations.at(0).minimum() //
                <= test.d_pointer->m_sectionValues.at(0),
            // comment
            "minimum <= value");
        QVERIFY2(
            // condition
            test.d_pointer->m_sectionValues.at(0) //
                <= test.d_pointer->m_sectionConfigurations.at(0).maximum(),
            // comment
            "value <= maximum");
    }

    void testMinimalSizeHint()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
        myMulti.setSectionConfigurations(exampleConfigurations);
        QCOMPARE(myMulti.minimumSizeHint(), myMulti.sizeHint());
    }

    void testSizeHint()
    {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        QList<MultiSpinBoxSection> config;
        MultiSpinBoxSection section;
        section.setMinimum(1);
        section.setMaximum(9);
        section.setFormatString(QStringLiteral(u"abcdefghij%1abcdefghij"));
        config.append(section);
        myMulti.setSectionConfigurations(config);
        const int referenceWidth = myMulti.sizeHint().width();

        // Now test various configurations that should lead to bigger sizes…

        section.setMinimum(-1);
        section.setMaximum(9);
        section.setFormatString(QStringLiteral(u"abcdefghij%1abcdefghij"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.setMinimum(1);
        section.setMaximum(19);
        section.setFormatString(QStringLiteral(u"abcdefghij%1abcdefghij"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.setMinimum(-1);
        section.setMaximum(9);
        section.setFormatString(QStringLiteral(u"abcdefghijh%1abcdefghij"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);

        section.setMinimum(-1);
        section.setMaximum(9);
        section.setFormatString(QStringLiteral(u"abcdefghij%1abcdefghijh"));
        config.clear();
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QVERIFY(myMulti.sizeHint().width() > referenceWidth);
    }

    void testUpdatePrefixValueSuffixText()
    {
        PerceptualColor::MultiSpinBox myMulti;
        // Example configuration with long prefix and suffix to make
        // sure being bigger than the default minimal widget size.
        QList<MultiSpinBoxSection> myConfigurations;
        MultiSpinBoxSection myConfiguration;
        QList<double> myValues;

        myConfiguration.setDecimals(0);
        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setFormatString(QStringLiteral(u"abc%1def"));
        myConfigurations.append(myConfiguration);
        myValues.append(8);

        myConfiguration.setMinimum(10);
        myConfiguration.setMaximum(90);
        myConfiguration.setFormatString(QStringLiteral(u"ghi%1jkl"));
        myConfigurations.append(myConfiguration);
        myValues.append(80);

        myMulti.setSectionConfigurations(myConfigurations);
        myMulti.setSectionValues(myValues);
        myMulti.d_pointer->m_currentIndex = 1;
        myMulti.d_pointer->updatePrefixValueSuffixText();
        QCOMPARE(myMulti.d_pointer->m_textBeforeCurrentValue, //
                 QStringLiteral(u"abc8defghi"));
        QCOMPARE(myMulti.d_pointer->m_textOfCurrentPendingValue, //
                 QStringLiteral(u"80"));
        QCOMPARE(myMulti.d_pointer->m_textAfterCurrentValue, //
                 QStringLiteral(u"jkl"));
    }

    void testSetCurrentSectionIndexWithoutSelectingText()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBoxSection> myConfigurations;
        MultiSpinBoxSection myConfiguration;
        QList<double> myValues;

        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setFormatString(QStringLiteral(u"abc%1def"));
        myConfigurations.append(myConfiguration);
        myValues.append(8);

        myConfiguration.setMinimum(10);
        myConfiguration.setMaximum(90);
        myConfiguration.setFormatString(QStringLiteral(u"ghi%1jkl"));
        myConfigurations.append(myConfiguration);
        myValues.append(80);

        myMulti.setSectionConfigurations(myConfigurations);
        myMulti.setSectionValues(myValues);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 1);
        QVERIFY2(!myMulti.lineEdit()->hasSelectedText(), //
                 "No text should be selected.");
    }

    void testSetCurrentSectionIndex()
    {
        PerceptualColor::MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(exampleConfigurations);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(2);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 2);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 0);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 1);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(2);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 2);
        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 0);

        QList<MultiSpinBoxSection> myConfigurations;
        MultiSpinBoxSection myConfiguration;
        QList<double> myValues;

        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setFormatString(QStringLiteral(u"abc%1def"));
        myConfigurations.append(myConfiguration);
        myValues.append(8);

        myConfiguration.setMinimum(10);
        myConfiguration.setMaximum(90);
        myConfiguration.setFormatString(QStringLiteral(u"ghi%1jkl"));
        myConfigurations.append(myConfiguration);
        myValues.append(80);

        myMulti.setSectionConfigurations(myConfigurations);
        myMulti.setSectionValues(myValues);

        myMulti.d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QCOMPARE(myMulti.d_pointer->m_currentIndex, 1);
        QVERIFY2(!myMulti.lineEdit()->hasSelectedText(),
                 "No text should be selected because invisible widgets "
                 "have no focus.");
    }

    void testStepEnabledSimple()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBoxSection> myConfigurations;
        MultiSpinBoxSection myConfiguration;
        QList<double> myValues;

        myConfigurations.clear();
        myConfiguration.setMinimum(1);
        myConfiguration.setMaximum(9);
        myConfiguration.setFormatString(QStringLiteral(u"abc%1def"));
        myConfigurations.append(myConfiguration);
        myMulti.setSectionConfigurations(myConfigurations);
        myValues.clear();
        myValues.append(8);
        myMulti.setSectionValues(myValues);
        QAbstractSpinBox::StepEnabled flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), //
                 "Step up should be enabled");
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepDownEnabled), //
                 "Step down should be enabled");

        myValues.clear();
        myValues.append(9);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepUpEnabled), //
                 "Step up should be disabled");
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepDownEnabled), //
                 "Step down should be enabled");

        myValues.clear();
        myValues.append(10);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepUpEnabled), //
                 "Step up should be disabled");
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepDownEnabled), //
                 "Step down should be enabled");

        myValues.clear();
        myValues.append(1);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), //
                 "Step up should be enabled");
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepDownEnabled), //
                 "Step down should be disabled");

        myValues.clear();
        myValues.append(0);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), //
                 "Step up should be enabled");
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepDownEnabled), //
                 "Step down should be disabled");

        myValues.clear();
        myValues.append(-1);
        myMulti.setSectionValues(myValues);
        flags = myMulti.stepEnabled();
        QVERIFY2(flags.testFlag(QAbstractSpinBox::StepUpEnabled), //
                 "Step up should be enabled");
        QVERIFY2(!flags.testFlag(QAbstractSpinBox::StepDownEnabled), //
                 "Step down should be disabled");
    }

    void testStepEnabledAndSectionIndex_data()
    {
        QTest::addColumn<int>("cursorPosition");
        QTest::addColumn<int>("sectionIndex");
        QTest::addColumn<int>("minimum");
        QTest::addColumn<int>("value");
        QTest::addColumn<int>("maximum");
        QTest::addColumn<bool>("StepUpEnabled");
        QTest::addColumn<bool>("StepDownEnabled");

        QTest::newRow("0") << 0 << 0 << 0 << 0 << 360 << true << false;
        QTest::newRow("1") << 1 << 0 << 0 << 0 << 360 << true << false;
        QTest::newRow("2") << 2 << 0 << 0 << 0 << 360 << true << false;
        QTest::newRow("4") << 4 << 1 << 0 << 5 << 100 << true << true;
        QTest::newRow("5") << 5 << 1 << 0 << 5 << 100 << true << true;
        QTest::newRow("6") << 6 << 1 << 0 << 5 << 100 << true << true;
        QTest::newRow("8") << 8 << 2 << 0 << 0 << 255 << true << false;
        QTest::newRow("9") << 9 << 2 << 0 << 0 << 255 << true << false;
    }

    void testStepEnabledAndSectionIndex()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSection> specialConfigurations = //
            exampleConfigurations;
        QList<double> myValues;
        while (myValues.size() < specialConfigurations.size()) {
            myValues.append(0);
        }
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfigurations);
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
            sampleSectionNumber);

        // Assertions: Assert that the setup is okay.
        // Assert statements seem to be not always reliably within QTest.
        // Therefore we do some assertions here with QCOMPARE.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  5%  0"));
        QCOMPARE(widget->sectionValues().at(sampleSectionNumber), sampleValue);
        QAbstractSpinBox::StepEnabled flags;

        // Actual testing
        QFETCH(int, cursorPosition);
        QFETCH(int, sectionIndex);
        QFETCH(int, minimum);
        QFETCH(int, value);
        QFETCH(int, maximum);
        QFETCH(bool, StepUpEnabled);
        QFETCH(bool, StepDownEnabled);
        widget->lineEdit()->setCursorPosition(cursorPosition);
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        flags = widget->stepEnabled();
        const auto &d = widget->d_pointer;
        QCOMPARE(d->m_currentIndex, sectionIndex);
        QCOMPARE(d->m_sectionConfigurations.at(d->m_currentIndex).minimum(), //
                 minimum);
        QCOMPARE(d->m_sectionValues.at(d->m_currentIndex), //
                 value);
        QCOMPARE(d->m_sectionConfigurations.at(d->m_currentIndex).maximum(), //
                 maximum);
        QCOMPARE(flags.testFlag(QAbstractSpinBox::StepUpEnabled), //
                 StepUpEnabled);
        QCOMPARE(flags.testFlag(QAbstractSpinBox::StepDownEnabled), //
                 StepDownEnabled);
    }

    void testConfiguration()
    {
        PerceptualColor::MultiSpinBox myMulti;
        QList<MultiSpinBoxSection> config;
        MultiSpinBoxSection section;
        section.setMinimum(1);
        section.setMaximum(9);
        section.setFormatString(QStringLiteral(u"abc%1def"));
        config.append(section);
        myMulti.setSectionConfigurations(config);
        QCOMPARE(myMulti.sectionConfigurations().size(), 1);
        QCOMPARE(myMulti.sectionConfigurations().at(0).minimum(), 1);
        QCOMPARE(myMulti.sectionConfigurations().at(0).maximum(), 9);
        QCOMPARE(myMulti.sectionConfigurations().at(0).prefix(), //
                 QStringLiteral(u"abc"));
        QCOMPARE(myMulti.sectionConfigurations().at(0).suffix(), //
                 QStringLiteral(u"def"));
    }

    void testFocusIntegrationForwardTab()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = //
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), //
                                    parentWidget.data());
        label2->setBuddy(widget2);
        widget1->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

        // It is necessary to show the widget and make it active
        // to make focus and widget events working within unit tests.
        parentWidget->show();
        parentWidget->activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(parentWidget.data()));

        // Assert that the setup is okay.
        QVERIFY(widget1->hasFocus());
        QVERIFY(!widget2->hasFocus());
        QVERIFY(!widget3->hasFocus());
        QVERIFY(QApplication::focusWidget() == widget1);
        QVERIFY(widget2->d_pointer->m_sectionConfigurations.size() == 3);

        // Start actual testing

        // Apparently it isn’t possible to call simply the key click
        // on the parent widget. This code fails sometimes:
        // QTest::keyClick(parentWidget, Qt::Key::Key_Tab);
        // Therefore, we call QTest::keyClick() on
        // QApplication::focusWidget()

        // Move focus from widget1 to widget2/section0
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget2/section2
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 2);
        // Move focus from widget2/section2 to widget3
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);

        // Cleanup
        delete widget1;
        delete widget2;
        delete widget3;
        delete label2;
    }

    void testIsGroupSeparatorShown()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(3);
        myConfig.setMinimum(5);
        myConfig.setMaximum(10000);
        myConfig.setGroupSeparatorShown(true);
        MultiSpinBox myMulti;
        myMulti.setLocale(QLocale::German);
        myMulti.setSectionConfigurations({myConfig});
        myMulti.setSectionValues({6789.123});
        QCOMPARE(myMulti.text(), QStringLiteral("6.789,123"));
        myConfig.setGroupSeparatorShown(false);
        myMulti.setSectionConfigurations({myConfig});
        QCOMPARE(myMulti.text(), QStringLiteral("6789,123"));
        // All locales except QLocale::C enabled group separators by
        // default. Therefore, testing QLocale::C as a special case.
        myMulti.setLocale(QLocale::C);
        QCOMPARE(myMulti.text(), QStringLiteral("6789.123"));
        myConfig.setGroupSeparatorShown(true);
        myMulti.setSectionConfigurations({myConfig});
        QCOMPARE(myMulti.text(), QStringLiteral("6,789.123"));
    }

    void testInputWhileGroupSeparatorShown()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(3);
        myConfig.setMinimum(0);
        myConfig.setMaximum(10000);
        myConfig.setGroupSeparatorShown(true);
        MultiSpinBox myMulti;
        myMulti.setLocale(QLocale::German);
        myMulti.setSectionConfigurations({myConfig});
        myMulti.lineEdit()->setText(QStringLiteral("2"));
        QCOMPARE(myMulti.sectionValues().at(0), 2);
        // Test correctly placed group separator
        myMulti.lineEdit()->setText(QStringLiteral("2.345,6"));
        QCOMPARE(myMulti.sectionValues().at(0), 2345.6);
        // Test wrongly placed group separator: should be accepted nevertheless
        myMulti.lineEdit()->setText(QStringLiteral("73.45,6"));
        QCOMPARE(myMulti.sectionValues().at(0), 7345.6);
    }

    void testInputWhileNotGroupSeparatorShown()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(3);
        myConfig.setMinimum(0);
        myConfig.setMaximum(10000);
        myConfig.setGroupSeparatorShown(false);
        MultiSpinBox myMulti;
        myMulti.setLocale(QLocale::German);
        myMulti.setSectionConfigurations({myConfig});
        myMulti.lineEdit()->setText(QStringLiteral("2"));
        QCOMPARE(myMulti.sectionValues().at(0), 2);
        // Test correctly placed group separator
        myMulti.lineEdit()->setText(QStringLiteral("2.345,6"));
        QCOMPARE(myMulti.sectionValues().at(0), 2345.6);
        // Test wrongly placed group separator: should be accepted nevertheless
        myMulti.lineEdit()->setText(QStringLiteral("73.45,6"));
        QCOMPARE(myMulti.sectionValues().at(0), 7345.6);
    }

    void testTextFromValue()
    {
        QCOMPARE( //
            MultiSpinBoxPrivate::textFromValue(6789.123, 4, true, QLocale::German),
            QStringLiteral("6.789,1230"));
        QCOMPARE( //
            MultiSpinBoxPrivate::textFromValue(6789.123, 4, false, QLocale::German),
            QStringLiteral("6789,1230"));
        QCOMPARE( //
            MultiSpinBoxPrivate::textFromValue(6789.123, 4, true, QLocale::C),
            QStringLiteral("6,789.1230"));
        QCOMPARE( //
            MultiSpinBoxPrivate::textFromValue(6789.123, 4, false, QLocale::C),
            QStringLiteral("6789.1230"));
    }

    void testCorrectToPreviousValue()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(3);
        myConfig.setMinimum(3);
        myConfig.setMaximum(6);
        myConfig.setGroupSeparatorShown(false);
        MultiSpinBox myMulti;
        myMulti.setLocale(QLocale::German);
        myMulti.setCorrectionMode(QAbstractSpinBox::CorrectToPreviousValue);
        myMulti.setSectionConfigurations({myConfig});
        myMulti.lineEdit()->setText(QStringLiteral("3"));
        QCOMPARE(myMulti.sectionValues().at(0), 3);
        myMulti.lineEdit()->setText(QStringLiteral("6"));
        QCOMPARE(myMulti.sectionValues().at(0), 6);
        myMulti.lineEdit()->setText(QStringLiteral("4"));
        QCOMPARE(myMulti.sectionValues().at(0), 4);
        // Value too high
        myMulti.lineEdit()->setText(QStringLiteral("7"));
        QCOMPARE(myMulti.sectionValues().at(0), 4);
        // Value too low
        myMulti.lineEdit()->setText(QStringLiteral("2"));
        QCOMPARE(myMulti.sectionValues().at(0), 4);
        // Value invalid
        myMulti.lineEdit()->setText(QStringLiteral("xyz"));
        QCOMPARE(myMulti.sectionValues().at(0), 4);
    }

    void testCorrectToNearestValue()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(3);
        myConfig.setMinimum(3);
        myConfig.setMaximum(6);
        myConfig.setGroupSeparatorShown(false);
        MultiSpinBox myMulti;
        myMulti.setLocale(QLocale::German);
        myMulti.setCorrectionMode(QAbstractSpinBox::CorrectToNearestValue);
        myMulti.setSectionConfigurations({myConfig});

        // Value is valid
        myMulti.lineEdit()->setText(QStringLiteral("3"));
        QCOMPARE(myMulti.sectionValues().at(0), 3);
        myMulti.lineEdit()->setText(QStringLiteral("6"));
        QCOMPARE(myMulti.sectionValues().at(0), 6);
        myMulti.lineEdit()->setText(QStringLiteral("4"));
        QCOMPARE(myMulti.sectionValues().at(0), 4);

        // Value too high
        myMulti.lineEdit()->setText(QStringLiteral("7"));
        QCOMPARE(myMulti.sectionValues().at(0), 6);

        // Value too low
        myMulti.lineEdit()->setText(QStringLiteral("2"));
        QCOMPARE(myMulti.sectionValues().at(0), 3);

        // Value invalid
        myMulti.lineEdit()->setText(QStringLiteral("4"));
        QCOMPARE(myMulti.sectionValues().at(0), 4);
        myMulti.lineEdit()->setText(QStringLiteral("xyz"));
        QCOMPARE(myMulti.sectionValues().at(0), 4);
    }

    void testFocusIntegrationBackwardTab()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = //
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), //
                                    parentWidget.data());
        label2->setBuddy(widget2);
        widget3->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        parentWidget->activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(parentWidget.data()));
        // Assert that the setup is okay.
        QVERIFY(!widget1->hasFocus());
        QVERIFY(!widget2->hasFocus());
        QVERIFY(widget3->hasFocus());
        QVERIFY(QApplication::focusWidget() == widget3);
        QVERIFY(widget2->d_pointer->m_sectionConfigurations.size() == 3);

        // Start actual testing
        // Move focus from widget3 to widget2/section2
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_Tab, //
                        Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 2);
        // Move focus from widget2/section2 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_Tab, //
                        Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget2/section0
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_Tab, //
                        Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget1
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_Tab, //
                        Qt::KeyboardModifier::ShiftModifier);
        QCOMPARE(QApplication::focusWidget(), widget1);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);

        // Cleanup
        delete widget1;
        delete widget2;
        delete widget3;
        delete label2;
    }

    void testFocusIntegrationIntegrationWithMnemonicBuddy()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = //
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        widget2->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), //
                                    parentWidget.data());
        label2->setBuddy(widget2);
        QLabel *label3 = new QLabel(QStringLiteral(u"&Other widget"), //
                                    parentWidget.data());
        label3->setBuddy(widget3);
        widget3->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        parentWidget->activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(parentWidget.data()));
        // Assert that the setup is okay.
        QVERIFY(!widget1->hasFocus());
        QVERIFY(!widget2->hasFocus());
        QVERIFY(widget3->hasFocus());
        QVERIFY(QApplication::focusWidget() == widget3);
        QVERIFY(widget2->d_pointer->m_sectionConfigurations.size() == 3);
        QVERIFY(widget2->d_pointer->m_currentIndex == 1);

        // Start actual testing
        // Move focus from widget3 to widget2/section0
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_T, //
                        Qt::KeyboardModifier::AltModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);
        // Move focus from widget2/section1 to widget3
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_O, //
                        Qt::KeyboardModifier::AltModifier);
        QCOMPARE(QApplication::focusWidget(), widget3);
        // Move focus from widget3 to widget2/section0
        // This has to move to section0 (even if before this event, the last
        // selected section of widget2 was NOT section0.
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_T, //
                        Qt::KeyboardModifier::AltModifier);
        QCOMPARE(QApplication::focusWidget(), widget2);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);

        // Cleanup
        delete widget1;
        delete widget2;
        delete widget3;
        delete label2;
        delete label3;
    }

    void testFocusIntegrationFocusPolicy()
    {
        // Integration test for:
        // → MultiSpinBox::focusNextPrevChild()
        // → MultiSpinBox::focusInEvent()
        // → MultiSpinBox::focusOutEvent()
        QScopedPointer<QWidget> parentWidget(new QWidget());
        QSpinBox *widget1 = new QSpinBox(parentWidget.data());
        widget1->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        PerceptualColor::MultiSpinBox *widget2 = //
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), //
                                    parentWidget.data());
        label2->setBuddy(widget2);
        QLabel *label3 = new QLabel(QStringLiteral(u"&Other widget"), //
                                    parentWidget.data());
        label3->setBuddy(widget3);
        widget3->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        // The following statement make focus and widget events working.
        parentWidget->activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(parentWidget.data()));
        // Assert that the setup is okay.
        QVERIFY(!widget1->hasFocus());
        QVERIFY(!widget2->hasFocus());
        QVERIFY(widget3->hasFocus());
        QVERIFY(QApplication::focusWidget() == widget3);
        QVERIFY(widget2->d_pointer->m_sectionConfigurations.size() == 3);

        // Start actual testing
        // Make sure that MultiSpinBox does not react on incoming tab focus
        // events if the current focus policy does not allow tab focus.
        widget2->setFocusPolicy(Qt::FocusPolicy::ClickFocus);
        widget1->setFocus();
        QVERIFY(QApplication::focusWidget() == widget1);
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);
        widget2->setFocusPolicy(Qt::FocusPolicy::NoFocus);
        widget1->setFocus();
        QVERIFY(QApplication::focusWidget() == widget1);
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(QApplication::focusWidget(), widget3);

        // Cleanup
        delete widget1;
        delete widget2;
        delete widget3;
        delete label2;
        delete label3;
    }

    void testStepBy()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        widget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget->setSectionConfigurations(exampleConfigurations);
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(0);
        widget->stepBy(13);
        QCOMPARE(widget->sectionValues().at(0), 13);
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(1);
        widget->stepBy(130);
        QCOMPARE(widget->sectionValues().at(1), 100);
        widget->d_pointer->setCurrentIndexWithoutUpdatingText(2);
        widget->stepBy(-260);
        QCOMPARE(widget->sectionValues().at(2), 0);
    }

    void testStepUpDown()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        widget->setSectionConfigurations(exampleConfigurations);
        QCOMPARE(widget->sectionValues().at(0), 0);
        widget->stepUp();
        QCOMPARE(widget->sectionValues().at(0), 1);
        widget->stepUp();
        QCOMPARE(widget->sectionValues().at(0), 2);
        widget->stepDown();
        QCOMPARE(widget->sectionValues().at(0), 1);
        widget->stepDown();
        QCOMPARE(widget->sectionValues().at(0), 0);
        widget->stepDown();
        QCOMPARE(widget->sectionValues().at(0), 0);
    }

    void testUpdateValueFromText1()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        widget->setSectionConfigurations(exampleConfigurations);
        const quint8 sampleSectionNumber = 1;
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
            sampleSectionNumber);
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  0%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        widget->d_pointer->updateCurrentValueFromText( //
            QStringLiteral(u"0°  9%  0"));
        QCOMPARE(widget->sectionValues().at(sampleSectionNumber), 9);
    }

    void testUpdateValueFromText2()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSection> specialConfiguration = exampleConfigurations;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfiguration);
        QList<double> myValues;
        while (myValues.size() < specialConfiguration.size()) {
            myValues.append(0);
        }
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
            sampleSectionNumber);
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget->sectionValues().at(sampleSectionNumber) != sampleValue) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Execute the tested function (with an invalid argument)
        widget->d_pointer->updateCurrentValueFromText( //
            QStringLiteral(u"abcdef"));
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
        // The original value should not have changed.
        QCOMPARE(widget->sectionValues().at(sampleSectionNumber), sampleValue);
    }

    void testUpdateSectionFromCursorPosition()
    {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSection> specialConfiguration = exampleConfigurations;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfiguration);
        QList<double> myValues;
        while (myValues.size() < specialConfiguration.size()) {
            myValues.append(0);
        }
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        widget->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
            sampleSectionNumber);
        // Assert that the setup is okay.
        if (widget->lineEdit()->text() != QStringLiteral(u"0°  5%  0")) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }
        if (widget->sectionValues().at(sampleSectionNumber) != sampleValue) {
            // Throw an exception instead of using an assert statement.
            // Assert statements seem to be not always reliably within QTest.
            throw 0;
        }

        // Do testing
        widget->lineEdit()->setCursorPosition(0);
        QCOMPARE(widget->d_pointer->m_currentIndex, 0);
        widget->lineEdit()->setCursorPosition(1);
        QCOMPARE(widget->d_pointer->m_currentIndex, 0);
        widget->lineEdit()->setCursorPosition(2);
        QCOMPARE(widget->d_pointer->m_currentIndex, 0);
        widget->lineEdit()->setCursorPosition(4);
        QCOMPARE(widget->d_pointer->m_currentIndex, 1);
        widget->lineEdit()->setCursorPosition(5);
        QCOMPARE(widget->d_pointer->m_currentIndex, 1);
        widget->lineEdit()->setCursorPosition(6);
        QCOMPARE(widget->d_pointer->m_currentIndex, 1);
        widget->lineEdit()->setCursorPosition(8);
        QCOMPARE(widget->d_pointer->m_currentIndex, 2);
        widget->lineEdit()->setCursorPosition(9);
        QCOMPARE(widget->d_pointer->m_currentIndex, 2);
    }

    void testInitialLineEditValue()
    {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSection> specialConfiguration = exampleConfigurations;
        const quint8 sampleSectionNumber = 1;
        const quint8 sampleValue = 5;
        widget->setSectionConfigurations(specialConfiguration);
        QList<double> myValues;
        while (myValues.size() < specialConfiguration.size()) {
            myValues.append(0);
        }
        myValues[sampleSectionNumber] = sampleValue;
        widget->setSectionValues(myValues);
        // Assert that the initial content of the line edit is okay
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"0°  5%  0"));
    }

    void testLocalizationAndInternationalization()
    {
        // Setup
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        QList<MultiSpinBoxSection> mySectionList;
        MultiSpinBoxSection mySection;
        mySection.setDecimals(1);
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySectionList.append(mySection);
        widget->setSectionConfigurations(mySectionList);
        QList<double> myValues;
        myValues.append(50);
        widget->setSectionValues(myValues);

        // Begin testing

        widget->setLocale(QLocale::English);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"50.0"));

        widget->setLocale(QLocale::German);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"50,0"));

        widget->setLocale(QLocale::Bengali);
        // Without calling update() or other functions, the new locale should
        // be applied on-the-fly.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"৫০.০"));
    }

    void testArrowKeys()
    {
        QScopedPointer<QWidget> parentWidget(new QWidget());
        PerceptualColor::MultiSpinBox *widget2 = //
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        widget2->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        widget2->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        // The following statement make focus and widget events working.
        parentWidget->activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(parentWidget.data()));
        // Assert that the setup is okay.
        QVERIFY(widget2->hasFocus());
        QVERIFY(QApplication::focusWidget() == widget2);
        QVERIFY(widget2->d_pointer->m_sectionConfigurations.size() == 3);
        QVERIFY(widget2->lineEdit()->text() == QStringLiteral(u"0°  0%  0"));

        // Start actual testing
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Up);
        QCOMPARE(widget2->sectionValues().at(1), 1);
        QCOMPARE(widget2->lineEdit()->text(), QStringLiteral(u"0°  1%  0"));

        // Cleanup
        delete widget2;
    }

    void testSectionConfigurationDebug()
    {
        // suppress warnings
        qInstallMessageHandler(voidMessageHandler);
        // Test if QDebug support does not make a crash.
        qDebug() << MultiSpinBoxSection();
        // do not suppress warning for generating invalid QColor anymore
        qInstallMessageHandler(nullptr);
    }

    void testAddActionButtonSizeHint()
    {
        // Adding an action button necessarily changes the size hint of the
        // widget.
        MultiSpinBox mySpinBox;
        int oldWidth = 0;
        QCOMPARE(mySpinBox.lineEdit()->actions().size(), 0);
        oldWidth = mySpinBox.sizeHint().width();
        mySpinBox.addActionButton( //
            new QAction(QStringLiteral(u"test"), &mySpinBox), //
            QLineEdit::ActionPosition::TrailingPosition);
        QCOMPARE(mySpinBox.lineEdit()->actions().size(), 1);
        QVERIFY2(mySpinBox.sizeHint().width() > oldWidth,
                 "Verify: After adding an action button, "
                 "the size hint has a bigger width than before.");
        oldWidth = mySpinBox.sizeHint().width();
        mySpinBox.addActionButton( //
            new QAction(QStringLiteral(u"test"), &mySpinBox), //
            QLineEdit::ActionPosition::TrailingPosition);
        QCOMPARE(mySpinBox.lineEdit()->actions().size(), 2);
        QVERIFY2(mySpinBox.sizeHint().width() > oldWidth,
                 "Verify: After adding an action button, "
                 "the size hint has a bigger width than before.");
    }

    void testActions()
    {
        // addAction() should make the action also visible in the action
        // list of our widget, and QWidget::removeAction on our widget
        // should also remove it from the QLineEdit child widget.
        MultiSpinBox mySpinBox;
        QCOMPARE(mySpinBox.actions(), {});
        QCOMPARE(mySpinBox.lineEdit()->actions(), {});
        QAction *action1 = new QAction();
        mySpinBox.addAction(action1); // QWidget::addAction()
        QAction *action2 = new QAction();
        mySpinBox.addActionButton(action2, QLineEdit::LeadingPosition);
        QAction *action3 = new QAction();
        mySpinBox.addAction(action3); // QWidget::addAction()
        const auto actionList123 = QList<QAction *>{action1, action2, action3};
        const auto actionList2 = QList<QAction *>{action2};
        const auto actionList13 = QList<QAction *>{action1, action3};
        QCOMPARE(mySpinBox.actions(), actionList123);
        QCOMPARE(mySpinBox.lineEdit()->actions(), actionList2);
        mySpinBox.removeAction(action2);
        QCOMPARE(mySpinBox.actions(), actionList13);
        QCOMPARE(mySpinBox.lineEdit()->actions(), {});

        // Cleanup
        delete action1;
        delete action2;
        delete action3;
    }

    void testFixSectionValue_data()
    {
        QTest::addColumn<double>("value");
        QTest::addColumn<double>("expectedOnIsWrappigFalse");
        QTest::addColumn<double>("expectedOnIsWrappigTrue");

        QTest::newRow(" -5") << -05. << 000. << 355.;
        QTest::newRow("  0") << 000. << 000. << 000.;
        QTest::newRow("  5") << 005. << 005. << 005.;
        QTest::newRow("355") << 355. << 355. << 355.;
        QTest::newRow("360") << 360. << 360. << 000.;
        QTest::newRow("365") << 365. << 360. << 005.;
        QTest::newRow("715") << 715. << 360. << 355.;
        QTest::newRow("720") << 720. << 360. << 000.;
        QTest::newRow("725") << 725. << 360. << 005.;
    }

    void testFixSectionValue()
    {
        MultiSpinBox mySpinBox;

        QFETCH(double, value);
        QFETCH(double, expectedOnIsWrappigFalse);
        QFETCH(double, expectedOnIsWrappigTrue);

        MultiSpinBoxSection myConfiguration;
        myConfiguration.setMinimum(0);
        myConfiguration.setMaximum(360);
        myConfiguration.setWrapping(false);
        QList<MultiSpinBoxSection> myConfigurations;
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        QList<double> myValues;
        myValues.append(value);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigFalse);

        myConfiguration.setWrapping(true);
        myConfigurations.clear();
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigTrue);
    }

    void testFixedSectionOther_data()
    {
        QTest::addColumn<double>("value");
        QTest::addColumn<double>("expectedOnIsWrappigFalse");
        QTest::addColumn<double>("expectedOnIsWrappigTrue");

        QTest::newRow("-25") << -25. << -20. << 335.;
        QTest::newRow("-20") << -20. << -20. << -20.;
        QTest::newRow("-15") << -15. << -15. << -15.;
        QTest::newRow("335") << 335. << 335. << 335.;
        QTest::newRow("340") << 340. << 340. << -20.;
        QTest::newRow("345") << 345. << 340. << -15.;
        QTest::newRow("695") << 695. << 340. << 335.;
        QTest::newRow("700") << 700. << 340. << -20.;
        QTest::newRow("705") << 705. << 340. << -15.;
    }

    void testFixedSectionOther()
    {
        MultiSpinBox mySpinBox;

        QFETCH(double, value);
        QFETCH(double, expectedOnIsWrappigFalse);
        QFETCH(double, expectedOnIsWrappigTrue);

        MultiSpinBoxSection myConfiguration;
        myConfiguration.setMinimum(-20);
        myConfiguration.setMaximum(340);
        myConfiguration.setWrapping(false);
        QList<MultiSpinBoxSection> myConfigurations;
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        QList<double> myValues;
        myValues.append(value);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigFalse);

        myConfiguration.setWrapping(true);
        myConfigurations.clear();
        myConfigurations.append(myConfiguration);
        mySpinBox.setSectionConfigurations(myConfigurations);
        mySpinBox.setSectionValues(myValues);
        QCOMPARE(mySpinBox.sectionValues().at(0), expectedOnIsWrappigTrue);
    }

    void testValuesSetterAndConfigurationsSetter()
    {
        // Both, sectionValues() and sectionConfigurations() have a size()
        // that has to be identical. The count of sectionConfigurations() is
        // mandatory. Make sure that different setters let the size()s
        // in a correct state. Our reference for default values is
        // QDoubleSpinBox.
        MultiSpinBox myMulti;
        QDoubleSpinBox myDoubleSpinBox;
        QList<MultiSpinBoxSection> myConfigurations;
        QList<double> myValues;

        // Section count should be 1 (by default):
        QCOMPARE(myMulti.sectionConfigurations().size(), 1);
        QCOMPARE(myMulti.sectionValues().size(), 1);
        // Control that sections has default value:
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Raise the section count to 3:
        myConfigurations.append(MultiSpinBoxSection());
        myConfigurations.append(MultiSpinBoxSection());
        myConfigurations.append(MultiSpinBoxSection());
        myMulti.setSectionConfigurations(myConfigurations);
        // Control that all the new sections got the default value:
        QCOMPARE(myMulti.sectionValues().at(1), myDoubleSpinBox.value());
        QCOMPARE(myMulti.sectionValues().at(2), myDoubleSpinBox.value());

        // Put specific values into each of the 3 sections:
        myValues.clear();
        myValues.append(10);
        myValues.append(11);
        myValues.append(12);
        myValues.append(13); // Too many values for current configuration count
        myMulti.setSectionValues(myValues);
        // Assert that the values have been applied correctly
        QCOMPARE(myMulti.sectionValues().at(0), 10);
        QCOMPARE(myMulti.sectionValues().at(1), 11);
        QCOMPARE(myMulti.sectionValues().at(2), 12);
        // The last value has to be ignored (as there are not so many sections):
        QCOMPARE(myMulti.sectionConfigurations().size(), 3);
        QCOMPARE(myMulti.sectionValues().size(), 3);

        // Apply a configuration with less sections
        myConfigurations.removeLast();
        QCOMPARE(myConfigurations.size(), 2); // Assertion
        myMulti.setSectionConfigurations(myConfigurations);
        QCOMPARE(myMulti.sectionConfigurations().size(), 2);
        QCOMPARE(myMulti.sectionValues().size(), 2);
        // The values that survive should not be changed:
        QCOMPARE(myMulti.sectionValues().at(0), 10);
        QCOMPARE(myMulti.sectionValues().at(1), 11);

        // Set sectionValues that has not enough values
        QCOMPARE(myMulti.sectionConfigurations().size(), 2); // Assertion
        QCOMPARE(myMulti.sectionValues().size(), 2); // Assertion
        QCOMPARE(myMulti.sectionValues().at(0), 10); // Assertion
        QCOMPARE(myMulti.sectionValues().at(1), 11); // Assertion
        myValues.clear();
        myValues.append(20);
        // Apply a value list with only 1 value:
        myMulti.setSectionValues(myValues);
        QCOMPARE(myMulti.sectionValues().at(0), 20); // This values was applied
        // Section count has not been altered:
        QCOMPARE(myMulti.sectionConfigurations().size(), 2);
        QCOMPARE(myMulti.sectionValues().size(), 2);
        // The last section, that got no particular value assigned,
        // has been changed to the default value. (This behaviour
        // is not documented, so not part of the public API, but
        // is seems reasonable (and less confusing and more
        // predictable than just stay with the old value:
        QCOMPARE(myMulti.sectionValues().at(1), 0);
    }

    void testSectionValuesChangedSignalBasic()
    {
        // Initialize
        MultiSpinBox myMulti;
        MultiSpinBoxSection myConfig;
        QList<MultiSpinBoxSection> myConfigs;
        myConfigs.append(myConfig);
        myConfigs.append(myConfig);
        myMulti.setSectionConfigurations(myConfigs);
        myMulti.show();
        QSignalSpy spyMulti( //
            &myMulti, //
            &MultiSpinBox::sectionValuesChanged);
        QSignalSpy spyMultiAsQString //
            (&myMulti, //
             &MultiSpinBox::sectionValuesChangedAsQString);
        QDoubleSpinBox myDouble;
        myDouble.show();
        QSignalSpy spyDouble( //
            &myDouble, //
            QOverload<double>::of(&QDoubleSpinBox::valueChanged));
        // QDoubleSpinBox::textChanged is a bad name. Effectively, it is
        // the counterpart of MultiSpinBox::sectionValuesChangedAsQString
        QSignalSpy spyDoubleAsQString( //
            &myDouble, //
            &QDoubleSpinBox::textChanged);

        // Make sure MultiSpinBox behaves correctly and analogous to
        // QDoubleSpinBox

        // Set a value different from the default
        myMulti.setSectionValues(QList<double>{2, 2});
        myDouble.setValue(2);
        QCOMPARE(spyMulti.size(), 1);
        QCOMPARE(spyMulti.size(), spyDouble.size());
        QCOMPARE(spyMultiAsQString.size(), 1);
        QCOMPARE(spyMultiAsQString.size(), spyDoubleAsQString.size());

        // Setting the same value again should not call again the signal
        myMulti.setSectionValues(QList<double>{2, 2});
        myDouble.setValue(2);
        QCOMPARE(spyMulti.size(), 1);
        QCOMPARE(spyMulti.size(), spyDouble.size());
        QCOMPARE(spyMultiAsQString.size(), 1);
        QCOMPARE(spyMultiAsQString.size(), spyDoubleAsQString.size());

        // Setting a value list which has only one different element triggers:
        myMulti.setSectionValues(QList<double>{2, 3});
        myDouble.setValue(3);
        QCOMPARE(spyMulti.size(), 2);
        QCOMPARE(spyMulti.size(), spyDouble.size());
        QCOMPARE(spyMultiAsQString.size(), 2);
        QCOMPARE(spyMultiAsQString.size(), spyDoubleAsQString.size());
    }

    void testLocaleChange()
    {
        MultiSpinBoxSection mySection;
        mySection.setDecimals(2);
        mySection.setMinimum(0);
        mySection.setMaximum(10000);
        mySection.setGroupSeparatorShown(true);
        MultiSpinBox mySpinBox;
        mySpinBox.setLocale( //
            QLocale(QLocale::English));
        mySpinBox.setSectionConfigurations({mySection});
        mySpinBox.setSectionValues({3456.78});
        QCOMPARE(mySpinBox.sectionValues(), {3456.78});
        QCOMPARE(mySpinBox.text(), QStringLiteral("3,456.78"));
        mySpinBox.setLocale( //
            QLocale(QLocale::German));
        QCOMPARE(mySpinBox.sectionValues(), {3456.78});
        QCOMPARE(mySpinBox.text(), QStringLiteral("3.456,78"));
        // Sort of a special case: QLocale::C is a simplified English locale
        // with quirks (e.g. group separators are off by default).
        mySpinBox.setLocale(QLocale(QLocale::C));
        QCOMPARE(mySpinBox.sectionValues(), {3456.78});
        QCOMPARE(mySpinBox.text(), QStringLiteral("3,456.78"));
    }

    void testSectionValuesChangedSignalKeyboardTrackingEnabled()
    {
        // Initialize
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Use only one section to allow to compare easily
            // with QDoubleSpinBox
            QList<MultiSpinBoxSection>{MultiSpinBoxSection()});
        myMulti.show();
        QSignalSpy spyMulti( //
            &myMulti, //
            &MultiSpinBox::sectionValuesChanged);
        QSignalSpy spyMultiAsQString( //
            &myMulti, //
            &MultiSpinBox::sectionValuesChangedAsQString);
        myMulti.show();
        QDoubleSpinBox myDouble;
        myDouble.show();
        QSignalSpy spyDouble( //
            &myDouble, //
            QOverload<double>::of(&QDoubleSpinBox::valueChanged));
        QSignalSpy spyDoubleAsQString( //
            &myDouble, //
            &QDoubleSpinBox::textChanged);

        // Test with keyboard tracking enabled
        myMulti.setKeyboardTracking(true);
        myDouble.setKeyboardTracking(true);

        // Get test data
        myMulti.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&myMulti));
        myMulti.setFocus();
        myMulti.setSectionValues({8});
        QTest::keyClick(&myMulti, Qt::Key_Up); // Get text selection
        QTest::keyClick(&myMulti, Qt::Key::Key_5);
        QTest::keyClick(&myMulti, Qt::Key::Key_4);
        QCOMPARE(myMulti.sectionValues().at(0), 54); // Assertion

        // Get reference data
        myDouble.setValue(8);
        myDouble.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&myDouble));
        myDouble.setFocus();
        QTest::keyClick(&myDouble, Qt::Key_Up);
        QTest::keyClick(&myDouble, Qt::Key::Key_5);
        QTest::keyClick(&myDouble, Qt::Key::Key_4);
        QCOMPARE(myDouble.value(), 54); // Assertion

        // Test conformance of MultiSpinBox with QDoubleSpinBox’s behaviour
        QCOMPARE(spyMulti.size(), spyDouble.size());
        QCOMPARE(spyMultiAsQString.size(), spyDoubleAsQString.size());
        for (int i = 0; i < spyMulti.size(); ++i) {
            QCOMPARE(spyMulti
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .value<QList<double>>() // Convert to original type
                         .at(0), // First section of the MultiSpinBox
                     spyDouble
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .toDouble() // Convert to original type
            );
            QCOMPARE(spyMultiAsQString
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .toString(), // Convert to original type
                     spyDoubleAsQString
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .toString() // Convert to original type
            );
        }
    }

    void testSectionValuesChangedSignalKeyboardTrackingDisabled()
    {
        // Initialize
        QWidget helper;
        helper.show();
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Use only one section to allow to compare easily
            // with QDoubleSpinBox
            QList<MultiSpinBoxSection>{MultiSpinBoxSection()});
        myMulti.show();
        QSignalSpy spyMulti( //
            &myMulti, //
            &MultiSpinBox::sectionValuesChanged);
        QSignalSpy spyMultiAsQString( //
            &myMulti, //
            &MultiSpinBox::sectionValuesChangedAsQString);
        QSignalSpy spyMultiEditingFinished( //
            &myMulti, //
            &MultiSpinBox::editingFinished);
        QDoubleSpinBox myDouble;
        myDouble.show();
        QSignalSpy spyDouble( //
            &myDouble, //
            QOverload<double>::of(&QDoubleSpinBox::valueChanged));
        QSignalSpy spyDoubleAsQString( //
            &myDouble, //
            &QDoubleSpinBox::textChanged);

        // Test with keyboard tracking disabled
        myMulti.setKeyboardTracking(false);
        myDouble.setKeyboardTracking(false);

        // Get test data
        myMulti.setSectionValues({8});
        myMulti.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&myMulti));
        myMulti.setFocus();
        spyMultiEditingFinished.clear();
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        QTest::keyClick(&myMulti, Qt::Key_Up); // Get text selection
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        QTest::keyClick(&myMulti, Qt::Key::Key_5);
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        QTest::keyClick(&myMulti, Qt::Key::Key_4);
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        QTest::keyClick(&myMulti, Qt::Key::Key_Return);
        QCOMPARE(spyMultiEditingFinished.size(), 1);
        QTest::keyClick(&myMulti, Qt::Key::Key_3);
        QCOMPARE(spyMultiEditingFinished.size(), 1);
        QTest::keyClick(&myMulti, Qt::Key::Key_2);
        myMulti.stepUp();
        QCOMPARE(spyMultiEditingFinished.size(), 1);
        helper.activateWindow(); // Make spinbox loose focus
        QVERIFY(QTest::qWaitForWindowActive(&helper));
        helper.setFocus();
        // activateWindow() and setFocus() might behave differently across
        // non-graphical testing environments and different Qt versions,
        // however the invocation of both should result in at least one
        // editingFinished() signal emitted.
        QVERIFY(spyMultiEditingFinished.size() >= 2);

        // Get reference data
        myDouble.setValue(8);
        myDouble.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&myDouble));
        myDouble.setFocus();
        QTest::keyClick(&myDouble, Qt::Key_Up);
        QTest::keyClick(&myDouble, Qt::Key::Key_5);
        QTest::keyClick(&myDouble, Qt::Key::Key_4);
        QTest::keyClick(&myDouble, Qt::Key::Key_Return);
        QTest::keyClick(&myDouble, Qt::Key::Key_3);
        QTest::keyClick(&myDouble, Qt::Key::Key_2);
        myDouble.stepUp();
        helper.activateWindow(); // Make spinbox loose focus
        QVERIFY(QTest::qWaitForWindowActive(&helper));
        helper.setFocus();

        // Test conformance of MultiSpinBox with QDoubleSpinBox’s behaviour
        QCOMPARE(spyMulti.size(), spyDouble.size());
        QCOMPARE(spyMultiAsQString.size(), spyDoubleAsQString.size());
        for (int i = 0; i < spyMulti.size(); ++i) {
            QCOMPARE(spyMulti
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .value<QList<double>>() // Convert to original type
                         .at(0), // First section of the MultiSpinBox
                     spyDouble
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .toDouble() // Convert to original type
            );
            QCOMPARE(spyMultiAsQString
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .toString(), // Convert to original type
                     spyDoubleAsQString
                         .at(i) // Signal at position i
                         .at(0) // First argument of this signal
                         .toString() // Convert to original type
            );
        }
    }

    void signalsOnTabWhithoutKeyboardTracking()
    {
        QScopedPointer<QWidget> parentWidget(new QWidget());
        MultiSpinBox *widget2 = //
            new MultiSpinBox(parentWidget.data());
        widget2->setKeyboardTracking(false);
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        QSignalSpy spyMultiEditingFinished( //
            widget2, //
            &MultiSpinBox::editingFinished);
        QSignalSpy spyMultiValueChanged( //
            widget2, //
            &MultiSpinBox::sectionValuesChanged);
        QSpinBox *widget3 = new QSpinBox(parentWidget.data());
        widget3->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        QLabel *label2 = new QLabel(QStringLiteral(u"&Test"), //
                                    parentWidget.data());
        label2->setBuddy(widget2);
        widget2->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);

        // It is necessary to show the widget and make it active
        // to make focus and widget events working within unit tests.
        parentWidget->show();
        parentWidget->activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(parentWidget.data()));
        QVERIFY(widget2->hasFocus());
        QVERIFY(!widget3->hasFocus());
        QVERIFY(QApplication::focusWidget() == widget2);
        QVERIFY(widget2->d_pointer->m_sectionConfigurations.size() == 3);

        // Start actual testing

        // Apparently it isn’t possible to call simply the key click
        // on the parent widget. This code fails sometimes:
        // QTest::keyClick(parentWidget, Qt::Key::Key_Tab);
        // Therefore, we call QTest::keyClick() on
        // QApplication::focusWidget()

        QCOMPARE(widget2->d_pointer->m_currentIndex, 0);
        QCOMPARE(widget2->sectionValues().at(0), 0);
        QCOMPARE(spyMultiValueChanged.size(), 0);
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Up);
        QCOMPARE(widget2->sectionValues().at(0), 1);
        QCOMPARE(spyMultiValueChanged.size(), 1);
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_2);
        QCOMPARE(widget2->sectionValues().at(0), 1);
        QCOMPARE(spyMultiValueChanged.size(), 1);
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        // Move focus from widget2/section0 to widget2/section1
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(widget2->sectionValues().at(0), 2);
        QCOMPARE(spyMultiValueChanged.size(), 2);
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        // Move focus from widget2/section1 to widget2/section2
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(widget2->sectionValues().at(0), 2);
        QCOMPARE(spyMultiValueChanged.size(), 2);
        QCOMPARE(spyMultiEditingFinished.size(), 0);
        // Move focus from widget2/section2 to widget3
        QTest::keyClick(QApplication::focusWidget(), Qt::Key::Key_Tab);
        QCOMPARE(widget2->sectionValues().at(0), 2);
        QCOMPARE(spyMultiValueChanged.size(), 2);
        QCOMPARE(spyMultiEditingFinished.size(), 1);

        // Cleanup
        delete widget2;
        delete widget3;
        delete label2;
    }

    void testRoundingBehaviourCompliance()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(5);
        myConfig.setMaximum(360);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Create on-the-fly a list with only one section
            QList<MultiSpinBoxSection>{myConfig});
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(0);
        myDoubleSpinBox.setMinimum(5);
        myDoubleSpinBox.setMaximum(360);

        myMulti.setSectionValues(QList<double>{-1});
        myDoubleSpinBox.setValue(-1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{0});
        myDoubleSpinBox.setValue(0);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds down and stays too small
        myMulti.setSectionValues(QList<double>{4.1});
        myDoubleSpinBox.setValue(4.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that is too small, but rounds up to the minimum
        myMulti.setSectionValues(QList<double>{4.9}); // Rounds up to 5
        myDoubleSpinBox.setValue(4.9); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{5});
        myDoubleSpinBox.setValue(5);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds down to the minimum
        myMulti.setSectionValues(QList<double>{5.1});
        myDoubleSpinBox.setValue(5.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value in the middle that rounds down
        myMulti.setSectionValues(QList<double>{72.1}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value in the middle that rounds up
        myMulti.setSectionValues(QList<double>{72.9}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that is in range and rounds down
        myMulti.setSectionValues(QList<double>{359.1});
        myDoubleSpinBox.setValue(359.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds up to the maximum
        myMulti.setSectionValues(QList<double>{359.9});
        myDoubleSpinBox.setValue(359.9);

        // Test with maximum
        myMulti.setSectionValues(QList<double>{360});
        myDoubleSpinBox.setValue(360);

        // Test with value that rounds down to maximum
        myMulti.setSectionValues(QList<double>{360.1});
        myDoubleSpinBox.setValue(360.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{361});
        myDoubleSpinBox.setValue(361);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testRoundingBehaviourComplianceWithRoundedRanges()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(4.8);
        myConfig.setMaximum(360.2);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Create on-the-fly a list with only one section
            QList<MultiSpinBoxSection>{myConfig});
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(0);
        myDoubleSpinBox.setMinimum(4.8);
        myDoubleSpinBox.setMaximum(360.2);

        myMulti.setSectionValues(QList<double>{-1});
        myDoubleSpinBox.setValue(-1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{0});
        myDoubleSpinBox.setValue(0);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        // Test with a value that rounds down and stays too small
        myMulti.setSectionValues(QList<double>{4.1});
        myDoubleSpinBox.setValue(4.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{4.7});
        myDoubleSpinBox.setValue(4.7); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{4.8});
        myDoubleSpinBox.setValue(4.8); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{4.9});
        myDoubleSpinBox.setValue(4.9); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{5});
        myDoubleSpinBox.setValue(5);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{5.1}); // Rounds up to 5
        myDoubleSpinBox.setValue(5.1); // Rounds up to 5
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{72.1}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{72.9}); // Rounds up to 5
        myDoubleSpinBox.setValue(72.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{359.1});
        myDoubleSpinBox.setValue(359.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{359.9});
        myDoubleSpinBox.setValue(359.9);

        myMulti.setSectionValues(QList<double>{360});
        myDoubleSpinBox.setValue(360);

        myMulti.setSectionValues(QList<double>{360.1});
        myDoubleSpinBox.setValue(360.1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{360.2});
        myDoubleSpinBox.setValue(360.2);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{360.3});
        myDoubleSpinBox.setValue(360.3);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{360.9});
        myDoubleSpinBox.setValue(360.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{361});
        myDoubleSpinBox.setValue(361);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testRoundingBehaviourCornerCases()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(4.8);
        myConfig.setMaximum(359.8);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(
            // Create on-the-fly a list with only one section
            QList<MultiSpinBoxSection>{myConfig});
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(0);
        myDoubleSpinBox.setMinimum(4.8);
        myDoubleSpinBox.setMaximum(359.8);

        myMulti.setSectionValues(QList<double>{359});
        myDoubleSpinBox.setValue(359);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{359.7});
        myDoubleSpinBox.setValue(359.7);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{359.8});
        myDoubleSpinBox.setValue(359.8);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{359.9});
        myDoubleSpinBox.setValue(359.9);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myMulti.setSectionValues(QList<double>{360});
        myDoubleSpinBox.setValue(360);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testRoundingAfterChangingDecimals()
    {
        // Test the compliance of the behaviour of this class with
        // the behaviour of QDoubleSpinBox
        QList<MultiSpinBoxSection> myConfigs
            // Initialize the list with one single, default section
            {MultiSpinBoxSection()};
        myConfigs[0].setDecimals(2);
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(myConfigs);
        QDoubleSpinBox myDoubleSpinBox;
        myDoubleSpinBox.setDecimals(2);
        const double initialTestValue = 12.34;
        myMulti.setSectionValues(QList<double>{initialTestValue});
        myDoubleSpinBox.setValue(initialTestValue);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(1);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(0);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(3);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(3);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());

        myConfigs[0].setDecimals(-1);
        myMulti.setSectionConfigurations(myConfigs);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myMulti.sectionValues().at(0), myDoubleSpinBox.value());
    }

    void testMaximumWrappingRounding_data()
    {
        QTest::addColumn<double>("value");

        QTest::newRow("-360.1") << -360.1;
        QTest::newRow("-360") << -360.0;
        QTest::newRow("-359.9") << -359.9;
        QTest::newRow("-0.1") << -0.1;
        QTest::newRow("0") << 0.0;
        QTest::newRow("0.1") << 0.1;
        QTest::newRow("359.9") << 359.9;
        QTest::newRow("360") << 360.0;
        QTest::newRow("360.1") << 360.1;
        QTest::newRow("719.9") << 719.9;
        QTest::newRow("720") << 720.0;
        QTest::newRow("720.1") << 720.1;
    }

    void testClear()
    {
        QScopedPointer<QWidget> parentWidget(new QWidget());
        PerceptualColor::MultiSpinBox *widget2 = //
            new PerceptualColor::MultiSpinBox(parentWidget.data());
        widget2->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        widget2->setSectionConfigurations(exampleConfigurations);
        widget2->setFocus();
        parentWidget->setFocusPolicy(Qt::FocusPolicy::StrongFocus);
        parentWidget->show();
        widget2->d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(1);
        // The following statement make focus and widget events working.
        parentWidget->activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(parentWidget.data()));
        // Assert that the setup is okay.
        QVERIFY(widget2->hasFocus());
        QVERIFY(QApplication::focusWidget() == widget2);
        QVERIFY(widget2->d_pointer->m_sectionConfigurations.size() == 3);
        QVERIFY(widget2->lineEdit()->text() == QStringLiteral(u"0°  0%  0"));

        // Assert that the setup is okay.
        widget2->setSectionValues({1, 1, 1});
        QCOMPARE(widget2->lineEdit()->text(), QStringLiteral(u"1°  1%  1"));
        widget2->lineEdit()->setCursorPosition(5);
        QCOMPARE( //
            widget2->stepEnabled(), //
            QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled);
        QCOMPARE(widget2->d_pointer->m_currentIndex, 1);

        // Start actual testing
        widget2->clear();
        QCOMPARE(widget2->lineEdit()->text(), QStringLiteral(u"1°  %  1"));

        // Cleanup
        delete widget2;
    }

    void testReadOnly()
    {
        QScopedPointer<PerceptualColor::MultiSpinBox> widget( //
            new PerceptualColor::MultiSpinBox());
        widget->setSectionConfigurations(exampleConfigurations);
        widget->setSectionValues({1, 1, 1});
        // Assert that the setup is okay.
        QCOMPARE(widget->lineEdit()->text(), QStringLiteral(u"1°  1%  1"));
        QCOMPARE( //
            widget->stepEnabled(), //
            QAbstractSpinBox::StepUpEnabled | QAbstractSpinBox::StepDownEnabled);
        // Actual test:
        widget->setReadOnly(true);
        QCOMPARE(widget->stepEnabled(), QAbstractSpinBox::StepNone);
    }

    void testMaximumWrappingRounding()
    {
        // When using wrapping, the MultiSpinBox is supposed to never
        // show “360”, but instead “0”. This should also be true when
        // rounding applies. And when being a magnitude higher or lower.

        // Get data
        QFETCH(double, value);

        // Initialization
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(0);
        myConfig.setMinimum(0);
        myConfig.setMaximum(360);
        myConfig.setWrapping(true);
        MultiSpinBox mySpinBox;
        mySpinBox.setSectionConfigurations(
            // Create the QList on the fly…
            QList<MultiSpinBoxSection>{myConfig});

        mySpinBox.setSectionValues(
            // Create the QList on the fly…
            QList<double>{value});
        QCOMPARE(mySpinBox.text(), QStringLiteral("0"));

        mySpinBox.setSectionValues(
            // Create the QList on the fly…
            QList<double>{359.9});
        QCOMPARE(mySpinBox.text(), QStringLiteral("0"));
    }

    void testMetaTypeDeclaration()
    {
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(MultiSpinBoxSection());
    }

    void testMetaTypeDeclarationForPropertySectionValues()
    {
        // The data type QList<double> seems to be automatically declared
        // because it’s an instance of QList. This unit test controls this
        // assumption.
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(QList<double>());
    }

    void testValidate()
    {
        MultiSpinBoxSection mySection;
        mySection.setFormatString(QStringLiteral("abc%1def"));
        mySection.setMinimum(0);
        mySection.setMaximum(1000);
        mySection.setDecimals(0);
        QList<MultiSpinBoxSection> myConfigs{mySection};
        MultiSpinBox myMulti;
        myMulti.setSectionConfigurations(myConfigs);

        const QString originalInput = QStringLiteral("abc123def");
        QString myInput = originalInput;
        const int originalPos = 5;
        int myPos = originalPos;
        QValidator::State result = myMulti.validate(myInput, myPos);
        // The input should be considered valid.
        QCOMPARE(result, QValidator::State::Acceptable);
        // On simple cases of valid input, the string should not change.
        QCOMPARE(myInput, originalInput);
        // On simple cases of valid input, the position should not change.
        QCOMPARE(myPos, originalPos);
    }

    void testDecimalSeparatorJump()
    {
        // Initialize
        MultiSpinBox myMulti;
        MultiSpinBoxSection section;
        section.setFormatString(QStringLiteral("%1"));
        section.setMinimum(0);
        section.setMaximum(1000);
        section.setDecimals(2);
        myMulti.setLocale(QLocale::German);
        myMulti.setSectionConfigurations(QList<MultiSpinBoxSection>{section});
        myMulti.show();
        myMulti.setKeyboardTracking(true);
        myMulti.activateWindow();
        QVERIFY(QTest::qWaitForWindowActive(&myMulti));
        myMulti.setFocus();
        myMulti.setSectionValues({12.34});
        QSignalSpy spyMulti( //
            &myMulti, //
            &MultiSpinBox::sectionValuesChanged);
        QSignalSpy spyMultiAsQString( //
            &myMulti, //
            &MultiSpinBox::sectionValuesChangedAsQString);
        QCOMPARE(myMulti.text(), QStringLiteral("12,34"));
        QCOMPARE(spyMulti.size(), 0);
        QCOMPARE(spyMultiAsQString.size(), 0);

        myMulti.lineEdit()->setCursorPosition(1);
        QTest::keyClick(&myMulti, Qt::Key_Comma); // Should be ignored
        QCOMPARE(myMulti.text(), QStringLiteral("12,34"));
        QCOMPARE(spyMulti.size(), 0);
        QCOMPARE(spyMultiAsQString.size(), 0);
        QCOMPARE(myMulti.lineEdit()->cursorPosition(), 1);

        myMulti.lineEdit()->setCursorPosition(2);
        QTest::keyClick(&myMulti, Qt::Key_Comma); // Should advance cursor
        QCOMPARE(myMulti.text(), QStringLiteral("12,34"));
        QCOMPARE(spyMulti.size(), 0);
        QCOMPARE(spyMultiAsQString.size(), 0);
        QCOMPARE(myMulti.lineEdit()->cursorPosition(), 3);

        myMulti.lineEdit()->setCursorPosition(3);
        QTest::keyClick(&myMulti, Qt::Key_Comma); // Should be ignored
        QCOMPARE(myMulti.text(), QStringLiteral("12,34"));
        QCOMPARE(spyMulti.size(), 0);
        QCOMPARE(spyMultiAsQString.size(), 0);
        QCOMPARE(myMulti.lineEdit()->cursorPosition(), 3);
    }

    void testSnippet02()
    {
        snippet02();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestMultiSpinBox)

// The following “include” is necessary because we do not use a header file:
#include "testmultispinbox.moc"
