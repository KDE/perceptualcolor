#!/bin/bash -fxv
# NOTE #!/bin/bash -fxv prints all executed commands.

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0

# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
rm --recursive --force build
scripts/automatic-integration.sh
git diff > artifact_automatic_integration_diff.txt
git reset --hard HEAD
[ -s artifact_automatic_integration_diff.txt ] && ((errorcount++))

echo Terminating ci-automatic-integration.sh with exit code $errorcount.
# NOTE The exit code of the last command is available with $? in the shell.
exit $errorcount
