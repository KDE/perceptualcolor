# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# Version number #################
# Substitutes all occurrences of @MAJOR_VERSION@… and similar declarations
# in the source code file by the actual value in CMake:
configure_file(
    # Input:
    version.in.hpp
    # Output:
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
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
    helperconversion.cpp
    initializetranslation.cpp
    interlacingpass.cpp
    iohandlerfactory.cpp
    languagechangeeventfilter.cpp
    lchadouble.cpp
    lchdouble.cpp
    multicolor.cpp
    multirgb.cpp
    multispinbox.cpp
    multispinboxsection.cpp
    palettewidget.cpp
    polarpointf.cpp
    refreshiconengine.cpp
    rgbcolorspace.cpp
    rgbcolorspacefactory.cpp
    rgbdouble.cpp
    screencolorpicker.cpp
    settranslation.cpp
    staticasserts.cpp
    version.cpp
    wheelcolorpicker.cpp
)
# NOTE Keep the following list synchronized
# between scripts/static-codecheck.sh and src/CMakeLists.txt
set(lib_PUBLICHEADERS
    abstractdiagram.h
    chromahuediagram.h
    colordialog.h
    colorpatch.h
    colorwheel.h
    constpropagatinguniquepointer.h
    gradientslider.h
    importexport.h
    lchadouble.h
    lchdouble.h
    multispinbox.h
    multispinboxsection.h
    rgbcolorspacefactory.h
    settranslation.h
    wheelcolorpicker.h
    ${CMAKE_CURRENT_BINARY_DIR}/generated/version.h
)
# TODO Also explicitly list all private headers to get more stable CMake
#      and make sure these files pop up in IDEs.





################# Translation #################
include(ECMPoQmTools)
# NOTE ECMPoQmTools provides also ecm_install_po_files_as_qm to conveniently
# generate and install qm files as individual files, and ecm_create_qm_loader
# to conveniently load the translations automatically. However, currently
# we compile the translation into the library binary itself. First, this
# avoids that translations can ever be lost. Second, ecm_create_qm_loader
# does not work on static libraries.
get_filename_component(absolute_poqm_directory "../poqm" ABSOLUTE)
file(
    # TODO Is it possible to get rid of GLOB_RECURSE?
    GLOB_RECURSE po_file_list # Name of the output variable.
    LIST_DIRECTORIES FALSE
    # CMake will add logic to the main build system check target to
    # rerun the flagged GLOB commands at build time:
    CONFIGURE_DEPENDS
    # Examples of recursive globing include:
    # /dir/*.py  - match all python files in /dir and subdirectories(!)
    "${absolute_poqm_directory}/*/perceptualcolor-0_qt.po"
)
# Generate a .qrc file (list of .qm files to include) and the targets
# that will produce the individual .qm files.
set(qrc_file "${CMAKE_CURRENT_BINARY_DIR}/resourcelist.qrc")
file(
    WRITE
    # Filename:
    ${qrc_file}
    # Content to write:
    "<!DOCTYPE RCC>\n"
    "<RCC version=\"1.0\">\n"
    "    <qresource prefix=\"PerceptualColor/i18n\">\n")
foreach(po_file_original ${po_file_list})
    get_filename_component(po_dir ${po_file_original} DIRECTORY)
    get_filename_component(lang ${po_dir} NAME)
    ecm_process_po_files_as_qm(
        ${lang}
        ALL # Add this target to the ALL target.
        PO_FILES ${po_file_original})
    # The attributes threshold="100" compress-algo="none" will prevent these
    # files from being compressed. This is because there were discussions
    # that QTranslator does not load compressed resourced, but only
    # uncompressed resources (https://forum.qt.io/post/368666). As the
    # translations are not big anyway, we try to prevent problems and
    # disable the compression.
    file(
        APPEND ${qrc_file}
        "        <file "
        "threshold=\"100\" "
        "compress-algo=\"none\" "
        "alias=\"localization.${lang}.qm\">"
        "${lang}/perceptualcolor-0_qt.qm"
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
    # NOTE Starting with Qt6, it will no longer be necessary to have a
    # .qrc file, but the list of files can be passed in the very same
    # qt_add_resources() command. Problem: The attributes
    # threshold="100" compress-algo="none" (see explication above)
    # cannot be specified than.
)





################# Build library #################
# For the library name, we follow the KDE policy:
# https://community.kde.org/Policies/New_KDE_Library_API_Policy#Library_Naming
set(LIBRARY_NAME "perceptualcolor-${MAJOR_VERSION}")
# Add library. If it will build as STATIC or as SHARED library is not
# hardcoded, so add_library() it use BUILD_SHARED_LIBS to decide.
add_library(${LIBRARY_NAME})
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/>
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
    PUBLIC Qt::Core Qt::Gui Qt::Widgets Qt::DBus Qt::Concurrent
    PRIVATE ${LCMS2_LIBRARIES})
install(
    DIRECTORY
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/${LIBRARY_NAME}")
install(
    # On Linux, both STAIC and SHARED libraries go to the default library
    # directory. And in RPM, SHARED libraries are part of a normal package,
    # while STATIC libraries go to “%package devel”.
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
install(
    FILES
    "${CMAKE_CURRENT_BINARY_DIR}/generated/${LIBRARY_NAME}-config.cmake"
    "${CMAKE_CURRENT_BINARY_DIR}/generated/${LIBRARY_NAME}-config-version.cmake"
    DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/${LIBRARY_NAME}
)





################# Build internal library #################
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
# Add library. If it will build as STATIC or as SHARED library is not
# hardcoded, so add_library() it use BUILD_SHARED_LIBS to decide.
add_library(${INTERNAL_LIBRARY_NAME})
# target_sources should normally always use PRIVATE. Details:
# crascit.com/2016/01/31/enhanced-source-file-handling-with-target_sources
target_sources(
    ${INTERNAL_LIBRARY_NAME}
    PRIVATE
        ${library_SRC}
        ${lib_PUBLICHEADERS})
target_compile_definitions(
    ${INTERNAL_LIBRARY_NAME}
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
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated/>
        # No $<INSTALL_INTERFACE:xxx> because this target is only meant to
        # be build as part of this project, and not by external projects.
    PRIVATE
        ${LCMS2_INCLUDE_DIRS})
target_link_libraries(
    ${INTERNAL_LIBRARY_NAME}
    # TODO We could remove the Qt::Concurrent (only) if we were absolutely sure
    # that we will not need it in the future either. Otherwise, it’s better
    # to keep it, for future binary compatibility.
    PUBLIC Qt::Core Qt::Gui Qt::Widgets Qt::DBus Qt::Concurrent
    PRIVATE ${LCMS2_LIBRARIES})
