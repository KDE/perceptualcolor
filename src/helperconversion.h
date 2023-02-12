// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERCONVERSION_H
#define HELPERCONVERSION_H

#include "lchdouble.h"
#include <lcms2.h>
#include <qgenericmatrix.h>

/** @internal
 *
 * @file
 *
 * Provides type conversions. */

namespace PerceptualColor
{

[[nodiscard]] cmsCIELab fromCmscielabD50ToOklab(const cmsCIELab &cielabD50);

[[nodiscard]] QGenericMatrix<1, 3, double> fromXyzd65ToOklab(const QGenericMatrix<1, 3, double> &value);

[[nodiscard]] cmsCIELab toCmsCieLab(const cmsCIELCh &value);

[[nodiscard]] cmsCIELCh toCmsCieLch(const LchDouble &value);

[[nodiscard]] LchDouble toCielchDouble(const cmsCIELCh &value);

[[nodiscard]] LchDouble toCielchDouble(const cmsCIELab &value);

/** @brief White point D65 for 2°-observer.
 *
 * According to
 * <a href="https://en.wikipedia.org/w/index.php?title=Illuminant_D65&oldid=1100467073#Definition">
 * Wikipedia</a>:
 *
 * > “Using the standard 2° observer […] of D65 […] Normalizing for
 * > relative luminance (i.e. set Y=100), the XYZ tristimulus
 * > values are:<br/>
 * > X = 95.047<br/>
 * > Y = 100<br/>
 * > Z = 108.883”
 *
 * Normalizing this to Y = 1 as expected by LittleCMS, gives this value. */
constexpr cmsCIEXYZ whitePointD65TwoDegree{0.95047, 1.00000, 1.08883};

} // namespace PerceptualColor

#endif // HELPERCONVERSION_H
