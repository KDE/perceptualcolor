// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "portaleyedropper.h"
// Second, the private implementation.
#include "portaleyedropper_p.h" // IWYU pragma: associated

#include <qdbusargument.h>
#include <qdbusconnection.h>
#include <qdbusextratypes.h>
#include <qdbusmessage.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qglobal.h>
#include <qguiapplication.h>
#include <qlist.h>
#include <qmap.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qvariant.h>
#include <qwidget.h>
#include <utility>

namespace PerceptualColor
{

/**
 * @brief Constructor.
 *
 * Also starts asynchroniously the detection wether Portal
 * support @ref isAvailable.
 */
PortalEyedropper::PortalEyedropper()
    : d_pointer(new PortalEyedropperPrivate(this))
{
    QDBusMessage message = QDBusMessage::createMethodCall( //
        QStringLiteral("org.freedesktop.portal.Desktop"), // service
        QStringLiteral("/org/freedesktop/portal/desktop"), // path
        QStringLiteral("org.freedesktop.DBus.Properties"), // interface
        QStringLiteral("Get")); // method
    message << QStringLiteral("org.freedesktop.portal.Screenshot") // argument
            << QStringLiteral("version"); // argument

    QDBusPendingCall pendingCall = //
        QDBusConnection::sessionBus().asyncCall(message);

    auto watcher = new QDBusPendingCallWatcher(pendingCall, this);

    connect(watcher, //
            &QDBusPendingCallWatcher::finished, //
            this, //
            [this](QDBusPendingCallWatcher *myWatcher) {
                myWatcher->deleteLater();
                // Create a reply with exactly 1 argument of type QDBusVariant
                QDBusPendingReply<QDBusVariant> reply(*myWatcher);
                bool temp = false;
                if (!reply.isError()) {
                    const qulonglong actualPortalVersion = //
                        reply.value().variant().toULongLong();
                    if (actualPortalVersion >= 2) {
                        // No screen color picker support available
                        // before Portal version 2.
                        temp = true;
                    }
                }
                d_pointer->m_isAvailable = temp;
                Q_EMIT isAvailableChanged(d_pointer->m_isAvailable);
            });
}

/**
 * @brief Destructor
 */
PortalEyedropper::~PortalEyedropper()
{
}

/**
 * @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 *                 is the private implementation.
 */
PortalEyedropperPrivate::PortalEyedropperPrivate(PortalEyedropper *backLink)
    : q_pointer(backLink)
{
}

/**
 * @brief Destructor
 */
PortalEyedropperPrivate::~PortalEyedropperPrivate() noexcept
{
}

/**
 * @brief Access to the @ref PortalEyedropper singleton.
 *
 * @returns A reference to the instance.
 */
PortalEyedropper &PortalEyedropper::getInstance()
{
    static PortalEyedropper singleton; // Meyer’s Singleton
    return singleton;
}

// No documentation here (documentation of properties
// and its getters are in the header)
std::optional<bool> PortalEyedropper::isAvailable() const
{
    return d_pointer->m_isAvailable;
}

/**
 * @brief Initiates screen color picking via the Portal service.
 *
 * @param eyedropperParent Pointer to the parent widget for this call, or
 * <tt>std::nullptr</tt> if no parent is provided. In rare cases where the Portal
 * displays user interface elements (e.g. a message box requesting user permission
 * for eyedropper functionality), then under the X Window System this dialog will be
 * centered relative to the given parent widget. On other window systems, including
 * Wayland, placement is at the discretion of the Portal service.
 *
 * @post If Portal support for eyedropper functionality is available, the
 * process is started. Results can be obtained via @ref newColor. If not
 * supported, no action is taken.
 */
void PortalEyedropper::startPicking(QWidget *eyedropperParent)
{
    if (!isAvailable()) {
        return;
    }

    // The format of the parent window handle is defined in:
    // https://flatpak.github.io/xdg-desktop-portal/#parent_window
    // It differs between X11 and Wayland.
    //
    // On X11, this is straightforward: QWidget::winId() returns the required
    // identifier.
    //
    // On Wayland, however, QWidget::winId() returns only a pseudo value
    // (an internal reference number) rather than the actual native wl_surface
    // handle. The actual handle can be retrieved via
    // QWaylandNativeInterface, see:
    // https://codebrowser.dev/qt6/qtbase/src/plugins/platforms/wayland/qwaylandnativeinterface.cpp.html
    // It is accessible through  QGuiApplication::platformNativeInterface().
    // However, this is private API, which we avoid by policy.
    // The xdg_foreign protocol is also not useful here, since QWidget does not
    // expose its surfaces through it.
    //
    // For other windowing systems, an empty string should be passed.
    //
    // In summary: we provide a parent window identifier only under the
    // X11 window system.
    QString parentWindowIdentifier;
    if (QGuiApplication::platformName() == QStringLiteral("xcb")) {
        if (eyedropperParent != nullptr) {
            parentWindowIdentifier = QStringLiteral("x11:") //
                + QString::number(eyedropperParent->winId(), 16);
        }
    }

    // “Portal” documentation:
    // https://flatpak.github.io/xdg-desktop-portal/docs/doc-org.freedesktop.portal.Screenshot.html#org-freedesktop-portal-screenshot-pickcolor
    QDBusMessage message = QDBusMessage::createMethodCall( //
        QStringLiteral("org.freedesktop.portal.Desktop"), // service
        QStringLiteral("/org/freedesktop/portal/desktop"), // path
        QStringLiteral("org.freedesktop.portal.Screenshot"), // interface
        QStringLiteral("PickColor")); // method
    message << parentWindowIdentifier // argument: parent_window
            << QVariantMap(); // argument: options
    QDBusPendingCall pendingCall = //
        QDBusConnection::sessionBus().asyncCall(message);
    auto watcher = new QDBusPendingCallWatcher(pendingCall, this);
    connect(watcher, //
            &QDBusPendingCallWatcher::finished, //
            this, //
            [this](QDBusPendingCallWatcher *myWatcher) {
                myWatcher->deleteLater();
                QDBusPendingReply<QDBusObjectPath> reply(*myWatcher);
                if (!reply.isError()) {
                    QDBusConnection::sessionBus().connect(
                        // service
                        QStringLiteral("org.freedesktop.portal.Desktop"),
                        // path
                        reply.value().path(),
                        // interface
                        QStringLiteral("org.freedesktop.portal.Request"),
                        // name
                        QStringLiteral("Response"),
                        // receiver
                        d_pointer.get(),
                        // slot
                        SLOT(getPortalResponse(uint, QVariantMap)));
                    // We ignore the return value of connect() because repeated
                    // calls with the same path() will return false once the
                    // connection is already established. This is expected and
                    // not an error; the slot will still be invoked exactly
                    // once.
                }
            });
}

/**
 * @brief Process the response we get from the “Portal” service.
 *
 * @param exitCode The exit code of the answer.
 * @param responseArguments The arguments of teh answer.
 */
void PortalEyedropperPrivate::getPortalResponse(uint exitCode, const QVariantMap &responseArguments)
{
    if (exitCode != 0) {
        return;
    }
    const QDBusArgument responseColor = responseArguments //
                                            .value(QStringLiteral("color")) //
                                            .value<QDBusArgument>();
    QList<double> rgb;
    responseColor.beginStructure();
    while (!responseColor.atEnd()) {
        double temp;
        responseColor >> temp;
        rgb.append(temp);
    }
    responseColor.endStructure();
    if (rgb.size() == 3) {
        // The documentation of Portal claims to return always sRGB values,
        // so if the screen has a different color space, portal is supposed
        // to apply color management and return the sRGB correspondence.
        Q_EMIT q_pointer->newColor(rgb.at(0), rgb.at(1), rgb.at(2));
    }
}

/**
 * @brief Main event handler.
 *
 * Reimplemented from base class.
 *
 * @param eventParameter The event to be processed.
 *
 * @return This function returns true if the event was recognized, otherwise
 * it returns false.
 *
 * @internal
 *
 * @note This is a dummy reimplementation, provided to ensure future
 * extensibility while preserving binary compatibility, as
 * <a href="https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#You_should...">
 * recommended by the KDE binary compatibility policy</a>.
 * The policy states that
 * <a href="https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#Adding_a_reimplemented_virtual_function">
 * “you can safely reimplement a virtual function defined in one of the base
 * classes only if it is safe that the programs linked with the prior version
 * call the implementation in the base class rather than the derived one.”</a>.
 * Consistent behavior across versions would be difficult to guarantee.
 * By reimplementing the main event handler now,
 * we retain the option to extend it later to process new event types
 * without introducing binary compatibility issues. Since this is the
 * central event dispatcher, it can eventually handle all event types,
 * eliminating the need for dummy overrides of each specialized handler.
 */
bool PortalEyedropper::event(QEvent *eventParameter)
{
    return QObject::event(eventParameter);
}

} // namespace PerceptualColor
