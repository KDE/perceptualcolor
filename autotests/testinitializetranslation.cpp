﻿// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "initializetranslation.h"

#include "perceptualcolor-0/colordialog.h"
#include "perceptualcolor-0/rgbcolorspacefactory.h"
#include <qcoreapplication.h>
#include <qlocale.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>

namespace PerceptualColor
{

class TestInitializeTranslation : public QObject
{
    Q_OBJECT

public:
    TestInitializeTranslation(QObject *parent = nullptr)
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

        QLocale::setDefault(QLocale(QStringLiteral("de")));
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::ColorDialog test1(m_srgbBuildinColorSpace);
        // Test if the window title is translated to German.
        QCOMPARE(test1.windowTitle(), QStringLiteral("Farbauswahl"));

        // Try another language
        QLocale::setDefault(QLocale(QStringLiteral("es")));
        PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                               QLocale().uiLanguages());
        PerceptualColor::ColorDialog test2(m_srgbBuildinColorSpace);
        // Test if the window title is translated to German.
        QCOMPARE(test2.windowTitle(), QStringLiteral("Selecciona color"));

        // try if capital language codes work
        QLocale::setDefault(QLocale(QStringLiteral("FR")));
        PerceptualColor::initializeTranslation( //
            QCoreApplication::instance(), //
            QStringList(QStringLiteral("FR")));
        PerceptualColor::ColorDialog test3(m_srgbBuildinColorSpace);
        // Test if the window title is translated to German.
        QCOMPARE(test3.windowTitle(), //
                 QStringLiteral("Sélectionner une couleur"));

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