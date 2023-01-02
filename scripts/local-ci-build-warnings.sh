#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# Configure #################

# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh





################# Run static code analysis #################

# There are various applications available that do style checking, like
# kwstyle (from Kitware) or cpplint (Google coding style). However,
# this project follows KDE’s coding style, which is ensured by clang-format.
# Because clang-format can also auto-correct the source code, it does not
# make sense integrate it here. Use the build target “clang-format” to
# correct automatically your code.

# NOTE cppcheck is currently not used, though there is CMake support for it:
# cmake “-DCMAKE_CXX_CPPCHECK=/usr/bin/cppcheck;–std=c++11” ../path/to/source
# However, it seems that quite some configuration is necessary
# to avoid too many false-positives when running it.

# TODO Add (and compile, but only if necessary) dummy cpp files for each header
#      that does not have an own cpp file. This will make static analysis more
#      reliably.

mkdir --parents build \
    && rm --recursive --force build/* \
    && cd build \
    \
    && echo "clazy, clang-tidy, iwyu started." \
    && cmake \
        -DCMAKE_CXX_COMPILER=clazy \
        -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy \
        -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/usr/bin/iwyu \
            -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
        -DADDITIONAL_WARNINGS=TRUE \
        .. \
        > /dev/null \
    && nice --adjustment 19 \
        make --jobs $PARALLEL_PROCESSES --keep-going \
        > /dev/null \
    && echo "clazy, clang-tidy, iwyu finished." \
    \
    && cd .. \
    && rm --recursive --force build/* \
    \

# Clean the directory even if one of the commands below has failed:
mkdir --parents build \
    && rm --recursive --force build/* \
