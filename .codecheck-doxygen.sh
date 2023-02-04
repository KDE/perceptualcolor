#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

# Doxygen run. We let Doxygen update the config files, so that no
# warnings for deprecated options will ever be issued. This seems
# useful, because it is likely to have different Doxygen versions
# on the developer’s system and the CI system.
mkdir --parents doxyconf
rm --recursive --force doxyconf/*
echo Doxygen “public API and internals” started.
cp Doxyfile.internal doxyconf/internaldoc
doxygen -u doxyconf/internaldoc
# Redirect Doxygen’s stderr (2) to stdout (1) to be able to filter it via pipe
doxygen doxyconf/internaldoc > artifact_doxygen_temp 2>&1
echo Doxygen “public API and internals” finished.
echo Doxygen “public API” started.
cp Doxyfile.external doxyconf/externaldoc
doxygen -u doxyconf/externaldoc
# Redirect Doxygen’s stderr (2) to stdout (1) to be able to filter it via pipe
doxygen doxyconf/internaldoc >> artifact_doxygen_temp 2>&1
echo Doxygen “public API” finished.
sort --unique artifact_doxygen_temp  > artifact_doxygen.txt
rm artifact_doxygen_temp
rm --recursive --force doxyconf
[ -s artifact_doxygen ] && ((errorcount++))

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
