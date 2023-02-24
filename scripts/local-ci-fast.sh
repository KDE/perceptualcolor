#!/bin/bash

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# Documentation #################
# This scripts runs locally some reasonable fast parts of the
# Continuous Integration done online.





################# Configure #################

# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh





################# CMakeLists.txt linter #################
# “2> /dev/null” throws away stderr, at which cmakelint only outputs
# the count of errors. However, stdout is preserved, which is the really
# interesting part, at which cmakelint outputs detailed errors.
echo cmakelint started.
cmakelint --spaces=4 `find -name "CMakeLists.txt"`
echo cmakelint finished.
#
# An alternative linter for CMake is available at
# https://github.com/cheshirekow/cmake_format
# but our current linter works fine, so no need to change.





################# Doxygen #################
# Before generating the new Doxygen documentation we
# make sure old data is deleted. This is important because otherwise,
# old documentation for classes that do not exist anymore, or old
# screenshots that are not used anymore, would stay in the directory
# and occupy space:
rm --recursive --force docs/publicapi
rm --recursive --force docs/publicapiandinternals
# Redirect Doxygen’s stderr (2) to stdout (1) to be able to filter it via pipe
nice --adjustment 19 doxygen Doxyfile.internal 2>&1 \
    | sed 's/^/Doxygen “public API and internals”: /'
nice --adjustment 19 doxygen Doxyfile.external 2>&1 \
    | sed 's/^/Doxygen “public API”: /'





################# Compliance with REUSE specification #################
# Test if we provide all licenses as required by the “reuse” specification.
# This check needs the “reuse” application installed in your local bin
# directory. If you do not have that, you can install it with:
# pip3 install --user reuse
# Then, you have to make available $HOME/.local/bin/reuse in your path.
# Or, you can install it as root:
# sudo pip3 install reuse
# Then, you do not have to add it manually to the path.
nice --adjustment 19 reuse lint > /dev/null
if [ $? -eq 0 ];
then
    # Everything is fine. No message is printed.
    echo
else
    # “reuse lint” found problems. We call it again to print its messages.
    nice --adjustment 19 reuse lint
fi





################# Static code check #################
scripts/static-codecheck.sh





################# Build from scratch #################

# Deleting completely the old build makes sure that the new build
# will be done with the default settings that are used here.
# Furthermore, it makes sure that the translations are updated,
# which only happens when CMake is run again.

# -DCMAKE_AUTOGEN_PARALLEL=$PARALLEL_PROCESSES would speed up MOC
# but unfortunately breaks the Qt Linguist, so we don’t do it.
(
while true; do
    read -p "Delete existing build and build from scratch with additional warnings? (yes/NO) " yn
    case $yn in
        yes ) mkdir --parents build \
                && rm --recursive --force build/* \
                && cd build \
                && cmake -DADDITIONAL_WARNINGS=TRUE .. > /dev/null
            exit;;
        * ) exit;;
    esac
done
)





################# Build the project and run Unit Tests #################
# Run within a sub-shell (therefore the parenthesis),
# so that after this we go back to the original working directory.
( \
mkdir --parents build \
    && cd build \
    && nice --adjustment 19 cmake --build . --parallel $PARALLEL_PROCESSES > /dev/null \
    && nice --adjustment 19 ctest --parallel $PARALLEL_PROCESSES --verbose \
    | grep --invert-match --perl-regexp "^\d+: PASS   : " \
    | grep --invert-match --perl-regexp "^\d+: Test command: " \
    | grep --invert-match --perl-regexp "^\d+: Test timeout computed to be: " \
    | grep --invert-match --perl-regexp "^\d+: \*\*\*\*\*\*\*\*\*" \
    | grep --invert-match --perl-regexp "^\d+: Config: " \
    | grep --invert-match --perl-regexp "^\d+: Totals: " \
    | grep --invert-match --perl-regexp "^\s*\d+/\d+\sTest\s*#\d+:\s*\w+\s*\.*\s*Passed" \
    | grep --invert-match --perl-regexp "^test \d+" \
    | grep --invert-match --perl-regexp "^UpdateCTestConfiguration  from :" \
    | grep --invert-match --perl-regexp "^Test project " \
    | grep --invert-match --perl-regexp "^Constructing a list of tests" \
    | grep --invert-match --perl-regexp "^Done constructing a list of tests" \
    | grep --invert-match --perl-regexp "^Updating test list for fixtures" \
    | grep --invert-match --perl-regexp "^Added \d+ tests to meet fixture requirements" \
    | grep --invert-match --perl-regexp "^Checking test dependency graph\.\.\." \
    | grep --invert-match --perl-regexp "^Checking test dependency graph end" \
    | grep --invert-match --perl-regexp "^\d+:\s*[\.0123456789]* msecs per iteration" \
    | grep --invert-match --perl-regexp "^\d+: RESULT : " \
    | grep --invert-match --perl-regexp "^      Start " \
    | grep --invert-match --perl-regexp "^$"
)
