// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#include "screencolorpicker.h"
#include "helperqttypes.h"
#include <qcolordialog.h>
#include <qdbusargument.h>
#include <qdbusconnection.h>
#include <qdbusextratypes.h>
#include <qdbusmessage.h>
#include <qdbuspendingcall.h>
#include <qdbuspendingreply.h>
#include <qglobal.h>
#include <qguiapplication.h>
#include <qlist.h>
#include <qobjectdefs.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qvariant.h>
#include <qwidget.h>
#include <type_traits>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qmap.h>
#else
#include <qmetatype.h>
#endif

namespace PerceptualColor
{

/** @brief Constructor
 *
 *  @param parent pointer to the parent widget, if any */
ScreenColorPicker::ScreenColorPicker(QWidget *parent)
    : QWidget(parent)
{
    hide();
}

/** @brief Destructor */
ScreenColorPicker::~ScreenColorPicker()
{
}

/** @brief If screen color picking is available at the current platform.
 *
 * @returns If screen color picking is available at the current platform. */
bool ScreenColorPicker::isAvailable()
{
    if (hasPortalSupport()) {
        return true;
    }
    initializeQColorDialogSupport();
    return m_hasQColorDialogSupport.value();
}

/** @brief If “Portal” support is available.
 *
 * “Portal” is a Freedesktop (formerly XDG) service maintained by
 * Flatpack intended to provide access to desktop functionality for
 * sandboxed Flatpack applications.
 *
 * @returns If “Portal” support is available. */
bool ScreenColorPicker::hasPortalSupport()
{
    static const bool m_hasPortalSupport = queryPortalSupport();
    return m_hasPortalSupport;
}

/** @brief Make a DBus query for “Portal” screen color picker support.
 *
 * This function makes a synchronous DBus query to see if there is
 * support for screen color picker in the current system.
 * It might be slow.
 *
 * @note Do not use this function directly. Instead, for performance
 * reasons, use @ref hasPortalSupport which provides a cached value.
 *
 * @returns If there is support for “Portal” color picking. */
bool ScreenColorPicker::queryPortalSupport()
{
    QDBusMessage message = QDBusMessage::createMethodCall( //
        QStringLiteral("org.freedesktop.portal.Desktop"), // service
        QStringLiteral("/org/freedesktop/portal/desktop"), // path
        QStringLiteral("org.freedesktop.DBus.Properties"), // interface
        QStringLiteral("Get")); // method
    message << QStringLiteral("org.freedesktop.portal.Screenshot") // argument
            << QStringLiteral("version"); // argument
    const QDBusMessage reply = QDBusConnection::sessionBus().call(message);
    if (reply.type() != QDBusMessage::MessageType::ReplyMessage) {
        return false;
    }
    constexpr quint8 minimumSupportedPortalVersion = 2;
    const qulonglong actualPortalVersion = reply //
                                               .arguments() //
                                               .value(0) //
                                               .value<QDBusVariant>() //
                                               .variant() //
                                               .toULongLong();
    if (actualPortalVersion < minimumSupportedPortalVersion) {
        // No screen color picker support available
        return false;
    }
    return true;
}

/** @brief Translates a given text in the context of QColorDialog.
 *
 * @param sourceText The text to be translated.
 * @returns The translation. */
QString ScreenColorPicker::translateViaQColorDialog(const char *sourceText)
{
    return QColorDialog::tr(sourceText);
}

/** @brief Test for QColorDialog support, and if available, initialize it.
 *
 * @post @ref m_hasQColorDialogSupport holds if QColorDialog support is
 * available. If so, also @ref m_qColorDialogScreenButton holds a value.
 *
 * Calling this function the first time might be expensive, but subsequent
 * calls will be cheap.
 *
 * @note This basically hijacks QColorDialog’s screen picker, but
 * this relies on internals of Qt and could therefore theoretically
 * fail in later Qt versions. On the other hand, making a
 * cross-platform implementation ourself would also be a lot
 * of work. However, if we could solve this, we could claim again at
 * @ref index "main page" that we do not use internal APIs. There is
 * also a <a href="https://bugreports.qt.io/browse/QTBUG-109440">request
 * to add a public API to Qt</a> for this. */
void ScreenColorPicker::initializeQColorDialogSupport()
{
    if (m_hasQColorDialogSupport.has_value()) {
        if (m_hasQColorDialogSupport.value() == false) {
            // We know yet from a previous attempt that there is no
            // support for QColorDialog.
            return;
        }
    }

    if (m_qColorDialogScreenButton) {
        // Yet initialized.
        return;
    }

    m_qColorDialog = new QColorDialog();
    m_qColorDialog->setOptions( //
        QColorDialog::DontUseNativeDialog | QColorDialog::NoButtons);
    const auto buttonList = m_qColorDialog->findChildren<QPushButton *>();
    for (const auto &button : std::as_const(buttonList)) {
        button->setDefault(false); // Prevent interfering with our dialog.
        // Going through translateViaQColorDialog() to avoid that the
        // string will be included in our own translation file; instead
        // intentionally fallback to Qt-provided translation.
        if (button->text() == translateViaQColorDialog("&Pick Screen Color")) {
            m_qColorDialogScreenButton = button;
        }
    }
    m_hasQColorDialogSupport = m_qColorDialogScreenButton;
    if (m_hasQColorDialogSupport) {
        m_qColorDialog->setParent(this);
        m_qColorDialog->hide();
        connect(m_qColorDialog, //
                &QColorDialog::currentColorChanged, //
                this, //
                &ScreenColorPicker::newColor);
    } else {
        delete m_qColorDialog;
        m_qColorDialog = nullptr;
    }
}

/** @brief Start the screen color picking.
 *
 * @pre This widget has a parent widget which should be a widget within
 * the currently active window.
 *
 * @post If supported on the current platform, the screen color picking is
 * started. Results can be obtained via @ref newColor.
 *
 * @param previousColor On some platforms, the signal @ref newColor is
 * emitted with this color if the user cancels the color picking with
 * the ESC key. */
void ScreenColorPicker::startPicking(const QColor &previousColor)
{
    if (!parent()) {
        // This class derives (currently) from QWidget, and QWidget guarantees
        // that parent() will always return a QWidget (and not just a QObject).
        // Without a parent widget, the QColorDialog support does not work.
        // While the Portal support works also without parent widgets, it
        // seems better to enforce a widget parent here, so that we get
        // consistent behaviour for all possible backends.
        return;
    }

    // The “Portal” implementation has priority over the “QColorDialog”
    // implementation, because
    // 1. “Portal” works reliably also on multi-monitor setups.
    //    QColorDialog doesn’t: https://bugreports.qt.io/browse/QTBUG-94748
    //    In Qt 6.5, QColorDialog starts to use “Portal” too, see
    //    https://bugreports.qt.io/browse/QTBUG-81538 but only for Wayland,
    //    and not for X11. We, however, also want it for X11.
    // 2. The “QColorDialog” implementation is a hack because it relies on
    //    Qt’s internals, which could change in future versions and break
    //    our implementation, so we should avoid it if we can.
    if (hasPortalSupport()) {
        pickWithPortal();
        return;
    }

    initializeQColorDialogSupport();
    if (m_qColorDialogScreenButton) {
        m_qColorDialog->setCurrentColor(previousColor);
        m_qColorDialogScreenButton->click();
    }
}

/** @brief Start color picking using the “Portal”. */
void ScreenColorPicker::pickWithPortal()
{
    // For “Portal”, the parent window identifier is used if the
    // requested function shows a dialog: This dialog will then be
    // centered within and modal to the parent window. This includes
    // permission dialog with which the user is asked if he grants permission
    // to the application to use the requested function. Apparently,
    // for screen color picker there is no permission dialog in KDE, so the
    // identifier is rather useless. The format of the handle is defined in
    // https://flatpak.github.io/xdg-desktop-portal/#parent_window
    // and has different content for X11 and Wayland. X11 is easy to
    // implement, while Wayland handles are more complex requiring a call
    // with the xdg_foreign protocol. For other windowing systems, an
    // empty string should be used. While tests show that is works fine
    // with an empty string in X11, we provide at least the easy
    // identifier for X11.
    QString parentWindowIdentifier;
    if (QGuiApplication::platformName() == QStringLiteral("xcb")) {
        const QWidget *const parentWidget = qobject_cast<QWidget *>(parent());
        if (parentWidget != nullptr) {
            parentWindowIdentifier = QStringLiteral("x11:") //
                + QString::number(parentWidget->winId(), 16);
        }
    }

    // “Portal” documentation: https://flatpak.github.io/xdg-desktop-portal
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
                QDBusPendingReply<QDBusObjectPath> reply = *myWatcher;
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

/** @brief Process the response we get from the “Portal” service. */
void ScreenColorPicker::getPortalResponse(uint exitCode, const QVariantMap &responseArguments)
{
    if (exitCode != 0) {
        return;
    }
    const QDBusArgument responseColor = responseArguments //
                                            .value(QStringLiteral("color")) //
                                            .value<QDBusArgument>();
    QList<QColorFloatType> rgb;
    responseColor.beginStructure();
    while (!responseColor.atEnd()) {
        double temp;
        responseColor >> temp;
        rgb.append(static_cast<QColorFloatType>(temp));
    }
    responseColor.endStructure();
    if (rgb.count() == 3) {
        Q_EMIT newColor(QColor::fromRgbF(rgb.at(0), rgb.at(1), rgb.at(2)));
    }
}

} // namespace PerceptualColor
