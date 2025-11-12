// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef COLORDIALOG_H
#define COLORDIALOG_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qcolor.h>
#include <qcolordialog.h>
#include <qdialog.h>
#include <qglobal.h>
#include <qmetatype.h>
#include <qnamespace.h>
#include <qsharedpointer.h>
#include <qstring.h>
class QEvent;
class QObject;
class QShowEvent;
class QWidget;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class ColorDialogPrivate;

class RgbColorSpace;

/** @brief A perceptually uniform color picker dialog
 *
 * The color dialog’s function is to allow users to choose colors intuitively.
 * For example, you might use this in a drawing program to allow the user to
 * set the brush color.
 *
 * At difference to QColorDialog, this
 * dialog’s graphical components are perceptually uniform and therefore more
 * intuitive. It’s internally based on the LCH color model, which does reflect
 * the human perception much better than RGB or its transforms like HSV. At
 * the same time, this dialog does not require the user itself to know
 * anything about LCH at all, because the graphical representations is
 * intuitive.
 *
 * @image html ColorDialog.png "ColorDialog"
 * @image html ColorDialogTab1.png "ColorDialog"
 * @image html ColorDialogTab2.png "ColorDialog"
 *
 * It is an @ref qdialogsourceompatibility "mostly source-compatible"
 * replacement for QColorDialog. It also adds some extra functionality
 * that is not available in QColorDialog.
 *
 * Just as with QColorDialog, the static functions provide a modal color
 * dialog. The static getColor() function shows the dialog, and allows
 * the user to specify a color:
 *
 * @snippet testcolordialog.cpp ColorDialog Get color
 *
 * The function can also be used to let
 * users choose a color with a level of transparency: pass the alpha channel
 * option as an additional argument:
 *
 * @snippet testcolordialog.cpp ColorDialog Get color with alpha channel
 *
 * @image html ColorDialogAlpha.png "ColorDialog with alpha channel"
 *
 * More features:
 *
 * - A screen color picker is provided on many platforms.
 *
 * - For a non-modal dialog, use the normal constructors of this class.
 *
 * - The default window title is <em>Select Color</em>, and not the title of
 * your application. It can of course be customized with
 * <tt>QWidget::setWindowTitle()</tt>. The window title will
 * <em>not</em> be updated on <tt>QEvent::LanguageChange</tt> events.
 *
 * - At difference to the usual native platform color dialog, <em>this</em>
 * dialog can be resized. That makes sense, because it allows to see better
 * the gamut image. Therefore, this dialog is by default bigger than the usual
 * native platform color dialog. You can of course customize the dialog
 * size with QWidget::resize() or force a more space-saving layout through
 * the @ref layoutDimensions property.
 *
 * - The @ref ColorPatch that indicates the selected color is placed prominently
 * at the top of the widget. That is also useful for touch screens as the
 * @ref ColorPatch will not be hidden by the hand of the user when the user
 * is touching the above color selection widgets.
 *
 * - This dialog uses icons. See @ref hidpisupport "High DPI support"
 * about how to enable support for high-DPI icons.
 *
 * @note @anchor qdialogsourceompatibility The API of this class is mostly
 * source-compatible to the API of QColorDialog. This is a list of
 * incompatibilities:
 * - The constructors and also @ref getColor() require a color space
 *   as argument.
 * - As this dialog does not provide functionality for custom colors and
 *   standard color, the corresponding static functions of QColorDialog are
 *   not available in this class.
 * - The option <tt>ColorDialogOption::DontUseNativeDialog</tt>
 *   will always remain <tt>false</tt> (even if set explicitly), because it’s
 *   just the point of this library to provide an own, non-native dialog.
 * - While the enum declaration @ref ColorDialogOption itself is aliased
 *   here, this isn't possible for the enum values itself. Therefor, when
 *   working with @ref options, you cannot use <tt>ShowAlphaChannel</tt> but
 *   you have to use the fully qualified identifier (either
 *   <tt>PerceptualColor::ColorDialog::ColorDialogOption::ShowAlphaChannel</tt>
 *   or <tt>QColorDialog::ShowAlphaChannel</tt>, at your option.
 * - Calling @ref setCurrentColor() with colors that
 *   are <em>not</em> <tt>QColor::Spec::Rgb</tt> will lead to an automatic
 *   conversion like QColorDialog does, but at difference to QColorDialog, it
 *   is done with more precision, therefor the resulting
 *   @ref currentColor() might be slightly different. The same is true
 *   for <tt>QColor::Spec::Rgb</tt> types with floating point precision:
 *   While QColorDialog would round to full integers, <em>this</em> dialog
 *   preserves the floating point precision.
 * - When the default constructor is used, unlike QColorDialog, the default
 *   color is <em>not</em> guaranteed to be <tt>Qt::white</tt>.
 *
 * @internal
 *
 * @todo NICETOHAVE The
 * @ref ColorDialogPrivate::m_lchLightnessSelector has a different
 * scale than the @ref ColorDialogPrivate::m_chromaHueDiagram, but both are
 * directly side-by-side in the dialog. Would it make sense to use the
 * same scale for both?
 *
 * @todo NICETOHAVE Provide <tt>setResizable(bool resizable)</tt>. Do not
 * provide a property, because in the background this is setting <em>two</em>
 * different values, which might be conflicting in the moment when read
 * access is done. Code within ColorDialog to enable:
 * <tt>layout()->setSizeConstraint(QLayout::SetFixedSize);
 * setSizeGripEnabled(false);</tt>
 * Document the performance impact: When resizing is allowed (which is the
 * default), the user can make the window very big, which might increase
 * the rendering time considerable.
 *
 * @todo NICETOHAVE In KDE systemsettings, in “Fonts” settings, there are small
 * buttons showing a “i” symbol (for “information”) that has no functionality,
 * but a tooltip for the widget at the left. Would this be also good for
 * @ref ColorDialog?
 * The button does nothing when it’s clicked, but when hovering with the mouse,
 * it shows the tooltip. This might be interesting at least for the tooltip of
 * the color-space information, which is quite big. And could this help to make
 * tooltip information available for touch-screen users, maybe displaying them
 * when the user clicks the button?
 *
 * @todo NICETOHAVE KDE merged something that displays tooltips and integrates a
 * link that calls the <tt>QWhatsThis</tt> which has more information. Would
 * this be helpful here also?
 *
 * @todo NICETOHAVE Provide (on demand) two patches,
 * like Scribus also does: One for the
 * old color (cannot be modified by the user) and another one for the new
 * color (same behavior as the yet existing color patch). One could be
 * named “before” and the other “after”. Or maybe make this configurable?
 * And put an arrow between the patches, from “before” to “after”. (Be aware:
 * RTL support necessary!)
 *
 * @todo NICETOHAVE Provide more <tt>tooltip()</tt> help for widgets. For
 * @ref WheelColorPicker and @ref ChromaLightnessDiagram, this
 * help text could describe the keyboard controls and be integrated
 * as default value in the class itself. For the other widgets, a
 * help text could be defined here within <em>this</em> class,
 * if appropriate.
 *
 * @todo NICETOHAVE Touch screen compatibility:
 * A spin box can also be used on mobile phone (putting the numbers
 * with on-screen keyboard). But the + and - button for increasing
 * or decreasing the values might be too small. And mobile UI uses
 * often wheels for this use case…
 *
 * @todo NICETOHAVE The QLineEdit for the hexadecimal RGB values: Maybe it
 * could even be switched to @ref MultiSpinBox (but that would mean implement
 * support for hexadecimal digits in @ref MultiSpinBox).
 *
 * @todo NICETOHAVE Accept <tt>F5</tt> and <tt>Ctrl+R</tt> just with the same
 * functionality as the gamut button in the HCL @ref MultiSpinBox.
 * But attention: If a library user <em>embeds</em> the dialog, he does
 * not want his shortcuts to be intercepted!
 *
 * @todo NICETOHAVE It might be nice to
 * support keyboard shortcuts for switching tabs
 * like in browsers, which is a concept many users might be familiar to.
 * Crtl+Tab to switch to the next tab in the list. Crtl+Shift+Tab to switch
 * to the previous tab in the list.
 *
 * @todo NICETOHAVE Start with dialog with Qt::yellow RGB 255 255 0. You get
 * CIELchD50 98% 95 100°. Change the value, than enter manually
 * CIELchD50 98% 95 100°. Expected result: Ideally there would be no rounding
 * difference at all and we get RGB 255 255 0. Actual result:
 * CIELchD50 98% 95 100° is marked as out-of-gamut. The correspondingly used
 * in-gamut value is RGB 255 251 202, which is really quite far from
 * Qt::yellow RGB 255 255 0 (much more pale).
 *
 * @todo SHOULDHAVE Provide an overloaded version of @ref open() that accepts
 * arguments for new-style connect statements, making use of compiler
 * checks.
 *
 * @todo SHOULDHAVE Use the <em>actual</em> color profile of the monitor.
 *
 * @todo NICETOHAVE
 * The LCh-hue (and so the graphical widgets) jumps forward and backward
 * when changing RGB-based values (also HSV) when entering and leaving the gray
 * axis, due to lack of hue information. Would it be an option to store the
 * old hue to get a meaningful hue?
 * Should it be only really for the gray axis, or allow a certain tolerance
 * around the gray axis is necessary to make this work well - and if so,
 * how much tolerance? Would it be useful to define a certain hue, for
 * example 0°, as default hue for when no old hue is available but the
 * new value is on the gray axis?
 *
 * @todo NICETOHAVE Support for other models like
 * Munsell? With an option to enable or disable them? (NCS not, because
 * it is not free…)
 *
 * @todo SHOWSTOPPER “Hijack screen picker” should be optional and
 * off-by-default
 *
 * @todo NICETOHAVE Add more @ref MultiSpinBox for
 * Oklab (feature parity with CSS Color 5),
 * Cielab (feature parity with CSS Color 5), Okhsl?
 *
 * @todo NICETOHAVE Info about the whitepoint (D50, D65…) in the tooltip for
 * the ICC profile. And about the mean or average gamma, CIE’s shoe sole
 * diagram for gamut visualisation, gamma curve diagrams… At least
 * @ref RgbColorSpace::profileTagBlackpoint,
 * @ref RgbColorSpace::profileTagWhitepoint,
 * @ref RgbColorSpace::profileTagRedPrimary,
 * @ref RgbColorSpace::profileTagGreenPrimary,
 * @ref RgbColorSpace::profileTagBluePrimary are yet available. Since
 * LittleCMS 2.13 also <tt>cmsDetectRGBProfileGamma()</tt> is available.
 *
 * @todo NICETOHAVE Restore the previous window geometry when the dialog is
 * shown? We restore already the chosen tab and swatchbook page, but not the
 * window geometry. Would it make sense to do so app-wise or system-wise?
 * Would this work on Wayland at all, or maybe only for the window size, but
 * not for the window position? Anyway, the native QColorDialog does’t to
 * that.
 *
 * @todo SHOULDHAVE NICETOHAVE QMimeData::setColorData() is used by
 * QColorDialog? For clipbord or drag-and-drop? Accept it in @ref ColorDialog
 * for compatibility?
 *
 * @todo NICETOHAVE A property in @ref ColorDialog that substitutes
 * @ref ColorDialogPrivate::decimals and so provides an API to change this
 * within a reasonably range like [0..2].
 *
 * @todo NICETOHAVE Implement more of the <tt>QColorDialog</tt> API here.
 *
 * @note The swatch book has the basic color page, which provides a perceptual
 * color palette. Commom palettes like QColorDialog’s standard colors or the
 * <a href="https://www.w3.org/TR/css-color-4/#named-colors">CSS colors</a>
 * or the
 * <a href="https://en.wikipedia.org/wiki/Web_colors#Web-safe_colors">
 * Web-safe colors</a> do
 * not make sense, since we provide a perceptual color dialog,
 * which allows things much beyond the 216 web-save colors, and 216 colors
 * isn’t a useful standard anymore, and not a nice swatch grid either.
 */
class PERCEPTUALCOLOR_IMPORTEXPORT ColorDialog : public QDialog
{
    Q_OBJECT

    /** @brief The currently selected color in the dialog.
     *
     * This property represents the selected color using the dialog’s
     * working color space (gamut), with a precision of 16 bits per channel.
     *
     * @invariant The signal @ref currentColorChanged() is emitted if and
     * only if the value of this property changes.
     *
     * @note The setter @ref setCurrentColor() does not accept all
     * possible <tt>QColor</tt> values. Refer to its documentation for
     * specific constraints.
     *
     * @sa READ @ref currentColor() const
     * @sa WRITE @ref setCurrentColor()
     * @sa NOTIFY @ref currentColorChanged()
     *
     * @internal
     *
     * @invariant <tt>QColor::isValid()</tt> is guaranteed to
     * return <tt>true</tt>, and <tt>QColor::spec()</tt> is
     * always <tt>QColor::Spec::Rgb</tt>.
     */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged USER true)

    /** @brief Layout dimensions
     *
     * Defines if the dialog uses a rather collapsed (small) or a rather
     * expanded (large) layout. In both cases, all elements are present.
     * But for the collapsed variant, more elements are put in
     * tab widgets, while for the expanded variant, more elements are
     * visible at the same time.
     *
     * @image html ColorDialogCollapsed.png "collapsed"
     * @image html ColorDialogExpanded.png "expanded"
     *
     * Default value:
     * @snippet src/colordialog_p.h layoutDimensionsDefaultValue
     *
     * When the layout dimension effectively changes, also the dialog size
     * is adapted.
     *
     * @sa @ref DialogLayoutDimensions
     * @sa READ @ref layoutDimensions() const
     * @sa WRITE @ref setLayoutDimensions()
     * @sa NOTIFY @ref layoutDimensionsChanged
     *
     * @internal
     *
     * @sa @ref ColorDialogPrivate::m_layoutDimensionsEffective
     *
     * @todo SHOULDHAVE Remove this property? Instead,
     * implement a truly convertible
     * layout: Show/hide the widget for numbers depending on dialog size,
     * and maybe even re-arrange even more of the widgets with varying
     * sizes. */
    Q_PROPERTY(DialogLayoutDimensions layoutDimensions READ layoutDimensions WRITE setLayoutDimensions NOTIFY layoutDimensionsChanged)

    /** @brief Various options that affect the look and feel of the dialog
     *
     * These are the same settings as for QColorDialog. For compatibility
     * reasons, they are also of the same type: @ref ColorDialogOptions
     *
     * | Option              | Default value | Description
     * | :------------------ | :------------ | :----------
     * | ShowAlphaChannel    | false         | Allow the user to select the alpha component of a color.
     * | NoButtons           | false         | Don't display OK and Cancel buttons. (Useful for “live dialogs”.)
     * | NoEyeDropperButton  | false         | Hide the Eye Dropper button. This value was added in Qt 6.6.
     * | DontUseNativeDialog | true          | Use Qt’s standard color dialog instead of the operating system native color dialog.
     *
     *   @invariant The option <tt>ColorDialogOption::DontUseNativeDialog</tt>
     *   will always be <tt>true</tt> because it’s just the point of
     *   this library to provide an own, non-native dialog. (If you
     *   set  <tt>ColorDialogOption::DontUseNativeDialog</tt> explicitly
     *   to <tt>false</tt>, this will silently be ignored, while the
     *   other options that you might have set, will be correctly applied.)
     *
     * Example:
     * @snippet testcolordialog.cpp setOptionsWithLocalEnum
     * Or:
     * @snippet testcolordialog.cpp setOptionsWithQColorDialogEnum
     * @note At difference to QColorDialog, you need a fully qualified
     * identifier for the enum values. The following code would therefore
     * fail:<br/>
     * <tt>myDialog->setOption(ShowAlphaChannel, false);</tt>
     *
     * @sa READ @ref options() const
     * @sa @ref testOption()
     * @sa WRITE @ref setOptions()
     * @sa @ref setOption()
     * @sa NOTIFY @ref optionsChanged()*/
    Q_PROPERTY(ColorDialogOptions options READ options WRITE setOptions NOTIFY optionsChanged)

public:
    /** @brief Local alias for QColorDialog::ColorDialogOption
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    using ColorDialogOption = QColorDialog::ColorDialogOption;
    /** @brief Local alias for QColorDialog::ColorDialogOptions
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    using ColorDialogOptions = QColorDialog::ColorDialogOptions;
    /** @brief Layout dimensions
     *
     * This enum is declared to the meta-object system with <tt>Q_ENUM</tt>.
     * This happens automatically. You do not need to make any manual calls.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    enum class DialogLayoutDimensions {
        ScreenSizeDependent, /**< Decide automatically between
            <tt>collapsed</tt> and <tt>expanded</tt> layout: <tt>collapsed</tt>
            is used on small screens, and <tt>expanded</tt> on big screens. The
            decision is based on the screen size of the <em>default screen</em>
            of the widget (see <tt>QGuiApplication::primaryScreen()</tt> for
            details). The decision is evaluated at the moment when setting this
            value, and again each time the widget is shown again. It is
            <em>not</em> evaluated again when a yet existing dialog is just
            moved to another screen. */
        Collapsed, /**< Use the small, “collapsed“ layout of this dialog. */
        Expanded /**< Use the large, “expanded” layout of this dialog.  */
    };
    Q_ENUM(DialogLayoutDimensions)
    Q_INVOKABLE explicit ColorDialog(QWidget *parent = nullptr);
    Q_INVOKABLE explicit ColorDialog(const QColor &initial, QWidget *parent = nullptr);
    Q_INVOKABLE explicit ColorDialog(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                                     const QString &gamutIdentifier,
                                     QWidget *parent = nullptr);
    Q_INVOKABLE explicit ColorDialog(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                                     const QString &gamutIdentifier,
                                     const QColor &initial,
                                     QWidget *parent = nullptr);
    virtual ~ColorDialog() noexcept override;
    /** @brief Getter for property @ref currentColor
     *  @returns the property @ref currentColor */
    [[nodiscard]] QColor currentColor() const;
    [[nodiscard]] static QColor
    getColor(const QColor &initial = Qt::white, QWidget *parent = nullptr, const QString &title = QString(), ColorDialogOptions options = ColorDialogOptions());
    [[nodiscard]] static QColor getColor(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                                         const QString &gamutIdentifier,
                                         const QColor &initial = Qt::white,
                                         QWidget *parent = nullptr,
                                         const QString &title = QString(),
                                         ColorDialogOptions options = ColorDialogOptions());
    /** @brief Getter for property @ref layoutDimensions
     *  @returns the property @ref layoutDimensions */
    [[nodiscard]] ColorDialog::DialogLayoutDimensions layoutDimensions() const;
    // Make sure not to override the base class’s “open“ function:
    using QDialog::open;
    Q_INVOKABLE void open(QObject *receiver, const char *member);
    /** @brief Getter for property @ref options
     * @returns the current @ref options */
    [[nodiscard]] ColorDialogOptions options() const;
    [[nodiscard]] Q_INVOKABLE QColor selectedColor() const;
    virtual void setVisible(bool visible) override;
    [[nodiscard]] Q_INVOKABLE bool testOption(PerceptualColor::ColorDialog::ColorDialogOption option) const;

public Q_SLOTS:
    void setCurrentColor(const QColor &color);
    void setLayoutDimensions(const PerceptualColor::ColorDialog::DialogLayoutDimensions newLayoutDimensions);
    Q_INVOKABLE void setOption(PerceptualColor::ColorDialog::ColorDialogOption option, bool on = true);
    void setOptions(PerceptualColor::ColorDialog::ColorDialogOptions newOptions);

Q_SIGNALS:
    /** @brief This signal is emitted just after the user has clicked OK to
     * select a color to use.
     *  @param color the chosen color */
    void colorSelected(const QColor &color);
    /** @brief Notify signal for property @ref currentColor.
     *
     * This signal is emitted whenever the “current color” changes in the
     * dialog.
     * @param color the new “current color” */
    void currentColorChanged(const QColor &color);
    /** @brief Notify signal for property @ref layoutDimensions.
     * @param newLayoutDimensions the new layout dimensions */
    void layoutDimensionsChanged(const PerceptualColor::ColorDialog::DialogLayoutDimensions newLayoutDimensions);
    /** @brief Notify signal for property @ref options.
     * @param newOptions the new options */
    void optionsChanged(const PerceptualColor::ColorDialog::ColorDialogOptions newOptions);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual void done(int result) override;
    virtual void showEvent(QShowEvent *event) override;

private:
    Q_DISABLE_COPY(ColorDialog)

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ColorDialogPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ColorDialogPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestColorDialog;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ColorDialog::ColorDialogOption)
Q_DECLARE_METATYPE(PerceptualColor::ColorDialog::ColorDialogOptions)
Q_DECLARE_METATYPE(PerceptualColor::ColorDialog::DialogLayoutDimensions)

#endif // COLORDIALOG_H
