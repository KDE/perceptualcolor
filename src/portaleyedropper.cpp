// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#include "portaleyedropper.h"
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
                m_isAvailable = temp;
                Q_EMIT isAvailableChanged(m_isAvailable);
            });
}

/**
 * @brief Destructor
 */
PortalEyedropper::~PortalEyedropper()
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
    return m_isAvailable;
}

/**
 * @brief Start the screen color picking.
 *
 * @param eyedropperParent Pointer to the parent widget for this call, or
 * <tt>std::nullptr</tt> for no parent. In the rare case that Portal displays
 * user interface elements like e.g. a message box to request user permission
 * for the  eyedropper functionality, some portal implementations might use
 * this information to improve positioning.
 *
 * @post If Portal support for eyedropper functionality is available, it is
 * started. Results can be obtained via @ref newColor. If not supported,
 * no action is taken.
 *
 * @internal
 *
 * @todo NICETOHAVE Currently, placing Portal dialogs for user interaction
 * relative to the center of the parent widget only works on X11. On Wayland,
 * QWidget->winID() returns pseudo values instead of the actual
 * native wl_surface handle, which would be necessary.
 * It is possible to get the actual handle through a
 * <a href="https://codebrowser.dev/qt6/qtbase/src/plugins/platforms/wayland/qwaylandnativeinterface.cpp.html">
 * QWaylandNativeInterface</a> object, which in return can be obtained through
 * <tt>QGuiApplication::platformNativeInterface()</tt>, but all this is
 * private API, which we do not use by policy.
 */
void PortalEyedropper::startPicking(QWidget *eyedropperParent)
{
    if (!isAvailable()) {
        return;
    }

    // The format of the handle to identify the parent window is defined in
    // https://flatpak.github.io/xdg-desktop-portal/#parent_window
    // and has different content for X11 and Wayland. X11 is easy to
    // implement through QWidget::winId(), which returns it. Unfortunately, on
    // Wayland, QWidget::inId() does not the necessary wl_surface from Wayland,
    // but instead an internal reference number. To get the corrent handle
    // on Wayland would require to call the the xdg_foreign protocol. For other
    // windowing systems, an empty string should be used. While tests show that
    // it works fine with an empty string in X11, we provide at least the easy
    // identifier for X11.
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
                        this,
                        // slot
                        SLOT(getPortalResponse(uint, QVariantMap)));
                    // Ignoring the result of connect() because subsequent
                    // calls might occur with the same path(), which will
                    // make connect() return “false” because the connection
                    // is yet established, which is okay and not a failure;
                    // the slot will be called only once nevertheless.
                }
            });
}

/**
 * @brief Process the response we get from the “Portal” service.
 *
 * @param exitCode The exit code of the answer.
 * @param responseArguments The arguments of teh answer.
 */
void PortalEyedropper::getPortalResponse(uint exitCode, const QVariantMap &responseArguments)
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
        Q_EMIT newColor(rgb.at(0), rgb.at(1), rgb.at(2));
    }
}

} // namespace PerceptualColor
