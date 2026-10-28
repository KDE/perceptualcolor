// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_HELPER_H
#define PERCEPTUALCOLOR_HELPER_H

#include "helpermath.h"
#include "logging.h"
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qguiapplication.h>
#include <qicon.h>
#include <qimage.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qmetaobject.h>
#include <qmetatype.h>
#include <qmimedata.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpair.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qthread.h>
#include <type_traits>

class QApplication;
class QWheelEvent;
class QWidget;

namespace PerceptualColor
{

[[nodiscard]] QString fromMnemonicToRichText(const QString &mnemonicText);

[[nodiscard]] bool isDarkColorScheme();

/*!
 *
 *
 * \brief Convenience function template that tests if a value is in a list.
 *
 * \a first The value
 *
 * \a t The list
 *
 * Usage:
 *
 * \snippet testhelper.cpp isInUsage
 *
 * Returns \c {true} if “value” is in “list”. \c {false} otherwise. */
template<typename First, typename... T>
bool isIn(First &&first, T &&...t)
{
    // Solution as proposed by Nikos C. in https://stackoverflow.com/a/15181949
    return ((first == t) || ...);
}

QList<QPair<int, int>> splitElementsTapered(int elementCount, int segmentCount, int alignment, double peak);

[[nodiscard]] qreal standardWheelStepCount(QWheelEvent *event);

[[nodiscard]] QColor toQColor(const QMimeData *mimeData);

[[nodiscard]] QImage transparencyBackground(QSize physicalImageSize, qreal devicePixelRatioF);

/*!
 * \inmodule ModulePrivateApi
 *
 * \class PerceptualColor::Array2D
 *
 * \brief Two-dimensional array.
 */
template<typename T>
class Array2D
{
public:
    /*!
     * \brief Constructor.
     *
     * Constructs an array with the size 0 × 0. */
    Array2D()
        : m_iCount(0)
        , m_jCount(0)
    {
    }

    /*!
     * \brief Constructor.
     *
     * \a iCount size (first dimension)
     *
     * \a jCount size (second dimension)
     *
     * The elements are initialized with default-constructed values. */
    Array2D(qsizetype iCount, qsizetype jCount)
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
        for (qsizetype i = 0; i < elementCount; ++i) {
            m_data.append(T());
        }
    }

    /*!
     * \brief Constructor.
     *
     * \a iCount size (first dimension)
     *
     * \a jCount size (second dimension)
     *
     * \a init Initial values. Excess elements are ignored. Missing
     *        elements are initialized with default-constructed values. */
    Array2D(qsizetype iCount, qsizetype jCount, QList<T> init)
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
        for (qsizetype i = 0; i < elementCount; ++i) {
            if (i < init.size()) {
                m_data.append(init.value(i));
            } else {
                m_data.append(T());
            }
        }
    }

    // Define also these functions recommended by “rule of five”:
    /*!
     * \brief Default copy assignment operator
     *
     * \a other the object to copy
     *
     * Returns the default implementation’s return value.
     */
    Array2D &operator=(const Array2D &other) = default; // clazy:exclude=function-args-by-value
    /*!
     * \brief Default move assignment operator
     *
     * \a other the object to move-assign
     *
     * Returns the default implementation’s return value.
     */
    Array2D &operator=(Array2D &&other) = default;
    /*!
     * \brief Default copy constructor
     *
     * \a other the object to copy
     */
    Array2D(const Array2D &other) = default;
    /*!
     * \brief Default move constructor
     *
     * \a other the object to move
     */
    Array2D(Array2D &&other) = default;

    /*!
     * \brief Equal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if equal, \c {false} otherwise. */
    [[nodiscard]] bool operator==(const Array2D &other) const = default;

    /*!
     * \brief Unequal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if unequal, \c {false} otherwise. */
    [[nodiscard]] bool operator!=(const Array2D &other) const = default;

    /*!
     * \brief Returns if a given index combination is in range.
     *
     * \a i index (first dimension)
     *
     * \a j index (second dimension)
     */
    [[nodiscard]] bool isInRange(qsizetype i, qsizetype j) const
    {
        const bool iOkay = //
            PerceptualColor::isInRange<qsizetype>(0, i, m_iCount - 1);
        const bool jOkay = //
            PerceptualColor::isInRange<qsizetype>(0, j, m_jCount - 1);
        return (iOkay && jOkay);
    }

    /*!
     * \brief Set value at a given index.
     *
     * \a i index (first dimension)
     *
     * \a j index (second dimension)
     *
     * \a newValue value to set
     */
    void setValue(qsizetype i, qsizetype j, const T &newValue)
    {
        if (isInRange(i, j)) {
            m_data[i + m_iCount * j] = newValue;
        }
    }

    /*!
     * \brief Converts the array to a QList.
     */
    [[nodiscard]] QList<T> toQList() const
    {
        return m_data;
    }

    /*!
     * \brief Get value at a given index.
     *
     * \a i index (first dimension)
     *
     * \a j index (second dimension)
     *
     * If the indices are valid, it returns the value at the given indeces.
     * Otherwise, it returns a default-constructed value.
     */
    [[nodiscard]] T value(qsizetype i, qsizetype j) const
    {
        if (isInRange(i, j)) {
            return m_data.at(i + m_iCount * j);
        }
        return T(); // Default value if indices are out of bounds
    }

    /*!
     * \brief Size of the first dimension.
     */
    [[nodiscard]] qsizetype iCount() const
    {
        return m_iCount;
    }

    /*!
     * \brief Size of the second dimension.
     */
    [[nodiscard]] qsizetype jCount() const
    {
        return m_jCount;
    }

private:
    /*!
     * \brief Internal storage of the elements. */
    QList<T> m_data;
    /*!
     * \brief Internal storage of \l iCount(). */
    qsizetype m_iCount;
    /*!
     * \brief Internal storage of \l jCount(). */
    qsizetype m_jCount;
};

/*!
 * \typealias PerceptualColor::QColorArray2D
 *
 * \relates PerceptualColor::Array2D
 *
 * \brief Swatches organized in a grid.
 *
 * This type is declared as type to Qt’s type system via
 * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
 * example if you want to use for \e {queued} signal-slot connections),
 * you might consider calling \c {qRegisterMetaType()} for
 * this type, once you have a QApplication object. */
using QColorArray2D = Array2D<QColor>;

[[nodiscard]] QColor toOpaque(const QColor &color);

[[nodiscard]] QColorArray2D toOpaque(const QColorArray2D &array);

/*!
 *
 *
 * \fn template<typename T = void> void delayedEventProcessing(unsigned long msecWaitInitially = 50, unsigned long msecWaitBetweenEventLoopPasses = 50, int
 * numberEventLoopPasses = 3)
 *
 * \brief Force processing of events in a delayed fashion.
 *
 * When there is no running event loop (like in unit tests or in tools
 * like the screenshot generator), some parts of the asynchronous API
 * of this library does not work. Calling this function fixes this by
 * forcing the processing of pending events, but with some delay
 * in-between, so that maybe existing parallel threads have also
 * a chance to terminate their work.
 *
 * \a msecWaitInitially Delay before starting event processing.
 *
 * \a msecWaitBetweenEventLoopPasses Delay before each pass through
 *        through the pending events.
 *
 * \a numberEventLoopPasses Number of passes through the pending events.
 *
 *
 *
 * \note This is declared as template to prevent that this code is compiled
 * into the library itself, which does \e {not} actually use it itself,
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

/*!
 *
 *
 * \fn template<typename T = void> [[nodiscard]] bool onWayland()
 *
 * \brief Determines whether the current session is running under the
 * Wayland Window System.
 *
 * \b Precondition: A valid QApplication instance must be available
 * to perform detection.
 *
 * Returns \c {true} if Wayland is detected as the active window system.
 * Returns \c {false} if another window system is detected.
 */
template<typename T = void>
[[nodiscard]] bool onWayland()
{
    QApplication *app = //
        qobject_cast<QApplication *>(QCoreApplication::instance());
    if (app == nullptr) {
        qCWarning(logging) << "onWayland() called, but no QApplication exists.";
    }
    const QString platform = QGuiApplication::platformName();
    return platform.contains(QStringLiteral("wayland"), Qt::CaseInsensitive);
}

/*!
 *
 *
 * \fn template<typename T = void> [[nodiscard]] bool onX()
 *
 * \brief Determines whether the current session is running under the
 * X Window System.
 *
 * \b Precondition: A valid QApplication instance must be available
 * to perform detection.
 *
 * Returns \c {true} if X is detected as the active window system.
 * Returns \c {false} if another window system is detected.
 */
template<typename T = void>
[[nodiscard]] bool onX()
{
    QApplication *app = //
        qobject_cast<QApplication *>(QCoreApplication::instance());
    if (app == nullptr) {
        qCWarning(logging) << "onX() called, but no QApplication exists.";
    }
    const QString platform = QGuiApplication::platformName();
    return platform.contains(QStringLiteral("xcb"), Qt::CaseInsensitive);
}

[[nodiscard]] QColorArray2D wcsBasicColors();

[[nodiscard]] QIcon qIconFromTheme(const QStringList &names, const QString &fallback);

[[nodiscard]] QIcon qIconFromTheme(const QIcon::ThemeIcon nativeIcon, const QStringList &names, const QString &fallback);

[[nodiscard]] QPair<QString, QString> getPrefixSuffix(const QString &formatString);

/*!
 *
 *
 * \fn template<typename T> [[nodiscard]] QString enumerationToFullString()
 *
 * \brief The full-qualified C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * \b T The enumeration.
 *
 * \b Precondition: The enumeration type is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * Returns the full-qualified C++ identifier as QString. */
template<typename T>
[[nodiscard]] QString enumerationToFullString()
{
    static_assert( //
        std::is_enum_v<T>, //
        "Template enumerationToFullString() only works with enumeration types.");

    const QMetaEnum myMeta = QMetaEnum::fromType<T>();
    const QString scope = QString::fromUtf8(myMeta.scope());
    const QString name = QString::fromUtf8(myMeta.name());
    return QStringLiteral("%1::%2").arg(scope, name);
}

/*!
 *
 *
 * \fn template<typename T> [[nodiscard]] QString enumeratorToFullString(const T &enumerator)
 *
 * \brief The full-qualified C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * \b T The enumeration type. Can usually be omitted.
 *
 * \a enumerator An enumerator.
 *
 * \b Precondition: The enumeration type of the enumerator is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * Returns the full-qualified C++ identifier as QString, followed by the
 * underlying integer value in parenthesis. If the enumerator does not
 * exist (for example because you have done a static_cast of an invalid
 * integer to  the enum class), an empty String is returned instead. If
 * the enumerator has synonyms (that means, there exist other enumerators
 * that share the same integer with the current enumerator), all synonym
 * enumerators are returned.
 *
 * \sa enumeratorToString()
 */
template<typename T>
[[nodiscard]] QString enumeratorToFullString(const T &enumerator)
{
    static_assert( //
        std::is_enum_v<T>, //
        "Template enumeratorToFullString() only works with enumeration types.");
    static_assert( //
        sizeof(T) <= sizeof(int), //
        "Template enumeratorToFullString(): Underlying type of "
        "enumeration must not exceed the size of 'int'."
        // signed/unsigned does not matter for QMetaEnum!
    );

    const auto value = static_cast<quint64>(enumerator);
    const QMetaEnum myMeta = QMetaEnum::fromType<T>();

    // QMetaEnum::valueToKeys (identifier with a final s) returns all existing
    // (synonym) keys for a given value. But it also returns happily
    // fantasy strings for non-existing values. Therefore, we have check
    // first with QMetaEnum::valueToKeys (identifier with a final s) which
    // does only return one single key for each value, but is guaranteed to
    // return nullptr if the value has no key.
    if (!myMeta.valueToKey(value)) {
        return QString();
    }

    const QString scope = QString::fromUtf8(myMeta.scope());
    const QString name = QString::fromUtf8(myMeta.name());
    const QString keys = QString::fromUtf8(myMeta.valueToKeys(value));
    return QStringLiteral("%1::%2::%3(%4)").arg(scope, name, keys).arg(value);
}

/*!
 *
 *
 * \fn template<typename T> [[nodiscard]] QString enumeratorToString(const T &enumerator)
 *
 * \brief The C++ identifier as QString.
 *
 * This can be useful for debugging purposes.
 *
 * \b T The enumeration type. Can usually be omitted.
 *
 * \a enumerator An enumerator.
 *
 * \b Precondition: The enumeration type of the enumerator is declared with
 * Q_ENUM or Q_ENUM_NS.
 *
 * Returns the C++ identifier as QString, followed by the
 * underlying integer value in parenthesis. If the enumerator does not
 * exist (for example because you have done a static_cast of an invalid
 * integer to  the enum class), an empty String is returned instead. If
 * the enumerator has synonyms (that means, there exist other enumerators
 * that share the same integer with the current enumerator), all synonym
 * enumerators are returned.
 *
 * \sa enumeratorToFullString()
 */
template<typename T>
[[nodiscard]] QString enumeratorToString(const T &enumerator)
{
    static_assert( //
        std::is_enum_v<T>, //
        "Template enumeratorToString() only works with enumeration types.");
    static_assert( //
        sizeof(T) <= sizeof(int), //
        "Template enumeratorToFullString(): Underlying type of "
        "enumeration must not exceed the size of 'int'."
        // signed/unsigned does not matter for QMetaEnum!
    );

    const auto value = static_cast<quint64>(enumerator);
    const QMetaEnum myMeta = QMetaEnum::fromType<T>();

    // QMetaEnum::valueToKeys (identifier with a final s) returns all existing
    // (synonym) keys for a given value. But it also returns happily
    // fantasy strings for non-existing values. Therefore, we have check
    // first with QMetaEnum::valueToKeys (identifier with a final s) which
    // does only return one single key for each value, but is guaranteed to
    // return nullptr if the value has no key.
    if (!myMeta.valueToKey(value)) {
        return QString();
    }

    const QString keys = QString::fromUtf8(myMeta.valueToKeys(value));
    return QStringLiteral("%1(%2)").arg(keys).arg(value);
}

[[nodiscard]] bool isThemeChange(const QEvent::Type eventType);

/*!
 *
 *
 * \fn template<typename T> QList<QPair<T, T>> splitElements(T elementCount, T segmentCount)
 *
 * \brief Splits a given number of elements into equal segments.
 *
 * This function divides \c {elementCount} elements (indices from
 * \c {[0..(elementCount - 1)]}) into \c {segmentCount} segments,
 * ensuring that each segment has a start and end index. If
 * \c {segmentCount} is greater than \c {elementCount}, fewer segments
 * will be created.
 *
 * \b T Numeric type of the elements (e.g., int, long, size_t).
 *
 * \a elementCount Total number of elements. Should be ≥ 1.
 *
 * \a segmentCount Number of segments to split into. Should be ≥ 1.
 *
 * Returns a list of pairs representing the start and end indices of each
 * segment.
 */
template<typename T>
[[nodiscard]] QList<QPair<T, T>> splitElements(T elementCount, T segmentCount)
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

/*!
 *
 *
 * \fn template<typename T> QList<QList<T>> splitList(const QList<T> &originalList, qsizetype numParts)
 *
 * \brief Splits a \c {QList} into a specified number of parts.
 *
 * This function divides the elements of a \c {QList} as evenly as
 * possible into a given number of sublists. If \c {numParts} is greater
 * than \c {originalList.size()}, fewer parts will be created.
 *
 * \b T The type of elements in the list.
 *
 * \a originalList The original \c {QList} to be split.
 *
 * \a numParts The desired number of parts. Should be ≥ 1.
 *
 * Returns a list of sublists containing
 * the divided data.
 */
template<typename T>
[[nodiscard]] QList<QList<T>> splitList(const QList<T> &originalList, qsizetype numParts)
{
    if (originalList.isEmpty()) {
        return {};
    }

    QList<QList<T>> result;
    const auto segments = splitElements<qsizetype>(originalList.size(), //
                                                   numParts);
    for (const auto &segment : segments) {
        // cppcheck-suppress useStlAlgorithm
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

#endif // PERCEPTUALCOLOR_HELPER_H
