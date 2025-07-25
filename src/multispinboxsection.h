﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef MULTISPINBOXSECTION_H
#define MULTISPINBOXSECTION_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qdebug.h>
#include <qmetatype.h>
#include <qstring.h>

namespace PerceptualColor
{
class MultiSpinBoxSectionPrivate;

/** @brief Configuration of a single section
 * within a @ref MultiSpinBox.
 *
 * This class encapsulates the configuration settings for an individual
 * section of a @ref MultiSpinBox widget.
 *
 * This data type can be passed to QDebug thanks to
 * @ref operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSection &value)
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object.
 *
 * @internal
 *
 * The design of this configuration class is similar to Qt’s
 * QNetworkConfiguration: an fully copyable object that can be
 * passed to other classes, which then interpret its settings.
 * Internally, the class uses the @ref pimpl idiom to hide implementation
 * details and allow for future binary-compatible extensions.
 */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBoxSection
{
public:
    MultiSpinBoxSection();
    MultiSpinBoxSection(const MultiSpinBoxSection &other);
    ~MultiSpinBoxSection() noexcept;
    MultiSpinBoxSection &operator=(const MultiSpinBoxSection &other);
    MultiSpinBoxSection(MultiSpinBoxSection &&other) noexcept;
    MultiSpinBoxSection &operator=(MultiSpinBoxSection &&other) noexcept;

    [[nodiscard]] int decimals() const;
    [[nodiscard]] QString formatString() const;
    [[nodiscard]] bool isGroupSeparatorShown() const;
    [[nodiscard]] bool isWrapping() const;
    [[nodiscard]] double maximum() const;
    [[nodiscard]] double minimum() const;
    [[nodiscard]] QString prefix() const;
    void setDecimals(int newDecimals);
    void setFormatString(const QString &formatString);
    void setGroupSeparatorShown(bool shown);
    void setMaximum(double newMaximum);
    void setMinimum(double newMinimum);
    void setSingleStep(double newSingleStep);
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
    friend class MultiSpinBoxSectionPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<MultiSpinBoxSectionPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestMultiSpinBoxSection;

    /** @internal
     * @brief Internal friend declaration.
     *
     * @note This class is used as configuration for @ref MultiSpinBox. Thought
     * currently there is no need for this <tt>friend</tt> declaration,
     * it is done nevertheless. (If it would become necessary later,
     * adding it would break the binary API, which we want to avoid.) */
    friend class MultiSpinBox;
};

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSection &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::MultiSpinBoxSection)

#endif // MULTISPINBOXSECTION_H
