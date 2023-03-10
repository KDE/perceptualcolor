#!/bin/sh

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





echo Messages.sh here.
# $EXTRACT_TR_STRINGS extracts tr() strings.
# “-o” in “find” behaves as “logical OR”.
$EXTRACT_TR_STRINGS `find . -name \*.hpp -o -name \*.cpp -o -name \*.h` -o $podir/perceptualcolor-0_qt.pot
