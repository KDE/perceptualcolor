#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

echo "QCH build started."
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES
rm --recursive --force build_qch
mkdir --parents build_qch
cd build_qch
# NOTE -DQHelpGenerator_EXECUTABLE=/usr/lib/qt5/bin/qhelpgenerator seems to be
# necessary to find qhelpgenerator on the Gitlab CI system and other
# systems where “if (TARGET Qt5::qhelpgenerator)” is not available.
cmake -DBUILD_WITH_QT6=ON -DBUILD_QCH=ON -DQHelpGenerator_EXECUTABLE=/usr/lib/qt5/bin/qhelpgenerator -D_qmake_executable_default=/usr/lib/qt5/bin/qmake ..
export XDG_RUNTIME_DIR="/tmp/runtime-root"
cmake --build . --target perceptualcolor-0_QCH --parallel $PARALLEL_PROCESSES 2>../artifact_warnings_qch.txt
# Also make an installation to test if everything works fine. Unfortunately,
# this triggers a complete build of the whole project, but this seems
# unavoidable.
cmake --build . --target install --parallel $PARALLEL_PROCESSES
cd ..
[ -s artifact_warnings_qch.txt ] && ((errorcount++))
echo "QCH build finished."

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
