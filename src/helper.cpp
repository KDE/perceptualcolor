// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helper.h"

#include "absolutecolor.h"
#include "genericcolor.h"
#include "helperconversion.h"
#include "helperposixmath.h"
#include "initializelibraryresources.h"
#include "rgbcolorspace.h"
#include <array>
#include <lcms2.h>
#include <qchar.h>
#include <qcolor.h>
#include <qevent.h>
#include <qkeysequence.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qsize.h>
#include <qstringliteral.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qlist.h>
#else
#include <qstringlist.h>
#endif

#ifndef PERCEPTUALCOLORINTERNAL
#include <type_traits>
#include <utility>
#endif

namespace PerceptualColor
{
/** @internal
 *
 * @brief Number of vertical <em>standard</em> wheel steps done by a
 *  wheel event
 *
 * As the QWheelEvent documentation explains, there is a common physical
 * standard wheel step size for mouse wheels: 15°. But there are some
 * mouse models which use non-standard physical wheel step sizes for
 * their mouse wheel, for example because they have a higher wheel
 * resolution.
 *
 * This function converts the values in a QMouseEvent to the
 * <em>standard</em> wheel step count.
 *
 * @param event the QWheelEvent
 * @returns the count of vertical <em>standard</em> wheel steps done
 * within this mouse event. The value is positive for up-steps and
 * negative for down-steps. On a standard mouse wheel, moving the wheel
 * one physical step up will return the value 1. On a non-standard,
 * higher resolution mouse wheel, moving the wheel one physical step up
 * will return a smaller value, for example 0.7 */
qreal standardWheelStepCount(QWheelEvent *event)
{
    // QWheelEvent::angleDelta() returns 8 units for each degree.
    // The standard wheel step is 15°. So on a standard
    // mouse, one wheel step results in (8 × 15) units.
    return event->angleDelta().y() / static_cast<qreal>(8 * 15);
}

/** @internal
 *
 * @brief Background for semi-transparent colors.
 *
 * When showing a semi-transparent color, there has to be a background
 * on which it is shown. This function provides a suitable background
 * for showcasing a color.
 *
 * @param devicePixelRatioF The desired device-pixel ratio. Must be ≥ 1.
 *
 * @returns An image of a mosaic of neutral gray rectangles of different
 * lightness. You can use this as tiles to paint a background, starting from
 * the top-left corner. This image is made for LTR layouts. If you have an
 * RTL layout, you should horizontally mirror your paint buffer after painting
 * the tiles. The image has its device pixel ratio set to the value that was
 * given in the parameter.
 *
 * @note The image is considering the given device-pixel ratio to deliver
 * sharp (and correctly scaled) images also for HiDPI devices.
 * The painting does not use floating point drawing, but rounds
 * to full integers. Therefore, the result is always a sharp image.
 * This function takes care that each square has the same pixel size,
 * without scaling errors or anti-aliasing errors.
 *
 * @sa @ref AbstractDiagram::transparencyBackground()
 *
 * @todo Provide color management support! Currently, we use the same
 * value for red, green and blue, this might <em>not</em> be perfectly
 * neutral gray depending on the color profile of the monitor… And: We
 * should make sure that transparent colors are not applied by Qt on top
 * of this image. Instead, add a parameter to this function to get the
 * transparent color to paint above, and do color-managed overlay of the
 * transparent color, in Lch space. For each Lab (not Lch!) channel:
 * result = opacity * foreground + (100% - opacity) * background. */
QImage transparencyBackground(qreal devicePixelRatioF)
{
    // The valid lightness range is [0, 255]. The median is 127/128.
    // We use two color with equal distance to this median to get a
    // neutral gray.
    constexpr int lightnessDistance = 15;
    constexpr int lightnessOne = 127 - lightnessDistance;
    constexpr int lightnessTwo = 128 + lightnessDistance;
    constexpr int squareSizeInLogicalPixel = 10;
    const int squareSize = qRound(squareSizeInLogicalPixel * devicePixelRatioF);

    QImage temp(squareSize * 2, squareSize * 2, QImage::Format_RGB32);
    temp.fill(QColor(lightnessOne, lightnessOne, lightnessOne));
    QPainter painter(&temp);
    QColor foregroundColor(lightnessTwo, lightnessTwo, lightnessTwo);
    painter.fillRect(0, 0, squareSize, squareSize, foregroundColor);
    painter.fillRect(squareSize, squareSize, squareSize, squareSize, foregroundColor);
    temp.setDevicePixelRatio(devicePixelRatioF);
    return temp;
}

/** @internal
 *
 * @brief Draws a QWidget respecting Qt Style Sheets.
 *
 * When subclassing QWidget-derived classes, the Qt Style Sheets are
 * considered automatically. But when subclassing QWidget itself, the
 * Qt Style Sheets are <em>not</em> considered automatically. Also,
 * calling <tt>QWidget::paintEvent()</tt> from the subclass’s paint
 * event does not help. Instead, call this function from within your
 * subclass’s paint event. It uses some special code as documented
 * in the <em>Qt Style Sheets Reference</em> in the section about QWidget.
 *
 * @warning This function creates a QPainter for the widget. As there
 * should be not more than one QPainter at the same time for a given
 * paint device, you may not call this function while a QPainter
 * exists for the widget. Therefore, it is best to call this
 * function as very first statement in your paintEvent() implementation,
 * before initializing any QPainter.
 *
 * @param widget the widget */
void drawQWidgetStyleSheetAware(QWidget *widget)
{
    QStyleOption opt;
    opt.initFrom(widget);
    QPainter p(widget);
    widget->style()->drawPrimitive(QStyle::PE_Widget, &opt, &p, widget);
}

/** @internal
 *
 * @brief Provides prefix and suffix of a value from a given format string.
 *
 * A typical use case: You want to put a percent value into a spinbox. The
 * simple approach would be:
 * @snippet testhelper.cpp percentTraditional
 * It could be improved like this:
 * @snippet testhelper.cpp percentImproved
 * However, in some languages, the position of the prefix and suffix may be
 * reversed compared to English. Example: In English, you write 50\%, but in
 * Turkish, you write \%50. Qt does not offer an out-of-the-box solution for
 * this. This helper now provides complete internationalization for prefixes
 * and suffixes of spin boxes, allowing to do this easily in a fail-safe way:
 * @snippet testhelper.cpp percentFullyInternationalized
 *
 * @param formatString A translated string in the format "prefix%1suffix". It
 * should contain exactly <em>one</em> place marker as described in
 * <tt>QString::arg()</tt> like <tt>\%1</tt> or <tt>\%L2</tt>. This place
 * marker represents the value. Example: “Prefix\%1Suffix”. Prefix and suffix
 * may be empty.
 *
 * @returns If the <tt>formatString</tt> parameter has the correct format,
 * the prefix will be returned at <tt>QPair::first</tt> and the suffix will
 * be returned at <tt>QPair::second</tt>. Otherwise, they will be set to an
 * empty string.
 *
 * @note The functionality implemented here served as the inspiration for
 * <a href="https://api.kde.org/frameworks/ki18n/html/namespaceKLocalization.html">
 * KLocalization::setupSpinBoxFormatString()</a>. However, there are key
 * differences.
 * <a href="https://api.kde.org/frameworks/ki18n/html/namespaceKLocalization.html">
 * KLocalization::setupSpinBoxFormatString()</a> extends this approach to
 * include support for plural handling, specifically for integers (but not
 * floating-point numbers). It uses %v as a placeholder instead of %1.
 * Because %v is not compatible with the translation system, this design allows
 * the string to be translated while enabling the selection of the appropriate
 * plural form by passing the relevant integer – without actually replacing the
 * placeholder with an actual value. Despite its broader functionality, it is
 * limited to handling QSpinBox and QDoubleSpinBox. Since this project does
 * not link to KLocalization, and our implementation already meets our specific
 * requirements, we continue to use our own approach.
 */
[[nodiscard]] QPair<QString, QString> getPrefixSuffix(const QString &formatString)
{
    // QString::arg() support for %L2, %5 etc which translators might expect:
    const auto list = formatString //
                          .arg(QStringLiteral("%1")) //
                          .split(QStringLiteral("%1"));
    if (list.count() == 2) {
        return QPair<QString, QString>(list.at(0), list.at(1));
    }
    return QPair<QString, QString>(QString(), QString());
}

/** @internal
 *
 * @brief Icon from theme.
 *
 * @param names List of names, preferred names first. The system’s icon
 *              themes are searched for this.
 * @param fallback If the system icon themes do not provide an icon, use
 *                 this fallback icon from the built-in resources.
 * @param type Type of widget color scheme for which the fallback icon (if
 *             used) should be suitable.
 *
 * @returns An icon from the system icons or a fallback icons. If none is
 * available, an empty icon.
 *
 * @internal
 *
 * @note Defining PERCEPTUALCOLORINTERNAL bypasses the platform's
 * icon theme, relying exclusively on the fallback. This behavior
 * is primarily intended to ensure that utils/generatescreenshots.cpp,
 * used for generating documentation screenshots, produces results
 * that are independent of the platform.
 *
 * @note By default, the library leverages any available icon
 * theme supported by Qt. On Linux, such themes are typically
 * present. Support for Windows, macOS, iOS, and Android was
 * <a href="https://doc.qt.io/qt-6/whatsnew67.html#qt-gui-module">
 * introduced only in Qt 6.7</a> via a
 * <a href="https://doc.qt.io/qt-6/qicon.html#ThemeIcon-enum">new
 * enum</a> and by <a href="https://www.qt.io/blog/qt-6.7-released">
 * mapping XDG icon names to platform-native symbols</a>.
 * However, none of the icons available via the new enum does
 * suit our needs. QIcon::ThemeIcon::DialogWarning could potentially be
 * (mis)used for the out-of-gamut warning, but all other necessary
 * icons—mostly color-related—are unavailable. Until Qt provides more
 * color-related icons in this enum, we will continue using FreeDesktop
 * icon names and bundled built-in icons as our traditional approach.
 */
QIcon qIconFromTheme(const QStringList &names, const QString &fallback, ColorSchemeType type)
{
#ifdef PERCEPTUALCOLORINTERNAL
    Q_UNUSED(names)
#else
    // Try to find icon in theme
    for (auto const &name : std::as_const(names)) {
        const QIcon myIcon = QIcon::fromTheme(name);
        if (!myIcon.isNull()) {
            return myIcon;
        }
    }
#endif

    // Return fallback icon
    initializeLibraryResources();
    QString path = QStringLiteral( //
        ":/PerceptualColor/icons/lighttheme/%1.svg");
    if (type == ColorSchemeType::Dark) {
        path = QStringLiteral( //
            ":/PerceptualColor/icons/darktheme/%1.svg");
    }
    return QIcon(path.arg(fallback));
}

/** @internal
 *
 * @brief Converts text with mnemonics to rich text rendering the mnemonics.
 *
 * At some places in Qt, mnemonics are used. For example, setting
 * <tt>QPushButton::setText()</tt> to "T&est" will make appear the text
 * "Test". If mnemonic support is enabled in the current platform theme,
 * the "e" is underlined.
 *
 * At some other places in Qt, rich text is used. For example, setting
 * <tt>QWidget::setToolTip()</tt> to "T<u>e</u>st" will make appear the text
 * "Test", but with the "e" underlined.
 *
 * @param mnemonicText A text that might contain mnemonics
 *
 * @returns A rich text that will render in rich-text-functions with the same
 * rendering as if the mnemonic text would have been
 * used in mnemonic-functions: If currently in the platform theme,
 * auto-mnemonics are enabled, the mnemonics are underlined. Otherwise,
 * the mnemonics are not underlined nor is the “&” character visible
 *
 * @note This function mimics Qt’s algorithm form mnemonic rendering quite
 * well, but there might be subtile differences in corner cases. Like Qt,
 * this function accepts multiple occurrences of "&" in the same string, even
 * before different characters, and underlines all of them, though
 * <tt>QKeySequence::mnemonic()</tt> will return only one of them as
 * shortcut. */
QString fromMnemonicToRichText(const QString &mnemonicText)
{
    QString result;

    const bool doUnderline = !QKeySequence::mnemonic(mnemonicText).isEmpty();
    const auto underlineStart = doUnderline ? QStringLiteral("<u>") : QString();
    const auto underlineStop = doUnderline ? QStringLiteral("</u>") : QString();

    bool underlineNextChar = false;
    for (int i = 0; i < mnemonicText.length(); ++i) {
        if (mnemonicText[i] == QStringLiteral("&")) {
            const auto nextChar = //
                (i + 1 < mnemonicText.length()) //
                ? mnemonicText[i + 1]
                : QChar();
            if (nextChar == QStringLiteral("&")) {
                // Double ampersand: Escape the "&"
                result.append(QStringLiteral("&"));
                i++; // Skip the second "&"
            } else {
                // Single ampersand: Start underline
                underlineNextChar = true;
            }
        } else {
            if (underlineNextChar) {
                // End underline
                result.append(underlineStart);
                result.append(mnemonicText[i]);
                result.append(underlineStop);
                underlineNextChar = false;
            } else {
                result.append(mnemonicText[i]);
            }
        }
    }

    return result;
}

/** @internal
 *
 * @brief Guess the actual @ref ColorSchemeType of a given widget.
 *
 * It guesses the color scheme type actually used by the current widget style,
 * and not the type of the current color palette. This makes a difference
 * for example on the Windows Vista style, which might ignore the palette and
 * use always a light theme instead.
 *
 * @param widget The widget to evaluate
 *
 * @returns The guessed scheme.
 *
 * @note The exact implementation of the guess  might change over time.
 *
 * @internal
 *
 * The current implementation creates a QLabel as child widget of the current
 * widget, than takes a screenshot of the QLabel and calculates
 * the average lightness of this screenshot and determines the color schema
 * type accordingly.
 *
 * @note With Qt 6.5, there is
 * <a href="https://www.qt.io/blog/dark-mode-on-windows-11-with-qt-6.5">
 * better access to color themes</a>. Apparently, the Windows Vista style
 * now seems to polish the widgets by setting a light color palette, so
 * also on Windows Vista style we could simply rely on the color palette
 * and test if the text color is lighter or darker than the background color
 * to determine the color scheme type. This would also give us more reliable
 * results with color schemes that have background colors around 50% lightness,
 * which our currently implementation has problems to get right. But on
 * the other hand, other styles like Kvantum might still chose to ignore
 * the color palette, so it seems safer to stay with the current
 * implementation. */
ColorSchemeType guessColorSchemeTypeFromWidget(QWidget *widget)
{
    if (widget == nullptr) {
        return ColorSchemeType::Light;
    }

    // Create a QLabel
    QScopedPointer<QLabel> label{new QLabel(widget)};
    label->setText(QStringLiteral("abc"));
    label->resize(label->sizeHint()); // Smaller size means faster guess.

    // Take a screenshot of the QLabel
    const QImage screenshot = label->grab().toImage();
    if (screenshot.size().isEmpty()) {
        return ColorSchemeType::Light;
    }

    // Calculate the average lightness of the screenshot
    float lightnessSum = 0;
    for (int y = 0; y < screenshot.height(); ++y) {
        for (int x = 0; x < screenshot.width(); ++x) {
            lightnessSum += QColor(screenshot.pixel(x, y)).lightnessF();
        }
    }
    const auto pixelCount = screenshot.width() * screenshot.height();
    constexpr float threeshold = 0.5;
    const bool isDark = //
        (lightnessSum / static_cast<float>(pixelCount)) < threeshold;
    if (isDark) {
        return ColorSchemeType::Dark;
    }
    return ColorSchemeType::Light;
}

/** @brief Swatch grid derived from the basic colors as by WCS (World color
 * survey).
 *
 * The swatch grid contains various tints and shades of the
 * basic colors. The choice of the basic colors is based on the
 * <a href="https://en.wikipedia.org/wiki/Basic_Color_Terms:_Their_Universality_and_Evolution">
 * study by Brent Berlin and Paul Kay</a>, who suggest that the
 * basic color terms in almost all languages on earth follow a universal
 * pattern. They propose that there are eleven basic color terms that appear
 * in this order during the evolution of a language:
 *
 * 1. black, white
 * 2. red
 * 3. green, yellow
 * 4. blue
 * 5. brown
 * 6. purple, pink, orange, gray
 *
 * Additionally, people worldwide seem to agree quite well on the typical
 * values of each of these color terms. This theory is a fascinating one
 * and forms a good basis for choosing basic colors for this swatch grid.
 *
 * This widget's colors have been arranged largely according to the color
 * wheel of the perceptually uniform color space. We start with the saturated
 * basic colors: red, orange, yellow, green, blue, and purple in order of
 * their hue angles. Next, we have pink and brown, which have roughly the
 * same hue as red or orange but are less saturated. These are simply the
 * less chromatic parts of this hue but are nevertheless perceived by humans as
 * independent colors. For each of these basic colors, there are five variants
 * in the order of <a href="https://en.wikipedia.org/wiki/Tints_and_shades">
 * tint, pure color, and shade</a>. Following the saturated colors and
 * eventually the less saturated ones, the gray axis comes in last place.
 *
 * What exact colors are used? What exactly is a typical “red” or a
 * “green”? Doesn’t every human have a slightly different
 * feeling what a “typical” red or a “typical” blue is? We
 * need a <em>focal color</em>, which is, according to
 * <a href="https://www.oxfordreference.com/display/10.1093/oi/authority.20110803095825870">
 * Oxford Reference</a>:
 *
 * > “A colour that is a prototypical instance of a particular colour name,
 * > such as a shade of red that a majority of viewers consider to be the
 * > best example of a red colour.”
 *
 * The <a href="https://www1.icsi.berkeley.edu/wcs/">World Color Survey</a>
 * (WCS) is a significant study about focal colors of speakers of different
 * languages across the world. The data from this survey is available online,
 * and while it does not provide direct values for focal colors, various
 * studies have used this data to determine focal colors for some
 * <em>basic color terms</em> and a naming centroid for others.
 *
 * The table below shows the WCS grid coordinates for the basic color terms
 * along with the corresponding Cielab values for the focal color (where
 * available) or the naming centroid (where focal color data is unavailable).
 *
 * |Basic color term|WCS grid coordinates|Cielab³ L|Cielab³ a|Cielab³ b|
 * | :--------------| -----------------: | ------: | ------: | ------: |
 * | white¹         |                 A0 |   96.00 |   -0.06 |    0.06 |
 * | black¹         |                 J0 |   15.60 |   -0.02 |    0.02 |
 * | red¹           |                 G1 |   41.22 |   61.40 |   17.92 |
 * | yellow¹        |                 C9 |   81.35 |    7.28 |  109.12 |
 * | green¹         |                F17 |   51.57 |  -63.28 |   28.95 |
 * | blue¹          |                F29 |   51.57 |   -3.41 |  -48.08 |
 * | brown³         |                 G7 |   41.22 |   17.04 |   45.95 |
 * | purple³        |                G34 |   41.22 |   33.08 |  -30.50 |
 * | pink³          |                 E1 |   61.70 |   49.42 |   18.23 |
 * | orange³        |                 E6 |   61.70|    29.38 |   64.40 |
 * | gray           |      not available |         |         |         |
 *
 * ¹ Focal color as proposed by
 *   <a href="https://www.pnas.org/doi/10.1073/pnas.0503281102">Focal colors
 *   are universal after all</a>.
 *
 * ² Raw estimation of the naming centroid based on Fig. 4 of
 *   <a href="https://sites.socsci.uci.edu/~kjameson/ECST/Kay_Cook_WorldColorSurvey.pdf">
 *   this document</a>. (Fig. 5 would be the better choice, as it gives the
 *   focal color instead of the naming centroid, but unfortunately contains
 *   only red, yellow, green and blue, for which we have yet direct data.)
 *
 * ³ <a href="https://www1.icsi.berkeley.edu/wcs/data/cnum-maps/cnum-vhcm-lab-new.txt">
 *   Lookup table providing Lab values for WCS grid coordinates</a> and the
 *   <a href="https://www1.icsi.berkeley.edu/wcs/data/cnum-maps/cnum-vhcm-lab-new-README.txt">
 *   corresponding explanation</a>.
 *
 * From this data, the colors in our swatch grid have been derived as follows:
 * - The gray axis has been defined manually, ignoring the WCS data. Chroma
 *   is 0. The lightness is 100% for white, 0% for black, and 75%, 50%,
 *   and 25% for the intermediate grays.
 * - The other columns for chromatic colors use the WCS data for the swatch in
 *   the middle. Tints and shades are calculated by adding or reducing chroma
 *   and lightness within the Oklab color space. If the resulting color falls
 *   outside the color space, a nearby in-gamut color is chosen instead.
 *
 * @param colorSpace The color space in which the return value is calculated.
 *
 * @returns Swatch grid derived from the basic colors. Provides as a list of
 * basic colors (in this order: red, orange, yellow, green, blue, purple, pink,
 * brown, gray axis). Each basic color is a list of 5 swatches (starting with
 * the lightest and finishing with the darkest: 2 tints, the tone itself,
 * 2 shades).
 *
 * @note The RGB value is rounded to full integers in the range [0, 255]. */
QColorArray2D wcsBasicColors(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
{
    constexpr GenericColor red{41.22, 61.40, 17.92};
    constexpr GenericColor orange{61.70, 29.38, 64.40};
    constexpr GenericColor yellow{81.35, 07.28, 109.12};
    constexpr GenericColor green{51.57, -63.28, 28.95};
    constexpr GenericColor blue{51.57, -03.41, -48.08};
    constexpr GenericColor purple{41.22, 33.08, -30.50};
    constexpr GenericColor pink{61.70, 49.42, 18.23};
    constexpr GenericColor brown{41.22, 17.04, 45.95};
    constexpr std::array<GenericColor, 8> chromaticCielabColors //
        {{red, orange, yellow, green, blue, purple, pink, brown}};

    // Lowest common denominator of QList‘s and std::array’s size types:
    using MySizeType = quint8;

    constexpr MySizeType columnCount = //
        chromaticCielabColors.size() + 1; // + 1 for gray axis
    constexpr auto rowCount = 5;
    QColorArray2D wcsSwatches{columnCount, rowCount};

    // Chromatic colors
    constexpr double strongTint = 0.46;
    constexpr double weakTint = 0.23;
    constexpr double weakShade = 0.18;
    constexpr double strongShade = 0.36;
    std::array<GenericColor, rowCount> tintsAndShades;
    for (MySizeType i = 0; i < chromaticCielabColors.size(); ++i) { //
        const auto oklch = AbsoluteColor::convert( //
                               ColorModel::CielabD50, //
                               chromaticCielabColors.at(i),
                               ColorModel::OklchD65 //
                               )
                               .value_or(GenericColor());
        tintsAndShades[0] = GenericColor //
            {oklch.first + (1 - oklch.first) * strongTint, //
             oklch.second * (1 - strongTint), //
             oklch.third};
        tintsAndShades[1] = GenericColor //
            {oklch.first + (1 - oklch.first) * weakTint, //
             oklch.second * (1 - weakTint), //
             oklch.third};
        tintsAndShades[2] = oklch;
        tintsAndShades[3] = GenericColor //
            {oklch.first * (1 - weakShade), //
             oklch.second * (1 - weakShade), //
             oklch.third};
        tintsAndShades[4] = GenericColor //
            {oklch.first * (1 - strongShade), //
             oklch.second * (1 - strongShade), //
             oklch.third};
        for (MySizeType j = 0; j < rowCount; ++j) {
            const auto variationCielchD50 = AbsoluteColor::convert( //
                                                ColorModel::OklchD65, //
                                                tintsAndShades.at(j), //
                                                ColorModel::CielchD50 //
                                                )
                                                .value_or(GenericColor());
            const auto variationRgb = colorSpace->fromCielchD50ToQRgbBound( //
                variationCielchD50);
            wcsSwatches.setValue(i, //
                                 j,
                                 variationRgb);
        }
    }

    // Gray axis
    QList<double> lightnesses{1, 0.75, 0.5, 0.25, 0};
    for (int j = 0; j < lightnesses.count(); ++j) {
        const GenericColor myOklab{lightnesses.at(j), 0, 0};
        const auto cielchD50 = AbsoluteColor::convert( //
                                   ColorModel::OklabD65, //
                                   myOklab, //
                                   ColorModel::CielchD50 //
                                   )
                                   .value_or(GenericColor());
        const auto rgb = colorSpace->fromCielchD50ToQRgbBound(cielchD50);
        wcsSwatches.setValue(columnCount - 1, j, rgb);
    }

    return wcsSwatches;
}

/**
 * @brief The rendering intents supported by the LittleCMS library.
 *
 * Contains all rendering intents supported by the LittleCMS library
 * against which this we are currently linking (or by one of the
 * LittleCMS library plugigs). Each entry contains the code and the
 * description (english-language, might be empty) just as
 * provided by LittleCMS’ <tt>cmsGetSupportedIntents()</tt>.
 *
 * Note that LittleCMS supports as built-in intents the four official
 * ICC intents and also some other, non-ICC intents. Furthermore,
 * LittleCMS plugins can provide even more intents. As of LittleCMS 2.13
 * the following built-in intents are available:
 *
 * | Type    | Macro name                                    | Code |
 * | :------ | :-------------------------------------------- | ---: |
 * | ICC     | INTENT_PERCEPTUAL                             |    0 |
 * | ICC     | INTENT_RELATIVE_COLORIMETRIC                  |    1 |
 * | ICC     | INTENT_SATURATION                             |    2 |
 * | ICC     | INTENT_ABSOLUTE_COLORIMETRIC                  |    3 |
 * | Non-ICC | INTENT_PRESERVE_K_ONLY_PERCEPTUAL             |   10 |
 * | Non-ICC | INTENT_PRESERVE_K_ONLY_RELATIVE_COLORIMETRIC  |   11 |
 * | Non-ICC | INTENT_PRESERVE_K_ONLY_SATURATION             |   12 |
 * | Non-ICC | INTENT_PRESERVE_K_PLANE_PERCEPTUAL            |   13 |
 * | Non-ICC | INTENT_PRESERVE_K_PLANE_RELATIVE_COLORIMETRIC |   14 |
 * | Non-ICC | INTENT_PRESERVE_K_PLANE_SATURATION            |   15 |
 *
 * @returns A QMap of key-value pairs. The key is the code representing the
 * rendering intent. The value is a QString with the description.
 */
QMap<cmsUInt32Number, QString> lcmsIntentList()
{
    // Thread-Safe Lazy Initialization: Starting with C++11, function-local
    // static variables are guaranteed to be thread-safe during initialization.
    static const QMap<cmsUInt32Number, QString> result = []() {
        QMap<cmsUInt32Number, QString> lambdaResult;
        const cmsUInt32Number intentCount = //
            cmsGetSupportedIntents(0, nullptr, nullptr);
        cmsUInt32Number *codeArray = new cmsUInt32Number[intentCount];
        char **descriptionArray = new char *[intentCount];
        cmsGetSupportedIntents(intentCount, codeArray, descriptionArray);
        for (cmsUInt32Number i = 0; i < intentCount; ++i) {
            lambdaResult.insert(codeArray[i], QString::fromUtf8(descriptionArray[i]));
        }
        delete[] codeArray;
        delete[] descriptionArray;
        return lambdaResult;
    }();
    return result;
}

/**
 * @brief Sets the opacity of a color to fully opaque.
 *
 * @param color The input color.
 *
 * @return The same color with its alpha channel set to maximum opacity.
 */
QColor toOpaque(const QColor &color)
{
    auto temp = color;
    if (temp.alphaF() != 1) {
        temp.setAlphaF(1);
    }
    return temp;
}

/**
 * @brief Makes all colors in the array fully opaque.
 *
 * @param array The input array of colors.
 *
 * @return The same array with the alpha channel of each valid color set to
 * full opacity.
 */
QColorArray2D toOpaque(const QColorArray2D &array)
{
    auto result = array;
    for (qsizetype i = 0; i < result.iCount(); ++i) {
        for (qsizetype j = 0; j < result.iCount(); ++j) {
            const auto temp = result.value(i, j);
            result.setValue(i, j, toOpaque(temp));
        }
    }
    return result;
}

/**
 * @brief Splits a number of elements into segments with a tapered
 * distribution.
 *
 * Divides a total of <tt>elementCount</tt> elements (indexed from 0
 * to elementCount - 1) into <tt>segmentCount</tt> contiguous segments. The
 * distribution produces larger segments near the beginning and end of the
 * range, and smaller segments near the center, based on a piecewise linear
 * weighting function.
 *
 * Segment sizes are determined proportionally to a linear slope that decreases
 * toward <tt>peak</tt> from both ends. The center point of this tapering
 * effect is defined by <tt>peak</tt>. All segment start indices are aligned to
 * multiples of <tt>alignment</tt>, and all elements are covered without gaps
 * or overlap.
 *
 * This method is fast, deterministic, and suitable for scenarios where a
 * non-uniform but predictable distribution is desired.
 *
 * @param elementCount Total number of elements to divide. Must be ≥ 0.
 * @param segmentCount Number of segments to produce. Must be ≥ 1.
 * @param alignment Alignment step for the start index of each segment. Each
 *        segment starts at a multiple of this value. Must be ≥ 1.
 * @param peak Normalized center point of the distribution (0 < peak < 1),
 *        where segment size reaches its minimum. Use 0.5 for a symmetric
 *        tapering. Values closer to 0 or 1 shift the smallest segments
 *        accordingly.
 *
 * @return A list of segment ranges, where each pair represents the
 *         [start, end] index (inclusive) of a segment. The ranges are
 *         disjoint, aligned, and collectively span the full element range.
 */
QList<QPair<int, int>> splitElementsTapered(int elementCount, int segmentCount, int alignment, double peak)
{
    QList<QPair<int, int>> result;

    // Enforce preconditions
    if (elementCount < 1) {
        return result;
    }
    if (segmentCount < 1) {
        segmentCount = 1;
    }
    if (alignment < 1) {
        alignment = 1;
    }
    if ((peak <= 0) || (peak >= 1)) {
        peak = 0.5;
    }

    const double firstFactor = -1 / peak;
    constexpr double firstOffset = 1;
    const double secondFactor = 1 / (1 - peak);
    const double secondOffset = secondFactor * peak * (-1);

    QList<double> relativeSegmentSize;
    relativeSegmentSize.reserve(segmentCount);
    for (int i = 0; i < segmentCount; ++i) {
        const double x = (i + 0.5) / segmentCount;
        if (x < peak) {
            relativeSegmentSize.append(x * firstFactor + firstOffset);
        } else if (x > peak) {
            relativeSegmentSize.append(x * secondFactor + secondOffset);
        } else {
            relativeSegmentSize.append(0);
        }
    }
    double total = 0;
    for (const auto value : std::as_const(relativeSegmentSize)) {
        // cppcheck-suppress useStlAlgorithm
        total += value;
    }
    // NOTE “total” might be 0 if we have only 1 element which takes ist value
    // exactly from the peak, which has value 0 (or maybe if the
    // floating point precision is not enough to distinguish the value from 0).

    const double factor = (total > 0.0000001) ? elementCount / total : 1;

    int nextFirstElement = 0;
    for (int i = 0; i < segmentCount; ++i) {
        const int firstElement = nextFirstElement;
        nextFirstElement = roundToNearestMultiple( //
            firstElement + relativeSegmentSize[i] * factor,
            alignment);
        if (nextFirstElement <= firstElement) {
            // Make sure to not have an empty segment.
            nextFirstElement = firstElement + alignment;
        }
        const int lastElement = qMin(nextFirstElement - 1, elementCount - 1);
        if (lastElement < firstElement) {
            break;
        }
        result.append(QPair<int, int>(firstElement, lastElement));
    }
    if (result.last().second < elementCount - 1) {
        // Might happen is “total” was 0.
        result.last().second = elementCount - 1;
    }

    return result;
}

} // namespace PerceptualColor
