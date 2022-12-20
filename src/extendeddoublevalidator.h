// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef EXTENDEDDOUBLEVALIDATOR_H
#define EXTENDEDDOUBLEVALIDATOR_H

#include "constpropagatinguniquepointer.h"
#include <qglobal.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qvalidator.h>
class QObject;

namespace PerceptualColor
{
class ExtendedDoubleValidatorPrivate;

/** @internal
 *
 *  @brief The @ref ExtendedDoubleValidator class provides range checking
 * of floating-point numbers with support for prefix and/or suffix.
 *
 * This class behaves exactly like its base class <tt>QDoubleValidator</tt>
 * with the difference that is allows to specify prefixes and/or suffixed
 * that are considered by @ref validate(). */
class ExtendedDoubleValidator : public QDoubleValidator
{
    Q_OBJECT

    /** @brief The prefix of the number.
     *
     * @sa @ref prefix()
     * @sa @ref setPrefix()
     * @sa @ref prefixChanged()
     * @sa @ref ExtendedDoubleValidatorPrivate::m_prefix */
    Q_PROPERTY(QString prefix READ prefix WRITE setPrefix NOTIFY prefixChanged)

    /** @brief The suffix of the number.
     *
     * @sa @ref suffix()
     * @sa @ref setSuffix()
     * @sa @ref suffixChanged()
     * @sa @ref ExtendedDoubleValidatorPrivate::m_suffix */
    Q_PROPERTY(QString suffix READ suffix WRITE setSuffix NOTIFY suffixChanged)

public:
    Q_INVOKABLE ExtendedDoubleValidator(QObject *parent = nullptr);
    virtual ~ExtendedDoubleValidator() noexcept override;
    /** @brief Getter for property @ref prefix
     *  @returns the property @ref prefix */
    [[nodiscard]] QString prefix() const;
    /** @brief Getter for property @ref suffix
     *  @returns the property @ref suffix */
    [[nodiscard]] QString suffix() const;
    /** @brief Validate input.
     *
     * @param input the input string
     * @param pos the cursor position
     * @returns
     * - An input that does not contain the @ref prefix() and @ref suffix()
     *   (if these are not empty) is always invalid.
     * - If an input contains correct prefix and suffix, the floating point
     *   portion is validated as <tt>QDoubleValidator</tt> does and
     *   the corresponding result is returned. */
    [[nodiscard]] virtual QValidator::State validate(QString &input, int &pos) const override;

public Q_SLOTS:
    void setPrefix(const QString &prefix);
    void setSuffix(const QString &suffix);

Q_SIGNALS:
    /** @brief Notify signal for property @ref prefix.
     *
     * @param prefix the new prefix */
    void prefixChanged(const QString &prefix);
    /** @brief Notify signal for property @ref suffix.
     *
     * @param suffix the new suffix */
    void suffixChanged(const QString &suffix);

private:
    Q_DISABLE_COPY(ExtendedDoubleValidator)

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class ExtendedDoubleValidatorPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<ExtendedDoubleValidatorPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestExtendedDoubleValidator;
};

} // namespace PerceptualColor

#endif // EXTENDEDDOUBLEVALIDATOR_H
