#!/bin/bash

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# Screenshots #################
mkdir --parents docs
mkdir --parents docs/pics
rm --recursive --force docs/pics/*
# Run generatescreenshots within docs/pics working
# directory, but within a sub-shell (therefore the parenthesis),
# so that after this we go back to the original working directory.
( \
cd docs/pics \
    && echo "generatescreenshots started." \
    && ../../build/utils/generatescreenshots \
    && echo "generatescreenshots finished." \
    && for FILE in *; do cp ../../Doxyfile.external.license "$FILE.license"; done
)
