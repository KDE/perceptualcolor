// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef SETTINGS_H
#define SETTINGS_H

#include <qcolor.h>
#include <qfilesystemwatcher.h>
#include <qglobal.h>
#include <qlist.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qsettings.h>
#include <qstring.h>
#include <qstringliteral.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

namespace PerceptualColor
{

/** @internal
 *
 * @brief Represents the settings file and allows for inter-process
 * communication of changes.
 *
 * This object provides persistent, platform-independent settings and writes
 * changes to the settings file relatively quickly. Changes made to the
 * settings file by other processes are read in immediately, and the
 * corresponding NOTIFY signal is emitted for changed properties.
 *
 * @warning This object is implemented as a singleton and is not thread-safe.
 * It must only be used in the main (widget) thread!
 *
 * @note For more sophisticated use cases requiring
 * type-safe access to <tt>QSettings</tt>, there are
 * <a href="https://www.vikingsoftware.com/blog/more-type-safety-with-qsettings/">
 * alternative approaches available</a>, but they may be overkill for our
 * limited requirements here. */
class Settings : public QObject
{
    Q_OBJECT

    /** @brief Custom colors of @ref ColorDialog.
     *
     * @sa READ @ref customColors() const
     * @sa WRITE @ref setCustomColors()
     * @sa NOTIFY @ref customColorsChanged */
    Q_PROPERTY(ColorList customColors READ customColors WRITE setCustomColors NOTIFY customColorsChanged)

    /** @brief The currently visible tag of @ref ColorDialog with
     * @ref ColorDialog::DialogLayoutDimensions::Collapsed.
     *
     * @sa READ @ref tab() const
     * @sa WRITE @ref setTab()
     * @sa NOTIFY @ref tabChanged */
    Q_PROPERTY(QString tab READ tab WRITE setTab NOTIFY tabChanged)

    /** @brief The currently visible tag of @ref ColorDialog with
     * @ref ColorDialog::DialogLayoutDimensions::Expanded.
     *
     * @sa READ @ref tabExpanded() const
     * @sa WRITE @ref setTabExpanded()
     * @sa NOTIFY @ref tabExpandedChanged */
    Q_PROPERTY(QString tabExpanded READ tabExpanded WRITE setTabExpanded NOTIFY tabExpandedChanged)

public:
    /** @brief Data type for the @ref customColors property.
     *
     * Has serialization support as required by <tt>QSettings</tt>. */
    using ColorList = QList<QColor>;

    virtual ~Settings() override = default;
    // Prevent copy and assignment operations to force that only references
    // to the instance are possible.
    Settings(const Settings &) = delete;
    Settings &operator=(const Settings &) = delete;

    static Settings &instance();

    /** @brief Getter for property @ref customColors
     *  @returns the property @ref customColors */
    Settings::ColorList customColors() const;
    void setCustomColors(const ColorList &newCustomColors);
    void setTab(const QString &newTab);
    void setTabExpanded(const QString &newTab);
    /** @brief Getter for property @ref tab
     *  @returns the property @ref tab */
    QString tab() const;
    /** @brief Getter for property @ref tabExpanded
     *  @returns the property @ref tabExpanded */
    QString tabExpanded() const;

Q_SIGNALS:
    /** @brief Notify signal for property @ref customColors.
     * @param newCustomColors the new options */
    void customColorsChanged(const PerceptualColor::Settings::ColorList &newCustomColors);
    /** @brief Notify signal for property @ref tab.
     * @param newTab the new tab */
    void tabChanged(const QString &newTab);
    /** @brief Notify signal for property @ref tabExpanded.
     * @param newTab the new tab */
    void tabExpandedChanged(const QString &newTab);

private:
    Settings();

    // For maximum portability:
    // - No upper case should ever be used.
    //   (Some systems, like the INI that we are using, are case-insensitive.
    //   And even if we always use INI, having both capital and small letters
    //   is error-prone because typos are not checked by the compiler.)
    // - Only the letters a-z should be used.
    //   (Also, some characters like the backslash are not allowed on
    //   platforms.)
    // - “group/key”: Each key has exactly one group. Don't use subgroups.
    //   Use the class name as group name.
    //   (This makes the settings file well readable for humans. Missing
    //   groups are confusing because the system generates a “General”
    //   group which is not easy to understand. And using class identifiers
    //   helps to understand the structure of the settings file.)
    // - In C++, use “const” variables to define key strings, instead of
    //   manually typing the key strings.
    //   (This avoids typing errors.)
    /** @brief <tt>QSettings</tt> key for the @ref customColors property. */
    static const inline auto keyCustomColors = QStringLiteral("colordialog/customcolors");
    /** @brief <tt>QSettings</tt> key for the @ref tab property. */
    static const inline auto keyTab = QStringLiteral("colordialog/tab");
    /** @brief <tt>QSettings</tt> key for the @ref tabExpanded property. */
    static const inline auto keyTabExpanded = QStringLiteral("colordialog/tabexpanded");

    /** @brief Internal storage for property @ref customColors */
    ColorList m_customColors;
    /** @brief Internal storage for property @ref tab */
    QString m_tab;
    /** @brief Internal storage for property @ref tabExpanded */
    QString m_tabExpanded;

    /** @brief The internal QSettings object.
     *
     * There are important reasons to use <tt>QSettings::IniFormat</tt>.
     *
     * - It makes <tt>QSettings</tt> behave identical on all platforms.
     *   Though <tt>QSettings</tt> is an abstraction, it has still a lot
     *   of platform-dependant behaviour, like the fact the numbers are
     *   saved as numbers but read back as QString when using
     *   <tt>QSettings::IniFormat</tt> or when using the native format
     *   and the native platform uses Ini (like Linux); other platforms
     *   preserve the type information. By using <tt>QSettings::IniFormat</tt>
     *   the behaviour becomes at least predictable and is identical also
     *   cross-platform.
     *
     * - <tt>QSettings::IniFormat</tt> is a file-based approach (while the
     *   native approach for example on Windows is the Windows Registry
     *   instead of a file). Using a file is necessary to be able to monitor
     *   changes that other processes might make. */
    QSettings m_qSettings = QSettings(QSettings::IniFormat, QSettings::UserScope, QStringLiteral("kde.org"), QStringLiteral("libperceptualcolor"));
    /** @brief A watcher for the file used by @ref m_qSettings.
     *
     * This allows to react immediately to settings changes done by other
     * applications using this library. This is also useful as simple but
     * cross-platform inter-process communication for synchronizing for
     * example custom colors between various simultaneously running
     * applications using this library. */
    QFileSystemWatcher m_watcher;

    void updateFromFile();

    /** @internal @brief Only for unit tests. */
    friend class TestSettings;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::Settings::ColorList)

#endif // SETTINGS_H
