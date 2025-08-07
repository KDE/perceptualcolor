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
/** @brief Constructor
 *
 * The object is initialized with default values. */
MultiSpinBoxSection::MultiSpinBoxSection()
    : d_pointer(new MultiSpinBoxSectionPrivate())
{
}

/** @brief Destructor */
MultiSpinBoxSection::~MultiSpinBoxSection() noexcept
{
}

/** @brief Copy constructor
 *
 * @param other the object to be copied */
MultiSpinBoxSection::MultiSpinBoxSection(const MultiSpinBoxSection &other)
{
    d_pointer.reset(
        // Create a copy of the private implementation object
        new MultiSpinBoxSectionPrivate(*other.d_pointer));
}

/** @brief Copy assignment operator
 *
 * @returns By convention, always returns <tt>*this</tt>.
 *
 * @param other the object to be copied */
MultiSpinBoxSection &MultiSpinBoxSection::operator=(const MultiSpinBoxSection &other)
{
    if (this != &other) { // protect against invalid self-assignment
        d_pointer.reset(
            // Create a copy of the private implementation object
            new MultiSpinBoxSectionPrivate(*other.d_pointer));
    }

    return *this; // By convention, always return *this.
}

/** @brief Move constructor
 *
 * @param other the object to move */
MultiSpinBoxSection::MultiSpinBoxSection(MultiSpinBoxSection &&other) noexcept
{
    d_pointer.swap(other.d_pointer);
}

/** @brief Move assignment operator
 *
 * @returns By convention, always returns <tt>*this</tt>.
 *
 * @param other the object to move-assign */
MultiSpinBoxSection &MultiSpinBoxSection::operator=(MultiSpinBoxSection &&other) noexcept
{
    if (this != &other) { // protect against invalid self-assignment
        d_pointer.swap(other.d_pointer);
    }

    return *this; // By convention, always return *this.
}

/** @brief The number of digits after the decimal point.
 *
 * This value can also be <tt>0</tt> to get integer-like behavior.
 *
 * @returns The property value.
 *
 * @sa @ref setDecimals */
int MultiSpinBoxSection::decimals() const
{
    return d_pointer->m_decimals;
}

/** @brief Setter for @ref decimals property.
 *
 * @param newDecimals The new decimals values. */
void MultiSpinBoxSection::setDecimals(int newDecimals)
{
    d_pointer->m_decimals = qBound(0, newDecimals, 323);
}

/** @brief Holds whether or not @ref MultiSpinBox::values wrap
 * around when they reaches @ref minimum or @ref maximum.
 *
 * The default is <tt>false</tt>.
 *
 * If <tt>false</tt>, @ref MultiSpinBox::values shall be bound
 * between @ref minimum and  @ref maximum. If <tt>true</tt>,
 * @ref MultiSpinBox::values shall be treated as a circular.
 *
 * Example: You have a section that displays a value measured in
 * degree. @ref minimum is <tt>0</tt>. @ref maximum is <tt>360</tt>.
 * The following corrections would be applied to input:
 * | Input | isWrapping == false | isWrapping == true |
 * | ----: | ------------------: | -----------------: |
 * |    -5 |                   0 |                355 |
 * |     0 |                   0 |                  0 |
 * |     5 |                   5 |                  5 |
 * |   355 |                 355 |                355 |
 * |   360 |                 360 |                  0 |
 * |   365 |                 360 |                  5 |
 * |   715 |                 360 |                355 |
 * |   720 |                 360 |                  0 |
 * |   725 |                 360 |                  5 |
 *
 * @returns The property value.
 *
 * @sa @ref setWrapping */
bool MultiSpinBoxSection::isWrapping() const
{
    return d_pointer->m_isWrapping;
}

/** @brief Setter for @ref isWrapping property.
 *
 * @param newIsWrapping The new isWrapping value. */
void MultiSpinBoxSection::setWrapping(bool newIsWrapping)
{
    d_pointer->m_isWrapping = newIsWrapping;
}

/** @brief The maximum possible value of the section.
 *
 * @returns The property value.
 *
 * @sa @ref setMaximum */
double MultiSpinBoxSection::maximum() const
{
    return roundToDigits(d_pointer->m_maximum, d_pointer->m_decimals);
}

/** @brief Setter for @ref maximum property.
 *
 * @param newMaximum The new maximum value. */
void MultiSpinBoxSection::setMaximum(double newMaximum)
{
    d_pointer->m_maximum = newMaximum;
    if (d_pointer->m_minimum > d_pointer->m_maximum) {
        d_pointer->m_minimum = d_pointer->m_maximum;
    }
}

/** @brief The minimum possible value of the section.
 *
 * @returns The property value.
 *
 * @sa @ref setMinimum */
double MultiSpinBoxSection::minimum() const
{
    return roundToDigits(d_pointer->m_minimum, d_pointer->m_decimals);
}

/** @brief Setter for @ref minimum property.
 *
 * @param newMinimum The new minimum value. */
void MultiSpinBoxSection::setMinimum(double newMinimum)
{
    d_pointer->m_minimum = newMinimum;
    if (d_pointer->m_maximum < d_pointer->m_minimum) {
        d_pointer->m_maximum = d_pointer->m_minimum;
    }
}

/**
 * @brief This property holds whether the group separator (thousands separator)
 * is enabled.
 *
 * By default this property is false.
 *
 * @returns The property value.
 *
 * @sa @ref setGroupSeparatorShown
 */
bool MultiSpinBoxSection::isGroupSeparatorShown() const
{
    return d_pointer->m_isGroupSeparatorShown;
}

/**
 * @brief Setter for @ref isGroupSeparatorShown property.
 *
 * @param shown The new value.
 */
void MultiSpinBoxSection::setGroupSeparatorShown(bool shown)
{
    d_pointer->m_isGroupSeparatorShown = shown;
}

/** @brief A prefix to be displayed before the value.
 *
 * @returns The property value.
 *
 * @sa @ref setFormatString
 */
QString MultiSpinBoxSection::prefix() const
{
    return d_pointer->m_prefix;
}

/** @brief A smaller of two natural steps.
 *
 * Valid range: >= 0
 *
 * When the user uses the arrows to change the spin box’s value
 * the value will be incremented/decremented by the amount of the
 * @ref singleStep.
 *
 * @returns The property value.
 *
 * @sa @ref setSingleStep */
double MultiSpinBoxSection::singleStep() const
{
    return d_pointer->m_singleStep;
}

/** @brief Setter for @ref singleStep property.
 *
 * @param newSingleStep The new single step value. */
void MultiSpinBoxSection::setSingleStep(double newSingleStep)
{
    d_pointer->m_singleStep = qMax<double>(0, newSingleStep);
}

/** @brief The suffix to be displayed behind the value.
 *
 * @returns The property value.
 *
 * @sa @ref setFormatString
 */
QString MultiSpinBoxSection::suffix() const
{
    return d_pointer->m_suffix;
}

/** @brief Adds QDebug() support for data type
 * @ref PerceptualColor::MultiSpinBoxSection
 *
 * @param dbg Existing debug object
 * @param value Value to stream into the debug object
 * @returns Debug object with value streamed in */
QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSection &value)
{
    dbg.nospace() << "\nMultiSpinBoxSection(" // Opening line
                  << "\n    decimals: " << value.decimals() //
                  << "\n    formatString: " << value.formatString() //
                  << "\n    isWrapping: " << value.isWrapping() //
                  << "\n    maximum: " << value.maximum() //
                  << "\n    minimum: " << value.minimum() //
                  << "\n    prefix: " << value.prefix() //
                  << "\n    suffix: " << value.suffix() //
                  << "\n)" // Closing line
        ;
    return dbg.maybeSpace();
}

/**
 * @brief Setter for the @ref prefix(), @ref suffix() and @ref formatString()
 * properties.
 *
 * @param formatString A string in the format "prefix%1suffix". It
 * should contain exactly <em>one</em> place marker as described in
 * <tt>QString::arg()</tt> like <tt>\%1</tt> or <tt>\%L2</tt>. This place
 * marker represents the value. Example: “Prefix\%1Suffix”. Prefix and suffix
 * may be empty.
 */
void MultiSpinBoxSection::setFormatString(const QString &formatString)
{
    d_pointer->m_formatString = formatString;

    // QString::arg() support for %L2, %5 etc which translators might expect:
    const auto parts = formatString //
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

/**
 * @brief A string in the format "PREFIX%vSUFFIX".
 *
 * @returns The property value.
 *
 * @sa @ref setFormatString()
 * @sa @ref prefix()
 * @sa @ref suffix()
 */
QString MultiSpinBoxSection::formatString() const
{
    return d_pointer->m_formatString;
}

} // namespace PerceptualColor
