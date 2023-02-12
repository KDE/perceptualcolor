# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# General setup #################
# Contrary to the name, “cmake_minimum_required()” does not (only) set a
# minimal required CMake version, but a compatibility mode: If the library
# user or the packages has a newer CMake version than indicated, than it will
# run in a compatibility mode that makes sure we get the exact same result
# as if actually CMake in the indicated version would have been invoked.
#
# Qt 6.0 requires at least CMake version 3.16.
# https://doc.qt.io/archives/qt-6.0/cmake-get-started.html
# Qt 6.2 LTS requires at least CMake version 3.16 for the Qt shared library
# and CMake version 3.21.1 for Qt static library.
# https://doc.qt.io/archives/qt-6.2/cmake-get-started.html
cmake_minimum_required(VERSION 3.16.0)
message(STATUS "CMAKE_VERSION: ${CMAKE_VERSION}")

# Project setup. We need only a C++ (CXX) compiler, and no C compiler.
# Therefore, we set LANGUAGES explicitly.
project(
    perceptualcolor
    DESCRIPTION "Perceptual color components"
    LANGUAGES CXX
)
message(STATUS "CMAKE_CXX_COMPILER: ${CMAKE_CXX_COMPILER}")
message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMAKE_CXX_COMPILER_VERSION: ${CMAKE_CXX_COMPILER_VERSION}")

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR TRUE)

# Get predefined directories for installation process
# Don’t worry, this is no lock-in into GNU, it simply provides
# better default directories on Linux, and also works fine on
# non-Linux systems.
include(GNUInstallDirs)

message(CHECK_START "Checking for iwyu (include-what-you-use)")
find_program(
    iwyu_path
    NAMES "iwyu" "include-what-you-use")
if(iwyu_path)
    message(
        CHECK_PASS
        "Found.\n"
            "   Run cmake again with "
            "“-DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=${iwyu_path}” to use it.")
    # TODO This does not check headers that have neither a cpp
    #      file with the same name nor a cpp file declaring the header
    #      with “// IWYU pragma: associated”. It should check them however.
    #      At the time of writing this comment, this are: importexport.h,
    #      constpropagatinguniquepointer.h, and constpropagatingrawpointer.h.
    #      Maybe declare them by pragma within their unit tests?
else()
    message(CHECK_FAIL "Not found.")
endif()
message(
    STATUS
    "CMAKE_CXX_INCLUDE_WHAT_YOU_USE: ${CMAKE_CXX_INCLUDE_WHAT_YOU_USE}")

message(CHECK_START "Checking for clang-tidy")
find_program(
    clang_tidy_path
    NAMES
        "clang-tidy")
if(clang_tidy_path)
    message(
        CHECK_PASS
        "Found.\n"
            "   Run cmake again with "
            "“-DCMAKE_CXX_CLANG_TIDY=${clang_tidy_path}” to use it.")
else()
    message(CHECK_FAIL "Not found.")
endif()
message(
    STATUS
    "CMAKE_CXX_CLANG_TIDY: ${CMAKE_CXX_CLANG_TIDY}")

message(CHECK_START "Checking for cppcheck")
find_program(
    cppcheck_path
    NAMES
        "cppcheck")
if(cppcheck_path)
    message(
        CHECK_PASS
        "Found.\n"
            "   Run cmake again with "
            "“-DCMAKE_CXX_CPPCHECK=${cppcheck_path}” to use it.")
else()
    message(CHECK_FAIL "Not found.")
endif()
message(
    STATUS
    "CMAKE_CXX_CPPCHECK: ${CMAKE_CXX_CPPCHECK}")

# We set a default build type when the user did not specify a build type.
# Reason: An empty build type does not provide any compiler flags at all,
# thus no optimization at all, for example.
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()
message(STATUS "CMAKE_BUILD_TYPE: ${CMAKE_BUILD_TYPE}")





################# Version number #################
# Provide project version
# We are following “Semantic Versioning 2.0.0”.
# See https://semver.org/ for details.
set(MAJOR_VERSION "0")
set(MINOR_VERSION "0")
set(PATCH_VERSION "1")
set(FULL_VERSION "${MAJOR_VERSION}.${MINOR_VERSION}.${PATCH_VERSION}")





################# Compiler standard #################
# Our choice for the required C++ standard is documented via Doxygen (main
# page of the documentation).
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE) # Try to disable compiler-specific extensions.





################# Inter-procedural optimization #################
include(CheckIPOSupported)
message(
    CHECK_START
    "Checking for IPO/LTO "
        "(inter-procedural optimization/link-time optimization)")
check_ipo_supported(
    RESULT has_ipo_support
    OUTPUT output) # The variable “output” can be ignored.
if(has_ipo_support)
    message(CHECK_PASS "Found.")
else()
    message(CHECK_FAIL "Not found.")
endif()
message(
    STATUS
    "CMAKE_INTERPROCEDURAL_OPTIMIZATION: ${CMAKE_INTERPROCEDURAL_OPTIMIZATION}")





################# Ordinary optimization #################
# About optimization: We leave the decision on the optimization level
# up to CMake, which provides cross-platform support.
#
# Anyway, here some thoughts about optimization on GCC:
# GCC has quite a few optimization options like “O0” (no optimization)
# or “Os” (optimize for minimal binary size with the trade-off of slower
# binaries). “O2” seems to be quit the default. “O3”  optimizes further
# for speed, with the trade-off having a bigger binary file.
# Our image rendering is slow, so more optimization is better.  Probably
# “Ofast” is not an option, as it might make problems with multithreading.
# Probably ffast-mat though is not a good idea either because of its risks:
# This option breaks strict IEEE compatibility of floating point to provide
# a faster run-time; for example, division by 0 results in a wrong return
# value instead of an exception.
#
# TODO Profile-guided optimization (PGO) provides by GCC can further
# increase speed and shriek binary size, but requires a more complex
# build system. It would be nice to have this if it does not make the build
# process too complicate. For details, see also:
# https://documentation.suse.com/sbp/all/pdf/SBP-GCC-10_color_en.pdf





################# Preprocessor macro definitions #################
# Preprocessor macro definitions that configure Qt to be more
# strict or more compatible.
#
# Some Qt API functions provide implicit casts. By defining the QT_NO_CAST_…
# preprocessor macros, the code of our library is forced to use explicit
# casts instead; this will helps to reveal bugs in our library.
#
# Some other preprocessor macro definitions disable potentially error-prone
# Qt features. Or they make Qt more compatible, like QT_NO_KEYWORDS.
add_compile_definitions(
    # Unlike target_compile_definitions(), which distinguishes
    # between PRIVATE, INTERFACE and PUBLIC definitions,
    # with add_compile_definitions() all definitions are always PRIVATE.
    QT_NO_CAST_FROM_ASCII
    QT_NO_CAST_FROM_BYTEARRAY
    QT_NO_CAST_TO_ASCII
    QT_NO_NARROWING_CONVERSIONS_IN_CONNECT
    QT_NO_PROCESS_COMBINED_ARGUMENT_START
    QT_NO_URL_CAST_FROM_STRING
    QT_USE_QSTRINGBUILDER
    QT_NO_KEYWORDS
    # DQT_DISABLE_DEPRECATED_BEFORE disables all deprecated features.
    # While normal deprecation warnings are NOT issued for ALL deprecated
    # features, THIS flag actually seems to block actually all deprecated
    # features:
    QT_DISABLE_DEPRECATED_BEFORE=0x060000
    # This might crash on older Qt versions on Windows,
    # see https://bugreports.qt.io/browse/AUTOSUITE-946.
    # But we do not support older Qt versions anyway:
    QT_STRICT_ITERATORS
)





################# Symbol stripping #################
# NOTE The Clang/GCC option “-s” will “Remove all symbol table
# and relocation information from the executable.” This reduces
# the size of the binary and the loading time. According to
# https://www.technovelty.org/linux/stripping-shared-libraries.html
# shared libraries do not need the symbols, so they can safely be
# removed. But we have to the the option manually; there is no
# build-in CMake support that helps us do this in a portable way.
# Therefore, we leave it up to the packagers to strip the symbols
# finally with “strip --strip-unneeded libraryname”.





################# Compiler character sets #################
# NOTE It is possible to force UTF-8 as input character set:
# GCC and clang use “-finput-charset=UTF-8” to set the input character set.
# MSVC uses “/utf-8” which sets both, the input and the narrow execution
# character set. However, currently we try to get away with the default
# settings provided by CMake.





################# Warnings #################
# Available compiler flags for warnings might change incompatibly from
# version to version, which could let the build fail. Also, newer compiler
# versions might throw more warnings, which could break the build if
# warnings are treated as errors by “-Werror”. Therefore, by default
# we try to not use any hard-coded compiler flags. This gives us
# maximum compatibility for release builds. But a higher warning level
# can be requested by the user:
message(
    STATUS
    "By calling CMake with the option “-DADDITIONAL_WARNINGS=ON”\n"
    "   (indeed ON, not TRUE!) you can:\n"
    "   Get more warnings, which is only useful for the developers\n"
    "   of this library. It might not be portable and even break the build\n"
    "   process, so it should never be used in production builds.")
option(
    ADDITIONAL_WARNINGS               # Name
    "More warnings and errors"        # Help text
    FALSE)                            # Default value
# TODO Starting with CMake 3.19, this option could be replaced with
# a CMake preset, which might be more convenient? Anyway, currently we
# require only CMake 3.16 just as Qt 6 does…
message(
    STATUS
    "ADDITIONAL_WARNINGS: ${ADDITIONAL_WARNINGS}")
if(
    ADDITIONAL_WARNINGS
    AND
    (
        ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
        OR
        ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
    )
)
    add_compile_options(
        # Issue all the warnings demanded by strict ISO C++:
        -Wpedantic
        # Some pedantic warning are not turned into errors by -Werror,
        # as the gcc manual explains. -pedantic-errors fills this gap.
        -pedantic-errors
        # Treat all warnings as errors:
        # -Werror
        # Contrary to the name, "-Wall" does not activate all warnings,
        # but only the most important ones:
        -Wall
        # -Wextra enables some extra warning flags
        # that are not enabled by -Wall:
        -Wextra
        # Additional individual warning flags (in alphabetical order):
        -Wcast-align
        -Wcast-qual
        -Wchar-subscripts
        -Wcomment
        -Wconversion
        -Wdisabled-optimization
        # -Wfloat-equal # TODO Enable this check!
        -Wformat
        -Wformat=2 # even more checks on top of “-Wformat”
        -Wformat-nonliteral
        -Wformat-security
        -Wformat-y2k
        -Wimport
        -Winit-self
        -Winvalid-pch
        -Wlong-long
        -Wmissing-braces
        -Wmissing-field-initializers
        -Wmissing-format-attribute
        -Wmissing-include-dirs
        -Wmissing-noreturn
        -Wpacked
        -Wparentheses
        -Wpointer-arith
        -Wredundant-decls
        -Wreturn-type
        -Wsequence-point
        -Wshadow
        -Wsign-compare
        -Wsign-conversion
        -Wstack-protector
        -Wstrict-aliasing
        # It is possible to set “-Wstrict-aliasing=n”, but only after setting
        # “-Wstrict-aliasing” without n before! n is the level:
        #           ╭────────────────┬────────────────╮
        #           │ False positive │ False negative │
        # ╭─────────┼────────────────┼────────────────┤
        # │ Level 1 │      many      │    very few    │
        # ├─────────┼────────────────┼────────────────┤
        # │ Level 2 │   still many   │       few      │
        # ├─────────┼────────────────┼────────────────┤
        # │ Level 3 │    very few    │       few      │
        # ╰─────────┴────────────────┴────────────────╯
        -Wstrict-aliasing=1
        -Wswitch-enum
        -Wtrigraphs
        -Wuninitialized
        -Wunknown-pragmas
        -Wunreachable-code
        -Wunused
        -Wunused-function
        -Wunused-label
        -Wunused-parameter
        -Wunused-value
        -Wunused-variable
        -Wvariadic-macros
        -Wvla
        -Wvolatile-register-var
        -Wwrite-strings
        -Wzero-as-null-pointer-constant
    )
endif()
if(ADDITIONAL_WARNINGS AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU"))
    add_compile_options(
        -Wunsafe-loop-optimizations
    )
endif()
if(ADDITIONAL_WARNINGS AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang"))
    # Add additional flags for Clang:
    add_compile_options(
        # More warnings:
        -Wctad-maybe-unsupported
        -Winline
        -Wunreachable-code
        # Enable really all available warnings, even wired ones:
        -Weverything
        # Disable some of the warnings enabled by "-Everything"
        # that generate false positives…
        # We do know want to be compatible with C++98:
        -Wno-c++98-compat
        # We allow redundant parentheses because sometimes they
        # make code more readable:
        -Wno-redundant-parens
        -Wno-padded
        # We disable -Wextra-semi-stmt because moc-generated code
        # has indeed quite a few extra semicolon statements, and
        # we cannot change how the moc generated its code.
        -Wno-extra-semi-stmt
        # Necessary to prevent compile error when using clazy:
        -Wno-documentation-unknown-command
        # We use default sections in switch statements for reasonable
        # fallback code when an enum value is not handled:
        -Wno-covered-switch-default
        # We disable -Wfloat-equal because we make this kind of comparisons
        # in property setter functions to check if actually the property value
        # has to be changed or not, and this seems a reasonable use case.
        -Wno-float-equal
        # NOTE -Wnoexit-time-destructors and -Wnoglobal-constructors
        # are only necessary to suppress -Weverything generated
        # warnings on the C++ files that are produced by Qt’s
        # resource compiler. See also:
        # https://stackoverflow.com/questions/14335494
        # https://stackoverflow.com/questions/15708411
        -Wno-exit-time-destructors
        -Wno-global-constructors
        -Wno-aggregate-return
        # Suppress warnings for reserved identifiers, because Qt’s MOC
        # produces code that uses a lot of reserved identifiers and we
        # would get plenty of warnings for this.
        -Wno-reserved-identifier
    )
endif()
if(ADDITIONAL_WARNINGS AND ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC"))
    # NOTE Documentation of warning for MSVC:
    # docs.microsoft.com/cpp/build/reference/compiler-option-warning-level
    add_compile_options(
        /Wall # Enable many warnings.
        /WX # Treats all compiler warnings as errors.
    )
endif()





################# Setup external library dependencies #################
# Use also our local FIND modules
list(APPEND CMAKE_MODULE_PATH
    "${CMAKE_SOURCE_DIR}/cmake/Modules/")
find_package( # Necessary for find_package(LCMS2)
    PkgConfig
    REQUIRED)
find_package(
    LCMS2
    REQUIRED)
find_package(
    ECM 5.82 # Version 5.82 required for QtVersionOption
    REQUIRED
    NO_MODULE)
list(APPEND CMAKE_MODULE_PATH
    ${ECM_MODULE_PATH})
include(QtVersionOption) # Provides ${QT_MAJOR_VERSION}
# CheckAtomic: “Sometimes linking against libatomic is required for atomic ops,
#               if the platform doesn’t support lock-free atomics.”
#              “Check if the compiler supports std:atomic out of the box or if
#               libatomic is needed for atomic support. If it is needed
#               libatomicis added to ``CMAKE_REQUIRED_LIBRARIES``.”
include(CheckAtomic)
message(
    STATUS
    "Building against Qt${QT_MAJOR_VERSION}.\n"
    "   You can control this with “-DBUILD_WITH_QT6=ON” (build with Qt6)\n"
    "   or “-DBUILD_WITH_QT6=OFF” (build with Qt5).")
find_package(
    Qt${QT_MAJOR_VERSION}
    COMPONENTS Core Gui Widgets DBus Test Concurrent
    REQUIRED)
# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC TRUE)
# Instruct CMake to create code from Qt designer ui files
set(CMAKE_AUTOUIC TRUE)





################# Build STATIC or SHARED library #################
# Either a STATIC or a SHARED library is build, simply depending
# on BUILD_SHARED_LIBS.
#
# A more sophisticated handling of STATIC vs. SHARED is possible,
# but complex, and will therefore not be used here:
#alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html
option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
message(
    STATUS
    "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}.\n"
    "   You can control this with “-DBUILD_SHARED_LIBS=ON” (dynamic library)\n"
    "   or “-DBUILD_SHARED_LIBS=OFF” (static library).")





################# More targets? #################
# TODO https://gitlab.com/ubports/core/cmake-extras/ provides (among others)
# – DoxygenBuilder: Creates Doxygen API doc generation targets. But:
#   We have to make sure that the screenshots are always up-to-date. And:
#   Doxygen has currently many false-positive warnings, which we are
#   filtering in run-ci.sh. Is this possible in CMake, so that it can
#   be used as a tool to make sure the documentation is correct?
#   Or we could do like described in
#   https://majewsky.wordpress.com/2010/08/14/tip-of-the-day-cmake-and-doxygen/
#   and create a custom target manually…





################# Define targets #################
# When calling simply “make”, the default target is build. The default
# target is “all”. EXCLUDE_FROM_ALL can used when adding targets; it means
# that this new target will exceptionally not be added to the “all” target.





################# Subdirectories #################
set(CLANG_FORMAT_PATTERNS)

add_subdirectory(src)
list(APPEND CLANG_FORMAT_PATTERNS
    src/*.h # The .in.hpp file is intentionally missing in this pattern.
    src/*.cpp)

# Use unit tests only when building this project itself, and
# not when it is included within another project:
if("${CMAKE_PROJECT_NAME}" STREQUAL "${PROJECT_NAME}")
    include(CTest)
    if(BUILD_TESTING)
        add_subdirectory(autotests)
    endif()
endif()
list(APPEND CLANG_FORMAT_PATTERNS
    autotests/*.h
    autotests/*.cpp)

add_subdirectory(tests)
list(APPEND CLANG_FORMAT_PATTERNS
    tests/*.h
    tests/*.cpp)

add_subdirectory(utils)
list(APPEND CLANG_FORMAT_PATTERNS
    utils/*.h
    utils/*.cpp)





################# Clang format #################
include(KDEClangFormat)
file(
    # TODO Is it possible to get rid of GLOB_RECURSE?
    GLOB_RECURSE ALL_CLANG_FORMAT_SOURCE_FILES # Name of the output variable.
    LIST_DIRECTORIES FALSE
    # CMake will add logic to the main build system check target to
    # rerun the flagged GLOB commands at build time:
    CONFIGURE_DEPENDS
    # Examples of recursive globing include:
    # /dir/*.py  - match all python files in /dir and subdirectories(!)
    ${CLANG_FORMAT_PATTERNS}
)
# The following line makes available the target “make clang-format”.
# It requires the clang-format binary (Ubuntu package “clang-format”).
kde_clang_format(${ALL_CLANG_FORMAT_SOURCE_FILES})
