// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_ABSTRACTDIAGRAM_P_H
#define PERCEPTUALCOLOR_ABSTRACTDIAGRAM_P_H

#include <qtclasshelpermacros.h>

namespace PerceptualColor
{
/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 *  \brief Private implementation within the \e {Pointer to
 *  implementation} idiom. */
class AbstractDiagramPrivate
{
public:
    /*!
     * \brief Constructor */
    AbstractDiagramPrivate() = default;
    virtual ~AbstractDiagramPrivate() noexcept;

    /*!
     * \brief Internal storage for \l AbstractDiagram::isActuallyVisible. */
    bool m_isActuallyVisible = false;

private:
    Q_DISABLE_COPY(AbstractDiagramPrivate)
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_ABSTRACTDIAGRAM_P_H
