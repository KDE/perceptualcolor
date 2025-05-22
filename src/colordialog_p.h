// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef COLORDIALOG_P_H
#define COLORDIALOG_P_H

// Include the header of the public class of this private implementation.
#include "colordialog.h"

#include "constpropagatingrawpointer.h"
#include "genericcolor.h"
#include "helper.h"
#include "helperconversion.h"
#include "helperqttypes.h"
#include "languagechangeeventfilter.h"
#include "perceptualsettings.h"
#include "rgbcolor.h"
#include <lcms2.h>
#include <optional>
#include <qbytearray.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qhash.h>
#include <qobject.h>
#include <qpointer.h>
#include <qsharedpointer.h>
#include <qstring.h>
#include <qstringliteral.h>
class QAction;
class QComboBox;
class QDialogButtonBox;
class QDoubleSpinBox;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QShortcut;
class QStackedLayout;
class QTabWidget;
class QToolButton;
class QWidget;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

namespace PerceptualColor
{
class ChromaHueDiagram;
class ColorPatch;
class GradientSlider;
class MultiSpinBox;
class SwatchBook;
class RgbColorSpace;
class WheelColorPicker;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ColorDialogPrivate final : public QObject
{
    Q_OBJECT
public:
    explicit ColorDialogPrivate(ColorDialog *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    virtual ~ColorDialogPrivate() noexcept override = default;

    /** @brief Horizontal count of swatches for history. */
    static constexpr QListSizeType historyHSwatchCount = 10;
    /** @brief  Vertical count of swatches for history. */
    static constexpr QListSizeType historyVSwatchCount = 4;
    /** @brief  Total count of swatches for history. */
    static constexpr QListSizeType historySwatchCount = //
        historyHSwatchCount * historyVSwatchCount;

    /** @brief Horizontal count of swatches for custom colors. */
    static constexpr QListSizeType customColorsHSwatchCount = 9;
    /** @brief  Vertical count of swatches for custom colors. */
    static constexpr QListSizeType customColorsVSwatchCount = 4;
    /** @brief  Total count of swatches for custom colors. */
    static constexpr QListSizeType customColorsSwatchCount = //
        historyHSwatchCount * historyVSwatchCount;

    /** @brief @ref GradientSlider widget for the alpha channel. */
    QPointer<GradientSlider> m_alphaGradientSlider;
    /** @brief Pointer to the QLabel for the alpha value.
     *
     * We store this in a pointer to allow toggle the visibility later. */
    QPointer<QLabel> m_alphaLabel;
    /** @brief Spin box for the alpha channel.
     *
     * This spin box shows always the value of @ref m_alphaGradientSlider.
     *
     * @note It’s value is not set directly, but is updated via signals from
     * @ref m_alphaGradientSlider. Do not use it directly! */
    QPointer<QDoubleSpinBox> m_alphaSpinBox;
    /** @brief Pointer to the QButtonBox of this dialog.
     *
     * We store this in a pointer
     * to allow toggle the visibility later. */
    QPointer<QDialogButtonBox> m_buttonBox;
    /** @brief Pointer to the “Cancel” button of @ref m_buttonBox. */
    QPointer<QPushButton> m_buttonCancel;
    /** @brief Pointer to the “Ok” button of @ref m_buttonBox. */
    QPointer<QPushButton> m_buttonOK;
    /** @brief Pointer to the @ref ChromaHueDiagram. */
    QPointer<ChromaHueDiagram> m_chromaHueDiagram;
    /** @brief Pointer to the @ref MultiSpinBox for CIELCH. */
    QPointer<MultiSpinBox> m_cielchD50SpinBox;
    /** @brief Pointer to the gamut action for @ref m_cielchD50SpinBox. */
    QPointer<QAction> m_cielchD50SpinBoxGamutAction;
    /** @brief Pointer to the label for @ref m_cielchD50SpinBox. */
    QPointer<QLabel> m_cielchD50SpinBoxLabel;
    /** @brief Pointer to the @ref ColorPatch widget. */
    QPointer<ColorPatch> m_colorPatch;
    /** @brief Holds the currently used icon theme.
     *
     * Initially this is set to an arbitrary value. Once @ref reloadIcons() has
     * been called, it has an actual value. */
    ColorSchemeType m_currentIconThemeType = ColorSchemeType::Light;
    /** @brief Current color without alpha information
     *
     * Holds the color in absolutely defined color models.
     *
     * @note This value is considered in-gamut (even thought @ref RgbColor
     * might tell different because of rounding errors).
     *
     * @sa @ref ColorDialog::currentColor()
     * @sa @ref m_currentOpaqueColorRgb */
    QHash<ColorModel, GenericColor> m_currentOpaqueColorAbs;
    /** @brief Current color without alpha information
     *
     * Holds the color in the RGB color model and derived color models.
     *
     * @note This value is in-gamut by definition.
     *
     * @sa @ref ColorDialog::currentColor()
     * @sa @ref m_currentOpaqueColorAbs */
    RgbColor m_currentOpaqueColorRgb;
    /** @brief If @ref q_pointer has ever been shown. */
    bool everShown = false;
    /** @brief Pointer to the @ref MultiSpinBox for HSL. */
    QPointer<MultiSpinBox> m_hslSpinBox;
    /** @brief Pointer to the label for @ref m_hslSpinBox. */
    QPointer<QLabel> m_hslSpinBoxLabel;
    /** @brief Pointer to the @ref MultiSpinBox for HSV. */
    QPointer<MultiSpinBox> m_hsvSpinBox;
    /** @brief Pointer to the label for @ref m_hsvSpinBox. */
    QPointer<QLabel> m_hsvSpinBoxLabel;
    /** @brief Pointer to the @ref MultiSpinBox for HWB. */
    QPointer<MultiSpinBox> m_hwbSpinBox;
    /** @brief Pointer to the label for @ref m_hwbSpinBox. */
    QPointer<QLabel> m_hwbSpinBoxLabel;
    /** @brief Shortcut to show the tab with @ref m_hueFirstWrapperWidget. */
    QPointer<QShortcut> m_hueFirstTabShortcut;
    /** @brief Pointer to the QWidget wrapper that contains
     * @ref m_wheelColorPicker. */
    QPointer<QWidget> m_hueFirstWrapperWidget;
    /** @brief Holds whether currently a color change is ongoing, or not.
     *
     * Used to avoid infinite recursions when updating the different widgets
     * within this dialog.
     * @sa @ref setCurrentOpaqueColor() */
    bool m_isColorChangeInProgress = false;
    /** @brief Holds whether the current text of @ref m_rgbLineEdit differs
     * from the value in @ref m_currentOpaqueColorRgb.
     * @sa @ref readRgbHexValues
     * @sa @ref updateRgbHexButBlockSignals */
    bool m_isDirtyRgbLineEdit = false;
    /** @brief An event filter used for some child widgets. */
    LanguageChangeEventFilter m_languageChangeEventFilter;
    /** @brief Internal storage for property
     * @ref ColorDialog::layoutDimensions */
    PerceptualColor::ColorDialog::DialogLayoutDimensions m_layoutDimensions =
        //! [layoutDimensionsDefaultValue]
        ColorDialog::DialogLayoutDimensions::Collapsed
        //! [layoutDimensionsDefaultValue]
        ;
    /** @brief The <em>effective</em> layout dimensions.
     *
     * The property @ref ColorDialog::layoutDimensions has a value
     * @ref ColorDialog::DialogLayoutDimensions::ScreenSizeDependent.
     * <em>This</em> variable holds whatever <em>effectively</em>
     * is applied. So it can only have the values
     * @ref ColorDialog::DialogLayoutDimensions::Collapsed or
     * @ref ColorDialog::DialogLayoutDimensions::Expanded. */
    PerceptualColor::ColorDialog::DialogLayoutDimensions m_layoutDimensionsEffective = m_layoutDimensions;
    /** @brief Shortcut to show the tab with @ref m_lightnessFirstWrapperWidget. */
    QPointer<QShortcut> m_lightnessFirstTabShortcut;
    /** @brief Pointer to the QWidget wrapper that contains
     * @ref m_lchLightnessSelector and @ref m_chromaHueDiagram. */
    QPointer<QWidget> m_lightnessFirstWrapperWidget;
    /** @brief Pointer to the @ref GradientSlider for LCH lightness. */
    QPointer<GradientSlider> m_lchLightnessSelector;
    /** @brief Holds the receiver slot (if any) to be disconnected
     *  automatically after closing the dialog.
     *
     * Its value is only meaningful if
     * @ref m_receiverToBeDisconnected is not null.
     * @sa @ref m_receiverToBeDisconnected
     * @sa @ref ColorDialog::open() */
    QByteArray m_memberToBeDisconnected;
    /** @brief String that is used as separator between two sections
     * within a @ref MultiSpinBox.
     *
     * This string is introduced <em>twice</em> between two sections
     * within a @ref MultiSpinBox. */
    static inline const QString m_multispinboxSectionSeparator = QStringLiteral(u" ");
    /** @brief Shortcut to show the tab with @ref m_numericalWidget. */
    QPointer<QShortcut> m_numericalTabShortcut;
    /** @brief Pointer to the widget that holds the numeric color
     *         representation. */
    QPointer<QWidget> m_numericalWidget;
    /** @brief Pointer to the @ref MultiSpinBox for Oklch. */
    QPointer<MultiSpinBox> m_oklchSpinBox;
    /** @brief Pointer to the gamut action for @ref m_oklchSpinBox. */
    QPointer<QAction> m_oklchSpinBoxGamutAction;
    /** @brief Pointer to the label for @ref m_oklchSpinBox. */
    QPointer<QLabel> m_oklchSpinBoxLabel;
    /** @brief Pointer to the basic colors widget. */
    QPointer<PerceptualColor::SwatchBook> m_swatchBookBasicColors;
    /** @brief Pointer to the basic colors widget. */
    QPointer<PerceptualColor::SwatchBook> m_swatchBookCustomColors;
    /** @brief Pointer to the history widget. */
    QPointer<PerceptualColor::SwatchBook> m_swatchBookHistory;
    /** @brief The selector widget to choose which swatch book to show.
     *
     * @sa @ref m_swatchBookStack */
    QPointer<QComboBox> m_swatchBookSelector;
    /** @brief Stack containing the various @ref SwatchBook widgets.
     *
     * @sa @ref m_swatchBookSelector */
    QPointer<QStackedLayout> m_swatchBookStack;
    /** @brief Shortcut to show the tab with @ref m_swatchBookWrapperWidget. */
    QPointer<QShortcut> m_swatchBookTabShortcut;
    /** @brief Pointer to the QWidget wrapper that contains
     * the swatch books. */
    QPointer<QWidget> m_swatchBookWrapperWidget;
    /** @brief Holds the receiver object (if any) to be disconnected
     *  automatically after closing the dialog.
     *
     * @sa @ref m_memberToBeDisconnected
     * @sa @ref ColorDialog::open() */
    QPointer<QObject> m_receiverToBeDisconnected;
    /** @brief Internal storage for property @ref ColorDialog::options */
    ColorDialog::ColorDialogOptions m_options;
    /** @brief Pointer to the RgbColorSpace object. */
    QSharedPointer<RgbColorSpace> m_rgbColorSpace;
    /** @brief Group box that contains all RGB widgets and all widget for
     * color spaces that are defined with RGB as base (HSV, Hex…). */
    QPointer<QGroupBox> m_rgbGroupBox;
    /** @brief Pointer to the QLineEdit that represents the hexadecimal
     *  RGB value. */
    QPointer<QLineEdit> m_rgbLineEdit;
    /** @brief Pointer to the label for @ref m_rgbLineEdit. */
    QPointer<QLabel> m_rgbLineEditLabel;
    /** @brief Pointer to the @ref MultiSpinBox for RGB. */
    QPointer<MultiSpinBox> m_rgbSpinBox;
    /** @brief Pointer to the label for @ref m_rgbSpinBox. */
    QPointer<QLabel> m_rgbSpinBoxLabel;
    /** @brief Internal storage for @ref ColorDialog::selectedColor(). */
    QColor m_selectedColor;
    /** @brief Layout that holds the graphical and numeric selectors. */
    QPointer<QHBoxLayout> m_selectorLayout;
    /** @brief Access to the @ref Settings singleton. */
    PerceptualSettings &m_settings = PerceptualSettings::getInstance();
    /** @brief Button that allows to pick with the mouse a color somewhere
     * from the screen. */
    QPointer<QToolButton> m_screenColorPickerButton;
    /** @brief A row with two columns within a table in Qt’s rich text
     * formatting.
     *
     * To use it, call QString::arg() twice: Once with the content of the
     * first column and once with the content of the second column. */
    const QString tableRow = QStringLiteral(u"<tr><td>%1</td><td>%2</td></tr>");
    /** @brief Table assigning to each tab a value for the @ref Settings.
     *
     * This helps to convert from QString values stored in @ref Settings
     * to the actual tab widgets and vice versa. */
    QHash<QPointer<QWidget> *, QString> m_tabTable;
    /** @brief Pointer to the tab widget. */
    QPointer<QTabWidget> m_tabWidget;
    /** @brief @ref m_wcsBasicColors for @ref m_rgbColorSpace. */
    QColorArray2D m_wcsBasicColors;
    /** @brief A default color within @ref m_wcsBasicColors.
     *
     * Choosing the blue tone (no tint, no shade). Arguments in favor:
     *
     * - Blue seems to be harmonious and integrate well in many designs.
     * - The blue color is quite chromatic, giving a vivid impression.
     * - Blue does not “screem” like red.
     * - Blue is exactly at the middle of the swatch book.
     * - The tone (no tint, no shade)  is exactly at the middle of the
     *   swatch book. */
    QColor m_wcsBasicDefaultColor;
    /** @brief Pointer to the @ref WheelColorPicker widget. */
    QPointer<WheelColorPicker> m_wheelColorPicker;

    /** @brief Number of decimals to for most values.
     *
     * @sa @ref okdecimals */
    static constexpr quint8 decimals = 0;
    /** @brief Number of decimals to use for the Oklab/Oklch values
     * L, C, a, b (but not for h!).
     *
     * @sa @ref decimals */
    static constexpr quint8 okdecimals = decimals + 2;

    void applyLayoutDimensions();
    [[nodiscard]] QColor defaultColor() const;
    void initialize(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    [[nodiscard]] QWidget *initializeNumericPage();
    void initializeScreenColorPicker();
    void loadCustomColorsFromSettingsToSwatchBook();
    void loadHistoryFromSettingsToSwatchBook();
    [[nodiscard]] QString translateColorModel(cmsColorSpaceSignature model);

public Q_SLOTS:
    void readChromaHueDiagramValue();
    void readColorPatchValue();
    void readHslNumericValues();
    void readHsvNumericValues();
    void readHwbNumericValues();
    void readLchNumericValues();
    void readLightnessValue();
    void readOklchNumericValues();
    void readRgbHexValues();
    void readRgbNumericValues();
    void readSwatchBookBasicColorsValue();
    void readSwatchBookCustomColorsValue();
    void readSwatchBookHistoryValue();
    void readWheelColorPickerValues();
    void reloadIcons();
    void retranslateUi();
    void saveCurrentTab();
    void setCurrentOpaqueColor(const QHash<PerceptualColor::ColorModel, PerceptualColor::GenericColor> &abs, QWidget *const ignoreWidget);
    void setCurrentOpaqueColor(const PerceptualColor::RgbColor &rgb, QWidget *const ignoreWidget);
    void setCurrentOpaqueColor(const QHash<PerceptualColor::ColorModel, PerceptualColor::GenericColor> &abs,
                               const PerceptualColor::RgbColor &rgb,
                               QWidget *const ignoreWidget);
    void updateColorPatch();
    void updateLchButBlockSignals();
    void updateOklchButBlockSignals();
    void updateRgbHexButBlockSignals();

private:
    Q_DISABLE_COPY(ColorDialogPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorDialog> q_pointer;
};

} // namespace PerceptualColor

#endif // COLORDIALOG_P_H
