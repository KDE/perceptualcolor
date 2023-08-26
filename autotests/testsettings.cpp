// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "settings.h"

#include "setting.h"
#include "settingbase.h"
#include <qfile.h>
#include <qglobal.h>
#include <qobject.h>
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
#endif

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

private:
    static const inline QString organization = QStringLiteral("kde.org");
    // Avoid side-effects on the actual configuration file of the actual
    // library: Use a value different application name:
    static const inline QString application = QStringLiteral("libperceptualcolortestsettings");

private Q_SLOTS:
    void initTestCase()
    {
        // Called before the first test function is executed

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

    void testConstructorDestructor()
    {
        // There should be no crash:
        const Settings mySettings(QSettings::UserScope, //
                                  organization, //
                                  application);
        Q_UNUSED(mySettings)
    }

#ifndef MSVC_DLL
    // The automatic export of otherwise private symbols on MSVC
    // shared libraries via CMake's WINDOWS_EXPORT_ALL_SYMBOLS property
    // does not work well for Qt meta objects, resulting in non-functional
    // signals. Since the following unit tests require signals, it cannot be
    // built for MSVC shared libraries.

    void testIntegration()
    {
        // Test integration with Setting and SettingBase.
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);

        Setting<QString> tab(QStringLiteral("group/testSetting"), &mySettings);

        const QString newTab1 = QStringLiteral("testTab");
        tab.setValue(newTab1);
        QCOMPARE(tab.value(), newTab1);

        QSignalSpy spy(&tab, &PerceptualColor::SettingBase::valueChanged);

        const QString newTab2 = QStringLiteral("differentTestTab");
        tab.setValue(newTab2);
        tab.setValue(newTab2); // Intentional duplicate
        QCOMPARE(tab.value(), newTab2);
        QVERIFY(spy.isValid());
        // The second call to the setter with an identical value
        // should not trigger a signal.
        QCOMPARE(spy.count(), 1);
    }
#endif

    void testInternalQSettings()
    {
        // Test integration with Setting and SettingBase.
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);

        QCOMPARE(mySettings.m_qSettings.organizationName(), organization);
        QCOMPARE(mySettings.m_qSettings.applicationName(), application);
        QCOMPARE(mySettings.m_qSettings.scope(), QSettings::UserScope);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestSettings)

// The following “include” is necessary because we do not use a header file:
#include "testsettings.moc"
