// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "perceptualsettings.h"

#include "setting.h"
#include "settingbase.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsignalspy.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtest_gui.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

static void snippet01()
{
    // cppcheck-suppress constVariableReference // snippet for documentation
    //! [PerceptualSettings Instance]
    auto &settingsRef = PerceptualColor::PerceptualSettings::getInstance();
    //! [PerceptualSettings Instance]
    Q_UNUSED(settingsRef)
}

namespace PerceptualColor
{
class TestPerceptualSettings : public QObject
{
    Q_OBJECT

public:
    explicit TestPerceptualSettings(QObject *parent = nullptr)
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

    void testSnippet01()
    {
        snippet01();
    }

    void testColorListConstructor()
    {
        PerceptualSettings::ColorList myList;
        Q_UNUSED(myList);
    }

    void testConstructorDestructor()
    {
        // There should be no crash:
        const auto &mySettings = PerceptualSettings::getInstance();
        Q_UNUSED(mySettings)
    }

#ifndef MSVC_DLL
    // The automatic export of otherwise private symbols on MSVC
    // shared libraries via CMake's WINDOWS_EXPORT_ALL_SYMBOLS property
    // does not work well for Qt meta objects, resulting in non-functional
    // signals. Since the following unit tests require signals, it cannot be
    // built for MSVC shared libraries.

    void testCustomColors()
    {
        auto &mySettings = PerceptualSettings::getInstance();

        const PerceptualColor::PerceptualSettings::ColorList newColors1 = {QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue)};
        mySettings.customColors.setValue(newColors1);
        QCOMPARE(mySettings.customColors.value(), newColors1);

        QSignalSpy spy(&mySettings.customColors, &PerceptualColor::SettingBase::valueChanged);

        const PerceptualColor::PerceptualSettings::ColorList newColors2 = {QColor(Qt::cyan), QColor(Qt::magenta), QColor(Qt::yellow)};
        mySettings.customColors.setValue(newColors2);
        mySettings.customColors.setValue(newColors2); // Intentional duplicate
        QCOMPARE(mySettings.customColors.value(), newColors2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
    }

    void testTab()
    {
        auto &mySettings = PerceptualSettings::getInstance();

        const QString newTab1 = QStringLiteral("testTab");
        mySettings.tab.setValue(newTab1);
        QCOMPARE(mySettings.tab.value(), newTab1);

        QSignalSpy spy(&mySettings.tab, &PerceptualColor::SettingBase::valueChanged);

        const QString newTab2 = QStringLiteral("differentTestTab");
        mySettings.tab.setValue(newTab2);
        mySettings.tab.setValue(newTab2); // Intentional duplicate
        QCOMPARE(mySettings.tab.value(), newTab2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
    }

    void testInstancesAreIdenticalForTab()
    {
        // As this is implemented as singleton, calling the singleton function
        // various times should still produce interchangeable results.

        auto &mySettings1 = PerceptualSettings::getInstance();
        auto &mySettings2 = PerceptualSettings::getInstance();

        const QString newTab1 = QStringLiteral("testTabInstance");
        mySettings1.tab.setValue(newTab1);
        QCOMPARE(mySettings1.tab.value(), newTab1);
        QCOMPARE(mySettings2.tab.value(), newTab1);

        QSignalSpy spy1(&mySettings1.tab, &PerceptualColor::SettingBase::valueChanged);
        QSignalSpy spy2(&mySettings1.tab, &PerceptualColor::SettingBase::valueChanged);

        const QString newTab2 = QStringLiteral("differentTestTabInstance");
        mySettings2.tab.setValue(newTab2);
        mySettings2.tab.setValue(newTab2); // Intentional duplicate
        QCOMPARE(mySettings1.tab.value(), newTab2);
        QCOMPARE(mySettings2.tab.value(), newTab2);
        QVERIFY(spy1.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy1.count(), 1);
        QVERIFY(spy2.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy2.count(), 1);
    }

    void testTabExpanded()
    {
        auto &mySettings = PerceptualSettings::getInstance();

        const QString newTab1 = QStringLiteral("testTabExpanded");
        mySettings.tabExpanded.setValue(newTab1);
        QCOMPARE(mySettings.tabExpanded.value(), newTab1);

        QSignalSpy spy(&mySettings.tabExpanded, &PerceptualColor::SettingBase::valueChanged);

        const QString newTab2 = QStringLiteral("differentTestTab");
        mySettings.tabExpanded.setValue(newTab2);
        mySettings.tabExpanded.setValue(newTab2); // Intentional duplicate
        QCOMPARE(mySettings.tabExpanded.value(), newTab2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
    }

    void testInstancesAreIdenticalForTabExpanded()
    {
        auto &mySettings1 = PerceptualSettings::getInstance();
        auto &mySettings2 = PerceptualSettings::getInstance();

        const QString newTabExpanded1 = QStringLiteral("testTabExpandedInstance");
        mySettings1.tabExpanded.setValue(newTabExpanded1);
        QCOMPARE(mySettings1.tabExpanded.value(), newTabExpanded1);
        QCOMPARE(mySettings2.tabExpanded.value(), newTabExpanded1);

        QSignalSpy spy1(&mySettings1.tabExpanded, &PerceptualColor::SettingBase::valueChanged);
        QSignalSpy spy2(&mySettings2.tabExpanded, &PerceptualColor::SettingBase::valueChanged);

        const QString newTabExpanded2 = QStringLiteral("differentTestTabExpandedInstance");
        mySettings2.tabExpanded.setValue(newTabExpanded2);
        mySettings2.tabExpanded.setValue(newTabExpanded2); // Intentional duplicate
        QCOMPARE(mySettings1.tabExpanded.value(), newTabExpanded2);
        QCOMPARE(mySettings2.tabExpanded.value(), newTabExpanded2);
        QVERIFY(spy1.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy1.count(), 1);
        QVERIFY(spy2.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy2.count(), 1);
    }

#endif
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestPerceptualSettings)

// The following “include” is necessary because we do not use a header file:
#include "testperceptualsettings.moc"
