// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#include "chromahuediagram.h"
#include "chromalightnessdiagram.h"
#include "colordialog.h"
#include "colorpatch.h"
#include "colorwheel.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "gradientslider.h"
#include "helper.h"
#include "multispinbox.h"
#include "multispinboxsection.h"
#include "rgbcolorspace.h"
#include "rgbcolorspacefactory.h"
#include "settranslation.h"
#include "swatchbook.h"
#include "version.h"
#include "wheelcolorpicker.h"
#include <cstdlib>
#include <qaction.h>
#include <qapplication.h>
#include <qcolor.h>
#include <qcombobox.h>
#include <qcommandlineoption.h>
#include <qcommandlineparser.h>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qfont.h>
#include <qfontdatabase.h>
#include <qfontinfo.h>
#include <qglobal.h>
#include <qicon.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlocale.h>
#include <qnamespace.h>
#include <qobjectdefs.h>
#include <qpalette.h>
#include <qpixmap.h>
#include <qscopedpointer.h>
#include <qsharedpointer.h>
#include <qstring.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qstyle.h>
#include <qstylefactory.h>
#include <qtabwidget.h>
#include <qversionnumber.h>
#include <qwidget.h>
#include <type_traits>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#else
#include <qstringlist.h>
#endif

using namespace PerceptualColor;

// Force a font for a widget and all direct or indirect children widgets.
//
// The given font is set on the widget and all its direct or indirect
// children which are subclasses of <tt>QWidget</tt>. If the widget is
// a <tt>nullptr</tt>, nothing happens.
//
// Use case: QApplication::setFont() occasionally does not work on all
// child widgets, so a special enforcement is needed.
static void forceFont(QWidget *widget, const QFont &font = qApp->font())
{
    if (widget == nullptr) {
        return;
    }
    widget->setFont(font);
    for (auto child : std::as_const(widget->children())) {
        forceFont(qobject_cast<QWidget *>(child), font);
    }
}

static void screenshotInternal(QWidget *widget, const QString &comment = QString())
{
    // Get fully qualified class name
    QString className = QString::fromUtf8(widget->metaObject()->className());
    // Strip all the qualifiers
    className = className.split(QStringLiteral("::")).last();
    widget->grab().save(
        // File name:
        className + comment + QStringLiteral(".png"),
        // File format: nullptr means: The file format will be chosen
        // from file name’s suffix.
        nullptr,
        // Compression:
        // 0 means: The compression is slow and results in a small file size.
        // 100 means: The compression is fast and results in a big file size.
        0);
}

// Screenshots of widgets with asynchronous image processing.
//
// This function is not deterministic! If the delays are enough to
// get the full-quality screenshot depends on the speed of your
// hardware and on how much other applications are running on
// your system!
static void screenshotDelayed(QWidget *widget, const QString &comment = QString())
{
    QWidget parent;
    const auto oldParent = widget->parentWidget();
    widget->setParent(&parent);
    parent.show();
    // forceFont influences the metrics. Therefore, calling it before
    // QWidget::resize() and QWidget::show().
    forceFont(widget);
    // Set an acceptable widget size (important
    // standalone-widgets without layout management):
    widget->resize(widget->sizeHint());
    widget->show(); // Necessary to receive and process events like paintEvent()
    delayedEventProcessing();
    screenshotInternal(widget, comment);
    widget->hide();
    widget->setParent(oldParent);
}

// A message handler that does not print any messages.
static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
{
    // dummy message handler that does not print messages
}

// This function tries to set as many settings as possible to hard-coded
// values: The widget style, the translation, the icon set and many more.
// This makes it more likely to get the same screenshots on different
// computers with different settings.
static void initWidgetAppearance(QApplication *app)
{
    // We prefer the Fusion style because he is the most cross-platform
    // style, so generating the screenshots does not depend on the
    // current system. Furthermore, it has support for fraction
    // scale factors such as 1.25 or 1.5.
    //
    // Possible styles (not all available in all setups):
    // "Breeze", "dsemilight", "dsemidark", "dlight", "ddark", "kvantum-dark",
    // "kvantum", "cleanlooks", "gtk2", "cde", "motif", "plastique", "Oxygen",
    // "QtCurve", "Windows", "Fusion"
    const QStringList styleNames{QStringLiteral("Fusion"), //
                                 QStringLiteral("Breeze"), //
                                 QStringLiteral("Oxygen")};
    QStyle *style = nullptr;
    for (const QString &styleName : styleNames) {
        style = QStyleFactory::create(styleName);
        if (style != nullptr) {
            break;
        }
    }
    QApplication::setStyle(style); // This call is safe even if style==nullptr.

    // Fusion uses by default the system’s palette, but
    // we want something system-independent to make the screenshots
    // look always the same. Therefore, we explicitly set Fusion’s
    // standard palette.
    {
        QScopedPointer<QStyle> tempStyle( //
            QStyleFactory::create(QStringLiteral("Fusion")));
        QPalette tempPalette = tempStyle->standardPalette();
        // The following colors are missing in Fusion’s standard palette:
        // They appear in Qt’s documentation of QPalette::ColorRole,
        // but do not appear when passing Fusion’s standard palette to
        // qDebug. Therefore, we set them explicitly to the default values
        // that are mentioned in the documentation of QPalette::ColorRole.
        tempPalette.setColor(QPalette::Link, Qt::blue);
        tempPalette.setColor(QPalette::Link, Qt::magenta);
        QApplication::setPalette(tempPalette);
    }

    // By default, the icons of the system are made available by
    // QPlatformTheme. However, we want to make screenshots that
    // are independent of the currently selected icon theme on
    // the computer that produces the screenshots. Therefore, we
    // choose an invalid search path for icon themes to avoid
    // that missing icons are found in other themes that are
    // available on the current computer:
    QIcon::setThemeSearchPaths(QStringList(QStringLiteral("invalid")));
    // Now, we change the standard icon theme to an invalid value.
    // As the search path has also been set to an invalid, missing
    // icons cannot be replaced by fallback icons.
    QIcon::setThemeName(QStringLiteral("invalid"));
    qInstallMessageHandler(voidMessageHandler); // Suppress warnings
    {
        // Trigger a call to the new, invalid icon theme. This call
        // will produce a message on the console:
        //     “Icon theme "invalid" not found.”
        // Here, we trigger it intentionally while having the message
        // suppressed. The message appears only at the first call
        // to the invalid icon theme, but not on subsequent calls.
        // Therefore, we will not get more messages for this in the
        // rest of this program.
        QWidget widget;
        widget.repaint();
        QCoreApplication::processEvents();
    }
    qInstallMessageHandler(nullptr); // Do not suppress warnings anymore

    // Other initializations
    app->setApplicationName(QStringLiteral("Perceptual color picker"));
    app->setLayoutDirection(Qt::LeftToRight);
    QLocale::setDefault(QLocale::English);
    PerceptualColor::setTranslation(app, //
                                    QLocale(QLocale::English).uiLanguages());
}

// We try to be as explicit as possible about the fonts.
// std::exit() is called if one of the fontfiles couldn’t be loaded.
static void initFonts(QApplication *app, const QStringList &fontfiles)
{
    // NOTE It would even be possible to bundle a font as Qt resource
    // to become completely independent from the fonts that are
    // installed on the system: https://stackoverflow.com/a/30973961

    QStringList fontFamilies;
    for (const QString &fontfile : std::as_const(fontfiles)) {
        const int id = QFontDatabase::addApplicationFont(fontfile);
        if (id == -1) {
            qWarning() << "Font file could not be loaded:" << fontfile;
            std::exit(-1);
        }
        fontFamilies.append(QFontDatabase::applicationFontFamilies(id));
    }
    fontFamilies.append(QStringLiteral("Noto Sans")); // Fallback
    fontFamilies.append(QStringLiteral("Noto Sans Symbols2")); // Fallback
    // NOTE The font size is defined in “point”, whatever “point” is.
    // Actually, the size of a “point” depends on the scale factor,
    // which is set elsewhere yet. So, when the scale factor is
    // correct, than using a fixed “point” size should give us
    // identical results also on different systems.
    QFont myFont = QFont(fontFamilies.first(), //
                         10, //
                         QFont::Weight::Normal, //
                         QFont::Style::StyleNormal);
    // Anti-alias might be different on different systems. Disabling it
    // entirely would look too ugly, but we disable subpixel antialias to make
    // the results between different systems at least smaller.
    constexpr auto styleStrategy = static_cast<QFont::StyleStrategy>( //
        QFont::PreferAntialias | QFont::NoSubpixelAntialias);
    myFont.setStyleStrategy(styleStrategy);
    myFont.setStyleHint(QFont::SansSerif, styleStrategy);
    myFont.setFamilies(fontFamilies);
    // It seems QFont::exactMatch() and QFontInfo::exactMatch() do not
    // work reliable on the X Window System, because this systems does
    // not provide the required functionality. Workaround: Compare
    // the actually used family (available via QFontInfo) with the
    // originally requested family (available via QFont):
    if (QFontInfo(myFont).family() != myFont.family()) {
        qWarning() << "Could not load font correctly:" << myFont.family();
    }
    app->setFont(myFont);
}

static void setCurrentTab(ColorDialog *dialog, int index)
{
    QList<QTabWidget *> tabWidgets = dialog->findChildren<QTabWidget *>();
    if (tabWidgets.count() != 1) {
        throw 0;
    }
    QTabWidget *myTabWidget = tabWidgets.first();
    const auto count = myTabWidget->count();
    if (count > 1) {
        // After calling QTabWidget::setCurrentIndex(), the tab widget may not
        // repaint properly if the event loop is not running.
        // Workaround: temporarily switch to a different index, then set the
        // desired index afterward.
        if (index == 0) {
            myTabWidget->setCurrentIndex(1);
        } else {
            myTabWidget->setCurrentIndex(0);
        }
    }
    myTabWidget->setCurrentIndex(index);
}

static void setCurrentSwatchBookSelector(ColorDialog *dialog, int index)
{
    QList<QComboBox *> comboBoxes = dialog->findChildren<QComboBox *>();
    if (comboBoxes.count() != 1) {
        throw 0;
    }
    QComboBox *myComboBox = comboBoxes.first();
    const auto count = myComboBox->count();
    if (count > 1) {
        // After calling QComboBox::setCurrentIndex(), the tab widget may not
        // repaint properly if the event loop is not running.
        // Workaround: temporarily switch to a different index, then set the
        // desired index afterward.
        if (index == 0) {
            myComboBox->setCurrentIndex(1);
        } else {
            myComboBox->setCurrentIndex(0);
        }
    }
    myComboBox->setCurrentIndex(index);
}

static void makeScreenshots()
{
    // Variables
    QSharedPointer<RgbColorSpace> m_colorSpace = //
        RgbColorSpaceFactory::createSrgb();
    // Chose a default color:
    // — that is present in the basic colors (to show the selection mark)
    // — is quite chromatic (which looks nice on screenshots)
    // — has nevertheless a little bit of distance to the outer
    //   hull (which  puts the marker somewhere in the inner of
    //   the gamut, which makes the screenshots easier to understand).
    const QColor defaultColorRgb = QColor::fromRgb(50, 127, 206);
    const GenericColor defaultColorCielchD50 = //
        m_colorSpace->toCielchD50(defaultColorRgb.rgba64());
    QColor myColor;

    {
        ChromaHueDiagram m_chromaHueDiagram(m_colorSpace);
        m_chromaHueDiagram.setCurrentColorCielchD50(defaultColorCielchD50);
        screenshotDelayed(&m_chromaHueDiagram);
    }

    {
        ChromaLightnessDiagram m_chromaLightnessDiagram(m_colorSpace);
        m_chromaLightnessDiagram.setCurrentColorCielchD50(defaultColorCielchD50);
        screenshotDelayed(&m_chromaLightnessDiagram);
    }

    {
        ColorDialog m_colorDialog(m_colorSpace);
        m_colorDialog.setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        m_colorDialog.setCurrentColor(defaultColorRgb);
        setCurrentTab(&m_colorDialog, 0);
        setCurrentSwatchBookSelector(&m_colorDialog, 0); // basic colors
        screenshotDelayed(&m_colorDialog);
    }

    {
        ColorDialog m_colorDialog(m_colorSpace);
        m_colorDialog.setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        m_colorDialog.setCurrentColor(defaultColorRgb);
        setCurrentTab(&m_colorDialog, 1);
        screenshotDelayed(&m_colorDialog, QStringLiteral("Tab1"));
    }

    {
        ColorDialog m_colorDialog(m_colorSpace);
        m_colorDialog.setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        m_colorDialog.setCurrentColor(defaultColorRgb);
        setCurrentTab(&m_colorDialog, 2);
        screenshotDelayed(&m_colorDialog, QStringLiteral("Tab2"));
    }

    {
        ColorDialog m_colorDialog(m_colorSpace);
        m_colorDialog.setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        m_colorDialog.setCurrentColor(defaultColorRgb);
        setCurrentTab(&m_colorDialog, 1);
        m_colorDialog.setOption( //
            ColorDialog::ColorDialogOption::ShowAlphaChannel);
        myColor = m_colorDialog.currentColor();
        myColor.setAlphaF(0.5);
        m_colorDialog.setCurrentColor(myColor);
        screenshotDelayed(&m_colorDialog, QStringLiteral("Alpha"));
    }

    {
        ColorDialog m_colorDialog(m_colorSpace);
        m_colorDialog.setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Expanded);
        m_colorDialog.setCurrentColor(defaultColorRgb);
        setCurrentTab(&m_colorDialog, 1);
        m_colorDialog.setOption( //
            ColorDialog::ColorDialogOption::ShowAlphaChannel);
        myColor = m_colorDialog.currentColor();
        myColor.setAlphaF(0.5);
        m_colorDialog.setCurrentColor(myColor);
        screenshotDelayed(&m_colorDialog, QStringLiteral("Expanded"));
    }

    {
        ColorDialog m_colorDialog(m_colorSpace);
        m_colorDialog.setLayoutDimensions( //
            ColorDialog::DialogLayoutDimensions::Collapsed);
        m_colorDialog.setCurrentColor(defaultColorRgb);
        setCurrentTab(&m_colorDialog, 1);
        m_colorDialog.setOption( //
            ColorDialog::ColorDialogOption::ShowAlphaChannel);
        myColor = m_colorDialog.currentColor();
        myColor.setAlphaF(0.5);
        m_colorDialog.setCurrentColor(myColor);
        screenshotDelayed(&m_colorDialog, QStringLiteral("Collapsed"));
    }

    {
        ColorPatch m_colorPatch;
        myColor = defaultColorRgb;
        m_colorPatch.setColor(myColor);
        screenshotDelayed(&m_colorPatch);
        myColor.setAlphaF(0.5);
        m_colorPatch.setColor(myColor);
        screenshotDelayed(&m_colorPatch, QStringLiteral("SemiTransparent"));
        m_colorPatch.setColor(QColor());
        screenshotDelayed(&m_colorPatch, QStringLiteral("Invalid"));
    }

    {
        ColorWheel m_colorWheel(m_colorSpace);
        m_colorWheel.setHue(defaultColorCielchD50.third);
        screenshotDelayed(&m_colorWheel);
    }

    {
        GradientSlider m_gradientSlider(m_colorSpace);
        m_gradientSlider.setOrientation(Qt::Horizontal);
        screenshotDelayed(&m_gradientSlider);
    }

    {
        MultiSpinBox m_multiSpinBox;
        PerceptualColor::MultiSpinBoxSection mySection;
        QList<PerceptualColor::MultiSpinBoxSection> hsvSectionConfigurations;
        QList<double> values;
        mySection.setDecimals(1);
        mySection.setPrefix(QString());
        mySection.setMinimum(0);
        mySection.setWrapping(true);
        mySection.setMaximum(360);
        mySection.setSuffix(QStringLiteral(u"° "));
        hsvSectionConfigurations.append(mySection);
        values.append(310);
        mySection.setPrefix(QStringLiteral(u" "));
        mySection.setMinimum(0);
        mySection.setMaximum(255);
        mySection.setWrapping(false);
        mySection.setSuffix(QStringLiteral(u" "));
        hsvSectionConfigurations.append(mySection);
        values.append(200);
        mySection.setSuffix(QString());
        hsvSectionConfigurations.append(mySection);
        values.append(100);
        m_multiSpinBox.setSectionConfigurations(hsvSectionConfigurations);
        m_multiSpinBox.setSectionValues(values);
        screenshotDelayed(&m_multiSpinBox);

        // Out-of-gamut button for the HLC spin box
        QAction *myAction = new QAction(
            // Icon:
            qIconFromTheme( //
                QStringList(),
                QStringLiteral("eye-exclamation"),
                ColorSchemeType::Light),
            // Text:
            QString(),
            // Parent object:
            &m_multiSpinBox);
        MultiSpinBox m_multiSpinBoxWithButton;
        m_multiSpinBoxWithButton.setSectionConfigurations( //
            hsvSectionConfigurations);
        m_multiSpinBoxWithButton.setSectionValues(values);
        m_multiSpinBoxWithButton.addActionButton( //
            myAction, //
            QLineEdit::ActionPosition::TrailingPosition);
        screenshotDelayed(&m_multiSpinBoxWithButton, QStringLiteral("WithButton"));
    }

    {
        WheelColorPicker m_wheelColorPicker(m_colorSpace);
        m_wheelColorPicker.setCurrentColorCielchD50(defaultColorCielchD50);
        screenshotDelayed(&m_wheelColorPicker);
    }

    {
        SwatchBook m_swatchBook(m_colorSpace, //
                                wcsBasicColors(m_colorSpace),
                                Qt::Orientation::Horizontal);
        m_swatchBook.setCurrentColor(defaultColorRgb);
        screenshotDelayed(&m_swatchBook);
    }

    {
        QList<QColor> myColorList;
        myColorList.append(Qt::red);
        myColorList.append(Qt::green);
        myColorList.append(Qt::blue);
        const Swatches mySwatches(4, 4, myColorList);
        SwatchBook m_swatchBook(m_colorSpace, //
                                mySwatches,
                                Qt::Orientation::Horizontal);
        m_swatchBook.setCurrentColor(defaultColorRgb);
        m_swatchBook.setEditable(true);
        screenshotDelayed(&m_swatchBook, QStringLiteral("Editable"));
    }
}

// Creates a set of screenshots of the library and saves these
// screenshots as .png files in the working directory.
int main(int argc, char *argv[])
{
    // Adjust the scale factor before constructing our real QApplication
    // object:
    {
        // See https://doc.qt.io/qt-6/highdpi.html for documentation
        // about QT_SCALE_FACTOR. In short: For testing purpose, it
        // can be used to adjust the current system-default scale
        // factor. This affects both, widget painting and font
        // rendering (font DPI).
        //
        // We choose a small factor, because the actual default size
        // of dialog and top-level widgets in Qt is: smaller or
        // than ⅔ of the screen. This affects our color dialog, which
        // allows small sizes, but recommends bigger ones. As the
        // screen size of the computer running this program is not
        // known in advance, we try to minimize the effects be choosing
        // the smallest possible scale factor, which is 1. (Values
        // smaller than 1 are working: They break the layout.)
        constexpr qreal screenshotScaleFactor = 1;
        // Create a temporary QApplication object within this block scope.
        // Necessary to get the system’s scale factor.
        QApplication app(argc, argv);
        const qreal systemScaleFactor = QWidget().devicePixelRatioF();
        bool conversionOkay;
        double qtScaleFactor = //
            qEnvironmentVariable("QT_SCALE_FACTOR").toDouble(&conversionOkay);
        if (!conversionOkay) {
            qtScaleFactor = 1;
        }
        qtScaleFactor = //
            qtScaleFactor / systemScaleFactor * screenshotScaleFactor;
        // Set QT_SCALE_FACTOR to a corrected factor. This will only
        // take effect when the current QApplication object has been
        // destroyed and a new one has been created.
        qputenv("QT_SCALE_FACTOR", QString::number(qtScaleFactor).toUtf8());
    }

    // Prepare configuration before instantiating the application object
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);
#endif

    // Instantiate the application object
    QApplication app(argc, argv);
    app.setApplicationName(QStringLiteral("generatescreenshots"));
    app.setApplicationVersion(perceptualColorRunTimeVersion().toString());
    QCommandLineParser parser;
    const QString description = QStringLiteral( //
        "Generate screenshots of PerceptualColor widgets for documentation.\n"
        "\n"
        "The generated screenshots are similar also when this application\n"
        "is used on different operation systems. The used QStyle() and\n"
        "color schema and scaling factor are hard-coded. However, fonts\n"
        "render slightly different on different systems. You can explicitly\n"
        "specify the font files to use; this might reduce the differences,\n"
        "but will not eliminate them entirely.");
    parser.setApplicationDescription(description);
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption native //
        {QStringLiteral("native"),
         QStringLiteral("Use the current environment’s default style instead "
                        "of a hard-coded style. Also, “fontfiles” will be "
                        "ignored.")};
    parser.addOption(native);
    parser.addPositionalArgument( //
        QStringLiteral("fontfiles"), //
        QStringLiteral("Zero or more font files (preferred fonts first)."));
    parser.process(app);
    if (!parser.isSet(native)) {
        initWidgetAppearance(&app);
        initFonts(&app, parser.positionalArguments());
    }

    // Do the actual work
    makeScreenshots();

    // Return
    return EXIT_SUCCESS;
}
