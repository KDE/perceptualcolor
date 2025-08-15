// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "multispinbox.h"
// Second, the private implementation.
#include "multispinbox_p.h" // IWYU pragma: associated

#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "helpermath.h"
#include "multispinboxsection.h"
#include <math.h>
#include <qaccessible.h>
#include <qaccessiblewidget.h>
#include <qcoreevent.h>
#include <qdebug.h>
#include <qevent.h>
#include <qfontmetrics.h>
#include <qglobal.h>
#include <qlineedit.h>
#include <qlocale.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpointer.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qvalidator.h>
#include <qwidget.h>
class QAction;

namespace PerceptualColor
{
/** @brief If the text cursor is touching at the current section’s value.
 *
 * Everything from the cursor position exactly before the value itself up
 * to the cursor position exactly after the value itself. Prefixes and
 * suffixes are not considered as part of the value. Example: “ab12cd”
 * (prefix “ab”, value 12, suffix “cd”). The cursor positions 2, 3 and 4 are
 * considered <em>touching</em> the current value.
 *
 * @returns <tt>true</tt> if the text cursor is touching at the current
 * section’s value.. <tt>false</tt> otherwise. */
bool MultiSpinBoxPrivate::isCursorTouchingCurrentSectionValue() const
{
    const auto cursorPosition = q_pointer->lineEdit()->cursorPosition();
    const bool highEnough = //
        (cursorPosition >= m_textBeforeCurrentValue.length());
    const auto after = q_pointer->lineEdit()->text().length() //
        - m_textAfterCurrentValue.length();
    const bool lowEnough = (cursorPosition <= after);
    return (highEnough && lowEnough);
}

/** @brief The recommended minimum size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns the recommended minimum size for the widget
 *
 * @internal
 *
 * @sa @ref sizeHint()
 *
 * @note The minimum size of the widget is the same as @ref sizeHint(). This
 * behaviour is different from <tt>QSpinBox</tt> and <tt>QDoubleSpinBox</tt>
 * that have a minimum size hint that allows for displaying only prefix and
 * value, but not the suffix. However, such a behavior does not seem
 * appropriate for a @ref MultiSpinBox because it could be confusing, given
 * that its content is more complex. */
QSize MultiSpinBox::minimumSizeHint() const
{
    return sizeHint();
}

/** @brief Constructor
 *
 * @param parent the parent widget, if any */
MultiSpinBox::MultiSpinBox(QWidget *parent)
    : QAbstractSpinBox(parent)
    , d_pointer(new MultiSpinBoxPrivate(this))
{
    // Set up the m_validator
    d_pointer->m_validator = new QDoubleValidator(this);
    // QDoubleSpinBox does not accept QDoubleValidator::ScientificNotation,
    // so we don't either.
    d_pointer->m_validator->setNotation(QDoubleValidator::StandardNotation);

    // Initialize the configuration (default: only one section).
    // This will also change section values to exactly one element.
    setFormat( //
        QList<MultiSpinBoxSection>{MultiSpinBoxSection()});
    setValues(QList<double>{MultiSpinBoxPrivate::defaultSectionValue});
    d_pointer->m_currentIndex = -1; // This will force
    // setCurrentIndexAndUpdateTextAndSelectValue()
    // to really apply the changes, including updating
    // the validator:
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);

    // Connect signals and slots
    connect(lineEdit(), // sender
            &QLineEdit::textChanged, // signal
            d_pointer.get(), // receiver
            &MultiSpinBoxPrivate::updateCurrentValueFromText // slot
    );
    connect(lineEdit(), // sender
            &QLineEdit::cursorPositionChanged, // signal
            d_pointer.get(), // receiver
            &MultiSpinBoxPrivate::reactOnCursorPositionChange // slot
    );

    // Initialize accessibility support
    QAccessible::installFactory(
        // It’s safe to call installFactory() multiple times with the
        // same factory. If the factory is yet installed, it will not
        // be installed again.
        &AccessibleMultiSpinBox::factory);
}

/** @brief Default destructor */
MultiSpinBox::~MultiSpinBox() noexcept
{
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
MultiSpinBoxPrivate::MultiSpinBoxPrivate(MultiSpinBox *backLink)
    : q_pointer(backLink)
{
}

/** @brief The recommended size for the widget
 *
 * Reimplemented from base class.
 *
 * @returns the size hint
 *
 * @internal
 *
 * @note Some widget styles like CDE and Motif calculate badly (too small)
 * the  size for QAbstractSpinBox and its child classes, and therefore also
 * for this widget.
 *
 * @sa @ref minimumSizeHint() */
QSize MultiSpinBox::sizeHint() const
{
    // This function intentionally does not cache the text string.
    // Which variant is the longest text string, that depends on the current
    // font policy. This might have changed since the last call. Therefore,
    // each time this function is called, we calculate again the longest
    // test string (“completeString”).

    ensurePolished();

    const QFontMetrics myFontMetrics(fontMetrics());
    QList<MultiSpinBoxSection> myConfiguration = //
        d_pointer->m_format;
    const int height = lineEdit()->sizeHint().height();
    int width = 0;
    QString completeString;

    // Get the text for all the sections
    for (int i = 0; i < myConfiguration.size(); ++i) {
        // Prefix
        completeString += myConfiguration.at(i).prefix();
        // For each section, test if the minimum value or the maximum
        // takes more space (width). Choose the one that takes more place
        // (width).
        const QString textOfMinimumValue = d_pointer->textFromValue( //
            myConfiguration.at(i).minimum(), // value
            myConfiguration.at(i).decimals(), //
            myConfiguration.at(i).isGroupSeparatorShown(), //
            locale());
        const QString textOfMaximumValue = d_pointer->textFromValue( //
            myConfiguration.at(i).maximum(), // value
            myConfiguration.at(i).decimals(),
            myConfiguration.at(i).isGroupSeparatorShown(), //
            locale());
        const auto minValueAdvance = //
            myFontMetrics.horizontalAdvance(textOfMinimumValue);
        const auto maxValueAdvance = //
            myFontMetrics.horizontalAdvance(textOfMaximumValue);
        if (minValueAdvance > maxValueAdvance) {
            completeString += textOfMinimumValue;
        } else {
            completeString += textOfMaximumValue;
        }
        // Suffix
        completeString += myConfiguration.at(i).suffix();
    }

    // Add some extra space, just as QSpinBox seems to do also.
    completeString += QStringLiteral(u" ");

    // Calculate string width and add two extra pixel for cursor
    // blinking space.
    width = myFontMetrics.horizontalAdvance(completeString) + 2;

    // Calculate the final size in pixel
    QStyleOptionSpinBox myStyleOptionsForSpinBoxes;
    initStyleOption(&myStyleOptionsForSpinBoxes);
    myStyleOptionsForSpinBoxes.buttonSymbols = QAbstractSpinBox::PlusMinus;

    const QSize contentSize(width, height);
    // Calculate widget size necessary to display a given content
    QSize result = style()->sizeFromContents(
        // In the Kvantum style in version 0.18, there was a bug that returned
        // via QStyle::sizeFromContents() a width that was too small. In
        // Kvantum version 1.0.1 this is fixed.
        QStyle::CT_SpinBox, // type
        &myStyleOptionsForSpinBoxes, // style options
        contentSize, // size of the content
        this // optional widget argument (for better calculations)
    );

    const auto buttonCount = lineEdit()->actions().size();
    if (buttonCount > 0) {
        // Determine the size of icons for actions similar to what Qt
        // does in QLineEditPrivate::sideWidgetParameters() and than
        // add this to the size hint.
        const int actionButtonIconSize = style()->pixelMetric( //
            QStyle::PM_SmallIconSize, // pixel metric type
            nullptr, // style options
            lineEdit() // widget (optional)
        );
        const int actionButtonMargin = actionButtonIconSize / 4;
        const int actionButtonWidth = actionButtonIconSize + 6;
        // Only 1 margin per button:
        const int actionButtonSpace = actionButtonWidth + actionButtonMargin;
        result.setWidth( //
            result.width() //
            + static_cast<int>(buttonCount) * actionButtonSpace);
    }

    return result;
}

/** @brief Handle state changes
 *
 * Implements reaction on <tt>QEvent::LanguageChange</tt>.
 *
 * Reimplemented from base class.
 *
 * @param event The event to process */
void MultiSpinBox::changeEvent(QEvent *event)
{
    // QEvent::StyleChange or QEvent::FontChange are not handled here
    // because they trigger yet a content and geometry update in the
    // base class’s implementation of this function.
    if ( //
        (event->type() == QEvent::LanguageChange) //
        || (event->type() == QEvent::LocaleChange) //
        // The base class’s implementation for QEvent::LayoutDirectionChange
        // would only call update, not updateGeometry…
        || (event->type() == QEvent::LayoutDirectionChange) //
    ) {
        d_pointer->m_validator->setLocale(locale());
        d_pointer->updatePrefixValueSuffixText();
        d_pointer->updateValidator();
        lineEdit()->setText( //
            d_pointer->m_textBeforeCurrentValue //
            + d_pointer->m_textOfCurrentPendingValue //
            + d_pointer->m_textAfterCurrentValue);
        update();
        updateGeometry();
    }
    QAbstractSpinBox::changeEvent(event);
}

/**
 * @brief Formats a floating-point value into a localized string.
 *
 * @param value The floating-point number to be formatted.
 * @param decimals The number of digits to appear after the decimal point.
 * @param showGroupSeparator Determines whether group separators (e. g.,
 *        thousands separators) should be included in the output.
 *        QLocale::numberOptions().testFlag(QLocale::OmitGroupSeparator) is
 *        ignored.
 * @param locale The locale used for the formatting.
 * @return A localized string representation of the input value.
 */
QString MultiSpinBoxPrivate::textFromValue(const double value, const int decimals, const bool showGroupSeparator, const QLocale &locale)
{
    QLocale adaptedLocale = locale;
    auto options = adaptedLocale.numberOptions();
    options.setFlag(QLocale::OmitGroupSeparator, !showGroupSeparator);
    adaptedLocale.setNumberOptions(options);
    return adaptedLocale.toString(value, 'f', decimals);
}

/** @brief Adds to the widget a button associated with the given action.
 *
 * The icon of the action will be displayed as button. If the action has
 * no icon, just an empty space will be displayed.
 *
 * @image html MultiSpinBoxWithButton.png "MultiSpinBox with action button"
 *
 * It is possible to add more than one action.
 *
 * @param action This action that will be executed when clicking the button.
 * (The ownership of the action object remains unchanged.)
 * @param position The position of the button within the widget (left
 * or right)
 *
 * The action can be removed with <tt>QWidget::remove()</tt>.
 *
 * @internal
 *
 * This is the counterpart to
 * <tt>void QLineEdit::addAction(QAction *action, QLineEdit::ActionPosition
 * position)</tt>.
 */
void MultiSpinBox::addActionButton(QAction *action, QLineEdit::ActionPosition position)
{
    addAction(action);
    lineEdit()->addAction(action, position);
    // The size hints have changed, because an additional button needs
    // more space.
    updateGeometry();
}

/**
 * @brief Get formatted pending value for a given section.
 *
 * @param index The index of the section
 *
 * @returns The pending value of the given section, formatted (without prefix
 * or suffix), as text.
 *
 * @sa @ref m_pendingValues
 */
QString MultiSpinBoxPrivate::formattedPendingValue(qsizetype index) const
{
    return textFromValue( //
        m_pendingValues.at(index),
        m_format.at(index).decimals(),
        m_format.at(index).isGroupSeparatorShown(), //
        q_pointer->locale());
}

/** @brief Updates prefix, value and suffix text
 *
 * @pre <tt>0 <= @ref m_currentIndex < @ref MultiSpinBox::sectionCount()</tt>
 *
 * @post Updates @ref m_textBeforeCurrentValue,
 * @ref m_textOfCurrentPendingValue, @ref m_textAfterCurrentValue to the
 * correct values based on @ref m_currentIndex and @ref m_pendingValues.
 */
void MultiSpinBoxPrivate::updatePrefixValueSuffixText()
{
    qsizetype i;

    // Update m_currentSectionTextBeforeValue
    m_textBeforeCurrentValue = QString();
    for (i = 0; i < m_currentIndex; ++i) {
        m_textBeforeCurrentValue.append( //
            m_format.at(i).prefix());
        m_textBeforeCurrentValue.append(formattedPendingValue(i));
        m_textBeforeCurrentValue.append( //
            m_format.at(i).suffix());
    }
    m_textBeforeCurrentValue.append( //
        m_format.at(m_currentIndex).prefix());

    // Update m_currentSectionTextOfTheValue
    m_textOfCurrentPendingValue = formattedPendingValue(m_currentIndex);

    // Update m_currentSectionTextAfterValue
    m_textAfterCurrentValue = QString();
    m_textAfterCurrentValue.append( //
        m_format.at(m_currentIndex).suffix());
    for (i = m_currentIndex + 1; i < q_pointer->sectionCount(); ++i) {
        m_textAfterCurrentValue.append(m_format.at(i).prefix());

        m_textAfterCurrentValue.append(formattedPendingValue(i));
        m_textAfterCurrentValue.append(m_format.at(i).suffix());
    }

    updateValidator();
}

/** @brief Sets the current section index.
 *
 * Updates the text in the QLineEdit of this widget. If the widget has focus,
 * it also selects the value of the new current section.
 *
 * @param newIndex The index of the new current section. Must be a valid
 * index. The update will be done even if this argument is identical to
 * the @ref m_currentIndex.
 *
 * @sa @ref setCurrentIndexWithoutUpdatingText */
void MultiSpinBoxPrivate::setCurrentIndexAndUpdateTextAndSelectValue(qsizetype newIndex)
{
    QSignalBlocker myBlocker(q_pointer->lineEdit());
    setCurrentIndexWithoutUpdatingText(newIndex);
    // Update the line edit widget
    q_pointer->lineEdit()->setText(m_textBeforeCurrentValue //
                                   + m_textOfCurrentPendingValue //
                                   + m_textAfterCurrentValue);
    const int lengthOfTextBeforeCurrentValue = //
        static_cast<int>(m_textBeforeCurrentValue.length());
    const int lengthOfTextOfCurrentValue = //
        static_cast<int>(m_textOfCurrentPendingValue.length());
    if (q_pointer->hasFocus()) {
        q_pointer->lineEdit()->setSelection( //
            lengthOfTextBeforeCurrentValue, //
            lengthOfTextOfCurrentValue);
    } else {
        q_pointer->lineEdit()->setCursorPosition( //
            lengthOfTextBeforeCurrentValue + lengthOfTextOfCurrentValue);
    }
    // Make sure that the buttons for step up and step down are updated.
    q_pointer->update();
}

/** @brief Sets the current section index without updating
 * the <tt>QLineEdit</tt>.
 *
 * Does not change neither the text nor the cursor in the <tt>QLineEdit</tt>.
 *
 * @param newIndex The index of the new current section. Must be a valid index.
 *
 * @sa @ref setCurrentIndexAndUpdateTextAndSelectValue */
void MultiSpinBoxPrivate::setCurrentIndexWithoutUpdatingText(qsizetype newIndex)
{
    if (!isInRange<qsizetype>(0, newIndex, q_pointer->sectionCount() - 1)) {
        return;
    }

    if (newIndex == m_currentIndex) {
        // There is nothing to do here.
        return;
    }

    // Apply the changes
    m_currentIndex = newIndex;
    updatePrefixValueSuffixText();
    updateValidator();

    // The state (enabled/disabled) of the buttons “Step up” and “Step down”
    // has to be updated. To force this, update() is called manually here:
    q_pointer->update();
}

/** @brief Virtual function that determines whether stepping up and down is
 * legal at any given time.
 *
 * Reimplemented from base class.
 *
 * @returns whether stepping up and down is legal */
QAbstractSpinBox::StepEnabled MultiSpinBox::stepEnabled() const
{
    if (isReadOnly()) {
        return QAbstractSpinBox::StepNone;
    }

    const MultiSpinBoxSection currentSectionConfiguration = //
        d_pointer->m_format.at(d_pointer->m_currentIndex);
    const double currentSectionValue = //
        d_pointer->m_pendingValues.at(d_pointer->m_currentIndex);

    // When wrapping is enabled, step up and step down are always possible.
    if (currentSectionConfiguration.isWrapping()) {
        return QAbstractSpinBox::StepEnabled(StepUpEnabled | StepDownEnabled);
    }

    // When wrapping is not enabled, we have to compare the value with
    // maximum and minimum.
    QAbstractSpinBox::StepEnabled result;
    // Test is step up should be enabled…
    if (currentSectionValue < currentSectionConfiguration.maximum()) {
        result.setFlag(StepUpEnabled, true);
    }

    // Test is step down should be enabled…
    if (currentSectionValue > currentSectionConfiguration.minimum()) {
        result.setFlag(StepDownEnabled, true);
    }
    return result;
}

/** @brief Sets the configuration for the sections.
 *
 * The first section will be selected as current section.
 *
 * @param newFormat Defines the new sections. The new section
 * count in this widget is the section count given in this list. Each section
 * should have valid values: <tt>@ref MultiSpinBoxSection.minimum ≤
 * @ref MultiSpinBoxSection.maximum</tt>. If the @ref values are
 * not valid within the new section configurations, they will be fixed.
 *
 * @sa @ref format()
 *
 * @internal
 *
 * This is the counterpart to
 * <tt>void QDateTimeEdit::setDisplayFormat(const QString &format)</tt>.
 */
void MultiSpinBox::setFormat(const QList<PerceptualColor::MultiSpinBoxSection> &newFormat)
{
    if (newFormat.size() < 1) {
        return;
    }

    // Make sure that m_currentIndex will not run out-of-bound.
    d_pointer->m_currentIndex = qBound<qsizetype>( //
        0, //
        d_pointer->m_currentIndex, //
        newFormat.size());

    // Set new section configuration
    const auto oldSectionCount = d_pointer->m_sectionCount;
    d_pointer->m_sectionCount = newFormat.size();
    d_pointer->m_format = newFormat;
    d_pointer->updateValidator();

    // Make sure the value list has the correct length and the
    // values are updated to the new configuration:
    setValues(d_pointer->m_pendingValues);

    // As the configuration has changed, the text selection might be
    // undefined. Define it:
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
        d_pointer->m_currentIndex);

    // Make sure that the buttons for step up and step down are updated.
    update();

    // Make sure that the geometry is updated: sizeHint() and minimumSizeHint()
    // both depend on the section configuration!
    updateGeometry();

    if (d_pointer->m_sectionCount != oldSectionCount) {
        Q_EMIT sectionCountChanged(d_pointer->m_sectionCount);
    }
}

/** @brief Returns the configuration of all sections.
 *
 * @returns the configuration of all sections.
 *
 * @sa @ref setFormat()
 *
 * @internal
 *
 * This is the counterpart to
 * <tt>QString QDateTimeEdit::displayFormat() const</tt>.
 */
QList<PerceptualColor::MultiSpinBoxSection> MultiSpinBox::format() const
{
    return d_pointer->m_format;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QList<double> MultiSpinBox::values() const
{
    return d_pointer->m_values;
}

/** @brief Sets @ref m_pendingValues without updating other things.
 *
 * Other data of this widget, including the <tt>QLineEdit</tt> text,
 * stays unmodified.
 *
 * @param newValues The new section values. This list must have
 * exactly as many items as @ref MultiSpinBox::format.
 * If the new values are not within the boundaries defined in
 * the @ref MultiSpinBox::format,
 * they will be adapted before being applied.
 *
 * @post @ref m_pendingValues gets updated.
 * No signals are emitted.
 */
void MultiSpinBoxPrivate::setPendingValuesWithoutFurtherUpdating(const QList<double> &newValues)
{
    if (newValues.size() < 1) {
        return;
    }

    const auto sectionCount = q_pointer->sectionCount();

    QList<double> fixedNewValues = newValues;

    // Adapt the count of values:
    while (fixedNewValues.size() < sectionCount) {
        // Add elements if there are not enough:
        fixedNewValues.append(MultiSpinBoxPrivate::defaultSectionValue);
    }
    while (fixedNewValues.size() > sectionCount) {
        // Remove elements if there are too many:
        fixedNewValues.removeLast();
    }

    // Make sure the new section values are
    // valid (minimum <= value <= maximum):
    MultiSpinBoxSection myConfig;
    double rangeWidth;
    double temp;
    for (int i = 0; i < sectionCount; ++i) {
        myConfig = m_format.at(i);
        fixedNewValues[i] =
            // Round value _before_ applying boundaries/wrapping.
            roundToDigits(fixedNewValues.at(i), myConfig.decimals());
        if (myConfig.isWrapping()) {
            rangeWidth = myConfig.maximum() - myConfig.minimum();
            if (rangeWidth <= 0) {
                // This is a special case.
                // This happens when minimum == maximum (or
                // if minimum > maximum, which is invalid).
                fixedNewValues[i] = myConfig.minimum();
            } else {
                // floating-point modulo (fmod) operation
                temp = fmod(
                    // Dividend:
                    fixedNewValues.at(i) - myConfig.minimum(),
                    // Divisor:
                    rangeWidth);
                if (temp < 0) {
                    // Negative results shall be converted
                    // in positive results:
                    temp += rangeWidth;
                }
                temp += myConfig.minimum();
                fixedNewValues[i] = temp;
            }
        } else {
            fixedNewValues[i] = qBound(
                // If there is no wrapping, simply bound:
                myConfig.minimum(),
                fixedNewValues.at(i),
                myConfig.maximum());
        }
    }

    m_pendingValues = fixedNewValues;
}

/**
 * @brief Applies pending section values.
 *
 * Transfers @ref m_pendingValues into
 * @ref m_values, updating the @ref MultiSpinBox::values
 * property.
 *
 * If the value has changed, both corresponding signals are emitted:
 * @ref MultiSpinBox::valuesChanged() and
 * @ref MultiSpinBox::valuesChangedAsQString().
 */
void MultiSpinBoxPrivate::applyPendingValuesAndEmitSignals()
{
    if (m_values != m_pendingValues) {
        m_values = m_pendingValues;
        Q_EMIT q_pointer->valuesChangedAsQString( //
            q_pointer->lineEdit()->text());
        Q_EMIT q_pointer->valuesChanged(m_values);
    }
}

/** @brief Setter for @ref values property.
 *
 * @param newValues The new section values. This list must have
 * exactly as many items as @ref format.
 *
 * The values will be bound between
 * @ref MultiSpinBoxSection::minimum and
 * @ref MultiSpinBoxSection::maximum. Their precision will be
 * reduced to as many decimal places as given by
 * @ref MultiSpinBoxSection::decimals.
 *
 * @internal
 *
 * This is the counterpart to
 * <tt>void QDoubleSpinBox::setValue(double val)</tt>.
 */
void MultiSpinBox::setValues(const QList<double> &newValues)
{
    d_pointer->setPendingValuesWithoutFurtherUpdating(newValues);

    // Update some internals…
    d_pointer->updatePrefixValueSuffixText();

    // Update the QLineEdit
    { // Limit scope of QSignalBlocker
        const QSignalBlocker blocker(lineEdit());
        lineEdit()->setText(d_pointer->m_textBeforeCurrentValue //
                            + d_pointer->m_textOfCurrentPendingValue //
                            + d_pointer->m_textAfterCurrentValue); //
        d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
            d_pointer->m_currentIndex);
    }

    // Update internal storage and signals
    d_pointer->applyPendingValuesAndEmitSignals();

    // Make sure that the buttons for step-up and step-down are updated.
    update();
}

/** @brief Focus handling for <em>Tab</em> respectively <em>Shift+Tab</em>.
 *
 * Reimplemented from base class.
 *
 * @note If it’s about moving the focus <em>within</em> this widget, the focus
 * move is actually done. If it’s about moving the focus to <em>another</em>
 * widget, the focus move is <em>not</em> actually done.
 * The documentation of the base class is not very detailed. This
 * reimplementation does not exactly behave as the documentation of the
 * base class suggests. Especially, it handles directly the focus move
 * <em>within</em> the widget itself. This was, however, the only working
 * solution we found.
 *
 * @param next <tt>true</tt> stands for focus handling for <em>Tab</em>.
 * <tt>false</tt> stands for focus handling for <em>Shift+Tab</em>.
 *
 * @returns <tt>true</tt> if the focus has actually been moved within
 * this widget or if a move to another widget is possible. <tt>false</tt>
 * otherwise. */
bool MultiSpinBox::focusNextPrevChild(bool next)
{
    const auto currentIndex = d_pointer->m_currentIndex;
    const auto cursorPosition = lineEdit()->cursorPosition();
    const bool isBeforeCurrentValue = //
        cursorPosition < d_pointer->m_textBeforeCurrentValue.size();
    if (next == true) { // Move focus forward (Tab)
        const auto newIndex = //
            isBeforeCurrentValue //
            ? currentIndex //
            : currentIndex + 1;
        if (newIndex < sectionCount()) {
            d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(newIndex);
            d_pointer->applyPendingValuesAndEmitSignals();
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return true;
        }
    } else { // Move focus backward (Shift+Tab)
        const auto newIndex = //
            isBeforeCurrentValue || d_pointer->isCursorTouchingCurrentSectionValue() //
            ? currentIndex - 1 //
            : currentIndex;
        if (newIndex >= 0) {
            d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(newIndex);
            d_pointer->applyPendingValuesAndEmitSignals();
            // Make sure that the buttons for step up and step down
            // are updated.
            update();
            return true;
        }
    }

    // Make sure that the buttons for step up and step down are updated.
    update();

    // Return
    return QWidget::focusNextPrevChild(next);
}

/** @brief Handles a <tt>QEvent::FocusOut</tt>.
 *
 * Reimplemented from base class.
 *
 * Updates the widget (except for windows that do not
 * specify a <tt>focusPolicy()</tt>).
 *
 * @param event the <tt>QEvent::FocusOut</tt> to be handled. */
void MultiSpinBox::focusOutEvent(QFocusEvent *event)
{
    d_pointer->updatePrefixValueSuffixText();
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
        d_pointer->m_currentIndex);
    // Make sure that the buttons for step up and step down
    // are updated.
    update();
    d_pointer->applyPendingValuesAndEmitSignals();

    QAbstractSpinBox::focusOutEvent(event);
}

/** @brief Handles a <tt>QEvent::FocusIn</tt>.
 *
 * Reimplemented from base class.
 *
 * Updates the widget (except for windows that do not
 * specify a <tt>focusPolicy()</tt>).
 *
 * @param event the <tt>QEvent::FocusIn</tt> to be handled. */
void MultiSpinBox::focusInEvent(QFocusEvent *event)
{
    QAbstractSpinBox::focusInEvent(event);
    switch (event->reason()) {
    case Qt::ShortcutFocusReason:
    case Qt::TabFocusReason:
        d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(0);
        // Make sure that the buttons for step up and step down
        // are updated.
        update();
        return;
    case Qt::BacktabFocusReason:
        d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
            sectionCount() - 1);
        // Make sure that the buttons for step up and step down
        // are updated.
        update();
        return;
    case Qt::MouseFocusReason:
    case Qt::ActiveWindowFocusReason:
    case Qt::PopupFocusReason:
    case Qt::MenuBarFocusReason:
    case Qt::OtherFocusReason:
    case Qt::NoFocusReason:
    default:
        update();
        return;
    }
}

/**
 * @brief This function handles keyboard input.
 *
 * Reimplemented from base class.
 *
 * @param event The key press event
 */
void MultiSpinBox::keyPressEvent(QKeyEvent *event)
{
    const auto eventText = event->text();
    if (!eventText.isEmpty() && !lineEdit()->hasSelectedText()) {
        const auto i = d_pointer->m_currentIndex;
        const bool hasNextSection = //
            i < sectionCount() - 1;
        if (hasNextSection) {
            const auto posEndOfCurrentSectionValue = //
                lineEdit()->text().size() //
                - d_pointer->m_textAfterCurrentValue.size();
            if (lineEdit()->cursorPosition() == posEndOfCurrentSectionValue) {
                const QString nextSeparator = //
                    d_pointer->m_format.value(i).suffix() //
                    + d_pointer->m_format.value(i + 1).prefix();
                if (nextSeparator.startsWith(eventText)) {
                    // Analogous to QDateTimeEdit, treat typing the next
                    // separator as if Tab had been pressed: Move to the next
                    // section.
                    focusNextPrevChild(true);
                    return;
                }
            }
        }
    }

    const auto key = event->key();
    if ((key == Qt::Key_Enter) || (key == Qt::Key_Return)) {
        d_pointer->updatePrefixValueSuffixText();
        d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
            d_pointer->m_currentIndex);
        d_pointer->applyPendingValuesAndEmitSignals();
    }

    // The base class’ implementation will emit editingFinished(). Therefore,
    // it is called AFTER our own treatment and AFTER our own signals about
    // value changes have been emitted. So editingFinished() is the last signal
    // just like in Qt’s own subclasses of QAbstractSpinBox.
    QAbstractSpinBox::keyPressEvent(event);
}

/** @brief Increase or decrease the current section’s value.
 *
 * Reimplemented from base class.
 *
 * As of the base class’s documentation:
 *
 * > Virtual function that is called whenever the user triggers a step.
 * > For example, pressing <tt>Qt::Key_Down</tt> will trigger a call
 * > to <tt>stepBy(-1)</tt>, whereas pressing <tt>Qt::Key_PageUp</tt> will
 * > trigger a call to <tt>stepBy(10)</tt>.
 *
 * @param steps Number of steps to be taken. The step size is
 * the @ref MultiSpinBoxSection::singleStep of the current section. */
void MultiSpinBox::stepBy(int steps)
{
    const auto currentIndex = d_pointer->m_currentIndex;
    QList<double> myValues = d_pointer->m_pendingValues;
    myValues[currentIndex] += steps //
        * d_pointer->m_format.at(currentIndex).singleStep();
    // As explained in QAbstractSpinBox documentation:
    //    “Note that this function is called even if the resulting value will
    //     be outside the bounds of minimum and maximum. It’s this function’s
    //     job to handle these situations.”
    // Therefore, the result has to be bound to the actual minimum and maximum
    // values.
    setValues(myValues);
    // Update the content of the QLineEdit and select the current
    // value (as cursor text selection):
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue(currentIndex);
    update(); // Make sure the buttons for step-up and step-down are updated.
}

/** @brief Updates the value of the current section.
 *
 * This slot is meant to be connected to the
 * <tt>&QLineEdit::textChanged()</tt> signal of
 * the <tt>MultiSpinBox::lineEdit()</tt> child widget.
 * ,
 * @param lineEditText The text of the <tt>lineEdit()</tt>. The value
 * will be updated according to this parameter. Only changes in
 * the <em>current</em> section’s value are expected, no changes in
 * other sections. If this parameter cannot be interpreted, the function
 * returns without further action. If it can be interpreted, but is out of
 * range, it behaves according to <tt>QAbstractSpinBox::correctionMode</tt>.
 */
void MultiSpinBoxPrivate::updateCurrentValueFromText(const QString &lineEditText)
{
    // Get the clean test. That means, we start with “text”, but
    // we remove the m_currentSectionTextBeforeValue and the
    // m_currentSectionTextAfterValue, so that only the text of
    // the value itself remains.
    QString cleanText = lineEditText;
    if (cleanText.startsWith(m_textBeforeCurrentValue)) {
        cleanText.remove(0, m_textBeforeCurrentValue.size());
    } else {
        // The text does not start with the correct characters.
        // This is an error.
        qWarning() << "The function" << __func__ //
                   << "in file" << __FILE__ //
                   << "near to line" << __LINE__ //
                   << "was called with the invalid “lineEditText“ argument “" //
                   << lineEditText //
                   << "” that does not start with the expected character sequence “" //
                   << m_textBeforeCurrentValue << ". " //
                   << "The call is ignored. This is a bug.";
        return;
    }
    if (cleanText.endsWith(m_textAfterCurrentValue)) {
        cleanText.chop(m_textAfterCurrentValue.size());
    } else {
        // The text does not start with the correct characters.
        // This is an error.
        qWarning() << "The function" << __func__ //
                   << "in file" << __FILE__ //
                   << "near to line" << __LINE__ //
                   << "was called with the invalid “lineEditText“ argument “" //
                   << lineEditText //
                   << "” that does not end with the expected character sequence “" //
                   << m_textAfterCurrentValue << ". " //
                   << "The call is ignored. This is a bug.";
        return;
    }

    // Remove trailing and leading whitespace and replace whitespace in
    // the middle by a single whitespace:
    cleanText = cleanText.simplified();
    // Remove maybe existing group separators before further processing,
    // because group separators at bad positions do not pass validation nor
    // conversion to floating point numbers.
    cleanText.remove(q_pointer->locale().groupSeparator());
    m_validator->fixup(cleanText);

    // Update…
    bool ok;
    const auto newValue = q_pointer->locale().toDouble(cleanText, &ok);
    if (!ok) {
        return;
    }

    const auto min = m_format.value(m_currentIndex).minimum();
    const auto max = m_format.value(m_currentIndex).maximum();
    const bool doCorrectToPrevious = q_pointer->correctionMode() //
        == QAbstractSpinBox::CorrectionMode::CorrectToPreviousValue;
    const bool outOfRange = !isInRange(min, newValue, max);
    if (outOfRange && doCorrectToPrevious) {
        return;
    }

    QList<double> newPendingValues = m_pendingValues;
    newPendingValues[m_currentIndex] = newValue;
    // This also clamps value to valid range:
    setPendingValuesWithoutFurtherUpdating(newPendingValues);
    if (q_pointer->keyboardTracking()) {
        applyPendingValuesAndEmitSignals();
    }
    // Make sure that the buttons for step up and step down are updated.
    q_pointer->update();
    // The lineEdit()->text() property is intentionally not updated because
    // this function is meant to receive signals of the very same lineEdit().
}

/** @brief The main event handler.
 *
 * Reimplemented from base class.
 *
 * @param event the event to be handled.
 *
 * @returns The base class’s return value.
 *
 * @internal
 *
 * @note For future extensions.
 */
bool MultiSpinBox::event(QEvent *event)
{
    return QAbstractSpinBox::event(event);
}

/** @brief Updates the widget according to the new cursor position.
 *
 * This slot is meant to be connected to the
 * <tt>QLineEdit::cursorPositionChanged()</tt> signal of
 * the <tt>MultiSpinBox::lineEdit()</tt> child widget.
 *
 * @param oldPos the old cursor position (previous position)
 * @param newPos the new cursor position (current position) */
void MultiSpinBoxPrivate::reactOnCursorPositionChange(const int oldPos, const int newPos)
{
    Q_UNUSED(oldPos)

    // We are working here with QString::length() and
    // QLineEdit::cursorPosition(). Both are of type “int”, and both are
    // measured in UTF-16 code units.  While it feels quite uncomfortable
    // to measure cursor positions in code _units_ and not at least in
    // in code _points_, it does not matter for this code, as the behaviour
    // is consistent between both usages.

    if (isCursorTouchingCurrentSectionValue()) {
        // We are within the value text of our current section value.
        // There is nothing to do here.
        return;
    }

    QSignalBlocker myBlocker(q_pointer->lineEdit());

    // The new position is not at the current value, but the old one might
    // have been. So maybe we have to correct the value, which might change
    // its length. If the new cursor position is after this value, it will
    // have to be adapted (if the value had been changed or alternated).
    const auto oldTextLength = q_pointer->lineEdit()->text().length();
    const bool mustAdjustCursorPosition = //
        (newPos > (oldTextLength - m_textAfterCurrentValue.length()));

    // Calculate in which section the cursor is
    int sectionOfTheNewCursorPosition;
    qsizetype reference = 0;
    for (sectionOfTheNewCursorPosition = 0; //
         sectionOfTheNewCursorPosition < q_pointer->sectionCount() - 1; //
         ++sectionOfTheNewCursorPosition //
    ) {
        reference += m_format //
                         .at(sectionOfTheNewCursorPosition) //
                         .prefix() //
                         .length();
        reference += //
            formattedPendingValue(sectionOfTheNewCursorPosition).length();
        reference += m_format //
                         .at(sectionOfTheNewCursorPosition) //
                         .suffix() //
                         .length();
        if (newPos <= reference) {
            break;
        }
    }

    updatePrefixValueSuffixText();
    setCurrentIndexWithoutUpdatingText(sectionOfTheNewCursorPosition);
    q_pointer->lineEdit()->setText(m_textBeforeCurrentValue //
                                   + m_textOfCurrentPendingValue //
                                   + m_textAfterCurrentValue);
    int correctedCursorPosition = newPos;
    if (mustAdjustCursorPosition) {
        correctedCursorPosition = //
            static_cast<int>(newPos //
                             + q_pointer->lineEdit()->text().length() //
                             - oldTextLength);
    }
    q_pointer->lineEdit()->setCursorPosition(correctedCursorPosition);

    // Make sure that the buttons for step up and step down are updated.
    q_pointer->update();
}

/** @brief Constructor
 *
 * @param w The widget to which the newly created object will correspond. */
AccessibleMultiSpinBox::AccessibleMultiSpinBox(MultiSpinBox *w)
    : QAccessibleWidget(w, QAccessible::Role::SpinBox)
{
}

/** @brief Destructor */
AccessibleMultiSpinBox::~AccessibleMultiSpinBox()
{
}

/** @brief Factory function.
 *
 * This signature of this function is exactly as defined by
 * <tt>QAccessible::InterfaceFactory</tt>. A pointer to this function
 * can therefore be passed to <tt>QAccessible::installFactory()</tt>.
 *
 * @param classname The class name for which an interface is requested
 * @param object The object for which an interface is requested
 *
 * @returns If this class corresponds to the request, it returns an object
 * of this class. Otherwise, a null-pointer will be returned. */
QAccessibleInterface *AccessibleMultiSpinBox::factory(const QString &classname, QObject *object)
{
    QAccessibleInterface *interface = nullptr;
    const QString multiSpinBoxClassName = QString::fromUtf8(
        // className() returns const char *. Its encoding is not documented.
        // Hopefully, as we use UTF8  in this library as “input character set”
        // and also as “Narrow execution character set”, the encoding
        // might be also UTF8…
        MultiSpinBox::staticMetaObject.className());
    MultiSpinBox *myMultiSpinBox = qobject_cast<MultiSpinBox *>(object);
    if ((classname == multiSpinBoxClassName) && myMultiSpinBox) {
        interface = new AccessibleMultiSpinBox(myMultiSpinBox);
    }
    return interface;
}

/**
 * @brief Clears the value of the current section.
 *
 * The other sections and also the prefix and suffix of the current
 * section stay visible.
 *
 * Reimplemented from base class.
 */
void MultiSpinBox::clear()
{
    // Update the content of the QLineEdit and select the current
    // value (as cursor text selection):
    d_pointer->setCurrentIndexAndUpdateTextAndSelectValue( //
        d_pointer->m_currentIndex);
    // Substitute currently selected text with an empty string.
    lineEdit()->insert(QStringLiteral(""));
    update(); // Make sure the buttons for step-up and step-down are updated.
}

/**
 * @brief Updates @ref m_validator according the the current state.
 */
void MultiSpinBoxPrivate::updateValidator()
{
    // WARNING
    //
    // setRange(): QDoubleValidator’s default behavior changed in Qt 6.3.
    // Prior to Qt 6.3, QDoubleValidator::setRange() was defined as:
    //
    //     void setRange(double minimum, double maximum, int decimals = 0);
    //
    // Starting with Qt 6.3, it is overloaded as follows:
    //
    //     void setRange(double minimum, double maximum, int decimals);
    //     void setRange(double minimum, double maximum);
    //
    // The two-argument overload (introduced in Qt 6.3) preserves the existing
    // number of decimal digits, as per the documentation: “Sets the validator
    // to accept doubles from minimum to maximum inclusive without changing the
    // number of digits after the decimal point.”
    //
    // This constitutes a breaking change: calling setRange(min, max)
    // in Qt ≤ 6.2 implicitly sets decimals to 0, whereas in Qt ≥ 6.3, it
    // retains the current decimal setting. To ensure consistent and
    // predictable behavior across Qt versions, always invoke setRange()
    // with all three arguments explicitly.
    m_validator->setRange( //
        m_format.at(m_currentIndex).minimum(),
        m_format.at(m_currentIndex).maximum(),
        m_format.at(m_currentIndex).decimals());
}

/**
 * @brief Intentionally empty override.
 *
 * @param input The complete user-visible string from the line edit, passed by
 *              reference for potential in-place modification.
 *
 * @internal
 *
 * <tt>QAbstractSpinBox::fixup()</tt> is designed to correct the full input
 * text. Subclasses like <tt>QDateTimeEdit</tt> apply fixup to all sections
 * simultaneously using internal Qt APIs. However, <tt>QAbstractSpinBox</tt>
 * itself currently does not invoke fixup in non-Qt subclasses.
 *
 * For @ref MultiSpinBox, this mechanism is unsuitable: its parsing logic
 * interprets segmented input using arbitrary separators—including empty
 * prefixes and suffixes—which makes whole-string correction ambiguous. Parsing
 * is handled internally via @ref MultiSpinBoxPrivate and cannot replicate the
 * behavior of QDateTimeEdit, which applies fixup uniformly across all
 * sections. As such, providing a non-empty implementation here could lead to
 * behavior that diverges surprisingly from QDateTimeEdit. An empty override
 * is therefore preferred.
 *
 * This override also prevents any inherited behavior from QAbstractSpinBox,
 * ensuring that future changes to its internal logic do not inadvertently
 * affect @ref MultiSpinBox.
 *
 * @sa @ref validate()
 */
void MultiSpinBox::fixup(QString &input) const
{
    Q_UNUSED(input);
}

/**
 * @brief Validates the user input for the currently active section.
 *
 * Reimplemented from base class.
 *
 * @param input The input to validate (may be modified/fixed during processing)
 * @param pos The current text cursor position (may be modified/fixed during
 *        processing)
 * @returns A validation state indicating whether the input is acceptable
 *          with regard to the current section.
 *
 * @note This validator allows changes only to the <em>current</em>
 * section. It is <em>not</em> possible to change various values at the
 * same time, for example by marking all the current text and use
 * Ctrl-V to past a complete new value from the clipboard. This would
 * be impossible to parse reliably, because the prefixes and suffixes
 * of each section might contain (localized) digits that would be
 * difficult to differentiate from the actual value.
 *
 * @sa @ref fixup()
 *
 * @internal
 *
 * If the section configuration or the current section index change,
 * @ref MultiSpinBoxPrivate::updateValidator() must be called in order to
 * have this function work correctly.
 */
QValidator::State MultiSpinBox::validate(QString &input, int &pos) const
{
    QString myInput = input;
    int myPos = pos;

    // If a decimal separator is typed while the cursor is positioned before an
    // existing one, do not insert a second separator. Instead, move the cursor
    // to the position immediately after the existing decimal separator.
    const QString decimalSeparator = locale().decimalPoint();
    const QString doubleDecimalPoint = decimalSeparator + decimalSeparator;
    if (pos > 0 && !decimalSeparator.isEmpty()) {
        if (myInput.mid(pos - 1).startsWith(doubleDecimalPoint)) { // clazy:exclude=qstring-ref
            myInput.remove(pos, decimalSeparator.length());
        }
    }

    // IF (m_prefix.isEmpty && !m_prefix.isNull)
    // THEN input.startsWith(m_prefix)
    // →  will be true IF !input.isEmpty
    // →  will be false IF input.isEmpty
    // This is inconsistent. Therefore, we test if m_prefix is empty.
    // If not, we do nothing.
    // The same also applies to suffix.
    // That means, Qt documentation is wrong, because
    // https://doc.qt.io/qt-6/qstring.html#distinction-between-null-and-empty-strings
    // (and at its Qt-5-counterpart) says:
    //     “All functions except isNull() treat null strings the same
    //      as empty strings.”
    // This is apparently wrong (at least for Qt 5).
    if (!d_pointer->m_textBeforeCurrentValue.isEmpty()) {
        if (myInput.startsWith(d_pointer->m_textBeforeCurrentValue)) {
            myInput.remove(0, d_pointer->m_textBeforeCurrentValue.size());
            // In Qt6, QString::size() returns a qsizetype aka “long long int”.
            // HACK We do a simple static_cast because a so long QString isn’t
            // useful anyway.
            myPos -= static_cast<int>(d_pointer->m_textBeforeCurrentValue.size());
        } else {
            return QValidator::State::Invalid;
        }
    }
    if (!d_pointer->m_textAfterCurrentValue.isEmpty()) {
        if (myInput.endsWith(d_pointer->m_textAfterCurrentValue)) {
            myInput.chop(d_pointer->m_textAfterCurrentValue.size());
        } else {
            return QValidator::State::Invalid;
        }
    }

    // If decimals() == 0 then we want integer-like behaviour: decimal
    // separators are not allowed.
    if (d_pointer->m_format.value(d_pointer->m_currentIndex).decimals() == 0) {
        if (!decimalSeparator.isEmpty() && myInput.contains(decimalSeparator)) {
            return QValidator::State::Invalid;
        }
    }

    QValidator::State result = //
        d_pointer->m_validator->validate(myInput, myPos);
    // Following the Qt documentation, QValidator::validate() is allowed
    // and indented to make changes to the arguments passed by reference
    // (“input” and “pos”). However, we use its child class QDoubleValidator.
    // The documentation of QDoubleValidator states that the “pos” argument
    // is not used. Therefore, write back only the “input” argument.
    input = d_pointer->m_textBeforeCurrentValue //
        + myInput //
        + d_pointer->m_textAfterCurrentValue;

    return result;
}

/**
 * @brief Event handler for action events.
 *
 * Reimplemented from base class.
 *
 * Makes sure that actions added by @ref addActionButton are handled as
 * expected.
 *
 * @param event The action event
 */
void MultiSpinBox::actionEvent(QActionEvent *event)
{
    if (event->type() == QEvent::ActionRemoved) {
        lineEdit()->removeAction(event->action());
    }
    QAbstractSpinBox::actionEvent(event);
}

qsizetype MultiSpinBox::sectionCount() const
{
    return d_pointer->m_sectionCount;
}

} // namespace PerceptualColor
