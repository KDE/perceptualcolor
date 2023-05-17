// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef MULTISPINBOX_H
#define MULTISPINBOX_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qabstractspinbox.h>
#include <qglobal.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qsize.h>
class QAction;
class QEvent;
class QFocusEvent;
class QWidget;

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
// Including multispinboxsection.h is necessary on Qt6,
// otherwise moc will fail. (IWYU does not detect this dependency.)
#include "multispinboxsection.h" // IWYU pragma: keep
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
namespace PerceptualColor
{
class MultiSpinBoxSection;
}
class QObject;
#endif

namespace PerceptualColor
{
class MultiSpinBoxPrivate;

/** @brief A spin box that can hold multiple sections (each with its own
 * value) at the same time.
 *
 * This widget is similar to <tt>QDateTimeEdit</tt> which also provides
 * multiple sections (day, month, year…) within a single spin box.
 * However, <em>this</em> widget is flexible. You can define on your own
 * the behaviour of each section.
 *
 * @image html MultiSpinBox.png "MultiSpinBox" width=200
 *
 * This widget works with floating point precision. You can set
 * the number of decimal places for each section individually,
 * via @ref MultiSpinBoxSection::decimals. (This
 * value can also be <tt>0</tt> to get integer-like behaviour.)
 *
 * Example code to create a @ref MultiSpinBox for a HSV color value
 * (Hue 0°–360°, Saturation 0–255, Value 0–255) comes here:
 * @snippet testmultispinbox.cpp MultiSpinBox Basic example
 *
 * You can also have additional buttons within the spin box via the
 * @ref addActionButton() function.
 *
 * @note This class inherits from <tt>QAbstractSpinBox</tt>, but some
 * parts of the parent class’s API are not supported in <em>this</em>
 * class. Do not use them:
 * - <tt>selectAll()</tt> does not work as expected.
 * - <tt>wrapping()</tt> is ignored. Instead, you can configures
 *   the <em>wrapping</em> individually for each section via
 *   @ref MultiSpinBoxSection::isWrapping.
 * - <tt>specialValue()</tt> is not supported.
 *   <!-- Just as in QDateTimeEdit! -->
 * - <tt>hasAcceptableInput()</tt> is not guaranteed to obey to a particular
 *   and stable semantic.
 * - <tt>fixup()</tt>, <tt>interpretText()</tt>, <tt>validate()</tt> are
 *   not used nor do they anything.
 * - <tt>keyboardTracking()</tt> is ignored. See the signal
 *   @ref sectionValuesChanged for details.
 * - <tt>correctionMode()</tt> is ignored.
 * - <tt>isGroupSeparatorShown</tt> is ignored.
 *
 * @internal
 *
 * Further remarks on inherited API of <tt>QAbstractSpinBox</tt>:
 * - <tt>selectAll()</tt>:
 *   This slot has a default behaviour that relies on internal
 *   <tt>QAbstractSpinBox</tt> private implementations, which we cannot use
 *   because they are not part of the public API and can therefore change
 *   at any moment. As it isn’t virtual, we cannot reimplement it either.
 * - <tt>fixup(), interpretText(), validate()</tt>:
 *   As long as we do not  interact with the private API of
 *   <tt>QAbstractSpinBox</tt> (which we  cannot do because
 *   there is no stability guaranteed), those functions  are never
 *   called by <tt>QAbstractSpinBox</tt> nor does their default
 *   implementation do anything. (They seem rather like an implementation
 *   detail of Qt that was leaked to the public API.) We don’t use them
 *   either.
 * - <tt>isGroupSeparatorShown</tt>:
 *   Implementing this seems complicate. In the base class, the setter
 *   is  not virtual, and this property does not have a notify signal
 *   either.  But we would have to react on a changes in this property:
 *   The content of the <tt>QLineEdit</tt> has to be updated. And the
 *   @ref minimumSizeHint and the @ref sizeHint will change, therefore
 *   <tt>updateGeometry</tt> has to be called. It seems better not to
 *   implement this. Alternatively, it could be implemented with a
 *   per-section approach via  @ref MultiSpinBoxSection.
 *
 * @note The interface of this class could theoretically
 * be similar to other Qt classes that offer similar concepts of various
 * data within a list: QComboBox, QHeaderView, QDateTimeEdit, QList – of
 * course with consistent naming. But usually you will not modify a single
 * section configuration, but the hole set of configurations. Therefore we do
 * the configuration by @ref MultiSpinBoxSection objects, similar
 * to <tt>QNetworkConfiguration</tt> objects. Allowing changes to individual
 * sections would require a lot of additional code to make sure that after
 * such a change, the text cursor is set the the appropriate position and
 * the text selection is also appropriate. This might be problematic,
 * and gives also little benefit.
 * However, a full-featured interface could look like that:
 * @snippet testmultispinbox.cpp MultiSpinBox Full-featured interface
 *
 * @todo i18n bug: Use a MultiSpinBox with a locale that uses “,” as decimal
 * separator, and with a value with some decimals. Try to type “0,1”. It will
 * not be accepted. However, “0.1” will be accepted (and, when moving on,
 * corrected to “0,1”). This is not the expected behaviour.
 *
 * @todo i18n bug: Enter HLC values like “<tt>80.</tt>” or “<tt>80,</tt>”
 * or “<tt>80e</tt>”. Depending on the locale, it is possible to
 * actually enter these characters, but apparently on validation it
 * is not accepted and the value is replaced by <tt>0</tt>.
 * MultiSpinBox should never become 0 because the validator
 * allows something that the converter cannot convert!
 *
 * @todo In @ref ColorDialog go to the HLC @ref MultiSpinBox and place
 * the text cursor behind the degree sign, than press the ⌫ (backspace) key.
 * Actual behaviour: An error message is printed on the console: “The function
 * updateCurrentValueFromText in file […]multispinbox.cpp near […] was called
 * with the invalid “lineEditText“ argument […].  The call is ignored.
 * This is a bug.” Expected behaviour: No error message is printed.
 *
 * @todo <tt>Ctrl-A</tt> support for this class. (Does this shortcut
 * trigger <tt>selectAll()</tt>?) <tt>Ctrl-U</tt> support for this class?
 * If so, do it via @ref clear(). And: If the user tries to delete
 * everything, delete instead only the current value!? (By the way:
 * How does QDateTimeEdit handle this?)
 *
 * @todo Bug: In @ref ColorDialog, choose a tab with one of the diagrams.
 * Then, switch back the the “numeric“ tab. Expected behaviour: When
 * a @ref MultiSpinBox gets back the focus, always the first section should
 * be <em>highlighted/selected</em>, independent from what was selected or
 * the cursor position before the @ref MultiSpinBox lost the focus.
 * (While <tt>QSpinBox</tt> and <tt>QDoubleSpinBox</tt> don’t do that
 * either, <tt>QDateTimeEdit</tt> indeed <em>does</em>, and that seems
 * appropriate also for @ref MultiSpinBox.
 *
 * @todo Now, @ref setSectionValues does not select automatically the first
 * section anymore. Is this in conformance with <tt>QDateTimeEdit</tt>?
 * Test: Change the value in the middle. Push “Apply” button. Now, the
 * curser is at the end of the spin box, but the active section is still
 * the one in the middle (you can try this by using your mouse wheel on
 * the widget).
 *
 * @todo Currently, if the widget has <em>not</em> the focus but the
 * mouse moves over it and the scroll wheel is used, it’s the first
 * section that will be changed, and not the one where the mouse is,
 * as the user might expect. Even QDateTimeEdit does the same thing
 * (thus they do not change the first section, but the last one that
 * was editing before). But it would be great if we could do better here.
 * But: Is this realistic and will the required code work on all
 * platforms?
 *
 * @todo When adding Bengali digits (for example by copy and paste) to a
 * @ref MultiSpinBox that was localized to en_US, than sometimes this is
 * accepted (thought later “corrected” to 0), and sometimes not. This
 * behaviour is inconsistent and wrong.
 *
 * @todo Apparently, the validator doesn’t restrict the input actually to the
 * given range. For QDoubleSpinBox however, the line edit <em>is</em>
 * restricted! Example: even if 100 is maximum, it is possible to write 444.
 * Maybe our @ref ExtendedDoubleValidator should not rely on Qt’s validator,
 * but on if QLocale is able to convert (result: valid) or not (result:
 * invalid)?!.
 *
 * @todo If exposing this class as public API of this library, would
 * it make sense to implement the complete public API of QAbstractSpinBox
 * from which we inherit? Currently, some parts of the QAbstractSpinBox API
 * are nor (properly) implemented by this class… */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBox : public QAbstractSpinBox
{
    Q_OBJECT

    /** @brief A list containing the values of all sections.
     *
     * @note It is not this property, but @ref sectionConfigurations
     * which determines the actually available count of sections in this
     * widget. If you want to change the number of available sections,
     * call <em>first</em> @ref setSectionConfigurations and only
     * <em>after</em> that adapt this property.
     *
     * @invariant This property contains always as many elements as
     * @ref sectionConfigurations contains.
     *
     * @sa READ @ref sectionValues() const
     * @sa WRITE @ref setSectionValues()
     * @sa NOTIFY @ref sectionValuesChanged() */
    Q_PROPERTY(QList<double> sectionValues READ sectionValues WRITE setSectionValues NOTIFY sectionValuesChanged USER true)

public:
    Q_INVOKABLE explicit MultiSpinBox(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~MultiSpinBox() noexcept override;
    void addActionButton(QAction *action, QLineEdit::ActionPosition position);
    virtual void clear() override;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] Q_INVOKABLE QList<PerceptualColor::MultiSpinBoxSection> sectionConfigurations() const;
    /** @brief Getter for property @ref sectionValues
     *  @returns the property @ref sectionValues */
    [[nodiscard]] QList<double> sectionValues() const;
    Q_INVOKABLE void setSectionConfigurations(const QList<PerceptualColor::MultiSpinBoxSection> &newSectionConfigurations);
    [[nodiscard]] virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;

public Q_SLOTS:
    void setSectionValues(const QList<double> &newSectionValues);

Q_SIGNALS:
    /** @brief Notify signal for property @ref sectionValues.
     *
     * This signal is emitted whenever the value in one or more sections
     * changes.
     *
     * @param newSectionValues the new @ref sectionValues
     *
     * Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @note The property <tt>keyboardTracking()</tt> of the base class
     * is currently ignored. Keyboard tracking is <em>always</em> enabled:
     * The spinbox emits this signal while the new value is being entered
     * from the keyboard – one signal for each key stroke. */
    void sectionValuesChanged(const QList<double> &newSectionValues);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual bool event(QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual bool focusNextPrevChild(bool next) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    [[nodiscard]] virtual QAbstractSpinBox::StepEnabled stepEnabled() const override;

private:
    Q_DISABLE_COPY(MultiSpinBox)

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class MultiSpinBoxPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<MultiSpinBoxPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestMultiSpinBox;
};

} // namespace PerceptualColor

#endif // MULTISPINBOX_H
