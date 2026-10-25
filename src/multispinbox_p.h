// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_MULTISPINBOX_P_H
#define PERCEPTUALCOLOR_MULTISPINBOX_P_H

// Include the header of the public class of this private implementation.
// #include "multispinbox.h"

#include "constpropagatingrawpointer.h"
#include "multispinboxsection.h"
#include <qaccessiblewidget.h>
#include <qglobal.h>
#include <qlist.h>
#include <qlocale.h>
#include <qobject.h>
#include <qpointer.h>
#include <qstring.h>
#include <qtmetamacros.h>
class QDoubleValidator;

namespace PerceptualColor
{
class MultiSpinBox;

/*!
 * \inmodule ModulePrivateApi
 *
 *  \brief Private implementation within the \e {Pointer to
 *  implementation} idiom. */
class MultiSpinBoxPrivate : public QObject
{
    Q_OBJECT

public:
    explicit MultiSpinBoxPrivate(MultiSpinBox *backLink);
    /*!
     * \brief Default destructor
     */
    virtual ~MultiSpinBoxPrivate() noexcept override = default;

    friend class TestMultiSpinBox;

    // constexpr
    /*!
     * \brief Default value of a section */
    static constexpr double defaultSectionValue = 0;

    /*!
     * \brief Holds the index of the currently selected section.
     *
     * \sa setCurrentIndexAndUpdateTextAndSelectValue
     * \sa setCurrentIndexWithoutUpdatingText
     */
    qsizetype m_currentIndex = 0;
    /*!
     * \brief Holds the data for the sections.
     *
     * This list is guaranteed to contain at least \e {one} section.
     *
     * \sa MultiSpinBox::format()
     * \sa MultiSpinBox::setFormat()
     */
    QList<MultiSpinBoxSection> m_format;
    /*!
     * \brief Section values pending to be applied to \l m_values.
     *
     * If QAbstractSpinBox::keyboardTracking is disabled, changes to
     * \l m_values caused by keyboard input are deferred. This
     * variable helps to keep track.
     */
    QList<double> m_pendingValues = QList<double>{MultiSpinBoxPrivate::defaultSectionValue};
    /*!
     * \brief Internal storage for property \l MultiSpinBox::sectionCount.
     */
    qsizetype m_sectionCount = 1;
    /*!
     * \brief The string of everything \e {after} the value of the
     * current section.
     *
     * This includes the suffix of the current section and everything
     * (prefixes, values and suffixes) of all sections that come after
     * the current sections. */
    QString m_textAfterCurrentValue;
    /*!
     * \brief The string of everything \e {before} the value of the
     * current section.
     *
     * This includes everything (prefixes, values and suffixes) of all
     * sections that come before the current section, and the prefix
     * of the current section. */
    QString m_textBeforeCurrentValue;
    /*!
     * \brief The string of the pending value of the current section. */
    QString m_textOfCurrentPendingValue;
    /*!
     * \brief Basic validator functionality.
     */
    QPointer<QDoubleValidator> m_validator;
    /*!
     * \brief Internal storage for property \l MultiSpinBox::values.
     *
     * \sa m_pendingValues
     */
    QList<double> m_values = QList<double>{MultiSpinBoxPrivate::defaultSectionValue};

    // Functions
    void applyPendingValuesAndEmitSignals();
    [[nodiscard]] int cursorSection(const int cursorPosition) const;
    [[nodiscard]] QString formattedPendingValue(qsizetype index) const;
    [[nodiscard]] bool isCursorTouchingCurrentSectionValue() const;
    void setCurrentIndexAndUpdateTextAndSelectValue(qsizetype newIndex);
    void setCurrentIndexWithoutUpdatingText(qsizetype newIndex);
    void setPendingValuesWithoutFurtherUpdating(const QList<double> &newValues);
    [[nodiscard]] static QString textFromValue(const double value, const int decimals, const bool showGroupSeparator, const QLocale &locale);
    void updatePrefixValueSuffixText();
    void updateValidator();

public Q_SLOTS:
    void reactOnCursorPositionChange(const int oldPos, const int newPos);
    void updateCurrentValueFromText(const QString &lineEditText);

private:
    Q_DISABLE_COPY(MultiSpinBoxPrivate)

    /*!
     * \brief Pointer to the object from which \e {this} object
     *  is the private implementation. */
    ConstPropagatingRawPointer<MultiSpinBox> q_pointer;
};

/*!
 * \inmodule ModulePrivateApi
 *
 * \brief Interface for accessible objects. */
class AccessibleMultiSpinBox : public QAccessibleWidget
{
public:
    explicit AccessibleMultiSpinBox(MultiSpinBox *w);
    virtual ~AccessibleMultiSpinBox() override;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_MULTISPINBOX_P_H
