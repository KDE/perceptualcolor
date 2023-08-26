// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "setting.h"

#include "settingbase.h"
#include "settings.h"
#include <qfile.h>
#include <qglobal.h>
#include <qobject.h>
#include <qpointer.h>
#include <qsettings.h>
#include <qsignalspy.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestSetting : public QObject
{
    Q_OBJECT

public:
    explicit TestSetting(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    static const inline QString organization = QStringLiteral("kde.org");
    // Avoid side-effects on the actual configuration file of the actual
    // library: Use a value different application name:
    static const inline QString application = QStringLiteral("libperceptualcolortestsettings");

private Q_SLOTS:
    void initTestCase()
    {
        // Called before the first test function is executedQFile file(fileName);

        const QString fileName = QSettings(QSettings::IniFormat, //
                                           QSettings::UserScope, //
                                           organization, //
                                           application)
                                     .fileName();
        QFile(fileName).remove();
    }

    void cleanupTestCase()
    {
        // Called after the last test function was executed

        const QString fileName = QSettings(QSettings::IniFormat, //
                                           QSettings::UserScope, //
                                           organization, //
                                           application)
                                     .fileName();
        QFile(fileName).remove();
    }

    void init()
    {
        // Called before each test function is executed

        const QString fileName = QSettings(QSettings::IniFormat, //
                                           QSettings::UserScope, //
                                           organization, //
                                           application)
                                     .fileName();
        QFile(fileName).remove();
    }

    void cleanup()
    {
        // Called after every test function

        const QString fileName = QSettings(QSettings::IniFormat, //
                                           QSettings::UserScope, //
                                           organization, //
                                           application)
                                     .fileName();
        QFile(fileName).remove();
    }

#ifndef MSVC_DLL
    // The automatic export of otherwise private symbols on MSVC
    // shared libraries via CMake's WINDOWS_EXPORT_ALL_SYMBOLS property
    // does not work well for Qt meta objects, resulting in non-functional
    // signals. Since the following unit tests require signals, it cannot be
    // built for MSVC shared libraries.

    void testConstuctorDestructor()
    {
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);
        {
            Setting<int> mySetting(QStringLiteral("group/key"), //
                                   &mySettings,
                                   nullptr);
        }
    }

    void testParent()
    {
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);
        QPointer<Setting<int>> mySetting;
        {
            QObject myParent;
            mySetting = new Setting<int>( //
                QStringLiteral("group/key"), //
                &mySettings,
                &myParent);
            QCOMPARE(mySetting.data()->parent(), &myParent);
        }
        QVERIFY(mySetting.isNull());
    }

    void testGetSetSignal()
    {
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);

        Setting<QString> tab(QStringLiteral("group/testSetting"), &mySettings);

        const QString newTab1 = QStringLiteral("testTab");
        tab.setValue(newTab1);
        QCOMPARE(tab.m_value, newTab1);
        QCOMPARE(tab.value(), newTab1);

        QSignalSpy spy(&tab, &PerceptualColor::SettingBase::valueChanged);

        const QString newTab2 = QStringLiteral("differentTestTab");
        tab.setValue(newTab2);
        tab.setValue(newTab2); // Intentional duplicate
        QCOMPARE(tab.m_value, newTab2);
        QCOMPARE(tab.value(), newTab2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
    }

    void testUpdateFromSettings()
    {
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);

        const QString key = QStringLiteral("group/testUpdateFromSetting");
        Setting<QString> tab(key, //
                             &mySettings);

        const QString newTab1 = QStringLiteral("testTab");
        tab.setValue(newTab1);
        QCOMPARE(tab.m_value, newTab1);
        QCOMPARE(tab.value(), newTab1);

        const QString newTab2 = QStringLiteral("differentTestTab");
        tab.underlyingQSettings()->setValue(key, newTab2);
        QCOMPARE(tab.m_value, newTab1);
        QCOMPARE(tab.value(), newTab1);

        tab.updateFromQSettings();
        QCOMPARE(tab.m_value, newTab2);
        QCOMPARE(tab.value(), newTab2);
    }
#endif
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestSetting)

// The following “include” is necessary because we do not use a header file:
#include "testsetting.moc"
