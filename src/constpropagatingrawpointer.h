// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CONSTPROPAGATINGRAWPOINTER_H
#define PERCEPTUALCOLOR_CONSTPROPAGATINGRAWPOINTER_H

namespace PerceptualColor
{
/** @internal
 *
 *  @brief A <tt>const</tt> propagating raw pointer
 *
 * With normal raw C++ pointers (and also with <tt>std::unique_ptr</tt>
 * pointers), within <tt>const</tt> functions you can do non-const
 * operations <em>on objects that a pointer points to</em>.
 *
 * <em>This</em> pointer type is different: It propagates the const-ness of
 * the object members and propagates them to the call through the pointer;
 * it will trigger a compiler error if non-const access to object members
 * or methods is done from within const functions. Apart from that, it
 * behaves similar to raw pointers. For compatibility with raw pointers,
 * it also casts implicitly to the corresponding raw pointer (but only
 * within non-<tt>const</tt> contexts).
 *
 * Example code:
 * @snippet testconstpropagatingrawpointer.cpp example
 *
 * @note Currently, this class does not implement assignment operators
 * or assignment constructors. Once constructed, an instance of
 * this class cannot be changed anymore.
 *
 * @note It also propagates the const-ness of the
 * @ref ConstPropagatingRawPointer pointer variable itself. Consequently
 * <em>all</em> access to non-const functions is impossible if the pointer
 * variable itself is <tt>const</tt>.
 *
 * @sa @ref ConstPropagatingUniquePointer
 *
 * @note There exist very sophisticated implementations like
 * https://github.com/jbcoe/propagate_const but we use nevertheless
 * our own light-wise implementation because this seems to be enough
 * for our limited use case.
 */
template<typename T>
class ConstPropagatingRawPointer
{
public:
    /** @brief Default constructor
     *
     * Creates a pointer that points to <tt>nullptr</tt>. */
    explicit ConstPropagatingRawPointer()
        : m_pointer(nullptr)
    {
    }

    /** @brief Constructor
     *
     * @param pointer Object to which to point */
    explicit ConstPropagatingRawPointer(T *pointer)
        : m_pointer(pointer)
    {
    }

    /** @brief Default destructor */
    ~ConstPropagatingRawPointer() noexcept = default;

    /** @brief Non-const pointer operator
     *
     * @returns Non-const pointer operator */
    [[nodiscard]] T *operator->()
    {
        return m_pointer;
    }

    /** @brief Const pointer operator
     *
     * @returns Const pointer */
    [[nodiscard]] const T *operator->() const
    {
        return m_pointer;
    }

    /** @brief Non-const dereference operator
     *
     * @returns Non-const dereference operator */
    [[nodiscard]] T &operator*()
    {
        return *m_pointer;
    }

    /** @brief Const dereference operator
     *
     * @returns Const dereference operator */
    [[nodiscard]] const T &operator*() const
    {
        return *m_pointer;
    }

    /** @brief Cast to a normal raw pointer.
     *
     * @sa @ref toPointerToConstObject() */
    [[nodiscard]] operator T *()
    {
        return m_pointer;
    }

    /** @brief Cast to a normal raw pointer that points to a const object.
     *
     * @returns Cast to a normal raw pointer that points to a const object.
     *
     * @sa @ref operator T *() */
    // We could provide a const overload to operator T*() instead of
    // providing this function with a strange name. But overloading
    // operator T* leads to compile errors because of ambiguity
    // on GCC and Clang when using this pointer within a “delete” statement.
    // That’s confusing, therefore we should avoid it. By giving this
    // function a different name prevents automatic casts in non-const
    // objects, which also forbids deleting directly pointers of this
    // type in non-const contexts, which is nice.
    [[nodiscard]] const T *toPointerToConstObject() const
    {
        return m_pointer;
    }

private:
    /** @brief Internal storage for the pointer */
    T *m_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CONSTPROPAGATINGUNIQUEPOINTER_H
