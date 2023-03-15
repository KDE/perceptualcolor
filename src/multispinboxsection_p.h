﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef MULTISPINBOXSECTION_P_H
#define MULTISPINBOXSECTION_P_H

// Include the header of the public class of this private implementation.
// #include "multispinboxsection.h"

#include <qstring.h>

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class MultiSpinBoxSectionPrivate final
{
public:
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSection::decimals() property */
    int m_decimals = 2;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSection::isWrapping() property */
    bool m_isWrapping = false;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSection::maximum() property */
    double m_maximum = 99.99;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSection::minimum() property */
    double m_minimum = 0;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSection::prefix() property */
    QString m_prefix;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSection::singleStep() property */
    double m_singleStep = 1;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSection::suffix() property */
    QString m_suffix;
};

} // namespace PerceptualColor

#endif // MULTISPINBOXSECTION_P_H
