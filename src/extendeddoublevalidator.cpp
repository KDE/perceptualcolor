﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "extendeddoublevalidator.h"
// Second, the private implementation.
#include "extendeddoublevalidator_p.h" // IWYU pragma: associated

#include "constpropagatinguniquepointer.h"
#include <qstringbuilder.h>
class QObject;

namespace PerceptualColor
{
/** @brief Default constructor
 *  @param parent pointer to the parent widget, if any */
ExtendedDoubleValidator::ExtendedDoubleValidator(QObject *parent)
    : QDoubleValidator(parent)
    , d_pointer(new ExtendedDoubleValidatorPrivate)
{
    // NOTE decimals(): QDoubleValidator’s default behavior changed in
    // Qt 6.3. In Qt ≤ 6.2, QDoubleValidator::decimals() returned 1000 by
    // default. From Qt 6.3 onward, the default was changed to -1, breaking
    // API stability. To guarantee consistent behaviour across all
    // Qt versions, this class explicitly sets the default decimals value
    // to -1 during initialization.
    setDecimals(-1);
}

/** @brief Destructor */
ExtendedDoubleValidator::~ExtendedDoubleValidator() noexcept
{
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString ExtendedDoubleValidator::prefix() const
{
    return d_pointer->m_prefix;
}

/** @brief Set the @ref prefix property. */
void ExtendedDoubleValidator::setPrefix(const QString &prefix)
{
    if (prefix != d_pointer->m_prefix) {
        d_pointer->m_prefix = prefix;
        Q_EMIT prefixChanged(prefix);
    }
}

/** @brief Set the @ref suffix property. */
void ExtendedDoubleValidator::setSuffix(const QString &suffix)
{
    if (suffix != d_pointer->m_suffix) {
        d_pointer->m_suffix = suffix;
        Q_EMIT suffixChanged(suffix);
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString ExtendedDoubleValidator::suffix() const
{
    return d_pointer->m_suffix;
}

QValidator::State ExtendedDoubleValidator::validate(QString &input, int &pos) const
{
    QString myInput = input;
    int myPos = pos;

    // IF (m_prefix.isEmpty && !m_prefix.isNull)
    // THEN input.startsWith(m_prefix)
    // →  will be true IF !input.isEmpty
    // →  will be false IF input.isEmpty
    // This is inconsistent. Therefore, we test is m_prefix is empty.
    // If not, we do nothing.
    // The same also applies to suffix.
    // TODO Bug report, because Qt documentation says at
    // https://doc.qt.io/qt-6/qstring.html#distinction-between-null-and-empty-strings
    // (and at its Qt-5-counterpart):
    //     “All functions except isNull() treat null strings the same
    //      as empty strings.”
    // This is apparently wrong (at least for Qt 5).
    if (!d_pointer->m_prefix.isEmpty()) {
        if (myInput.startsWith(d_pointer->m_prefix)) {
            myInput.remove(0, d_pointer->m_prefix.size());
            // In Qt6, QString::size() returns a qsizetype aka “long long int”.
            // HACK We do a simple static_cast because a so long QString isn’t
            // useful anyway.
            myPos -= static_cast<int>(d_pointer->m_prefix.size());
        } else {
            return QValidator::State::Invalid;
        }
    }
    if (!d_pointer->m_suffix.isEmpty()) {
        if (myInput.endsWith(d_pointer->m_suffix)) {
            myInput.chop(d_pointer->m_suffix.size());
        } else {
            return QValidator::State::Invalid;
        }
    }

    QValidator::State result = QDoubleValidator::validate(myInput, myPos);
    // Following the Qt documentation, QValidator::validate() is allowed
    // and indented to make changes to the arguments passed by reference
    // (“input” and “pos”). However, we use its child class QDoubleValidator.
    // The documentation of QDoubleValidator states that the “pos” argument
    // is not used. Therefore, write back only the “input” argument.
    input = d_pointer->m_prefix + myInput + d_pointer->m_suffix;

    return result;
}

} // namespace PerceptualColor
