#!/bin/bash
# NOTE #!/bin/bash -fxv would print all executed commands.

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





errorcount=0

scripts/static-codecheck.sh &> artifact_staticcodecheck
[ -s artifact_staticcodecheck ] || ((errorcount++))

echo cmakelint started.
cmakelint --spaces=4 `find -name "CMakeLists.txt"` > artifact_cmakelint
echo cmakelint finished.
[ -s artifact_cmakelint ] || ((errorcount++))

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
doxygen doxyconf/internaldoc 2>&1 > artifact_doxygen_temp
echo Doxygen “public API and internals” finished.
echo Doxygen “public API” started.
cp Doxyfile.external doxyconf/externaldoc
doxygen -u doxyconf/externaldoc
# Redirect Doxygen’s stderr (2) to stdout (1) to be able to filter it via pipe
doxygen doxyconf/internaldoc 2>&1 >> artifact_doxygen_temp
echo Doxygen “public API” finished.
sort --unique artifact_doxygen_temp  > artifact_doxygen
rm artifact_doxygen_temp
rm --recursive --force doxyconf
[ -s artifact_doxygen ] || ((errorcount++))

echo Build with warnings against Qt5 started.
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES
mkdir --parents build
rm --recursive --force build/*
cd build
cmake \
    -DCMAKE_CXX_COMPILER=clazy \
    -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy \
    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/usr/bin/iwyu \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
    -DADDITIONAL_WARNINGS=TRUE \
    -DBUILD_WITH_QT6=OFF \
    ..
cmake --build . --parallel $PARALLEL_PROCESSES 2>../artifact_warnings_qt5
cd ..
[ -s artifact_warnings ] || ((errorcount++))
echo Build with warnings against Qt5 finished

echo Build with warnings against Qt6 started.
# The “.” command will execute the given script within the context of
# the current script, which is necessary in order to preserve the
# environment variables that are set by the given script.
. scripts/export-environment.sh
echo Number of available CPU threads: $PARALLEL_PROCESSES
mkdir --parents build
rm --recursive --force build/*
cd build
cmake \
    -DCMAKE_CXX_COMPILER=clazy \
    -DCMAKE_CXX_CLANG_TIDY=/usr/bin/clang-tidy \
    -DCMAKE_CXX_INCLUDE_WHAT_YOU_USE=/usr/bin/iwyu \
        -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE \
    -DADDITIONAL_WARNINGS=TRUE \
    -DBUILD_WITH_QT6=ON \
    ..
cmake --build . --parallel $PARALLEL_PROCESSES 2>../artifact_warnings_qt6
cd ..
[ -s artifact_warnings ] || ((errorcount++))
echo Build with warnings against Qt6 finished

scripts/automatic-integration.sh
git diff > artifact_automatic_integration_diff
[ -s artifact_automatic_integration_diff ] || ((errorcount++))

echo Terminating continuous integration with exit code $errorcount.

# NOTE The exit code of the last command is available with $? in the shell.

exit $errorcount
