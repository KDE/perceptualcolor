# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause





################# Tool launching the color dialog #################
add_executable(perceptualcolorpicker)
# target_sources should normally always use PRIVATE. Details:
# crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources
target_sources(
    perceptualcolorpicker
    PRIVATE main.cpp)
target_link_libraries(
    perceptualcolorpicker
    # Transitive dependencies (PUBLIC, INTERFACE) don’t make sense
    # for executables, since you  can’t link against an executable.
    # Therefore using PRIVAT:
    PRIVATE "perceptualcolorinternal-${MAJOR_VERSION}" ${LCMS2_LIBRARIES})
install(TARGETS perceptualcolorpicker)

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
install(TARGETS perceptualcolorgeneratescreenshots)
