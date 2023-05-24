// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

/** @file importexport.h
 *
 * This file provides support for C++ symbol import and export. */

#ifndef IMPORTEXPORT_H
#define IMPORTEXPORT_H

#include <qglobal.h>

/** @def PERCEPTUALCOLOR_IMPORTEXPORT
 *
 * @brief A macro that either exports dynamic library symbols or imports
 * dynamic library symbols or does nothing.
 *
 * This approach is proposed in Qt’s documentation (chapter “Creating
 * Shared Libraries”) – see there for more details. In short:
 *
 * @section buildappdynamic Build an application using the dynamic library
 *
 * When your application is build <em>using</em> the dynamic
 * library and includes the header files of the library, the
 * macro <em>imports</em> the corresponding symbols of the library
 * for you by expanding to Qt’s <tt>Q_DECL_IMPORT</tt> macro. This
 * is the default behaviour of this macro.
 *
 * @section buildlibdynamic Build the dynamic library
 *
 * When the dynamic library <em>itself</em> is build, the macro
 * <em>exports</em> the corresponding symbol by expanding to Qt’s
 * <tt>Q_DECL_EXPORT</tt> macro. Exported symbols will be visible
 * symbols in the dynamic library. To get this behaviour, it is
 * necessary to define <tt>PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY</tt>
 * always when this library itself is build.
 *
 * @section buildstatic Build either the static library itself or an application using it
 *
 * When either
 *
 * - building the static library itself
 *
 * or
 *
 * - your application is build <em>using</em> the static
 * library and includes the header files of the library,
 *
 * the macro expands to <em>nothing</em>, because for static libraries
 * no import nor export must happen. To get this behaviour, it is
 * necessary to define <tt>PERCEPTUALCOLORLIB_STATIC</tt>.
 *
 * @section cmakeimportexport CMake code
 *
 * The definition can be made within CMake:
 *
 * @code{.unparsed}
 * if(BUILD_SHARED_LIBS)
 *     target_compile_definitions(
 *         my_target_name
 *         PRIVATE PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY)
 * else()
 *     target_compile_definitions(
 *         my_target_name
 *         PUBLIC PERCEPTUALCOLORLIB_STATIC)
 * endif()
 * @endcode
 *
 * <tt>PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY</tt> is defined
 * <tt>PRIVATE</tt>, so it only becomes available when building
 * the library <em>itself</em> dynamically, and not when building
 * an application <em>using</em> the dynamic library.
 *
 * <tt>PERCEPTUALCOLORLIB_STATIC</tt> however is defined
 * <tt>PUBLIC</tt>, so it only becomes available both, when building
 * the library <em>itself</em> statically, and also when building
 * an application <em>using</em> the static library.
 *
 * And you also have to make sure that all symbols that are <em>not</em>
 * explicitly exported will be actually hidden on all platforms:
 *
 * @code{.unparsed}
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
 * @endcode
 *
 * For your information: The opposite would look like this:
 *
 * @code{.unparsed}
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
 * @endcode
 *
 * @section importexportlinks Further reading
 *
 * @note CMake also offers support for symbol import and export.
 * It can generate a corresponding header by using the <tt><a
 * href="https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html">
 * generate_export_header()</a></tt> command. However, this is always
 * an additional step and makes the build and install configuration
 * more complex. Furthermore, we produce a second internal library
 * target out of the same source code, which has a different symbol
 * visibility for unit tests. As CMake-generated import-export headers
 * use the name of the target as part of the macro names it defines, this
 * would get complicated. Having our own macro is easier.
 *
 * @note CMake’s <a
 * href="https://cmake.org/cmake/help/latest/module/GenerateExportHeader.html">
 * <tt>generate_export_header()</tt></a> command also has portable
 * support for deprecating symbols. However, since C++14 there is
 * <a href="https://en.cppreference.com/w/cpp/language/attributes/deprecated">
 * <tt>[[deprecated(string-literal)]]</tt></a> part of the standard.
 * As we require even C++17 anyway, we can use this as a portable standard
 * instead of CMake’s macros.
 *
 * @sa https://doc.qt.io/qt-5/sharedlibrary.html#using-symbols-from-shared-libraries
 * @sa http://anadoxin.org/blog/control-over-symbol-exports-in-gcc.html
 * @sa https://labjack.com/news/simple-cpp-symbol-visibility-demo */

#ifdef PERCEPTUALCOLORLIB_STATIC

#ifndef PERCEPTUALCOLOR_IMPORTEXPORT
#define PERCEPTUALCOLOR_IMPORTEXPORT
#endif

#else

#ifndef PERCEPTUALCOLOR_IMPORTEXPORT
#ifdef PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY
#define PERCEPTUALCOLOR_IMPORTEXPORT Q_DECL_EXPORT
#else
#define PERCEPTUALCOLOR_IMPORTEXPORT Q_DECL_IMPORT
#endif
#endif

#endif

#endif // IMPORTEXPORT_H
