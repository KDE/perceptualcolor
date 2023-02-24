// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "initializetranslation.h"

#include "colordialog.h"
#include "rgbcolorspacefactory.h"
#include <qcoreapplication.h>
#include <qglobal.h>
#include <qlocale.h>
#include <qobject.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qstring.h>
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#include <qstringlist.h>
#endif

namespace PerceptualColor
{

class TestInitializeTranslation : public QObject
{
    Q_OBJECT

public:
    explicit TestInitializeTranslation(QObject *parent = nullptr)
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

    // NOTE This MUST be the very first test, so that still
    // PerceptualColor::initializeTranslation() has never been called before.
    void testIfTranslationIsActuallyLoaded()
    {
        auto m_srgbBuildinColorSpace = RgbColorSpaceFactory::createSrgb();

        QLocale::setDefault(QLocale(QStringLiteral("nl")));
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::ColorDialog test1(m_srgbBuildinColorSpace);
        // Test if the window title is translated.
        QCOMPARE(test1.windowTitle(), QStringLiteral("Kleur selecteren"));

        // Try another language
        QLocale::setDefault(QLocale(QStringLiteral("es")));
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::ColorDialog test2(m_srgbBuildinColorSpace);
        // Test if the window title is translated.
        QCOMPARE(test2.windowTitle(), QStringLiteral("Seleccionar color"));

        // try if capital language codes work
        QLocale::setDefault(QLocale(QStringLiteral("CA")));
        PerceptualColor::initializeTranslation( //
            QCoreApplication::instance(), //
            QStringList(QStringLiteral("CA")));
        PerceptualColor::ColorDialog test3(m_srgbBuildinColorSpace);
        // Test if the window title is translated.
        QCOMPARE(test3.windowTitle(), //
                 QStringLiteral("Selecció de color"));

        // Test if removing all translations works
        QLocale::setDefault(QLocale(QString()));
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QStringList(QString()));
        PerceptualColor::ColorDialog test4(m_srgbBuildinColorSpace);
        // Test if the window title is actually not translated.
        QCOMPARE(test4.windowTitle(), QStringLiteral("Select color"));
    }

    void testInitializeDoesNotCrash()
    {
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
    }

    void testInitializeDoesNotCrashOnSuccessiveCalls()
    {
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestInitializeTranslation)

// The following “include” is necessary because we do not use a header file:
#include "testinitializetranslation.moc"
