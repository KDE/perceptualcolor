// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef COLORDIALOG_P_H
#define COLORDIALOG_P_H

// Include the header of the public class of this private implementation.
#include "colordialog.h"

#include "constpropagatingrawpointer.h"
#include "languagechangeeventfilter.h"
#include "multicolor.h"
#include <QtCore/qsharedpointer.h>
#include <lcms2.h>
#include <optional>
#include <qbytearray.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpointer.h>
#include <qstring.h>
#include <qstringliteral.h>
class QDialogButtonBox;
class QDoubleSpinBox;
class QGroupBox;
class QHBoxLayout;
class QLabel;
class QLineEdit;
class QPushButton;
class QTabWidget;
class QWidget;

namespace PerceptualColor
{
class ChromaHueDiagram;
class ColorPatch;
class GradientSlider;
class MultiSpinBox;
class PaletteWidget;
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
    ColorDialogPrivate(ColorDialog *backLink);
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    virtual ~ColorDialogPrivate() noexcept override = default;

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
    /** @brief Pointer to the @ref MultiSpinBox for CIEHLC. */
    QPointer<MultiSpinBox> m_ciehlcSpinBox;
    /** @brief Pointer to the label for @ref m_ciehlcSpinBox. */
    QPointer<QLabel> m_ciehlcSpinBoxLabel;
    /** @brief Pointer to the @ref ColorPatch widget. */
    QPointer<ColorPatch> m_colorPatch;
    /** @brief Holds the current color without alpha information
     *
     * @sa @ref ColorDialog::currentColor() */
    MultiColor m_currentOpaqueColor;
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
     * from the value in @ref m_currentOpaqueColor.
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
     * within a @ref MultiSpinBox. */
    const QString m_multispinboxSectionSeparator = QStringLiteral(u" ");
    /** @brief Pointer to the widget that holds the numeric color
     *         representation. */
    QPointer<QWidget> m_numericalWidget;
    /** @brief Pointer to the @ref MultiSpinBox for CIEHLC. */
    QPointer<MultiSpinBox> m_oklchSpinBox;
    /** @brief Pointer to the label for @ref m_oklchSpinBox. */
    QPointer<QLabel> m_oklchSpinBoxLabel;
    /** @brief Pointer to the palette widget. */
    QPointer<PerceptualColor::PaletteWidget> m_paletteWidget;
    /** @brief Pointer to the QWidget wrapper that contains
     * @ref m_paletteWidget. */
    QPointer<QWidget> m_paletteWrapperWidget;
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
    /** @brief Button that allows to pick with the mouse a color somewhere
     * from the screen. */
    QPointer<QPushButton> m_screenColorPickerButton;
    /** @brief Indicates if on the system <em>we are currently running on</em>
     * we have support for screen color picker functionality. */
    static inline std::optional<bool> m_screenColorPickerSupportAvailable = std::nullopt;
    /** @brief Widget that holds the @ref m_screenColorPickerButton. */
    QPointer<QWidget> m_screenColorPickerWidget;
    /** @brief A row with two columns within a table in Qt’s rich text
     * formatting.
     *
     * To use it, call QString::arg() twice: Once with the content of the
     * first column and once with the content of the second column. */
    const QString tableRow = QStringLiteral(u"<tr><td>%1</td><td>%2</td></tr>");
    /** @brief Pointer to the tab widget. */
    QPointer<QTabWidget> m_tabWidget;
    /** @brief Pointer to the @ref WheelColorPicker widget. */
    QPointer<WheelColorPicker> m_wheelColorPicker;

    /** @brief Number of decimals to use in the spin boxes.
     *
     * @sa @ref okdecimals */
    static constexpr quint8 decimals = 0;
    /** @brief Number of decimals to use in the spin boxes for the Oklab and
     * Oklch values L, C, a, b (but not for h).
     *
     * @sa @ref decimals */
    static constexpr quint8 okdecimals = decimals + 2;

    void applyLayoutDimensions();
    void initialize(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);
    [[nodiscard]] QWidget *initializeNumericPage();
    void initializeScreenColorPicker();
    [[nodiscard]] QString translateColorModel(cmsColorSpaceSignature model);
    [[nodiscard]] static QString translateViaQColorDialog(const char *sourceText);

public Q_SLOTS:
    void readChromaHueDiagramValue();
    void readHlcNumericValues();
    void readHslNumericValues();
    void readHsvNumericValues();
    void readHwbNumericValues();
    void readLightnessValue();
    void readPaletteWidget();
    void readRgbHexValues();
    void readRgbNumericValues();
    void readWheelColorPickerValues();
    void retranslateUi();
    void setCurrentOpaqueColor(const PerceptualColor::MultiColor &color, QWidget *const ignoreWidget);
    void updateColorPatch();
    void updateHlcButBlockSignals();
    void updateRgbHexButBlockSignals();

private:
    Q_DISABLE_COPY(ColorDialogPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<ColorDialog> q_pointer;
};

} // namespace PerceptualColor

#endif // COLORDIALOG_P_H
