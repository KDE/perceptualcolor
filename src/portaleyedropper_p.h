// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_COLORWHEEL_P_H
#define PERCEPTUALCOLOR_COLORWHEEL_P_H

// Include the header of the public class of this private implementation.
#include "portaleyedropper.h"

#include "constpropagatingrawpointer.h"
#include <qdbusargument.h>

namespace PerceptualColor
{
class PortalEyedropper;

/**
 * @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom
 */
class PortalEyedropperPrivate : public QObject
{
    Q_OBJECT
public:
    explicit PortalEyedropperPrivate(PortalEyedropper *backLink);
    virtual ~PortalEyedropperPrivate() noexcept override;

    /**
     * @brief Internal storage for property
     * @ref PortalEyedropper::isAvailable */
    std::optional<bool> m_isAvailable = std::nullopt;

public Q_SLOTS:
    void getPortalResponse(uint exitCode, const QVariantMap &responseArguments);

private:
    Q_DISABLE_COPY(PortalEyedropperPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<PortalEyedropper> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_COLORWHEEL_P_H
