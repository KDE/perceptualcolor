// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPER_H
#define HELPER_H

#include "helpermath.h"
#include "helperqttypes.h"
#include "lcms2.h"
#include <QtCore/qsharedpointer.h>
#include <optional>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qglobal.h>
#include <qicon.h>
#include <qimage.h>
#include <qlist.h>
#include <qmap.h>
#include <qmetaobject.h>
#include <qmetatype.h>
#include <qpair.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qthread.h>

class QWheelEvent;
class QWidget;

namespace PerceptualColor
{

class RgbColorSpace;

/** @brief Represents the appearance of a theme.
 *
 * @todo Substitute this by
 * <a href="https://doc-snapshots.qt.io/qt6-dev/qt.html#ColorScheme-enum"><tt>
 * enum class Qt::ColorScheme</tt></a> which is available since Qt 6.
 */
enum class ColorSchemeType {
    Light, /**< Light theme. */
    Dark /**< Dark theme. */
};

void drawQWidgetStyleSheetAware(QWidget *widget);

QString fromMnemonicToRichText(const QString &mnemonicText);

ColorSchemeType guessColorSchemeTypeFromWidget(QWidget *widget);

QMap<cmsUInt32Number, QString> lcmsIntentList();

/** @internal
 *
 * @brief Convenience function template that tests if a value is in a list.
 *
 * @param first The value
 * @param t The list
 *
 * Usage:
 * @snippet testhelper.cpp isInUsage
 *
 * @returns <tt>true</tt> if “value” is in “list”. <tt>false</tt> otherwise. */
template<typename First, typename... T>
bool isIn(First &&first, T &&...t)
{
    // Solution as proposed by Nikos C. in https://stackoverflow.com/a/15181949
    return ((first == t) || ...);
}

QList<QPair<int, int>> splitElementsTapered(int elementCount, int segmentCount, int alignment, double peak);

[[nodiscard]] qreal standardWheelStepCount(QWheelEvent *event);

[[nodiscard]] QImage transparencyBackground(qreal devicePixelRatioF);

/** @internal
 *
 * @brief Two-dimensional array */
template<typename T>
class Array2D
{
public:
    /** @brief Constructor.
     *
     * Constructs an array with the size 0 × 0. */
    Array2D()
        : m_iCount(0)
        , m_jCount(0)
    {
    }

    /** @brief Constructor.
     *
     * @param iCount size (first dimension)
     * @param jCount size (second dimension)
     *
     * The elements are initialized with default-constructed values. */
    Array2D(QListSizeType iCount, QListSizeType jCount)
        : m_iCount(iCount)
        , m_jCount(jCount)
    {
        if (m_iCount < 0) {
            m_iCount = 0;
        }
        if (m_jCount < 0) {
            m_jCount = 0;
        }
        const auto elementCount = m_iCount * m_jCount;
        m_data.reserve(elementCount);
        for (QListSizeType i = 0; i < elementCount; ++i) {
            m_data.append(T());
        }
    }

    /** @brief Constructor.
     *
     * @param iCount size (first dimension)
     * @param jCount size (second dimension)
     * @param init Initial values. Excess elements are ignored. Missing
     *        elements are initialized with default-constructed values. */
    Array2D(QListSizeType iCount, QListSizeType jCount, QList<T> init)
        : m_iCount(iCount)
        , m_jCount(jCount)
    {
        if (m_iCount < 0) {
            m_iCount = 0;
        }
        if (m_jCount < 0) {
            m_jCount = 0;
        }
        const auto elementCount = m_iCount * m_jCount;
        m_data.reserve(elementCount);
        for (QListSizeType i = 0; i < elementCount; ++i) {
            if (i < init.count()) {
                m_data.append(init.value(i));
            } else {
                m_data.append(T());
            }
        }
    }

    // Define also these functions recommended by “rule of five”:
    /** @brief Default copy assignment operator
     * @param other the object to copy
     * @returns The default implementation’s return value. */
    Array2D &operator=(const Array2D &other) = default; // clazy:exclude=function-args-by-value
    /** @brief Default move assignment operator
     *
     * @param other the object to move-assign
     *
     * @returns The default implementation’s return value. */
    Array2D &operator=(Array2D &&other) = default;
    /** @brief Default copy constructor
     * @param other the object to copy */
    Array2D(const Array2D &other) = default;
    /** @brief Default move constructor
     * @param other the object to move */
    Array2D(Array2D &&other) = default;

    /** @brief Equal operator
     *
     * @param other The object to compare with.
     *
     * @returns <tt>true</tt> if equal, <tt>false</tt> otherwise. */
    bool operator==(const Array2D &other) const
    {
        return ( //
            (m_data == other.m_data) //
            && (m_iCount == other.m_iCount) //
            && (m_jCount == other.m_jCount) //
        );
    }

    /** @brief Unequal operator
     *
     * @param other The object to compare with.
     *
     * @returns <tt>true</tt> if unequal, <tt>false</tt> otherwise. */
    bool operator!=(const Array2D &other) const
    {
        return !(*this == other);
    }

    /** @brief If a given indices combination is in range.
     *
     * @param i index (first dimension)
     * @param j index (second dimension)
     * @returns If the indices combination is in range.
     */
    bool isInRange(QListSizeType i, QListSizeType j) const
    {
        const bool iOkay = //
            PerceptualColor::isInRange<QListSizeType>(0, i, m_iCount - 1);
        const bool jOkay = //
            PerceptualColor::isInRange<QListSizeType>(0, j, m_jCount - 1);
        return (iOkay && jOkay);
    }

    /** @brief Set value at a given index.
     *
     * @param i index (first dimension)
     * @param j index (second dimension)
     * @param value value to set */
    void setValue(QListSizeType i, QListSizeType j, const T &value)
    {
        if (isInRange(i, j)) {
            m_data[i + m_iCount * j] = value;
        }
    }

    /**
     * @brief Converts the array to a QList.
     *
     * @return Converts the array to a QList. */
    // Disable cppcheck's returnByReference warning.
    // Note: cppcheck is giving bad advice here. Returning by (const)
    // reference can lead to unexpected behavior: the underlying data might
    // be modified later by the original owner, making it a risky practice.
    // Anyway, we return a QList. Since QList uses implicit sharing,
    // creating a copy from it is efficient and inexpensive.
    // cppcheck-suppress returnByReference
    QList<T> toQList() const
    {
        return m_data;
    }

    /** @brief Get value at a given index.
     *
     * @param i index (first dimension)
     * @param j index (second dimension)
     * @returns If the indices are valid, the value at the given indeces.
     * A default-constructed value otherwise. */
    T value(QListSizeType i, QListSizeType j) const
    {
        if (isInRange(i, j)) {
            return m_data.at(i + m_iCount * j);
        }
        return T(); // Default value if indices are out of bounds
    }

    /** @brief Size of the first dimension.
     *
     * @returns Size of the first dimension. */
    QListSizeType iCount() const
    {
        return m_iCount;
    }

    /** @brief Size of the second dimension.
     *
     * @returns Size of the second dimension. */
    QListSizeType jCount() const
    {
        return m_jCount;
    }

private:
    /** @brief Internal storage of the elements. */
    QList<T> m_data;
    /** @brief Internal storage of @ref iCount(). */
    QListSizeType m_iCount;
    /** @brief Internal storage of @ref jCount(). */
    QListSizeType m_jCount;
};

/** @brief Swatches organized in a grid.
 *
 * This type is declared as type to Qt’s type system via
 * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
 * example if you want to use for <em>queued</em> signal-slot connections),
 * you might consider calling <tt>qRegisterMetaType()</tt> for
 * this type, once you have a QApplication object. */
using QColorArray2D = Array2D<QColor>;

QColor toOpaque(const QColor &color);

QColorArray2D toOpaque(const QColorArray2D &array);

/** @internal
 *
 * @brief Force processing of events in a delayed fashion.
 *
 * When there is no running event loop (like in unit tests or in tools
 * like the screenshot generator), some parts of the asynchronous API
 * of this library does not work. Calling this function fixes this by
 * forcing the processing of pending events, but with some delay
 * in-between, so that maybe existing parallel threads have also
 * a chance to terminate their work.
 *
 * @param msecWaitInitially Delay before starting event processing.
 * @param msecWaitBetweenEventLoopPasses Delay before each pass through
 *        through the pending events.
 * @param numberEventLoopPasses Number of passes through the pending events.
 *
 * @internal
 *
 * @note This is declared as template to prevent that this code is compiled
 * into the library itself, which does <em>not</em> actually use it itself,
 * but includes this header file. */
template<typename T = void>
void delayedEventProcessing(unsigned long msecWaitInitially = 50, unsigned long msecWaitBetweenEventLoopPasses = 50, int numberEventLoopPasses = 3)
{
    // Some OSes might round the sleep time up to 15 ms. We do it ourself
    // here to make the behaviour a little bit more predictable.
    msecWaitInitially = qMax<unsigned long>( //
        msecWaitInitially, //
        15);
    msecWaitBetweenEventLoopPasses = //
        qMax<unsigned long>(msecWaitBetweenEventLoopPasses, 15);

    QThread::msleep(msecWaitInitially);
    // Hopefully, now the render function has terminated…
    for (int i = 0; i < numberEventLoopPasses; ++i) {
        // Wait again (apparently, threaded event processing needs some time…)
        QThread::msleep(msecWaitBetweenEventLoopPasses);
        QCoreApplication::processEvents();
    }
}

[[nodiscard]] QColorArray2D wcsBasicColors(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace);

[[nodiscard]] QIcon qIconFromTheme(const QStringList &names, const QString &fallback, ColorSchemeType type);

[[nodiscard]] QPair<QString, QString> getPrefixSuffix(const QString &formatString);

/** @brief The full-qualified C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * @tparam T The enumeration.
 *
 * @pre The enumeration type is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * @returns The full-qualified C++ identifier as QString. */
template<typename T>
[[nodiscard]] QString enumerationToFullString()
{
    const auto myMeta = QMetaEnum::fromType<T>();
    const auto scope = QString::fromUtf8(myMeta.scope());
    const auto name = QString::fromUtf8(myMeta.name());
    return QStringLiteral("%1::%2").arg(scope, name);
}

/** @brief The full-qualified C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * @tparam T The enumeration type. Can usually be omitted.
 *
 * @param enumerator An enumerator.
 *
 * @pre The enumeration type of the enumerator is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * @returns The full-qualified C++ identifier as QString, followed by the
 * underlying integer value in parenthesis. If the enumerator does not
 * exist (for example because you have done a static_cast of an invalid
 * integer to  the enum class), an empty String is returned instead. If
 * the enumerator has synonyms (that means, there exist other enumerators
 * that share the same integer with the current enumerator), all synonym
 * enumerators are returned.
 *
 * @sa @ref enumeratorToString() */
template<typename T>
[[nodiscard]] QString enumeratorToFullString(const T &enumerator)
{
    const auto value = static_cast<int>(enumerator);
    const auto myMeta = QMetaEnum::fromType<T>();

    // QMetaEnum::valueToKeys (identifier with a final s) returns all existing
    // (synonym) keys for a given value. But it also returns happily
    // fantasy strings for non-existing values. Therefore, we have check
    // first with QMetaEnum::valueToKeys (identifier with a final s) which
    // does only return one single key for each value, but is guaranteed to
    // return nullptr if the value has no key.
    if (!myMeta.valueToKey(value)) {
        return QString();
    }

    const auto scope = QString::fromUtf8(myMeta.scope());
    const auto name = QString::fromUtf8(myMeta.name());
    const auto keys = QString::fromUtf8(myMeta.valueToKeys(value));
    return QStringLiteral("%1::%2::%3(%4)").arg(scope, name, keys).arg(value);
}

/** @brief The C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * @tparam T The enumeration type. Can usually be omitted.
 *
 * @param enumerator An enumerator.
 *
 * @pre The enumeration type of the enumerator is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * @returns The C++ identifier as QString, followed by the
 * underlying integer value in parenthesis. If the enumerator does not
 * exist (for example because you have done a static_cast of an invalid
 * integer to  the enum class), an empty String is returned instead. If
 * the enumerator has synonyms (that means, there exist other enumerators
 * that share the same integer with the current enumerator), all synonym
 * enumerators are returned.
 *
 * @sa @ref enumeratorToFullString() */
template<typename T>
[[nodiscard]] QString enumeratorToString(const T &enumerator)
{
    const auto value = static_cast<int>(enumerator);
    const auto myMeta = QMetaEnum::fromType<T>();

    // QMetaEnum::valueToKeys (identifier with a final s) returns all existing
    // (synonym) keys for a given value. But it also returns happily
    // fantasy strings for non-existing values. Therefore, we have check
    // first with QMetaEnum::valueToKeys (identifier with a final s) which
    // does only return one single key for each value, but is guaranteed to
    // return nullptr if the value has no key.
    if (!myMeta.valueToKey(value)) {
        return QString();
    }

    const auto keys = QString::fromUtf8(myMeta.valueToKeys(value));
    return QStringLiteral("%1(%2)").arg(keys).arg(value);
}

/**
 * @brief Splits a given number of elements into equal segments.
 *
 * This function divides <tt>elementCount</tt> elements (indices from
 * <tt>[0..(elementCount - 1)]</tt>) into <tt>segmentCount</tt> segments,
 * ensuring that each segment has a start and end index. If
 * <tt>segmentCount</tt> is greater than <tt>elementCount</tt>, fewer segments
 * will be created.
 *
 * @tparam T Numeric type of the elements (e.g., int, long, size_t).
 * @param elementCount Total number of elements. Should be ≥ 1.
 * @param segmentCount Number of segments to split into. Should be ≥ 1.
 * @return A list of pairs representing the start and end indices of each
 * segment.
 */
template<typename T>
QList<QPair<T, T>> splitElements(T elementCount, T segmentCount)
{
    static_assert(std::is_integral_v<T>, //
                  "Template splitElements() only works with integer types.");

    if (elementCount <= 0) {
        return {}; // Return empty list if elements are available
    }
    if (segmentCount <= 1) {
        segmentCount = 1;
    }
    if (segmentCount > elementCount) {
        segmentCount = elementCount; // Restrict segments to available elements
    }

    QList<QPair<T, T>> result;
    T baseSize = elementCount / segmentCount;
    // baseSize is guaranteed to be ≥ 1! Because elementCount ≥ segmentCount!
    T remainder = elementCount % segmentCount;

    T start = 0;
    for (T i = 0; i < segmentCount; ++i) {
        T end = start + baseSize - 1;
        if (i < remainder) {
            end++; // Distribute remaining elements evenly
        }

        result.append(QPair<T, T>{start, end});
        start = end + 1;
    }

    return result;
}

/**
 * @brief Splits a <tt>QList&lt;T&gt;</tt> into a specified number of parts.
 *
 * This function divides the elements of a <tt>QList&lt;T&gt;</tt> as evenly as
 * possible into a given number of sublists. If <tt>numParts</tt> is greater
 * than <tt>originalList.count()</tt>, fewer parts will be created.
 *
 * @tparam T The type of elements in the list.
 * @param originalList The original <tt>QList&lt;T&gt;</tt> to be split.
 * @param numParts The desired number of parts. Should be ≥ 1.
 * @return <tt>QList&lt;QList&lt;T&gt;&gt;</tt> A list of sublists containing
 * the divided data.
 */
template<typename T>
QList<QList<T>> splitList(const QList<T> &originalList, QListSizeType numParts)
{
    if (originalList.isEmpty()) {
        return {};
    }

    QList<QList<T>> result;
    const auto segments = splitElements(originalList.count(), numParts);
    for (const auto segment : segments) {
        result.append( //
            originalList.mid(segment.first, //
                             segment.second - segment.first + 1));
    }
    return result;
}

} // namespace PerceptualColor

// Use Q_DECLARE_METATYPE with this data type.
// Attention: This must be done outside of all name-spaces.
Q_DECLARE_METATYPE(PerceptualColor::QColorArray2D)

#endif // HELPER_H
