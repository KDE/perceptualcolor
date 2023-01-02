#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# Configure #################

# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh





################# Run static code analysis #################
# NOTE LWYU uses only 1 process to make sure that error messages
# are printed at the correct position below the linker command
# that indicates the affected file.

mkdir --parents build \
    && rm --recursive --force build/* \
    && cd build \
    \
    && echo "LWYU (link what you use) started." \
    && cmake -DCMAKE_LINK_WHAT_YOU_USE=TRUE .. > /dev/null \
    && nice --adjustment 19 \
        make --jobs 1 all 2>&1 \
        | grep --invert-match --perl-regexp "^\[ *[0-9]*\%\] [^L]" \
        | grep --invert-match --perl-regexp "^Scanning" \
    && echo "LWYU (link what you use) finished." \
    && cd .. \
    && rm --recursive --force build/* \
    \

# Clean the directory even if one of the commands below has failed:
mkdir --parents build \
    && rm --recursive --force build/* \
