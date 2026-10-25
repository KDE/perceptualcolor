// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_IMPORTEXPORT_H
#define PERCEPTUALCOLOR_IMPORTEXPORT_H

#include <qglobal.h>

/*!
 * \headerfile importexport.h
 *
 * \inmodule ModulePublicApi
 */

/*!
 * \macro PERCEPTUALCOLOR_IMPORTEXPORT
 *
 * \relates importexport.h
 *
 * \ingroup GroupPublicSymbols
 *
 * \brief A macro that either exports dynamic library symbols or imports
 * dynamic library symbols or does nothing.
 *
 * To use this macro, include \c {importexport.h}
 *
 *
 * This approach is proposed in Qt’s documentation (chapter “Creating
 * Shared Libraries”) – see there for more details. In short:
 *
 * \section1 Build an application using the dynamic library
 *
 * When your application is build \e {using} the dynamic
 * library and includes the header files of the library, the
 * macro \e {imports} the corresponding symbols of the library
 * for you by expanding to Qt’s \c {Q_DECL_IMPORT} macro. This
 * is the default behaviour of this macro.
 *
 * \section1 Build the dynamic library
 *
 * When the dynamic library \e {itself} is build, the macro
 * \e {exports} the corresponding symbol by expanding to Qt’s
 * \c {Q_DECL_EXPORT} macro. Exported symbols will be visible
 * symbols in the dynamic library. To get this behaviour, it is
 * necessary to define \c {PERCEPTUALCOLOR_BUILD_DYNAMIC_LIBRARY}
 * always when this library itself is build.
 *
 * \section1 Build either the static library itself or an application using it
 *
 * When either
 *
 * \list
 *     \li building the static library itself
 * \endlist
 *
 * or
 *
 * \list
 *     \li your application is build \e {using} the static
 *         library and includes the header files of the library,
 * \endlist
 *
 * the macro expands to \e {nothing}, because for static libraries
 * no import nor export must happen. To get this behaviour, it is
 * necessary to define \c {PERCEPTUALCOLOR_STATIC}.
 *
 * \section1 CMake code
 *
 * The definition is made within CMake:
 *
 * \code
 * if(BUILD_SHARED_LIBS)
 *     target_compile_definitions(
 *         my_target_name
 *         PRIVATE PERCEPTUALCOLOR_BUILD_DYNAMIC_LIBRARY)
 * else()
 *     target_compile_definitions(
 *         my_target_name
 *         PUBLIC PERCEPTUALCOLOR_STATIC)
 * endif()
 * \endcode
 *
 * \c {PERCEPTUALCOLOR_BUILD_DYNAMIC_LIBRARY} is defined
 * \c {PRIVATE}, so it only becomes available when building
 * the library \e {itself} dynamically, and not when building
 * an application \e {using} the dynamic library.
 *
 * \c {PERCEPTUALCOLOR_STATIC} however is defined
 * \c {PUBLIC}, so it becomes available both, when building
 * the library \e {itself} statically, and also when building
 * an application \e {using} the static library.
 *
 * And you also have to make sure that all symbols that are \e {not}
 * explicitly exported will be actually hidden on all platforms:
 *
 * \code
 * set_target_properties(
 *     my_target_name PROPERTIES
 *     # By default, on Windows all symbols are hidden except those that are
 *     # explicitly marked for export using the "__declspec(dllexport)"
 *     # or "__declspec(dllimport)" keywords in the code. On Unix-based systems,
 *     # however, all symbols are exported by default unless they are explicitly
 *     # marked as hidden. To achieve the same behavior as on Windows, set
 *     # the "CXX_VISIBILITY_PRESET" property to "hidden" in CMake to hide all
 *     # symbols by default, unless they are explicitly marked for export using
 *     # compiler-specific attributes.
 *     CXX_VISIBILITY_PRESET "hidden"
 *     VISIBILITY_INLINES_HIDDEN TRUE)
 * \endcode
 *
 * For your information: The opposite would look like this:
 *
 * \code
 * set_target_properties(
 *     my_target_name PROPERTIES
 *     # We want all symbols to be publicly available. On Unix-based systems, this
 *     # is the default behavior, and no additional configuration is required.
 *     CXX_VISIBILITY_PRESET "default"
 *     # However, on Windows, all symbols are hidden by default except for those
 *     # that are explicitly marked for export using "__declspec(dllexport)" or
 *     # "__declspec(dllimport)" keywords. To achieve the same behavior on Windows
 *     # as on Unix-based systems, CMake provides the "WINDOWS_EXPORT_ALL_SYMBOLS"
 *     # property, which can be set to "TRUE" to automatically generate the
 *     # necessary export symbols for all classes and functions on Windows.
 *     # However, please note that this option does not work for global variables.
 *     WINDOWS_EXPORT_ALL_SYMBOLS TRUE
 *     VISIBILITY_INLINES_HIDDEN FALSE)
 * \endcode
 *
 * \section1 Further reading
 *
 * \note CMake also offers support for symbol import and export.
 * It can generate a corresponding header by using the
 * \l {https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html}
 * {generate_export_header()} command. However, this is always
 * an additional step and makes the build and install configuration
 * more complex. Furthermore, we produce a second internal library
 * target out of the same source code, which has a different symbol
 * visibility for unit tests. As CMake-generated import-export headers
 * use the name of the target as part of the macro names it defines, this
 * would get complicated. Having our own macro is easier.
 *
 * \note CMake’s
 * \l {https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html}
 * {generate_export_header()} command also has portable
 * support for deprecating symbols. However, since C++14 there is
 * \l {https://en.cppreference.com/w/cpp/language/attributes/deprecated}
 * {[[deprecated(string-literal)]} part of the standard.
 * We can use this as a portable standard
 * instead of CMake’s macros.
 *
 * \sa https://doc.qt.io/qt-5/sharedlibrary.html#using-symbols-from-shared-libraries
 * \sa http://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html
 * \sa https://labjack.com/news/simple-cpp-symbol-visibility-demo
 */

#ifndef PERCEPTUALCOLOR_IMPORTEXPORT

#ifdef PERCEPTUALCOLOR_STATIC
#define PERCEPTUALCOLOR_IMPORTEXPORT
#else
#ifdef PERCEPTUALCOLOR_BUILD_DYNAMIC_LIBRARY
#define PERCEPTUALCOLOR_IMPORTEXPORT Q_DECL_EXPORT
#else
#define PERCEPTUALCOLOR_IMPORTEXPORT Q_DECL_IMPORT
#endif
#endif

#endif // #ifndef PERCEPTUALCOLOR_IMPORTEXPORT

#endif // PERCEPTUALCOLOR_IMPORTEXPORT_H
