#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# Add missing BOM #################
# Add a byte-order-mark (\xef\xbb\xbf) to files that does not have one 
# but should have it.
# (Unfortunately, this touches all files, even those that don’t need a change.
# Therefore, after calling this script, the next “make” call will rebuild
# everything.
nice --adjustment 19 sed -i '1s/^\(\xef\xbb\xbf\)\?/\xef\xbb\xbf/' lib/src/* lib/include/perceptualcolor-0/* unittests/*.cpp tools/*
