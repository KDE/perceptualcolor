// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef MULTISPINBOXSECTIONCONFIGURATION_P_H
#define MULTISPINBOXSECTIONCONFIGURATION_P_H

// Include the header of the public class of this private implementation.
// #include "PerceptualColor/multispinboxsectionconfiguration.h"

#include <qstring.h>

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class MultiSpinBoxSectionConfigurationPrivate final
{
public:
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSectionConfiguration::decimals() property */
    int m_decimals = 2;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSectionConfiguration::isWrapping() property */
    bool m_isWrapping = false;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSectionConfiguration::maximum() property */
    double m_maximum = 99.99;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSectionConfiguration::minimum() property */
    double m_minimum = 0;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSectionConfiguration::prefix() property */
    QString m_prefix;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSectionConfiguration::singleStep() property */
    double m_singleStep = 1;
    /** @brief Internal storage of the
     * @ref MultiSpinBoxSectionConfiguration::suffix() property */
    QString m_suffix;
};

} // namespace PerceptualColor

#endif // MULTISPINBOXSECTIONCONFIGURATION_P_H
