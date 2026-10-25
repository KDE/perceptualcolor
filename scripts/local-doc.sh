# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail

# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh

rm --recursive --force builddoc
mkdir --parents builddoc
cmake \
    -B builddoc \
    -S . \
    1>> /dev/null
export KDE_DOCS="/usr/share/qt6/doc"
cmake \
    --build builddoc \
    --target prepare_docs \
    --parallel "$PARALLEL_PROCESSES" \
    1>> /dev/null
cmake \
    --build builddoc \
    --target generate_docs \
    --parallel "$PARALLEL_PROCESSES" \
    1>> /dev/null \
    2> >(grep -vE 'Cannot locate index file for dependency|connecting-overloaded-slots|Failed to find qhp.*subprojects\.classes\.indexTitle' >&2)
cmake \
    --build builddoc \
    --target generate_qch \
    --parallel "$PARALLEL_PROCESSES" \
    1>> /dev/null
rm --recursive --force docs/perceptualcolor/
rm --recursive --force docs/perceptualcolor.qch
rm --recursive --force docs/perceptualcolor-0_Release_includes.txt
cp -r builddoc/.doc/* docs
