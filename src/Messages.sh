#!/bin/sh

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





echo Messages.sh here.
# $EXTRACT_TR_STRINGS extracts tr() strings.
# “-o” in “find” behaves as “logical OR”.
$EXTRACT_TR_STRINGS `find . -name \*.hpp -o -name \*.cpp -o -name \*.h` -o $podir/perceptualcolor-0_qt.pot
