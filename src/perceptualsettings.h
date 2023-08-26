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
#else
#include <qobjectdefs.h>
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
     * Has serialization support as required by <tt>QSettings</tt>. */
    using ColorList = QList<QColor>;

    virtual ~PerceptualSettings() override;
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    PerceptualSettings(const PerceptualSettings &) = delete;
    PerceptualSettings &operator=(const PerceptualSettings &) = delete;

    static PerceptualSettings &instance();

    /** @brief Custom colors of @ref ColorDialog. */
    Setting<ColorList> customColors;

    /** @brief History of actually selected (confirmed by Okay button
     * respectively Enter key) colors of @ref ColorDialog. */
    Setting<ColorList> history;

    /** @brief The currently visible tab of @ref ColorDialog with
     * @ref ColorDialog::DialogLayoutDimensions::Collapsed. */
    Setting<QString> tab;

    /** @brief The currently visible tab of @ref ColorDialog with
     * @ref ColorDialog::DialogLayoutDimensions::Expanded. */
    Setting<QString> tabExpanded;

private:
    PerceptualSettings();

    /** @internal @brief Only for unit tests. */
    friend class TestPerceptualSettings;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::PerceptualSettings::ColorList)

#endif // PERCEPTUALSETTINGS_H
