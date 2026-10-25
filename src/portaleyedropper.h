// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_PORTALEYEDROPPER
#define PERCEPTUALCOLOR_PORTALEYEDROPPER

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <optional>
#include <qcontainerfwd.h>
#include <qglobal.h>
#include <qobject.h>
#include <qtmetamacros.h>

class QWidget;

namespace PerceptualColor
{
class PortalEyedropperPrivate;

/*!
 * \inmodule ModulePublicApi
 *
 * \brief Pick a color from the screen using Portal.
 *
 * Provides a simple, yet practical asynchronous interface to let the user
 * pick a color from the screen via Portal.
 *
 * This is an interface to the
 * \l {https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.Screenshot.html#org-freedesktop-portal-screenshot-pickcolor}
 * {org.freedesktop.portal.Screenshot.PickColor Portal service}.
 * “Portal” is a Freedesktop (formerly XDG) service maintained by
 * Flatpak intended to provide access to desktop functionality for
 * sandboxed Flatpak applications. It is usually present on many
 * Unix desktop environments.
 */
class PERCEPTUALCOLOR_IMPORTEXPORT PortalEyedropper : public QObject
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::PortalEyedropper::isAvailable
     *
     * \brief Returns the current Portal support status.
     *
     * The asynchronous detection of Portal support is initiated only once
     * during the lifetime of the library, in the constructor of this class.
     *
     * This property represents the availability of Portal support
     * using a three-state logic:
     *
     * \list
     *     \li \c {true}: Portal support is available.
     *     \li \c {false}: Portal support is not available.
     *     \li \c {std::nullopt}: The detection of Portal support has not yet
     *         completed.
     * \endlist
     */
    Q_PROPERTY(std::optional<bool> isAvailable READ isAvailable NOTIFY isAvailableChanged)

public:
    static PortalEyedropper &getInstance();

    /*!
     * \brief Getter for property \l isAvailable
     */
    [[nodiscard]] std::optional<bool> isAvailable() const;

public Q_SLOTS:
    void startPicking(QWidget *eyedropperParent);

Q_SIGNALS:
    /*!
     * \brief Notify signal for property \l isAvailable.
     *
     *  \a newIsAvailable the new value
     */
    void isAvailableChanged(std::optional<bool> newIsAvailable);

    // Choosing three “double” values as return type, as it makes clear
    // what data type returns and as “Portal” actually provides
    // double-precision in its return values.
    /*!
     * \brief Signal emitted when a new color is selected.
     *
     * The color in in the sRGB color space.
     *
     * This signal is triggered after the user clicks on the screen to
     * pick a color, following a call to \l startPicking.
     *
     * \a red The \e {red} component of the new color.
     *        Range: \c {[0, 1]}
     *
     * \a green The \e {green} component of the new color.
     *        Range: \c {[0, 1]}
     *
     * \a blue The \e {blue} component of the new color.
     *        Range: \c {[0, 1]}
     */
    void newColor(double red, double green, double blue);

protected:
    virtual bool event(QEvent *eventParameter) override;

private:
    Q_DISABLE_COPY_MOVE(PortalEyedropper)

    friend class TestPortalEyedropper;

    explicit PortalEyedropper();

    virtual ~PortalEyedropper() override;

    /*!
     * \brief Pointer to implementation (pimpl)
     */
    ConstPropagatingUniquePointer<PortalEyedropperPrivate> d_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_PORTALEYEDROPPER
