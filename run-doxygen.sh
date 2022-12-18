#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# CI: Continuous integration #################
ulimit -Sv 2000000

PARALLEL_PROCESSES="6"





################# Doxygen #################
# This section will generate up-to-date screenshots and 
# new Doxygen documentation.
mkdir --parents docs
mkdir --parents docs/pics
# Make sure old data is deleted. This is important because otherwise,
# old documentation for classes that do not exist anymore, or old
# screenshots that are not used anymore, would stay in the directory
# and occupy space:
rm --recursive --force docs/screenshots/*
rm --recursive --force docs/publicapi
rm --recursive --force docs/publicapiandinternals
# Run generatescreenshots within docs/pics working
# directory, but within a sub-shell (therefore the parenthesis),
# so that after this we go back to the original working directory.
( \
cd docs/pics \
    && ../../build/tools/perceptualcolorgeneratescreenshots \
    && for FILE in *; do cp ../../Doxyfile.external.license "$FILE.license"; done
)
# We are not interested in the normal Doxygen output, but only in the errors.
# We have to filter the errors, because Doxygen produces errors where it
# should not (https://github.com/doxygen/doxygen/issues/7411 errors on
# missing documentation of return value for functions that return “void”).
# Therefore, first we redirect Doxygen’s stderr to stdout (the pipe) to
# be able to filter it with grep. And we redirect stdout to /dev/null
# (without changing where stderr is going):
nice --adjustment 19 doxygen Doxyfile.internal 2>&1 >/dev/null \
    | grep \
        --invert-match \
        --perl-regexp "warning: return type of member .* is not documented" \
           | sed 's/^/Doxygen “public API and internals”: /'
nice --adjustment 19 doxygen Doxyfile.external 2>&1 >/dev/null \
    | grep \
        --invert-match \
        --perl-regexp "warning: return type of member .* is not documented" \
           | sed 's/^/Doxygen “public API”: /'
