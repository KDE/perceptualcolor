// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "colordialog.h"
// Second, the private implementation.
#include "colordialog_p.h" // IWYU pragma: associated

#include "absolutecolor.h"
#include "chromahuediagram.h"
#include "cielchd50values.h"
#include "colorpatch.h"
#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "gradientslider.h"
#include "helper.h"
#include "helperconstants.h"
#include "helperconversion.h"
#include "helperqttypes.h"
#include "initializetranslation.h"
#include "lchadouble.h"
#include "lchdouble.h"
#include "multispinbox.h"
#include "multispinboxsection.h"
#include "oklchvalues.h"
#include "rgbcolor.h"
#include "rgbcolorspace.h"
#include "rgbcolorspacefactory.h"
#include "screencolorpicker.h"
#include "setting.h"
#include "swatchbook.h"
#include "wheelcolorpicker.h"
#include <algorithm>
#include <lcms2.h>
#include <optional>
#include <qaction.h>
#include <qapplication.h>
#include <qboxlayout.h>
#include <qbytearray.h>
#include <qchar.h>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qdatetime.h>
#include <qdebug.h>
#include <qdialogbuttonbox.h>
#include <qfontmetrics.h>
#include <qformlayout.h>
#include <qgroupbox.h>
#include <qguiapplication.h>
#include <qicon.h>
#include <qkeysequence.h>
#include <qlabel.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qlocale.h>
#include <qobject.h>
#include <qpair.h>
#include <qpointer.h>
#include <qpushbutton.h>
#include <qregularexpression.h>
#include <qscopedpointer.h>
#include <qscreen.h>
#include <qsharedpointer.h>
#include <qshortcut.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include <qspinbox.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qtabwidget.h>
#include <qtoolbutton.h>
#include <qvalidator.h>
#include <qversionnumber.h>
#include <qwidget.h>
#include <utility>
class QShowEvent;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qobjectdefs.h>
#else
#include <qstringlist.h>
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
#include <qstylehints.h>
#endif

namespace PerceptualColor
{

/** @brief A text with the name of the color model.
 *
 * @param model The signature of the color model.
 *
 * @returns A text with the name of the color model, or an empty
 * QString if the model is unknown. If a translation is available,
 * the translation is returned instead of the original English text. */
QString ColorDialogPrivate::translateColorModel(cmsColorSpaceSignature model)
{
    switch (model) {
    case cmsSigXYZData:
        /*: @item A color model: X, Y, Z. */
        return tr("XYZ");
    case cmsSigLabData:
        /*: @item A color model: Lightness, a, b. */
        return tr("Lab");
    case cmsSigRgbData:
        /*: @item A color model: red, green, blue. */
        return tr("RGB");
    case cmsSigLuvData:
        // return tr("Luv"); // Currently not supported.
        return QString();
    case cmsSigYCbCrData:
        // return tr("YCbCr"); // Currently not supported.
        return QString();
    case cmsSigYxyData:
        // return tr("Yxy"); // Currently not supported.
        return QString();
    case cmsSigGrayData:
        // return tr("Grayscale"); // Currently not supported.
        return QString();
    case cmsSigHsvData:
        // return tr("HSV"); // Currently not supported.
        return QString();
    case cmsSigHlsData:
        // return tr("HSL"); // Currently not supported.
        return QString();
    case cmsSigCmykData:
        // return tr("CMYK"); // Currently not supported.
        return QString();
    case cmsSigCmyData:
        // return tr("CMY"); // Currently not supported.
        return QString();
    case cmsSigNamedData: // Does not exist in ICC 4.4.
    case cmsSig2colorData:
    case cmsSig3colorData:
    case cmsSig4colorData:
    case cmsSig5colorData:
    case cmsSig6colorData:
    case cmsSig7colorData:
    case cmsSig8colorData:
    case cmsSig9colorData:
    case cmsSig10colorData:
    case cmsSig11colorData:
    case cmsSig12colorData:
    case cmsSig13colorData:
    case cmsSig14colorData:
    case cmsSig15colorData:
        // return tr("Named color"); // Currently not supported.
        return QString();
    case cmsSig1colorData:
    case cmsSigLuvKData:
    case cmsSigMCH1Data:
    case cmsSigMCH2Data:
    case cmsSigMCH3Data:
    case cmsSigMCH4Data:
    case cmsSigMCH5Data:
    case cmsSigMCH6Data:
    case cmsSigMCH7Data:
    case cmsSigMCH8Data:
    case cmsSigMCH9Data:
    case cmsSigMCHAData:
    case cmsSigMCHBData:
    case cmsSigMCHCData:
    case cmsSigMCHDData:
    case cmsSigMCHEData:
    case cmsSigMCHFData:
    // Unhandeled: These values do not exist in ICC 4.4 standard as
    // published at https://www.color.org/specification/ICC.1-2022-05.pdf
    // page 35, table 19 — Data colour space signatures.
    default:
        break;
    }
    return QString();
}

/** @brief Retranslate the UI with all user-visible strings.
 *
 * This function updates all user-visible strings by using
 * <tt>Qt::tr()</tt> to get up-to-date translations.
 *
 * This function is meant to be called at the end of the constructor and
 * additionally after each <tt>QEvent::LanguageChange</tt> event.
 *
 * @note This is the same concept as
 * <a href="https://doc.qt.io/qt-5/designer-using-a-ui-file.html">
 * Qt Designer, which also provides a function of the same name in
 * uic-generated code</a>.
 *
 * @internal
 *
 * @todo Add to the color-space tooltip information about available rendering
 * intents (we have yet RgbColorSpacePrivate::intentList but do not use it
 * anywhere) and the RGB profile illuminant? (This would have to be implemented
 * in @ref RgbColorSpace first.)
 *
 * @todo As the tooltip for color-space information is quite big, would
 * it be better to do what systemsettings does in globaldesign/fonts? They
 * have a small button with an “i” symbol (for information), which does
 * nothing when it’s clicked, but when hovering with the mouse, it shows
 * the tooltip?
 *
 * @todo How to make tooltip information available for touch-screen users? */
void ColorDialogPrivate::retranslateUi()
{
    /*: @item/plain Percentage value in a spinbox. Range: 0%–100%. */
    const QPair<QString, QString> percentageInSpinbox = //
        getPrefixSuffix(tr("%1%"));

    /*: @item/plain Arc-degree value in a spinbox. Range: 0°–360°. */
    const QPair<QString, QString> arcDegreeInSpinbox = //
        getPrefixSuffix(tr("%1°"));

    QStringList profileInfo;
    const QString name = //
        m_rgbColorSpace->profileName().toHtmlEscaped();
    if (!name.isEmpty()) {
        /*: @item:intext An information from the color profile to be added
        to the info text about current color space. */
        profileInfo.append(tableRow.arg(tr("Name:"), name));
    }
    /*: @item:intext The maximum chroma. */
    const QString maximumCielchD50Chroma = //
        tr("%L1 (estimated)")
            .arg(m_rgbColorSpace->profileMaximumCielchD50Chroma(), //
                 0, //
                 'f', //
                 decimals);
    /*: @item:intext An information from the color profile to be added
    to the info text about current color space. */
    profileInfo.append( //
        tableRow.arg(tr("Maximum CIELCh-D50 chroma:"), maximumCielchD50Chroma));
    /*: @item:intext The maximum chroma. */
    const QString maximumOklchChroma = //
        tr("%L1 (estimated)")
            .arg(m_rgbColorSpace->profileMaximumOklchChroma(), //
                 0, //
                 'f', //
                 okdecimals);
    /*: @item:intext An information from the color profile to be added
    to the info text about current color space. */
    profileInfo.append( //
        tableRow.arg(tr("Maximum Oklch chroma:"), maximumOklchChroma));
    QString profileClass;
    switch (m_rgbColorSpace->profileClass()) {
    case cmsSigDisplayClass:
        /*: @item:intext The class of an ICC profile. */
        profileClass = tr("Display profile");
        break;
    case cmsSigAbstractClass: // Image effect profile (Abstract profile)
                              // This ICC profile class is called "abstract
                              // profile" in the official standard. However,
                              // the name is misleading. The actual function of
                              // these ICC profiles is to apply image effects.
    case cmsSigColorSpaceClass: // Color space conversion profile
    case cmsSigInputClass: // Input profile
    case cmsSigLinkClass: // Device link profile
    case cmsSigNamedColorClass: // Named color profile
    case cmsSigOutputClass: // Output profile
        // These profile classes are currently not supported.
        break;
    }
    if (!profileClass.isEmpty()) {
        /*: @item:intext An information from the color profile to be added
        to the info text about current color space. */
        profileInfo.append( //
            tableRow.arg(tr("Profile class:"), profileClass));
    }
    const QString colorModel = //
        translateColorModel(m_rgbColorSpace->profileColorModel());
    if (!colorModel.isEmpty()) {
        /*: @item:intext An information from the color profile to be added
        to the info text about current color space.
        The color model of the color space which is described by this
        profile. */
        profileInfo.append(tableRow.arg(tr("Color model:"), colorModel));
    }
    const QString manufacturer = //
        m_rgbColorSpace->profileManufacturer().toHtmlEscaped();
    if (!manufacturer.isEmpty()) {
        /*: @item:intext An information from the color profile to be added
        to the info text about current color space.
        This is usually the manufacturer of the device to which
        the colour profile applies. */
        profileInfo.append(tableRow.arg(tr("Manufacturer:"), manufacturer));
    }
    const QString model = //
        m_rgbColorSpace->profileModel().toHtmlEscaped();
    if (!model.isEmpty()) {
        /*: @item:intext An information from the color profile to be added to
        the info text about current color space.
        This is usually the model identifier of the device to which
        the colour profile applies. */
        profileInfo.append(tableRow.arg(tr("Device model:"), (model)));
    }
    const QDateTime creationDateTime = //
        m_rgbColorSpace->profileCreationDateTime();
    if (!creationDateTime.isNull()) {
        const auto creationDateTimeString = QLocale().toString(
            // Date and time:
            creationDateTime,
            // Format:
            QLocale::LongFormat);
        /*: @item:intext An information from the color profile to be added to
        the info text about current color space.
        This is the date and time of the creation of the profile. */
        profileInfo.append( //
            tableRow.arg(tr("Created:"), (creationDateTimeString)));
    }
    const QVersionNumber iccVersion = m_rgbColorSpace->profileIccVersion();
    /*: @item:intext An information from the color profile to be added to
    the info text about current color space.
    This is the version number of the ICC file format that is used. */
    profileInfo.append( //
        tableRow.arg(tr("ICC format:"), (iccVersion.toString())));
    const bool hasMatrixShaper = //
        m_rgbColorSpace->profileHasMatrixShaper();
    const bool hasClut = //
        m_rgbColorSpace->profileHasClut();
    if (hasMatrixShaper || hasClut) {
        const QString matrixShaperString = tableRow.arg(
            /*: @item:intext An information from the color profile to be added
            to the info text about current color space.
            Wether the profile has a matrix shaper or a color lookup table
            (CLUT) or both. */
            tr("Implementation:"));
        if (hasMatrixShaper && hasClut) {
            /*: @item:intext An information from the color profile to be added
            to the info text about current color space.
            Wether the profile has a matrix shaper or a color lookup table
            (CLUT) or both. */
            profileInfo.append( //
                matrixShaperString.arg(tr("Matrices and color lookup tables")));
        } else if (hasMatrixShaper) {
            /*: @item:intext An information from the color profile to be added
            to the info text about current color space.
            Wether the profile has a matrix shaper or a color lookup table
            (CLUT) or both. */
            profileInfo.append(matrixShaperString.arg(tr("Matrices")));
        } else if (hasClut) {
            /*: @item:intext An information from the color profile to be added
            to the info text about current color space.
            Wether the profile has a matrix shaper or a color lookup table
            (CLUT) or both. */
            profileInfo.append( //
                matrixShaperString.arg(tr("Color lookup tables")));
        }
    }
    const QString pcsColorModelText = //
        translateColorModel(m_rgbColorSpace->profilePcsColorModel());
    if (!pcsColorModelText.isEmpty()) {
        /*: @item:intext An information from the color profile to be added
        to the info text about current color space.
        The color model of the PCS (profile connection space) which is used
        internally by this profile. */
        profileInfo.append( //
            tableRow.arg(tr("PCS color model:"), pcsColorModelText));
    }
    const QString copyright = m_rgbColorSpace->profileCopyright();
    if (!copyright.isEmpty()) {
        /*: @item:intext An information from the color profile to be added
        to the info text about current color space.
        The copyright of this profile. */
        profileInfo.append(tableRow.arg(tr("Copyright:"), copyright));
    }
    const qint64 fileSize = //
        m_rgbColorSpace->profileFileSize();
    if (fileSize >= 0) {
        /*: @item:intext An information from the color profile to be added to
        the info text about current color space.
        This is the size of the ICC file that was read in. */
        profileInfo.append(tableRow.arg(tr("File size:"), //
                                        QLocale().formattedDataSize(fileSize)));
    }
    const QString fileName = //
        m_rgbColorSpace->profileAbsoluteFilePath();
    if (!fileName.isEmpty()) {
        /*: @item:intext An information from the color profile to be added to
        the info text about current color space. */
        profileInfo.append(tableRow.arg(tr("File name:"), fileName));
    }
    if (profileInfo.isEmpty()) {
        m_rgbGroupBox->setToolTip(QString());
    } else {
        const QString tableString = QStringLiteral(
            "<b>%1</b><br/>"
            "<table border=\"0\" cellpadding=\"2\" cellspacing=\"0\">"
            "%2"
            "</table>");
        m_rgbGroupBox->setToolTip(richTextMarker
                                  + tableString.arg(
                                      /*: @info:intext Title of info text about
                                      current color space (will be followed by
                                      other information as available
                                      in the color profile. */
                                      tr("Color space information"), //
                                      profileInfo.join(QString())));
    }

    /*: @label:spinbox Label for CIE’s CIEHLC color model, based on Hue,
    Lightness, Chroma, and using the D50 illuminant as white point.*/
    m_ciehlcD50SpinBoxLabel->setText(tr("CIEHL&C D50:"));

    /*: @label:spinbox Label for Oklch color model, based on Lightness, Chroma,
    Hue, and using the D65 illuminant as white point. */
    m_oklchSpinBoxLabel->setText(tr("O&klch:"));

    /*: @label:spinbox Label for RGB color model, based on Red, Green, Blue. */
    m_rgbSpinBoxLabel->setText(tr("&RGB:"));

    /*: @label:textbox Label for hexadecimal RGB representation like #12ab45 */
    m_rgbLineEditLabel->setText(tr("He&x:"));

    const int paletteIndex = m_tabWidget->indexOf(m_paletteWrapperWidget);
    if (paletteIndex >= 0) {
        /*: @title:tab
        The tab contains a swatch book showing the basic colors like yellow,
        orange, red… Same text as in QColorDialog */
        const auto mnemonic = tr("&Basic colors");
        m_tabWidget->setTabToolTip( //
            paletteIndex, //
            richTextMarker + fromMnemonicToRichText(mnemonic));
        m_paletteTabShortcut->setKey(QKeySequence::mnemonic(mnemonic));
    }
    const int hueFirstIndex = m_tabWidget->indexOf(m_hueFirstWrapperWidget);
    if (hueFirstIndex >= 0) {
        /*: @title:tab
        The tab contains a visual UI to choose first the hue, and in a
        second step chroma and lightness. */
        const auto mnemonic = tr("&Hue-based");
        m_tabWidget->setTabToolTip( //
            hueFirstIndex, //
            richTextMarker + fromMnemonicToRichText(mnemonic));
        m_hueFirstTabShortcut->setKey(QKeySequence::mnemonic(mnemonic));
    }
    const int lightnessFirstIndex = //
        m_tabWidget->indexOf(m_lightnessFirstWrapperWidget);
    if (lightnessFirstIndex >= 0) {
        /*: @title:tab
        The tab contains a visual UI to choose first the lightness, and in a
        second step chroma and hue.
        “Lightness” is different from “brightness”/“value”
        and should therefore get a different translation. */
        const auto mnemonic = tr("&Lightness-based");
        m_tabWidget->setTabToolTip( //
            lightnessFirstIndex, //
            richTextMarker + fromMnemonicToRichText(mnemonic));
        m_lightnessFirstTabShortcut->setKey(QKeySequence::mnemonic(mnemonic));
    }
    const int numericIndex = //
        m_tabWidget->indexOf(m_numericalWidget);
    if (numericIndex >= 0) {
        /*: @title:tab
        The tab contains a UI to describe the color with numbers: Spin boxes
        and line edits containing values like “#2A7845” or “RGB 85 45 12”. */
        const auto mnemonic = tr("&Numeric");
        m_tabWidget->setTabToolTip( //
            numericIndex, //
            richTextMarker + fromMnemonicToRichText(mnemonic));
        m_numericalTabShortcut->setKey(QKeySequence::mnemonic(mnemonic));
    }

    /*: @label:spinbox HSL (hue, saturation, lightness) */
    m_hslSpinBoxLabel->setText(tr("HS&L:"));

    /*: @label:spinbox HSV (hue, saturation, value) and HSB (hue, saturation,
    brightness) are two different names for the very same color model. */
    m_hsvSpinBoxLabel->setText(tr("HS&V/HSB:"));

    /*: @label:spinbox HWB (hue, whiteness, blackness) */
    m_hwbSpinBoxLabel->setText(tr("H&WB:"));

    /*: @action:button */
    m_buttonOK->setText(tr("&OK"));

    /*: @action:button */
    m_buttonCancel->setText(tr("&Cancel"));
    /*: @info:tooltip Help text for RGB spinbox. */
    m_rgbSpinBox->setToolTip( //
        richTextMarker
        + tr("<p>Red: 0⁠–⁠255</p>"
             "<p>Green: 0⁠–⁠255</p>"
             "<p>Blue: 0⁠–⁠255</p>"));

    /*: @info:tooltip Help text for hexadecimal code. */
    m_rgbLineEdit->setToolTip( //
        richTextMarker
        + tr("<p>Hexadecimal color code, as used in HTML: #RRGGBB</p>"
             "<p>RR: two-digit code for red: 00⁠–⁠FF</p>"
             "<p>GG: two-digit code for green: 00⁠–⁠FF</p>"
             "<p>BB: two-digit code for blue: 00⁠–⁠FF</p>"));

    /*: @info:tooltip Help text for HSL (hue, saturation, lightness).
    Saturation: 0 means something on the grey axis; 255 means something
    between the grey axis and the most colorful color. This is different
    from “chroma” and should therefore get a different translation.
    Lightness: 0 means always black; 255 means always white. This is
    different from “brightness” and should therefore get a different
    translation. */
    m_hslSpinBox->setToolTip(richTextMarker
                             + tr("<p>Hue: 0°⁠–⁠360°</p>"
                                  "<p>HSL-Saturation: 0%⁠–⁠100%</p>"
                                  "<p>Lightness: 0%⁠–⁠100%</p>"));

    /*: @info:tooltip Help text for HWB (hue, whiteness, blackness).
    The idea behind is that the hue defines the pure (maximum colorful) color.
    Than, white color can be added, creating a “tint”. Or black color
    can be added, creating a “shade”. Or both can be added, creating a “tone“.
    See https://en.wikipedia.org/wiki/Tint,_shade_and_tone for more
    information. 0% white + 0% black = pure color. 100% white
    + 0% black = white. 0% white + 100% black = black. 50% white + 50% black
    = gray. 50% white + 0% black = tint. 25% white + 25% black = tone.
    0% white + 50% black = shade. */
    m_hwbSpinBox->setToolTip(richTextMarker
                             + tr("<p>Hue: 0°⁠–⁠360°</p>"
                                  "<p>Whiteness: 0%⁠–⁠100%</p>"
                                  "<p>Blackness: 0%⁠–⁠100%</p>"));

    /*: @info:tooltip Help text for HSV/HSB. HSV (hue, saturation, value)
    and HSB (hue, saturation, brightness) are two different names for the
    very same color model. Saturation: 0 means something between black and
    white; 255 means something between black and the most colorful color.
    This is different from “chroma” and should therefore get a different
    translation. Brightness/value: 0 means always black; 255 means something
    between white and the most colorful color. This is different from
    “lightness” and should therefore get a different translation. */
    m_hsvSpinBox->setToolTip(richTextMarker
                             + tr("<p>Hue: 0°⁠–⁠360°</p>"
                                  "<p>HSV/HSB-Saturation: 0%⁠–⁠100%</p>"
                                  "<p>Brightness/Value: 0%⁠–⁠100%</p>"));

    m_alphaSpinBox->setPrefix(percentageInSpinbox.first);
    m_alphaSpinBox->setSuffix(percentageInSpinbox.second);

    /*: @label:slider Accessible name for lightness slider. This is different
    from “brightness”/“value” and should therefore get a different
    translation. */
    m_lchLightnessSelector->setAccessibleName(tr("Lightness"));

    /*: @info:tooltip Help text for CIEHLC. “lightness” is different from
    “brightness”/“value” and should therefore get a different translation. */
    m_ciehlcD50SpinBox->setToolTip(richTextMarker
                                   + tr("<p>Hue: 0°⁠–⁠360°</p>"
                                        "<p>Lightness: 0%⁠–⁠100%</p>"
                                        "<p>Chroma: 0⁠–⁠%L1</p>")
                                         .arg(CielchD50Values::maximumChroma));

    constexpr double maxOklchChroma = OklchValues::maximumChroma;
    /*: @info:tooltip Help text for Oklch. “lightness” is different from
    “brightness”/“value” and should therefore get a different translation. */
    m_oklchSpinBox->setToolTip(richTextMarker
                               + tr("<p>Lightness: %L1⁠–⁠%L2</p>"
                                    "<p>Chroma: %L3⁠–⁠%L4</p>"
                                    "<p>Hue: 0°⁠–⁠360°</p>"
                                    "<p>Whitepoint: D65</p>")
                                     .arg(0., 0, 'f', okdecimals)
                                     .arg(1., 0, 'f', okdecimals)
                                     .arg(0., 0, 'f', okdecimals)
                                     .arg(maxOklchChroma, 0, 'f', okdecimals));

    /*: @label:slider An opacity of 0 means completely
    transparent. The higher the opacity value increases, the
    more opaque the colour becomes,  until it finally becomes
    completely opaque at the highest possible opacity value. */
    const QString opacityLabel = tr("Op&acity:");
    m_alphaGradientSlider->setAccessibleName(opacityLabel);
    m_alphaLabel->setText(opacityLabel);

    // HSL spin box
    QList<MultiSpinBoxSection> hslSections = //
        m_hslSpinBox->sectionConfigurations();
    if (hslSections.count() != 3) {
        qWarning() //
            << "Expected 3 sections in HSV MultiSpinBox, but got" //
            << hslSections.count() //
            << "instead. This is a bug in libperceptualcolor.";
    } else {
        hslSections[0].setPrefix(arcDegreeInSpinbox.first);
        hslSections[0].setSuffix( //
            arcDegreeInSpinbox.second + m_multispinboxSectionSeparator);
        hslSections[1].setPrefix( //
            m_multispinboxSectionSeparator + percentageInSpinbox.first);
        hslSections[1].setSuffix( //
            percentageInSpinbox.second + m_multispinboxSectionSeparator);
        hslSections[2].setPrefix( //
            m_multispinboxSectionSeparator + percentageInSpinbox.first);
        hslSections[2].setSuffix(percentageInSpinbox.second);
        m_hslSpinBox->setSectionConfigurations(hslSections);
    }

    // HWB spin box
    QList<MultiSpinBoxSection> hwbSections = //
        m_hwbSpinBox->sectionConfigurations();
    if (hwbSections.count() != 3) {
        qWarning() //
            << "Expected 3 sections in HSV MultiSpinBox, but got" //
            << hwbSections.count() //
            << "instead. This is a bug in libperceptualcolor.";
    } else {
        hwbSections[0].setPrefix(arcDegreeInSpinbox.first);
        hwbSections[0].setSuffix( //
            arcDegreeInSpinbox.second + m_multispinboxSectionSeparator);
        hwbSections[1].setPrefix( //
            m_multispinboxSectionSeparator + percentageInSpinbox.first);
        hwbSections[1].setSuffix( //
            percentageInSpinbox.second + m_multispinboxSectionSeparator);
        hwbSections[2].setPrefix( //
            m_multispinboxSectionSeparator + percentageInSpinbox.first);
        hwbSections[2].setSuffix( //
            percentageInSpinbox.second);
        m_hwbSpinBox->setSectionConfigurations(hwbSections);
    }

    // HSV spin box
    QList<MultiSpinBoxSection> hsvSections = //
        m_hsvSpinBox->sectionConfigurations();
    if (hsvSections.count() != 3) {
        qWarning() //
            << "Expected 3 sections in HSV MultiSpinBox, but got" //
            << hsvSections.count() //
            << "instead. This is a bug in libperceptualcolor.";
    } else {
        hsvSections[0].setPrefix(arcDegreeInSpinbox.first);
        hsvSections[0].setSuffix( //
            arcDegreeInSpinbox.second + m_multispinboxSectionSeparator);
        hsvSections[1].setPrefix( //
            m_multispinboxSectionSeparator + percentageInSpinbox.first);
        hsvSections[1].setSuffix( //
            percentageInSpinbox.second + m_multispinboxSectionSeparator);
        hsvSections[2].setPrefix( //
            m_multispinboxSectionSeparator + percentageInSpinbox.first);
        hsvSections[2].setSuffix(percentageInSpinbox.second);
        m_hsvSpinBox->setSectionConfigurations(hsvSections);
    }

    // CIEHLC-D50 spin box
    QList<MultiSpinBoxSection> ciehlcD50Sections = //
        m_ciehlcD50SpinBox->sectionConfigurations();
    if (ciehlcD50Sections.count() != 3) {
        qWarning() //
            << "Expected 3 sections in HLC MultiSpinBox, but got" //
            << ciehlcD50Sections.count() //
            << "instead. This is a bug in libperceptualcolor.";
    } else {
        ciehlcD50Sections[0].setPrefix(arcDegreeInSpinbox.first);
        ciehlcD50Sections[0].setSuffix( //
            arcDegreeInSpinbox.second + m_multispinboxSectionSeparator);
        ciehlcD50Sections[1].setPrefix( //
            m_multispinboxSectionSeparator + percentageInSpinbox.first);
        ciehlcD50Sections[1].setSuffix( //
            percentageInSpinbox.second + m_multispinboxSectionSeparator);
        ciehlcD50Sections[2].setPrefix(m_multispinboxSectionSeparator);
        ciehlcD50Sections[2].setSuffix(QString());
        m_ciehlcD50SpinBox->setSectionConfigurations(ciehlcD50Sections);
    }

    // Oklch spin box
    QList<MultiSpinBoxSection> oklchSections = //
        m_oklchSpinBox->sectionConfigurations();
    if (oklchSections.count() != 3) {
        qWarning() //
            << "Expected 3 sections in HLC MultiSpinBox, but got" //
            << oklchSections.count() //
            << "instead. This is a bug in libperceptualcolor.";
    } else {
        oklchSections[0].setPrefix(QString());
        oklchSections[0].setSuffix(m_multispinboxSectionSeparator);
        oklchSections[1].setPrefix(m_multispinboxSectionSeparator);
        oklchSections[1].setSuffix(m_multispinboxSectionSeparator);
        oklchSections[2].setPrefix( //
            m_multispinboxSectionSeparator + arcDegreeInSpinbox.first);
        oklchSections[2].setSuffix(arcDegreeInSpinbox.second);
        m_oklchSpinBox->setSectionConfigurations(oklchSections);
    }

    if (m_screenColorPickerButton) {
        /*: @action:button (eye dropper/pipette).
        A click on the button transforms the mouse cursor to a cross and lets
        the user choose a color from the screen by doing a left-click.
        Same text as in QColorDialog */
        const auto mnemonic = tr("&Pick screen color");
        m_screenColorPickerButton->setToolTip( //
            richTextMarker + fromMnemonicToRichText(mnemonic));
        m_screenColorPickerButton->setShortcut( //
            QKeySequence::mnemonic(mnemonic));
    }

    /*: @info:tooltip Tooltip for the gamut-correction action.
    The icon for this action is only visible in the UI while the
    color value within the corresponding spinbox is an out-of-gamut
    value. A click on the icon will change the spinbox’s values to
    the nearest in-gamut color (and make the icon disappear). */
    const auto gamutMnemonic = //
        tr("Click to snap to nearest in-&gamut color");
    const QString gamutTooltip = //
        richTextMarker + fromMnemonicToRichText(gamutMnemonic);
    const auto gamutShortcut = QKeySequence::mnemonic(gamutMnemonic);
    m_ciehlcD50SpinBoxGamutAction->setToolTip(gamutTooltip);
    m_ciehlcD50SpinBoxGamutAction->setShortcut(gamutShortcut);
    m_oklchSpinBoxGamutAction->setToolTip(gamutTooltip);
    m_oklchSpinBoxGamutAction->setShortcut(gamutShortcut);

    // NOTE No need to call
    //
    // q_pointer->adjustSize();
    //
    // because our layout adopts automatically to the
    // new size of the strings. Indeed, calling
    //
    // q_pointer->adjustSize();
    //
    // would change the height (!) of the widget: While it might seem
    // reasonable that the width changes when the strings change, the
    // height should not. We didn’t find the reason and didn’t manage
    // to reproduce this behaviour within the unit tests. But anyway
    // the call is not necessary, as mentioned earlier.
}

/** @brief Reloads all icons, adapting to the current color schema and
 * widget style. */
void ColorDialogPrivate::reloadIcons()
{
    QScopedPointer<QLabel> label{new QLabel(q_pointer)};
    label->setText(QStringLiteral("abc"));
    label->resize(label->sizeHint()); // Smaller size means faster guess.
    ColorSchemeType newType = guessColorSchemeTypeFromWidget(label.data()) //
                                  .value_or(newType);

    m_currentIconThemeType = newType;

    static const QStringList paletteIcons //
        {QStringLiteral("paint-swatch"),
         // For “symbolic” (monochromatic) vs “full-color” icons, see
         // https://pointieststick.com/2023/08/12/how-all-this-icon-stuff-is-going-to-work-in-plasma-6/
         QStringLiteral("palette"),
         QStringLiteral("palette-symbolic")};
    const int paletteIndex = //
        m_tabWidget->indexOf(m_paletteWrapperWidget);
    if (paletteIndex >= 0) {
        m_tabWidget->setTabIcon(paletteIndex, //
                                qIconFromTheme(paletteIcons, //
                                               QStringLiteral("color-swatch"),
                                               newType));
    }

    static const QStringList hueFirstIcons //
        {
            QStringLiteral("color-mode-hue-shift-positive"),
        };
    const int hueFirstIndex = //
        m_tabWidget->indexOf(m_hueFirstWrapperWidget);
    if (hueFirstIndex >= 0) {
        m_tabWidget->setTabIcon(hueFirstIndex, //
                                qIconFromTheme(hueFirstIcons, //
                                               QStringLiteral("steering-wheel"),
                                               newType));
    }

    static const QStringList lightnessFirstIcons //
        {
            QStringLiteral("brightness-high"),
        };
    const int lightnessFirstIndex = //
        m_tabWidget->indexOf(m_lightnessFirstWrapperWidget);
    if (lightnessFirstIndex >= 0) {
        m_tabWidget->setTabIcon(lightnessFirstIndex, //
                                qIconFromTheme(lightnessFirstIcons, //
                                               QStringLiteral("brightness-2"),
                                               newType));
    }

    static const QStringList numericIcons //
        {
            QStringLiteral("black_sum"),
        };
    const int numericIndex = //
        m_tabWidget->indexOf(m_numericalWidget);
    if (numericIndex >= 0) {
        m_tabWidget->setTabIcon(numericIndex, //
                                qIconFromTheme(numericIcons, //
                                               QStringLiteral("123"),
                                               newType));
    }

    // Gamut button for some spin boxes
    static const QStringList gamutIconNames //
        {
            QStringLiteral("data-warning"),
            QStringLiteral("dialog-warning-symbolic"),
        };
    const QIcon gamutIcon = qIconFromTheme(gamutIconNames, //
                                           QStringLiteral("eye-exclamation"),
                                           newType);
    m_ciehlcD50SpinBoxGamutAction->setIcon(gamutIcon);
    m_oklchSpinBoxGamutAction->setIcon(gamutIcon);

    static const QStringList candidates //
        {
            QStringLiteral("color-picker"), //
            QStringLiteral("gtk-color-picker"), //
            QStringLiteral("tool_color_picker"), //
        };
    if (!m_screenColorPickerButton.isNull()) {
        m_screenColorPickerButton->setIcon( //
            qIconFromTheme(candidates, //
                           QStringLiteral("color-picker"),
                           newType));
    }
}

/** @brief Basic initialization.
 *
 * @param colorSpace The color space within which this widget should operate.
 * Can be created with @ref RgbColorSpaceFactory.
 *
 * Code that is shared between the various overloaded constructors.
 *
 * @todo The RTL layout is broken for @ref SwatchBook. Thought a stretch
 * is added in the layout, the @ref SwatchBook stays left-aligned
 * instead of right-aligned if there is too much space. Why doesn’t this
 * right-align? For @ref m_wheelColorPicker and @ref m_chromaHueDiagram
 * the same code works fine! */
void ColorDialogPrivate::initialize(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace)
{
    // Do not show the “?” button in the window title. This button is displayed
    // by default on widgets that inherit from QDialog. But we do not want the
    // button because we do not provide What’s-This-help anyway, so having
    // the button would be confusing.
    q_pointer->setWindowFlag(Qt::WindowContextHelpButtonHint, false);

    // initialize color space and its dependencies
    m_rgbColorSpace = colorSpace;
    m_wcsBasicColors = wcsBasicColors(colorSpace);
    m_wcsBasicDefaultColor = m_wcsBasicColors.value(4, 2);

    // create the graphical selectors
    m_swatchBook = new SwatchBook(m_rgbColorSpace, //
                                  m_wcsBasicColors, //
                                  Qt::Orientation::Horizontal);
    QHBoxLayout *paletteInnerLayout = new QHBoxLayout();
    paletteInnerLayout->addWidget(m_swatchBook);
    paletteInnerLayout->addStretch();
    QVBoxLayout *paletteOuterLayout = new QVBoxLayout();
    paletteOuterLayout->addLayout(paletteInnerLayout);
    paletteOuterLayout->addStretch();
    m_paletteWrapperWidget = new QWidget();
    m_paletteWrapperWidget->setLayout(paletteOuterLayout);

    m_wheelColorPicker = new WheelColorPicker(m_rgbColorSpace);
    m_hueFirstWrapperWidget = new QWidget;
    QHBoxLayout *tempHueFirstLayout = new QHBoxLayout;
    tempHueFirstLayout->addWidget(m_wheelColorPicker);
    m_hueFirstWrapperWidget->setLayout(tempHueFirstLayout);

    m_lchLightnessSelector = new GradientSlider(m_rgbColorSpace);
    LchaDouble black;
    black.l = 0;
    black.c = 0;
    black.h = 0;
    black.a = 1;
    LchaDouble white;
    white.l = 100;
    white.c = 0;
    white.h = 0;
    white.a = 1;
    m_lchLightnessSelector->setColors(black, white);
    m_chromaHueDiagram = new ChromaHueDiagram(m_rgbColorSpace);
    QHBoxLayout *tempLightnesFirstLayout = new QHBoxLayout();
    tempLightnesFirstLayout->addWidget(m_lchLightnessSelector);
    tempLightnesFirstLayout->addWidget(m_chromaHueDiagram);
    m_lightnessFirstWrapperWidget = new QWidget();
    m_lightnessFirstWrapperWidget->setLayout(tempLightnesFirstLayout);

    initializeScreenColorPicker();

    m_tabWidget = new QTabWidget;
    // It would be good to have bigger icons. Via QStyle::pixelMetrics()
    // we could get values for this. QStyle::PM_LargeIconSize seems to large,
    // be we could use std::max() with QStyle::PM_ToolBarIconSize,
    // QStyle::PM_SmallIconSize, QStyle::PM_TabBarIconSize,
    // QStyle::PM_ButtonIconSize. But the problem is a regression in Qt6
    // (compared to Qt5) that breaks rendering of bigger icons via
    // QTabWidget::iconSize(): https://bugreports.qt.io/browse/QTBUG-114849
    // Furthermore, it appears that the MacOS style does not adjust the height
    // of the tab bar to match the icon height. This causes larger icons to
    // simply overflow, which looks like a rendering issue. Therefore,
    // currently we stick with the default icons size for tab bars.
    m_tabWidget->addTab(m_paletteWrapperWidget, QString());
    m_paletteTabShortcut = new QShortcut(q_pointer);
    connect(m_paletteTabShortcut, //
            &QShortcut::activated,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_paletteWrapperWidget));
            });
    connect(m_paletteTabShortcut, //
            &QShortcut::activatedAmbiguously,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_paletteWrapperWidget));
            });

    m_tabWidget->addTab(m_hueFirstWrapperWidget, QString());
    m_hueFirstTabShortcut = new QShortcut(q_pointer);
    connect(m_hueFirstTabShortcut, //
            &QShortcut::activated,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_hueFirstWrapperWidget));
            });
    connect(m_hueFirstTabShortcut, //
            &QShortcut::activatedAmbiguously,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_hueFirstWrapperWidget));
            });

    m_tabWidget->addTab(m_lightnessFirstWrapperWidget, QString());
    m_lightnessFirstTabShortcut = new QShortcut(q_pointer);
    connect(m_lightnessFirstTabShortcut, //
            &QShortcut::activated,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_lightnessFirstWrapperWidget));
            });
    connect(m_lightnessFirstTabShortcut, //
            &QShortcut::activatedAmbiguously,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_lightnessFirstWrapperWidget));
            });

    m_tabTable.insert(&m_paletteWrapperWidget, //
                      QStringLiteral("swatch"));
    m_tabTable.insert(&m_hueFirstWrapperWidget, //
                      QStringLiteral("hue-based"));
    m_tabTable.insert(&m_lightnessFirstWrapperWidget, //
                      QStringLiteral("lightness-based"));
    m_tabTable.insert(&m_numericalWidget, //
                      QStringLiteral("numerical"));
    connect(m_tabWidget, //
            &QTabWidget::currentChanged, //
            this, //
            &ColorDialogPrivate::saveCurrentTab);

    // Create the ColorPatch
    m_colorPatch = new ColorPatch();
    m_colorPatch->setMinimumSize(m_colorPatch->minimumSizeHint() * 1.5);

    QHBoxLayout *headerLayout = new QHBoxLayout();
    headerLayout->addWidget(m_colorPatch, 1);
    m_screenColorPickerButton->setSizePolicy(QSizePolicy::Minimum, // horizontal
                                             QSizePolicy::Minimum); // vertical
    headerLayout->addWidget(m_screenColorPickerButton,
                            // Do not grow the cell in the direction
                            // of the QBoxLayout:
                            0,
                            // No alignment: Fill the entire cell.
                            Qt::Alignment());

    // Create widget for the numerical values
    m_numericalWidget = initializeNumericPage();
    m_numericalTabShortcut = new QShortcut(q_pointer);
    connect(m_numericalTabShortcut, //
            &QShortcut::activated,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_numericalWidget));
            });
    connect(m_numericalTabShortcut, //
            &QShortcut::activatedAmbiguously,
            this,
            [this]() {
                m_tabWidget->setCurrentIndex( //
                    m_tabWidget->indexOf(m_numericalWidget));
            });

    // Create layout for graphical and numerical widgets
    m_selectorLayout = new QHBoxLayout();
    m_selectorLayout->addWidget(m_tabWidget);
    m_selectorLayout->addWidget(m_numericalWidget);

    // Create widgets for alpha value
    QHBoxLayout *m_alphaLayout = new QHBoxLayout();
    m_alphaGradientSlider = new GradientSlider(m_rgbColorSpace, //
                                               Qt::Orientation::Horizontal);
    m_alphaGradientSlider->setSingleStep(singleStepAlpha);
    m_alphaGradientSlider->setPageStep(pageStepAlpha);
    m_alphaSpinBox = new QDoubleSpinBox();
    m_alphaSpinBox->setAlignment(Qt::AlignmentFlag::AlignRight);
    m_alphaSpinBox->setMinimum(0);
    m_alphaSpinBox->setMaximum(100);
    // The suffix is set in retranslateUi.
    m_alphaSpinBox->setDecimals(decimals);
    m_alphaSpinBox->setSingleStep(singleStepAlpha * 100);
    // m_alphaSpinBox is of type QDoubleSpinBox which does not allow to
    // configure the pageStep.
    m_alphaLabel = new QLabel();
    m_alphaLabel->setBuddy(m_alphaSpinBox);
    m_alphaLayout->addWidget(m_alphaLabel);
    m_alphaLayout->addWidget(m_alphaGradientSlider);
    m_alphaLayout->addWidget(m_alphaSpinBox);

    // Create the default buttons
    // We use standard buttons, because these standard buttons are
    // created by Qt and have automatically the correct icons and so on
    // (as designated in the current platform and widget style).
    // Though we use standard buttons, (later) we set the text manually to
    // get full control over the translation. Otherwise, loading a
    // different translation files than the user’s QLocale::system()
    // default locale would not update the standard button texts.
    m_buttonBox = new QDialogButtonBox();
    // NOTE We start with the OK button, and not with the Cancel button.
    // This is because apparently, the first button becomes the default
    // one (though Qt documentation says differently). If Cancel would
    // be the first, it would become the default button, which is not
    // what we want. (Even QPushButton::setDefault() will not change this
    // afterwards.)
    m_buttonOK = m_buttonBox->addButton(QDialogButtonBox::Ok);
    m_buttonCancel = m_buttonBox->addButton(QDialogButtonBox::Cancel);
    // The Qt documentation at
    // https://doc.qt.io/qt-5/qcoreapplication.html#installTranslator
    // says that Qt::LanguageChange events are only send to top-level
    // widgets. However, our experience is that also the QDialogButtonBox
    // receives Qt::LanguageChange events and reacts on it by updating
    // the user-visible string of all standard buttons. We do not want
    // to use custom buttons because of the advantages of standard
    // buttons that are described above. On the other hand, we do not
    // want Qt to change our string because we use our own translation
    // here.
    m_buttonBox->installEventFilter(&m_languageChangeEventFilter);
    m_buttonOK->installEventFilter(&m_languageChangeEventFilter);
    m_buttonCancel->installEventFilter(&m_languageChangeEventFilter);
    connect(m_buttonBox, // sender
            &QDialogButtonBox::accepted, // signal
            q_pointer, // receiver
            &PerceptualColor::ColorDialog::accept); // slot
    connect(m_buttonBox, // sender
            &QDialogButtonBox::rejected, // signal
            q_pointer, // receiver
            &PerceptualColor::ColorDialog::reject); // slot

    // Create the main layout
    QVBoxLayout *tempMainLayout = new QVBoxLayout();
    tempMainLayout->addLayout(headerLayout);
    tempMainLayout->addLayout(m_selectorLayout);
    tempMainLayout->addLayout(m_alphaLayout);
    tempMainLayout->addWidget(m_buttonBox);
    q_pointer->setLayout(tempMainLayout);

    // initialize signal-slot-connections
    connect(m_colorPatch, // sender
            &ColorPatch::colorChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readColorPatchValue // slot
    );
    connect(m_swatchBook, // sender
            &SwatchBook::currentColorChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readSwatchBookValue // slot
    );
    connect(m_rgbSpinBox, // sender
            &MultiSpinBox::sectionValuesChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readRgbNumericValues // slot
    );
    connect(m_rgbLineEdit, // sender
            &QLineEdit::textChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readRgbHexValues // slot
    );
    connect(m_rgbLineEdit, // sender
            &QLineEdit::editingFinished, // signal
            this, // receiver
            &ColorDialogPrivate::updateRgbHexButBlockSignals // slot
    );
    connect(m_hslSpinBox, // sender
            &MultiSpinBox::sectionValuesChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readHslNumericValues // slot
    );
    connect(m_hwbSpinBox, // sender
            &MultiSpinBox::sectionValuesChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readHwbNumericValues // slot
    );
    connect(m_hsvSpinBox, // sender
            &MultiSpinBox::sectionValuesChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readHsvNumericValues // slot
    );
    connect(m_ciehlcD50SpinBox, // sender
            &MultiSpinBox::sectionValuesChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readHlcNumericValues // slot
    );
    connect(m_ciehlcD50SpinBox, // sender
            &MultiSpinBox::editingFinished, // signal
            this, // receiver
            &ColorDialogPrivate::updateHlcButBlockSignals // slot
    );
    connect(m_oklchSpinBox, // sender
            &MultiSpinBox::sectionValuesChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readOklchNumericValues // slot
    );
    connect(m_oklchSpinBox, // sender
            &MultiSpinBox::editingFinished, // signal
            this, // receiver
            &ColorDialogPrivate::updateOklchButBlockSignals // slot
    );
    connect(m_lchLightnessSelector, // sender
            &GradientSlider::valueChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readLightnessValue // slot
    );
    connect(m_wheelColorPicker, // sender
            &WheelColorPicker::currentColorChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readWheelColorPickerValues // slot
    );
    connect(m_chromaHueDiagram, // sender
            &ChromaHueDiagram::currentColorChanged, // signal
            this, // receiver
            &ColorDialogPrivate::readChromaHueDiagramValue // slot
    );
    connect(m_alphaGradientSlider, // sender
            &GradientSlider::valueChanged, // signal
            this, // receiver
            &ColorDialogPrivate::updateColorPatch // slot
    );
    connect(m_alphaGradientSlider, // sender
            &GradientSlider::valueChanged, // signal
            this, // receiver
            [this](const qreal newFraction) { // lambda
                const QSignalBlocker blocker(m_alphaSpinBox);
                m_alphaSpinBox->setValue(newFraction * 100);
            });
    connect(m_alphaSpinBox, // sender
            QOverload<double>::of(&QDoubleSpinBox::valueChanged), // signal
            this, // receiver
            [this](const double newValue) { // lambda
                // m_alphaGradientSlider has range [0, 1], while the signal
                // has range [0, 100]. This has to be adapted:
                m_alphaGradientSlider->setValue(newValue / 100);
            });

    // Initialize the options
    q_pointer->setOptions(QColorDialog::ColorDialogOption::DontUseNativeDialog);

    // We are setting the translated default window title here instead
    // of setting it within retranslateUi(). This is because also QColorDialog
    // does not update the window title on LanguageChange events (probably
    // to avoid confusion, because it’s difficult to tell exactly if the
    // library user did or did not explicitly change the window title.
    /*: @title:window Default window title. Same text as in QColorDialog */
    q_pointer->setWindowTitle(tr("Select color"));

    // Enable size grip
    // As this dialog can indeed be resized, the size grip should
    // be enabled. So, users can see the little triangle at the
    // right bottom of the dialog (or the left bottom on a
    // right-to-left layout). So, the user will be aware
    // that he can indeed resize this dialog, which is
    // important as the users are used to the default
    // platform dialog, which often do not allow resizing. Therefore,
    // by default, QDialog::isSizeGripEnabled() should be true.
    // NOTE: Some widget styles like Oxygen or Breeze leave the size grip
    // widget invisible; nevertheless it reacts on mouse events. Other
    // widget styles indeed show the size grip widget, like Fusion or
    // QtCurve.
    q_pointer->setSizeGripEnabled(true);

    // The q_pointer’s object is still not fully initialized at this point,
    // but it’s base class constructor has fully run; this should be enough
    // to use functionality based on QWidget, so we can use it as parent.
    m_ciehlcD50SpinBoxGamutAction = new QAction(q_pointer);
    connect(m_ciehlcD50SpinBoxGamutAction, // sender
            &QAction::triggered, // signal
            this, // receiver
            &ColorDialogPrivate::updateHlcButBlockSignals // slot
    );
    m_oklchSpinBoxGamutAction = new QAction(q_pointer);
    connect(m_oklchSpinBoxGamutAction, // sender
            &QAction::triggered, // signal
            this, // receiver
            &ColorDialogPrivate::updateOklchButBlockSignals // slot
    );
    // However, here we hide the action because initially the
    // current color should be in-gamut, so no need for the gamut action
    // to be visible.
    m_ciehlcD50SpinBoxGamutAction->setVisible(false);
    m_ciehlcD50SpinBox->addActionButton( //
        m_ciehlcD50SpinBoxGamutAction, //
        QLineEdit::ActionPosition::TrailingPosition);
    m_oklchSpinBoxGamutAction->setVisible(false);
    m_oklchSpinBox->addActionButton( //
        m_oklchSpinBoxGamutAction, //
        QLineEdit::ActionPosition::TrailingPosition);

    initializeTranslation(QCoreApplication::instance(),
                          // An empty std::optional means: If in initialization
                          // had been done yet, repeat this initialization.
                          // If not, do a new initialization now with default
                          // values.
                          std::optional<QStringList>());
    retranslateUi();

    reloadIcons();
#if (QT_VERSION >= QT_VERSION_CHECK(6, 5, 0))
    connect(qGuiApp->styleHints(), // sender
            &QStyleHints::colorSchemeChanged, // signal
            this, // receiver
            &ColorDialogPrivate::reloadIcons);
#endif
}

/** @brief Constructor
 *
 *  @param parent pointer to the parent widget, if any
 *  @post The @ref currentColor property is set to a default value. */
ColorDialog::ColorDialog(QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize(RgbColorSpaceFactory::createSrgb());
    setCurrentColor(d_pointer->m_wcsBasicDefaultColor);
}

/** @brief Constructor
 *
 *  @param initial the initially chosen color of the dialog
 *  @param parent pointer to the parent widget, if any
 *  @post The object is constructed and @ref setCurrentColor() is called
 *  with <em>initial</em>. See @ref setCurrentColor() for the modifications
 *  that will be applied before setting the current color. Especially, as
 *  this dialog is constructed by default without alpha support, the
 *  alpha channel of <em>initial</em> is ignored and a fully opaque color is
 *  used. */
ColorDialog::ColorDialog(const QColor &initial, QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize(RgbColorSpaceFactory::createSrgb());
    // Calling setCurrentColor() guaranties to update all widgets
    // because it always sets a valid color, even when the color
    // parameter was invalid. As m_currentOpaqueColor is invalid
    // be default, and therefor different, setCurrentColor()
    // guaranties to update all widgets.
    setCurrentColor(initial);
}

/** @brief Constructor
 *
 *  @param colorSpace The color space within which this widget should operate.
 *  Can be created with @ref RgbColorSpaceFactory.
 *  @param parent pointer to the parent widget, if any
 *  @post The @ref currentColor property is set to a default value. */
ColorDialog::ColorDialog(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize(colorSpace);
    setCurrentColor(d_pointer->m_wcsBasicDefaultColor);
}

/** @brief Constructor
 *
 *  @param colorSpace The color space within which this widget should operate.
 *  Can be created with @ref RgbColorSpaceFactory.
 *  @param initial the initially chosen color of the dialog
 *  @param parent pointer to the parent widget, if any
 *  @post The object is constructed and @ref setCurrentColor() is called
 *  with <em>initial</em>. See @ref setCurrentColor() for the modifications
 *  that will be applied before setting the current color. Especially, as
 *  this dialog is constructed by default without alpha support, the
 *  alpha channel of <em>initial</em> is ignored and a fully opaque color is
 *  used. */
ColorDialog::ColorDialog(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, const QColor &initial, QWidget *parent)
    : QDialog(parent)
    , d_pointer(new ColorDialogPrivate(this))
{
    d_pointer->initialize(colorSpace);
    // Calling setCurrentColor() guaranties to update all widgets
    // because it always sets a valid color, even when the color
    // parameter was invalid. As m_currentOpaqueColor is invalid
    // be default, and therefor different, setCurrentColor()
    // guaranties to update all widgets.
    setCurrentColor(initial);
}

/** @brief Destructor */
ColorDialog::~ColorDialog() noexcept
{
    // All the layouts and widgets used here are automatically child widgets
    // of this dialog widget. Therefor they are deleted automatically.
    // Also m_rgbColorSpace is of type RgbColorSpace(), which
    // inherits from QObject, and is a child of this dialog widget, does
    // not need to be deleted manually.
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
ColorDialogPrivate::ColorDialogPrivate(ColorDialog *backLink)
    : q_pointer(backLink)
{
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColor ColorDialog::currentColor() const
{
    QColor temp = d_pointer->m_currentOpaqueColorRgb.rgbQColor;
    temp.setAlphaF( //
        static_cast<QColorFloatType>( //
            d_pointer->m_alphaGradientSlider->value()));
    return temp;
}

/** @brief Setter for @ref currentColor property.
 *
 * @param color the new color
 * @post The property @ref currentColor is adapted as follows:
 * - If <em>color</em> is not valid, <tt>Qt::black</tt> is used instead.
 * - If <em>color</em>’s <tt>QColor::Spec</tt> is <em>not</em>
 *   <tt>QColor::Spec::Rgb</tt> then it will be converted silently
 *   to <tt>QColor::Spec::Rgb</tt>
 * - The RGB part of @ref currentColor will be the RGB part of <tt>color</tt>.
 * - The alpha channel of @ref currentColor will be the alpha channel
 *   of <tt>color</tt> if at the moment of the function call
 *   the <tt>QColorDialog::ColorDialogOption::ShowAlphaChannel</tt> option is
 *   set. It will be fully opaque otherwise. */
void ColorDialog::setCurrentColor(const QColor &color)
{
    QColor temp;
    if (color.isValid()) {
        // Make sure that the QColor::spec() is QColor::Spec::Rgb.
        temp = color.toRgb();
    } else {
        // For invalid colors same behavior as QColorDialog
        temp = QColor(Qt::black);
    }
    if (testOption(ColorDialog::ColorDialogOption::ShowAlphaChannel)) {
        d_pointer->m_alphaGradientSlider->setValue( //
            static_cast<double>(temp.alphaF()));
    } else {
        d_pointer->m_alphaGradientSlider->setValue(1);
    }
    // No need to update m_alphaSpinBox as this is done
    // automatically by signals emitted by m_alphaGradientSlider.
    const RgbColor myRgbColor = RgbColor::fromRgbQColor(temp);
    d_pointer->setCurrentOpaqueColor(myRgbColor, nullptr);
}

/** @brief Opens the dialog and connects its @ref colorSelected() signal to
 * the slot specified by receiver and member.
 *
 * The signal will be disconnected from the slot when the dialog is closed.
 *
 * Example:
 * @snippet testcolordialog.cpp ColorDialog Open
 *
 * @param receiver the object that will receive the @ref colorSelected() signal
 * @param member the slot that will receive the @ref colorSelected() signal */
void ColorDialog::open(QObject *receiver, const char *member)
{
    connect(this, // sender
            SIGNAL(colorSelected(QColor)), // signal
            receiver, // receiver
            member); // slot
    d_pointer->m_receiverToBeDisconnected = receiver;
    d_pointer->m_memberToBeDisconnected = member;
    QDialog::open();
}

/** @brief Updates the color patch widget
 *
 * @post The color patch widget will show the color
 * of @ref m_currentOpaqueColorRgb and the alpha
 * value of @ref m_alphaGradientSlider. */
void ColorDialogPrivate::updateColorPatch()
{
    QColor tempRgbQColor = m_currentOpaqueColorRgb.rgbQColor;
    tempRgbQColor.setAlphaF( //
        static_cast<QColorFloatType>(m_alphaGradientSlider->value()));
    m_colorPatch->setColor(tempRgbQColor);
}

/** @brief Overloaded function. */
void ColorDialogPrivate::setCurrentOpaqueColor(const QHash<PerceptualColor::ColorModel, PerceptualColor::GenericColor> &abs, QWidget *const ignoreWidget)
{
    const auto cielchD50 = //
        abs.value(ColorModel::CielchD50).reinterpretAsLchToLchDouble();
    const auto rgb1 = m_rgbColorSpace->fromCielchD50ToRgb1(cielchD50);
    const auto rgb255 = GenericColor(rgb1.first * 255, //
                                     rgb1.second * 255,
                                     rgb1.third * 255);
    const auto rgbColor = RgbColor::fromRgb255(rgb255);
    setCurrentOpaqueColor(abs, rgbColor, ignoreWidget);
}

/** @brief Overloaded function. */
void ColorDialogPrivate::setCurrentOpaqueColor(const PerceptualColor::RgbColor &rgb, QWidget *const ignoreWidget)
{
    const auto temp = rgb.rgb255;
    const QColor myQColor = QColor::fromRgbF( //
        static_cast<QColorFloatType>(temp.first / 255.), //
        static_cast<QColorFloatType>(temp.second / 255.), //
        static_cast<QColorFloatType>(temp.third / 255.));
    const auto cielchD50 = GenericColor( //
        m_rgbColorSpace->toCielchD50Double(myQColor.rgba64()));
    setCurrentOpaqueColor( //
        AbsoluteColor::allConversions(ColorModel::CielchD50, cielchD50),
        rgb,
        ignoreWidget);
}

/** @brief Updates @ref m_currentOpaqueColorAbs, @ref m_currentOpaqueColorRgb
 * and affected widgets.
 *
 * @param abs The new color in absolute color models
 * @param rgb The new color in RGB and RGB-derived models (profile-dependant)
 *
 * @param ignoreWidget A widget that should <em>not</em> be updated. Or
 * <tt>nullptr</tt> to update <em>all</em> widgets.
 *
 * @post If this function is called recursively, nothing happens. Else
 * the color is moved into the gamut, then @ref m_currentOpaqueColorAbs and
 * @ref m_currentOpaqueColorRgb are updated, and the corresponding widgets
 * are updated (except the widget specified to be ignored – if any).
 *
 * @note Recursive functions calls are ignored. This is useful, because you
 * can connect signals from various widgets to this slot without having to
 * worry about infinite recursions. */
void ColorDialogPrivate::setCurrentOpaqueColor(const QHash<PerceptualColor::ColorModel, PerceptualColor::GenericColor> &abs,
                                               const PerceptualColor::RgbColor &rgb,
                                               QWidget *const ignoreWidget)
{
    const bool isIdentical = (abs == m_currentOpaqueColorAbs) && (rgb == m_currentOpaqueColorRgb);
    if (m_isColorChangeInProgress || isIdentical) {
        // Nothing to do!
        return;
    }

    // If we have really some work to do, block recursive calls
    // of this function
    m_isColorChangeInProgress = true;

    // Save currentColor() for later comparison
    // Using currentColor() makes sure correct alpha treatment!
    QColor oldQColor = q_pointer->currentColor();

    // Update m_currentOpaqueColor
    m_currentOpaqueColorAbs = abs;
    m_currentOpaqueColorRgb = rgb;

    // Update palette
    if (m_swatchBook != ignoreWidget) {
        m_swatchBook->setCurrentColor(m_currentOpaqueColorRgb.rgbQColor);
    }

    // Update RGB widget
    if (m_rgbSpinBox != ignoreWidget) {
        m_rgbSpinBox->setSectionValues( //
            m_currentOpaqueColorRgb.rgb255.toQList3());
    }

    // Update HSL widget
    if (m_hslSpinBox != ignoreWidget) {
        m_hslSpinBox->setSectionValues( //
            m_currentOpaqueColorRgb.hsl.toQList3());
    }

    // Update HWB widget
    if (m_hwbSpinBox != ignoreWidget) {
        m_hwbSpinBox->setSectionValues( //
            m_currentOpaqueColorRgb.hwb.toQList3());
    }

    // Update HSV widget
    if (m_hsvSpinBox != ignoreWidget) {
        m_hsvSpinBox->setSectionValues( //
            m_currentOpaqueColorRgb.hsv.toQList3());
    }

    // Update CIEHLC-D50 widget
    const auto cielchD50 = m_currentOpaqueColorAbs.value(ColorModel::CielchD50);
    const auto ciehlcD50 = QList<double>{cielchD50.third, //
                                         cielchD50.first,
                                         cielchD50.second};
    if (m_ciehlcD50SpinBox != ignoreWidget) {
        m_ciehlcD50SpinBox->setSectionValues(ciehlcD50);
    }

    // Update Oklch widget
    const auto oklch = m_currentOpaqueColorAbs.value(ColorModel::OklchD65);
    if (m_oklchSpinBox != ignoreWidget) {
        m_oklchSpinBox->setSectionValues(oklch.toQList3());
    }

    // Update RGB hex widget
    if (m_rgbLineEdit != ignoreWidget) {
        updateRgbHexButBlockSignals();
    }

    // Update lightness selector
    if (m_lchLightnessSelector != ignoreWidget) {
        m_lchLightnessSelector->setValue( //
            cielchD50.first / static_cast<qreal>(100));
    }

    // Update chroma-hue diagram
    if (m_chromaHueDiagram != ignoreWidget) {
        m_chromaHueDiagram->setCurrentColor( //
            cielchD50.reinterpretAsLchToLchDouble());
    }

    // Update wheel color picker
    if (m_wheelColorPicker != ignoreWidget) {
        m_wheelColorPicker->setCurrentColor( //
            cielchD50.reinterpretAsLchToLchDouble());
    }

    // Update alpha gradient slider
    if (m_alphaGradientSlider != ignoreWidget) {
        LchaDouble tempColor;
        tempColor.l = cielchD50.first;
        tempColor.c = cielchD50.second;
        tempColor.h = cielchD50.third;
        tempColor.a = 0;
        m_alphaGradientSlider->setFirstColor(tempColor);
        tempColor.a = 1;
        m_alphaGradientSlider->setSecondColor(tempColor);
    }

    // Update widgets that take alpha information
    if (m_colorPatch != ignoreWidget) {
        updateColorPatch();
    }

    // Emit signal currentColorChanged() only if necessary
    if (q_pointer->currentColor() != oldQColor) {
        Q_EMIT q_pointer->currentColorChanged(q_pointer->currentColor());
    }

    // End of this function. Unblock recursive
    // function calls before returning.
    m_isColorChangeInProgress = false;
}

/** @brief Reads the value from the lightness selector in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readLightnessValue()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    auto cielchD50 = m_currentOpaqueColorAbs.value(ColorModel::CielchD50);
    cielchD50.first = m_lchLightnessSelector->value() * 100;
    cielchD50 = GenericColor( //
        m_rgbColorSpace->reduceCielchD50ChromaToFitIntoGamut( //
            cielchD50.reinterpretAsLchToLchDouble()));
    setCurrentOpaqueColor( //
        AbsoluteColor::allConversions(ColorModel::CielchD50, cielchD50), //
        m_lchLightnessSelector);
}

/** @brief Reads the HSL numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readHslNumericValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const auto temp = RgbColor::fromHsl( //
        GenericColor(m_hslSpinBox->sectionValues()));
    setCurrentOpaqueColor(temp, m_hslSpinBox);
}

/** @brief Reads the HWB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readHwbNumericValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const auto temp = RgbColor::fromHwb( //
        GenericColor(m_hwbSpinBox->sectionValues()));
    setCurrentOpaqueColor(temp, m_hwbSpinBox);
}

/** @brief Reads the HSV numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readHsvNumericValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const auto temp = RgbColor::fromHsv( //
        GenericColor(m_hsvSpinBox->sectionValues()));
    setCurrentOpaqueColor(temp, m_hsvSpinBox);
}

/** @brief Reads the decimal RGB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readRgbNumericValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const auto temp = RgbColor::fromRgb255( //
        GenericColor(m_rgbSpinBox->sectionValues()));
    setCurrentOpaqueColor(temp, m_rgbSpinBox);
}

/** @brief Reads the color of the color patch, and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readColorPatchValue()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const QColor temp = m_colorPatch->color();
    if (!temp.isValid()) {
        // No color is currently selected!
        return;
    }
    const auto myRgbColor = RgbColor::fromRgbQColor(temp);
    setCurrentOpaqueColor(myRgbColor, m_colorPatch);
}

/** @brief Reads the color of the palette widget, and (if any)
 * updates the dialog accordingly. */
void ColorDialogPrivate::readSwatchBookValue()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const QColor temp = m_swatchBook->currentColor();
    if (!temp.isValid()) {
        // No color is currently selected!
        return;
    }
    const auto myRgbColor = RgbColor::fromRgbQColor(temp);
    setCurrentOpaqueColor(myRgbColor, m_swatchBook);
}

/** @brief Reads the color of the @ref WheelColorPicker in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readWheelColorPickerValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const auto cielchD50 = GenericColor(m_wheelColorPicker->currentColor());
    setCurrentOpaqueColor( //
        AbsoluteColor::allConversions(ColorModel::CielchD50, cielchD50),
        m_wheelColorPicker);
}

/** @brief Reads the color of the @ref ChromaHueDiagram in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readChromaHueDiagramValue()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    const auto cielchD50 = GenericColor(m_chromaHueDiagram->currentColor());
    setCurrentOpaqueColor( //
        AbsoluteColor::allConversions(ColorModel::CielchD50, cielchD50),
        m_chromaHueDiagram);
}

/** @brief Reads the hexadecimal RGB numbers in the dialog and
 * updates the dialog accordingly. */
void ColorDialogPrivate::readRgbHexValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    QString temp = m_rgbLineEdit->text();
    if (!temp.startsWith(QStringLiteral(u"#"))) {
        temp = QStringLiteral(u"#") + temp;
    }
    QColor rgb;
    rgb.setNamedColor(temp);
    if (rgb.isValid()) {
        const auto myRgbColor = RgbColor::fromRgbQColor(rgb);
        setCurrentOpaqueColor(myRgbColor, m_rgbLineEdit);
    } else {
        m_isDirtyRgbLineEdit = true;
    }
}

/** @brief Updates the RGB Hex widget to @ref m_currentOpaqueColorRgb.
 *
 * @post The @ref m_rgbLineEdit gets the value of @ref m_currentOpaqueColorRgb.
 * During this operation, all signals of @ref m_rgbLineEdit are blocked. */
void ColorDialogPrivate::updateRgbHexButBlockSignals()
{
    QSignalBlocker mySignalBlocker(m_rgbLineEdit);

    // m_currentOpaqueColor is supposed to be always in-gamut. However,
    // because of rounding issues, a conversion to an unbounded RGB
    // color could result in an invalid color. Therefore, we must
    // use a conversion to a _bounded_ RGB color.
    const auto &rgbFloat = m_currentOpaqueColorRgb.rgb255;

    // We cannot rely on the convenient QColor.name() because this function
    // seems to use floor() instead of round(), which does not make sense in
    // our dialog, and it would be inconsistent with the other widgets
    // of the dialog. Therefore, we have to round explicitly (to integers):
    // This format string provides a non-localized format!
    // Format of the numbers:
    // 1) The number itself
    // 2) The minimal field width (2 digits)
    // 3) The base of the number representation (16, hexadecimal)
    // 4) The fill character (leading zero)
    const QString hexString = //
        QStringLiteral(u"#%1%2%3")
            .arg(qBound(0, qRound(rgbFloat.first), 255), //
                 2, //
                 16, //
                 QChar::fromLatin1('0'))
            .arg(qBound(0, qRound(rgbFloat.second), 255), //
                 2, //
                 16, //
                 QChar::fromLatin1('0'))
            .arg(qBound(0, qRound(rgbFloat.third), 255), //
                 2, //
                 16, //
                 QChar::fromLatin1('0'))
            .toUpper(); // Convert to upper case
    m_rgbLineEdit->setText(hexString);
}

/** @brief Updates the HLC spin box to @ref m_currentOpaqueColorAbs.
 *
 * @post The @ref m_ciehlcD50SpinBox gets the value of
 * @ref m_currentOpaqueColorAbs. During this operation, all signals of
 * @ref m_ciehlcD50SpinBox are blocked. */
void ColorDialogPrivate::updateHlcButBlockSignals()
{
    QSignalBlocker mySignalBlocker(m_ciehlcD50SpinBox);
    const auto cielchD50 = m_currentOpaqueColorAbs.value(ColorModel::CielchD50);
    const QList<double> ciehlcD50List{cielchD50.third, //
                                      cielchD50.first,
                                      cielchD50.second};
    m_ciehlcD50SpinBox->setSectionValues(ciehlcD50List);
    m_ciehlcD50SpinBoxGamutAction->setVisible(false);
}

/** @brief Updates the Oklch spin box to @ref m_currentOpaqueColorAbs.
 *
 * @post The @ref m_oklchSpinBox gets the value
 * of @ref m_currentOpaqueColorAbs. During this operation,
 * all signals of @ref m_oklchSpinBox are blocked. */
void ColorDialogPrivate::updateOklchButBlockSignals()
{
    QSignalBlocker mySignalBlocker(m_oklchSpinBox);
    const auto oklch = m_currentOpaqueColorAbs.value(ColorModel::OklchD65);
    m_oklchSpinBox->setSectionValues(oklch.toQList3());
    m_oklchSpinBoxGamutAction->setVisible(false);
}

/** @brief If no @ref m_isColorChangeInProgress, reads the HLC numbers
 * in the dialog and updates the dialog accordingly. */
void ColorDialogPrivate::readHlcNumericValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    QList<double> hlcValues = m_ciehlcD50SpinBox->sectionValues();
    LchDouble lch;
    lch.h = hlcValues.at(0);
    lch.l = hlcValues.at(1);
    lch.c = hlcValues.at(2);
    if (m_rgbColorSpace->isCielchD50InGamut(lch)) {
        m_ciehlcD50SpinBoxGamutAction->setVisible(false);
    } else {
        m_ciehlcD50SpinBoxGamutAction->setVisible(true);
    }
    const auto myColor = GenericColor( //
        m_rgbColorSpace->reduceCielchD50ChromaToFitIntoGamut(lch));
    setCurrentOpaqueColor( //
        AbsoluteColor::allConversions(ColorModel::CielchD50, myColor),
        // widget that will ignored during updating:
        m_ciehlcD50SpinBox);
}

/** @brief If no @ref m_isColorChangeInProgress, reads the Oklch numbers
 * in the dialog and updates the dialog accordingly. */
void ColorDialogPrivate::readOklchNumericValues()
{
    if (m_isColorChangeInProgress) {
        // Nothing to do!
        return;
    }
    // Get final color (in necessary moving the original color into gamut).
    // TODO xxx This code moves into gamut based on the Cielch-D50 instead of
    // the Oklch gamut. This leads to wrong results, because Oklch hue is not
    // guaranteed to be respected. Use actually Oklch to move into gamut!
    LchDouble originalOklch;
    originalOklch.l = m_oklchSpinBox->sectionValues().value(0);
    originalOklch.c = m_oklchSpinBox->sectionValues().value(1);
    originalOklch.h = m_oklchSpinBox->sectionValues().value(2);
    if (m_rgbColorSpace->isOklchInGamut(originalOklch)) {
        m_oklchSpinBoxGamutAction->setVisible(false);
    } else {
        m_oklchSpinBoxGamutAction->setVisible(true);
    }
    const auto inGamutOklch = GenericColor( //
        m_rgbColorSpace->reduceOklchChromaToFitIntoGamut(originalOklch));
    const auto inGamutColor = //
        AbsoluteColor::allConversions(ColorModel::OklchD65, inGamutOklch);
    setCurrentOpaqueColor(inGamutColor,
                          // widget that will ignored during updating:
                          m_oklchSpinBox);
}

/** @brief Try to initialize the screen color picker feature.
 *
 * @post If supported, @ref m_screenColorPickerButton
 * is created. Otherwise, it stays <tt>nullptr</tt>. */
void ColorDialogPrivate::initializeScreenColorPicker()
{
    auto screenPicker = new ScreenColorPicker(q_pointer);
    if (!screenPicker->isAvailable()) {
        return;
    }
    m_screenColorPickerButton = new QToolButton;
    screenPicker->setParent(m_screenColorPickerButton); // For better support
    connect(m_screenColorPickerButton,
            &QPushButton::clicked,
            screenPicker,
            // Default capture by reference, but screenPicker by value
            [&, screenPicker]() {
                const auto myColor = q_pointer->currentColor();
                // TODO Restore QColor exactly, but could potentially produce
                // rounding errors: If original MultiColor was derived form
                // LCH, it is not guaranteed that the new MultiColor derived
                // from this QColor will not have rounding errors for LCH.
                screenPicker->startPicking( //
                    fromFloatingToEightBit(myColor.redF()), //
                    fromFloatingToEightBit(myColor.greenF()), //
                    fromFloatingToEightBit(myColor.blueF()));
            });
    connect(screenPicker, //
            &ScreenColorPicker::newColor, //
            q_pointer, //
            [this](const double red, const double green, const double blue) {
                const GenericColor rgb255 //
                    {qBound<double>(0, red * 255, 255), //
                     qBound<double>(0, green * 255, 255),
                     qBound<double>(0, blue * 255, 255)};
                setCurrentOpaqueColor(RgbColor::fromRgb255(rgb255), nullptr);
            });
}

/** @brief Initialize the numeric input widgets of this dialog.
 * @returns A pointer to a new widget that has the other, numeric input
 * widgets as child widgets. */
QWidget *ColorDialogPrivate::initializeNumericPage()
{
    // Create RGB MultiSpinBox
    {
        m_rgbSpinBox = new MultiSpinBox();
        QList<MultiSpinBoxSection> rgbSections;
        MultiSpinBoxSection mySection;
        mySection.setDecimals(decimals);
        mySection.setMinimum(0);
        mySection.setMaximum(255);
        // R
        mySection.setPrefix(QString());
        mySection.setSuffix(m_multispinboxSectionSeparator);
        rgbSections.append(mySection);
        // G
        mySection.setPrefix(m_multispinboxSectionSeparator);
        mySection.setSuffix(m_multispinboxSectionSeparator);
        rgbSections.append(mySection);
        // B
        mySection.setPrefix(m_multispinboxSectionSeparator);
        mySection.setSuffix(QString());
        rgbSections.append(mySection);
        // Not setting prefix/suffix here. This will be done in retranslateUi()…
        m_rgbSpinBox->setSectionConfigurations(rgbSections);
    }

    // Create widget for the hex style color representation
    {
        m_rgbLineEdit = new QLineEdit();
        m_rgbLineEdit->setMaxLength(7);
        QRegularExpression tempRegularExpression( //
            QStringLiteral(u"#?[0-9A-Fa-f]{0,6}"));
        QRegularExpressionValidator *validator = new QRegularExpressionValidator( //
            tempRegularExpression, //
            q_pointer);
        m_rgbLineEdit->setValidator(validator);
    }

    // Create HSL spin box
    {
        m_hslSpinBox = new MultiSpinBox();
        QList<MultiSpinBoxSection> hslSections;
        MultiSpinBoxSection mySection;
        mySection.setDecimals(decimals);
        // H
        mySection.setMinimum(0);
        mySection.setMaximum(360);
        mySection.setWrapping(true);
        hslSections.append(mySection);
        // S
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySection.setWrapping(false);
        hslSections.append(mySection);
        // L
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySection.setWrapping(false);
        hslSections.append(mySection);
        // Not setting prefix/suffix here. This will be done in retranslateUi()…
        m_hslSpinBox->setSectionConfigurations(hslSections);
    }

    // Create HWB spin box
    {
        m_hwbSpinBox = new MultiSpinBox();
        QList<MultiSpinBoxSection> hwbSections;
        MultiSpinBoxSection mySection;
        mySection.setDecimals(decimals);
        // H
        mySection.setMinimum(0);
        mySection.setMaximum(360);
        mySection.setWrapping(true);
        hwbSections.append(mySection);
        // W
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySection.setWrapping(false);
        hwbSections.append(mySection);
        // B
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySection.setWrapping(false);
        hwbSections.append(mySection);
        // Not setting prefix/suffix here. This will be done in retranslateUi()…
        m_hwbSpinBox->setSectionConfigurations(hwbSections);
    }

    // Create HSV spin box
    {
        m_hsvSpinBox = new MultiSpinBox();
        QList<MultiSpinBoxSection> hsvSections;
        MultiSpinBoxSection mySection;
        mySection.setDecimals(decimals);
        // H
        mySection.setMinimum(0);
        mySection.setMaximum(360);
        mySection.setWrapping(true);
        hsvSections.append(mySection);
        // S
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySection.setWrapping(false);
        hsvSections.append(mySection);
        // V
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySection.setWrapping(false);
        hsvSections.append(mySection);
        // Not setting prefix/suffix here. This will be done in retranslateUi()…
        m_hsvSpinBox->setSectionConfigurations(hsvSections);
    }

    // Create RGB layout
    {
        QFormLayout *tempRgbFormLayout = new QFormLayout();
        m_rgbSpinBoxLabel = new QLabel();
        m_rgbSpinBoxLabel->setBuddy(m_rgbSpinBox);
        tempRgbFormLayout->addRow(m_rgbSpinBoxLabel, m_rgbSpinBox);
        m_rgbLineEditLabel = new QLabel();
        m_rgbLineEditLabel->setBuddy(m_rgbLineEdit);
        tempRgbFormLayout->addRow(m_rgbLineEditLabel, m_rgbLineEdit);
        m_hslSpinBoxLabel = new QLabel();
        m_hslSpinBoxLabel->setBuddy(m_hslSpinBox);
        tempRgbFormLayout->addRow(m_hslSpinBoxLabel, m_hslSpinBox);
        m_hwbSpinBoxLabel = new QLabel();
        m_hwbSpinBoxLabel->setBuddy(m_hwbSpinBox);
        tempRgbFormLayout->addRow(m_hwbSpinBoxLabel, m_hwbSpinBox);
        m_hsvSpinBoxLabel = new QLabel();
        m_hsvSpinBoxLabel->setBuddy(m_hsvSpinBox);
        tempRgbFormLayout->addRow(m_hsvSpinBoxLabel, m_hsvSpinBox);
        m_rgbGroupBox = new QGroupBox();
        m_rgbGroupBox->setLayout(tempRgbFormLayout);
        // Using the profile name as QGroupBox title. But on some styles, the
        // title is always shown completely, even if the text is extremly
        // long. As the text is out of our control, and some profiles
        // like Krita’s ITUR_2100_PQ_FULL.ICC have actually extremly
        // long names, we use eliding.
        const QFontMetricsF fontMetrics(m_rgbGroupBox->font());
        const auto elidedProfileName = fontMetrics.elidedText( //
            m_rgbColorSpace->profileName(),
            Qt::TextElideMode::ElideRight,
            // width (in device-independent pixels!):
            tempRgbFormLayout->minimumSize().width());
        m_rgbGroupBox->setTitle(elidedProfileName);
    }

    // Create widget for the CIEHLC-D50 color representation
    {
        QList<MultiSpinBoxSection> ciehlcD50Sections;
        m_ciehlcD50SpinBox = new MultiSpinBox;
        MultiSpinBoxSection mySection;
        mySection.setDecimals(decimals);
        // H
        mySection.setMinimum(0);
        mySection.setMaximum(360);
        mySection.setWrapping(true);
        ciehlcD50Sections.append(mySection);
        // L
        mySection.setMinimum(0);
        mySection.setMaximum(100);
        mySection.setWrapping(false);
        ciehlcD50Sections.append(mySection);
        // C
        mySection.setMinimum(0);
        mySection.setMaximum(CielchD50Values::maximumChroma);
        mySection.setWrapping(false);
        ciehlcD50Sections.append(mySection);
        // Not setting prefix/suffix here. This will be done in retranslateUi()…
        m_ciehlcD50SpinBox->setSectionConfigurations(ciehlcD50Sections);
    }

    // Create widget for the Oklch color representation
    {
        QList<MultiSpinBoxSection> oklchSections;
        MultiSpinBoxSection mySection;
        m_oklchSpinBox = new MultiSpinBox;
        // L
        mySection.setMinimum(0);
        mySection.setMaximum(1);
        mySection.setSingleStep(singleStepOklabc);
        mySection.setWrapping(false);
        mySection.setDecimals(okdecimals);
        oklchSections.append(mySection);
        // C
        mySection.setMinimum(0);
        mySection.setMaximum(OklchValues::maximumChroma);
        mySection.setSingleStep(singleStepOklabc);
        mySection.setWrapping(false);
        mySection.setDecimals(okdecimals);
        oklchSections.append(mySection);
        // H
        mySection.setMinimum(0);
        mySection.setMaximum(360);
        mySection.setSingleStep(1);
        mySection.setWrapping(true);
        mySection.setDecimals(decimals);
        oklchSections.append(mySection);
        // Not setting the suffix here. This will be done in retranslateUi()…
        m_oklchSpinBox->setSectionConfigurations(oklchSections);
    }

    // Create a global widget
    QWidget *tempWidget = new QWidget;
    QVBoxLayout *tempMainLayout = new QVBoxLayout;
    tempWidget->setLayout(tempMainLayout);
    tempWidget->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Maximum);
    QFormLayout *cielabFormLayout = new QFormLayout;
    m_ciehlcD50SpinBoxLabel = new QLabel();
    m_ciehlcD50SpinBoxLabel->setBuddy(m_ciehlcD50SpinBox);
    cielabFormLayout->addRow(m_ciehlcD50SpinBoxLabel, m_ciehlcD50SpinBox);
    m_oklchSpinBoxLabel = new QLabel();
    m_oklchSpinBoxLabel->setBuddy(m_oklchSpinBox);
    cielabFormLayout->addRow(m_oklchSpinBoxLabel, m_oklchSpinBox);
    tempMainLayout->addLayout(cielabFormLayout);
    tempMainLayout->addWidget(m_rgbGroupBox);
    tempMainLayout->addStretch();

    // Return
    return tempWidget;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QColorDialog::ColorDialogOptions ColorDialog::options() const
{
    return d_pointer->m_options;
}

/** @brief Setter for @ref options.
 *
 * Sets a value for just one single option within @ref options.
 * @param option the option to set
 * @param on the new value of the option */
void ColorDialog::setOption(PerceptualColor::ColorDialog::ColorDialogOption option, bool on)
{
    QColorDialog::ColorDialogOptions temp = d_pointer->m_options;
    temp.setFlag(option, on);
    setOptions(temp);
}

/** @brief Setter for @ref options
 * @param newOptions the new options
 * @post <em>All</em> options of the widget have the same state
 * (enabled/disabled) as in the given parameter. */
void ColorDialog::setOptions(PerceptualColor::ColorDialog::ColorDialogOptions newOptions)
{
    if (newOptions == d_pointer->m_options) {
        return;
    }

    // Save the new options
    d_pointer->m_options = newOptions;
    // Correct QColorDialog::ColorDialogOption::DontUseNativeDialog
    // which must be always on
    d_pointer->m_options.setFlag( //
        QColorDialog::ColorDialogOption::DontUseNativeDialog,
        true);

    // Apply the new options (alpha value)
    const bool alphaVisibility = d_pointer->m_options.testFlag( //
        QColorDialog::ColorDialogOption::ShowAlphaChannel);
    d_pointer->m_alphaLabel->setVisible(alphaVisibility);
    d_pointer->m_alphaGradientSlider->setVisible(alphaVisibility);
    d_pointer->m_alphaSpinBox->setVisible(alphaVisibility);

    // Apply the new options (buttons)
    d_pointer->m_buttonBox->setVisible(!d_pointer->m_options.testFlag( //
        QColorDialog::ColorDialogOption::NoButtons));

    // Notify
    Q_EMIT optionsChanged(d_pointer->m_options);
}

/** @brief Getter for @ref options
 *
 * Gets the value of just one single option within @ref options.
 *
 * @param option the requested option
 * @returns the value of the requested option
 */
bool ColorDialog::testOption(PerceptualColor::ColorDialog::ColorDialogOption option) const
{
    return d_pointer->m_options.testFlag(option);
}

/** @brief Pops up a modal color dialog, lets the user choose a color, and
 *  returns that color.
 *
 * @param colorSpace The color space within which this widget should operate.
 * @param initial    initial value for currentColor()
 * @param parent     parent widget of the dialog (or 0 for no parent)
 * @param title      window title (or an empty string for the default window
 *                   title)
 * @param options    the options() for customizing the look and feel of the
 *                   dialog
 * @returns          selectedColor(): The color the user has selected; or an
 *                   invalid color if the user has canceled the dialog. */
QColor ColorDialog::getColor(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                             const QColor &initial,
                             QWidget *parent,
                             const QString &title,
                             QColorDialog::ColorDialogOptions options)
{
    ColorDialog temp(colorSpace, parent);
    if (!title.isEmpty()) {
        temp.setWindowTitle(title);
    }
    temp.setOptions(options);
    // setCurrentColor() must be after setOptions()
    // to allow alpha channel support
    temp.setCurrentColor(initial);
    temp.exec();
    return temp.selectedColor();
}

/** @brief Pops up a modal color dialog, lets the user choose a color, and
 *  returns that color.
 *
 * @param initial    initial value for currentColor()
 * @param parent     parent widget of the dialog (or 0 for no parent)
 * @param title      window title (or an empty string for the default window
 *                   title)
 * @param options    the options() for customizing the look and feel of the
 *                   dialog
 * @returns          selectedColor(): The color the user has selected; or an
 *                   invalid color if the user has canceled the dialog. */
QColor ColorDialog::getColor(const QColor &initial, QWidget *parent, const QString &title, QColorDialog::ColorDialogOptions options)
{
    return getColor(RgbColorSpaceFactory::createSrgb(), //
                    initial, //
                    parent, //
                    title, //
                    options);
}

/** @brief The color that was actually selected by the user.
 *
 * At difference to the @ref currentColor property, this function provides
 * the color that was actually selected by the user by clicking the OK button
 * or pressing the return key or another equivalent action.
 *
 * This function most useful to get the actually selected color <em>after</em>
 * that the dialog has been closed.
 *
 * When a dialog that had been closed or hidden is shown again,
 * this function returns to an invalid QColor().
 *
 * @returns Just after showing the dialog, the value is an invalid QColor. If
 * the user selects a color by clicking the OK button or another equivalent
 * action, the value is the selected color. If the user cancels the dialog
 * (Cancel button, or by pressing the Escape key), the value remains an
 * invalid QColor. */
QColor ColorDialog::selectedColor() const
{
    return d_pointer->m_selectedColor;
}

/** @brief Setter for property <em>visible</em>
 *
 * Reimplemented from base class.
 *
 * When a dialog, that wasn't formerly visible, gets visible,
 * it’s @ref selectedColor value is cleared.
 *
 * @param visible holds whether or not the dialog should be visible */
void ColorDialog::setVisible(bool visible)
{
    if (visible && (!isVisible())) {
        // Only delete the selected color if the dialog wasn’t visible before
        // and will be made visible now.
        d_pointer->m_selectedColor = QColor();
        d_pointer->applyLayoutDimensions();
    }
    QDialog::setVisible(visible);
    // HACK If there is a QColorDialog as helper widget for the
    // screen color picker feature, QDialog::setVisible() sometimes
    // changes which is default button; however, this has only been
    // observed running the unit tests on KDE’s CI system running, but
    // not when running the unit tests locally. Force correct default button:
    d_pointer->m_buttonOK->setDefault(true);
}

/** @brief Various updates when closing the dialog.
 *
 * Reimplemented from base class.
 * @param result The result with which the dialog has been closed */
void ColorDialog::done(int result)
{
    if (result == QDialog::DialogCode::Accepted) {
        d_pointer->m_selectedColor = currentColor();
        Q_EMIT colorSelected(d_pointer->m_selectedColor);
    } else {
        d_pointer->m_selectedColor = QColor();
    }
    QDialog::done(result);
    if (d_pointer->m_receiverToBeDisconnected) {
        // This “disconnect” uses the old-style syntax, which does not
        // detect errors on compile time. However, we do not see a
        // possibility how to substitute it with the better new-style
        // syntax, given that d_pointer->m_memberToBeDisconnected
        // can contain different classes, which would be difficult
        // it typing the class name directly in the new syntax.
        disconnect(this, // sender
                   SIGNAL(colorSelected(QColor)), // signal
                   d_pointer->m_receiverToBeDisconnected, // receiver
                   d_pointer->m_memberToBeDisconnected.constData() // slot
        );
        d_pointer->m_receiverToBeDisconnected = nullptr;
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
ColorDialog::DialogLayoutDimensions ColorDialog::layoutDimensions() const
{
    return d_pointer->m_layoutDimensions;
}

/** @brief Setter for property @ref layoutDimensions
 * @param newLayoutDimensions the new layout dimensions */
void ColorDialog::setLayoutDimensions(const ColorDialog::DialogLayoutDimensions newLayoutDimensions)
{
    if (newLayoutDimensions == d_pointer->m_layoutDimensions) {
        return;
    }
    d_pointer->m_layoutDimensions = newLayoutDimensions;
    d_pointer->applyLayoutDimensions();
    Q_EMIT layoutDimensionsChanged(d_pointer->m_layoutDimensions);
}

/** @brief Arranges the layout conforming to @ref ColorDialog::layoutDimensions
 *
 * If @ref ColorDialog::layoutDimensions is DialogLayoutDimensions::automatic
 * than it is first evaluated again if for the current display the collapsed
 * or the expanded layout is used. */
void ColorDialogPrivate::applyLayoutDimensions()
{
    constexpr auto collapsed = ColorDialog::DialogLayoutDimensions::Collapsed;
    constexpr auto expanded = ColorDialog::DialogLayoutDimensions::Expanded;
    constexpr auto screenSizeDependent = //
        ColorDialog::DialogLayoutDimensions::ScreenSizeDependent;
    int effectivelyAvailableScreenWidth;
    int widthThreeshold;
    switch (m_layoutDimensions) {
    case collapsed:
        m_layoutDimensionsEffective = collapsed;
        break;
    case expanded:
        m_layoutDimensionsEffective = expanded;
        break;
    case screenSizeDependent:
        // Note: The following code works correctly on scaled
        // devices (high-DPI…).

        // We should not use more than 70% of the screen for a dialog.
        // That’s roughly the same as the default maximum sizes for
        // a QDialog.
        effectivelyAvailableScreenWidth = qRound( //
            QGuiApplication::primaryScreen()->availableSize().width() * 0.7);

        // Now we calculate the space we need for displaying the
        // graphical selectors and the numerical selector at their
        // preferred size in an expanded layout.
        // Start with the size of the graphical selectors.
        widthThreeshold = qMax( //
            m_wheelColorPicker->sizeHint().width(), //
            m_lightnessFirstWrapperWidget->sizeHint().width());
        // Add the size of the numerical selector.
        widthThreeshold += m_numericalWidget->sizeHint().width();
        // Add some space for margins.
        widthThreeshold = qRound(widthThreeshold * 1.2);

        // Now decide between collapsed layout and expanded layout
        if (effectivelyAvailableScreenWidth < widthThreeshold) {
            m_layoutDimensionsEffective = collapsed;
        } else {
            m_layoutDimensionsEffective = expanded;
        }
        break;
    default:
        // We should never reach this point, because we treat all possible
        // enum values in the switch statement.
        throw 0;
    }

    if (m_layoutDimensionsEffective == collapsed) {
        if (m_selectorLayout->indexOf(m_numericalWidget) >= 0) {
            // Indeed we have expanded layout and have to switch to
            // collapsed layout…
            const bool oldUpdatesEnabled = m_tabWidget->updatesEnabled();
            m_tabWidget->setUpdatesEnabled(false);
            // According to the documentation of QTabWidget::addTab it is
            // recommended to disable visual updates during adding new
            // tabs. This should avoid flickering.
            m_tabWidget->addTab(m_numericalWidget, QString());
            m_tabWidget->setUpdatesEnabled(oldUpdatesEnabled);
            retranslateUi(); // Will put a label for the recently inserted tab.
            reloadIcons(); // Will put an icon for the recently inserted tab.
            // We don’t call m_numericalWidget->show(); because this
            // is controlled by the QTabWidget.
            // Adopt size of dialog to new layout’s size hint:
            q_pointer->adjustSize();
        }
    } else {
        if (m_selectorLayout->indexOf(m_numericalWidget) < 0) {
            // Indeed we have collapsed layout and have to switch to
            // expanded layout…
            m_selectorLayout->addWidget(m_numericalWidget);
            // We call show because the widget is hidden by removing it
            // from its old parent, and needs to be shown explicitly.
            m_numericalWidget->show();
            // Adopt size of dialog to new layout’s size hint:
            q_pointer->adjustSize();
        }
    }
}

/** @brief Handle state changes.
 *
 * Implements reaction on <tt>QEvent::LanguageChange</tt>.
 *
 * Reimplemented from base class.
 *
 * @param event The event. */
void ColorDialog::changeEvent(QEvent *event)
{
    const auto type = event->type();

    if (type == QEvent::LanguageChange) {
        // From QCoreApplication documentation:
        //     “Installing or removing a QTranslator, or changing an installed
        //      QTranslator generates a LanguageChange event for the
        //      QCoreApplication instance. A QApplication instance will
        //      propagate the event to all toplevel widgets […].
        // Retranslate this widget itself:
        d_pointer->retranslateUi();
        // Retranslate all child widgets that actually need to be retranslated:
        {
            QEvent eventForSwatchBook(QEvent::LanguageChange);
            QApplication::sendEvent(d_pointer->m_swatchBook, //
                                    &eventForSwatchBook);
        }
        {
            QEvent eventForButtonOk(QEvent::LanguageChange);
            QApplication::sendEvent(d_pointer->m_buttonOK, //
                                    &eventForButtonOk);
        }
        {
            QEvent eventForButtonCancel(QEvent::LanguageChange);
            QApplication::sendEvent(d_pointer->m_buttonOK, //
                                    &eventForButtonCancel);
        }
    }

    if ((type == QEvent::PaletteChange) || (type == QEvent::StyleChange)) {
        d_pointer->reloadIcons();
    }

    QDialog::changeEvent(event);
}

/** @brief Handle show events.
 *
 * Reimplemented from base class.
 *
 * @param event The event.
 *
 * @internal
 *
 * On the first show event, make @ref ColorDialogPrivate::m_tabWidget use
 * the current tab corresponding to @ref ColorDialogPrivate::m_settings. */
void ColorDialog::showEvent(QShowEvent *event)
{
    if (!d_pointer->everShown) {
        constexpr auto expValue = ColorDialog::DialogLayoutDimensions::Expanded;
        const bool exp = d_pointer->m_layoutDimensionsEffective == expValue;
        const auto tabString = exp //
            ? d_pointer->m_settings.tabExpanded.value() //
            : d_pointer->m_settings.tab.value();
        const auto key = d_pointer->m_tabTable.key(tabString, nullptr);
        if (key != nullptr) {
            d_pointer->m_tabWidget->setCurrentWidget(*key);
        }
        // Save the new tab explicitly. If setCurrentWidget() is not
        // different from the default value, it does not trigger the
        // QTabWidget::currentChanged() signal, resulting in the tab
        // not being saved. However, we want to ensure that the tab
        // is saved whenever the user has first seen it.
        d_pointer->saveCurrentTab();
        d_pointer->everShown = true;
    }
    QDialog::showEvent(event);
}

/** @brief Saves the current tab of @ref m_tabWidget to @ref m_settings. */
void ColorDialogPrivate::saveCurrentTab()
{
    const auto currentIndex = m_tabWidget->currentIndex();
    QWidget const *const widget = m_tabWidget->widget(currentIndex);
    const auto keyList = m_tabTable.keys();
    auto it = std::find_if( //
        keyList.begin(),
        keyList.end(),
        [widget](const auto &key) {
            return ((*key) == widget);
        } //
    );
    if (it != keyList.end()) {
        const auto tabString = m_tabTable.value(*it);
        constexpr auto expValue = ColorDialog::DialogLayoutDimensions::Expanded;
        if (m_layoutDimensionsEffective == expValue) {
            m_settings.tabExpanded.setValue(tabString);
        } else {
            m_settings.tab.setValue(tabString);
        }
    }
}

} // namespace PerceptualColor
