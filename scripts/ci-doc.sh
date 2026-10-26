#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

# Run generatescreenshots. This makes sure that code documentation
# builds correctly also after a re-run of generatescreenshots.
rm --recursive --force buildgeneratescreenshots
scripts/update-screenshots.sh

# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES

echo "Run code documentation."
rm --recursive --force builddoc
mkdir --parents builddoc
cmake \
    -B builddoc \
    -S .
export KDE_DOCS="/usr/share/qt6/doc"
rm --force artifact_doc.txt
cmake \
    --build builddoc \
    --target prepare_docs \
    --parallel "$PARALLEL_PROCESSES" \
    2> artifact_doc.txt
cmake \
    --build builddoc \
    --target generate_docs \
    --parallel "$PARALLEL_PROCESSES" \
    2> >(grep -vE 'Cannot locate index file for dependency|connecting-overloaded-slots|Failed to find qhp.*subprojects\.classes\.indexTitle' > artifact_doc.txt)
cmake \
    --build builddoc \
    --target generate_qch \
    --parallel "$PARALLEL_PROCESSES" \
    2>> artifact_doc.txt
cp -r builddoc/.doc artifact_docbuild
rm --recursive --force builddoc
[ -s artifact_doc.txt ] && ((errorcount++))
echo "Run code documentation finished."

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
