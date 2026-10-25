// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CONSTPROPAGATINGUNIQUEPOINTER_H
#define PERCEPTUALCOLOR_CONSTPROPAGATINGUNIQUEPOINTER_H

#include <memory>

namespace PerceptualColor
{

// NOTE This class inherits privately \c {std::shared_ptr} and not
// \c {std::unique_ptr} because the latter will not compile on
// the MSVC compiler when used with incomplete types, which is however
// necessary for usage within pimpl.
//
// NOTE There are other implementation available. But it’s not worth to use
// https://github.com/jbcoe/propagate_const instead of having our own
// implementation, which is simpler, but just fine for our use case.
// This class could also be replaced in the future by
// \l {https://en.cppreference.com/w/cpp/experimental/propagate_const}
// {std::experimental::propagate_const} if this one ever becomes
// part of the C++ standard. (Experimental features however are optional
// for compilers, so not all of them implement them. Furthermore, they can
// still change. Therefore, we cannot use experimental features here.) But
// \l {https://en.cppreference.com/w/cpp/experimental/propagate_const}
// {std::experimental::propagate_const}}seems to be superseded by
// \l {https://en.cppreference.com/w/cpp/memory/indirect.html}
// {std::indirect} and
// \l {https://en.cppreference.com/w/cpp/memory/polymorphic.html}
// {std::polymorphic} anyway.
///
// NOTE No PERCEPTUALCOLOR_IMPORTEXPORT for generic template definitions!
/*!
 * \inmodule ModulePublicApi
 *
 * \class PerceptualColor::ConstPropagatingUniquePointer
 *
 * \brief A \c {const} propagating \c {std::unique_ptr}.
 *
 * With normal \c {std::unique_ptr} pointers (and also with raw
 * C++ pointers), within \c {const} functions you can do non-const
 * operations \e {on objects that a pointer points to}.
 *
 * \e {This} pointer type is different: It propagates the constness of
 * the object members and propagates them to the call through the pointer;
 * it will trigger a compiler error if non-const access to object members
 * or methods is done from within const functions. Apart from that, it
 * behaves like \c {std::unique_ptr}.
 *
 * Think of this template as a simple alternative to
 * \c {std::experimental::propagate_const< std::unique_ptr<T> >}
 *
 * Example code:
 *
 * \snippet testconstpropagatinguniquepointer.cpp example
 *
 * Currently, move-assignment is not supported. Use
 * \c {std::unique_prt::reset()} instead. As there is no
 * support for deleters anyway, this should be equivalent.
 *
 * \note It also propagates the const-ness of the
 * \l ConstPropagatingUniquePointer pointer variable itself. Consequently
 * \e {all} access to non-const functions is impossible if the pointer
 * variable itself is \c {const}.
 */
template<typename T>
class ConstPropagatingUniquePointer : private std::shared_ptr<T>
{
public:
    /*!
     * \brief Default constructor
     *
     * Creates a pointer that points to \c {nullptr}.
     */
    explicit ConstPropagatingUniquePointer()
        : std::shared_ptr<T>(nullptr)
    {
    }

    /*!
     * \brief Constructor
     *
     * \a pointerToObject Object to which to point
     */
    explicit ConstPropagatingUniquePointer(T *pointerToObject)
        : std::shared_ptr<T>(pointerToObject)
    {
    }

    /*!
     * \brief Default destructor
     *
     * This destructor is not marked as \c {override} because the
     * base class’s destructor is not virtual.*/
    ~ConstPropagatingUniquePointer() noexcept = default;

    // No copy/move assignment or constructor.
    ConstPropagatingUniquePointer(const ConstPropagatingUniquePointer &) = delete;
    ConstPropagatingUniquePointer(ConstPropagatingUniquePointer &&) = delete;
    ConstPropagatingUniquePointer &operator=(const ConstPropagatingUniquePointer &) = delete; // clazy:exclude=function-args-by-value
    ConstPropagatingUniquePointer &operator=(ConstPropagatingUniquePointer &&) = delete;

    /*!
     * \brief Non-const pointer operator
     */
    [[nodiscard]] T *operator->()
    {
        // cppcheck-suppress CastIntegerToAddressAtReturn // false positive
        return std::shared_ptr<T>::operator->();
    }

    /*!
     * \brief Const pointer operator
     */
    [[nodiscard]] const T *operator->() const
    {
        // cppcheck-suppress CastIntegerToAddressAtReturn // false positive
        return std::shared_ptr<T>::operator->();
    }

    /*!
     * \brief Non-const dereference operator
     */
    [[nodiscard]] T &operator*()
    {
        // cppcheck-suppress returnTempReference // false positive
        return std::shared_ptr<T>::operator*();
    }

    /*!
     * \brief Const dereference operator
     */
    [[nodiscard]] const T &operator*() const
    {
        // cppcheck-suppress returnTempReference // false positive
        return std::shared_ptr<T>::operator*();
    }

    /*!
     * \brief Deletes the previously managed object (if any) and starts
     * to manage a new object.
     *
     * \a newObject The new object that will be managed. Can
     * be \c {nullptr} to not manage any object anymore.
     */
    void reset(T *newObject = nullptr)
    {
        std::shared_ptr<T>::reset(newObject);
    }

    /*!
     * \brief Swaps the managed objects.
     *
     * \a other Another \l ConstPropagatingUniquePointer object to
     * swap the managed object with. */
    void swap(ConstPropagatingUniquePointer &other)
    {
        std::shared_ptr<T>::swap(other);
    }

    /*!
     * \brief Returns a pointer to the managed object or \c {nullptr} if
     * no object is owned.
     *
     * Returns a pointer to the managed object or \c {nullptr} if
     * no object is owned. */
    [[nodiscard]] T *get()
    {
        // cppcheck-suppress CastIntegerToAddressAtReturn // false positive
        return std::shared_ptr<T>::get();
    }
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CONSTPROPAGATINGUNIQUEPOINTER_H
