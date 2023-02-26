#!/bin/bash

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail





################# Configure #################
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh





################# Add missing BOM #################
# Add a byte-order-mark (\xef\xbb\xbf) to files that do not have one
# but should have it. This is important for the Microsoft compiler,
# because this compiler assumes a legacy one-byte Windows codepage
# whenever no BOM is present. Using a BOM makes it working out-of-the-box
# without relying on Microsoft-only compiler options like “/utf-8”. Details:
# https://devblogs.microsoft.com/cppblog/new-options-for-managing-character-sets-in-the-microsoft-cc-compiler/#dos-donts-and-the-future
#
# Unfortunately, this following command changes the modification date of all
# files, even those that have yet a BOM and don’t need a change. Therefore,
# after calling this script, the next “make” call will rebuild everything.
sed -i '1s/^\(\xef\xbb\xbf\)\?/\xef\xbb\xbf/' src/*.h src/*.cpp src/*.hpp autotests/*.cpp tests/* utils/* examples/example.cpp





################# Clang format #################
# Run within a sub-shell (therefore the parenthesis),
# so that after this we go back to the original working directory.
mkdir --parents build
cd build
cmake -DBUILD_WITH_QT6=ON ..
cmake --build . --parallel $PARALLEL_PROCESSES --target clang-format
cd ..
