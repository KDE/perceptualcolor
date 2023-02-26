#!/bin/bash

# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# Screenshots #################
# -e exits on error,
# -u errors on undefined variables,
# and -o (for option) pipefail exits on command pipe failures
set -euo pipefail
errorcount=0





################# Configure #################
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh





################# Build #################
echo "Build generatescreenshots."
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES
mkdir --parents build
cd build
cmake -DBUILD_WITH_QT6=ON ..
cmake --build . --target generatescreenshots --parallel $PARALLEL_PROCESSES
cd ..
echo "Build generatescreenshots finished."

################# Get fonts #################
# Possible fonts for screenshot generation? As long as the font isn’t in the
# repository, its license doesn’t matter. Otherwise, it has to comply with
# https://community.kde.org/Policies/Licensing_Policy#Policy for either
# public-API sources or application sources. As today’s free software fonts
# use mostly the OFL (Open Font License), this isn’t easy. Here some
# candidates:
# - Linux Libertine
#   https://sourceforge.net/projects/linuxlibertine/
#   Seems to be GPL-2 only, which is not compliant with KDE licensing.
#   GPL is a bad choice for font files (risque that resulting documents
#   must be GPL also.)
#   Has a Sans variant, but isn’t a typical UI font.
# - GNU Freefont
#   https://www.gnu.org/software/freefont/license.html
#   GPL-3-or-later is compliant with KDE licensing.
#   GPL is a bad choice for font files (risque that resulting documents
#   must be GPL also.)
#   Has a Sans variant that might work for UI.
#   Big coverage, but is jsut collection of various free fonts, hasn’t a
#   uniform design across scripts.
# - Victor Mono
#   https://github.com/rubjo/victor-mono/blob/e9c0f111221b7a871b97c51907d9bcbc58b7ce0d/LICENSE
#   Older versions (see link above) where licensed under MIT license.
#   Is a Monospace font, not a typical UI font.
# - ET Book
#   https://github.com/edwardtufte/et-book
#   MIT license.
#   Not a typical UI font (has serifs).
# - Go fonts
#   https://go.dev/blog/go-fonts
#   https://stackoverflow.com/a/40664202
#   https://go.googlesource.com/image/+/refs/heads/master/font/gofont/ttfs/
#   https://github.com/golang/image/blob/master/font/gofont/ttfs/Go-Regular.ttf
#   BSD-3-Clause.
#   Likely the best choice!
if ! [ -f .screenshotfont.ttf ]; then
  wget \
    --no-check-certificate \
    --output-document .screenshotfont.ttf \
    https://github.com/golang/image/raw/b6ac75bc5918c3a0a2200faa20aedebc76d5b349/font/gofont/ttfs/Go-Regular.ttf
fi

################# Run #################
mkdir --parents docs
mkdir --parents docs/pics
rm --recursive --force docs/pics/*
cd docs/pics
echo "Run generatescreenshots."
../../build/utils/generatescreenshots ../../.screenshotfont.ttf
echo "Run generatescreenshots finished."
echo "Run license generator."
for FILE in *; do cp ../../Doxyfile.external.license "$FILE.license"; done
echo "Run license generator finished."
