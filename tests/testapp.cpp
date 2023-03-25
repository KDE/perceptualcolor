// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#include "absolutecolor.h" // IWYU pragma: keep
#include "asyncimageprovider.h" // IWYU pragma: keep
#include "asyncimageproviderbase.h" // IWYU pragma: keep
#include "chromalightnessimageparameters.h" // IWYU pragma: keep
#include "colordialog.h" // IWYU pragma: keep
#include "constpropagatinguniquepointer.h" // IWYU pragma: keep
#include "csscolor.h" // IWYU pragma: keep
#include "genericcolor.h" // IWYU pragma: keep
#include "helper.h" // IWYU pragma: keep
#include "helperconstants.h" // IWYU pragma: keep
#include "helperconversion.h" // IWYU pragma: keep
#include "helpermath.h" // IWYU pragma: keep
#include "helperposixmath.h" // IWYU pragma: keep
#include "initializetranslation.h" // IWYU pragma: keep
#include "oklchvalues.h" // IWYU pragma: keep
#include "polarpointf.h" // IWYU pragma: keep
#include "rgbcolorspace.h" // IWYU pragma: keep
#include "rgbcolorspacefactory.h" // IWYU pragma: keep
#include "settings.h" // IWYU pragma: keep
#include "version.h" // IWYU pragma: keep
#include <qapplication.h> // IWYU pragma: keep
#include <qcolor.h> // IWYU pragma: keep
#include <qcolordialog.h> // IWYU pragma: keep
#include <qcommonstyle.h> // IWYU pragma: keep
#include <qcontainerfwd.h> // IWYU pragma: keep
#include <qcoreapplication.h> // IWYU pragma: keep
#include <qcoreevent.h> // IWYU pragma: keep
#include <qdebug.h> // IWYU pragma: keep
#include <qdialogbuttonbox.h> // IWYU pragma: keep
#include <qfileinfo.h> // IWYU pragma: keep
#include <qfontdatabase.h> // IWYU pragma: keep
#include <qgenericmatrix.h> // IWYU pragma: keep
#include <qglobal.h> // IWYU pragma: keep
#include <qhash.h> // IWYU pragma: keep
#include <qlabel.h> // IWYU pragma: keep
#include <qlayout.h> // IWYU pragma: keep
#include <qlibraryinfo.h> // IWYU pragma: keep
#include <qlist.h> // IWYU pragma: keep
#include <qlocale.h> // IWYU pragma: keep
#include <qmath.h> // IWYU pragma: keep
#include <qmetaobject.h> // IWYU pragma: keep
#include <qnamespace.h> // IWYU pragma: keep
#include <qobject.h> // IWYU pragma: keep
#include <qpixmap.h> // IWYU pragma: keep
#include <qpointer.h> // IWYU pragma: keep
#include <qrect.h> // IWYU pragma: keep
#include <qregularexpression.h> // IWYU pragma: keep
#include <qscopedpointer.h> // IWYU pragma: keep
#include <qsettings.h> // IWYU pragma: keep
#include <qsharedpointer.h> // IWYU pragma: keep
#include <qsize.h> // IWYU pragma: keep
#include <qsizepolicy.h> // IWYU pragma: keep
#include <qstring.h> // IWYU pragma: keep
#include <qstringlist.h> // IWYU pragma: keep
#include <qstringliteral.h> // IWYU pragma: keep
#include <qstyle.h> // IWYU pragma: keep
#include <qstylefactory.h> // IWYU pragma: keep
#include <qthread.h> // IWYU pragma: keep
#include <qtimer.h> // IWYU pragma: keep
#include <qtranslator.h> // IWYU pragma: keep
#include <utility> // IWYU pragma: keep

using namespace PerceptualColor;

// This is just a program for testing purposes.
int main(int argc, char *argv[])
{
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    // Prepare configuration before instantiating the application object
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    // Instantiate the application object
    QApplication app(argc, argv);

    // QLocale::setDefault(QLocale::Bengali);
    // QLocale::setDefault(QLocale(QLocale::German, QLocale::Country::Germany));
    // QLocale::setDefault(QLocale(QLocale::Georgian));
    // QLocale::setDefault(QLocale::English);
    // QLocale::setDefault(QLocale::French);
    // QLocale::setDefault(QLocale::Spanish);
    // QLocale::setDefault(QLocale::Ukrainian);
    // QLocale::setDefault(QLocale::Turkish);

    PerceptualColor::initializeTranslation(QCoreApplication::instance(), //
                                           QLocale().uiLanguages());

    // app.setLayoutDirection(Qt::RightToLeft);

    // Initialize the color dialog

    auto myColorSpace = //
        PerceptualColor::RgbColorSpaceFactory::createFromFile(
            // QStringLiteral("/usr/share/color/icc/colord/WideGamutRGB.icc") //
            // QStringLiteral("/usr/share/color/icc/test/PhotoGamutRGB_avg6c.icc")
            // QStringLiteral("WideGamutRGB.icc") //
            // QStringLiteral("/usr/share/color/icc/compatibleWithAdobeRGB1998.icc")
            QStringLiteral("invalid")
            // QStringLiteral("/usr/share/color/icc/sRGB.icc")
            // QStringLiteral("/usr/share/color/icc/krita/Rec2020-elle-V4-g10.icc") //
            // QStringLiteral("/usr/share/color/icc/krita/LargeRGB-elle-V2-g22.icc") //
            // QStringLiteral("/usr/share/color/icc/krita/WideRGB-elle-V2-g22.icc") //
            // QStringLiteral("/usr/share/color/icc/krita/XYZ-D50-Identity-elle-V4.icc") //
            // QStringLiteral("/usr/share/color/icc/krita/ACEScg-elle-V4-g10.icc") //
            // QStringLiteral("/usr/share/color/icc/krita/cmyk.icm") //
            // QStringLiteral("/usr/share/color/icc/krita/ITUR_2100_PQ_FULL.ICC") //
            // QStringLiteral("/usr/share/color/icc/ECI-RGB.V1.0.icc") //
        );
    if (myColorSpace.isNull()) {
        myColorSpace = PerceptualColor::RgbColorSpaceFactory::createSrgb();
    }

    PerceptualColor::ColorDialog m_colorDialog(myColorSpace);
    // For session management, according to https://doc.qt.io/qt-6/session.html
    //     “you must identify your top level widgets with
    //      unique application-wide object names”
    m_colorDialog.setObjectName(QStringLiteral("Percptual color dialog"));
    m_colorDialog.setOption(QColorDialog::ColorDialogOption::ShowAlphaChannel, true);
    QColor myColor = QColor(Qt::yellow);
    myColor.setAlphaF(0.5);
    // m_colorDialog.setCurrentColor(myColor);
    // m_colorDialog.setOption(QColorDialog::ColorDialogOption::NoButtons);
    m_colorDialog.setLayoutDimensions(PerceptualColor::ColorDialog::DialogLayoutDimensions::Expanded);
    // m_colorDialog.setEnabled(false);
    // m_colorDialog.setStyleSheet("background: yellow; color: red; border: 15px solid #FF0000;");
    m_colorDialog.show();

    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("kvantum")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("gtk2")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("QtCurve")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("cde")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Cleanlooks")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("IaOra"))); // https://store.kde.org/p/1183616
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("motif")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Plastik"))); // From https://github.com/MartinF99/PlastikStyle (Qt5 + Qt6)
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Plastique")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Oxygen")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Breeze")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("Windows")));
    // QApplication::setStyle(QStyleFactory::create(QStringLiteral("chameleon"))); // From package dde-qt5integration (deepin desktop environment)

    // QScopedPointer<QStyle> tempStyle(QStyleFactory::create(QStringLiteral("Fusion")));
    // qDebug() << tempStyle->standardPalette();
    // m_colorDialog.setPalette(tempStyle->standardPalette());

    /*
    m_colorDialog.setStyleSheet(
        QStringLiteral(" QDialog {"
                       "   background-color: green; "
                       "   border-radius: 30px; "
                       "   border: 1px solid black;"
                       "   border: 2px groove gray;"
                       " }"

                       " PerceptualColor--MultiSpinBox {"
                       "   background-color: red;"
                       "   border-radius: 3px; "
                       "   border: 10px solid black;"
                       "   border: 20px groove gray;"
                       "}"

                       " PerceptualColor--ColorDialog {"
                       "   background-color: yellow;"
                       "   border-radius: 30px; "
                       "   border: 1px solid black;"
                       "   border: 2px groove gray;"
                       "}"

                       " QFrameX {"
                       "   background-color: red;"
                       // "   border-radius: 30px; "
                       // "   border: 10px solid black;"
                       // "   border: 20px groove gray;"
                       "}"

                       " QPushButton {"
                       "   background-color: blue;"
                       // "   border-radius: 30px; "
                       // "   border: 10px solid black;"
                       // "   border: 20px groove gray;"
                       "}"

                       " PerceptualColor--ChromaHueDiagram {"
                       "   background-color: red;"
                       "   border-radius: 30px; "
                       // "   border: 10px solid black;"
                       "   border: 50px solid gray;"
                       "}"

                       " PerceptualColor--WheelColorPicker {"
                       "   background-color: red;"
                       "   border-radius: 30px; "
                       "   border: 1px solid black;"
                       "   border: 2px groove gray;"
                       "}"

                       " PerceptualColor--ColorPatchX {"
                       "   background-color: green;"
                       "   border-radius: 40px; "
                       "   border: 5px solid black;"
                       "   border: 10px groove gray;"
                       "}"
                       // " *:hover { background: red; }"
                       ));
    */

    /*
    QColorDialog *defaultColorDialog = new QColorDialog;
    defaultColorDialog->setOption(QColorDialog::ShowAlphaChannel);
    // For session management, according to https://doc.qt.io/qt-6/session.html
    //     “you must identify your top level widgets with
    //      unique application-wide object names”
    defaultColorDialog->setObjectName(QStringLiteral("Default Qt color dialog"));
    defaultColorDialog->show();
    */

    /*
    QScopedPointer<QTranslator> myTranslator;
    QTimer::singleShot(5000, // delay in milliseconds
                       QCoreApplication::instance(), // context object
                       [defaultColorDialog, &myTranslator]() {
                           // lambda to be executed
                           QLocale::setDefault(QLocale::German);
                           PerceptualColor::initializeTranslation( //
                               QCoreApplication::instance(), //
                               QLocale().uiLanguages());
                           myTranslator.reset(new QTranslator);
                           const bool hasBeenLoaded = myTranslator->load( //
                               QLocale(), //
                               QStringLiteral("qtbase"), //
                               QStringLiteral("_"), //
                               QStringLiteral("/usr/share/qt5/translations/"));
                           qDebug() << "Qt translation for" //
                                    << QLocale() //
                                    << "loaded?" //
                                    << hasBeenLoaded;
                           QCoreApplication::installTranslator( //
                                myTranslator.data());
                           QEvent languageChangeEvent(QEvent::LanguageChange);
                           QCoreApplication::sendEvent(defaultColorDialog, //
                                                       &languageChangeEvent);
                       } //
    );
    QTimer::singleShot(10000, // delay in milliseconds
                       QCoreApplication::instance(), // context object
                       [defaultColorDialog, &myTranslator]() {
                           // lambda to be executed
                           QLocale::setDefault(QLocale::German);
                           PerceptualColor::initializeTranslation( //
                               QCoreApplication::instance(), //
                               QLocale().uiLanguages());
                           myTranslator.reset(new QTranslator);
                           const bool hasBeenLoaded = myTranslator->load( //
                               QLocale(), //
                               QStringLiteral("qtbase"), //
                               QStringLiteral("_"), //
                               QStringLiteral("/usr/share/qt5/translations/"));
                           qDebug() << "Qt translation for" //
                                    << QLocale() //
                                    << "loaded?" //
                                    << hasBeenLoaded;
                           QCoreApplication::installTranslator( //
                                myTranslator.data());
                           QEvent languageChangeEvent(QEvent::LanguageChange);
                           QCoreApplication::sendEvent(defaultColorDialog, //
                                                       &languageChangeEvent);
                       } //
    );
    */

    auto &mySettings = Settings::instance();
    mySettings.setCustomColors(Settings::ColorList({Qt::black, Qt::green}));

#ifndef MSVC_DLL
    // Just for testing purpose: Miss-use the customColor property to
    // synchronize the current color of ColorDialog between various instances.
    QObject::connect(&m_colorDialog, //
                     &ColorDialog::currentColorChanged,
                     &mySettings,
                     [&](const QColor &color) {
                         mySettings.setCustomColors(Settings::ColorList({color}));
                     });
    QObject::connect(&mySettings, //
                     &Settings::customColorsChanged,
                     &m_colorDialog,
                     [&](const Settings::ColorList &customColors) {
                         m_colorDialog.setCurrentColor(customColors.value(0));
                     });
#endif

    // Run
    return app.exec();
}

#include "testapp.moc"
