// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CONSTPROPAGATINGRAWPOINTER_H
#define PERCEPTUALCOLOR_CONSTPROPAGATINGRAWPOINTER_H

namespace PerceptualColor
{
/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \class PerceptualColor::ConstPropagatingRawPointer
 *
 * \brief A \c {const} propagating raw pointer.
 *
 * With normal raw C++ pointers (and also with \c {std::unique_ptr}
 * pointers), within \c {const} functions you can do non-const
 * operations \e {on objects that a pointer points to}.
 *
 * \e {This} pointer type is different: It propagates the const-ness of
 * the object members and propagates them to the call through the pointer;
 * it will trigger a compiler error if non-const access to object members
 * or methods is done from within const functions. Apart from that, it
 * behaves similar to raw pointers. For compatibility with raw pointers,
 * it also casts implicitly to the corresponding raw pointer (but only
 * within non-\c {const} contexts).
 *
 * Example code:
 *
 * \snippet testconstpropagatingrawpointer.cpp example
 *
 * \note Currently, this class does not implement assignment operators
 * or assignment constructors. Once constructed, an instance of
 * this class cannot be changed anymore.
 *
 * \note It also propagates the const-ness of the
 * \l ConstPropagatingRawPointer pointer variable itself. Consequently
 * \e {all} access to non-const functions is impossible if the pointer
 * variable itself is \c {const}.
 *
 * \sa ConstPropagatingUniquePointer
 *
 * \note There exist very sophisticated implementations like
 * https://github.com/jbcoe/propagate_const but we use nevertheless
 * our own light-wise implementation because this seems to be enough
 * for our limited use case.
 */
template<typename T>
class ConstPropagatingRawPointer
{
public:
    /*!
     * \brief Default constructor
     *
     * Creates a pointer that points to \c {nullptr}. */
    explicit ConstPropagatingRawPointer()
        : m_pointer(nullptr)
    {
    }

    /*!
     * \brief Constructor
     *
     * \a pointer Object to which to point
     */
    explicit ConstPropagatingRawPointer(T *pointer)
        : m_pointer(pointer)
    {
    }

    /*!
     * \brief Default destructor */
    ~ConstPropagatingRawPointer() noexcept = default;

    /*!
     * \brief Non-const pointer operator
     */
    [[nodiscard]] T *operator->()
    {
        return m_pointer;
    }

    /*!
     * \brief Const pointer operator
     */
    [[nodiscard]] const T *operator->() const
    {
        return m_pointer;
    }

    /*!
     * \brief Non-const dereference operator
     */
    [[nodiscard]] T &operator*()
    {
        return *m_pointer;
    }

    /*!
     * \brief Const dereference operator
     */
    [[nodiscard]] const T &operator*() const
    {
        return *m_pointer;
    }

    /*!
     * \brief Cast to a normal raw pointer.
     *
     * \sa toPointerToConstObject()
     */
    [[nodiscard]] operator T *()
    {
        return m_pointer;
    }

    // We could provide a const overload to operator T*() instead of
    // providing this function with a strange name. But overloading
    // operator T* leads to compile errors because of ambiguity
    // on GCC and Clang when using this pointer within a “delete” statement.
    // That’s confusing, therefore we should avoid it. By giving this
    // function a different name prevents automatic casts in non-const
    // objects, which also forbids deleting directly pointers of this
    // type in non-const contexts, which is nice.
    /*!
     * \brief Cast to a normal raw pointer that points to a const object.
     */
    [[nodiscard]] const T *toPointerToConstObject() const
    {
        return m_pointer;
    }

private:
    /*!
     * \brief Internal storage for the pointer */
    T *m_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CONSTPROPAGATINGUNIQUEPOINTER_H
