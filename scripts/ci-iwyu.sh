#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

echo "iwyu (include what you use) against Qt6 started."
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
    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="/usr/bin/iwyu;-Xiwyu;--verbose=1" \
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
    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE="/usr/bin/iwyu;-Xiwyu;--verbose=1" \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
    -DBUILD_WITH_QT6=ON \
    ../examples
cmake --build . --parallel $PARALLEL_PROCESSES 2>>../artifact_iwyu.txt

# Remove noise from the iwyu result.
#
# “sed -i” modified the file directly.
#
# Remove from the line with “The full include-list for” up to the line before
# the line that contains “---”.
sed -i '/^The full include-list for \//,/^---$/ { /^---$/!d }' ../artifact_iwyu.txt
# Remove comments for correct files.
sed -i '/(.*has correct #includes\/fwd-decls)/d' ../artifact_iwyu.txt
# Remove noisy text.
sed -i '/^Warning: include-what-you-use reported diagnostics:$/d' ../artifact_iwyu.txt
# Keep “should add/remove these lines” only if the next line is not empty.
sed -i '/should \(add\|remove\) these lines:$/{
N
/^\(.*\)\n$/d
}' ../artifact_iwyu.txt

cd ..
[ -s artifact_iwyu.txt ] && ((errorcount++))
echo "iwyu (include what you use) against Qt6 finished."

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
