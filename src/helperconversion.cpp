// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helperconversion.h"

#include "genericcolor.h"
#include <qglobal.h>

namespace PerceptualColor
{

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

} // namespace PerceptualColor
