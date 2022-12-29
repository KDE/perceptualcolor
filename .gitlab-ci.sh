#!/bin/bash
# NOTE #!/bin/bash -fxv would print all executed commands.

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





errorcount=0

if ./.gitlab-ci-staticcodecheck.sh 2>&1 | grep '^'; then
    # there was some output"
    ((errorcount++))
fi

echo cmakelint started.
cmakelint --spaces=4 `find -name "CMakeLists.txt"` || ((errorcount++))
echo cmakelint finished.

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is always available with $?

exit $errorcount
