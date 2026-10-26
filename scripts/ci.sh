#!/bin/bash

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# Use “totalerrors” and not “errorcount” like in the called scripts,
# to avoid conflicts.
totalerrors=0

# Start the slowest task (ci-doc.sh) immediately in the background
echo "Starting ci-doc.sh in background …"
scripts/ci-doc.sh > ci-doc.log 2>&1 &
DOCS_PID=$!

# Run the remaining tasks sequentially

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
exitcode_format=$exitcode

echo "Starting ci-warnings.sh …"
scripts/ci-warnings.sh
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-warnings.sh finished with exit code $exitcode."
exitcode_warnings=$exitcode

# Wait for the background process (ci-doc.sh) to finish
# and collect its exit code
echo "Waiting for ci-doc.sh to finish …"
wait "$DOCS_PID"
# The "wait" command returns the exit code of the process "$DOCS_PID", even
# if "$DOCS_PID" has yet been terminated in the past.
exitcode=$?
totalerrors=$((totalerrors + exitcode))
echo "ci-doc.sh finished with exit code $exitcode."
exitcode_doc=$exitcode
echo "--- Output of ci-doc.sh (ci-doc.log) ---"
cat ci-doc.log
echo "----------------------------------------"

# Delete empty artifacts
echo "BEGIN list of artifacts"
for file in artifact_*
do
  if [ -s "$file" ]
  then
    echo "$file"
  else
    rm -f "$file"
  fi
done
echo "END list of artifacts"

echo "Exit codes:"
echo "staticcodecheck: $exitcode_staticcodecheck"
echo "cmakelint: $exitcode_cmakelint"
echo "format: $exitcode_format"
echo "doc: $exitcode_doc"
echo "warnings: $exitcode_warnings"
echo
echo "Terminating ci.sh with exit code $totalerrors."
# NOTE The exit code of the last command is available with $? in the shell.
exit $totalerrors
