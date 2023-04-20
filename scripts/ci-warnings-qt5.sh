#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

echo "Dynamic codecheck against Qt5 started."
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES
mkdir --parents build
rm --recursive --force build/*
cd build
cmake \
    -DCMAKE_CXX_COMPILER=clazy \
    -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
    -DADDITIONAL_WARNINGS=ON \
    -DBUILD_WITH_QT6=OFF \
    ..
cmake --build . --parallel $PARALLEL_PROCESSES 2>../artifact_warnings_qt5.txt
make install
cd ..
[ -s artifact_warnings_qt5.txt ] && ((errorcount++))
echo "Dynamic codecheck against Qt5 finished."

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount