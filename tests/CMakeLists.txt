# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# Tool launching the color dialog #################
add_executable(perceptualcolorpicker)
# target_sources should normally always use PRIVATE. Details:
# crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources
target_sources(
    perceptualcolorpicker
    PRIVATE perceptualcolorpicker.cpp)
target_link_libraries(
    perceptualcolorpicker
    # Transitive dependencies (PUBLIC, INTERFACE) don’t make sense
    # for executables, since you can’t link against an executable.
    # Therefore using PRIVAT:
    PRIVATE "perceptualcolor-${MAJOR_VERSION}" ${LCMS2_LIBRARIES})





################# Test tool #################
add_executable(testapp)
# target_sources should normally always use PRIVATE. Details:
# crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources
target_sources(
    testapp
    PRIVATE testapp.cpp)
target_link_libraries(
    testapp
    # Transitive dependencies (PUBLIC, INTERFACE) don’t make sense
    # for executables, since you can’t link against an executable.
    # Therefore using PRIVAT:
    PRIVATE "perceptualcolorinternal-${MAJOR_VERSION}" ${LCMS2_LIBRARIES})
