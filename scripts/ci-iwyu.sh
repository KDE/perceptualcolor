#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

echo "Dynamic codecheck against Qt6 started."
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES
rm --recursive --force build
mkdir --parents build
cd build
# iwyu is based on clang internals, therefore forcing usage of clang.
cmake \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/usr/bin/iwyu \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
    -DBUILD_WITH_QT6=ON \
    ..
cmake --build . --parallel $PARALLEL_PROCESSES 2>../artifact_iwyu.txt
make install
cd ..
rm --recursive --force buildexamples
mkdir --parents buildexamples
cd buildexamples
# iwyu is based on clang internals, therefore forcing usage of clang.
cmake \
    -DCMAKE_CXX_COMPILER=/usr/bin/clang++ \
    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/usr/bin/iwyu \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
    -DBUILD_WITH_QT6=ON \
    ../examples
cmake --build . --parallel $PARALLEL_PROCESSES 2>>../artifact_iwyu.txt
cd ..
[ -s artifact_iwyu.txt ] && ((errorcount++))
echo "Dynamic codecheck against Qt6 finished."

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
