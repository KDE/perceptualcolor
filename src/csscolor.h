// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CSSCOLOR_H
#define CSSCOLOR_H

#include "genericcolor.h"
#include "helperconversion.h"
#include <optional>
#include <qglobal.h>
#include <qhash.h>
#include <qrgb.h>
#include <qstring.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qtmetamacros.h>
#else
#include <qstringlist.h>
#endif

namespace PerceptualColor
{

/** @internal
 *
 * @brief Toolbox for CSS colors. */
class CssColor
{
public:
    /** @brief Represents the
     * <a href="https://www.w3.org/TR/css-color-4/#typedef-predefined-rgb">
     * predefined RGB color spaces</a> of CSS Color 4. */
    enum class CssPredefinedRgbColorSpace {
        Invalid, /**< Represents a non-existing color space. */
        Srgb, /**<
            <a href="https://www.w3.org/TR/css-color-4/#valdef-color-srgb">
            srgb</a> */
        SrgbLinear, /**<
            <a href="https://www.w3.org/TR/css-color-4/#valdef-color-srgb-linear">
            srgb-linear</a> */
        DisplayP3, /**<
            <a href="https://www.w3.org/TR/css-color-4/#valdef-color-display-p3">
            display-p3</a> */
        A98Rgb, /**<
           <a href="https://www.w3.org/TR/css-color-4/#valdef-color-a98-rgb">
           a98-rgb</a> */
        ProphotoRgb, /**<
           <a href="https://www.w3.org/TR/css-color-4/#valdef-color-prophoto-rgb">
           prophoto-rgb</a> */
        Rec2020 /**<
            <a href="https://www.w3.org/TR/css-color-4/#valdef-color-rec2020">
            rec2020</a> */
    };
    Q_ENUM(CssPredefinedRgbColorSpace)

    /** @internal
     *
     * @brief Represents a CSS color in a structured fashion. */
    struct CssColorValue {
    public:
        /** @brief The color model.
         *
         * This is @ref ColorModel::Invalid if the whole value is invalid. */
        ColorModel model = ColorModel::Invalid;
        /** @brief Which RGB profile is used.
         *
         * If @ref model is an RGB-based model, it contains
         * <em>which</em> RGB profile is used. Otherwise, it’s
         * @ref CssPredefinedRgbColorSpace::Invalid. */
        CssPredefinedRgbColorSpace rgbColorSpace = CssPredefinedRgbColorSpace::Invalid;
        /** @brief The numeric color description. */
        GenericColor color = GenericColor();
        /** @brief Opacity (alpha channel).
         *
         * Range: [0, 1] */
        double alpha1 = 0;
    };

    static CssColorValue parse(const QString &string);
    static QStringList generateCss(const QHash<ColorModel, GenericColor> &input, const double opacity1, const int significantFigures);

private:
    /** @brief Syntaxes of the CSS Color 4 color functions. */
    enum class FunctionSyntax {
        LegacySyntax, /**< Only
            <a href="https://www.w3.org/TR/css-color-4/#color-syntax-legacy">
            Legacy (comma-separated) syntax</a>. */
        StandardSyntax, /**< Only
            <a href="https://www.w3.org/TR/css-color-4/#color-syntax">Standard
            (whitespace-separated) syntax</a>, optionally with a slash and an
            alpha argument at the end. */
        BothSyntaxes /**< Both, @ref FunctionSyntax::LegacySyntax and
            @ref FunctionSyntax::StandardSyntax. */
    };

    static CssColorValue parseAbsoluteColorFunction(const QString &colorFunction);
    static std::optional<QStringList> parseAllFunctionArguments(const QString &arguments, const FunctionSyntax mode, const int count);
    static std::optional<QRgb> parseHexColor(const QString &hexColor);
    static std::optional<QRgb> parseNamedColor(const QString &namedColor);
    static std::optional<double> parseArgumentHueNoneTo360(const QString &argument);
    static std::optional<double> parseArgumentPercentNoneTo1(const QString &argument);
    static std::optional<double> parseArgumentPercentNumberNone(const QString &argument, const double full, const double none);
    static std::optional<QStringList> validateArguments(const QStringList &arguments);

    /** @internal @brief Only for unit tests. */
    friend class TestCssColor;
};

} // namespace PerceptualColor

#endif // CSSCOLOR_H
