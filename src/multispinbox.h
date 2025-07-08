// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef MULTISPINBOX_H
#define MULTISPINBOX_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
// Including multispinboxsection.h is necessary on Qt6,
// otherwise moc will fail. (IWYU does not detect this dependency.)
#include "multispinboxsection.h" // IWYU pragma: keep
#include <qabstractspinbox.h>
#include <qglobal.h>
#include <qlineedit.h>
#include <qlist.h>
#include <qsize.h>
#include <qvalidator.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

class QAction;
class QActionEvent;
class QEvent;
class QFocusEvent;
class QString;
class QWidget;

namespace PerceptualColor
{
class MultiSpinBoxPrivate;

/** @brief A spin box that can hold multiple sections (each with its own
 * value) simultaneously.
 *
 * This widget is conceptually similar to <tt>QDateTimeEdit</tt>, which also
 * displays multiple editable sections (e.g., day, month, year) within a
 * single spin box. However, <em>this</em> widget offers significantly more
 * flexibility—you can define an arbitrary number of sections, each with its
 * own behavior, formatting, and constraints.
 *
 * @image html MultiSpinBox.png "MultiSpinBox"
 *
 * This widget uses floating-point precision. You can configure the
 * number of decimal places individually for each section via
 * @ref MultiSpinBoxSection::decimals. Use <tt>0</tt> for integer-like input.
 *
 * Additional buttons can be embedded in the spin box
 * using @ref addActionButton().
 *
 * For example, you can use @ref MultiSpinBox to represent an HSV color:
 * - Hue: 0°–360°
 * - Saturation: 0–255
 * - Value: 0–255
 *
 * @snippet testmultispinbox.cpp MultiSpinBox Basic example
 *
 * @note This class inherits from <tt>QAbstractSpinBox</tt> and supports most
 * of its API, with a few key exceptions:
 * - <b><tt>wrapping</tt>, <tt>specialValueText</tt>,
 *   <tt>showGroupSeparator</tt>:</b><br/>
 *   These properties are ignored at the @ref MultiSpinBox level because they
 *   need to be defined individually for each section. Instead:
 *   - Use @ref MultiSpinBoxSection::setWrapping() to enable wrapping per
 *     section.
 *   - <tt>specialValueText</tt> and <tt>showGroupSeparator</tt> are not
 *     currently supported.
 *
 * - <b><tt>keyboardTracking()</tt>:</b><br/>
 *   This property is ignored—keyboard tracking is always enabled in
 *   @ref MultiSpinBox.
 *   <br><i>(In Qt’s own widgets, disabling keyboardTracking defers updates:
 *   user input via the keyboard (excluding Tab and Enter) doesn’t modify the
 *   value property or emit signals like <tt>valueChanged()</tt> or
 *   <tt>textChanged()</tt>. Changes are applied only when the widget loses
 *   focus, the user presses Enter or Tab, or uses the arrow buttons. That
 *   deferred behavior is not implemented here.)</i>
 *
 * - <b><tt>interpretText()</tt>:</b><br/>
 *   Although <tt>QAbstractSpinBox</tt> defines this method, it has no effect
 *   in the base class itself or in non-Qt subclasses like this one. (In Qt’s
 *   own derived classes, it performs meaningful parsing and updates—but this
 *   relies on private internal APIs inaccessible to external or custom
 *   subclasses.) As a result, <tt>interpretText()</tt> in this context is
 *   effectively a placeholder with no functional impact. It appears more like
 *   a leaked implementation detail than a truly extensible public API.
 *
 * @internal
 *
 * @todo <tt>selectAll()</tt>:
 *   This slot has a default behaviour that relies on internal
 *   <tt>QAbstractSpinBox</tt> private implementations, which we cannot use
 *   because they are not part of the public API and can therefore change
 *   at any moment. As it isn’t virtual, we cannot reimplement it either.
 *   (Does this shortcut
 *   trigger <tt>selectAll()</tt>?)
 *   Strg+A selects the whole text of the MultiSpinBox. The text cursor
 *   position is at the beginning. It's the first section that will react on
 *   Page-up and Page-down events. This is conform to the behaviour
 *   of QTimeDateEdit. The underlying problem is however that we should make
 *   the normal text selection work as expected:
 *   It is possible to select (either with the left mouse click + mouse
 *   move, or with Shift key + the arrow keys) arbitrary parts of the line
 *   edit, including partial and complete selection of prefixes, suffixes and
 *   separators. It's possible to copy this text (both, by Ctrl+C and by
 *   insertion by middle mouse click on Linux).
 *
 * @todo Oklch second value, German localization, set it to 0,10.
 * Then change it to 0.10 (with dot as decimal separator).
 * It should then either jump to 2.00 (nearest value) or to 0.10 (previous
 * value). But it jumps to 0.00 instead.
 *
 * @todo Bug: In @ref ColorDialog, choose a tab with one of the diagrams.
 * Then, switch back the the “numeric“ tab. Expected behaviour: When
 * a @ref MultiSpinBox gets back the focus, always the first section should
 * be <em>highlighted/selected</em>, independent from what was selected or
 * the cursor position before the @ref MultiSpinBox lost the focus.
 * (While <tt>QSpinBox</tt> and <tt>QDoubleSpinBox</tt> don’t do that
 * either, <tt>QDateTimeEdit</tt> indeed <em>does</em>, and that seems
 * appropriate also for @ref MultiSpinBox.
 * On the other hand: QLineEdit
 * leaves the curser where it is (as long as no diagram widget was clicked and
 * the values changed)
 * and that seems practical for small switches?
 *
 * @todo Now, @ref setSectionValues does not select automatically the first
 * section anymore. Is this in conformance with <tt>QDateTimeEdit</tt>?
 * Test: Change the value in the middle. Push “Apply” button. Now, the
 * curser is at the end of the spin box, but the active section is still
 * the one in the middle (you can try this by using your mouse wheel on
 * the widget).
 *
 * @todo When adding Bengali digits (for example by copy and paste) to a
 * @ref MultiSpinBox that was localized to en_US, than sometimes this is
 * accepted (thought later “corrected” to 0), and sometimes not. This
 * behaviour is inconsistent and wrong.
 *
 * @todo Validation behaviour of MultiSpinBox differs from QDoubleSpinBox.
 * The latter is much strikter: It seems to not even allow intermediate
 * states that do not directly translate (after fixup(), I suppose?) to a
 * valid value in the valid range. We have to test
 * correctionMode/setCorrectionMode which seems quite impossible in these
 * conditions. Can we get indirectly information by observing how value()
 * changes (in lambdas connected to valueChanged or textChanged)?
 *
 * @todo Right-click context menu behaves strange: When used, it changes
 * the cursor position. And increment/decrement is applied to the wrong
 * section (not the original one, the always the first one - while
 * QDateTimeEdit applies it correctly to the current one).
 *
 * @todo Before publishing the public API, ensure its stability.
 * The API does not need to be feature-complete, but its current shape must
 * remain unchanged going forward. In short: implement keyboardTracking before
 * release.
 *
 * For feature parity with QAbstractSpinBox, QDoubleSpinBox, QDateTimeEdit and
 * the relevant parts of QLineEdit, the API could be extended.
 *
 * Full-featured @ref MultiSpinBoxSection API:
 * @snippet testmultispinbox.cpp MultiSpinBox Full-featured MultiSpinBoxSection
 *
 * Full-featured @ref MultiSpinBox API:
 * @snippet testmultispinbox.cpp MultiSpinBox Full-featured MultiSpinBox
 */
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
    virtual void actionEvent(QActionEvent *event) override;
    void addActionButton(QAction *action, QLineEdit::ActionPosition position);
    virtual void clear() override;
    virtual void fixup(QString &input) const override;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] Q_INVOKABLE QList<PerceptualColor::MultiSpinBoxSection> sectionConfigurations() const;
    /** @brief Getter for property @ref sectionValues
     *  @returns the property @ref sectionValues */
    [[nodiscard]] QList<double> sectionValues() const;
    Q_INVOKABLE void setSectionConfigurations(const QList<PerceptualColor::MultiSpinBoxSection> &newSectionConfigurations);
    [[nodiscard]] virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;
    virtual QValidator::State validate(QString &input, int &pos) const override;

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
     */
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
