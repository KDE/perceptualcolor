#!/bin/bash

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# Configure #################
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh





################# Static code check #################
# NOTE Keep the following list synchronized
# between scripts/static-codecheck.sh and src/CMakeLists.txt
PUBLIC_HEADERS="
    src/abstractdiagram.h
    src/chromahuediagram.h
    src/colordialog.h
    src/colorenginefactory.h
    src/colorpatch.h
    src/colorwheel.h
    src/constpropagatinguniquepointer.h
    src/gradientslider.h
    src/importexport.h
    src/multispinbox.h
    src/multispinboxsection.h
    src/settranslation.h
    src/wheelcolorpicker.h
    src/version.in.hpp
    "
CODE_WITHOUT_UNIT_TESTS="src/* tests utils examples"
UNIT_TESTS="autotests/*"
ALL_CODE="$CODE_WITHOUT_UNIT_TESTS $UNIT_TESTS"

EXCLUDE_PUBLIC_HEADER_FROM_GREP="--exclude=$(echo $PUBLIC_HEADERS | sed 's/ / --exclude=/g')"

# TODO Test for qDebug() which should not be used never in production
# code. Exception: Unit tests.

# Search for files that do not start with a byte-order-mark (BOM).
# We do this because Microsoft’s compiler does require a BOM at the start
# of the file in order to interpret it as UTF-8.
grep \
    --recursive --exclude-dir=testbed \
    --files-without-match $'\xEF\xBB\xBF' \
    $ALL_CODE \
    | grep \
        --perl-regexp "\.(license|txt|icc|qrc|qph|ts|cmake\.in|sh)$" \
        --invert-match \
    | sed 's/^/Missing byte-order-mark: /'

grep \
    --recursive --exclude-dir=testbed \
    --files-with-match $'\t' \
    $ALL_CODE \
        | sed 's/^/Contains tab character: /'


# All header files in src/ should have an “@internal” in
# the Doxygen documentation, because when it would be public,
# the header file would not be in src/
grep \
    --recursive --exclude-dir=testbed \
    $EXCLUDE_PUBLIC_HEADER_FROM_GREP \
    --files-without-match $'@internal' \
    src/*.h \
         | sed 's/^/Missing “@internal” statement in non-public header: /'

# https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c139-use-final-on-classes-sparingly
# The C++ core guidelines remommend to use “final” sparingly. The public header
# files must not use “final” at all because it cannot be removed without
# breaking binary compatibility. Other headers should avoid it also.
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp --files-with-matches '(^|[[:space:]])final([[:space:]]|$)' \
    $ALL_CODE \
    | grep \
        --perl-regexp "\.cpp$" \
        --invert-match \
    | sed 's/^/“final” should not be used: /'

# All public header files should use
# the PERCEPTUALCOLOR_IMPORTEXPORT macro.
grep \
    --recursive --exclude-dir=testbed \
    --files-without-match $'PERCEPTUALCOLOR_IMPORTEXPORT' \
    $PUBLIC_HEADERS \
         | sed 's/^/Missing PERCEPTUALCOLOR_IMPORTEXPORT macro in public header: /'

# All non-public code should not use the PERCEPTUALCOLOR_IMPORTEXPORT macro.
grep \
    --recursive --exclude-dir={testbed,include} \
    $EXCLUDE_PUBLIC_HEADER_FROM_GREP \
    --files-with-matches $'PERCEPTUALCOLOR_IMPORTEXPORT' \
    $CODE_WITHOUT_UNIT_TESTS \
         | sed 's/^/Internal files may not use PERCEPTUALCOLOR_IMPORTEXPORT macro: /'

# Do not use constexpr in public headers as when we change the value
# later, compile time value and run time value might be different, and
# that might be dangerous.
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "constexpr" \
    $PUBLIC_HEADERS \
         | sed 's/^/Public headers may not use constexpr: /'

# Our policy with regard to namespace pollution requires all macros to be
# prefixed with PERCEPTUALCOLOR_
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings \
    "#define " \
    $ALL_CODE \
        | grep -v "#define PERCEPTUALCOLOR_" \
        | sed 's/^/Missing PERCEPTUALCOLOR_ prefix: /'

grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp \
    '^#define\s+\S+' \
    $ALL_CODE \
        | grep --perl-regexp --invert-match '.*#define\s+[A-Z_][A-Z_0-9]*\b' \
        | sed 's/^/Only A-Z_ in macro names: /'

# Search for some patterns that should not be used in the source code. If
# these patterns are found, a message is displayed. Otherwise, nothing is
# displayed.

# We do not include LittleCMS headers like lcms2.h in the public API of our
# library. But it is only be an internal dependency; library users should
# not need to care about that. Therefore, we grab all lines that contain
# identifiers starting with “cms” (except when in lines starting with
# “using”). This search is not done for all code directories, but only
# for files within the include directory (public API).
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "^cms" \
    $PUBLIC_HEADERS \
         | sed 's/^/Do not expose LittleCMS’ API in our public API: /'
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "[^a-zA-Z]cms[a-zA-Z0-9]" \
    $PUBLIC_HEADERS \
    | grep \
        --perl-regexp "\<tt\>cms" \
        --invert-match \
         | sed 's/^/Do not expose LittleCMS’ API in our public API: /'
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "lcms2.h" \
    $PUBLIC_HEADERS \
         | sed 's/^/Do not expose LittleCMS’ API in our public API: /'

# -> using versus typedef: Both methods for declaring aliases are equivalent,
#    but the using statement is preferred as it is considered easier to read
#    than the typedef statement.
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "(?<!\\S)typedef(?!\\S)" \
    $ALL_CODE \
         | sed 's/^/Prefer “using ALIAS = TYPE;” over “typedef TYPE ALIAS;”: /'

# -> Do not use the “code” and “endcode” tags for Doxygen documentation. Use
#    @snippet instead! That allows that the example code is actually compiled
#    and that helps detecting errors.
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "(@|\\\\)code(?!\{\.unparsed\})" \
    $ALL_CODE \
         | sed 's/^/Use snippets instead of “code” or “endcode”. (Exception: @code{.unparsed} is allowed.): /'

# -> Doxygen style: Do not use “@em xyz”. Prefer instead “<em>xyz</em>” which
#    might be longer, but has a clearer start point and end point, which is
#    better when non-letter characters are involved. The @ is reserved
#    for @ref with semantically tested references.
# -> Same thing for “@c xyz”: Prefer instead “<tt>xyz</tt>”.
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "\\em" \
    $ALL_CODE \
         | sed 's/^/Use HTML tags instead of Doxygen commands for “em”: /'
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "@em" \
    $ALL_CODE \
         | sed 's/^/Use HTML tags instead of Doxygen commands for “em”: /'
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "\\c" \
    $ALL_CODE \
         | sed 's/^/Use HTML tags instead of Doxygen commands for “em”: /'
grep \
   --recursive --exclude-dir=testbed  \
    --perl-regexp "@c(?=([^a-zA-Z]|$))" \
    $ALL_CODE \
         | sed 's/^/Use HTML tags instead of Doxygen commands for “em”: /'

# -> Coding style: Do not use the “NULL” macro, but its counterpart “nullptr”
#    which is more type save.
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "NULL" \
    $ALL_CODE \
         | sed 's/^/Do not use the NULL macro: /'

# -> Coding style: Do not use inline functions. If used in a header,
#    once exposed, they cannot be changed without breaking binary
#    compatibility, because applications linking against the library
#    will always execute the inline function version against they where
#    compiled, and never the inline function of the library version
#    against they are linking at run-time. This make maintaining binary
#    compatibility much harder, for little benefit.
#
#    However, “static inline” variables are good for the headers or
#    private PIMPL classes, because differently to “simple” “static”
#    variables they allow the definition within the very same header,
#    which is not only easier to read, but also protects against
#    a double definition in different compiler units.
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "(^|[^a-zA-Z0-9\-])inline(?![^()\/;]*=.*;)" \
    $ALL_CODE \
         | sed 's/^/Do not use inline functions respectively define inline variables at the same line.: /'

# -> In some Qt classes, devicePixelRatio() returns in integer.
#    Don’t do that and use floating point precision instead. Often,
#    devicePixelRatioF() is an alternative that provides
#    a qreal return value.
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "devicePixelRatio(?!F)" \
    $CODE_WITHOUT_UNIT_TESTS \
         | sed 's/^/Use devicePixelRatioF instead of devicePixelRatio: /'

# Qt’s documentation about QImage::Format says: For optimal performance only
# use the format types QImage::Format_ARGB32_Premultiplied,
# QImage::Format_RGB32 or QImage::Format_RGB16. Any other format, including
# QImage::Format_ARGB32, has significantly worse performance.
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "QImage::Format_(?!(ARGB32_Premultiplied|RGB32|RGB16))" \
    $ALL_CODE \
         | sed 's/^/Use exclusively ARGB32_Premultiplied or RGB32 or RGB16 as QImage formats: /'

# -> QLatin1String and QLatin1Char dates from the pre-Unicode epoch. It should
#    not be used anymore at all, yet to avoid confusion. Though the Clazy
#    codecheck suggests to use QLatin1String() instead of an empty
#    QStringLiteral because of a slight performance gain, this might not
#    compensate for the less readable code. Alternatively, QString() might be
#    used to avoid Clazy warnings.
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "QLatin1" \
    $ALL_CODE \
         | sed 's/^/QLatin1String or QLatin1Char: If emtpy, use substitute by QString(), otherwise by QStringLiteral.: /'

# -> volatile: Do not use volatile. For multi-threading, volatile is not enough
#    because it only guaranties that access to the variable is not optimized
#    away. It still lacks the necessary memory barriers and atomic behaviour
#    for multi-threading. On the other hand, std::atomic and mutexes provide
#    all this, and they also guarantee that access to the variable is not
#    optimized away. Therefore, the only use case for “volatile” is hardware
#    register programming, which we do not need here. Therefore: Do not use
#    “volatile”. See https://stackoverflow.com/a/2485177 for details.
grep \
    --recursive --exclude-dir=testbed \
    --perl-regexp "(?<!\\S)volatile(?!\\S)" \
    $ALL_CODE \
         | sed 's/^/Never use the “volatile” keyword: /'

# When using Doxygen snippets, don’t do this within a namespace. As they are
# meant for documentation, they should always contain fully-qualified
# names to make sure that they always work.
#
# As it is complicate to know if the snippet is actually within a namespace
# (namespaces can be opened, but also be closed again), instead we do a
# simplified check: We simply check if there are Doxygen snippets after the
# very first usage of the “namespace” keyword.
#
# 1) Display all the files that are unit tests (adding “/*” to the UNIT_TESTS
#    variable to make sure to get a file list that “cat” will understand.
# 2) For each file, get all lines starting from the first occurrence
#    of “namespace”, using sed.
# 3) Now, filter only those who actually contain snippet definitions
#    starting with “//!”, using grep.
for FILE in $UNIT_TESTS
do
    cat $FILE 2>/dev/null \
        | sed -n -e '/\([^a-zA-Z]namespace\)\|\(^namespace\)/,$p' \
        | grep --fixed-strings "//!" \
        | sed 's/^/When using snippets, don’t do this after having used the “namespace” keyword: /'
done
