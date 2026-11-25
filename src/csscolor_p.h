// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CUSTOMCOLORPATCH_P_H
#define PERCEPTUALCOLOR_CUSTOMCOLORPATCH_P_H

#include <optional>
#include <qcontainerfwd.h>
#include <qrgb.h>
#include <qstring.h>

#include "genericcolor.h"

namespace PerceptualColor
{

/*!
 * \inmodule ModulePrivateApi
 *
 *  \brief Private implementation within the \e {Pointer to
 *  implementation} idiom. */
class CssColorPrivate
{
public:
    /*!
     * \brief Represents the
     * \l {https://www.w3.org/TR/css-color-4/#typedef-predefined-rgb}
     * {predefined RGB color spaces} of CSS Color 4.
     *
     * \value Invalid
     * Represents a non-existing color space.
     *
     * \value Srgb
     * \l {https://www.w3.org/TR/css-color-4/#valdef-color-srgb} {srgb}
     *
     * \value SrgbLinear
     * \l {https://www.w3.org/TR/css-color-4/#valdef-color-srgb-linear}
     * {srgb-linear}
     *
     * \value DisplayP3
     * \l {https://www.w3.org/TR/css-color-4/#valdef-color-display-p3}
     * {display-p3}
     *
     * \value A98Rgb
     * \l {https://www.w3.org/TR/css-color-4/#valdef-color-a98-rgb}
     * {a98-rgb}
     *
     * \value ProphotoRgb
     * \l {https://www.w3.org/TR/css-color-4/#valdef-color-prophoto-rgb}
     * {prophoto-rgb}
     *
     * \value Rec2020
     * \l {https://www.w3.org/TR/css-color-4/#valdef-color-rec2020}
     * {rec2020}
     */
    enum class CssPredefinedRgbColorSpace {
        Invalid,
        Srgb,
        SrgbLinear,
        DisplayP3,
        A98Rgb,
        ProphotoRgb,
        Rec2020
    };

    /*!
     * \brief Syntaxes of the CSS Color 4 color functions.
     *
     * \value LegacySyntax
     * Only
     * \l {https://www.w3.org/TR/css-color-4/#color-syntax-legacy}
     * {Legacy (comma-separated) syntax}.
     *
     * \value StandardSyntax
     * Only
     * \l {https://www.w3.org/TR/css-color-4/#color-syntax} {Standard
     * (whitespace-separated) syntax}, optionally with a slash and an
     * alpha argument at the end.
     *
     * \value BothSyntaxes
     * Both, \l FunctionSyntax::LegacySyntax and
     * \l FunctionSyntax::StandardSyntax.
     */
    enum class FunctionSyntax {
        LegacySyntax,
        StandardSyntax,
        BothSyntaxes
    };

    /*!
     * \brief Identifiers for color models.
     *
     * \value CielabD50
     * The absolute Cielab color space using a D50
     * illuminant.
     * Lightness: [0, 100]. a: unbound. b: unbound.
     *
     * \value CielchD50
     * The absolute Cielch color space using a D50
     * illuminant.
     * Lightness: [0, 100]. Chroma: unbound. Hue: [0, 360[.
     *
     * \value Hsl_360_1_1
     * Any color space using the HSL color model, which
     * is a transformation of the RGB color model. Color values only have a
     * well-defined absolute meaning when associated with a corresponding
     * RGB color profile.
     * Hue: [0, 360[. Saturation: [0, 1]. Lightness: [0, 1].
     *
     * \value Hwb_360_1_1
     * Any color space using the HSL color model, which
     * is a transformation of the HWB color model. Color values only have a
     * well-defined absolute meaning when associated with a corresponding
     * RGB color profile.
     * Hue: [0, 360[. Whiteness: [0, 1]. Blackness: [0, 1].
     *
     * \value Invalid
     * Represents invalid data.
     *
     * \value OklabD65
     * The absolute Oklab color space, which by
     * definition always and exclusively uses a D65 illuminant.
     * Lightness: [0, 1]. a: unbound. b: unbound.
     *
     * \value OklchD65
     * The absolute Oklch color space, which by
     * definition always and exclusively uses a D65 illuminant.
     * Lightness: [0, 1]. Chroma: unbound. Hue: [0, 360[.
     *
     * \value Rgb_1
     * Any color space using the RGB color model. Color
     * values only have a well-defined absolute meaning when associated with a
     * corresponding RGB color profile.
     * R: [0, 1]. G: [0, 1]. B: [0, 1].
     *
     * \value XyzD50_1
     * The absolute XYZ color space assuming chromatic
     * adaption for the D50 illuminant.
     * X: unbound. Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0. Z: unbound.
     *
     * \value XyzD65_1
     * The absolute XYZ color space assuming chromatic
     * adaption for the D65 illuminant.
     * X: unbound. Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0.  Z: unbound.
     */
    enum class CssColorModel {
        CielabD50,
        CielchD50,
        Hsl_360_1_1,
        Hwb_360_1_1,
        Invalid,
        OklabD65,
        OklchD65,
        Rgb_1,
        XyzD50_1,
        XyzD65_1
    };

    /*!
     *
     *
     * \inmodule ModulePrivateApi
     *
     * \brief Represents a CSS color in a structured fashion.
     */
    struct CssColorValue {
    public:
        /*!
         * \brief The color model.
         *
         * This is \l CssColorModel::Invalid if the whole value is invalid. */
        CssColorModel model = CssColorModel::Invalid;
        /*!
         * \brief Which RGB profile is used.
         *
         * If \l model is an RGB-based model, it contains
         * \e {which} RGB profile is used. Otherwise, it’s
         * \l CssPredefinedRgbColorSpace::Invalid. */
        CssPredefinedRgbColorSpace rgbColorSpace = CssPredefinedRgbColorSpace::Invalid;
        /*!
         *\brief The numeric color description. */
        GenericColor color = GenericColor();
        /*!
         * \brief Opacity (alpha channel).
         *
         * Range: [0, 1] */
        double alpha1 = 0;
    };

    [[nodiscard]] static CssColorPrivate::CssColorValue parseToStructuredValue(const QString &string);
    [[nodiscard]] static CssColorPrivate::CssColorValue parseAbsoluteColorFunction(const QString &colorFunction);
    [[nodiscard]] static std::optional<QStringList>
    parseAllFunctionArguments(const QString &arguments, const CssColorPrivate::FunctionSyntax mode, const int count);
    [[nodiscard]] static std::optional<QRgb> parseHexColor(const QString &hexColor);
    [[nodiscard]] static std::optional<QRgb> parseNamedColor(const QString &namedColor);
    [[nodiscard]] static std::optional<double> parseArgumentHueNoneTo360(const QString &argument);
    [[nodiscard]] static std::optional<double> parseArgumentPercentNoneTo1(const QString &argument);
    [[nodiscard]] static std::optional<double> parseArgumentPercentNumberNone(const QString &argument, const double full, const double none);
    [[nodiscard]] static std::optional<QStringList> validateArguments(const QStringList &arguments);
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CUSTOMCOLORPATCH_P_H
