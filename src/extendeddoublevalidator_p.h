// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef EXTENDEDDOUBLEVALIDATOR_P_H
#define EXTENDEDDOUBLEVALIDATOR_P_H

// Include the header of the public class of this private implementation.
// #include "extendeddoublevalidator.h"

#include <qglobal.h>
#include <qstring.h>

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class ExtendedDoubleValidatorPrivate final
{
public:
    /** @brief Constructor */
    ExtendedDoubleValidatorPrivate() = default;
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~ExtendedDoubleValidatorPrivate() noexcept = default;

    /** @brief Internal storage for property
     * @ref ExtendedDoubleValidator::prefix */
    QString m_prefix;
    /** @brief Internal storage for property
     * @ref ExtendedDoubleValidator::suffix */
    QString m_suffix;

private:
    Q_DISABLE_COPY(ExtendedDoubleValidatorPrivate)
};

} // namespace PerceptualColor

#endif // EXTENDEDDOUBLEVALIDATOR_P_H
