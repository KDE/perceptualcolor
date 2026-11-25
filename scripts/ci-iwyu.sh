#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

echo "iwyu (include what you use) started."
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
    ../examples
cmake --build . --parallel $PARALLEL_PROCESSES 2>../artifact_iwyu_examples.txt

cp ../artifact_iwyu.txt ../artifact_iwyu_simplified.txt
# Remove noise from the iwyu result.
#
# “sed -i” modifies the file directly.
#
# Remove from the line with “The full include-list for” up to the line before
# the line that contains “---”.
sed -i '/^The full include-list for \//,/^---$/ { /^---$/!d }' ../artifact_iwyu_simplified.txt
# Remove comments for correct files.
sed -i '/(.*has correct #includes\/fwd-decls)/d' ../artifact_iwyu_simplified.txt
# Remove warnings about wrong mapping in some header files of external libs
sed -i '/no private include name for @headername mapping/d' ../artifact_iwyu_simplified.txt
# Remove noisy text.
sed -i '/^Warning: include-what-you-use reported diagnostics:$/d' ../artifact_iwyu_simplified.txt
# Keep “should add/remove these lines” only if the next line is not empty.
sed -i '/should \(add\|remove\) these lines:$/{
N
/^\(.*\)\n$/d
}' ../artifact_iwyu_simplified.txt || true

cd ..

# We use fix_include to get a diff file that will be available in the
# artifacts and can be used later to apply changes manually on a local
# machine.
# NOTE fix_include applies some of the suggestions in the IWYU file, but does
# not apply others. Espacially, suggestions to remove something are often
# ignored by fix_include. We run it nevertheless because it makes applying
# the suggestions so much easier. Better to have an incomplete diff than
# not having a diff at all.
# WARNING The diff must be revied manually before applying the changes and
# comitting them. fix_include has the tendency to reorder lines, sometimes
# remove comments it thinks that they belong to an include that has to be
# deleted. Especially, fix_include breaks sometimes that pattern that we
# include always the own header as the very fist include to make sure the
# header is self-contained.

# The appended “|| true” will make the script ignore error codes of
# fix_include. This is necesarry because fix_include returns as error code
# the number of modified files (or 100, whichever is smaller). However, the
# script shall not abort, but continue to run and generate the diff.
fix_include --noreorder < artifact_iwyu.txt || true
scripts/format.sh
git diff > artifact_iwyu_diff.txt
git reset --hard HEAD

fix_include --noreorder < artifact_iwyu_examples.txt || true
scripts/format.sh
git diff > artifact_iwyu_examples_diff.txt
git reset --hard HEAD

# Delete empty artifact
if [ ! -s artifact_iwyu.txt ]; then
  rm -f artifact_iwyu.txt
fi
if [ ! -s artifact_iwyu_simplified.txt ]; then
  rm -f artifact_iwyu_simplified.txt
fi
if [ ! -s artifact_format_diff.txt ]; then
  rm -f artifact_format_diff.txt
fi
if [ ! -s artifact_iwyu_diff.txt ]; then
  rm -f artifact_iwyu_diff.txt
fi
if [ ! -s artifact_iwyu_examples.txt ]; then
  rm -f artifact_iwyu_examples.txt
fi
if [ ! -s artifact_iwyu_examples_diff.txt ]; then
  rm -f artifact_iwyu_examples_diff.txt
fi

[ -s artifact_iwyu.txt ] && errorcount=$((errorcount + 1))
[ -s artifact_iwyu_simplified.txt ] && errorcount=$((errorcount + 1))
[ -s artifact_format_diff.txt ] && errorcount=$((errorcount + 1))
[ -s artifact_iwyu_diff.txt ] && errorcount=$((errorcount + 1))
[ -s artifact_iwyu_examples.txt ] && errorcount=$((errorcount + 1))
echo "iwyu (include what you use) finished."

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
