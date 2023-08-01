// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helperconversion.h"

#include "genericcolor.h"
#include <qglobal.h>

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Conversion to @ref GenericColor with @ref ColorModel::CielabD50
 * @param value a point in Lab representation
 * @returns the same point as @ref GenericColor with
 * @ref ColorModel::CielabD50
 */
GenericColor toGenericColorCielabD50(const cmsCIELab &value)
{
    cmsCIELCh tempLch;
    cmsLab2LCh(&tempLch, &value);
    return GenericColor(tempLch);
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
