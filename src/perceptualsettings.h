// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALSETTINGS_H
#define PERCEPTUALSETTINGS_H

#include "setting.h"
#include "settings.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qlist.h>
#include <qmetatype.h>
#include <qstring.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{

/** @internal
 *
 * @brief Settings for @ref PerceptualColor.
 *
 * @warning This object is not thread-safe.
 * It must only be used in the main (widget) thread! */
class PerceptualSettings : public Settings
{
    Q_OBJECT

public:
    /** @brief Data type for color lists.
     *
     * Has serialization support as required by <tt>QSettings</tt>
     * thanks to <tt>QList</tt>.
     */
    using ColorList = QList<QColor>;

    virtual ~PerceptualSettings() override;
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    PerceptualSettings(const PerceptualSettings &) = delete;
    PerceptualSettings &operator=(const PerceptualSettings &) = delete;

    [[nodiscard]] static PerceptualSettings &getInstance(const QString &identifier);

    /** @brief Custom colors of @ref ColorDialog. */
    Setting<PerceptualSettings::ColorList> customColors;

    /**
     * @brief Last actually selected (confirmed by Okay button
     * respectively Enter key) color of @ref ColorDialog.
     *
     * @note This contains also the alpha channel.
     *
     * @sa @ref history
     */
    Setting<QColor> lastColor;

    /** @brief History of actually selected (confirmed by Okay button
     * respectively Enter key) colors of @ref ColorDialog.
     *
     * @note This alpha channel (opacity) is stripped. All colors are fully
     * opaque.
     *
     * @sa @ref lastColor
     */
    Setting<PerceptualSettings::ColorList> history;

    /** @brief A specific page from the swatch book.
     *
     * This enum is declared to the meta-object system with <tt>Q_ENUM</tt>.
     * This happens automatically. You do not need to make any manual calls.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object. */
    enum class SwatchBookPage {
        BasicColors, /**< @ref ColorDialogPrivate::m_swatchBookBasicColors */
        History, /**< @ref ColorDialogPrivate::m_swatchBookHistory */
        CustomColors /**< @ref ColorDialogPrivate::m_swatchBookCustomColors */
    };
    Q_ENUM(SwatchBookPage)
    /** @brief The swatch book page currently visible at
     * @ref ColorDialogPrivate::m_swatchBookStack. */
    Setting<SwatchBookPage> swatchBookPage;

    /** @brief The currently visible tab of @ref ColorDialog with
     * @ref ColorDialog::DialogLayoutDimensions::Collapsed. */
    Setting<QString> tab;

    /** @brief The currently visible tab of @ref ColorDialog with
     * @ref ColorDialog::DialogLayoutDimensions::Expanded. */
    Setting<QString> tabExpanded;

private:
    explicit PerceptualSettings(const QString &identifier);

    /** @internal @brief Only for unit tests. */
    friend class TestPerceptualSettings;

    static QString fixedIdentifierWithHyphenMinus(const QString &input);
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::PerceptualSettings::ColorList)
Q_DECLARE_METATYPE(PerceptualColor::PerceptualSettings::SwatchBookPage)

#endif // PERCEPTUALSETTINGS_H
