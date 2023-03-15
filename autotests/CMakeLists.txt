# SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
# SPDX-License-Identifier: BSD-2-Clause OR MIT





################# General setup #################
include(CTest)





################# Test bed #################

if(BUILD_TESTING)

    message(
        STATUS
        "Unit testing is enabled because BUILD_TESTING was set. \n"
        "   CMake’s own “test” target unfortunately runs the unit tests \n"
        "   without building them if the source files have changed. \n"
        "   Therefore, to run unit tests, you should always execute \n"
        "   the target “buildandruntests” instead, which build and updates \n"
        "   the unit tests if necessary.")

    add_custom_target(buildtests)

    add_custom_target(
        buildandruntests
        COMMAND "${CMAKE_CTEST_COMMAND}")
    add_dependencies(
        buildandruntests
        buildtests)

    enable_testing() # Also defines the target “test”

    # Define add_unit_tests(). For adding the unit tests “testfoo.cpp” and
    # “testbar.cpp”, call “add_unit_tests(testfoo testbar)”. If a file
    # “foo.qrc” or “bar.qrc” exists, the corresponding resources are compiled
    # into the respective unit test using the Qt resource system. Every
    # unit test links against the “perceptualcolorinternal” library, which
    # exports all symbols as public symbols, which makes white box tests
    # possible. Furthermore, every unit test links against Qt::Test to
    # provide a test framework, and against LCMS2 to simplify white box tests.
    # For the sake of simplicity, all these libraries are always linked to
    # regardless of the actual needs of the individual unit test.
    function(add_unit_tests)
        foreach(test_name IN LISTS ARGV)
            set(test_target_name "${test_name}")
            add_executable("${test_target_name}")
            set(test_sources "${test_name}.cpp")
            if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/${test_name}.qrc")
                # Add all resources from the .qrt file to our sources
                # so they will get compiled in.
                qt_add_resources(
                    test_sources # existing source list
                    "${test_name}.qrc" # List of Qt resource to be added
                )
            endif()
            # target_sources should normally always use PRIVATE. Details:
            # crascit.com/enhanced-source-file-handling-with-target_sources
            target_sources(
                ${test_target_name}
                PRIVATE ${test_sources})
            target_link_libraries(
                ${test_target_name}
                # Transitive dependencies (PUBLIC, INTERFACE) don’t make sense
                # for executables, since you can’t link against an executable.
                # Therefore using PRIVAT:
                PRIVATE
                    "perceptualcolorinternal-${MAJOR_VERSION}"
                    Qt::Test
                    ${LCMS2_LIBRARIES})

            # The CMakeLists.txt in the main directory might add
            # -Weverything as compile option when the user requests
            # additional warnings. On Clang, this also pulls in
            # -Wreserved-identifier, which in turn prints warnings
            # when using Qt’s QBENCHMARK macro:
            # https://bugreports.qt.io/browse/QTBUG-102431
            # As we need this macro, we have to disable
            # -Wreserved-identifier. Unfortunately the corresponding
            # -Wno-reserved-identifier does not work when using clazy
            # instead of clang. Therefore, we cannot disable it. To
            # work around this problem, we remove -Weverything from
            # the compile options for unit tests:
            get_target_property(
                target_options
                ${test_target_name}
                COMPILE_OPTIONS)
            if(NOT ("${target_options}" STREQUAL "target_options-NOTFOUND"))
                list(REMOVE_ITEM target_options "-Weverything")
                set_property(
                    TARGET ${test_target_name}
                    PROPERTY COMPILE_OPTIONS
                    ${target_options})
            endif()

            add_test(
                NAME "${test_target_name}"
                COMMAND "${test_target_name}")
            add_dependencies(
                buildtests
                "${test_target_name}")
        endforeach()
    endfunction()

    add_unit_tests(
        testabstractdiagram
        testasyncimageprovider
        testasyncimageproviderbase
        testasyncimagerendercallback
        testasyncimagerenderthread
        testchromalightnessdiagram
        testchromalightnessimageparameters
        testchromahuediagram
        testchromahueimageparameters
        testcolordialog
        testcolorpatch
        testcolorwheel
        testcolorwheelimage
        testconstpropagatinguniquepointer
        testconstpropagatingrawpointer
        testextendeddoublevalidator
        testgradientimageparameters
        testgradientslider
        testhelper
        testhelperconstants
        testhelperconversion
        testhelpermath
        testhelperqttypes
        testinitializetranslation
        testinterlacingpass
        testiohandlerfactory
        testlanguagechangeeventfilter
        testlchadouble
        testlchdouble
        testcielchvalues
        testmulticolor
        testmultispinbox
        testmultispinboxsection
        testoklchvalues
        testpalettewidget
        testpolarpointf
        testrefreshiconengine
        testrgbcolorspace
        testrgbcolorspacefactory
        testrgbdouble
        testsettranslation
        testversion
        testwheelcolorpicker)

else()

    message(
        STATUS
        "Unit testing is disabled because BUILD_TESTING wasn’t set.")

endif()
