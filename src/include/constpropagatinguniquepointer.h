// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CONSTPROPAGATINGUNIQUEPOINTER_H
#define CONSTPROPAGATINGUNIQUEPOINTER_H

#include "importexport.h"

#include <memory>

namespace PerceptualColor
{
/** @internal
 *
 * @brief A <tt>const</tt> propagating <tt>std::unique_ptr</tt>
 *
 * With normal <tt>std::unique_ptr</tt> pointers (and also with raw
 * C++ pointers), within <tt>const</tt> functions you can do non-const
 * operations <em>on objects that a pointer points to</em>.
 *
 * <em>This</em> pointer type is different: It propagates the constness of
 * the object members and propagates them to the call through the pointer;
 * it will trigger a compiler error if non-cost access to object members
 * or methods is done from within const functions. Apart from that, it
 * behaves like <tt>std::unique_ptr</tt> (from which it inherits).
 *
 * Think of this template as a simple alternative to
 * <tt>std::experimental::propagate_const&lt; std::unique_ptr&lt;T&gt; &gt;</tt>
 *
 * Example code:
 * @snippet testconstpropagatinguniquepointer.cpp example
 *
 * Currently, move-assignment is not supported. Use
 * <tt>std::unique_prt::reset()</tt> instead. As there is no
 * support for deleters anyway, this should be equivalent.
 *
 * @note A @ref ConstPropagatingUniquePointer pointer variable itself
 * may not be const! @internal (Otherwise, this would make <tt>const</tt>
 * <em>all</em> access even to non-const functions of the pointed
 * object.) @endinternal
 *
 * @internal
 *
 * @sa @ref ConstPropagatingRawPointer
 * @sa This code is based on the idea in
 * <a href="http://torbjoernk.github.io/deep_const_ptr/">deep_const_ptr</a>
 * which is a more general implementation of this concept, that does not
 * provide a <tt>std::unique_ptr</tt> but acts as a wrapper template around
 * smart pointers in general. It lacks however the dereference operator.
 *
 * @note This class could be replaced in the future by <tt>
 * <a href="https://en.cppreference.com/w/cpp/experimental/propagate_const">
 * std::experimental::propagate_const</a></tt> if this one ever becomes
 * part of the C++ standard. (Experimental features however are optional
 * for compilers, so not all of them implement them. Furthermore, they
 * can still change. Therefore, we cannot use experimental features here.)
 *
 * @todo Would it be better to include (or link to)
 * https://github.com/jbcoe/propagate_const instead of having our own
 * implementation? Or remove propagate_const header from this library? */
template<typename T>
class PERCEPTUALCOLOR_IMPORTEXPORT ConstPropagatingUniquePointer : public std::unique_ptr<T>
{
public:
    /** @brief Default constructor
     *
     * Creates a pointer that points to <tt>nullptr</tt>. */
    explicit ConstPropagatingUniquePointer()
        : std::unique_ptr<T>(nullptr)
    {
    }

    /** @brief Constructor
     *
     * @param pointerToObject Object to which to point */
    explicit ConstPropagatingUniquePointer(typename std::unique_ptr<T>::element_type *pointerToObject)
        : std::unique_ptr<T>(pointerToObject)
    {
    }

    /** @brief Default destructor
     *
     * This destructor is not marked as <tt>override</tt> because the
     * base class’s destructor is not virtual.*/
    ~ConstPropagatingUniquePointer() noexcept = default;

    /** @brief Non-const pointer operator
     *
     * @returns Non-const pointer operator */
    [[nodiscard]] typename std::unique_ptr<T>::element_type *operator->()
    {
        return std::unique_ptr<T>::operator->();
    }

    /** @brief Const pointer operator
     *
     * @returns Const pointer */
    [[nodiscard]] const typename std::unique_ptr<T>::element_type *operator->() const
    {
        return std::unique_ptr<T>::operator->();
    }

    /** @brief Non-const dereference operator
     *
     * @returns Non-const dereference operator */
    [[nodiscard]] typename std::unique_ptr<T>::element_type &operator*()
    {
        return std::unique_ptr<T>::operator*();
    }

    /** @brief Const dereference operator
     *
     * @returns Const dereference operator */
    [[nodiscard]] const typename std::unique_ptr<T>::element_type &operator*() const
    {
        return std::unique_ptr<T>::operator*();
    }
};

} // namespace PerceptualColor

#endif // CONSTPROPAGATINGUNIQUEPOINTER_H
