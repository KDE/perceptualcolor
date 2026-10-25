// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORDIALOG_H
#define PERCEPTUALCOLOR_COLORDIALOG_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qcolor.h>
#include <qcolordialog.h>
#include <qdialog.h>
#include <qglobal.h>
#include <qmetatype.h>
#include <qnamespace.h>
#include <qstring.h>
#include <qtmetamacros.h>
class QEvent;
class QObject;
class QShowEvent;
class QWidget;

namespace PerceptualColor
{
class ColorDialogPrivate;

/*!
 * \inmodule ModulePublicApi
 *
 * \brief A perceptually uniform color picker dialog.
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
 * \image ColorDialog.png "ColorDialog"
 * \image ColorDialogTab1.png "ColorDialog"
 * \image ColorDialogTab2.png "ColorDialog"
 *
 * It is a mostly source-compatible
 * replacement for QColorDialog. It also adds some extra functionality
 * that is not available in QColorDialog.
 *
 * Just as with QColorDialog, the static functions provide a modal color
 * dialog. The static \l getColor() function shows the dialog, and allows
 * the user to specify a color:
 *
 * \snippet testcolordialog.cpp ColorDialog Get color
 *
 * The function can also be used to let
 * users choose a color with a level of transparency: pass the alpha channel
 * option as an additional argument:
 *
 * \snippet testcolordialog.cpp ColorDialog Get color with alpha channel
 *
 * \image ColorDialogAlpha.png "ColorDialog with alpha channel"
 */
class PERCEPTUALCOLOR_IMPORTEXPORT ColorDialog : public QDialog
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::ColorDialog::currentColor
     *
     * \brief The currently selected color in the dialog.
     *
     * This property represents the selected color using the dialog’s
     * working color space (gamut), with a precision of 16 bits per channel.
     *
     * \note The setter \l setCurrentColor() does not accept all
     * possible \c {QColor} values. Refer to its documentation for
     * specific constraints.
     */
    Q_PROPERTY(QColor currentColor READ currentColor WRITE setCurrentColor NOTIFY currentColorChanged USER true)

    /*!
     * \property PerceptualColor::ColorDialog::layoutDimensions
     *
     * \brief Layout dimensions
     *
     * Defines if the dialog uses a rather collapsed (small) or a rather
     * expanded (large) layout. In both cases, all elements are present.
     * But for the collapsed variant, more elements are put in
     * tab widgets, while for the expanded variant, more elements are
     * visible at the same time.
     *
     * \image ColorDialogCollapsed.png "collapsed"
     * \image ColorDialogExpanded.png "expanded"
     *
     * Default value:
     *
     * \snippet colordialog_p.h layoutDimensionsDefaultValue
     *
     * When the layout dimension effectively changes, also the dialog size
     * is adapted.
     */
    Q_PROPERTY(DialogLayoutDimensions layoutDimensions READ layoutDimensions WRITE setLayoutDimensions NOTIFY layoutDimensionsChanged)

    /*!
     * \property PerceptualColor::ColorDialog::options
     *
     * \brief Various options that affect the look and feel of the dialog
     *
     * \table
     *     \header
     *         \li Option
     *         \li Default value
     *         \li Description
     *     \row
     *         \li ShowAlphaChannel
     *         \li false
     *         \li Allow the user to select the alpha component of a color.
     *     \row
     *         \li NoButtons
     *         \li false
     *         \li Don't display OK and Cancel buttons. (Useful
     *             for “live dialogs”.)
     *     \row
     *         \li NoEyeDropperButton
     *         \li false
     *         \li Hide the Eye Dropper button. This value was added in Qt 6.6.
     *     \row
     *         \li DontUseNativeDialog
     *         \li true
     *         \li Use Qt’s standard color dialog instead of the
     *             operating system native color dialog.
     * \endtable
     *
     *   The option
     *   \c {QColorDialog::ColorDialogOption::DontUseNativeDialog}
     *   will always be \c {true} because it’s just the point of
     *   this library to provide an own, non-native dialog. (If you
     *   set \c {QColorDialog::ColorDialogOption::DontUseNativeDialog}
     *   explicitly
     *   to \c {false}, this will silently be ignored, while the
     *   other options that you might have set, will be correctly applied.)
     *
     * \note Eyedropper functionality is only available if the current system
     * supports this feature through Portal. If eyedropper support is
     * unavailable, setting NoEyeDropperButton to \c {false}> has no
     * effect, and the eyedropper button will not be shown.
     *
     * Example:
     *
     * \snippet testcolordialog.cpp setOptionsWithLocalEnum
     *
     * Or:
     *
     * \snippet testcolordialog.cpp setOptionsWithQColorDialogEnum
     *
     * At difference to QColorDialog, you need a fully qualified
     * identifier for the enum values. The following code would therefore
     * fail:
     *
     * \c {myDialog->setOption(ShowAlphaChannel, false);}
     */
    Q_PROPERTY(QColorDialog::ColorDialogOptions options READ options WRITE setOptions NOTIFY optionsChanged)

public:
    /*!
     * \brief Layout dimensions
     *
     * This enum is declared to the meta-object system with \c {Q_ENUM}.
     * This happens automatically. You do not need to make any manual calls.
     *
     * This type is declared as type to Qt’s type system via
     * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
     * example if you want to use for \e queued signal-slot connections),
     * you might consider calling \c {qRegisterMetaType()} for
     * this type, once you have a QApplication object.
     *
     * \value ScreenSizeDependent
     * Decide automatically between
     * \c {collapsed} and \c {*expanded} layout: \c {collapsed}
     * is used on small screens, and \c {expanded} on big screens. The
     * decision is based on the screen size of the default screen
     * of the widget (see \c {QGuiApplication::primaryScreen()} for
     * details). The decision is evaluated at the moment when setting this
     * value, and again each time the widget is shown again. It is
     * not evaluated again when a yet existing dialog is just
     * moved to another screen.
     *
     * \value Collapsed
     * Use the small, “collapsed“ layout of this dialog.
     *
     * \value Expanded
     * Use the large, “expanded” layout of this dialog.
     */
    enum class DialogLayoutDimensions {
        ScreenSizeDependent,
        Collapsed,
        Expanded
    };
    Q_ENUM(DialogLayoutDimensions)
    Q_INVOKABLE explicit ColorDialog(QWidget *parent = nullptr);
    Q_INVOKABLE explicit ColorDialog(const QColor &initial, QWidget *parent = nullptr);
    Q_INVOKABLE explicit ColorDialog(const QString &identifier, QWidget *parent = nullptr);
    Q_INVOKABLE explicit ColorDialog(const QString &identifier, const QColor &initial, QWidget *parent = nullptr);
    virtual ~ColorDialog() noexcept override;
    /*!
     * \brief Getter for property \l currentColor
     */
    [[nodiscard]] QColor currentColor() const;
    [[nodiscard]] static QColor getColor(const QColor &initial = Qt::white,
                                         QWidget *parent = nullptr,
                                         const QString &title = QString(),
                                         QColorDialog::ColorDialogOptions options = QColorDialog::ColorDialogOptions());
    /*!
     * \brief Getter for property \l layoutDimensions
     */
    [[nodiscard]] ColorDialog::DialogLayoutDimensions layoutDimensions() const;
    // Make sure not to override the base class’s “open“ function:
    using QDialog::open;
    Q_INVOKABLE void open(QObject *receiver, const char *member);
    /*!
     * \brief Getter for property \l options
     */
    [[nodiscard]] QColorDialog::ColorDialogOptions options() const;
    [[nodiscard]] Q_INVOKABLE QColor selectedColor() const;
    virtual void setVisible(bool visible) override;
    [[nodiscard]] Q_INVOKABLE bool testOption(QColorDialog::ColorDialogOption option) const;

public Q_SLOTS:
    void setCurrentColor(const QColor &color);
    void setLayoutDimensions(const PerceptualColor::ColorDialog::DialogLayoutDimensions newLayoutDimensions);
    Q_INVOKABLE void setOption(QColorDialog::ColorDialogOption option, bool on = true);
    void setOptions(QColorDialog::ColorDialogOptions newOptions);

Q_SIGNALS:
    /*!
     * \brief This signal is emitted just after the user has clicked OK to
     * select a color to use.
     *
     * \a color the chosen color
     */
    void colorSelected(const QColor &color);
    /*!
     * \brief Notify signal for property \l currentColor.
     *
     * This signal is emitted whenever the “current color” changes in the
     * dialog.
     *
     * \a color the new “current color”
     */
    void currentColorChanged(const QColor &color);
    /*!
     * \brief Notify signal for property \l layoutDimensions.
     *
     * \a newLayoutDimensions the new layout dimensions
     */
    void layoutDimensionsChanged(const PerceptualColor::ColorDialog::DialogLayoutDimensions newLayoutDimensions);
    /*!
     * \brief Notify signal for property \l options.
     *
     * \a newOptions the new \l options
     */
    void optionsChanged(const QColorDialog::ColorDialogOptions newOptions);

protected:
    virtual void changeEvent(QEvent *eventParameter) override;
    virtual void done(int result) override;
    virtual bool event(QEvent *eventParameter) override;
    virtual void keyPressEvent(QKeyEvent *eventParameter) override;
    virtual void showEvent(QShowEvent *eventParameter) override;

private:
    Q_DISABLE_COPY(ColorDialog)

    friend class ColorDialogPrivate;

    /*!
     * \brief Pointer to implementation (pimpl)
     */
    ConstPropagatingUniquePointer<ColorDialogPrivate> d_pointer;

    friend class TestColorDialog;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ColorDialog::DialogLayoutDimensions)

#endif // PERCEPTUALCOLOR_COLORDIALOG_H
