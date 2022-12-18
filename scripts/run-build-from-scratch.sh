#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# CI: Continuous integration #################
ulimit -Sv 2000000

# Define the number of parallel processes to be used.
PARALLEL_PROCESSES="6"





################# Build the project #################
# Run within a sub-shell (therefore the parenthesis),
# so that after this we go back to the original working directory.
# Deleting completely the old build makes sure that the new build
# will be done with the default settings that are used here.
mkdir --parents build && rm --recursive --force build/*
( \
mkdir --parents build \
    && cd build \
    && nice --adjustment 19 cmake ../ \
    && nice --adjustment 19 make --jobs $PARALLEL_PROCESSES
)
