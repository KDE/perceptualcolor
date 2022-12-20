# SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause





################# Version number #################
# Substitutes all occurrences of @MAJOR_VERSION@… and similar declarations
# in the source code file by the actual value in CMake:
configure_file(
    # Input:
    include/version.in.hpp
    # Output:
    ${CMAKE_CURRENT_BINARY_DIR}/generated/include/version.h
)
# TODO Also configure rpm.spec and Doxygen files with this version information.





################# Setup source code #################
# Set the sources for our library
set(library_SRC
    abstractdiagram.cpp
    asyncimageproviderbase.cpp
    asyncimagerendercallback.cpp
    asyncimagerenderthread.cpp
    chromahuediagram.cpp
    chromahueimageparameters.cpp
    chromalightnessdiagram.cpp
    chromalightnessimageparameters.cpp
    colordialog.cpp
    colorpatch.cpp
    colorwheel.cpp
    colorwheelimage.cpp
    extendeddoublevalidator.cpp
    gradientimageparameters.cpp
    gradientslider.cpp
    helper.cpp
    helperconstants.cpp
    helperconversion.cpp
    initializetranslation.cpp
    interlacingpass.cpp
    iohandlerfactory.cpp
    languagechangeeventfilter.cpp
    lchadouble.cpp
    lchdouble.cpp
    multicolor.cpp
    multispinbox.cpp
    multispinboxsectionconfiguration.cpp
    palettewidget.cpp
    polarpointf.cpp
    refreshiconengine.cpp
    rgbcolorspace.cpp
    rgbcolorspacefactory.cpp
    rgbdouble.cpp
    settranslation.cpp
    staticasserts.cpp
    version.cpp
    wheelcolorpicker.cpp
)
# Set the headers for our library. This list only contains the headers
# that are _not_ within the source directory.
set(lib_PUBLICHEADERS
    include/abstractdiagram.h
    include/chromahuediagram.h
    include/colordialog.h
    include/colorpatch.h
    include/colorwheel.h
    include/constpropagatinguniquepointer.h
    include/gradientslider.h
    include/importexport.h
    include/lchadouble.h
    include/lchdouble.h
    include/multispinbox.h
    include/multispinboxsectionconfiguration.h
    include/rgbcolorspacefactory.h
    include/settranslation.h
    include/wheelcolorpicker.h
    ${CMAKE_CURRENT_BINARY_DIR}/generated/include/version.h
)
# TODO Also explicitly list all private headers to get more stable CMake
#      and make sure these files pop up in IDEs.





################# Setup translations #################
# qt_create_translation() does two quite different things:
# 1. It updates the .ts files according to the C++ code.
# 2. For each .ts file, it generates a .qm files in the current build directory.
# TODO qt_create_translation() is deprecated in Qt 6. When passing to Qt6,
#      we could also fix the problem with template.ts updates…
qt_create_translation(
    # Output variable where to store the list of generated .qm files:
    qm_file_list
    # C++ code:
    ${library_SRC} ${lib_PUBLICHEADERS} # TODO What about _private_ headers?
    # .ts files:
    "translation/perceptualcolorlib.de.ts"
    "translation/perceptualcolorlib.es.ts"
    "translation/perceptualcolorlib.fr.ts"
    # template.ts is the file that can be used as template to start new (empty)
    # translations. Additionally to the .ts files with actual translations,
    # we also put template.ts here, because otherwise it would not be updated.
    # However, this will also generate a template.qm file, which is useless
    # and waste of space (though the file itself is extremly small).
    # Unfortunately there is no workaround: We have to pass it actually to
    # the qt_add_resource (be means of being part of the qrc_file list)
    # because otherwise it would never get updated. On the other hand,
    # as the file is extremly small, the practical impact is minimal.
    translation/template.ts
)

# Generate a .qrc file (list of resources to include)
set(qrc_file "${CMAKE_CURRENT_BINARY_DIR}/resourcelist.qrc")
file(
    WRITE
    # Filename:
    ${qrc_file}
    # Content to write:
    "<!DOCTYPE RCC>\n"
    "<RCC version=\"1.0\">\n"
    "    <qresource prefix=\"PerceptualColor/i18n\">\n")
foreach(qm_file IN LISTS qm_file_list)
    # The variable qm_file contains an absolute path. It has to be converted
    # to a relative path: relative to the given directory.
    file(
        RELATIVE_PATH
        qm_file                     # output variable
        ${CMAKE_CURRENT_BINARY_DIR} # directory
        ${qm_file}                  # original file path
    )
    # The attributes threshold="100" compress-algo="none" will prevent these
    # files from being compressed. This is because there were discussions
    # that QTranslator does not load compressed resourced, but only
    # uncompressed resources (https://forum.qt.io/post/368666). As the
    # translations are not big anyway, we try to prevent problems and
    # disable the compression.
    file(
        APPEND ${qrc_file}
        "        "
        "<file threshold=\"100\" compress-algo=\"none\">"
        "${qm_file}"
        "</file>\n")
endforeach()
file(
    APPEND
    # File to which append some content:
    ${qrc_file}
    # Content to append:
    "    </qresource>\n"
    "</RCC>\n")

# Add all resources from the .qrt file to our C++ sources
# so they will get compiled in.
qt_add_resources(
    library_SRC # existing source list
    ${qrc_file} # File with list of Qt resource to be added
    # TODO Starting with Qt6, it will no longer be necessary to have a
    # .qrc file, but the list of files can be passed in the very same
    # qt_add_resources() command.
)





################# Build static library #################
# NOTICE Currently, we have no CMake code to build a static library.
# CMake’s BUILD_SHARED_LIBS is not set by default, but we ignore this
# and build nevertheless a shared library.
#
# One day we could provide support for static libraries. It could even
# be packaged. On Linux, it goes to the default library directory, just
# like the shared libraries. And in RPM, it does to “%package devel”.
#
# Than, we would have to respect BUILD_SHARED_LIBS which would change our
# default build type from SHARED to STATIC. Or we could use
# option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
# to get our old default. Anyway, we could use add_library() without
# specifying neither STATIC nor SHARED, and BUILD_SHARED_LIBS will be
# respected.
#
# Alternatively, we could simply add  a new, additional target
# named “perceptualcolorstatic-0”.
#
# There is also a solution that is more comfortable for the library user:
# https://alexreinking.com/blog/building-a-dual-shared-and-static-library-with-cmake.html
# However, this solution is quite complex to implement.
#
# If static libraries would be supported, we would have to make sure that
# build-in resources like the translation files are loaded correctly also
# on static libraries. That is not self-evident! The Qt resource system needs
# some special initialization on static libraries. The unit test
# “testinitialize” should give us the information we need, but this unit test
# obviously has to be compiled against the static library to test
# it actually for the static library. It might be worth, to compile and run
# not only “testinitialize” but all unit tests.





################# Build shared library #################
# For the library name, we follow the KDE policy:
# https://community.kde.org/Policies/New_KDE_Library_API_Policy#Library_Naming
set(LIBRARY_NAME "perceptualcolor-${MAJOR_VERSION}")
add_library(${LIBRARY_NAME} SHARED)
# The headers have to be added here explicitly because otherwise Qt’s MOC
# would not find them because they are not in the same directory as the
# corresponding .cpp files
#
# target_sources should normally always use PRIVATE. Details:
# crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources
target_sources(
    ${LIBRARY_NAME}
    PRIVATE
        ${library_SRC}
        ${lib_PUBLICHEADERS})
# Setting the symbol visibility:
target_compile_definitions(
    ${LIBRARY_NAME}
    PRIVATE PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY)
set_target_properties(
    ${LIBRARY_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET "hidden"
    VISIBILITY_INLINES_HIDDEN TRUE
    VERSION "${FULL_VERSION}"
    SOVERSION "${MAJOR_VERSION}"
    PUBLIC_HEADER "${lib_PUBLICHEADERS}")
target_include_directories(
    ${LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/include/>
        $<INSTALL_INTERFACE:include/${LIBRARY_NAME}/>
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/"
        "${LCMS2_INCLUDE_DIRS}")
# target_link_libraries, despite its name, does not only mean “link”, but
# actually rather “use”. It uses transitive dependencies:
# - If only our source files #include headers of the external library: PRIVATE
# - If only our header files #include headers of the external library: INTERFACE
# - If both (our source files AND our header files) #include headers of external
#   library: PUBLIC
# Than, CMake will make sure that targets that depend on our library will
# get the correct and necessary include directories and linking options
# automatically.
# For details, see https://cmake.org/pipermail/cmake/2016-May/063400.html
target_link_libraries(
    ${LIBRARY_NAME}
    # TODO We could remove the Qt::Concurrent (only) if we were absolutely sure
    # that we will not need it in the future either. Otherwise, it’s better
    # to keep it, for future binary compatibility.
    PUBLIC Qt::Core Qt::Gui Qt::Widgets Qt::Concurrent
    PRIVATE ${LCMS2_LIBRARIES})
install(
    DIRECTORY
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}")
install(
    TARGETS ${LIBRARY_NAME}
    EXPORT "${LIBRARY_NAME}"
    PUBLIC_HEADER DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}")
install(EXPORT ${LIBRARY_NAME}
        FILE ${LIBRARY_NAME}.cmake
        NAMESPACE "PerceptualColor::"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/generated/${LIBRARY_NAME}-config-version.cmake"
  VERSION "${FULL_VERSION}"
  COMPATIBILITY AnyNewerVersion
)
configure_package_config_file(${CMAKE_CURRENT_SOURCE_DIR}/Config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/generated/${LIBRARY_NAME}-config.cmake"
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)
install(FILES
            "${CMAKE_CURRENT_BINARY_DIR}/generated/${LIBRARY_NAME}-config.cmake"
            "${CMAKE_CURRENT_BINARY_DIR}/generated/${LIBRARY_NAME}-config-version.cmake"
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)





################# Build internal shared library #################
# The following library is like the normal library, with the
# following difference: It exports all symbols (also private ones), and it
# does not install. This is used for the unit testing, because exporting all
# symbols is necessary for whitebox testing, which we actually do during
# unit testing.
# (This makes every cpp file to compile twice, which makes the build time
# longer. It might be possible to avoid this by creating an OBJECT collection
# with add_library(object_collection OBJECT 1.cpp 2.cpp …) to avoid double
# compiling, but the resulting library has a bigger file size. Therefore,
# this is not a good option.)
set(INTERNAL_LIBRARY_NAME "perceptualcolorinternal-${MAJOR_VERSION}")
add_library(${INTERNAL_LIBRARY_NAME} SHARED)
# target_sources should normally always use PRIVATE. Details:
# crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources
target_sources(
    ${INTERNAL_LIBRARY_NAME}
    PRIVATE
        ${library_SRC}
        ${lib_PUBLICHEADERS})
target_compile_definitions(
    ${LIBRARY_NAME}
    PRIVATE PERCEPTUALCOLORLIB_BUILD_DYNAMIC_LIBRARY)
# Setting the symbol visibility to “visible”. Following GCC documentation,
# indeed the option for “visible” is called “default” (Sounds strange, but
# it’s true.)
set_target_properties(
    ${INTERNAL_LIBRARY_NAME} PROPERTIES
    CXX_VISIBILITY_PRESET "default"
    VISIBILITY_INLINES_HIDDEN FALSE
    VERSION "${FULL_VERSION}"
    SOVERSION "${MAJOR_VERSION}")
target_include_directories(
    ${INTERNAL_LIBRARY_NAME}
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/include>
        # No $<INSTALL_INTERFACE:xxx> because this target is only meant to
        # be build as part of this project, and not by external projects.
    PRIVATE
        ${LCMS2_INCLUDE_DIRS})
target_link_libraries(
    ${INTERNAL_LIBRARY_NAME}
    # TODO We could remove the Qt::Concurrent (only) if we were absolutely sure
    # that we will not need it in the future either. Otherwise, it’s better
    # to keep it, for future binary compatibility.
    PUBLIC Qt::Core Qt::Gui Qt::Widgets Qt::Concurrent
    PRIVATE ${LCMS2_LIBRARIES})
install(
    DIRECTORY
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}")
install(TARGETS ${INTERNAL_LIBRARY_NAME})
