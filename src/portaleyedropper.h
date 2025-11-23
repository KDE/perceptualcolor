// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_PORTALEYEDROPPER
#define PERCEPTUALCOLOR_PORTALEYEDROPPER

#include <optional>
#include <qcontainerfwd.h>
#include <qglobal.h>
#include <qobject.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

class QWidget;

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Pick a color from the screen using Portal.
 *
 * Provides a simple, yet practical asynchronous interface to let the user
 * pick a color from the screen via Portal.
 *
 * This is an interface to the
 * <a href="https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.Screenshot.html#org-freedesktop-portal-screenshot-pickcolor">
 * org.freedesktop.portal.Screenshot.PickColor Portal service</a>.
 * “Portal” is a Freedesktop (formerly XDG) service maintained by
 * Flatpak intended to provide access to desktop functionality for
 * sandboxed Flatpak applications. It is usually present on many
 * Unix desktop environments.
 *
 * @todo NICETOHAVE Make this class part of the public API (introduce d_pointer
 * before)
 */
class PortalEyedropper : public QObject
{
    Q_OBJECT

    /**
     * @brief Returns the current Portal support status.
     *
     * The asynchronous detection of Portal support is initiated only once
     * during the lifetime of the library, in the constructor of this class.
     *
     * This property represents the availability of Portal support
     * using a three-state logic:
     * - <tt>true</tt>: Portal support is available.
     * - <tt>false</tt>: Portal support is not available.
     * - <tt>std::nullopt</tt>: The detection of Portal support has not yet
     *   completed.
     *
     * @sa READ @ref isAvailable() const
     * @sa NOTIFY @ref isAvailableChanged()
     */
    Q_PROPERTY(std::optional<bool> isAvailable READ isAvailable NOTIFY isAvailableChanged)

public:
    static PortalEyedropper &getInstance();

    /**
     * @brief Getter for property @ref isAvailable
     *
     * @returns the current @ref isAvailable
     */
    [[nodiscard]] std::optional<bool> isAvailable() const;

public Q_SLOTS:
    void startPicking(QWidget *eyedropperParent);

Q_SIGNALS:
    /**
     * @brief Notify signal for property @ref isAvailable.
     *
     *  @param newIsAvailable the new value
     */
    void isAvailableChanged(std::optional<bool> newIsAvailable);

    /**
     * @brief Signal emitted when a new color is selected.
     *
     * The color is in the sRGB color space.
     *
     * This signal is triggered after the user clicks on the screen to
     * pick a color, following a call to @ref startPicking.
     *
     * @param red The <em>red</em> component of the new color.
     *        Range: <tt>[0, 1]</tt>
     * @param green The <em>green</em> component of the new color.
     *        Range: <tt>[0, 1]</tt>
     * @param blue The <em>blue</em> component of the new color.
     *        Range: <tt>[0, 1]</tt>
     */
    // Choosing three “double” values as return type, as it makes clear
    // what data type returns and as “Portal” actually provides
    // double-precision in its return values.
    void newColor(double red, double green, double blue);

private:
    Q_DISABLE_COPY_MOVE(PortalEyedropper)

    /** @internal @brief Only for unit tests. */
    friend class TestPortalEyedropper;

    explicit PortalEyedropper();
    virtual ~PortalEyedropper() override;

    /**
     * @brief Internal storage for property
     * @ref isAvailable */
    std::optional<bool> m_isAvailable = std::nullopt;

private Q_SLOTS:
    void getPortalResponse(uint exitCode, const QVariantMap &responseArguments);
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_PORTALEYEDROPPER
