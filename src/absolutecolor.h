// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef ABSOLUTECOLOR_H
#define ABSOLUTECOLOR_H

#include "genericcolor.h"
#include "helperconversion.h"
#include <array>
#include <optional>
#include <qhash.h>
#include <qlist.h>

namespace PerceptualColor
{

/** @internal
 *
 * @brief Toolbox for color conversions.
 *
 * @sa @ref RgbColor */
class AbsoluteColor final
{
public:
    AbsoluteColor() = delete;

    [[nodiscard]] static QHash<ColorModel, GenericColor> allConversions(const ColorModel model, const GenericColor &value);
    [[nodiscard]] static std::optional<GenericColor> convert(const ColorModel from, const GenericColor &value, const ColorModel to);
    static GenericColor fromXyzD50ToXyzD65(const GenericColor &value);
    static GenericColor fromXyzD65ToXyzD50(const GenericColor &value);
    static GenericColor fromXyzD65ToOklab(const GenericColor &value);
    static GenericColor fromOklabToXyzD65(const GenericColor &value);
    static GenericColor fromXyzD50ToCielabD50(const GenericColor &value);
    static GenericColor fromCielabD50ToXyzD50(const GenericColor &value);
    static GenericColor fromPolarToCartesian(const GenericColor &value);
    static GenericColor fromCartesianToPolar(const GenericColor &value);

private:
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
    static constexpr std::array<Conversion, 10> conversionList //
        {{{ColorModel::XyzD50, ColorModel::XyzD65, fromXyzD50ToXyzD65},
          {ColorModel::XyzD65, ColorModel::XyzD50, fromXyzD65ToXyzD50},
          {ColorModel::OklabD65, ColorModel::XyzD65, fromOklabToXyzD65},
          {ColorModel::XyzD65, ColorModel::OklabD65, fromXyzD65ToOklab},
          {ColorModel::XyzD50, ColorModel::CielabD50, fromXyzD50ToCielabD50},
          {ColorModel::CielabD50, ColorModel::XyzD50, fromCielabD50ToXyzD50},
          {ColorModel::CielchD50, ColorModel::CielabD50, fromPolarToCartesian},
          {ColorModel::OklchD65, ColorModel::OklabD65, fromPolarToCartesian},
          {ColorModel::CielabD50, ColorModel::CielchD50, fromCartesianToPolar},
          {ColorModel::OklabD65, ColorModel::OklchD65, fromCartesianToPolar}}};

    [[nodiscard]] static QList<AbsoluteColor::Conversion> conversionsFrom(const ColorModel model);

    static void addDirectConversionsRecursivly(QHash<ColorModel, GenericColor> *values, const ColorModel model);
};

} // namespace PerceptualColor

#endif // ABSOLUTECOLOR_H
