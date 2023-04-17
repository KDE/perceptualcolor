// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "settings.h"

#include <qcolor.h>
#include <qglobal.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qsignalspy.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtest_gui.h>
#include <qtestcase.h>
#include <qvariant.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

static void snippet01()
{
    //! [Settings Instance]
    auto &mySettings = PerceptualColor::Settings::instance();
    //! [Settings Instance]
    Q_UNUSED(mySettings)
}

namespace PerceptualColor
{
class TestSettings : public QObject
{
    Q_OBJECT

public:
    explicit TestSettings(QObject *parent = nullptr)
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

    void testConstructorDestructor()
    {
        // There should be no crash:
        const auto &mySettings = Settings::instance();
        Q_UNUSED(mySettings)
    }

    void testColorListConstructor()
    {
        Settings::ColorList myList;
        Q_UNUSED(myList);
    }

#ifndef MSVC_DLL
    // The automatic export of otherwise private symbols on MSVC
    // shared libraries via CMake's WINDOWS_EXPORT_ALL_SYMBOLS property
    // does not work well for Qt meta objects, resulting in non-functional
    // signals. Since the following unit tests require signals, it cannot be
    // built for MSVC shared libraries.

    void testCustomColors()
    {
        auto &mySettings = Settings::instance();

        const PerceptualColor::Settings::ColorList newColors1 = {QColor(Qt::red), QColor(Qt::green), QColor(Qt::blue)};
        mySettings.setCustomColors(newColors1);
        QCOMPARE(mySettings.customColors(), newColors1);

        QSignalSpy spy(&mySettings, &PerceptualColor::Settings::customColorsChanged);

        const PerceptualColor::Settings::ColorList newColors2 = {QColor(Qt::cyan), QColor(Qt::magenta), QColor(Qt::yellow)};
        mySettings.setCustomColors(newColors2);
        mySettings.setCustomColors(newColors2); // Intentional duplicate
        QCOMPARE(mySettings.customColors(), newColors2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).value<PerceptualColor::Settings::ColorList>(), newColors2);
    }

    void testTab()
    {
        auto &mySettings = Settings::instance();

        const QString newTab1 = QStringLiteral("testTab");
        mySettings.setTab(newTab1);
        QCOMPARE(mySettings.tab(), newTab1);

        QSignalSpy spy(&mySettings, &PerceptualColor::Settings::tabChanged);

        const QString newTab2 = QStringLiteral("differentTestTab");
        mySettings.setTab(newTab2);
        mySettings.setTab(newTab2); // Intentional duplicate
        QCOMPARE(mySettings.tab(), newTab2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toString(), newTab2);
    }

    void testInstancesAreIdenticalForTab()
    {
        auto &mySettings1 = Settings::instance();
        auto &mySettings2 = Settings::instance();

        const QString newTab1 = QStringLiteral("testTabInstance");
        mySettings1.setTab(newTab1);
        QCOMPARE(mySettings1.tab(), newTab1);
        QCOMPARE(mySettings2.tab(), newTab1);

        QSignalSpy spy1(&mySettings1, &PerceptualColor::Settings::tabChanged);
        QSignalSpy spy2(&mySettings2, &PerceptualColor::Settings::tabChanged);

        const QString newTab2 = QStringLiteral("differentTestTabInstance");
        mySettings2.setTab(newTab2);
        mySettings2.setTab(newTab2); // Intentional duplicate
        QCOMPARE(mySettings1.tab(), newTab2);
        QCOMPARE(mySettings2.tab(), newTab2);
        QVERIFY(spy1.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy1.count(), 1);
        QCOMPARE(spy1.takeFirst().at(0).toString(), newTab2);
        QVERIFY(spy2.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy2.count(), 1);
        QCOMPARE(spy2.takeFirst().at(0).toString(), newTab2);
    }

    void testTabExpanded()
    {
        auto &mySettings = Settings::instance();

        const QString newTab1 = QStringLiteral("testTabExpanded");
        mySettings.setTabExpanded(newTab1);
        QCOMPARE(mySettings.tabExpanded(), newTab1);

        QSignalSpy spy(&mySettings, &PerceptualColor::Settings::tabExpandedChanged);

        const QString newTab2 = QStringLiteral("differentTestTab");
        mySettings.setTabExpanded(newTab2);
        mySettings.setTabExpanded(newTab2); // Intentional duplicate
        QCOMPARE(mySettings.tabExpanded(), newTab2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
        QCOMPARE(spy.takeFirst().at(0).toString(), newTab2);
    }

    void testInstancesAreIdenticalForTabExpanded()
    {
        auto &mySettings1 = Settings::instance();
        auto &mySettings2 = Settings::instance();

        const QString newTabExpanded1 = QStringLiteral("testTabExpandedInstance");
        mySettings1.setTabExpanded(newTabExpanded1);
        QCOMPARE(mySettings1.tabExpanded(), newTabExpanded1);
        QCOMPARE(mySettings2.tabExpanded(), newTabExpanded1);

        QSignalSpy spy1(&mySettings1, &PerceptualColor::Settings::tabExpandedChanged);
        QSignalSpy spy2(&mySettings2, &PerceptualColor::Settings::tabExpandedChanged);

        const QString newTabExpanded2 = QStringLiteral("differentTestTabExpandedInstance");
        mySettings2.setTabExpanded(newTabExpanded2);
        mySettings2.setTabExpanded(newTabExpanded2); // Intentional duplicate
        QCOMPARE(mySettings1.tabExpanded(), newTabExpanded2);
        QCOMPARE(mySettings2.tabExpanded(), newTabExpanded2);
        QVERIFY(spy1.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy1.count(), 1);
        QCOMPARE(spy1.takeFirst().at(0).toString(), newTabExpanded2);
        QVERIFY(spy2.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy2.count(), 1);
        QCOMPARE(spy2.takeFirst().at(0).toString(), newTabExpanded2);
    }

#endif

    void testKeyTab()
    {
        QCOMPARE(PerceptualColor::Settings::keyTab, QStringLiteral("colordialog/tab"));
    }

    void testKeyTabExpanded()
    {
        QCOMPARE(PerceptualColor::Settings::keyTabExpanded, QStringLiteral("colordialog/tabexpanded"));
    }

    void testKeyCustomColors()
    {
        QCOMPARE(PerceptualColor::Settings::keyCustomColors, QStringLiteral("colordialog/customcolors"));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestSettings)

// The following “include” is necessary because we do not use a header file:
#include "testsettings.moc"
