#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# Add missing BOM #################
# Add a byte-order-mark (\xef\xbb\xbf) to files that do not have one
# but should have it. This is important for the Microsoft compiler,
# because this compiler assumes a legacy one-byte Windows codepage
# whenever no BOM is present. Using a BOM makes it working out-of-the-box
# without relying on Microsoft-only compiler options like “/utf-8”. Details:
# https://devblogs.microsoft.com/cppblog/new-options-for-managing-character-sets-in-the-microsoft-cc-compiler/#dos-donts-and-the-future
#
# Unfortunately, this following command changes the modification date of all
# files, even those that have yet a BOM and don’t need a change. Therefore,
# after calling this script, the next “make” call will rebuild everything.
nice --adjustment 19 sed -i '1s/^\(\xef\xbb\xbf\)\?/\xef\xbb\xbf/' src/*.h src/*.cpp src/include/* autotests/*.cpp tests/* utils/*
