#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# NOTE This script serves severel purposes at one:
# - Check if IPO/LTO compiles correctly (by building with IPO/LTO enabled)
# - Extensive ompiler warnings (by using the ADDITIONAL_WARNING flag)
# - Clazy warnings
# - Clang-tidy warnings

# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

echo "Warnings codecheck started."
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES
rm --recursive --force build
rm --recursive --force build_qch
mkdir --parents build
cd build
# NOTE -DQHelpGenerator_EXECUTABLE=/usr/lib/qt5/bin/qhelpgenerator seems to be
# necessary to find qhelpgenerator on the Gitlab CI system and other
# systems where “if (TARGET Qt5::qhelpgenerator)” is not available.
cmake \
    -DCMAKE_CXX_COMPILER=clazy \
    -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy \
    -DADDITIONAL_WARNINGS=ON \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE \
    -DBUILD_QCH=ON \
    -DQHelpGenerator_EXECUTABLE=/usr/lib/qt5/bin/qhelpgenerator \
    -D_qmake_executable_default=/usr/lib/qt5/bin/qmake \
    ..
export XDG_RUNTIME_DIR="/tmp/runtime-root"
cmake --build . --target perceptualcolor-0_QCH --parallel $PARALLEL_PROCESSES 2>../artifact_warnings.txt
cd ..
# Copy the QCH build to a directory that will not be deleted and therefore
# availeble as artifact.
cp -r build build_qch
cd build
cmake --build . --parallel $PARALLEL_PROCESSES 2>../artifact_warnings.txt
# It is necessary to install this, because otherwise the following
# build of the “examples” would fail.
# Furtherfore, this is also a test if QCH is installed without errors.
cmake --build . --target install --parallel $PARALLEL_PROCESSES
cd ..
rm --recursive --force buildexamples
mkdir --parents buildexamples
cd buildexamples
cmake \
    -DCMAKE_CXX_COMPILER=clazy \
    -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy \
    -DADDITIONAL_WARNINGS=ON \
    -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=TRUE \
    ../examples
cmake --build . --parallel $PARALLEL_PROCESSES 2>>../artifact_warnings.txt
cd ..
[ -s artifact_warnings.txt ] && ((errorcount++))
echo "Warnings codecheck finished."

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
