// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helper.h"

#include <qcolor.h>
#include <qevent.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qstringliteral.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qlist.h>
#else
#include <qstringlist.h>
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
 * However, some languages like Turkish put the percent sign <em>before</em>
 * the value, which is not possible with the above code. This function allows
 * to do this feature easily in a fail-safe way:
 * @snippet testhelper.cpp percentFullyInternationalized
 *
 * @param formatString The translated value string, which should contain
 * exactly <em>one</em> place marker as described in <tt>QString::arg()</tt>
 * like <tt>&amp;1</tt> or <tt>&amp;L1</tt>. This place marker represents
 * the value. Example: “Prefix&amp;1Suffix”. Prefix and suffix may be empty.
 * @param fallbackPrefix English fallback prefix.
 * @param fallbackSuffix English fallback suffix.
 *
 * @returns At <tt>QPair::first</tt> the prefix of the first place marker,
 * and at <tt>QPair::second</tt> the suffix. If <tt>formatString</tt> does
 * not contain any place markers (for example because of a broken translation),
 * the English <tt>fallbackPrefix</tt> and <tt>fallbackSuffix</tt> are used
 * instead. */
[[nodiscard]] QPair<QString, QString> valuePrefixSuffix(const QString &formatString, const QString &fallbackPrefix, const QString &fallbackSuffix)
{
    static const QString separator = //
        QStringLiteral("Just a string unlikely to occur in translations.");
    const auto temp = formatString //
                          .arg(separator) //
                          .split(separator, Qt::KeepEmptyParts);
    if (temp.count() == 2) {
        return QPair<QString, QString>(temp.value(0), temp.value(1));
    }
    return QPair<QString, QString>(fallbackPrefix, fallbackSuffix);
}

} // namespace PerceptualColor
