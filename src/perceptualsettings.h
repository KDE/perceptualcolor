// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_PERCEPTUALSETTINGS_H
#define PERCEPTUALCOLOR_PERCEPTUALSETTINGS_H

#include "internalimportexport.h"
#include "setting.h"
#include "settings.h"
#include <qcolor.h>
#include <qlist.h>
#include <qmetatype.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief \l Settings for \l PerceptualColor.
 *
 * \warning This object is not thread-safe.
 * It must only be used in the main (widget) thread! */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT PerceptualSettings : public Settings
{
    Q_OBJECT

public:
    /*!
     * \typealias PerceptualColor::PerceptualSettings::ColorList
     *
     * \brief Data type for color lists.
     *
     * Has serialization support as required by \c {QSettings}
     * thanks to \c {QList}.
     */
    using ColorList = QList<QColor>;

    virtual ~PerceptualSettings() override;
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    PerceptualSettings(const PerceptualSettings &) = delete;
    PerceptualSettings &operator=(const PerceptualSettings &) = delete;

    [[nodiscard]] static PerceptualSettings &getInstance(const QString &identifier);

    /*!
     * \brief Custom colors of \l ColorDialog. */
    Setting<PerceptualSettings::ColorList> customColors;

    /*!
     * \brief Last actually selected (confirmed by Okay button
     * respectively Enter key) color of \l ColorDialog.
     *
     * \note This contains also the alpha channel.
     *
     * \sa history
     */
    Setting<QColor> lastColor;

    /*!
     * \brief History of actually selected (confirmed by Okay button
     * respectively Enter key) colors of \l ColorDialog.
     *
     * \note This alpha channel (opacity) is stripped. All colors are fully
     * opaque.
     *
     * \sa lastColor
     */
    Setting<PerceptualSettings::ColorList> history;

    /*!
     * \brief A specific page from the swatch book.
     *
     * This enum is declared to the meta-object system with \c {Q_ENUM}.
     * This happens automatically. You do not need to make any manual calls.
     *
     * This type is declared as type to Qt’s type system via
     * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
     * example if you want to use for \e {queued} signal-slot connections),
     * you might consider calling \c {qRegisterMetaType()} for
     * this type, once you have a QApplication object.
     *
     * \value BasicColors
     * \l ColorDialogPrivate::m_swatchBookBasicColors
     *
     * \value History
     * \l ColorDialogPrivate::m_swatchBookHistory
     *
     * \value CustomColors
     * \l ColorDialogPrivate::m_swatchBookCustomColors
     */
    enum class SwatchBookPage {
        BasicColors,
        History,
        CustomColors
    };
    Q_ENUM(SwatchBookPage)

    /*!
     * \brief The swatch book page currently visible at
     * \l ColorDialogPrivate::m_swatchBookStack. */
    Setting<SwatchBookPage> swatchBookPage;

    /*!
     * \brief The currently visible tab of \l ColorDialog with
     * \l ColorDialog::DialogLayoutDimensions::Collapsed. */
    Setting<QString> tab;

    /*!
     * \brief The currently visible tab of \l ColorDialog with
     * \l ColorDialog::DialogLayoutDimensions::Expanded. */
    Setting<QString> tabExpanded;

private:
    explicit PerceptualSettings(const QString &identifier);

    friend class TestPerceptualSettings;

    static QString fixedIdentifierWithHyphenMinus(const QString &input);
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::PerceptualSettings::ColorList)
Q_DECLARE_METATYPE(PerceptualColor::PerceptualSettings::SwatchBookPage)

#endif // PERCEPTUALCOLOR_PERCEPTUALSETTINGS_H
