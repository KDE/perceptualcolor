// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "settingbase.h"

#include "settings.h"
#include <qfile.h>
#include <qglobal.h>
#include <qobject.h>
#include <qpointer.h>
#include <qsettings.h>
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
class TestSettingBase : public QObject
{
    Q_OBJECT

public:
    explicit TestSettingBase(QObject *parent = nullptr)
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

    void testConstuctorDestructor()
    {
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);
        {
            SettingBase mySettingBase(QStringLiteral("group/key"), //
                                      &mySettings,
                                      nullptr);
        }
    }

    void testParent()
    {
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);
        QPointer<SettingBase> mySettingBase;
        {
            QObject myParent;
            mySettingBase = new SettingBase( //
                QStringLiteral("group/key"), //
                &mySettings,
                &myParent);
            QCOMPARE(mySettingBase.data()->parent(), &myParent);
        }
        QVERIFY(mySettingBase.isNull());
    }

    void testKey()
    {
        Settings mySettings(QSettings::UserScope, //
                            organization, //
                            application);
        SettingBase mySettingBase(QStringLiteral("group/key"), //
                                  &mySettings,
                                  nullptr);
        QCOMPARE(mySettingBase.m_key, QStringLiteral("group/key"));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestSettingBase)

// The following “include” is necessary because we do not use a header file:
#include "testsettingbase.moc"
