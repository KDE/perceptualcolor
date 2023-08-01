// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helperconversion.h"

#include "lchdouble.h"
#include <qglobal.h>

namespace PerceptualColor
{

/** @internal
 *
 * @brief Type conversion.
 * @param value An LCH value
 * @returns Same LCH value as <tt>cmsCIELCh</tt>. */
cmsCIELCh toCmsLch(const LchDouble &value)
{
    cmsCIELCh result;
    result.L = value.l;
    result.C = value.c;
    result.h = value.h;
    return result;
}

/** @internal
 *
 * @brief Type conversion.
 * @param value An LCH value
 * @returns Same LCH value as @ref LchDouble. */
LchDouble toLchDouble(const cmsCIELCh &value)
{
    LchDouble result;
    result.l = value.L;
    result.c = value.C;
    result.h = value.h;
    return result;
}

/** @internal
 *
 * @brief Conversion to @ref LchDouble
 * @param value a point in Lab representation
 * @returns the same point in @ref LchDouble representation */
LchDouble toLchDouble(const cmsCIELab &value)
{
    cmsCIELCh tempLch;
    cmsLab2LCh(&tempLch, &value);
    return toLchDouble(tempLch);
}

/** @internal
 *
 * @brief Conversion to <tt>cmsCIELab</tt>
 * @param value the value to convert
 * @returns the same value as <tt>cmsCIELab</tt> */
cmsCIELab toCmsLab(const cmsCIELCh &value)
{
    cmsCIELab lab; // uses cmsFloat64Number internally
    // convert from LCH to Lab
    cmsLCh2Lab(&lab, &value);
    return lab;
}

#if (QT_VERSION < QT_VERSION_CHECK(6, 0, 0))
/** @internal
 *
 * @brief A qHash function for @ref ColorSpace.
 *
 * Qt5 needs a qHash function if QHash’s key is an enum class. Qt6 does not
 * need this. Therefore, this function is only compiled into the Qt5 builds.
 *
 * @warning This is not part of the public API! It can change or be
 * removed totally at any time. */
uint qHash(const ColorModel t, uint seed) // clazy:exclude=qt6-qhash-signature
{
    using UnderlyingType = std::underlying_type<ColorModel>::type;
    const auto underlyingValue = static_cast<UnderlyingType>(t);
    // “::” selects one of Qt’s qHash functions (instead of recursively calling
    // this very same function).
    return ::qHash(underlyingValue, seed);
}
#endif

} // namespace PerceptualColor
