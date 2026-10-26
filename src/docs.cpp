// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// NOTE Keep the following page in sync between src/docs.cpp and README.md
/*!
 * \page index.html
 *
 * \title PerceptualColor
 *
 * \brief This library provides Qt GUI components for choosing sRGB colors,
 * with a focus on an intuitive and perceptually uniform presentation.
 *
 * \l {ModulePublicApi}{Public API}
 *
 * \section1 Introduction
 *
 * The GUI widgets in this library are based on the LCH color model,
 * which is known to reflect human perception better than RGB or its variants
 * like HSV.  This leads to a highly intuitive user interface that is more
 * user-friendly and easier to navigate. Moreover, the widgets can be used
 * effectively without any prerequisite knowledge of the LCH color model
 * by the users.
 *
 * \image ColorDialog.png \l ColorDialog
 *
 * \image ColorDialogTab1.png \l ColorDialog
 *
 * \image  ColorDialogTab2.png \l ColorDialog
 *
 * \section1 Features
 *
 * This library:
 *
 * \list
 * \li Provides complete documentation for the public API (as well as for the
 *     internal API and source code itself).
 * \li Is cross-platform. It does not use platform-specific code nor rely on a
 *     specific byte order. CI-tested on Linux, Windows, FreeBSD.
 * \li Provides internationalization and localization, including right-to-left
 *     layouts.
 * \li Provides high-DPI support.
 * \li Uses QStyle wherever possible.
 * \li Provides support for Qt style sheets where it makes sense.
 * \li Has support for both pointer devices (mouse, touchpad, etc.) *and*
 *     keyboard navigation in the widgets.
 * \li Has a permissive license and follows
 *     the \l {https://reuse.software/} {“Reuse” specification}.
 * \li Can be built as either a static or a shared/dynamic library.
 * \li Is developed with unit tests.
 * \li Links only to Qt, so it is a Tier 1 library.
 * \li Uses only the public API of Qt, and no private
 *     features of Qt.
 * \endlist
 *
 * \section1 Links
 *
 * \list
 * \l { https://invent.kde.org/libraries/perceptualcolor }
 * { GIT Repository on invent.kde.org }
 * \endlist
 */

/*!
 * \module ModulePublicApi
 *
 * \title Public API
 *
 * \brief Public API for the \l {index.html}{PerceptualColor library}.
 *
 * \section1 Other public symbols
 *
 * \generatelist GroupPublicSymbols
 *
 * \section1 General public documentation
 *
 * \generatelist GroupPublicPages
 */

/*!
 * \module ModulePrivateApi
 *
 * \title Private API
 *
 * \brief Private API.
 *
 * See the \l [CPP] {PerceptualColor} {PerceptualColor namespace} for a
 * complete documentation index (contains public and private documentation).
 *
 * \noautolist
 */

/*!
 * \page build.html
 *
 * \ingroup GroupPublicPages
 *
 * \title Build instructions and requirements
 *
 * Build-time dependencies:
 *
 * \list
 *     \li Qt 6 (minimum version: 6.9).
 *         \omit
 *             NOTE  Keep in sync
 *             - This documentation of build requirements
 *             - .gitlab.ci.yml: codecheck_quality/codecheck_iwyu
 *               image‘s Qt version
 *             - CMakeLists.txt: find_package(Qt6
 *             - CMakeLists.txt: QT_DISABLE_DEPRECATED_UP_TO
 *             - CMakeLists.txt: QT_WARN_DEPRECATED_UP_TO
 *             - CMakeLists.txt: QT_ENABLE_STRICT_MODE_UP_TO
 *             - autotests/CMakeLists.txt: remove_definitions(
 *         \endomit
 *         Components: Core, Gui, Widgets, DBus, Test, Svg.
 *     \li CMake
 *     \li ECM (Extra CMake Modules from KDE)
 *     \li C++20
 *     \li Both, the input character set and the execution character set, have
 *         to be UTF8. (See \l {Compiler character sets} for more details.)
 *         \omit
 *             Qt 6 requires minimum C++17, as
 *             https://doc-snapshots.qt.io/qt6-dev/cmake-get-started.html
 *             explains. However, as https://doc.qt.io/qt-6/cpp20-overview.html
 *             explains, some additinal features get available if compiled
 *             with C++20.
 *
 *             KDE Frameworks
 *             Policy https://community.kde.org/Frameworks/Policies says:
 *                 “KDE Frameworks can use features of the previous to latest
 *                  C++ standard, at the time of writing (December 2025) that
 *                  means C++20.”
 *
 *             In the CMakeLists.txt file, we set -std=c++20 and we set
 *             also -Wpedantic and -pedantic-errors to enforce it.
 *         \endomit
 * \endlist
 *
 * Additional mandatory run-time dependencies:
 *
 * \list
 * \li QSvgIconEnginePlugin. Available plugins are loaded
 *   automatically by Qt. Therefore, just make sure that this plugin is
 *   present. On Linux, it seems possible to enforce this by linking
 *   dynamically to the plugin itself, if you want to. This forces Linux
 *   package managers to produce packages of your application that depend
 *   not only on Qt base, but also on the SVG plugin. A typical file name of
 *   the plugin is \c {plugins/iconengines/libqsvgicon.so}.
 * \endlist
 *
 * Please make sure that you comply with the licences of used libraries.
 *
 * To prepare the build, run cmake. We provide plenty of CMake options
 * that control the build type (shared/dynamic vs. static), IPO/LPO and
 * much more. The options are self-documenting. When you run CMake,
 * the options are listed with their values and a description of them.
 *
 * Then, to build and install the library:
 * \code
 * make && sudo make install
 * \endcode
 *
 * To do unit testing:
 * \code
 * make build_test test
 * \endcode
 */

/*!
 * \page circularwidgets.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Circular widgets
 *
 * This library provides several circular widgets, such as
 * \l PerceptualColor::WheelColorPicker, \l PerceptualColor::ColorWheel,
 * and \l PerceptualColor::ChromaHueDiagram.
 *
 * These widgets are designed with a circular look-and-feel. Although the
 * layout system treats them as rectangular (like any other QWidget), user
 * interaction is truly circular. From the user’s perspective, they behave as
 * circular widgets in both painting and mouse interaction. For example, a
 * mouse click outside the circle but inside the surrounding rectangle is
 * ignored by the widget; in particular, it does not even grant focus. If the
 * current widget style allows dragging the window by left-click-and-hold in
 * empty space, this action remains possible in the outer rectangular area.
 *
 * This widget \e {always} accepts focus when the mouse clicks inside
 * the circle, regardless of the \c {QWidget::focusPolicy} setting:
 *
 * \list
 * \li If \c {QWidget::focusPolicy} is set to disallow mouse focus, clicks
 *   inside the circle will still grant focus (default behavior).
 * \li If \c {QWidget::focusPolicy} allows mouse focus, then clicks inside
 *   the circle as well as anywhere within the rectangular widget will grant
 *   focus.
 * \endlist
 *
 * See also
 * \l {https://forum.qt.io/topic/118547/accept-reject-focus-coming-by-mouse-click-based-on-coordinates}
 * {this Qt Forum thread}.
 *
 * An alternative approach could be using \c {QWidget::setMask()} to
 * restrict mouse events and painting to a circular mask. However, we avoid
 * this method due to several drawbacks:
 *
 * \list
 * \li It is unclear whether focus management is correctly handled with masks.
 * \li Documentation warns of performance penalties for complex shapes. Is a
 *   circle considered complex in this context?
 * \li Most importantly, masking may interfere with our anti-aliased circle
 *   rendering.
 * \endlist
 */

/*!
 * \page codingstyle.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Coding style
 *
 * Always document your code.
 *
 * \section1 C++
 *
 * \list
 * \li Provide unit tests for your code.
 * \li If working with children within Qt’s object hierarchy, allocate on the
 *   heap and use raw pointers or guarded pointers (`QPointer`). If not,
 *   allocate on the stack or use smart pointers. Prefer Qt’s smart pointers
 *   over the \c {std} smart pointers of C++.
 * \li Specifier order: static, extern,
 *   inline, \omit =; prevent static codecheck complain about inline \endomit
 *   const, constexpr, volatile.
 * \li Use KDE’s
 *   \l {https://community.kde.org/Policies/Frameworks_Coding_Style}
 *   {Frameworks Coding Style}. This can be done automatically with
 *   clang-format.
 * \li Comments within the code should have the following form which
 *   allows to comment out quickly large parts of the code for
 *   testing purposes: \c {// comment}
 * \endlist
 *
 * \section1 CMake
 *
 * \section2 Useful documentation
 *
 * \list
 *     \li \l {https://preshing.com/20170522/learn-cmakes-scripting-language-in-15-minutes/}
 *         {Introduction to the CMake script language}
 *     \li \l {https://github.com/onqtam/awesome-cmake} {Curated list of
 *         awesome CMake scripts, modules, examples and others}
 *     \li \l {https://llvm.org/docs/CMakePrimer.html} {General introduction
 *         into CMake (both, the script language and the commands)}
 * \endlist
 *
 * \section2 Use “Modern CMake”
 *
 * Use \e {Modern CMake}: Avoid global settings like
 * \c {include_directories()}. Use target-based commands like
 * \c {target_include_directories()} instead. Use
 * \c {target_link_libraries()} to pull in dependencies, which gives you
 * automatically the correct include directories, compile options etc.
 * When using \c {target_link_libraries()}, always specify
 * \c {PUBLIC} or \c {PRIVATE} or \c {INTERFACE} explicitly.
 *
 * \section2 Indent
 *
 * Use 4 spaces for indenting. Do not use tabs.
 *
 * \section2 Quotation marks
 *
 * Use quotation marks when you want to represent a string.
 * Do not use quotation marks when you mean a keyword. Example:
 *
 * \code
 * set(myvar "foo")
 * \endcode
 *
 * Particularly, always quote strings within control structures:
 *
 * \code
 * if("${myvar}" STREQUAL "bar")
 * \endcode
 *
 * \section2 Boolean
 *
 * CMake allows various representations of boolean. Use only the forms
 * \c {TRUE} and \c {FALSE}. Do not quote them.
 *
 * \section2 Check if a variable exists
 *
 * Check if a variable exists like this:
 *
 * \code
 * if(DEFINED varname)
 * \endcode
 *
 * \section2 Naming
 *
 * Functions and macros:
 *
 * \code
 * lower_case()
 * \endcode
 *
 * Control structures: \c {lower_case()}, with empty \c {else()},
 * \c {endif()}, \c {endfunction()}…
 *
 * Operators/keywords/directives/extra options: \c {UPPER_CASE} without
 * quotes. Examples:
 *
 * \code
 * if(condition STREQUAL "")
 * \endcode
 *
 * \code
 * do_something(... USE_THIS)
 * \endcode
 *
 * \code
 * file(COPY ...)
 * \endcode
 *
 * \section2 Parenthesis
 *
 * Multi-line calls have trailing parenthesis on the same line as
 * last parameter, not on separate line:
 *
 * \code
 * set(my_variable
 *     "value1"
 *     "value2"
 *     "value3)
 * \endcode
 *
 * \section2 Order
 *
 * Source and header lists should be ordered alphabetically,
 * subdirectories last, preferably separated by a single empty line.
 *
 * \section2 Line length
 *
 * Keep the length of the line below 80 characters when possible.
 * No trailing whitespace.
 *
 * \section2 Directories
 *
 * A directory path may not have a trailing \c {/}. This is to avoid
 * duplicates like \c {//} when composing paths:
 *
 * \code
 * set(my_path "/usr/bin/") # BAD
 * set(my_path "/usr/bin") # GOOD
 * set(my_file "${my_path}/my_file")
 * \endcode
 *
 * \section2 Lists
 *
 * Declaring an empty list:
 *
 * \code
 * set(mylist)
 * \endcode
 *
 * Declaring and initializing a list at the same time:
 *
 * \code
 * set(mylist
 *     "first item"
 *     "second item"
 *     "third item")
 * \endcode
 *
 * Append to a list:
 *
 * \code
 * list(APPEND mylist "last item")
 * \endcode
 *
 * \section2 ${PROJECT_NAME}
 *
 * Use \c {${PROJECT_NAME}} for global variables, targets and labels
 * instead of repeating the project name manually or using fixed names:
 *
 * \code
 * add_executable(${PROJECT_NAME} …)
 * \endcode
 */

/*!
 * \page colorspaces.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Color spaces
 *
 * Major operating systems assume sRGB (with the standard piecewise
 * gamma function) as the default color space for application‑drawn content.
 * This is either because the OS performs active color management and expects
 * sRGB input, or because no color management is applied and most monitors
 * interpret incoming data as sRGB. Consequently, when drawing in QWidget,
 * colors should be specified in sRGB. This library therefore supports only
 * the sRGB gamut to ensure correct rendering.
 *
 * The RGB color space itself is not perceptually uniform. A perceptually
 * uniform color space is one in which equal numerical changes correspond
 * to equal visual differences as perceived by humans. Human color
 * perception is three‑dimensional, but it is not a Euclidean space and
 * \l {https://www.pnas.org/doi/10.1073/pnas.2119753119} {may not even
 * be Riemannian}.  Therefore, every Euclidean “perceptually uniform”
 * color space is only an approximation. Defining such spaces is always
 * a trade‑off between closely matching human perception and providing
 * efficient conversions. This library supports projections into the
 * \l {https://en.wikipedia.org/wiki/CIELAB_color_space} {CIELab} and
 * \l {https://bottosson.github.io/posts/oklab/} {Oklab} color spaces, both
 * widely used today. Less common perceptually uniform spaces such as CAM16,
 * \l {https://de.wikipedia.org/wiki/DIN99-Farbraum} {DIN99}, or Google’s
 * \l {https://github.com/material-foundation/material-color-utilities}
 * {HCT}, used \l {https://m3.material.io/blog/science-of-color-design}
 * {in Material Design}, are not supported. Instead
 * of Cartesian coordinates (Lab: Lightness, a, b), we prefer a
 * \l {https://en.wikipedia.org/wiki/Cylindrical_coordinate_system}
 * {cylindrical representation} (LCh: Lightness, Chroma radius, Hue angle),
 * which corresponds more naturally to human perception.
 */

/*!
 * \page compilercharacterset.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Compiler character sets
 *
 * \section1 Compiler character sets
 *
 * Compilers have three different character sets:
 *
 * \list
 * \li Input character set (the character set of the source code)
 * \li Narrow execution character set
 *   (for \c {char} and for string literals without prefix)
 * \li Wide execution character set
 *   (for \c {wchar_t} and for string literals with \c {L} prefix)
 * \endlist
 *
 * \section2 Input character set
 *
 * This source code of this library is encoded in UTF8. Therefore, your
 * compiler must treat is also as UTF-8.
 *
 * Why are we using UTF-8 instead of ASCII?
 *
 * \list
 * \li UTF-8 is more complete than ASCII. ASCII does not even provide basic
 *   typographic symbols like en-dash, em-dash or non-breaking space
 *   characters or quotes.
 * \li Unicode exists since 1991, UTF-8 since 1993. It’s time to get rid of
 *   the insufficient ASCII character. It’s time to use Unicode.
 * \li We use non-ASCII characters for (typographically
 *   correct) code documentation and partially also for
 *   source code comments. It would be quite annoying to use HTML
 *   entities for each non-ASCII character in the code documentation;
 *   and it would be pointless to do it for source code
 *   comments.
 * \li \c {i18n()} and \c {ki18n()} and \c {tr()} require both,
 *   the source file and \c {char*} to be encoded in UTF-8; no other
 *   encodings are supported. (Only ASCII would be UTF-8 compatible,
 *   but in practice this encoding is not supported, but only 8859-Latin
 *   encodings, which allow code points higher than 127, which risks to
 *   introduce incompatibilities. Therefore, this would not be a good
 *   option.)
 * \li The C++ identifiers of library symbols are however (currently)
 *   ASCII-only.
 * \endlist
 *
 * So we use a \c {static_assert} statement to control this.
 *
 * \section2 Narrow execution character set
 *
 * Why are we using UTF-8 as narrow execution character set?
 *
 * \list
 *     \li \c {i18n()} and \c {ki18n()} and \c {tr()} require both,
 *         the source file and \c {char*} to be encoded in UTF-8; no other
 *         encodings are supported.
 *     \li Implicit conversion from \c {char*} to \c {QString} assumes
 *         that \c {char*} is UTF-8 encoded. Thus we disable this implicit
 *         conversion in \c {CMakeLists.txt}, it’s wise to stay compatible.
 * \endlist
 *
 * Therefore, a static assert controls that really UTF-8 is used
 * as narrow execution character set.
 *
 * \section2 Wide execution character set
 *
 * We do not use actively the wide execution character set.
 * Therefore, currently, no static assert forces a specific
 * wide execution character set.
 */

/*!
 * \page generallist.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title General to-do list with ideas or issues
 *
 * SHOULDHAVE In the CI (as defined by our current configuration), we
 * get muss less warnings than when we run locally with
 * \c {-DADDITIONAL_WARNINGS=ON -DCMAKE_INTERPROCEDURAL_OPTIMIZATION=FALSE
 * -DCMAKE_CXX_COMPILER=clazy}.
 *
 * SHOWSTOPPER Define the precision of this library. We allow
 * changing the number of decimals in the \l PerceptualColor::ColorDialog
 * though there is only a private API, not a public one.
 * Document how exact the results of \l PerceptualColor::ColorDialog are.
 * If only 8 bit per channel, move to QRgb. Otherwise, either stay with
 * QColor, but deliver exact results. Or even move to
 * \l PerceptualColor::GenericColor.
 * The latter would allow to provide exact values also in color spaces
 * different from RGB, such as Oklch, Cielch etc. If we do that, probably
 * \l PerceptualColor::ColorDialogPrivate::decimals should become part
 * of the public API and \l PerceptualColor::ColorDialogPrivate::okdecimals
 * shall become a function that returns
 * \l PerceptualColor::ColorDialogPrivate::decimals + 2.
 * Also, \l PerceptualColor::SwatchBook is difficult to get right. Its
 * base color grid is based on Oklch, its and costum colors are
 * based on whatever the used has used originally to define the color,
 * which could be Oklch, Cielch, HSL or whatever. Maybe we have to switch
 * its grid data to \l PerceptualColor::GenericColor to provide full
 * precision and avoid rounding errors, which can be big on some extreme
 * positions of the Oklab color space?
 *
 * SHOULDHAVE Support CSS colors using \l PerceptualColor::CssColor
 * by allowing copy (right-click on the \l PerceptualColor::ColorPatch,
 * however this will not work for keyboard-only usage because the context menu
 * key (Qt::Key_Menu and maybe Qt::Key_MenuKB and Qt::Key_MenuPB
 * but rather not Qt::Key_TopMenu)
 * cannot be used because this widget cannot get focus, or should that
 * change?) and paste (maybe even whereever in the
 * \l PerceptualColor::ColorDialog, even within
 * \l PerceptualColor::MultiSpinBox. And also incoming drag-and-drop
 * (outgoing drag-and-drop instead should use the standard clipboard format).
 * And maybe accept something that can be parsed also when Crtl-V or dragged
 * to a multispinbox if it can be interpreted as such a value.
 *
 * NICETOHAVE Remove remaining usage of \c {ifndef MSVC_DLL}
 * which is used in unit tests to exclude some tests that would otherwise
 * fail to link on MSVC, probably because of missing symbol exports.
 * Probably \l PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT can help to export
 * the symbols explicitly for the unit tests, making most
 * \c {ifndef MSVC_DLL} obsolete.
 *
 * SHOWSTOPPER The sRGB gamut in the Oklab space has
 * an irregular shape in the chroma-lightness diagram around 264.1°, see also
 * https://github.com/color-js/color.js/issues/81 for details: There is a
 * "cut" in the gamut body. Similar, sRGB in CieLch has at yellow hues
 * on the chroma-lightness diagram some positions, where reducing chroma
 * results in out-of-gamut colors. What we need: Appropriate user control code
 * (mouse and keyboard, but also the move-into-gamut functions used for
 * manually entered values) that deals correctly with all these pecularities.
 * And from noahdvs in Matrix: “I noticed an issue with the arrow
 * key navigation of the lightness/saturation picker in the "Hue-based" tab.
 * If you press Up or Down, the point is able to climb up or down until it
 * reaches the maximum/minimum lightness (good). If you press Right, the point
 * gets stuck when it reaches an edge rather than being able to continue to the
 * right until maximum saturation is reached (bad).” Using
 * \l PerceptualColor::ChromaLightnessDiagramPrivate::nearestInGamutLchByAdjustingChromaLightness
 * might help here, though its precision depends on the rendering size of the
 * image.
 *
 * NICETOHAVE The re-rendering is a bit slow (not reactive enough)
 * when changing the window size of \l PerceptualColor::ColorDialog
 * for the \l PerceptualColor::GradientSlider with the lightness at
 * the left. And during fast window size changes, it takes a moment to
 * adapt the size the on the top of the lightness slider the position chnages.
 * That looks ugly. It would be better if the slider reacts slowly on the
 * bottom (visually less intrusive). Probably to get this, it would be
 * necessary to make “top” the “first” color, and “bottom” the “second”,
 * because currently it is the other way around, which causes this visual
 * glitch?
 * Would it make sense to use simply image scaling with
 * Qt::SmoothTransformation while waiting for the correctly-sized image?
 *
 * NICETOHAVE When the CI tests for warnings, it should not only build
 * and Clang/Clazy, but additionally also on GCC which produces some warnings
 * that Clang/Clazy does not have.
 *
 * NICETOHAVE Provide more \c {tooltip()} help for widgets.
 * For \l PerceptualColor::WheelColorPicker
 * and \l PerceptualColor::ChromaLightnessDiagram, this
 * help text could describe the accepted keyboard controls
 * and mouse movements?
 * These tooltips could be integrated
 * as default value in the class itself. For the other widgets, a
 * help text could be defined within \l PerceptualColor::ColorDialog,
 * if appropriate.
 * There, we could also add a tooltip about the
 * whitepoint (D65) of sRGB in the sRGB group box. But maybe this is overkill
 * and we should instead do the opposide and remove the whitepoint information
 * from the Oklch tooltip.
 * And/or: A help button in the QDialogButtonBox that shows a
 * QMessageBox that explains "hidden"/non-obvious features like
 * mouse/key events and shortcuts.
 *
 * SHOULDHAVE Circular diagrams should be right-aligned on RTL layouts.
 * Or, like QDial, centered both horizontially and vertically? Does that help
 * with our use case?
 *
 * NICETOHAVE Use words as hints for color ranges? Muted/dull colors have
 * a low chroma value. The dark ones (getrübte/gebrochene Farben) are created by
 * adding black (and possibly a bit of white) and include both, warm tones (the
 * browns) and cool tones (the olives). The light ones are called Pastel
 * colors and are created by adding white (and possibly a bit of
 * black) and also include both warm tones (like baby pink) and cool
 * tones (like baby blue). Warm colors are located at a color angle of
 * about 45°, cool colors at about 225°. Could we mark this in the diagrams?
 * Could we provide an option to enable labels for the
 * \l {https://www.handprint.com/HP/WCL/color12.html} {warm-cool}?
 * Cold and warm could be marked by a text outside the color wheel at the
 * given position. The other ones seem to be more complicated: These specific
 * color terms do not have a translation in all languages. (For “muted colors”,
 * there is no good German translation, and for “getrübte Farben”, there is
 * no good English translation.
 * Another idea might be to mark the wavelength of the corresponding spectral
 * color outside around the color wheel, respectively the term
 * “Line of purples” for the non-spectral colors.
 *
 * NICETOHAVE
 * https://invent.kde.org/plasma/kdeplasma-addons/-/merge_requests/249
 * allows the user to drag and drop an image file. The image’s average color
 * is calculated and set as current color of Plasma’s color picker widget.
 * Furthermore, it seems that Plasma’s color picker widget also accepts
 * color codes for drag-and-drop. (Which ones? Maybe the #128945 style?)
 * Would this make sense also for our library?
 *
 * SHOULDHAVE STYLING Consider deeper integration with QStyle. KStyle
 * (\l {https://invent.kde.org/frameworks/frameworkintegration/-/tree/master/src/kstyle?ref_type=heads}
 * {code} / \l {https://api.kde.org/kstyle.html} {documentation}) is a
 * QCommonStyle‑derived class that enables QString‑based queries for custom
 * style hints, control elements, and sub‑elements. The Tier‑1 framework
 * KWidgetsAddons provides the namespace
 * \l {https://api.kde.org/kstyleextensions.html} {KStyleExtensions},
 * which allows custom widgets to perform these queries independently of the
 * active style, avoiding the need to hard‑code custom enum values for
 * QStyle::ControlElement (and similar enums) in each style. KStyleExtensions
 * works with all styles inheriting from KStyle and, in theory, could also
 * function with non‑KStyle styles that implement the required interface (see
 * \l {https://codebrowser.dev/kde/kwidgetsaddons/src/kstyleextensions.cpp.html#14}
 * {source}), though this is unlikely. It reports whether a given query is
 * supported by the underlying style. Breeze, KDE’s default style, inherits
 * either from KStyle or directly from QCommonStyle depending on build
 * configuration; when using KStyle it leverages this mechanism to provide
 * custom elements such as \c {CE_CapacityBar} (see
 * \l {https://invent.kde.org/plasma/breeze/-/blob/bdeab2b04c82a1e3b0e14976c67db1d70042d842/kstyle/breezestyle.cpp#L271}
 * {example 1},
 * \l {https://invent.kde.org/plasma/breeze/-/blob/bdeab2b04c82a1e3b0e14976c67db1d70042d842/kstyle/breezestyle.cpp#L1417}
 * {example 2},
 * \l {https://invent.kde.org/plasma/breeze/-/blob/master/kstyle/breezestyle.h?ref_type=heads#L528}
 * {header}), which is used in KWidgetsAddons’
 * \l {https://api.kde.org/kcapacitybar.html} {KCapacityBar widget}.
 * This approach could similarly enable style‑dependent rendering functions for
 * \l PerceptualColor widgets, such as \c {renderColorWheel}.
 *
 * NICETOHAVE
 * In https://phabricator.kde.org/T12359 is recommended to provide
 * RESET statements for all properties for better compatibility with QML.
 * As we provide widgets, this should not be too important. Are there also
 * good arguments for widgets to provide RESET?
 *
 * SHOULDHAVE STYLING Switch
 * \l PerceptualColor::AbstractDiagram::handleOutlineThickness and
 * handleRadius() and spaceForFocusIndicator() to use PM_DefaultFrameWidth.
 * (PM_DefaultFrameWidth seems to be used
 * yet in \l PerceptualColor::ColorPatch.)
 *
 * SHOULDHAVE STYLING
 * It might be interesting to use \c {QStyle::PM_FocusFrameHMargin}
 * \e {(Horizontal margin that the focus frame will outset the widget
 * by.)} Or: \c {QStyle::PM_FocusFrameVMargin}. Using this for the
 * distance between the focus indicator and the actual content of the widget
 * maybe give a more \c {QStyle} compliant look. But: If using this,
 * ensurePolished() must be called before!
 *
 * SHOULDHAVE \l {https://bugs.kde.org/show_bug.cgi?id=517274}
 * {Spectacle has a magnifier glass for cutting screenshots.} It would be
 * cool to have this also for eyedropper; it could be even better if the
 * magnifier would be round instead of rectangular. But eyedropper is not
 * implemented within this library, but as a KDE portal providing this
 * functionality. New features would have to be implemented there.
 * Also interesting: Screen picker with magnifier glass in two steps
 * similar to https://colorsnapper.com which has a normal-sized magnifying
 * glass, and also a giant magnifying glass occupying almost the hole screen?
 * How does its UI work? Is there the normal magnifying glass, then a first
 * click, then the giant magnifying glass? Advantage: Users are always aware
 * of the function. Disadvantage: It always requires two clicks, what might
 * be cumbersome. Or do you get the giant magnifying glass only on demand,
 * for example by pushing the Ctrl key? Or should we do it more simply
 * \l {https://web.archive.org/web/20250626170102/https://www.firefox.com/en-US/features/eyedropper/}
 * {like in Firefox} (Application Menu → More tools → Eyedropper)?
 * Also Spectacle (the new version in Plasma 6.4)
 * \l {https://kde.org/announcements/plasma/6/6.4.0/#screenshots--screen-recording}
 * {has a magnifying glass with haircross}, but is this only on X11 or also
 * on Wayland? And how does it work, because Wayland is very strict about
 * security?
 *
 * SHOULDHAVE ci-doc.sh and local-doc.sh contain a grep command that
 * filters warnings about "Cannot locate index file for dependency" and
 * "Failed to find … indexTitle". It would be better to resolve the issues
 * instead of filtering the warnings.
 *
 * SHOULDHAVE
 * Avoid default arguments like \c {void test(int i = 0)} in
 * public headers, as changes require re-compilation of the client application
 * to take effect, which might lead to a miss-match of behaviour between
 * application and library, if  compile-time and run-time version of the
 * library are not the same.
 *
 * SHOULDHAVE Property bindings: Can a Q_PROPERTY declaration be changed
 * afterwards without breaking binary compatibility? If not, we have to
 * decide now, before the first release, on property bindings:
 * Provide property bindings as described in
 * https://www.qt.io/blog/property-bindings-in-qt-6 or not? It is worth
 * when we do not support QML? What are the pitfalls? Imagine a property
 * that holds a percent value from 0 to 100; the setter enforces this
 * range; the binding bypasses the setter and allows every value? And:
 * How can callbacks know about when a setter was called in C++? See
 * also: https://doc.qt.io/qt-5/qtqml-cppintegration-exposecppattributes.html
 * and https://doc.qt.io/qt-5/qtqml-tutorials-extending-qml-example.html and
 * http://blog.aeguana.com/2015/12/12/writing-a-gui-using-qml-for-a-c-project/
 * for interaction between QML and C++. Pitfalls: Example of color() property
 * stored internally at m_color: Much implementation code of the class will
 * access directly m_color instead of color(), so when using bindings,
 * this code is broken?
 *
 * NICETOHAVE Provide QML support so that for
 * https://doc.qt.io/qt-5/qml-qtquick-dialogs-colordialog.html (or its
 * Qt6 counterpart) we provide a source compatible alternative, like for
 * QColorWidget? Split the library in three parts (Common, Widgets, QML)?
 * Support \l {https://mauikit.org/} {MauiKit}?
 * Apparently QWidget cannot be used from QML. (Though there is
 * https://www.kdab.com/declarative-widgets/ – how does that work?)
 *
 * NICETOHAVE Qt Creator and
 * Qt Designer support for the widgets. Quote from a blog from Viking
 * about Qt Designer plugins:
 * The problem is that you have to build it with exactly the same compiler
 * tool chain as designer was built with, and you have to do it in release
 * mode. Unless your Qt is built in debug, then your plugin needs to be
 * built in debug mode as well. So you can’t just always use the same
 * compiler as you build the application with, if you use the system Qt or
 * a downloaded Qt version.
 *
 * NICETOHAVE accessibility support
 *
 * NICETOHAVE Provide
 * \c {ColorDialog::setResizable(bool resizable)}. Do not
 * provide a property, because in the background this is setting \e {two}
 * different values, which might be conflicting in the moment when read
 * access is done. Code within \l PerceptualColor::ColorDialog to enable:
 * \c {ColorDialog::layout()->setSizeConstraint(QLayout::SetFixedSize);
 * setSizeGripEnabled(false);}
 * Document the performance impact: When resizing is allowed (which is the
 * default), the user can make the window very big, which might increase
 * the rendering time considerable.
 * Set to no-resizable by default to avoid performance problems.
 *
 * NICETOHAVE Provide (on demand) two patches,
 * within the \l PerceptualColor::ColorDialog
 * like Scribus also does: One for the
 * old color (cannot be modified by the user) and another one for the new
 * color (same behavior as the yet existing color patch). One could be
 * named “before” and the other “after”. Or maybe make this configurable?
 * And put an arrow between the patches, from “before” to “after”. (Be aware:
 * RTL support necessary!)
 *
 * NICETOHAVE Support for other models like
 * Munsell? With an option to enable or disable them? (NCS not, because
 * it is not free…)
 * Or, additional to the polar coordinates Oklch/Cielch support also:
 * Oklab (feature parity with CSS Color 5),
 * Cielab (feature parity with CSS Color 5), Okhsl?
 *
 * SHOULDHAVE Unit test if (and how) QColorDialog normalized its
 * \c currentColor to RGB-QColor if the API user had set a HSL-QColor etc, and
 * compare this with \l PerceptualColor::ColorDialog behavior.
 *
 * SHOULDHAVE Remove \l PerceptualColor::ColorDialog::layoutDimensions
 * property? Instead, implement a truly convertible
 * layout: Show/hide the widget for numbers depending on dialog size,
 * and maybe even re-arrange even more of the widgets with varying
 * sizes.
 *
 * NICETOHAVE \l PerceptualColor::WheelColorPicker:
 * Is the (double) focus indicator actually good design? Are there better
 * solutions?
 *
 * NICETOHAVE Decide wether it is worth to
 * keep \l PerceptualColor::SwatchBookPrivate::m_autoSyncSelectionMark and
 * make it available in the public API or if it is better to abolish it,
 * supposing always \c {false}.
 *
 * SHOULDHAVE \l PerceptualColor::SwatchBook:
 * Drag-and-drop  support: Provide outgoing drag events for all
 * non-empty color patches. Provide incoming drop events on all patches (empty
 * and non-empty) when not in read-only mode. And provide the possibility to
 * re-arrange the colors by drag-and-drop, either within a special editing mode
 * or during normal operation (or would this incompatible or confusing
 * together with our current selection fuctionality?)
 *
 * SHOULDHAVE \l PerceptualColor::SwatchBook:
 * Test how the context menu key works in this class (and maybe also test it
 * in other widgets). Does it trigger a mouse event or a key event?
 *
 * NICETOHAVE \l PerceptualColor::RgbColor:
 * Implement our own conversions from RGB to HSL etc. to
 * get higher precsion than we currently get by relying on QColor.
 *
 * SHOULDHAVE \l PerceptualColor::toQColor:
 * There is no standard mime type for color. But we should
 * support more than the current formats. Especially text(), html() and
 * text/css should be read and interpreted as CSS Color.
 *
 * SHOULDHAVE \l PerceptualColor::toQColor:
 * Provide support not only for drag-and-drop, but also for
 * the clipboard, which uses the same mime-encoded data types. For example
 * support for Ctrl+V to paste from the clipboard.
 *
 * SHOULDHAVE \l PerceptualColor::transparencyBackground:
 * The function \l PerceptualColor::transparencyBackground should
 * have color management support! Currently, we use the
 * same value for red, green and blue, this might \e {not} be perfectly
 * neutral gray depending on the color profile of the monitor…
 * Calculate the two color values in Oklab instead of RGB
 * because \l PerceptualColor::AbstractDiagram::neutralGray() also does.
 *
 * NICETOHAVE STYLING \l PerceptualColor::GradientSlider:
 * A better handle for the slider. Currently, the handle is just a
 * line. At a handle position at the very beginning or end of the slider,
 * furthermore only half of the line thickness is visible. It might be better
 * to have arrows outside the slider to mark the position. (On the other
 * hand, this would be different to the slider handles of the color
 * wheels…)
 *
 * SHOULDHAVE The classes \l PerceptualColor::GradientSlider
 * and \l PerceptualColor::ColorWheel could be subclasses of QAbstractSlider.
 * This might integrate better with the user’s Qt code. On the other hand,
 * this would mean a lot of work in this library to implement the
 * complete interface of QAbstractSlider, and probably we would
 * also need multiple inheritance because this class also depends
 * on \l PerceptualColor::AbstractDiagram which is itself yet a subclass
 * of QWidget.
 *
 * SHOULDHAVE \l PerceptualColor::ColorPatch::execDrag:
 * Provide more mime types, like Gimp/GEGL and CSS.
 *
 * NICETOHAVE
 * \l PerceptualColor::ChromaLightnessDiagramPrivate::nearestInGamutPixelPosition:
 * A possible optimization
 * might be to search initially, after a new
 * image is available, entire columns, starting from the right, until we hit
 * the first column that has a non-transparent pixel. This information can be
 * used to reduce the search rectangle significantly.
 * Or we could use the gamut boundary, mayby still available from the boundary
 * anti-aliasing.
 *
 * NICETOHAVE STYLING \l PerceptualColor::ChromaHueDiagram:
 * Use a cross cursor when the mouse cursor hovers
 * above the diagrams, for better usability? The cross cursor indicates
 * to the user that an area can be clicked in. Do it only within the gamut
 * (where the color handle can actually go) or in the hole gray circle,
 * which is the mouse sensitive area.
 *
 * SHOULDHAVE \l PerceptualColor::ChromaHueDiagram:
 * Support more mouse buttons. For example, “forward” and “backward” could
 * be used to increase or decrease chroma across various diagrams
 * like \l PerceptualColor::ChromaHueDiagram
 * and \l PerceptualColor::WheelColorPicker. The mouse wheel
 * could change the hue in \l PerceptualColor::ChromaHueDiagram
 * and \l PerceptualColor::WheelColorPicker and
 * even \l PerceptualColor::SwatchBook.
 *
 * SHOULDHAVE \l PerceptualColor::ChromaHueDiagram:
 * What if black or white are out of gamut on L=0.1 or L=99.9?
 * Where are the handles placed?
 * Visible or invisible?
 * How to react (for mouse input, keyboard input, but also API functions
 * like \l PerceptualColor::ChromaHueDiagram::setCurrentColorLch and
 * so on?
 * Should there be always a physical pixel in the middle that is visible (black
 * or white) even when out of gamut?
 * Make sure that \l PerceptualColor::ChromaHueDiagram always shows at least at
 * the central physical pixel with an in-gamut color? Solution: Limit the range
 * of the lightness selector?
 *
 * NICETOHAVE \l PerceptualColor::ChromaHueDiagram::paintEvent:
 * The radial guide line from the diagram center to the handle (which marks the
 * selected gamut point) shows poor anti‑aliasing in certain cases. When the
 * line is nearly horizontal or vertical and its color is black, faint visual
 * artifacts appear under Wayland on a single display scaled to 175%.
 *
 * NICETOHAVE \l PerceptualColor::ChromaHueDiagram::paintEvent:
 * Show the indicator on the color wheel not only while a mouse button
 * is pressed, but also while a keyboard button is pressed.
 *
 * SHOULDHAVE \l PerceptualColor::AbsoluteColor:
 * Test that all possible combinations actually convert.
 * But HSL etc are not supported here, and on the other
 * side \l PerceptualColor::ColorModel is also used
 * in \l PerceptualColor::CssColor and HSL is needed there.
 * Split into two different enums?
 *
 * SHOULDHAVE There are three similar functions like  \c {QDebug
 * operator<<(QDebug dbg, const PerceptualColor::Vec3d &value)}, one for each
 * Vec3l, Vec3f, Vec3ld, which are all specializatios of Vec3. We have the
 * same situation for Mat3. If technically possible, it would be better to
 * substitute them with a single function template that covers all possible
 * specialization of Vec3 respectively Mat3.
 *
 * SHOULDHAVE Make sure apidox actually generate up-to-date documentation
 * available online at api.kde.org based on our metainfo.yaml. (Is that even
 * possible if "release: false" and if this library is not part of Frameworks?)
 * Mayadd add back the link to the online documentation in README.md and its
 * counterpart in src/docs.cpp.
 *
 * SHOULDHAVE In codecheck_quality.sh, run ci-docs.sh in the background at
 * startup to overlap its execution with other tasks. Currently,
 * codecheck_quality.sh runs all tasks sequentially, and ci-docs.sh is
 * single-threaded and slow. Parallelizing the remaining fast/already-parallel
 * tasks offers minimal gains. However, the downside is that parallelizing
 * codecheck_quality.sh also adds script complexity.
 *
 * SHOULDHAVE QDoc autogenerates documentation of the necessary
 * \c {#include <MyHeader>} statement to use an item. However, either these
 * statements are wrong (for classes) or missing (macros, free functions,
 * types, typealiases, enums…). We should fix that.
 */

/*!
 * \page hidpisupport.html
 *
 * \ingroup GroupPublicPages
 *
 * \title High DPI support
 *
 * This library provides native support for High DPI displays.
 *
 * It uses vector-based drawing, which scales seamlessly and delivers crisp
 * rendering, even at fractional scale factors such as 1.25 or 1.5.
 *
 * The only aspect that may require special consideration is icon rendering.
 * \l {https://bugreports.qt.io/browse/QTBUG-89279} {As of Qt6},
 * icons are always rendered in high DPI if available.
 * By default, the library leverages any available icon
 * theme supported by Qt. On Linux, such themes are typically
 * present. Support for Windows, macOS, iOS, and Android was
 * \l {https://doc.qt.io/qt-6/whatsnew67.html#qt-gui-module}
 * {introduced only in Qt 6.7} via a
 * \l {https://doc.qt.io/qt-6/qicon.html#ThemeIcon-enum} {new
 * enum} and by \l {https://www.qt.io/blog/qt-6.7-released}
 * {mapping XDG icon names to platform-native symbols}. Icon themes
 * may include either pixel-based icons (which can appear blurry at scales
 * if not High DPI) or scalable vector icons, typically in SVG format,
 * which has become the de facto standard. SVG-based icons are the most
 * reliable way to ensure visual fidelity at intermediate scale factors
 * like 1.25 or 1.5. Ultimately, the quality of icon rendering above
 * 1.0 scaling depends on the operating system and its available icon set.
 * If no appropriate icon is provided by the system, the library falls back
 * to its own built-in scalable icons.
 *
 * Note that QSvgIconEnginePlugin is a mandatory run-time
 * dependency (see \l {Build instructions and requirements} for details).
 */

/*!
 * \page howtogetstarted.html
 *
 * \ingroup GroupPublicPages
 *
 * \title How to get started
 *
 * How to get started? \l PerceptualColor::ColorDialog provides a
 * perceptual replacement for QColorDialog:
 *
 * \snippet testcolordialog.cpp ColorDialog Get color
 *
 * This is a minimal, but complete example project showing how to use
 * this library:
 *
 * CMakeLists.txt:
 *
 * \quotefile examples/CMakeLists.txt
 *
 * example.cpp:
 *
 * \quotefile examples/example.cpp
 */

/*!
 * \page i18nl10n.html
 *
 * \ingroup GroupPublicPages
 *
 * \title Internationalization and localization
 *
 * \section1 Internationalization
 *
 * This library is internationalized (i18n). This include also support
 * for right-to-left layouts in the widgets.
 *
 * \section1 Localization
 *
 * \section2 Localization – Translation Component
 *
 * The translation of user-visible strings is a global setting for the whole
 * library. The language for the translation is auto-detected depending on
 * the settings of the current computer. You can specify the translation
 * explicitly with \l PerceptualColor::setTranslation, which can also
 * be used to change the translation dynamically (during program execution).
 * The various translations are build directly into the library binary;
 * no external files need to be available or loaded.
 *
 * \section2 Localization – Non-Translation Components
 *
 * All other localization settings (like which decimal separator to use or
 * which date format to use) are individual per widget, depending on the
 * \l {https://doc.qt.io/qt-6/qwidget.html#locale-prop}
 * {QWidget::locale()} property. Changing the localization dynamically
 * (during program execution) is currently not supported.
 */

/*!
 * \page measurementdetails.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Measurement details
 *
 * When this library deals with raster graphics, it simultaneously uses
 * concepts concerning measurement. This page describes the terminology
 * used within the documentation of this library.
 *
 * \section1 Introduction
 * Today’s displays have a wide range of physical pixel density (pixels
 * per length). Displays with a high physical pixel density are called
 * \b {High-DPI displays} or \b {HiDPI displays} or \b {Retina displays}.
 *
 * \section1 Units of measurement
 * As Qt documentation says:
 *      “\e {Qt uses a model where the application coordinate system is
 *      independent of the display device resolution. The application
 *      operates in }device-independent pixels\e {, which are then
 *      mapped to the physical pixels of the display via a scale
 *      factor, known as the }device pixel ratio\e {.}”
 *
 * So when rendering widgets, there are two different units of measurement
 * to consider:
 *
 * \list
 * \li \b {Device-independent pixels} are the  unit of measurement for
 *   widgets, windows, screens, mouse events and so on in Qt.
 * \li \b {Physical pixels} are the unit that measures actual physical
 *   display pixels.
 * \endlist
 *
 * The conversion factor between these two units of measurement is
 * \c {QPaintDevice::devicePixelRatioF()}, a floating point number.
 * It is usually \c {1.00} on classic low-resolution screens. It could be
 * for example \c {1.25} or \c {2.00} on displays with a higher
 * pixel density.
 *
 * \section1 Coordinate points versus pixel positions
 *
 * \list
 *     \li \b {Coordinate points} are points in the mathematical sense, that
 *         means they have zero surface. Coordinate points should be stored as
 *         \e {floating point numbers}.
 *     \li \b {Pixel positions} describe the position of a particular pixel
 *         within the pixel grid. Pixels are surfaces, not points. A pixel is a
 *         square of the width and length \c {1}. The pixel at position
 *         \c {QPoint(x, y)} is the square with the top-left edge at coordinate
 *         point \c {QPoint(x, y)} and the bottom-right edge at coordinate
 *         point \c {QPoint(x+1, y+1)}. Pixel positions should be stored
 *         as \e {integer numbers}.
 * \endlist
 *
 * Some functions (like mouse events) work with pixel positions, other
 * functions (like antialiased floating-point drawing operations) work
 * with coordinate points. It’s important to always distinguish correctly
 * these two different concepts. See https://doc.qt.io/qt-6/coordsys.html
 * for more details about integer precision vs floating point precision
 * on drawing operations. */

/*!
 * \page namespacepollution.html
 *
 * \ingroup GroupPublicPages
 *
 * \title Name⁠space pollution
 *
 * This library avoids namespace pollution and collisions:
 *
 * \list
 *     \li Macros are prefixed with \c {PERCEPTUALCOLOR_}.
 *     \li Symbols that have external linkage are within the
 *         namespace \c {PerceptualColor}. (Exception: The
 *         \l {https://doc.qt.io/qt-5/resources.html}
 *         {Qt resource system} generates functions prefixed
 *         with \c {qInitResources_} and \c {qCleanupResources_} that
 *         are not within the previously mentioned namespace.)
 *     \li Resources within
 *         the \l {https://doc.qt.io/qt-5/resources.html} {Qt resource system}
 *         are within the folder \c {:/PerceptualColor/}.
 * \endlist
 */

/*!
 * \page pimpl.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Pointer to implementation idiom
 *
 * This library uses the \e {pointer to implementation} idiom
 * (also known as pimpl idiom, d-pointer idiom or opaque-pointer idiom)
 * in almost all classes that are part of the public API, and also in
 * some classes that are part of the private API. The idiom is described
 * in detail in the Internet. The following flavour of the idiom is used
 * in our library:
 *
 * \list
 * \li The pointer to the implementation is called \c {d_pointer}. It’s of
 *   type \l PerceptualColor::ConstPropagatingUniquePointer which provides
 *   const-correctness.
 * \li The back pointer is called \c {q_pointer}. (A “q” is just a “d”
 *   pointing in a different direction, get it?). It’s
 *   of type \l PerceptualColor::ConstPropagatingRawPointer which provides
 *   const-correctness.
 * \li \l {https://euroquis.nl//kde/2022/01/31/dptr.html} { The
 *   \c {q_pointer} \e {must not} ever be used in the destructor
 *   of the private implementation.} Rationale: All functions of the
 *   public class could potentially use the d_pointer. However, at the moment
 *   the destructor of the private class has started, the use of the d_pointer
 *   is already undefined behaviour. With some compilers this leads to an
 *   immediate crash, with other compilers it leads to
 *   silent undefined behaviour.
 * \endlist
 *
 * The private classes are \e {not} nested
 * classes of their public counterpart. This is also
 * \l {https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#Using_a_d-Pointer}
 * {what the the KDE community recommends}:
 * \quotation
 *     It is also possible (but not recommended) to declare the private
 *     class […] as a nested private class (e.g. Foo::Private). […] remember
 *     that the nested private class will inherit the public symbol visibility
 *     of the containing exported class. This will cause the functions
 *     of the private class to be named in the dynamic library's symbol
 *     table. […] Other downsides […] include […] the fact that it can't be
 *     forward-declared in unrelated headers anymore (which can be useful to
 *     declare it as a friend class).
 * \endquotation
 *
 * \note This idiom is also used by Qt itself, and Qt even provides some macros
 * and extension points (\c {Q_DECLARE_PRIVATE}, \c {Q_D}, a protected
 * member called \c {d_ptr} in almost all classes…), that help dealing
 * with the pimpl idiom. Though available, these Qt features are not
 * officially documented; and they would also interfere with private
 * implementations of Qt itself without documented behaviour, which seems
 * inappropriate. Furthermore, the Qt pimpl idiom is complicate because
 * it uses (for performance reasons) inheritance between the private
 * implementation classes. This breaks, however, the encapsulation, because
 * all formerly private elements of a class become protected now. Our class
 * hierarchy is not that deep, so the performance gain might not be worth
 * the additional code complexity. Therefore, this library uses a more simple
 * pimpl idiom without inheritance of the private implementation. It has
 * however all the other features of the Qt pimpl idiom, including
 * \c {const} propagating access to the private implementation
 * thanks to \l PerceptualColor::ConstPropagatingUniquePointer and
 * \l PerceptualColor::ConstPropagatingRawPointer. And, at difference
 * to Qt’s pimpl idiom, it keeps private code strictly private.
 * Note however, that switching later from our current pimpl idiom to
 * the polymorphic Qt pimpl idiom would break the binary
 * compatibility. See also the document
 * \l {https://accu.org/journals/overload/18/100/love_1718/} {Interface
 * Versioning in C++} and KDE’s information document
 * \l {https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B}
 * {Binary Compatibility Issues With C++} for details.
 *
 * \note It would be nice to have the d_pointer and q_pointer
 * be themselves be declared \c {const}, because this would
 * clearly communicate that those pointers are not expected to change
 * the address they point to. Unfortunately, apparently this does not
 * work with neither \l PerceptualColor::ConstPropagatingUniquePointer nor
 * \l PerceptualColor::ConstPropagatingRawPointer as it would change also
 * all the access rights to the pointed object to always \c {const}.
 */

/*!
 * \page policies.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Policies
 *
 * \list
 * \li https://community.kde.org/Policies and child pages
 * \li https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B
 * \li https://community.kde.org/Policies/Library_Code_Policy
 * \li https://community.kde.org/Frameworks/Policies
 * \li https://community.kde.org/Frameworks/Frameworks_Localization_Policy
 * \li https://community.kde.org/Frameworks/Frameworks_Documentation_Policy
 * \li https://community.kde.org/Frameworks/Frameworks_Logging_Policy
 * \li https://develop.kde.org/docs/getting-started/common-programming-mistakes/
 * \endlist
 */

/*!
 * \page rangeoflchvalues.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Range of LCH values
 *
 * The LCH values in this library are implemented with the following range:
 *
 * \table
 *     \header
 *         \li
 *         \li l
 *         \li a
 *         \li b
 *         \li c
 *         \li h
 *     \row
 *         \li CIELab/CIELCh
 *         \li [0, 100]
 *         \li [0, 255]
 *         \li [0, 255]
 *         \li [0, 255]
 *         \li [0, 360[
 *     \row
 *         \li Oklab/Oklch
 *         \li [0, 1]
 *         \li [0, 2]
 *         \li [0, 2]
 *         \li [0, 2]
 *         \li [0, 360[
 * \endtable
 *
 * This range is enough to cover the hole range of human perception. (Note
 * that the actual range of human perception has an irregular shape and
 * covers only parts of all possible combinations of LCH values. And
 * the actual gamut of real-word output devices is smaller than the
 * human perception.)
 *
 * \section1 Rationale
 *
 * The gamut of actual human perception within the LAB color model (and
 * its alternative representation LCH) has an irregular shape. Its maximum
 * extensions:
 *
 * \b {Lightness (L)}
 * The maximum range for LAB/LCH lightness is limited by
 * definition: \c {[0, 100]} for CIELch and \c {[0, 1]} for Oklch.
 *
 * \b {Hue (H)}
 * The maximum range for LCH hue is limited by definition to
 * the full circle: \c {[0°, 360°[}.
 *
 * \b {a, b, Chroma (C)}
 * The maximum range for a, b and Chroma (C) is complex. It is \e {not}
 * limited by definition. A useful limit is the actual human perception.
 *
 * \table
 *     \header
 *         \li CIELab/CIELCh
 *         \li a
 *         \li b
 *         \li C
 *     \row
 *         \li Usual implementation¹
 *         \li [−128, 127]
 *         \li [−128, 127]
 *         \li
 *     \row
 *         \li Human perception (Wikipedia)²
 *         \li [−170, 100]
 *         \li [−100, 150]
 *         \li
 *     \row
 *         \li Human perception (2° D50)³
 *         \li [−165.39, 129.05]
 *         \li [−132.62, 146.69]
 *         \li [0, 183.42]
 *     \row
 *         \li Human perception (2° D65)³
 *         \li [−170.84, 147.84]
 *         \li [−129.66, 146.78]
 *         \li [0, 194.84]
 *     \row
 *         \li Human perception (10° D65)³
 *         \li [−164.29, 115.14]
 *         \li [−116.10, 145.53]
 *         \li [0, 186.17]
 * \endtable
 *
 * 1. The range of  \c {[−128, 127]} is in C++ a signed 8‑bit integer. But
 *    this data type usually used in software implementations is (as the table
 *    clearly shows) not enough to cover the hole range of actual human
 *    color perception.
 *
 * 2. Ranges of CIELAB coordinates according to the
 *    \l {https://de.wikipedia.org/w/index.php?title=Lab-Farbraum&oldid=197156292}
 *    {German Wikipedia}.
 *
 * 3. The German association \e {Freie Farbe e. V.} has
 *    published a calculation of the
 *    \l {https://www.freiefarbe.de/artikel/grenzen-des-cielab-farbraums/}
 *    {shape of actual human perception} for various observation angles
 *    and illuminants. This data contains only the CIELAB coordinates
 *    (L, a, b). From this data, the chroma (C) component can be calculated
 *    easily as Pythagoras of the a axis and b axis value pairs:
 *    √(a² + b²) = C.
 *
 * Logically, the chroma value can reach higher values than a and b, however,
 * for simplicity it seems appropriate to use the same range for chroma, a and
 * b. Following these tables, the maximum chroma in human perception in CIELCh
 * is \c {194.84}. As apparently this depends on viewing  conditions,
 * it might be a good idea to use a slightly higher limit, to be sure that the
 * value will never be too small. Here, \c {200} might be a good candidate.
 * However, some gamuts are wider. The \e {LargeRGB-elle-V2-g22.icc}
 * profile goes up to a chroma value of 245. Finally, we have fixed the valid
 * range to 255, because this is for sure enough to cover the human
 * perception, and it will cover almost all existing profiles.
 *
 * For Oklch we have observed up to 1.52 as chroma values when using the
 * \e {LargeRGB-elle-V2-g22.icc} profile. As with CIELCh chroma, we have
 * added a safety margin, rounded up to the next integer, and finally
 * chosen 2 as maximum.
 */

/*!
 * \page releasechecklist.html
 *
 * \ingroup GroupPrivatePages
 *
 * \title Release checklist
 *
 * Update iwyu.cpp and run iwyu and fix all notifications.
 *
 * Qt Designer support for the widgets. Quote from a blog from Viking
 * about Qt Designer plugins:
 * The problem is that you have to build it with exactly the same compiler
 * tool chain as designer was built with, and you have to do it in release
 * mode. Unless your Qt is built in debug, then your plugin needs to be
 * built in debug mode as well. So you can’t just always use the same
 * compiler as you build the application with, if you use the system Qt or
 * a downloaded Qt version.
 *
 * https://keepachangelog.com/en/1.1.0/
 *
 * Add missing friend declarations for unit
 * tests in Public API.
 *
 * Execute all scritps
 *
 * Update screenshots
 *
 * Control compile warnings in default CI jobs
 *
 * Manually control that only actual Public API appears in the
 * Public API documentation.
 *
 * Increase version number in CMakeLists.txt (MAJOR_VERSION,
 * MINOR_VERSION, PATCH_VERSION)
 *
 * From
 * \l {https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B}
 * {KDE’s binary compatibility info page}:
 * In order to make a class to extend in the future you should follow these
 * rules:
 *
 * \list
 * \li Add d-pointer. See below.
 * \li Add non-inlined virtual destructor even if the body is empty.
 * \li Reimplement \c {event} in QObject-derived classes, even if the body
 *   for the function is just calling the base class' implementation. This is
 *   specifically to avoid problems caused by adding a reimplemented virtual
 *   function as discussed below.
 * \li Make all constructors non-inlined.
 * \li Write non-inlined implementations of the copy constructor and assignment
 *   operator unless the class cannot be copied by value. (E.g. classes
 *   inherited from QObject can't be.)
 * \endlist
 *
 * Use \l {https://lvc.github.io/abi-compliance-checker/}
 * {abi-compliance-checker} to control ABI compatibility.
 *
 * Additional to \c {event()}, reimplement also all other
 * virtual functions that might potentially be useful in the future, using a
 * simple implementation that just calls the implementation of the base class.
 * This reduces the risk to have to break binary compatibility and make a new
 * mayor version release in the future.
 *
 * Test rendering manually for common QStyle (including
 * Breeze, Kvantum, Adwaita, Fusion) and style sheets.
 *
 * Add d-pointer using \l PerceptualColor::ConstPropagatingUniquePointer to
 * all public classes, including the non-pimpl classes, to allow for later
 * enhancements.
 *
 * Following
 * \l {https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B}
 * {KDE’s binary compatibility info page} you cannot:
 * “Add a virtual function to a class that doesn't have any virtual functions
 * or virtual bases.” Therefore, make sure all Public API classes have either
 * a virtual function or a virtual base class, if appropriate (though even so,
 * it seems to be impossible to add new virtual functions on Windows while
 * staying binary compatible.)
 *
 * \l {https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c37-make-destructors-noexcept}
 * {The C++ core guidelines recommand that destructors should
 * be noexcept, either implicitly or explicitly.}. Make sure
 * that in our Public API, every destructor is explicitly \c {noexcept}.
 *
 * Remove things form the public API, leaving only the
 * absolutely minimal API that is required by the user, then remove this
 * item from the release checklist.
 * Also change QColor to QRgb in all API
 * that do not offer more than QRgb precision anyway.
 *
 * Consider
 * \l {https://develop.kde.org/docs/getting-started/add-project/#kde-review}
 * {KDE review} and
 * \l {https://community.kde.org/Policies/Application_Lifecycle}
 * {Application Application_Lifecycle}, then remove this item from the
 * release checklist.
 *
 * Test on MacOS and Big-Endian and ARM, which are not in the
 * normal CI pipeline.
 *
 * Full-featured
 *   \l {https://doc.qt.io/qt-6/accessible-qwidget.html} {Qt Widget
 *   accessibility support}. \l PerceptualColor::AccessibleMultiSpinBox
 *   has currently not even an actual implementation! And: More work on other
 *   aspects of accessibility. [This includes to work well with bigger
 *   fonts. Should then the gradient be thicker and the marker
 *   thicker? setAccessibleName().] The application Accerciser provides
 *   inspection possibilities.
 * And: Ensure full accessibility for all functions via both mouse
 * and keyboard. Every function in the dialog should be operable using only the
 * mouse, and likewise using only the keyboard. Currently, at least two issues
 * remain: Adding and removing colors from the swatchbook is not possible via
 * keyboard. The hexadecimal RGB value cannot be modified without using the
 * keyboard. A potential solution for the hexadecimal RGB input might be to
 * replace the current widget with \l PerceptualColor::MultiSpinBox. However,
 * this could interfere with copy-and-paste of the entire value. Can we extend
 * \l PerceptualColor::MultiSpinBox to support full-value copy-paste? If such
 * support introduces binary or source incompatibilities, it must be
 * implemented before any eventual release.
 */
