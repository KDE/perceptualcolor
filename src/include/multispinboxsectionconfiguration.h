// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef MULTISPINBOXSECTIONCONFIGURATION_H
#define MULTISPINBOXSECTIONCONFIGURATION_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qdebug.h>
#include <qglobal.h>
#include <qmetatype.h>
#include <qstring.h>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
#include "multispinboxsectionconfiguration_p.h"
#else
namespace PerceptualColor
{
class MultiSpinBoxSectionConfigurationPrivate;
}
#endif

namespace PerceptualColor
{

/** @brief The configuration of a single section
 * within a @ref MultiSpinBox.
 *
 * For a specific section within a @ref MultiSpinBox, this configuration
 * contains various settings.
 *
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSectionConfiguration &value)
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 *
 * @internal
 *
 * Also Qt itself uses this configuration-object-based approach with its
 * QNetworkConfiguration class (including @ref pimpl and
 * copy-constructors). */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBoxSectionConfiguration
{
public:
    MultiSpinBoxSectionConfiguration();
    MultiSpinBoxSectionConfiguration(const MultiSpinBoxSectionConfiguration &other);
    ~MultiSpinBoxSectionConfiguration() noexcept;
    MultiSpinBoxSectionConfiguration &operator=(const MultiSpinBoxSectionConfiguration &other);
    MultiSpinBoxSectionConfiguration(MultiSpinBoxSectionConfiguration &&other) noexcept;
    MultiSpinBoxSectionConfiguration &operator=(MultiSpinBoxSectionConfiguration &&other) noexcept;

    [[nodiscard]] int decimals() const;
    [[nodiscard]] bool isWrapping() const;
    [[nodiscard]] double maximum() const;
    [[nodiscard]] double minimum() const;
    [[nodiscard]] QString prefix() const;
    void setDecimals(int newDecimals);
    void setMaximum(double newMaximum);
    void setMinimum(double newMinimum);
    void setPrefix(const QString &newPrefix);
    void setSingleStep(double newSingleStep);
    void setSuffix(const QString &newSuffix);
    void setWrapping(bool newIsWrapping);
    [[nodiscard]] double singleStep() const;
    [[nodiscard]] QString suffix() const;

private:
    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class MultiSpinBoxSectionConfigurationPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<MultiSpinBoxSectionConfigurationPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestMultiSpinBoxSectionConfiguration;

    /** @internal
     * @brief Internal friend declaration.
     *
     * This class is used as configuration for @ref MultiSpinBox. Thought
     * currently there is no need for this <tt>friend</tt> declaration,
     * it is done nevertheless. (If it would become necessary later,
     * adding it would break the binary API, which we want to avoid.) */
    friend class MultiSpinBox;
};

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSectionConfiguration &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::MultiSpinBoxSectionConfiguration)

#endif // MULTISPINBOXSECTIONCONFIGURATION_H
