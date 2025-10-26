// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef CONSTPROPAGATINGUNIQUEPOINTER_H
#define CONSTPROPAGATINGUNIQUEPOINTER_H

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
 * behaves like <tt>std::unique_ptr</tt>.
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
 *
 * This class inherits privately <tt>std::shared_ptr</tt> and not
 * <tt>std::unique_ptr</tt> because the latter will not compile on
 * the MSVC compiler when used with incomplete types, which is however
 * necessary for usage within @ref pimpl.
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
// NOTE No PERCEPTUALCOLOR_IMPORTEXPORT for generic template definitions!
template<typename T>
class ConstPropagatingUniquePointer : private std::shared_ptr<T>
{
public:
    /** @brief Default constructor
     *
     * Creates a pointer that points to <tt>nullptr</tt>. */
    explicit ConstPropagatingUniquePointer()
        : std::shared_ptr<T>(nullptr)
    {
    }

    /** @brief Constructor
     *
     * @param pointerToObject Object to which to point */
    explicit ConstPropagatingUniquePointer(T *pointerToObject)
        : std::shared_ptr<T>(pointerToObject)
    {
    }

    /** @brief Default destructor
     *
     * This destructor is not marked as <tt>override</tt> because the
     * base class’s destructor is not virtual.*/
    ~ConstPropagatingUniquePointer() noexcept = default;

    // No copy/move assignment or constructor.
    ConstPropagatingUniquePointer(const ConstPropagatingUniquePointer &) = delete;
    ConstPropagatingUniquePointer(ConstPropagatingUniquePointer &&) = delete;
    ConstPropagatingUniquePointer &operator=(const ConstPropagatingUniquePointer &) = delete; // clazy:exclude=function-args-by-value
    ConstPropagatingUniquePointer &operator=(ConstPropagatingUniquePointer &&) = delete;

    /** @brief Non-const pointer operator
     *
     * @returns Non-const pointer operator */
    [[nodiscard]] T *operator->()
    {
        // cppcheck-suppress CastIntegerToAddressAtReturn // false positive
        return std::shared_ptr<T>::operator->();
    }

    /** @brief Const pointer operator
     *
     * @returns Const pointer */
    [[nodiscard]] const T *operator->() const
    {
        // cppcheck-suppress CastIntegerToAddressAtReturn // false positive
        return std::shared_ptr<T>::operator->();
    }

    /** @brief Non-const dereference operator
     *
     * @returns Non-const dereference operator */
    [[nodiscard]] T &operator*()
    {
        // cppcheck-suppress returnTempReference // false positive
        return std::shared_ptr<T>::operator*();
    }

    /** @brief Const dereference operator
     *
     * @returns Const dereference operator */
    [[nodiscard]] const T &operator*() const
    {
        // cppcheck-suppress returnTempReference // false positive
        return std::shared_ptr<T>::operator*();
    }

    /** @brief Deletes the previously managed object (if any) and starts
     * to manage a new object.
     *
     * @param newObject The new object that will be managed. Can be
     * <tt>nullptr</tt> to not manage any object anymore. */
    void reset(T *newObject = nullptr)
    {
        std::shared_ptr<T>::reset(newObject);
    }

    /** @brief Swaps the managed objects.
     *
     * @param other Another @ref ConstPropagatingUniquePointer object to
     * swap the managed object with. */
    void swap(ConstPropagatingUniquePointer &other)
    {
        std::shared_ptr<T>::swap(other);
    }

    /** @brief Returns a pointer to the managed object or <tt>nullptr</tt> if
     * no object is owned.
     *
     * @returns A pointer to the managed object or <tt>nullptr</tt> if
     * no object is owned. */
    [[nodiscard]] T *get()
    {
        // cppcheck-suppress CastIntegerToAddressAtReturn // false positive
        return std::shared_ptr<T>::get();
    }
};

} // namespace PerceptualColor

#endif // CONSTPROPAGATINGUNIQUEPOINTER_H
