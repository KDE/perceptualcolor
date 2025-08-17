// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "csscolor.h"

#include "helpermath.h"
#include "helperposixmath.h"
#include <array>
#include <optional>
#include <qhash.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qregularexpression.h>
#include <qrgb.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qstringview.h>
#include <type_traits>
#include <utility>

namespace PerceptualColor
{

/** @brief Parses a hexadecimal color notations.
 *
 * Implements the
 * <a href="https://www.w3.org/TR/css-color-4/#typedef-hex-color">hexadecimal
 * notations as defined in CSS Color 4</a>.
 *
 * @param hexColor The hexadecimal color to parse, without any leading or
 * trailing whitespace.
 *
 * @returns The sRGB value if the syntax is valid. An
 * empty value otherwise. */
std::optional<QRgb> CssColor::parseHexColor(const QString &hexColor)
{
    if (hexColor.length() > 9) { // Maximum 8 digits + “#” allowed
        return std::nullopt;
    }
    static const QRegularExpression regex(QStringLiteral("^#([0-9A-Fa-f]*)$"));
    QString capturedDigits = regex.match(hexColor).captured(1);
    switch (capturedDigits.length()) {
    // Expand short forms
    case 3:
        capturedDigits = capturedDigits.at(0) + capturedDigits.at(0) //
            + capturedDigits.at(1) + capturedDigits.at(1) //
            + capturedDigits.at(2) + capturedDigits.at(2);
        break;
    case 4:
        capturedDigits = capturedDigits.at(0) + capturedDigits.at(0) //
            + capturedDigits.at(1) + capturedDigits.at(1) //
            + capturedDigits.at(2) + capturedDigits.at(2) //
            + capturedDigits.at(3) + capturedDigits.at(3);
        break;
    default:
        break;
    }
    if (capturedDigits.length() == 6) {
        // Add missing opacity.
        capturedDigits += QStringLiteral("ff");
    }
    if (capturedDigits.length() == 8) {
        const auto view = QStringView{capturedDigits};
        const auto red = view.mid(0, 2).toInt(nullptr, 16);
        const auto green = view.mid(2, 2).toInt(nullptr, 16);
        const auto blue = view.mid(4, 2).toInt(nullptr, 16);
        const auto alpha = view.mid(6, 2).toInt(nullptr, 16);
        return qRgba(red, green, blue, alpha);
    }
    return std::nullopt;
}

/** @brief Validate arguments.
 *
 * @param arguments The list of arguments to be validated.
 *
 * @returns For each argument, it is checked if it is valid, meaning it does
 * not contain any whitespace in the middle, comma, or slash. If all arguments
 * are valid, they are returned with leading and trailing whitespace removed.
 * Otherwise, an empty value is returned. */
std::optional<QStringList> CssColor::validateArguments(const QStringList &arguments)
{
    QStringList result;
    for (const QString &argument : std::as_const(arguments)) {
        QString simplifiedString = argument.simplified();
        if (simplifiedString.contains(QStringLiteral(" ")) //
            || simplifiedString.contains(QStringLiteral(",")) //
            || simplifiedString.contains(QStringLiteral("/")) //
            || simplifiedString.isEmpty()) {
            return std::nullopt;
        }
        result.append(simplifiedString);
    }
    return result;
}

/** @brief Parses arguments of a CSS Color 4 function.
 *
 * Accepts both, standard (white-space separated) and legacy (comma-separated)
 * syntax. It accepts an arbitrary number of normal arguments, and in standard
 * syntax also up to one alpha argument.
 *
 * @param arguments The function arguments to parse.
 * @param mode The syntaxes that are considered as valid.
 * @param count The exact number of expected arguments. Finally accepted
 * are arguments of this exact number, of of this exact number minus one.
 * (It is supposed that the last argument is the alpha arguments, which is
 * optional.) A missing argument is added automatically with the
 * value <tt>"none"</tt>.
 *
 * @returns A string list containing all arguments, or an empty value if the
 * syntax was invalid. Note that the individual arguments have leading and/or
 * trailing white space removed and are guaranteed to not contain any comma
 * or slash. */
std::optional<QStringList> CssColor::parseAllFunctionArguments(const QString &arguments, const FunctionSyntax mode, const int count)
{
    if (arguments.count(QStringLiteral(",")) > 0) {
        // Legacy syntax detected
        if (mode == FunctionSyntax::BothSyntaxes || mode == FunctionSyntax::LegacySyntax) {
            // Legacy syntax allowed, so interpret as legacy syntax.
            if (arguments.count(QStringLiteral("/")) > 0) {
                // No slash separator allowed in legacy function arguments.
                return std::nullopt;
            }
            auto result = arguments.split(QStringLiteral(","), //
                                          Qt::KeepEmptyParts);
            if (result.size() == count - 1) {
                // Add implicit alpha argument
                result.append(QStringLiteral("none"));
            }
            if (result.size() != count) {
                return std::nullopt;
            }
            return validateArguments(result);
        } else {
            return std::nullopt;
        }
    }

    // If it’s not legacy syntax, is must be standard syntax, so interpret as
    // standard syntax.
    if (mode == FunctionSyntax::LegacySyntax) {
        // Standard syntax isn’t allowed here, so return.
        return std::nullopt;
    }
    const auto parts = arguments.split(QStringLiteral("/"), //
                                       Qt::KeepEmptyParts);
    if (parts.size() > 2) {
        // Not more than one slash allowed.
        return std::nullopt;
    }
    const QString alphaArgument = (parts.size() == 2) //
        ? parts.value(1) //
        : QStringLiteral("none");
    auto result = parts.value(0).simplified().split(QStringLiteral(" "));
    result.append(alphaArgument);
    if (result.size() != count) {
        // Wrong number of arguments
        return std::nullopt;
    }
    return validateArguments(result);
}

/** @brief Parses a single argument.
 *
 * Accepts absolute numbers, percent values and <tt>"none"</tt>.
 *
 * Leading and trailing whitespace is ignored.
 *
 * @param argument The argument to parse.
 * @param full The absolute value that corresponds to 100%.
 * @param none The absolute value that correspond to <tt>"none"</tt>.
 *
 * @returns The absolute number if the syntax is valid. An empty value
 * otherwise. */
std::optional<double> CssColor::parseArgumentPercentNumberNone(const QString &argument, const double full, const double none)
{
    bool okay = true;
    QString cleanArgument = argument.simplified();
    if (cleanArgument.contains(QStringLiteral(" ")) //
        || cleanArgument.contains(QStringLiteral(",")) //
        || cleanArgument.contains(QStringLiteral("/")) //
        || cleanArgument.isEmpty()) {
        return std::nullopt;
    }
    std::optional<double> result;
    if (cleanArgument == QStringLiteral("none")) {
        return none;
    } else if (cleanArgument.endsWith(QStringLiteral("%"))) {
        cleanArgument.truncate(cleanArgument.length() - 1);
        result = cleanArgument.toDouble(&okay) / 100. * full;
    } else {
        result = cleanArgument.toDouble(&okay);
    }
    if (!okay) {
        return std::nullopt;
    }
    return result;
}

/** @brief Parses a single argument.
 *
 * Accepts percent values and <tt>"none"</tt>.
 *
 * Leading and trailing whitespace is ignored.
 *
 * @param argument The argument to parse.
 *
 * @returns For invalid syntax, an empty value is returned. For valid syntax,
 * <tt>100%</tt> corresponds to <tt>1</tt>, while <tt>0%</tt> and <tt>none</tt>
 * correspond to <tt>0</tt>. */
std::optional<double> CssColor::parseArgumentPercentNoneTo1(const QString &argument)
{
    bool okay = true;
    QString cleanArgument = argument.simplified();
    if (cleanArgument.contains(QStringLiteral(" ")) //
        || cleanArgument.contains(QStringLiteral(",")) //
        || cleanArgument.contains(QStringLiteral("/")) //
        || cleanArgument.isEmpty()) {
        return std::nullopt;
    }
    if (cleanArgument == QStringLiteral("none")) {
        return 0;
    }
    if (!cleanArgument.endsWith(QStringLiteral("%"))) {
        return std::nullopt;
    }
    cleanArgument.truncate(cleanArgument.length() - 1);
    const auto result = cleanArgument.toDouble(&okay) / 100.;
    if (okay) {
        return result;
    }
    return std::nullopt;
}

/** @brief Parses a single argument.
 *
 * Accepts percent values and <tt>"none"</tt>.
 *
 * Leading and trailing whitespace is ignored.
 *
 * @param argument The argument to parse.
 *
 * @returns For invalid syntax, an empty value is returned. For valid syntax,
 * a hue in the range [0, 360[ is returned, with 360 corresponding to the
 * full circle. */
std::optional<double> CssColor::parseArgumentHueNoneTo360(const QString &argument)
{
    bool okay = true;
    QString cleanArgument = argument.simplified();
    if (cleanArgument.contains(QStringLiteral(" ")) //
        || cleanArgument.contains(QStringLiteral(",")) //
        || cleanArgument.contains(QStringLiteral("/")) //
        || cleanArgument.isEmpty()) {
        return std::nullopt;
    }
    if (cleanArgument == QStringLiteral("none")) {
        return 0;
    }
    double correctionFactor = 1;

    if (cleanArgument.endsWith(QStringLiteral("deg"))) {
        cleanArgument.truncate(cleanArgument.length() - 3);
    }
    if (cleanArgument.endsWith(QStringLiteral("grad"))) {
        cleanArgument.truncate(cleanArgument.length() - 4);
        correctionFactor = 360. / 400.;
    }
    if (cleanArgument.endsWith(QStringLiteral("rad"))) {
        cleanArgument.truncate(cleanArgument.length() - 3);
        correctionFactor = 360. / (2 * pi);
    }
    if (cleanArgument.endsWith(QStringLiteral("turn"))) {
        cleanArgument.truncate(cleanArgument.length() - 4);
        correctionFactor = 360.;
    }

    const auto result = cleanArgument.toDouble(&okay);
    if (okay) {
        return normalizedAngle360(result * correctionFactor);
    }
    return std::nullopt;
}

/** @brief Parse
 * <a href="https://www.w3.org/TR/css-color-4/#typedef-absolute-color-function">
 * Absolute Color Functions</a> as defined in CSS Color 4.
 *
 * @param colorFunction The string to parse.
 *
 * @returns If the CSS fragment is valid, the corresponding color.
 * @ref ColorModel::Invalid otherwise. */
CssColor::CssColorValue CssColor::parseAbsoluteColorFunction(const QString &colorFunction)
{
    static const QRegularExpression regex( //
        QStringLiteral("^(\\w+)\\s*\\((.*)\\)$"));
    QRegularExpressionMatch match = regex.match(colorFunction);
    QString ident = match.captured(1).simplified();
    const QString argumentsString = match.captured(2).simplified();
    std::optional<QStringList> maybeArguments;

    if (ident == QStringLiteral("rgb") //
        || ident == QStringLiteral("hsl")) {
        maybeArguments = parseAllFunctionArguments( //
            argumentsString, //
            FunctionSyntax::BothSyntaxes, //
            4);
    }
    if (ident == QStringLiteral("rgba") //
        || ident == QStringLiteral("hsla")) {
        maybeArguments = parseAllFunctionArguments( //
            argumentsString, //
            FunctionSyntax::LegacySyntax, //
            4);
    }
    if (ident == QStringLiteral("hwb") //
        || ident == QStringLiteral("lch") //
        || ident == QStringLiteral("lab") //
        || ident == QStringLiteral("oklch") //
        || ident == QStringLiteral("oklab")) {
        maybeArguments = parseAllFunctionArguments( //
            argumentsString, //
            FunctionSyntax::StandardSyntax, //
            4);
    }
    if (ident == QStringLiteral("color")) {
        const auto colorArguments = parseAllFunctionArguments( //
            argumentsString, //
            FunctionSyntax::StandardSyntax, //
            5);
        if (!colorArguments.has_value()) {
            return CssColorValue();
        }
        ident = colorArguments.value().value(0);
        maybeArguments = colorArguments.value().mid(1);
    };
    if (!maybeArguments.has_value()) {
        return CssColorValue();
    }
    const auto arguments = maybeArguments.value();

    QList<double> list1;
    list1.reserve(3);
    ColorModel model = ColorModel::Invalid;
    CssPredefinedRgbColorSpace rgbColorSpace = //
        CssPredefinedRgbColorSpace::Invalid;

    using Pair = std::pair<QString, CssPredefinedRgbColorSpace>;
    // clang-format off
    static const QHash<QString, CssPredefinedRgbColorSpace> hash {
        Pair(QStringLiteral("srgb"), CssPredefinedRgbColorSpace::Srgb),
        Pair(QStringLiteral("srgb-linear"), CssPredefinedRgbColorSpace::SrgbLinear),
        Pair(QStringLiteral("display-p3"), CssPredefinedRgbColorSpace::DisplayP3),
        Pair(QStringLiteral("a98-rgb"), CssPredefinedRgbColorSpace::A98Rgb),
        Pair(QStringLiteral("prophoto-rgb"), CssPredefinedRgbColorSpace::ProphotoRgb),
        Pair(QStringLiteral("rec2020"), CssPredefinedRgbColorSpace::Rec2020)
    };
    // clang-format on
    if (ident == QStringLiteral("rgb") //
        || ident == QStringLiteral("rgba") //
        || hash.contains(ident)) {
        model = ColorModel::Rgb1;
        rgbColorSpace = hash.value( //
            ident, //
            CssPredefinedRgbColorSpace::Srgb);
        const double full = (hash.contains(ident)) //
            ? 1
            : 255;
        for (int i = 0; i < 3; ++i) {
            const auto absValue = //
                parseArgumentPercentNumberNone(arguments.value(i), full, 0);
            if (absValue.has_value()) {
                list1 += absValue.value() / full;
            }
        }
    }

    if (ident == QStringLiteral("xyz-d50") //
        || ident == QStringLiteral("xyz-d65") //
        || ident == QStringLiteral("xyz")) {
        model = (ident == QStringLiteral("xyz-d50")) //
            ? ColorModel::XyzD50
            : ColorModel::XyzD65;
        rgbColorSpace = CssPredefinedRgbColorSpace::Invalid;
        for (int i = 0; i < 3; ++i) {
            const auto absValue = //
                parseArgumentPercentNumberNone(arguments.value(i), 1, 0);
            if (absValue.has_value()) {
                list1 += absValue.value();
            }
        }
    }

    if (ident == QStringLiteral("hsl") //
        || ident == QStringLiteral("hsla") //
        || ident == QStringLiteral("hwb")) {
        model = (ident == QStringLiteral("hwb")) //
            ? ColorModel::Hwb360_1_1
            : ColorModel::Hsl360_1_1;
        rgbColorSpace = CssPredefinedRgbColorSpace::Srgb;
        const auto maybeHue = parseArgumentHueNoneTo360(arguments.value(0));
        if (maybeHue.has_value()) {
            list1 += maybeHue.value();
        }
        for (int i = 1; i < 3; ++i) {
            const auto absValue = //
                parseArgumentPercentNoneTo1(arguments.value(i));
            if (absValue.has_value()) {
                list1 += absValue.value();
            }
        }
    }

    if (ident == QStringLiteral("oklab") //
        || ident == QStringLiteral("lab")) {
        model = (ident == QStringLiteral("oklab")) //
            ? ColorModel::OklabD65
            : ColorModel::CielabD50;
        rgbColorSpace = CssPredefinedRgbColorSpace::Invalid;
        std::array<double, 3> full = (ident == QStringLiteral("oklab")) //
            ? std::array<double, 3>{{1, 0.4, 0.4}}
            : std::array<double, 3>{{100, 125, 125}};
        for (quint8 i = 0; i < 3; ++i) {
            const auto absValue = parseArgumentPercentNumberNone( //
                arguments.value(i), //
                full.at(i), //
                0);
            if (absValue.has_value()) {
                list1 += absValue.value();
            }
        }
    }

    if (ident == QStringLiteral("oklch") //
        || ident == QStringLiteral("lch")) {
        model = (ident == QStringLiteral("oklch")) //
            ? ColorModel::OklchD65
            : ColorModel::CielchD50;
        rgbColorSpace = CssPredefinedRgbColorSpace::Invalid;
        std::array<double, 2> full = (ident == QStringLiteral("oklch")) //
            ? std::array<double, 2>{{1, 0.4}}
            : std::array<double, 2>{{100, 150}};
        for (quint8 i = 0; i < 2; ++i) {
            const auto absValue = parseArgumentPercentNumberNone( //
                arguments.value(i), //
                full.at(i), //
                0);
            if (absValue.has_value()) {
                list1 += absValue.value();
            }
        }
        const auto maybeHue = parseArgumentHueNoneTo360(arguments.value(2));
        if (maybeHue.has_value()) {
            list1 += maybeHue.value();
        }
    }

    if (list1.size() != 3) {
        // One or more of the first three arguments were invalid.
        return CssColorValue();
    }

    const auto opacity1 = //
        parseArgumentPercentNumberNone(arguments.value(3), 1, 1);

    if (!opacity1.has_value()) {
        return CssColorValue();
    }
    CssColorValue result;
    result.model = model;
    result.rgbColorSpace = rgbColorSpace;
    result.color = GenericColor //
        {list1.value(0), list1.value(1), list1.value(2)};
    result.alpha1 = opacity1.value();
    return result;
}

/** @brief Parse a CSS color value.
 *
 * @param string The CSS fragment to parse
 *
 * @returns If the CSS fragment is valid, the corresponding color.
 * @ref ColorModel::Invalid otherwise.
 *
 * This parser accepts all valid <a href="https://www.w3.org/TR/css-color-4/">
 * CSS Colors 4</a>, except those who’s value is context-dependant like for
 * <tt><a href="https://www.w3.org/TR/css-color-4/#valdef-color-currentcolor">
 * currentcolor</a></tt>.
 *
 * @note A trailing “;” is ignored for your convenance. Other supplementary
 * characters will be considered as syntax error. For simplicity of
 * implementation, some very limited invalid CSS colors are considered as
 * valid when the can be no confusion about the meaning. For example,
 * <tt>rgba()</tt> does not allow to mix absolute
 * numbers and percent number: All values must be either a percentage or
 * an absolute number. However this parser accepts also mixed
 * values. */
CssColor::CssColorValue CssColor::parse(const QString &string)
{
    auto myString = string.simplified();
    if (myString.endsWith(QStringLiteral(";"))) {
        myString.chop(1);
        myString = myString.simplified();
    }

    std::optional<QRgb> srgb = parseNamedColor(myString);
    if (!srgb.has_value()) {
        srgb = parseHexColor(myString);
    }
    if (srgb.has_value()) {
        const auto srgbValue = srgb.value();
        CssColorValue result;
        result.model = ColorModel::Rgb1;
        result.rgbColorSpace = CssPredefinedRgbColorSpace::Srgb;
        result.color = GenericColor{qRed(srgbValue) / 255., //
                                    qGreen(srgbValue) / 255., //
                                    qBlue(srgbValue) / 255.};
        result.alpha1 = qAlpha(srgbValue) / 255.;
        return result;
    }

    return parseAbsoluteColorFunction(myString);
}

/** @internal
 *
 * @brief Converts a named color to sRGB (if any)
 *
 * Implements the
 * <a href="https://www.w3.org/TR/css-color-4/#typedef-named-color">
 * Named colors</a> and the
 * <a href="https://www.w3.org/TR/css-color-4/#transparent-color">
 * transparent keyword</a> as defined in CSS Color 4.
 *
 * @param namedColor The named color to search for.
 *
 * @returns The sRGB value if its a CSS named color (case-insensitive). An
 * empty value otherwise. */
std::optional<QRgb> CssColor::parseNamedColor(const QString &namedColor)
{
    using NamedColor = std::pair<QString, QRgb>;
    // clang-format off
    static const QHash<QString, QRgb> colorList {
        // From https://www.w3.org/TR/css-color-4/#transparent-color
        NamedColor(QStringLiteral("transparent"), 0x00000000),
        // From https://www.w3.org/TR/css-color-4/#named-colors
        NamedColor(QStringLiteral("aliceblue"), 0xfff0f8ff),
        NamedColor(QStringLiteral("antiquewhite"), 0xfffaebd7),
        NamedColor(QStringLiteral("aqua"), 0xff00ffff),
        NamedColor(QStringLiteral("aquamarine"), 0xff7fffd4),
        NamedColor(QStringLiteral("azure"), 0xfff0ffff),
        NamedColor(QStringLiteral("beige"), 0xfff5f5dc),
        NamedColor(QStringLiteral("bisque"), 0xffffe4c4),
        NamedColor(QStringLiteral("black"), 0xff000000),
        NamedColor(QStringLiteral("blanchedalmond"), 0xffffebcd),
        NamedColor(QStringLiteral("blue"), 0xff0000ff),
        NamedColor(QStringLiteral("blueviolet"), 0xff8a2be2),
        NamedColor(QStringLiteral("brown"), 0xffa52a2a),
        NamedColor(QStringLiteral("burlywood"), 0xffdeb887),
        NamedColor(QStringLiteral("cadetblue"), 0xff5f9ea0),
        NamedColor(QStringLiteral("chartreuse"), 0xff7fff00),
        NamedColor(QStringLiteral("chocolate"), 0xffd2691e),
        NamedColor(QStringLiteral("coral"), 0xffff7f50),
        NamedColor(QStringLiteral("cornflowerblue"), 0xff6495ed),
        NamedColor(QStringLiteral("cornsilk"), 0xfffff8dc),
        NamedColor(QStringLiteral("crimson"), 0xffdc143c),
        NamedColor(QStringLiteral("cyan"), 0xff00ffff),
        NamedColor(QStringLiteral("darkblue"), 0xff00008b),
        NamedColor(QStringLiteral("darkcyan"), 0xff008b8b),
        NamedColor(QStringLiteral("darkgoldenrod"), 0xffb8860b),
        NamedColor(QStringLiteral("darkgray"), 0xffa9a9a9),
        NamedColor(QStringLiteral("darkgreen"), 0xff006400),
        NamedColor(QStringLiteral("darkgrey"), 0xffa9a9a9),
        NamedColor(QStringLiteral("darkkhaki"), 0xffbdb76b),
        NamedColor(QStringLiteral("darkmagenta"), 0xff8b008b),
        NamedColor(QStringLiteral("darkolivegreen"), 0xff556b2f),
        NamedColor(QStringLiteral("darkorange"), 0xffff8c00),
        NamedColor(QStringLiteral("darkorchid"), 0xff9932cc),
        NamedColor(QStringLiteral("darkred"), 0xff8b0000),
        NamedColor(QStringLiteral("darksalmon"), 0xffe9967a),
        NamedColor(QStringLiteral("darkseagreen"), 0xff8fbc8f),
        NamedColor(QStringLiteral("darkslateblue"), 0xff483d8b),
        NamedColor(QStringLiteral("darkslategray"), 0xff2f4f4f),
        NamedColor(QStringLiteral("darkslategrey"), 0xff2f4f4f),
        NamedColor(QStringLiteral("darkturquoise"), 0xff00ced1),
        NamedColor(QStringLiteral("darkviolet"), 0xff9400d3),
        NamedColor(QStringLiteral("deeppink"), 0xffff1493),
        NamedColor(QStringLiteral("deepskyblue"), 0xff00bfff),
        NamedColor(QStringLiteral("dimgray"), 0xff696969),
        NamedColor(QStringLiteral("dimgrey"), 0xff696969),
        NamedColor(QStringLiteral("dodgerblue"), 0xff1e90ff),
        NamedColor(QStringLiteral("firebrick"), 0xffb22222),
        NamedColor(QStringLiteral("floralwhite"), 0xfffffaf0),
        NamedColor(QStringLiteral("forestgreen"), 0xff228b22),
        NamedColor(QStringLiteral("fuchsia"), 0xffff00ff),
        NamedColor(QStringLiteral("gainsboro"), 0xffdcdcdc),
        NamedColor(QStringLiteral("ghostwhite"), 0xfff8f8ff),
        NamedColor(QStringLiteral("gold"), 0xffffd700),
        NamedColor(QStringLiteral("goldenrod"), 0xffdaa520),
        NamedColor(QStringLiteral("gray"), 0xff808080),
        NamedColor(QStringLiteral("green"), 0xff008000),
        NamedColor(QStringLiteral("greenyellow"), 0xffadff2f),
        NamedColor(QStringLiteral("grey"), 0xff808080),
        NamedColor(QStringLiteral("honeydew"), 0xfff0fff0),
        NamedColor(QStringLiteral("hotpink"), 0xffff69b4),
        NamedColor(QStringLiteral("indianred"), 0xffcd5c5c),
        NamedColor(QStringLiteral("indigo"), 0xff4b0082),
        NamedColor(QStringLiteral("ivory"), 0xfffffff0),
        NamedColor(QStringLiteral("khaki"), 0xfff0e68c),
        NamedColor(QStringLiteral("lavender"), 0xffe6e6fa),
        NamedColor(QStringLiteral("lavenderblush"), 0xfffff0f5),
        NamedColor(QStringLiteral("lawngreen"), 0xff7cfc00),
        NamedColor(QStringLiteral("lemonchiffon"), 0xfffffacd),
        NamedColor(QStringLiteral("lightblue"), 0xffadd8e6),
        NamedColor(QStringLiteral("lightcoral"), 0xfff08080),
        NamedColor(QStringLiteral("lightcyan"), 0xffe0ffff),
        NamedColor(QStringLiteral("lightgoldenrodyellow"), 0xfffafad2),
        NamedColor(QStringLiteral("lightgray"), 0xffd3d3d3),
        NamedColor(QStringLiteral("lightgreen"), 0xff90ee90),
        NamedColor(QStringLiteral("lightgrey"), 0xffd3d3d3),
        NamedColor(QStringLiteral("lightpink"), 0xffffb6c1),
        NamedColor(QStringLiteral("lightsalmon"), 0xffffa07a),
        NamedColor(QStringLiteral("lightseagreen"), 0xff20b2aa),
        NamedColor(QStringLiteral("lightskyblue"), 0xff87cefa),
        NamedColor(QStringLiteral("lightslategray"), 0xff778899),
        NamedColor(QStringLiteral("lightslategrey"), 0xff778899),
        NamedColor(QStringLiteral("lightsteelblue"), 0xffb0c4de),
        NamedColor(QStringLiteral("lightyellow"), 0xffffffe0),
        NamedColor(QStringLiteral("lime"), 0xff00ff00),
        NamedColor(QStringLiteral("limegreen"), 0xff32cd32),
        NamedColor(QStringLiteral("linen"), 0xfffaf0e6),
        NamedColor(QStringLiteral("magenta"), 0xffff00ff),
        NamedColor(QStringLiteral("maroon"), 0xff800000),
        NamedColor(QStringLiteral("mediumaquamarine"), 0xff66cdaa),
        NamedColor(QStringLiteral("mediumblue"), 0xff0000cd),
        NamedColor(QStringLiteral("mediumorchid"), 0xffba55d3),
        NamedColor(QStringLiteral("mediumpurple"), 0xff9370db),
        NamedColor(QStringLiteral("mediumseagreen"), 0xff3cb371),
        NamedColor(QStringLiteral("mediumslateblue"), 0xff7b68ee),
        NamedColor(QStringLiteral("mediumspringgreen"), 0xff00fa9a),
        NamedColor(QStringLiteral("mediumturquoise"), 0xff48d1cc),
        NamedColor(QStringLiteral("mediumvioletred"), 0xffc71585),
        NamedColor(QStringLiteral("midnightblue"), 0xff191970),
        NamedColor(QStringLiteral("mintcream"), 0xfff5fffa),
        NamedColor(QStringLiteral("mistyrose"), 0xffffe4e1),
        NamedColor(QStringLiteral("moccasin"), 0xffffe4b5),
        NamedColor(QStringLiteral("navajowhite"), 0xffffdead),
        NamedColor(QStringLiteral("navy"), 0xff000080),
        NamedColor(QStringLiteral("oldlace"), 0xfffdf5e6),
        NamedColor(QStringLiteral("olive"), 0xff808000),
        NamedColor(QStringLiteral("olivedrab"), 0xff6b8e23),
        NamedColor(QStringLiteral("orange"), 0xffffa500),
        NamedColor(QStringLiteral("orangered"), 0xffff4500),
        NamedColor(QStringLiteral("orchid"), 0xffda70d6),
        NamedColor(QStringLiteral("palegoldenrod"), 0xffeee8aa),
        NamedColor(QStringLiteral("palegreen"), 0xff98fb98),
        NamedColor(QStringLiteral("paleturquoise"), 0xffafeeee),
        NamedColor(QStringLiteral("palevioletred"), 0xffdb7093),
        NamedColor(QStringLiteral("papayawhip"), 0xffffefd5),
        NamedColor(QStringLiteral("peachpuff"), 0xffffdab9),
        NamedColor(QStringLiteral("peru"), 0xffcd853f),
        NamedColor(QStringLiteral("pink"), 0xffffc0cb),
        NamedColor(QStringLiteral("plum"), 0xffdda0dd),
        NamedColor(QStringLiteral("powderblue"), 0xffb0e0e6),
        NamedColor(QStringLiteral("purple"), 0xff800080),
        NamedColor(QStringLiteral("rebeccapurple"), 0xff663399),
        NamedColor(QStringLiteral("red"), 0xffff0000),
        NamedColor(QStringLiteral("rosybrown"), 0xffbc8f8f),
        NamedColor(QStringLiteral("royalblue"), 0xff4169e1),
        NamedColor(QStringLiteral("saddlebrown"), 0xff8b4513),
        NamedColor(QStringLiteral("salmon"), 0xfffa8072),
        NamedColor(QStringLiteral("sandybrown"), 0xfff4a460),
        NamedColor(QStringLiteral("seagreen"), 0xff2e8b57),
        NamedColor(QStringLiteral("seashell"), 0xfffff5ee),
        NamedColor(QStringLiteral("sienna"), 0xffa0522d),
        NamedColor(QStringLiteral("silver"), 0xffc0c0c0),
        NamedColor(QStringLiteral("skyblue"), 0xff87ceeb),
        NamedColor(QStringLiteral("slateblue"), 0xff6a5acd),
        NamedColor(QStringLiteral("slategray"), 0xff708090),
        NamedColor(QStringLiteral("slategrey"), 0xff708090),
        NamedColor(QStringLiteral("snow"), 0xfffffafa),
        NamedColor(QStringLiteral("springgreen"), 0xff00ff7f),
        NamedColor(QStringLiteral("steelblue"), 0xff4682b4),
        NamedColor(QStringLiteral("tan"), 0xffd2b48c),
        NamedColor(QStringLiteral("teal"), 0xff008080),
        NamedColor(QStringLiteral("thistle"), 0xffd8bfd8),
        NamedColor(QStringLiteral("tomato"), 0xffff6347),
        NamedColor(QStringLiteral("turquoise"), 0xff40e0d0),
        NamedColor(QStringLiteral("violet"), 0xffee82ee),
        NamedColor(QStringLiteral("wheat"), 0xfff5deb3),
        NamedColor(QStringLiteral("white"), 0xffffffff),
        NamedColor(QStringLiteral("whitesmoke"), 0xfff5f5f5),
        NamedColor(QStringLiteral("yellow"), 0xffffff00),
        NamedColor(QStringLiteral("yellowgreen"), 0xff9acd32)
    };
    // clang-format on
    const QString lowerCase = namedColor.toLower();
    if (colorList.contains(lowerCase)) {
        return colorList.value(lowerCase);
    }
    return std::nullopt;
}

/** @brief Provides CSS code for existing color values.
 *
 * This function is meant for exporting colors to CSS code.
 *
 * @param input A hash table with color values.
 * @param opacity1 The opacity of the color in the range [0, 1].
 * @param significantFigures The requested number of significant figures.
 *
 * @returns A list of CSS color codes, ordered by importance: oklch, oklab,
 * lch, lab, xyz-d50, xyz-d65. oklch is considered most important, followed
 * by its less intuitive companion oklab, followed by the less perceptually
 * uniform lch and lab. Finally comes the technically important, but
 * uncomfortable xyz space, starting with its D50 variant because this
 * is more wide-spread used in color management than the D65 variant.
 * RGB-based color models are intentionally omitted, because we can never
 * be sure if a given color is available in all of these spaces, especially
 * if the library is using a wide-color gamut, but the CSS code requires
 * sRGB. And if it would sometimes work (in-gamut colors) and sometimes fail
 * (out-of-gamut colors), this might be highly confusing for the average
 * user. Note that the alpha value only appears explicitly if it’s partially
 * or fully transparent. Fully opaque colors do not need to specify the
 * alpha value in CSS explicitly, because CSS defaults to “fully opaque” if
 * no alpha value is given. */
QStringList CssColor::generateCss(const QHash<ColorModel, GenericColor> &input, const double opacity1, const int significantFigures)
{
    QStringList result;

    const auto decimals1 = decimalPlaces(2, significantFigures);
    const auto decimals2 = decimalPlaces(2, significantFigures);
    const auto decimals100 = decimalPlaces(100, significantFigures);
    const auto decimals255 = decimalPlaces(255, significantFigures);
    const auto decimals360 = decimalPlaces(360, significantFigures);

    const QString opacity1String = (opacity1 < 1) //
        ? QStringLiteral(" / %1%").arg(opacity1 * 100, 0, 'f', decimals100)
        : QString();

    if (input.contains(ColorModel::OklchD65)) {
        const auto temp = input.value(ColorModel::OklchD65);
        result.append(QStringLiteral("oklch(%1 %2 %3%4)") //
                          .arg(temp.first, 0, 'f', decimals1) //
                          .arg(temp.second, 0, 'f', decimals2) //
                          .arg(temp.third, 0, 'f', decimals360) //
                          .arg(opacity1String));
    }

    if (input.contains(ColorModel::OklabD65)) {
        const auto temp = input.value(ColorModel::OklabD65);
        result.append(QStringLiteral("oklab(%1 %2 %3%4)") //
                          .arg(temp.first, 0, 'f', decimals1) //
                          .arg(temp.second, 0, 'f', decimals2) //
                          .arg(temp.third, 0, 'f', decimals2) //
                          .arg(opacity1String));
    }

    if (input.contains(ColorModel::CielchD50)) {
        const auto temp = input.value(ColorModel::CielchD50);
        result.append(QStringLiteral("lch(%1 %2 %3%4)") //
                          .arg(temp.first, 0, 'f', decimals100) //
                          .arg(temp.second, 0, 'f', decimals255) //
                          .arg(temp.third, 0, 'f', decimals360) //
                          .arg(opacity1String));
    }

    if (input.contains(ColorModel::CielabD50)) {
        const auto temp = input.value(ColorModel::CielabD50);
        result.append(QStringLiteral("lab(%1 %2 %3%4)") //
                          .arg(temp.first, 0, 'f', decimals100) //
                          .arg(temp.second, 0, 'f', decimals255) //
                          .arg(temp.third, 0, 'f', decimals255) //
                          .arg(opacity1String));
    }

    if (input.contains(ColorModel::XyzD50)) {
        const auto temp = input.value(ColorModel::XyzD50);
        result.append(QStringLiteral("color (xyz-d50 %1 %2 %3%4)") //
                          .arg(temp.first, 0, 'f', decimals1) //
                          .arg(temp.second, 0, 'f', decimals1) //
                          .arg(temp.third, 0, 'f', decimals1) //
                          .arg(opacity1String));
    }

    if (input.contains(ColorModel::XyzD65)) {
        const auto temp = input.value(ColorModel::XyzD65);
        result.append(QStringLiteral("color (xyz-d65 %1 %2 %3%4)") //
                          .arg(temp.first, 0, 'f', decimals1) //
                          .arg(temp.second, 0, 'f', decimals1) //
                          .arg(temp.third, 0, 'f', decimals1) //
                          .arg(opacity1String));
    }

    return result;
}

} // namespace PerceptualColor
