// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#include "hackyeyedropper.h"
#include <qcolor.h>
#include <qcolordialog.h>
#include <qglobal.h>
#include <qlist.h>
#include <qobject.h>
#include <qpushbutton.h>
#include <qstring.h>
#include <qwidget.h>
#include <type_traits>
#include <utility>

namespace PerceptualColor
{

/**
 * @brief Constructor
 *
 *  @param parent pointer to the parent widget, if any
 */
HackyEyedropper::HackyEyedropper(QWidget *parent)
    : QWidget(parent)
{
    hide();
}

/**
 * @brief Destructor
 */
HackyEyedropper::~HackyEyedropper()
{
}

/**
 * @brief If screen color picking is available.
 *
 * @returns If screen color picking is available.
 */
bool HackyEyedropper::isAvailable()
{
    initializeQColorDialogSupport();
    return m_hasQColorDialogSupport.value();
}

/**
 * @brief Translates a given text in the context of QColorDialog.
 *
 * @param sourceText The text to be translated.
 * @returns The translation.
 */
QString HackyEyedropper::translateViaQColorDialog(const char *sourceText)
{
    return QColorDialog::tr(sourceText);
}

/**
 * @brief Test for QColorDialog support, and if available, initialize it.
 *
 * @post @ref m_hasQColorDialogSupport holds wether QColorDialog support is
 * available. If so, also @ref m_qColorDialogScreenButton holds a value.
 *
 * Calling this function the first time might be expensive, but subsequent
 * calls will be cheap.
 */
void HackyEyedropper::initializeQColorDialogSupport()
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
    m_qColorDialog->setOption(QColorDialog::DontUseNativeDialog, true);
    // Prevent interfering with our dialog.
    m_qColorDialog->setOption(QColorDialog::NoButtons, true);
    m_qColorDialog->setOption(QColorDialog::ShowAlphaChannel, false);
#if (QT_VERSION >= QT_VERSION_CHECK(6, 6, 0))
    m_qColorDialog->setOption(QColorDialog::NoEyeDropperButton, false);
#endif
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
                [this](const QColor &color) {
                    Q_EMIT newColor(color.red(), color.green(), color.blue());
                });
    } else {
        delete m_qColorDialog;
        m_qColorDialog = nullptr;
    }
}

/**
 * @brief Initiates screen color picking.
 *
 * @pre This widget must have a parent widget which should be a widget within
 * the currently active window.
 *
 * @warning Internally, QColorDialog is repurposed to perform color picking.
 * This may cause side effects, such as altering the default button  state of
 * the parent dialog. Workaround: If the parent dialog uses a default button,
 * reimplement <tt>QWidget::setVisible()</tt> in the parent dialog as follows:
 * Call the base class implementation. <em>Afterwards</em>, explicitly call
 * <tt>QPushButton::setDefault(true)</tt> on the default button.
 *
 * @post If supported and a parent widget is present, screen color
 * picking is started. The selected color can be retrieved via @ref newColor.
 * If not supported or no parent is available, no action is taken.
 *
 * @param previousColorRed The signal @ref newColor might be
 * emitted with this color if the user cancels the color picking with
 * the ESC key. Range: <tt>[0, 255]</tt>
 * @param previousColorGreen See above.
 * @param previousColorBlue See above.
 */
// Using quint8 to make clear what is the maximum range and maximum precision
// that can be expected. Indeed, QColorDialog uses QColor which allows for
// more precision. However, it seems to not use it: When ESC is pressed,
// previous value is restored only with this precision. So we use quint8
// to make clear which precision will actually be provided of the underlying
// implementation.
void HackyEyedropper::startPicking(int previousColorRed, int previousColorGreen, int previousColorBlue)
{
    if (!parent()) {
        // This class derives from QWidget, and QWidget guarantees
        // that parent() will always return a QWidget (and not just a QObject).
        // Without a parent widget, the QColorDialog hijacking does not work.
        return;
    }

    initializeQColorDialogSupport();
    if (m_qColorDialogScreenButton) {
        const auto previousColor = QColor(previousColorRed, //
                                          previousColorGreen, //
                                          previousColorBlue);
        {
            QSignalBlocker myBlocker(m_qColorDialog);
            m_qColorDialog->setCurrentColor(previousColor);
        }
        m_qColorDialogScreenButton->click();
    }
}

} // namespace PerceptualColor
