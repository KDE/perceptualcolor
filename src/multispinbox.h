// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_MULTISPINBOX_H
#define PERCEPTUALCOLOR_MULTISPINBOX_H

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
#include <qtmetamacros.h>
#include <qvalidator.h>

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

/*!
 * \inmodule ModulePublicApi
 *
 * \brief A spin box that can hold multiple sections (each with its own
 * value) simultaneously.
 *
 * This widget is conceptually similar to \c {QDateTimeEdit}, which also
 * displays multiple editable sections (e.g., day, month, year) within a
 * single spin box. However, \e {this} widget offers significantly more
 * flexibility—you can define an arbitrary number of sections, each with its
 * own behavior, formatting, and constraints.
 *
 * \image MultiSpinBox.png "MultiSpinBox"
 *
 * This widget uses floating-point precision. You can configure the
 * number of decimal places individually for each section via
 * \l MultiSpinBoxSection::decimals. Use \c {0} for integer-like input.
 *
 * Additional buttons can be embedded in the spin box
 * using \l addActionButton().
 *
 * For example, you can use \l MultiSpinBox to represent an HSV color:
 *
 * \list
 *     \li Hue: 0°–360°
 *     \li Saturation: 0–255
 *     \li Value: 0–255
 * \endlist
 *
 * \snippet testmultispinbox.cpp MultiSpinBox Basic example
 *
 * This class inherits from \c {QAbstractSpinBox} and supports most
 * of its API, with a few key exceptions:
 *
 * \list
 *
 * \li \b {\c {wrapping}, \c {specialValueText}, \c {showGroupSeparator}:}
 *     These properties are ignored at the \l MultiSpinBox level because they
 *     need to be defined individually for each section. Instead:
 *     \list
 *     \li Configure section-level behavior using
 *         \l MultiSpinBoxSection::setWrapping() and
 *         \l MultiSpinBoxSection::setGroupSeparatorShown().
 *     \li \c {specialValueText} is currently not supported.
 *     \endlist
 *
 * \li \b {\c {fixup()}, \c {QAbstractSpinBox::interpretText()}:}
 *     Although \c {QAbstractSpinBox} defines these methods, they have no
 *     effect in the base class itself or in non-Qt subclasses like this one.
 *     They appears more like leaked implementation details than a truly
 *     extensible public API. As a result, in this class these function are
 *     effectively placeholders with no functional impact.
 *     \list
 *     \li In Qt’s own derived classes, \c {QAbstractSpinBox::interpretText()}
 *         performs meaningful parsing and updates—but this relies on private
 *         Qt internals that are inaccessible to external or custom subclasses.
 *         And since the method is not declared \c {virtual}, it cannot be
 *         overridden anyway.
 *     \li For more details on \l fixup(), please refer to its documentation.
 *     \endlist
 *
 * \endlist
 */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBox : public QAbstractSpinBox
{
    // NOTE The API could be extended in the future:
    //
    // \snippet testmultispinbox.cpp MultiSpinBox Full-featured MultiSpinBoxSection
    //
    // \snippet testmultispinbox.cpp MultiSpinBox Full-featured MultiSpinBox
    Q_OBJECT

    /*!
     * \property PerceptualColor::MultiSpinBox::sectionCount
     *
     * \brief The current number of sections.
     *
     * \note The number of available sections is not determined by this
     * property, but by format.
     *
     * This property always contains the same number of elements as
     * format contains.
     *
     * This is the counterpart to
     * \c {int QDateTimeEdit::sectionCount() const}.
     */
    Q_PROPERTY(qsizetype sectionCount READ sectionCount NOTIFY sectionCountChanged)

    /*!
     * \property PerceptualColor::MultiSpinBox::values
     *
     * \brief A list containing the current values for all sections.
     *
     * \note The number of available sections is not determined by this
     * property, but by format. To modify the section
     * count, you must call \l setFormat first, then update
     * this property.
     *
     * \note If the \c {QAbstractSpinBox::keyboardTracking} property is
     * set to \c {false}, updates to the property triggered by keyboard
     * input are deferred until editing is complete (when the Return/Enter key
     * or Tab key is pressed, when keyboard focus is lost, or when other
     * spinbox functionality is used, e.g. pressing an arrow key or a button).
     * In contrast, the \c {QAbstractSpinBox::text} property always
     * reflects the current visible content of the editor.
     *
     * This property always contains the same number of elements as
     * format contains.
     *
     * \note Alternative notify signal: \l valuesChangedAsQString()
     */
    Q_PROPERTY(QList<double> values READ values WRITE setValues NOTIFY valuesChanged USER true)

public:
    Q_INVOKABLE explicit MultiSpinBox(QWidget *parent = nullptr);
    virtual ~MultiSpinBox() noexcept override;
    virtual void actionEvent(QActionEvent *event) override;
    Q_INVOKABLE void addActionButton(QAction *action, QLineEdit::ActionPosition position);
    virtual void clear() override;
    virtual void fixup(QString &input) const override;
    [[nodiscard]] Q_INVOKABLE QList<PerceptualColor::MultiSpinBoxSection> format() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    /*!
     * \brief Getter for property \l sectionCount
     *
     * This is the counterpart to
     * \c {int QDateTimeEdit::sectionCount() const}.
     */
    [[nodiscard]] Q_INVOKABLE qsizetype sectionCount() const;
    Q_INVOKABLE void setFormat(const QList<PerceptualColor::MultiSpinBoxSection> &newFormat);
    [[nodiscard]] virtual QSize sizeHint() const override;
    virtual void stepBy(int steps) override;
    virtual QValidator::State validate(QString &input, int &pos) const override;
    /*!
     * \brief Getter for property \l values
     *
     * This is the counterpart to
     * \c {double QDoubleSpinBox::value() const}.
     */
    [[nodiscard]] QList<double> values() const;

public Q_SLOTS:
    void setValues(const QList<double> &newValues);

Q_SIGNALS:
    /*!
     * \brief Notifier signal for the \l sectionCount property.
     *
     * \a newSectionCount The updated \l sectionCount
     */
    void sectionCountChanged(qsizetype newSectionCount);
    /*!
     * \brief Notifier signal for the \l values property.
     *
     * This signal is emitted simultaneously with
     * \l valuesChangedAsQString whenever the values in one or more
     * sections of the \l values property change.
     *
     * \a newValues The updated \l values
     *
     * Depending on your use case (for
     * example if you want to use for \e {queued} signal-slot connections),
     * you might consider calling \c {qRegisterMetaType()} for
     * this type, once you have a QApplication object.
     *
     * This is the counterpart to
     * \c {void QDoubleSpinBox::valueChanged(double d)}.
     */
    void valuesChanged(const QList<double> &newValues);
    /*!
     * \brief Alternative notifier signal for the \l values property
     *        with a QString argument.
     *
     * This signal is emitted simultaneously with
     * \l valuesChanged  whenever the values in one or more
     * sections of the \l values property change, but provides the
     * values as a QString representation.
     *
     * \a newValuesQString The updated \l values as a
     *        QString. This string reflects the current textual representation
     *        visible to the user in the widget. It may also include
     *        intermediate editing states, such as \c {01} instead of
     *        \c {1}, depending on the user’s input and formatting at the
     *        time the signal is emitted.
     *
     * This is the counterpart to
     * \c {QDoubleSpinBox::textChanged(const QString &)}.
     *
     * This signal serves as the counterpart to
     * \c {QDoubleSpinBox::textChanged(const QString &)},
     * but intentionally uses a distinct and more descriptive name.
     *
     * Qt’s design history provides helpful context: QDoubleSpinBox has the
     * \c {value} property, with the signal \c {valueChanged(double)}
     * as its notifier. In earlier versions of Qt, an overload
     * \c {valueChanged(const QString &)} existed alongside
     * \c {valueChanged(double)}, emitting the same value in string form.
     * However, overloaded signals complicate usage, especially in
     * \c {QObject::connect()}, as they require disambiguation through
     * constructs like \c {QOverload<double>::of()}. To resolve this,
     * Qt 5.14 introduced a new signal \c {textChanged(const QString &)},
     * and subsequently deprecated the overload—removing it in Qt 6.
     *
     * Unfortunately, the name \c {textChanged} is misleading: it suggests
     * a notifier for the \c {QAbstractSpinBox::text} property, which it
     * is not. In reality, it responds to changes in the \c {value}
     * property and is deferred just like \c {valueChanged()} when
     * \c {QAbstractSpinBox::keyboardTracking} is set to \c {false};
     * whereas the \c {text} property itself is never deferred.
     *
     * To avoid ambiguity, \l MultiSpinBox uses a name that clearly reflects
     * the signal’s true nature: it is a string-based alternative notifier
     * for the \l values property, and has no relation to
     * \c {QAbstractSpinBox::text}.
     */
    void valuesChangedAsQString(const QString &newValuesQString);

protected:
    virtual void changeEvent(QEvent *eventParameter) override;
    virtual bool event(QEvent *eventParameter) override;
    virtual void focusInEvent(QFocusEvent *eventParameter) override;
    virtual bool focusNextPrevChild(bool next) override;
    virtual void focusOutEvent(QFocusEvent *eventParameter) override;
    virtual void keyPressEvent(QKeyEvent *eventParameter) override;
    [[nodiscard]] virtual QAbstractSpinBox::StepEnabled stepEnabled() const override;

private:
    Q_DISABLE_COPY(MultiSpinBox)

    friend class MultiSpinBoxPrivate;
    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<MultiSpinBoxPrivate> d_pointer;

    friend class TestMultiSpinBox;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_MULTISPINBOX_H
