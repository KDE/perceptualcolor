// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_HELPERACCESSIBILITY_H
#define PERCEPTUALCOLOR_HELPERACCESSIBILITY_H

#include <qaccessible.h>
#include <qstring.h>
#include <type_traits>
class QObject;

namespace PerceptualColor
{

/*!
 *
 *
 * \fn template<typename T, typename AccessibleT> QAccessibleInterface *accessibleFactory(const QString &classname, QObject *object)
 *
 * \brief Factory function template for creating accessible interfaces.
 *
 * This template provides a factory function compatible with both the
 * signature of \c {QAccessiblePlugin::create()} and the \c {typedef
 * QAccessible::InterfaceFactory}. It can be directly passed to
 * \c {QAccessible::installFactory()}.
 *
 * \b T The class for which an accessible interface is provided by
 *         the factory. It must have the Q_OBJECT macro.
 *
 * \b AccessibleT The QAccessibleInterface implementation corresponding
 *         to \c {T}. It must have a constructor accepting as
 *         parameter a pointer to the target object.
 *
 * \a classname The class name of the object for which an interface is
 *        requested.
 *
 * \a object The object for which an interface is requested.
 *
 * Returns a pointer to a new instance of \c {AccessibleT} if
 * the \c {classname} matches \c {T} and the \c {object} can
 * be cast to \c {T}; otherwise, returns \c {nullptr}.
 *
 * \sa installAccessibleFactory
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

/*!
 *
 *
 * \fn template<typename T, typename AccessibleT> void installAccessibleFactory()
 *
 * \brief Defines and installs an accessible interface factory
 * using \c {QAccessible::installFactory()}.
 *
 * \b T The class for which an accessible interface is provided by
 *         the factory. It must have the Q_OBJECT macro.
 *
 * \b AccessibleT The QAccessibleInterface implementation corresponding
 *         to \c {T}. It must have a constructor accepting as
 *         parameter a pointer to the target object.
 *
 * \note This function is idempotent and can be safely invoked multiple times
 * without adverse effects.
 *
 * \sa accessibleFactory
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
