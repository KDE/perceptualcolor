// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helper.h"

#include "initializelibraryresources.h"
#include <qchar.h>
#include <qcolor.h>
#include <qevent.h>
#include <qkeysequence.h>
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
 * empty string. */
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
 * @returns An icon from the system icons and or a fallback icons. If none is
 * available, an empty icon. */
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

/** @brief If the the average lightness of a widget rendering is dark.
 *
 * Takes a screenshot of a widget and calculates the average lightness
 * of this screenshot.
 *
 * @param widget The widget to evaluate
 *
 * @returns <tt>std::nullopt</tt> if the widget is <tt>nullptr</tt> or
 * its size is empty. <tt>true</tt> if the average lightness is less than
 * rather dark. <tt>false</tt> otherwise.
 *
 * @note The exact measurement of “lightness” and “dark” is not specified
 * and might change over time. */
std::optional<ColorSchemeType> guessColorSchemeTypeFromWidget(QWidget *widget)
{
    if (widget == nullptr) {
        return std::nullopt;
    }

    // Take a screenshot of the widget
    const QImage screenshot = widget->grab().toImage();
    if (screenshot.size().isEmpty()) {
        return std::nullopt;
    }

    // Calculate the average lightness of the screenshot
    float totalLightness = 0;
    const int pixelCount = screenshot.width() * screenshot.height();
    for (int y = 0; y < screenshot.height(); ++y) {
        for (int x = 0; x < screenshot.width(); ++x) {
            totalLightness += QColor(screenshot.pixel(x, y)).lightnessF();
        }
    }
    const bool isDark = totalLightness / static_cast<float>(pixelCount) < 0.5f;

    if (isDark) {
        return ColorSchemeType::Dark;
    }
    return ColorSchemeType::Light;
}

} // namespace PerceptualColor
