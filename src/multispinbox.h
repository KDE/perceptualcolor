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
class QKeyEvent;
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
 *   - Configure section-level behavior using
 *     @ref MultiSpinBoxSection::setWrapping() and
 *     @ref MultiSpinBoxSection::setGroupSeparatorShown().
 *   - <tt>specialValueText</tt> is currently not supported.
 *
 * - <b><tt>@ref fixup()</tt>,
 *   <tt>QAbstractSpinBox::interpretText()</tt>:</b><br/>
 *   Although <tt>QAbstractSpinBox</tt> defines these methods, they have no
 *   effect in the base class itself or in non-Qt subclasses like this one.
 *   They appears more like leaked implementation details than a truly
 *   extensible public API. As a result, in this class these function are
 *   effectively placeholders with no functional impact.
 *   - In Qt’s own derived classes, <tt>QAbstractSpinBox::interpretText()</tt>
 *     performs meaningful parsing and updates—but this relies on private Qt
 *     internals that are inaccessible to external or custom subclasses. And
 *     since the method is not declared <tt>virtual</tt>, it cannot be
 *     overridden anyway.
 *   - For more details on @ref fixup(), please refer to its documentation.
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
 * For feature parity with QAbstractSpinBox, QDoubleSpinBox, QDateTimeEdit and
 * the relevant parts of QLineEdit, the API could be extended in the future:
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

    /** @brief A list containing the current values for all sections.
     *
     * @note The number of available sections is not determined by this
     * property, but by @ref format. To modify the section
     * count, you must call @ref setFormat first, then update
     * this property.
     *
     * @note If the <tt>QAbstractSpinBox::keyboardTracking</tt> property is
     * set to <tt>false</tt>, updates to the property triggered by keyboard
     * input are deferred until editing is complete (when the Return/Enter key
     * or Tab key is pressed, when keyboard focus is lost, or when other
     * spinbox functionality is used, e.g. pressing an arrow key or a button).
     * In contrast, the <tt>QAbstractSpinBox::text</tt> property always
     * reflects the current visible content of the editor.
     *
     * @invariant This property always contains the same number of elements as
     * @ref format contains.
     *
     * @sa READ @ref values() const
     * @sa WRITE @ref setValues()
     * @sa NOTIFY @ref valuesChanged()
     * @sa Alternative notify signal: @ref valuesChangedAsQString()
     */
    Q_PROPERTY(QList<double> values READ values WRITE setValues NOTIFY valuesChanged USER true)

public:
    Q_INVOKABLE explicit MultiSpinBox(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~MultiSpinBox() noexcept override;
    virtual void actionEvent(QActionEvent *event) override;
    void addActionButton(QAction *action, QLineEdit::ActionPosition position);
    virtual void clear() override;
    virtual void fixup(QString &input) const override;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    [[nodiscard]] Q_INVOKABLE QList<PerceptualColor::MultiSpinBoxSection> format() const;
    /** @brief Getter for property @ref values
     *  @returns the property @ref values
     *
     * @internal
     *
     * This it the counterpart to
     * <tt>double QDoubleSpinBox::value() const</tt>.
     */
    [[nodiscard]] QList<double> values() const;
    Q_INVOKABLE void setFormat(const QList<PerceptualColor::MultiSpinBoxSection> &newFormat);
    [[nodiscard]] virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;
    virtual QValidator::State validate(QString &input, int &pos) const override;

public Q_SLOTS:
    void setValues(const QList<double> &newValues);

Q_SIGNALS:
    /**
     * @brief Notifier signal for the @ref values property.
     *
     * This signal is emitted simultaneously with
     * @ref valuesChangedAsQString whenever the values in one or more
     * sections of the @ref values property change.
     *
     * @param newValues The updated @ref values
     *
     * Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @internal
     *
     * This it the counterpart to
     * <tt>void QDoubleSpinBox::valueChanged(double d)</tt>.
     */
    void valuesChanged(const QList<double> &newValues);
    /**
     * @brief Alternative notifier signal for the @ref values property
     *        with a QString argument.
     *
     * This signal is emitted simultaneously with
     * @ref valuesChanged  whenever the values in one or more
     * sections of the @ref values property change, but provides the
     * values as a QString representation.
     *
     * @param newValuesQString The updated @ref values as a
     *        QString. This string reflects the current textual representation
     *        visible to the user in the widget. It may also include
     *        intermediate editing states, such as <tt>01</tt> instead of
     *        <tt>1</tt>, depending on the user’s input and formatting at the
     *        time the signal is emitted.
     *
     * @internal
     *
     * This it the counterpart to
     * <tt>QDoubleSpinBox::textChanged(const QString &)</tt>.
     *
     * @note This signal serves as the counterpart to
     * <tt>QDoubleSpinBox::textChanged(const QString &)</tt>,
     * but intentionally uses a distinct and more descriptive name.
     * <br/>
     * Qt’s design history provides helpful context: QDoubleSpinBox has the
     * <tt>value</tt> property, with the signal <tt>valueChanged(double)</tt>
     * as its notifier. In earlier versions of Qt, an overload
     * <tt>valueChanged(const QString &)</tt> existed alongside
     * <tt>valueChanged(double)</tt>, emitting the same value in string form.
     * However, overloaded signals complicate usage, especially in
     * <tt>QObject::connect()</tt>, as they require disambiguation through
     * constructs like <tt>QOverload<double>::of()</tt>. To resolve this,
     * Qt 5.14 introduced a new signal <tt>textChanged(const QString &)</tt>,
     * and subsequently deprecated the overload—removing it in Qt 6.
     * <br/>
     * Unfortunately, the name <tt>textChanged</tt> is misleading: it suggests
     * a notifier for the <tt>QAbstractSpinBox::text</tt> property, which it
     * is not. In reality, it responds to changes in the <tt>value</tt>
     * property and is deferred just like <tt>valueChanged()</tt> when
     * <tt>QAbstractSpinBox::keyboardTracking</tt> is set to <tt>false</tt>;
     * whereas the <tt>text</tt> property itself is never deferred.
     * <br/>
     * To avoid ambiguity, @ref MultiSpinBox uses a name that clearly reflects
     * the signal’s true nature: it is a string-based alternative notifier
     * for the @ref values property, and has no relation to
     * <tt>QAbstractSpinBox::text</tt>.
     */
    void valuesChangedAsQString(const QString &newValuesQString);

protected:
    virtual void changeEvent(QEvent *event) override;
    virtual bool event(QEvent *event) override;
    virtual void focusInEvent(QFocusEvent *event) override;
    virtual bool focusNextPrevChild(bool next) override;
    virtual void focusOutEvent(QFocusEvent *event) override;
    virtual void keyPressEvent(QKeyEvent *event) override;
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
