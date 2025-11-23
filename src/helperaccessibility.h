// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_HELPERACCESSIBILITY_H
#define PERCEPTUALCOLOR_HELPERACCESSIBILITY_H

#include <qaccessible.h>
#include <qstring.h>
#include <type_traits>
class QObject;

/** @internal
 *
 * @file
 *
 * Provides accessibility-related features. */

namespace PerceptualColor
{

/**
 * @internal
 *
 * @brief Factory function template for creating accessible interfaces.
 *
 * This template provides a factory function compatible with both the
 * signature of <tt>QAccessiblePlugin::create()</tt> and the <tt>typedef
 * QAccessible::InterfaceFactory</tt>. It can be directly passed to
 * <tt>QAccessible::installFactory()</tt>.
 *
 * @tparam T The class for which an accessible interface is provided by
 *         the factory. It must have the Q_OBJECT macro.
 * @tparam AccessibleT The QAccessibleInterface implementation corresponding
 *         to <tt>T</tt>. It must have a constructor accepting as
 *         parameter a pointer to the target object.
 *
 * @param classname The class name of the object for which an interface is
 *        requested.
 * @param object The object for which an interface is requested.
 *
 * @return A pointer to a new instance of <tt>AccessibleT</tt> if the
 *         <tt>classname</tt> matches <tt>T</tt> and the <tt>object</tt>
 *         can be cast to <tt>T</tt>; otherwise, returns <tt>nullptr</tt>.
 *
 * @sa @ref installAccessibleFactory
 */
template<typename T, typename AccessibleT>
QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
{
    static_assert(std::is_base_of<QObject, T>::value, //
                  "T must be a subclass of QObject");
    static_assert(std::is_base_of<QAccessibleInterface, AccessibleT>::value, //
                  "AccessibleT must be a subclass of QAccessibleInterface");

    const QString tClassname = QString::fromUtf8(
        // className() returns const char *. Its encoding is not documented.
        // Hopefully, as we use UTF8  in this library as “input character set”
        // and also as “Narrow execution character set”, the encoding
        // might be also UTF8…
        T::staticMetaObject.className());
    // Unlike dynamic_cast<>(), qobject_cast<>() performs a static_assert to
    // ensure that the target type includes the Q_OBJECT macro. This is helpful
    // here because it guarantees, at compile time, that the type supports
    // Qt’s meta-object system. Without Q_OBJECT, QMetaObject::className()
    // — which we also rely on in this function — returns incorrect results.
    T *typedObject = qobject_cast<T *>(object);
    if ((classname == tClassname) && typedObject) {
        return new AccessibleT(typedObject);
    }

    return nullptr;
}

/**
 * @internal
 *
 * @brief Defines and installs an accessible interface factory using
 * <tt>QAccessible::installFactory()</tt>.
 *
 * @tparam T The class for which an accessible interface is provided by
 *         the factory. It must have the Q_OBJECT macro.
 * @tparam AccessibleT The QAccessibleInterface implementation corresponding
 *         to <tt>T</tt>. It must have a constructor accepting as
 *         parameter a pointer to the target object.
 *
 * @note This function is idempotent and can be safely invoked multiple times
 * without adverse effects.
 *
 * @sa @ref accessibleFactory
 */
template<typename T, typename AccessibleT>
void installAccessibleFactory()
{
    static_assert(std::is_base_of<QObject, T>::value, //
                  "T must be a subclass of QObject");
    static_assert(std::is_base_of<QAccessibleInterface, AccessibleT>::value, //
                  "AccessibleT must be a subclass of QAccessibleInterface");

    // It’s safe to call installFactory() multiple times with the
    // same factory. If the factory is yet installed, it will not
    // be installed again. However, for performance reasons, we check
    // with a static variable.
    static bool isInstalled = false;
    if (isInstalled) {
        return;
    }
    QAccessible::installFactory(accessibleFactory<T, AccessibleT>);
    isInstalled = true;
}

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_HELPERACCESSIBILITY_H
