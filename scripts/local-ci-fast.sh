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
echo cmakelint started.
cmakelint \
    --spaces=4 \
    `find -name "CMakeLists.txt" -not -path "./build/*" -not -path "./buildclangformat/*"`
echo cmakelint finished.
#
# An alternative linter for CMake is available at
# https://github.com/cheshirekow/cmake_format
# but our current linter works fine, so no need to change.





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
    read -p "Delete existing build and build from scratch with additional warnings? (YES/no) " yn
    case $yn in
        no ) exit;;
        * ) mkdir --parents build \
                && rm --recursive --force build/* \
                && cd build \
                && cmake -DADDITIONAL_WARNINGS=ON .. > /dev/null
            exit;;
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
    | grep --invert-match --perl-regexp "^Total Test time" \
    | grep --invert-match --perl-regexp "^Parse Config file:.*DartConfiguration.tcl$" \
    | grep --invert-match --perl-regexp "^\d+: Working Directory: " \
    | grep --invert-match --perl-regexp "^      Start " \
    | grep --invert-match --perl-regexp "^$"
# To filter out even benchmark results:
#     | grep --invert-match --perl-regexp "^\d+:\s*[\.0123456789]* msecs per iteration" \
#     | grep --invert-match --perl-regexp "^\d+: RESULT : " \
)
