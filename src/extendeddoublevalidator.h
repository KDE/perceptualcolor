// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef EXTENDEDDOUBLEVALIDATOR_H
#define EXTENDEDDOUBLEVALIDATOR_H

#include "constpropagatinguniquepointer.h"
#include <qglobal.h>
#include <qstring.h>
#include <qvalidator.h>
class QObject;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

namespace PerceptualColor
{
class ExtendedDoubleValidatorPrivate;

/** @internal
 *
 *  @brief The @ref ExtendedDoubleValidator class provides range checking
 * of floating-point numbers with support for prefix and/or suffix.
 *
 * This class behaves mostly like its base class <tt>QDoubleValidator</tt>
 * with the difference that is allows to specify prefixes and/or suffixed
 * that are considered by @ref validate().
 *
 * @note decimals(): QDoubleValidator’s default behavior changed in
 * Qt 6.3. In Qt ≤ 6.2, QDoubleValidator::decimals() returned 1000 by
 * default. From Qt 6.3 onward, the default was changed to -1, breaking
 * API stability. To guarantee consistent behaviour across all
 * Qt versions, this class explicitly sets the default decimals value
 * to -1 during initialization.
 *
 * @note setRange(): QDoubleValidator’s default behavior changed in Qt 6.3.
 * Prior to Qt 6.3, QDoubleValidator::setRange() was defined as:<br/>
 * <tt>void setRange(double minimum, double maximum, int decimals = 0);</tt>
 * <br/>Starting with Qt 6.3, it is overloaded as follows:<br/>
 * <tt>void setRange(double minimum, double maximum, int decimals);<br/>
 * void setRange(double minimum, double maximum);</tt><br/>
 * The two-argument overload (introduced in Qt 6.3) preserves the existing
 * number of decimal digits, as per the documentation: <em>“Sets the validator
 * to accept doubles from minimum to maximum inclusive without changing the
 * number of digits after the decimal point.”</em>
 * This constitutes a breaking change: calling <tt>setRange(min, max)</tt>
 * in Qt ≤ 6.2 implicitly sets decimals to 0, whereas in Qt ≥ 6.3, it
 * retains the current decimal setting. To ensure consistent and
 * predictable behavior across Qt versions, always invoke setRange()
 * with all three arguments explicitly.
 */
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
    Q_INVOKABLE explicit ExtendedDoubleValidator(QObject *parent = nullptr);
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
