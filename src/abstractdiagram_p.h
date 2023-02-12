// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef ABSTRACTDIAGRAM_P_H
#define ABSTRACTDIAGRAM_P_H

// Include the header of the public class of this private implementation.
// #include "abstractdiagram.h"

#include <qglobal.h>

namespace PerceptualColor
{
/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class AbstractDiagramPrivate final
{
public:
    /** @brief Constructor */
    AbstractDiagramPrivate() = default;
    /** @brief Default destructor
     *
     * The destructor is non-<tt>virtual</tt> because
     * the class as a whole is <tt>final</tt>. */
    ~AbstractDiagramPrivate() noexcept = default;

    /** @brief Internal storage for @ref AbstractDiagram::isActuallyVisible. */
    bool m_isActuallyVisible = false;

private:
    Q_DISABLE_COPY(AbstractDiagramPrivate)
};

} // namespace PerceptualColor

#endif // ABSTRACTDIAGRAM_P_H
