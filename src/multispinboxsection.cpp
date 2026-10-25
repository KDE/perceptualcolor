// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "multispinboxsection.h"
// Second, the private implementation.
#include "multispinboxsection_p.h" // IWYU pragma: associated

#include "constpropagatinguniquepointer.h"
#include "helpermath.h"
#include <qglobal.h>
#include <qlist.h>
#include <qstringliteral.h>

namespace PerceptualColor
{

/*!
 * \brief Constructor
 *
 * The object is initialized with default values. */
MultiSpinBoxSection::MultiSpinBoxSection() noexcept
    : d_pointer(new MultiSpinBoxSectionPrivate())
{
}

/*!
 * \brief Destructor */
MultiSpinBoxSection::~MultiSpinBoxSection() noexcept
{
}

/*!
 * \brief Copy constructor
 *
 * \a other the object to be copied
 */
MultiSpinBoxSection::MultiSpinBoxSection(const MultiSpinBoxSection &other) noexcept
{
    d_pointer.reset(
        // Create a copy of the private implementation object
        new MultiSpinBoxSectionPrivate(*other.d_pointer));
}

/*!
 * \brief Copy assignment operator
 *
 * By convention, always returns \c {*this}.
 *
 * \a other the object to be copied
 */
MultiSpinBoxSection &MultiSpinBoxSection::operator=(const MultiSpinBoxSection &other) noexcept
{
    if (this != &other) { // protect against invalid self-assignment
        d_pointer.reset(
            // Create a copy of the private implementation object
            new MultiSpinBoxSectionPrivate(*other.d_pointer));
    }

    return *this; // By convention, always return *this.
}

/*!
 * \brief Move constructor
 *
 * \a other the object to move
 */
MultiSpinBoxSection::MultiSpinBoxSection(MultiSpinBoxSection &&other) noexcept
{
    d_pointer.swap(other.d_pointer);
}

/*!
 * \brief Move assignment operator
 *
 * By convention, always returns \c {*this}.
 *
 * \a other the object to move-assign
 */
MultiSpinBoxSection &MultiSpinBoxSection::operator=(MultiSpinBoxSection &&other) noexcept
{
    if (this != &other) { // protect against invalid self-assignment
        d_pointer.swap(other.d_pointer);
    }

    return *this; // By convention, always return *this.
}

/*!
 * \brief The number of digits after the decimal point.
 *
 * This value can also be \c {0} to get integer-like behavior.
 *
 * \sa setDecimals
 */
int MultiSpinBoxSection::decimals() const
{
    return d_pointer->m_decimals;
}

/*!
 * \brief Setter for \l decimals property.
 *
 * \a newDecimals The new \l decimals values.
 */
void MultiSpinBoxSection::setDecimals(int newDecimals)
{
    d_pointer->m_decimals = qBound(0, newDecimals, 323);
}

/*!
 * \brief This property holds whether or not \l MultiSpinBox::values wrap
 * around when they reaches \l minimum or \l maximum.
 *
 * Return the property value.
 *
 * The default is \c {false}.
 *
 * If \c {false}, \l MultiSpinBox::values shall be bound
 * between \l minimum and  \l maximum. If \c {true},
 * \l MultiSpinBox::values shall be treated as a circular.
 *
 * Example: You have a section that displays a value measured in
 * degree. \l minimum is \c {0}. \l maximum is \c {360}.
 * The following corrections would be applied to input:
 *
 * \table
 *     \header \li Input \li \l isWrapping == false \li \l isWrapping == true
 *     \row    \li -5    \li 0                      \li 355
 *     \row    \li 0     \li 0                      \li 0
 *     \row    \li 5     \li 5                      \li 5
 *     \row    \li 355   \li 355                    \li 355
 *     \row    \li 360   \li 360                    \li 0
 *     \row    \li 365   \li 360                    \li 5
 *     \row    \li 715   \li 360                    \li 355
 *     \row    \li 720   \li 360                    \li 0
 *     \row    \li 725   \li 360                    \li 5
 * \endtable
 *
 * \sa setWrapping
 */
bool MultiSpinBoxSection::isWrapping() const
{
    return d_pointer->m_isWrapping;
}

/*!
 * \brief Setter for \l isWrapping property.
 *
 * \a newIsWrapping The new \l isWrapping value.
 */
void MultiSpinBoxSection::setWrapping(bool newIsWrapping)
{
    d_pointer->m_isWrapping = newIsWrapping;
}

/*!
 * \brief Convenience function to set the \l minimum and maximum values
 * with a single function call.
 *
 * \a newMinimum The new minimum value.
 *
 * \a newMaximum The new maximum value.
 */
void MultiSpinBoxSection::setRange(double newMinimum, double newMaximum)
{
    d_pointer->m_minimum = newMinimum;
    d_pointer->m_maximum = //
        newMaximum < newMinimum //
        ? newMinimum
        : newMaximum;
}

/*!
 * \brief The \l maximum possible value of the section.
 *
 * \sa setMaximum
 * \sa setRange
 */
double MultiSpinBoxSection::maximum() const
{
    return roundToDigits(d_pointer->m_maximum, d_pointer->m_decimals);
}

/*!
 * \brief Setter for \l maximum property.
 *
 * \a newMaximum The new maximum value.
 *
 * \sa setRange
 */
void MultiSpinBoxSection::setMaximum(double newMaximum)
{
    d_pointer->m_maximum = newMaximum;
    if (d_pointer->m_minimum > d_pointer->m_maximum) {
        d_pointer->m_minimum = d_pointer->m_maximum;
    }
}

/*!
 * \brief The minimum possible value of the section.
 *
 * \sa setMinimum
 * \sa setRange
 */
double MultiSpinBoxSection::minimum() const
{
    return roundToDigits(d_pointer->m_minimum, d_pointer->m_decimals);
}

/*!
 * \brief Setter for \l minimum property.
 *
 * \a newMinimum The new minimum value.
 *
 * \sa setRange
 */
void MultiSpinBoxSection::setMinimum(double newMinimum)
{
    d_pointer->m_minimum = newMinimum;
    if (d_pointer->m_maximum < d_pointer->m_minimum) {
        d_pointer->m_maximum = d_pointer->m_minimum;
    }
}

/*!
 * \brief This property holds whether the group separator (thousands separator)
 * is enabled.
 *
 * Returns the property value.
 *
 * By default this property is false.
 *
 * \sa setGroupSeparatorShown
 */
bool MultiSpinBoxSection::isGroupSeparatorShown() const
{
    return d_pointer->m_isGroupSeparatorShown;
}

/*!
 * \brief Setter for \l isGroupSeparatorShown property.
 *
 * \a shown The new value.
 */
void MultiSpinBoxSection::setGroupSeparatorShown(bool shown)
{
    d_pointer->m_isGroupSeparatorShown = shown;
}

/*!
 * \brief A prefix to be displayed before the value.
 *
 * \sa setFormatString
 */
QString MultiSpinBoxSection::prefix() const
{
    return d_pointer->m_prefix;
}

/*!
 * \brief A smaller of two natural steps.
 *
 * Valid range: >= 0
 *
 * When the user uses the arrows to change the spin box’s value
 * the value will be incremented/decremented by the amount of the
 * \l singleStep.
 *
 * \sa setSingleStep
 */
double MultiSpinBoxSection::singleStep() const
{
    return d_pointer->m_singleStep;
}

/*!
 * \brief Setter for \l singleStep property.
 *
 * \a newSingleStep The new single step value.
 */
void MultiSpinBoxSection::setSingleStep(double newSingleStep)
{
    d_pointer->m_singleStep = qMax<double>(0, newSingleStep);
}

/*!
 * \brief The suffix to be displayed behind the value.
 *
 * \sa setFormatString
 */
QString MultiSpinBoxSection::suffix() const
{
    return d_pointer->m_suffix;
}

/*!
 * \relates PerceptualColor::MultiSpinBoxSection
 *
 * \brief Adds QDebug() support for data type
 * \l PerceptualColor::MultiSpinBoxSection
 *
 * \a dbg Existing debug object
 *
 * \a value Value to stream into the debug object
 */
QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSection &value)
{
    dbg.nospace() << "\nMultiSpinBoxSection(" // Opening line
                  << "\n    decimals: " << value.decimals() //
                  << "\n    formatString: " << value.formatString() //
                  << "\n    isGroupSeparatorShown: " << value.isGroupSeparatorShown() //
                  << "\n    isWrapping: " << value.isWrapping() //
                  << "\n    maximum: " << value.maximum() //
                  << "\n    minimum: " << value.minimum() //
                  << "\n    prefix: " << value.prefix() //
                  << "\n    singleStep: " << value.singleStep() //
                  << "\n    suffix: " << value.suffix() //
                  << "\n)" // Closing line
        ;
    return dbg.maybeSpace();
}

/*!
 * \brief Setter for the prefix(), suffix() and formatString()
 * properties.
 *
 * \a newFormatString A string in the format "prefix%1suffix". It
 * should contain exactly \e {one} place marker as described in
 * \c {QString::arg()} like \c {\%1} or \c {\%L2}. This place
 * marker represents the value. Example: “Prefix\%1Suffix”. Prefix and suffix
 * may be empty.
 */
void MultiSpinBoxSection::setFormatString(const QString &newFormatString)
{
    d_pointer->m_formatString = newFormatString;

    // QString::arg() support for %L2, %5 etc which translators might expect:
    const auto parts = newFormatString //
                           .arg(QStringLiteral("%1")) //
                           .split(QStringLiteral("%1"));

    if (parts.size() == 2) {
        d_pointer->m_prefix = parts.at(0);
        d_pointer->m_suffix = parts.at(1);
    } else {
        d_pointer->m_prefix = QString();
        d_pointer->m_suffix = QString();
    }
}

/*!
 * \brief A string in the format "PREFIX%vSUFFIX".
 *
 * \sa setFormatString()
 * \sa prefix()
 * \sa suffix()
 */
QString MultiSpinBoxSection::formatString() const
{
    return d_pointer->m_formatString;
}

} // namespace PerceptualColor
