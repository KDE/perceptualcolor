// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SCREENCOLORPICKER
#define SCREENCOLORPICKER

#include <optional>
#include <qglobal.h>
#include <qpointer.h>
#include <qstring.h>
#include <qwidget.h>
class QColorDialog;
class QPushButton;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qtmetamacros.h>
#else
#include <qmetatype.h>
#include <qobjectdefs.h>
class QObject;
#endif

namespace PerceptualColor
{

/** @internal
 *
 * @brief Pick a color from the screen.
 *
 * Provides an interface to let the user pick a color from the screen.
 *
 * This feature is not available on all platforms. Use @ref isAvailable()
 * to check it.
 *
 * @warning On some platforms, QColorDialog is used to perform the color
 * picking. This might mix up the default button setting of the parent dialog.
 * Workaround: If using default buttons in a parent dialog, reimplement
 * <tt>QWidget::setVisible()</tt> in this parent dialog: Call the
 * parent’s class implementation, and <em>after</em> that, call
 * <tt>QPushButton::setDefault(true)</tt> on the default button. */
class ScreenColorPicker : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenColorPicker(QWidget *parent);
    virtual ~ScreenColorPicker() override;
    [[nodiscard]] bool isAvailable();

public Q_SLOTS:
    void startPicking(quint8 previousColorRed, quint8 previousColorGreen, quint8 previousColorBlue);

Q_SIGNALS:
    /** @brief A new color.
     *
     * Emitted when the user has clicked on the screen to select a new color.
     *
     * @note On some platforms, furthermore this signal is also emitted while
     * the user hovers over the screen with the mouse. Than, if the user
     * cancels with the ESC key, a new signal is emitted with the old color
     * passed originally to @ref startPicking.
     *
     * @param red The <em>red</em> component of the new color.
     *            Range: <tt>[0, 255]</tt>
     * @param green Same for green.
     * @param blue Same for blue.
     *
     * @internal
     *
     * @note This signal uses integers with the range <tt>[0, 255]</tt> as
     * return values because this is the maximum precision of the underlying
     * implementation: The QColorDialog implementation rounds on this
     * precision when the user pushes the ESC key, even if the previous
     * value was more exact. */
    // Choosing thre “double” values as return type, as it makes clear
    // what data type returns and as “Portal” actually provides
    // double-precision in its return values.
    void newColor(double red, double green, double blue);

private:
    void pickWithPortal();
    [[nodiscard]] static bool hasPortalSupport();
    void initializeQColorDialogSupport();
    [[nodiscard]] static bool queryPortalSupport();
    [[nodiscard]] static QString translateViaQColorDialog(const char *sourceText);
    /** @brief If on the current platform there is support for
     * QColorDialog-based screen color picking.
     *
     * Might hold an empty value if @ref initializeQColorDialogSupport has
     * never been called. */
    static inline std::optional<bool> m_hasQColorDialogSupport = std::nullopt;
    /** @brief The hidden QColorDialog widget (if any).
     *
     * @sa @ref initializeQColorDialogSupport */
    QPointer<QColorDialog> m_qColorDialog;
    /** @brief The screen-color-picker button of the hidden QColorDialog widget
     * (if any).
     *
     * @sa @ref initializeQColorDialogSupport */
    QPointer<QPushButton> m_qColorDialogScreenButton;

private Q_SLOTS:
    void getPortalResponse(uint exitCode, const QVariantMap &responseArguments);
};

} // namespace PerceptualColor

#endif // SCREENCOLORPICKER
