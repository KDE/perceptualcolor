# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause





################# Tool generating screenshots of widgets #################
add_executable(perceptualcolorgeneratescreenshots)
# target_sources should normally always use PRIVATE. Details:
# crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources
target_sources(
    perceptualcolorgeneratescreenshots
    PRIVATE generatescreenshots.cpp)
target_link_libraries(
    perceptualcolorgeneratescreenshots
    # Transitive dependencies (PUBLIC, INTERFACE) don’t make sense
    # for executables, since you  can’t link against an executable.
    # Therefore using PRIVAT:
    PRIVATE "perceptualcolorinternal-${MAJOR_VERSION}")
