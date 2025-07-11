#!/bin/bash

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# Use “totalerrors” and not “errorcount” like in the called scripts,
# to avoid conflicts.
totalerrors=0

echo "Starting ci-staticcodecheck.sh …"
scripts/ci-staticcodecheck.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-staticcodecheck.sh finished with exit code $exitcode."
exitcode_staticcodecheck=$exitcode

echo "Starting ci-cmakelint.sh …"
scripts/ci-cmakelint.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-cmakelint.sh finished with exit code $exitcode."
exitcode_cmakelint=$exitcode

echo "Starting ci-format.sh …"
scripts/ci-format.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-format.sh finished with exit code $exitcode."
exitcode_automatic_integration=$exitcode

echo "Starting ci-doxygen.sh …"
scripts/ci-doxygen.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-doxygen.sh finished with exit code $exitcode."
exitcode_doxygen=$exitcode

echo "Starting ci-ipo-lto …"
scripts/ci-ipo-lto.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-ipo-lto finished with exit code $exitcode."
exitcode_ipo_lto=$exitcode

echo "Starting ci-qch.sh …"
scripts/ci-qch.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-qch.sh finished with exit code $exitcode."
exitcode_qch=$exitcode

echo "Starting ci-warnings.sh …"
scripts/ci-warnings.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-warnings.sh finished with exit code $exitcode."
exitcode_warnings=$exitcode

echo "Starting ci-qt5 …"
scripts/ci-qt5.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-qt5 finished with exit code $exitcode."
exitcode_qt5=$exitcode

echo "Starting ci-iwyu.sh …"
scripts/ci-iwyu.sh
exitcode=$?
# NOTE Do NOT count this exit code. There is a separate job that will
# use iwyu artifacts later…
echo "ci-iwyu.sh finished with exit code $exitcode."
exitcode_iwyu=$exitcode

# Delete empty artifacts
echo "BEGIN list of artifacts"
for file in artifact_*
do
  if [ -s "$file" ]
  then
    echo "$file"
  else
    rm "$file"
  fi
done
echo "END list of artifacts"

echo "Exit codes:"
echo "staticcodecheck: $exitcode_staticcodecheck"
echo "cmakelint: $exitcode_cmakelint"
echo "automatic_integration: $exitcode_automatic_integration"
echo "doxygen: $exitcode_doxygen"
echo "ipo_lto: $exitcode_ipo_lto"
echo "qch: $exitcode_qch"
echo "warnings: $exitcode_warnings"
echo "qt5: $exitcode_qt5"
echo "iwyu: $exitcode_iwyu (ignored)"
echo
echo "Terminating ci.sh with exit code $totalerrors."
# NOTE The exit code of the last command is available with $? in the shell.
exit $totalerrors
