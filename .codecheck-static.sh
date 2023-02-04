#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

scripts/static-codecheck.sh &> artifact_staticcodecheck.txt
[ -s artifact_staticcodecheck.txt ] && ((errorcount++))

echo Terminating continuous integration with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
