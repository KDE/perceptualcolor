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
rm --recursive --force docs/pics/*
rm --recursive --force docs/publicapi
rm --recursive --force docs/publicapiandinternals
# Run generatescreenshots within docs/pics working
# directory, but within a sub-shell (therefore the parenthesis),
# so that after this we go back to the original working directory.
( \
cd docs/pics \
    && nice --adjustment 19 ../../build/utils/generatescreenshots \
    && for FILE in *; do cp ../../Doxyfile.external.license "$FILE.license"; done
)
# We redirect Doxygen’s stderr to stdout (the pipe) to be able to filter it
nice --adjustment 19 doxygen Doxyfile.internal 2>&1 \
    | sed 's/^/Doxygen “public API and internals”: /'
nice --adjustment 19 doxygen Doxyfile.external 2>&1 \
    | sed 's/^/Doxygen “public API”: /'
