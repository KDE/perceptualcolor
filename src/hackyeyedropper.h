// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HACKYEYEDROPPER
#define HACKYEYEDROPPER

#include <optional>
#include <qglobal.h>
#include <qpointer.h>
#include <qstring.h>
#include <qwidget.h>
class QColorDialog;
class QPushButton;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Pick a color from the screen.
 *
 * This feature is not always available. Use @ref isAvailable()
 * to check if it is actually available at runtime.
 *
 * @note This class inherits from QWidget and necessarily needs a parent
 * widget. This widget itself stays however invisible. As all QWidget-based
 * classes, in multithread applications, this class may only be used from the
 * main thread.
 *
 * @warning This basically hijacks QColorDialog’s eyedropper,
 * but this relies on internals of Qt and could therefore
 * fail in later Qt versions. Furthermore,
 * <a href="https://bugreports.qt.io/browse/QTBUG-94748">QColorDialog’s
 * eyedropper is broken for multi-monitor setups</a>.
 *
 * @internal
 *
 * @note Making an actual cross-platform eyedropper implementation ourself
 * would be a lot of work. There is
 * also a <a href="https://bugreports.qt.io/browse/QTBUG-109440">request
 * to add a public API to Qt</a> for this.
 *
 * @warning  We do not currently use it in other parts of the library, because
 * it is not mature and at @ref index "main page" we claim that we do not use
 * internal APIs. Making it a public API would break this promise.
 */
class HackyEyedropper : public QWidget
{
    Q_OBJECT

public:
    explicit HackyEyedropper(QWidget *parent);
    virtual ~HackyEyedropper() override;
    [[nodiscard]] bool isAvailable();

public Q_SLOTS:
    void startPicking(int previousColorRed, int previousColorGreen, int previousColorBlue);

Q_SIGNALS:
    /**
     * @brief A new color, measured in an undefined color space.
     *
     * It is not guaranteed (but still likely) that the color is defined in
     * the sRGB color space.
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
     * value was more exact and QColor supports more precision.
     */
    void newColor(int red, int green, int blue);

private:
    /** @internal @brief Only for unit tests. */
    friend class TestHackyEyedropper;

    void initializeQColorDialogSupport();

    [[nodiscard]] static QString translateViaQColorDialog(const char *sourceText);

    /** @brief If there is support for
     * QColorDialog-based eyedropper functionality.
     *
     * Might hold an empty value if @ref initializeQColorDialogSupport has
     * never been called.
     *
     * @warning The declaration as <tt>static in‍line</tt> can be problematic:
     * At least when linking on MSVC against a shared/static library,
     * apparently there are two instances of this variable: One that is used
     * within the shared/dynamic library and another one that is used within
     * the executable that links against this library. While on GCC and Clang
     * this does not happen, maybe this behaviour is implementation-defined.
     * And we do not want to rely on implementation-defined behaviour. However,
     * because the variable is <tt>private</tt>, this won't make any problems
     * under normal circumstances, because it's inaccessible anyway. Only when
     * doing a whitebox test and bypass the private access modifier via the
     * @ref HackyEyedropper::TestHackyEyedropper "friend declaration" for
     * unit tests, you might see the wrong variable and consequently possibly
     * the wrong value. Therefore, unit tests should only access this variable
     * when building against the static library.
     */
    static inline std::optional<bool> m_hasQColorDialogSupport = std::nullopt;

    /**
     * @brief The hidden QColorDialog widget (if any).
     *
     * @sa @ref initializeQColorDialogSupport
     */
    QPointer<QColorDialog> m_qColorDialog;

    /**
     * @brief The eyedropper button of the hidden QColorDialog widget
     * (if any).
     *
     * @sa @ref initializeQColorDialogSupport
     */
    QPointer<QPushButton> m_qColorDialogScreenButton;
};

} // namespace PerceptualColor

#endif // HACKYEYEDROPPER
