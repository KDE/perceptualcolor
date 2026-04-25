// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_ABSOLUTECOLOR_H
#define PERCEPTUALCOLOR_ABSOLUTECOLOR_H

#include "genericcolor.h"
#include "helperconversion.h"
#include <array>
#include <optional>
#include <qhash.h>
#include <qlist.h>
#include <qrgb.h>

namespace PerceptualColor
{

/** @internal
 *
 * @brief Toolbox for color conversions.
 *
 * This provides color conversion between absolutely defined color spaces.
 *
 * The functions named like fromXToY provide direct conversions between
 * certain color spaces. The input and output data is @ref GenericColor.
 * These functions have high precision, but are not necesarily optimed
 * for speed. These functions are also registered in @ref convert() which
 * allows to convert between arbitrary combinations of these color spaces.
 *
 * Furthermore, there are some functions prefixed with “fast”, for instance
 * @ref fastFromOklabToSRgbOrTransparent(). These functions are optimized
 * for speed, but might have a slightly lower precision. They convert
 * between specific data formats and are created when more than one
 * widget uses them and code can be shared.
 *
 * @sa @ref RgbColor
 *
 * @internal
 *
 * @todo NICETOHAVE This is a class with no instances. Use free functions
 * in a sub-namespace instead?
 *
 * @todo SHOULDHAVE Test that all possible combinations actually convert.
 * But HSL etc are not supported here, and on the other side @ref ColorModel is
 * also used in @ref CssColor and HSL is needed there. Split into two different
 * enums?
 *
 * @todo SHOULDHAVE Refactoring of this class.
 */
class AbsoluteColor
{
public:
    AbsoluteColor() = delete;

    [[nodiscard]] static QHash<ColorModel, GenericColor> allConversions(const ColorModel model, const GenericColor &value);
    [[nodiscard]] static std::optional<GenericColor> convert(const ColorModel from, const GenericColor &value, const ColorModel to);
    [[nodiscard]] static GenericColor fromXyzD50ToXyzD65(const GenericColor &value);
    [[nodiscard]] static GenericColor fromXyzD65ToXyzD50(const GenericColor &value);
    [[nodiscard]] static GenericColor fromXyzD65ToOklab(const GenericColor &value);
    [[nodiscard]] static GenericColor fromOklabToXyzD65(const GenericColor &value);
    [[nodiscard]] static GenericColor fromXyzD50ToCielabD50(const GenericColor &value);
    [[nodiscard]] static GenericColor fromCielabD50ToXyzD50(const GenericColor &value);
    [[nodiscard]] static GenericColor fromPolarToCartesian(const GenericColor &value);
    [[nodiscard]] static GenericColor fromCartesianToPolar(const GenericColor &value);
    [[nodiscard]] static GenericColor fromLinearSRgbToSRgb(const GenericColor &value);
    [[nodiscard]] static GenericColor fromSRgbToLinearSRgb(const GenericColor &value);
    [[nodiscard]] static GenericColor fromLinearSRgbToXyzD65(const GenericColor &value);
    [[nodiscard]] static GenericColor fromXyzD65ToLinearSRgb(const GenericColor &value);

    [[nodiscard]] static QRgb fastFromOklabToSRgbOrTransparent(const GenericColor &oklab);
    [[nodiscard]] static QRgb fastFromOklabToSRgbClamped(const GenericColor &oklab);

    [[nodiscard]] static QRgb fromCielabD50ToSRgbOrTransparent(const GenericColor &cielabD50);
    [[nodiscard]] static QRgb fromCielchD50ToSRgbClamped(const GenericColor &cielchD50);
    [[nodiscard]] static bool isLabInSRgbGamut(const GenericColor &lab, const LchSpace lchSpace);
    [[nodiscard]] static bool isLchInSRgbGamut(const GenericColor &lch, const LchSpace lchSpace);

    [[nodiscard]] static GenericColor reduceChromaToFitIntoGamut(const GenericColor &lch, const LchSpace lchSpace);

private:
    /**
     * @internal
     *
     * @brief Only for unit tests.
     */
    friend class TestAbsoluteColor;

    /** @brief Function pointer type for the conversion functions. */
    // NOTE std::function<> has nicer syntax for function pointers, but does
    // not allow constexpr.
    using ConversionFunction = GenericColor (*)(const GenericColor &);

    /** @brief Gives access to a conversion function. */
    struct Conversion {
    public:
        /** @brief The color space from which the function converts. */
        ColorModel from;
        /** @brief The color space to which the function converts. */
        ColorModel to;
        /** @brief The function. */
        ConversionFunction conversionFunction;
    };

    /** @brief List of all conversion accesses. */
    static constexpr std::array<Conversion, 14> conversionList //
        {{{ColorModel::XyzD50_1, ColorModel::XyzD65_1, fromXyzD50ToXyzD65},
          {ColorModel::XyzD65_1, ColorModel::XyzD50_1, fromXyzD65ToXyzD50},
          {ColorModel::SRgb_1, ColorModel::LinearSRgb_1, fromSRgbToLinearSRgb},
          {ColorModel::LinearSRgb_1, ColorModel::SRgb_1, fromLinearSRgbToSRgb},
          {ColorModel::XyzD65_1, ColorModel::LinearSRgb_1, fromXyzD65ToLinearSRgb},
          {ColorModel::LinearSRgb_1, ColorModel::XyzD65_1, fromLinearSRgbToXyzD65},
          {ColorModel::OklabD65, ColorModel::XyzD65_1, fromOklabToXyzD65},
          {ColorModel::XyzD65_1, ColorModel::OklabD65, fromXyzD65ToOklab},
          {ColorModel::XyzD50_1, ColorModel::CielabD50, fromXyzD50ToCielabD50},
          {ColorModel::CielabD50, ColorModel::XyzD50_1, fromCielabD50ToXyzD50},
          {ColorModel::CielchD50, ColorModel::CielabD50, fromPolarToCartesian},
          {ColorModel::OklchD65, ColorModel::OklabD65, fromPolarToCartesian},
          {ColorModel::CielabD50, ColorModel::CielchD50, fromCartesianToPolar},
          {ColorModel::OklabD65, ColorModel::OklchD65, fromCartesianToPolar}}};

    [[nodiscard]] static QList<AbsoluteColor::Conversion> conversionsFrom(const ColorModel model);

    static void addDirectConversionsRecursivly(QHash<ColorModel, GenericColor> *values, const ColorModel model);

    [[nodiscard]] static bool isCielchD50InSRgbGamut(const GenericColor &cielchD50);
    [[nodiscard]] static bool isCielabD50InSRgbGamut(const GenericColor &cielabD50);
    [[nodiscard]] static bool isOklabInSRgbGamut(const GenericColor &oklab);
    [[nodiscard]] static bool isOklchInSRgbGamut(const GenericColor &oklch);

    /**
     * @internal
     *
     * @brief Convert a <tt>float</tt> in the range [0..1] to a <tt>quint8</tt>
     * in the range [0..255].
     *
     * @param x A floating point value in the range [0..1].
     *
     * @returns The corresponding value in the range in the range [0..255],
     * rounded to full integers. If the input value
     * differes from the valid input range by more than 0.1, than it returns
     * an arbitrary value.
     */
    template<typename T>
    [[nodiscard]] static quint8 toByte(T x)
    {
        return static_cast<quint8>( //
            x * static_cast<T>(255.f) + static_cast<T>(0.5f));
    }

    /**
     * @internal
     *
     * @brief Convert a channel from linear sRGB to (gamma-encoded)
     * sRGB.
     *
     * @param x An linear sRGB channel in the range [0..1].
     *
     * @returns The corresponding gamma-encoded sRGB channel
     * in the range [0..1] or slightly above or below because
     * of rounding errors.
     *
     * @note The implementations of @ref channelFromLinearSRgbToSRgb() and
     *       @ref channelFromSRgbToLinearSRgb() strictly follow their official
     *       definitions. As a result of rounding in the official
     *       specifications, they do not form exact inverses.
     *
     * @internal
     *
     * @note This function is based on
     * <a href="https://en.wikipedia.org/wiki/SRGB#Transfer_function_(%22gamma%22)">
     * Wikipedia</a>.
     *
     * @note Unfortunately, it cannot be <tt>constexpr</tt> because it relies
     * on <a href="https://en.cppreference.com/w/cpp/numeric/math/pow.html">
     * <tt>std::pow</tt></a> which only becomes <tt>constexpr</tt> in C++26,
     * which is beyond our current target C++ standard.
     */
    template<typename T>
    [[nodiscard]] static T channelFromLinearSRgbToSRgb(T x)
    {
        static_assert(std::is_floating_point<T>::value, //
                      "T must be a floating-point type");
        if (x <= static_cast<T>(0.0031308)) {
            return static_cast<T>(12.92) * x;
        }
        constexpr T exponent = static_cast<T>(1) / static_cast<T>(2.4);
        return //
            static_cast<T>(1.055) * std::pow(x, exponent) //
            - static_cast<T>(0.055);
    }

    /**
     * @internal
     *
     * @brief Convert a channel from sRGB to linear
     * sRGB.
     *
     * @param x An linear sRGB channel in the range [0..1].
     *
     * @returns The corresponding linear sRGB channel
     * in the range [0..1] or slightly above or below because
     * of rounding errors.
     *
     * @note The implementations of @ref channelFromLinearSRgbToSRgb() and
     *       @ref channelFromSRgbToLinearSRgb() strictly follow their official
     *       definitions. As a result of rounding in the official
     *       specifications, they do not form exact inverses.
     *
     * @internal
     *
     * @note This function is based on the
     * <a href="https://en.wikipedia.org/wiki/SRGB#Transfer_function_(%22gamma%22)">
     * Wikipedia</a>.
     *
     * @note Unfortunately, it cannot be <tt>constexpr</tt> because it relies
     * on <a href="https://en.cppreference.com/w/cpp/numeric/math/pow.html">
     * <tt>std::pow</tt></a> which only becomes <tt>constexpr</tt> in C++26,
     * which is beyond our current target C++ standard.
     */
    template<typename T>
    [[nodiscard]] static T channelFromSRgbToLinearSRgb(T x)
    {
        static_assert(std::is_floating_point<T>::value, //
                      "T must be a floating-point type");

        if (x <= static_cast<T>(0.04045)) {
            return x / static_cast<T>(12.92);
        }
        return std::pow( //
            (x + static_cast<T>(0.055)) / static_cast<T>(1.055), //
            static_cast<T>(2.4));
    }
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_ABSOLUTECOLOR_H
