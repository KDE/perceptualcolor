#!/bin/bash

# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: MIT





################# CI: Continuous integration #################
ulimit -Sv 2000000

# Define the number of parallel processes to be used.
# NOTE LWYU is not affected because unfortunately it is necessary
# to run it with only 1 process to be sure that error messages
# are printed at the correct position below the linker command
# that indicates the affected file.
PARALLEL_PROCESSES="8"

# Define which checks we want to run. We run at level 2 (highest possible
# number of warnings). Specific checks can be disabled. Example:
# no-qproperty-without-notify will disable the qproperty-without-notify
# check. Currently disabled are:
# • no-inefficient-qlist-soft: In Qt6, QList and QVector will be aliases
#   anyway. And, in Qt6 QList will be the default type. So we will follow
#   this recommendation and always use QList.
#
# There are some more checks that are not part of any level and have to be
# enabled manually. We enable all of them (that are available at the time
# when writing this script), with the following exceptions:
# • inefficient-qlist: In Qt6, QList and QVector will be aliases
#   anyway. And, in Qt6 QList will be the default type. So we will follow
#   this recommendation and always use QList. Anyway, in newer clazy
#   versions it is not available anymore anyway.
# • empty-qstringliteral: QLatin1String is a little bit faster, but we try
#   to avoid non-Unicode encoding wherever we can, for simplicity.
# • jni-signatures: When enabling this check, the whole build won’t work
#   anymore (at least as long as you won’t build for Android).
# • ifndef-define-typo: Shows many false-positives for Qt headers that we are
#   including.
# • qt6-header-fixes: Generates warnings also for Qt5 headers we are
#   currently still using.
# • qt6-fwd-fixes: Produces warnings that are not too useful while still
#   using Qt5. And also, we are using IWYU, which is quite pedantic about
#   which header to include and which one not, and IWYU gives probably the
#   more exact information.
export CLAZY_CHECKS="level2,\
\
no-ctor-missing-parent-argument,\
no-empty-qstringliteral,\
\
assert-with-side-effects,\
container-inside-loop,\
detaching-member,\
heap-allocated-small-trivial-type,\
isempty-vs-count,\
qhash-with-char-pointer-key,\
qproperty-type-mismatch,\
qrequiredresult-candidates,\
qstring-varargs,\
qt4-qstring-from-array,\
qt6-deprecated-api-fixes,\
qt6-qhash-signature,\
qt6-qlatin1stringchar-to-u,\
qt-keywords,\
qvariant-template-instantiation,\
raw-environment-function,\
reserve-candidates,\
signal-with-return-value,\
thread-with-slots,\
tr-non-literal,\
unneeded-cast,\
use-chrono-in-qtimer"

# All paths that match this regular expression will be ignored. This is
# necessary to prevent Clazy from generating errors for Qt headers.
export CLAZY_IGNORE_DIRS="(.*/qt5/QtGui/.*)|(.*/qt5/QtCore/.*)|(.*/qt5/QtWidgets/.*)"
