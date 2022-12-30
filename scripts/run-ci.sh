#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# Configure #################

# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/run-export-environment.sh

# Deleting completely the old build makes sure that the new build
# will be done with the default settings that are used here.
# Furthermore, it makes sure that the translations are updated,
# which only happens when CMake is run again.

# -DCMAKE_AUTOGEN_PARALLEL=$PARALLEL_PROCESSES would speed up MOC
# but unfortunately breaks the Qt Linguist, so we don’t do it.
(
while true; do
    read -p "Delete existing build and build from scratch with codecheck settings? (yes/NO) " yn
    case $yn in
        yes ) mkdir --parents build \
                && rm --recursive --force build/* \
                && cd build \
                && cmake \
                    -DCMAKE_CXX_COMPILER_LAUNCHER=ccache \
                    -DCMAKE_CXX_COMPILER=clazy \
                    -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy \
                    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/usr/bin/iwyu \
                        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
                    -DADDITIONAL_WARNINGS=TRUE \
                    .. \
                    > /dev/null
            exit;;
        * ) exit;;
    esac
done
)





################# Build the project #################
# Run within a sub-shell (therefore the parenthesis),
# so that after this we go back to the original working directory.
( \
mkdir --parents build \
    && cd build \
    && nice --adjustment 19 cmake --build . --parallel $PARALLEL_PROCESSES -- --keep-going > /dev/null \
)





################# Clang format #################
(
while true; do
    read -p "Format all the code with clang-format? (yes/NO) " yn
    case $yn in
        yes ) mkdir --parents build \
                && cd build \
                && nice --adjustment 19 cmake --build . --parallel $PARALLEL_PROCESSES --target clang-format \
                    | grep --perl-regexp "^Formatting " --invert-match; \
            echo "Code formatting done."; \
            exit;;
        * ) echo "Code formatting canceled."; \
            exit;;
    esac
done
)





################# Doxygen #################
./scripts/run-doxygen.sh





################# Compliance with REUSE specification #################
# Test if we provide all licenses as required by the “reuse” specification.
# This check needs the “reuse” application installed in your local bin
# directory. If you do not have that, you can install it with:
# pip3 install --user reuse
# Then, you have to make available $HOME/.local/bin/reuse in your path.
# Or, you can install it as root:
# sudo pip3 install reuse
# Then, you do not have to add it manually to the path.
nice --adjustment 19 reuse lint > /dev/null
if [ $? -eq 0 ];
then
    # Everything is fine. No message is printed.
    echo
else
    # “reuse lint” found problems. We call it again to print its messages.
    nice --adjustment 19 reuse lint
fi





################# CMakeLists.txt linter #################
# “2> /dev/null” throws away stderr, at which cmakelint only outputs
# the count of errors. However, stdout is preserved, which is the really
# interesting part, at which cmakelint outputs detailed errors.
echo cmakelint started.
cmakelint --spaces=4 `find -name "CMakeLists.txt"`
echo cmakelint finished.
#
# An alternative linter for CMake is available at
# https://github.com/cheshirekow/cmake_format
# but our current linter works fine, so no need to change.





################# Static code check #################
PUBLIC_HEADERS="src/include"
CODE_WITHOUT_UNIT_TESTS="src tests utils"
ALL_CODE="src autotests/* tests utils"
UNIT_TESTS="autotests/*"

# Search for files that do not start with a byte-order-mark (BOM).
# We do this because Microsoft’s compiler does require a BOM at the start
# of the file in order to interpret it as UTF-8.
grep \
    --recursive --exclude-dir=testbed \
    --files-without-match $'\xEF\xBB\xBF' \
    $ALL_CODE \
    | grep \
        --perl-regexp "\.(license|txt|icc|qrc|qph|ts|cmake\.in)$" \
        --invert-match \
    | sed 's/^/Missing byte-order-mark: /'

# All header files in src/ should have an “@internal” in
# the Doxygen documentation, because when it would be public,
# the header file would not be in src/
grep \
    --recursive --exclude-dir=testbed \
    --files-without-match $'@internal' \
    src/*.h \
         | sed 's/^/Missing “@internal” statement in non-public header: /'

# The public header files should not use “final” because it cannot be removed
# without breaking binary compatibility.
grep \
    --recursive --exclude-dir=testbed \
    --files-with-matches $'final' \
    $PUBLIC_HEADERS \
         | sed 's/^/“final” should not show up in public headers: /'

# All public header files in include/ should use
# the PERCEPTUALCOLOR_IMPORTEXPORT macro.
grep \
    --recursive --exclude-dir=testbed \
    --files-without-match $'PERCEPTUALCOLOR_IMPORTEXPORT' \
    $PUBLIC_HEADERS \
         | sed 's/^/Missing PERCEPTUALCOLOR_IMPORTEXPORT macro in public header: /'

# All non-public code should not use the PERCEPTUALCOLOR_IMPORTEXPORT macro.
grep \
    --recursive --exclude-dir={testbed,include} \
    --files-with-matches $'PERCEPTUALCOLOR_IMPORTEXPORT' \
    $ALL_CODE \
         | sed 's/^/Internal files may not use PERCEPTUALCOLOR_IMPORTEXPORT macro: /'

# Do not use constexpr in public headers as when we change the value
# later, compile time value and run time value might be different, and
# that might be dangerous.
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "constexpr" \
    $PUBLIC_HEADERS \
         | sed 's/^/Public headers may not use constexpr: /'

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

# -> QLatin1String dates from the pre-Unicode epoch. It should not be used
#    anymore at all, yet to avoid confusion. Though the Clazy codecheck
#    suggests to use QLatin1String() instead of an empty QStringLiteral
#    because of a slight performance gain, this might not compensate
#    for the less readable code. Alternatively, QString() might be
#    used to avoid Clazy warnings.
grep \
    --recursive --exclude-dir=testbed \
    --fixed-strings "QLatin1String" \
    $ALL_CODE \
         | sed 's/^/QLatin1String: Empty → QString(). Non-empty → QStringLiteral.: /'

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





################# Unit tests #################
(\
cd build && nice --adjustment 19 ctest --parallel $PARALLEL_PROCESSES --verbose \
    | grep --invert-match --perl-regexp "^\d+: PASS   : " \
    | grep --invert-match --perl-regexp "^\d+: Test command: " \
    | grep --invert-match --perl-regexp "^\d+: Test timeout computed to be: " \
    | grep --invert-match --perl-regexp "^\d+: \*\*\*\*\*\*\*\*\*" \
    | grep --invert-match --perl-regexp "^\d+: Config: " \
    | grep --invert-match --perl-regexp "^\d+: Totals: " \
    | grep --invert-match --perl-regexp "^\s*\d+/\d+\sTest\s*#\d+:\s*\w+\s*\.*\s*Passed" \
    | grep --invert-match --perl-regexp "^test \d+" \
    | grep --invert-match --perl-regexp "^UpdateCTestConfiguration  from :" \
    | grep --invert-match --perl-regexp "^Test project " \
    | grep --invert-match --perl-regexp "^Constructing a list of tests" \
    | grep --invert-match --perl-regexp "^Done constructing a list of tests" \
    | grep --invert-match --perl-regexp "^Updating test list for fixtures" \
    | grep --invert-match --perl-regexp "^Added \d+ tests to meet fixture requirements" \
    | grep --invert-match --perl-regexp "^Checking test dependency graph\.\.\." \
    | grep --invert-match --perl-regexp "^Checking test dependency graph end" \
    | grep --invert-match --perl-regexp "^\d+:\s*[\.0123456789]* msecs per iteration" \
    | grep --invert-match --perl-regexp "^\d+: RESULT : " \
    | grep --invert-match --perl-regexp "^      Start " \
    | grep --invert-match --perl-regexp "^$"
)
