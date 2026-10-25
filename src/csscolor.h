// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CSSCOLOR_H
#define PERCEPTUALCOLOR_CSSCOLOR_H

#include "genericcolor.h"
#include "internalimportexport.h"
#include "perceptualcolornamespace.h"
#include <optional>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qrgb.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Toolbox for CSS colors. */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT CssColor
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
     * \value ProphotoRgb,
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
    Q_ENUM(CssPredefinedRgbColorSpace)

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
         * This is \l ColorModel::Invalid if the whole value is invalid. */
        ColorModel model = ColorModel::Invalid;
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

    [[nodiscard]] static CssColorValue parse(const QString &string);
    [[nodiscard]] static QStringList generateCss(const QHash<ColorModel, GenericColor> &input, const double opacity1, const int significantFigures);

private:
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

    [[nodiscard]] static CssColorValue parseAbsoluteColorFunction(const QString &colorFunction);
    [[nodiscard]] static std::optional<QStringList> parseAllFunctionArguments(const QString &arguments, const FunctionSyntax mode, const int count);
    [[nodiscard]] static std::optional<QRgb> parseHexColor(const QString &hexColor);
    [[nodiscard]] static std::optional<QRgb> parseNamedColor(const QString &namedColor);
    [[nodiscard]] static std::optional<double> parseArgumentHueNoneTo360(const QString &argument);
    [[nodiscard]] static std::optional<double> parseArgumentPercentNoneTo1(const QString &argument);
    [[nodiscard]] static std::optional<double> parseArgumentPercentNumberNone(const QString &argument, const double full, const double none);
    [[nodiscard]] static std::optional<QStringList> validateArguments(const QStringList &arguments);

    friend class TestCssColor;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CSSCOLOR_H
