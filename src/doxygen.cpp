// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// This file contains the following documentation:
// – All the @page documentation. Putting all @page documentation in this
//   single file allows that they show up in the documentation in
//   alphabetical order.
// – The namespace documentation

/** @page build Build instructions and requirements
 *
 * Requirements:
 * - LittleCMS 2 (minimum version: 2.0)
 * - Qt 5 (minimum version: 5.15). Components: Core, Gui, Widgets,
 *   DBus, Concurrent, Test. <!-- Qt 5.15 has an API that is close
 *   to Qt 6. It introduces some new functions we are using to avoid
 *   deprecated older functions. -->
 * - CMake
 * - ECM (Extra CMake Modules from KDE)
 * - C++17
 * - Both, the input character set and the execution character set, have
 *   to be UTF8. (See @ref compilercharacterset for more details.)
 * <!--
 *      Qt 5.6 (which is the minimum Qt version required
 *      by this library) only requires C++03. Only starting
 *      with Qt 5.7, Qt itself requires C++11. Source:
 *      https://doc.qt.io/qt-5.9/cmake-manual.html#using-qt-5-with-cmake-older-than-3-1-0
 *
 *      Qt 6 requires minimum C++17, as
 *      https://doc-snapshots.qt.io/qt6-dev/cmake-get-started.html
 *      explains.
 *
 *      Our library code uses C++11 features, for example “constexpr”.
 *
 *      In the CMakeLists.txt file, we set -std=c++17 and we set
 *      also -Wpedantic and -pedantic-errors to enforce it. That is
 *      a useful option for this library if we decide to make it Qt-6-only.
 *      But it is even be useful if we support Qt 5, so we have future-proof
 *      requirements that we do not have to raise soon, and that are a
 *      good base for LTS.
 * -->
 * - Optional: There is also a LittleCMS plugin called
 *   <em>fast_float plug-in</em> that you can include into the
 *   source code of your application and load it in your main function
 *   before using this library. This can make color management faster.
 *   (Note that this plugin has a different license than LittleCMS itself.)
 *
 * Please make sure that you comply with the licences of used libraries.
 *
 * To prepare the build, run cmake. Then, to build and install the library:
 * @code{.unparsed}
 * make && sudo make install
 * @endcode
 *
 * To do unit testing:
 * @code{.unparsed}
 * make build_test test
 * @endcode */

/** @internal
 *
 * @page codingstyle Coding style
 *
 * Always document your code.
 *
 * @section codingstylecpp C++
 *
 * - Provide unit tests for your code.
 * - If working with children within Qt’s object hierarchy, allocate on the
 *   heap and use raw pointers or guarded pointers (`QPointer`). If not,
 *   allocate on the stack or use smart pointers. Prefer Qt’s smart pointers
 *   over the <tt>std</tt> smart pointers of C++.
 * - Use KDE’s
 *   <a href="https://community.kde.org/Policies/Frameworks_Coding_Style">
 *   Frameworks Coding Style</a>. This can be done automatically with
 *   clang-format.
 * - Comments within the code should have this form: <tt>// comment</tt>
 *   <br/> This allows to comment out quickly large parts of the code for
 *   testing purposes.
 * - Comments for Doxygen should have this form: <tt>/⁠** Comment *⁠/</tt>
 *
 * @section codingstylecmake CMake
 *
 * @subsection codingstylecmakeusefuldocumentation Useful documentation
 *
 * - <a href="https://preshing.com/20170522/learn-cmakes-scripting-language-in-15-minutes/">
 *   Introduction to the CMake script language</a>
 * - <a href="https://github.com/onqtam/awesome-cmake">
 *   Curated list of awesome CMake scripts, modules, examples and others</a>
 * - <a href="https://llvm.org/docs/CMakePrimer.html">General introduction
 *   into CMake (both, the script language and the commands)</a>
 *
 * @subsection codingstylecmakemodern Use “Modern CMake”
 *
 * Use <em>Modern CMake</em>: Avoid global settings like
 * <tt>include_directories()</tt>. Use target-based commands like
 * <tt>target_include_directories()</tt> instead. Use
 * <tt>target_link_libraries()</tt> to pull in dependencies, which gives you
 * automatically the correct include directories, compile options etc.
 * When using <tt>target_link_libraries()</tt>, always specify the
 * <tt>PUBLIC</tt> or <tt>PRIVATE</tt> or <tt>INTERFACE</tt> explicitly.
 *
 * @subsection codingstylecmakeindent Indent
 *
 * Use 4 spaces for indenting. Do not use tabs.
 *
 * @subsection codingstylecmakequotationmarks Quotation marks
 *
 * Use quotation marks when you want to represent a string.
 * Do not use quotation marks when you mean a keyword. Example:
 *
 * @code{.unparsed}
 * set(myvar "foo")
 * @endcode
 *
 * Particularly, always quote strings within control structures:
 *
 * @code{.unparsed}
 * if("${myvar}" STREQUAL "bar")
 * @endcode
 *
 * @subsection codingstylecmakeboolean Boolean
 *
 * CMake allows various representations of boolean. Use only the forms
 * <tt>TRUE</tt> and <tt>FALSE</tt>. Do not quote them.
 *
 * @subsection codingstylevariablecheck Check if a variable exists
 *
 * Check if a variable exists like this:
 *
 * @code{.unparsed}
 * if(DEFINED varname)
 * @endcode
 *
 * @subsection codingstylecmakenaming Naming
 *
 * Functions and macros:
 *
 * @code{.unparsed}
 * lower_case()
 * @endcode
 *
 * Control structures: <tt>lower_case()</tt>, with empty <tt>else()</tt>,
 * <tt>endif()</tt>, <tt>endfunction()</tt>…
 *
 * Operators/keywords/directives/extra options: <tt>UPPER_CASE</tt> without
 * quotes. Examples:
 *
 * @code{.unparsed}
 * if(condition STREQUAL "")
 * @endcode
 *
 * @code{.unparsed}
 * do_something(... USE_THIS)
 * @endcode
 *
 * @code{.unparsed}
 * file(COPY ...)
 * @endcode
 *
 * @subsection codingstyleparenthesis Parenthesis
 *
 * Multi-line calls have trailing parenthesis on the same line as
 * last parameter, not on separate line:
 *
 * @code{.unparsed}
 * set(my_variable
 *     "value1"
 *     "value2"
 *     "value3)
 * @endcode
 *
 * @subsection codingstyleorder Order
 *
 * Source and header lists should be ordered alphabetically,
 * subdirectories last, preferably separated by a single empty line.
 *
 * @subsection codingstylelinelength Line length
 *
 * Keep the length of the line below 80 characters when possible.
 * No trailing whitespace.
 *
 * @subsection codingstyledirectories Directories
 *
 * A directory path may not have a trailing <tt>/</tt>. This is to avoid
 * duplicates like <tt>//</tt> when composing paths:
 *
 * @code{.unparsed}
 * set(my_path "/usr/bin/") # BAD
 * set(my_path "/usr/bin") # GOOD
 * set(my_file "${my_path}/my_file")
 * @endcode
 *
 * @subsection codingstylelists Lists
 *
 * Declaring an empty list:
 *
 * @code{.unparsed}
 * set(mylist)
 * @endcode
 *
 * Declaring and initializing a list at the same time:
 *
 * @code{.unparsed}
 * set(mylist
 *     "first item"
 *     "second item"
 *     "third item")
 * @endcode
 *
 * Append to a list:
 *
 * @code{.unparsed}
 * list(APPEND mylist "last item")
 * @endcode
 *
 * @subsection codingstyleprojectname ${PROJECT_NAME}
 *
 * Use <tt>${PROJECT_NAME}</tt> for global variables, targets and labels
 * instead of repeating the project name manually or using fixed names:
 *
 * @code{.unparsed}
 * add_executable(${PROJECT_NAME} …)
 * @endcode */

/** @page compilercharacterset Compiler character sets
 *
 * @section compilercharacterset_ Compiler character sets
 *
 * Compilers have three different character sets:
 * - Input character set (the character set of the source code)
 * - Narrow execution character set
 *   (for <tt>char</tt> and for string literals without prefix)
 * - Wide execution character set
 *   (for <tt>wchar_t</tt> and for string literals with <tt>L</tt> prefix)
 *
 * @subsection inputcharacterset Input character set
 *
 * This source code of this library is encoded in UTF8. Therefore, your
 * compiler must treat is also as UTF-8.
 *
 * Why are we using UTF-8 instead of ASCII?
 * - UTF-8 is more complete than ASCII. ASCII does not even provide basic
 *   typographic symbols like en-dash, em-dash or non-breaking space
 *   characters or quotes.
 * - Unicode exists since 1991, UTF-8 since 1993. It’s time to get rid of
 *   the insufficient ASCII character. It’s time to use Unicode.
 * - We use non-ASCII characters for (typographically
 *   correct) Doxygen documentation and partially also for non-Doxygen
 *   source code comments. It would be quite annoying to use HTML
 *   entities for each non-ASCII character in the Doxygen documentation;
 *   and it would be pointless to do it for non-Doxygen source code
 *   comments.
 * - <tt>i18n()</tt> and <tt>ki18n()</tt> and <tt>tr()</tt> require both,
 *   the source file and <tt>char*</tt> to be encoded in UTF-8; no other
 *   encodings are supported. (Only ASCII would be UTF-8 compatible,
 *   but in practice this encoding is not supported, but only 8859-Latin
 *   encodings, which allow code points higher than 127, which risks to
 *   introduce incompatibilities. Therefore, this would not be a good
 *   option.)
 * - The C++ identifiers of library symbols are however (currently)
 *   ASCII-only.
 *
 * So we use a <tt>static_assert</tt> statement to control this.
 *
 * @subsection narowexecutioncharacterset Narrow execution character set
 *
 * Why are we using UTF-8 as narrow execution character set?
 * - <tt>i18n()</tt> and <tt>ki18n()</tt> and <tt>tr()</tt> require both,
 *   the source file and <tt>char*</tt> to be encoded in UTF-8; no other
 *   encodings are supported.
 * - Implicit conversion from <tt>char*</tt> to <tt>QString</tt> assumes
 *   that <tt>char*</tt> is UTF-8 encoded. Thus we disable this implicit
 *   conversion in <tt>CMakeLists.txt</tt>, it’s wise to stay compatible.
 *
 * Therefore, a static assert controls that really UTF-8 is used
 * as narrow execution character set.
 *
 * @subsection wideexecutioncharacterset Wide execution character set
 *
 * We do not use actively the wide execution character set. There is
 * a usage when communicating with LittleCMS, but there we depend anyway
 * from LittleCMS. Therefore, currently, no static assert forces a specific
 * wide execution character set.
 *
 * @internal
 *
 * @note The static asserts that enforce the character sets are located
 * in @ref staticasserts.cpp. */

/** @internal
 *
 * @page datatypes Data types
 *
 * The library uses in general <tt>int</tt> for integer values, because
 * <tt>QSize()</tt> and <tt>QPoint()</tt> also do. As the library relies
 * heavily on the usage of <tt>QSize()</tt> and <tt>QPoint()</tt>, this
 * seems reasonable.
 *
 * For the same reason, it uses generally <tt>qreal</tt>
 * for floating point values, because <tt>QPointF()</tt> also does.
 *
 * Output colors that are shown on the screen, are usually 8-bit-per-channel
 * colors. For internal transformation, usually <tt>qreal</tt>
 * is used for each channel, giving a better precision and reducing rounding
 * errors. */

/** @internal
 *
 * @page generaltodolist General TODO list
 *
 * This is a TODO list that contains general ideas or issues of this
 * library.
 *
 * @todo Use words as hints for color ranges? Muted/dull colors have a low
 * chroma value. The dark ones (getrübte/gebrochene Farben) are created by
 * adding black (and possibly a bit of white) and include warm tones (the
 * browns) and cool tones (the olives). The light ones are called Pastel
 * colors and are created by adding white (and possibly a bit of
 * black) and include warm tones (like baby pink) and cool
 * tones (like baby blue). Warm colors are located at a color angle of
 * about 45°, cool colors at about 225°. Could we mark this in the diagrams?
 * Cold and warm could be marked by a text outside the color wheel at the
 * given position. The other ones seem to be more complicated: These specific
 * color terms do not have a translation in all languages. (For “muted colors”,
 * there is no good German translation, and for “getrübte Farben”, there is
 * no good English translation.
 *
 * @todo A design question: Following KDE’s HIG, if the command requires
 * additional user interaction to complete, at the end its label there
 * should be an elipsis sfs (…). Currently, this only seems to apply
 * to @ref PerceptualColor::ColorDialogPrivate::m_screenColorPickerButton.
 *
 * @todo https://invent.kde.org/plasma/kdeplasma-addons/-/merge_requests/249
 * allows the user to drag and drop an image file. The image’s average color
 * is calculated and set as current color of Plasma’s color picker widget.
 * Furthermore, it seems that Plasma’s color picker widget also accepts
 * color codes for drag-and-drop. (Which ones? Maybe the #128945 style?)
 * Would this make sense also for our library?
 *
 * @todo ITUR profile: Minimum widget size must be smaller! On high sizes, the
 * inner focus indicator of color wheel too narrow to hue circle.
 * On RGB 255 0 0 no value indicator is visible. The high-chroma values
 * are empty in the diagram!
 *
 * @todo Touch friendly: @ref PerceptualColor::ColorPatch,
 * @ref PerceptualColor::GradientSlider etc. at least
 * as thick as (normal) buttons. Qt6 replaces QTouchDevice by
 * QInputDevice::devices()…
 *
 * @todo For all diagram images: No abort during first interlacing pass. (See
 * @ref PerceptualColor::AsyncImageProvider for details.)
 *
 * @todo All scripts (both, local and CI scripts) should break and stop
 * on every error. When implementing this, be beware of side effects
 * (some local scripts are also called from the CI and so on…).
 *
 * @todo Review @ref PerceptualColor::RgbColorSpace. And change
 * it in order to allow support for Oklab. And maybe Googles
 * <a href="https://github.com/material-foundation/material-color-utilities">
 * HTC</a>.
 *
 * @todo A design question: In the chroma-lightness diagram, the color
 * wheel is a slider and reacts as such. However, in the chroma-hue-diagram,
 * the same color wheel is just decoration (for orientation). Isn’t this
 * different behaviour of two visually identical elements confusing?
 *
 * @todo A design question: Should we use
 * <a href="https://doc.qt.io/qt-6/qt.html#CursorShape-enum">
 * <tt>Qt::CrossCursor</tt></a> for two-dimensional selections like
 * @ref PerceptualColor::ChromaHueDiagram? And should we use
 * <a href="https://doc.qt.io/qt-6/qt.html#CursorShape-enum"><tt>
 * Qt::UpArrowCursor</tt></a> for one-dimensional selections like
 * @ref PerceptualColor::GradientSlider?
 *
 * @todo Remove things form the public API, leaving only the absolutely
 * minimal API that is required by the user.
 *
 * @todo Avoid “final” in the public API (or even altogether?). Implement
 * a codecheck for this.
 *
 * @todo <a href="https://valgrind.org/docs/manual/quick-start.html"> Test with
 * valgrind.org</a>
 *
 * @todo Most widgets of this library allocate in each paint event a new
 * buffer to paint on, before painting on the widget. This is also done
 * because only <tt>QImage</tt> guarantees the same result on all platforms,
 * while <tt>QPixmap</tt> is platform-dependent and Qt does not guarantee that
 * for example <tt>QPainter::Antialiasing</tt> is available on all platforms.
 * However, not using a buffer would save memory! Can we know if the current
 * platform supports <tt>QPainter::Antialiasing</tt> and buffer only if
 * necessary? Or could we at least instantiate only one single buffer per
 * application, that is than shared between all the widgets of our library?
 * This buffer would never be freed, so it will always occupy memory. But
 * this avoids the time-consuming memory allocations at each paint event!
 *
 * @todo Use <tt>QCache</tt> where is makes sense. Maybe
 * @ref PerceptualColor::RgbColorSpace::reduceChromaToFitIntoGamut() or
 * @ref PerceptualColor::RgbColorSpace::isCielchD50InGamut() or
 * @ref PerceptualColor::RgbColorSpace::isCielabD50InGamut() or
 * @ref PerceptualColor::ChromaLightnessDiagramPrivate::nearestInGamutColorByAdjustingChromaLightness(().
 *
 * @todo Switch AbstractDiagram::handleOutlineThickness() and
 * handleRadius() and spaceForFocusIndicator() to use PM_DefaultFrameWidth.
 * (PM_DefaultFrameWidth seems to be used yet in ColorPatch.)
 *
 * @todo QColor is ambiguous: It allows different types of color system,
 * or even various versions of the same color system at different
 * precisions (RGB). This makes it difficult to communicate in the API
 * which is the type of color (model) it contains. Therefore, we should
 * eliminate all its usage within this library (except where it is necessary
 * for API compatibility with Qt).
 *
 * @todo If using the Motif style, the @ref PerceptualColor::ChromaHueDiagram
 * widget, which has circular look-and-feel, has a rectangular focus
 * indicator corresponding the rectangular widget geometry, which looks
 * quite ugly. On the other hand, @ref PerceptualColor::WheelColorPicker
 * has also circular  look-and-feel, but no rectangular focus indicator
 * corresponding the rectangular widget geometry, which looks better.
 * Why doesn’t @ref PerceptualColor::ChromaHueDiagram also behave
 * like @ref PerceptualColor::WheelColorPicker? And
 * how does @ref PerceptualColor::ColorWheel behave?
 *
 * @todo Idea: Provide QColorWidget (like QColorDialog, but inheriting
 * from QWidget, and no buttons). Does this make sense?
 *
 * @todo Use implicit data sharing in @ref PerceptualColor::RgbColorSpace
 * instead of <tt>QSharedPointer\< @ref PerceptualColor::RgbColorSpace \></tt>.
 * But: Wouldn’t this require to make the declaration
 * of @ref PerceptualColor::RgbColorSpace, which was secret until today,
 * public? If so, this would not be good…
 *
 * @todo Dual-Licence with Apache2 and/or Boost licence?
 *
 * @todo Reduce number of exported symbols.
 *
 * @todo Rename main.cpp.
 *
 * @todo Currently, we consider that a mouse clicks click a pixel, but mean
 * the coordinate point in the middle of this pixel. This seems an approach
 * that other widgets (including Qt itself) do not use. And maybe is meant also
 * the coordinate at the top of the mouse cursor, so no offset by (0.5, 0.5)
 * would be necessary. This would give better results (at least on LTR
 * mouse cursors, but what's about crosshair cursors and RTL cursors?)
 *
 * @todo We do some hacks to get circle-like (instead of rectangular)
 * feeling for our circular widgets, which is not perfect when talking
 * about mouse events. It seems that QWidget::setMask() offers an
 * alternative, restricting mouse events (and painting) to a given
 * mask. Does this actually work also for mouse focus management?
 * If so: Has it performance penalties? If not, we should probably use
 * it! And document that those widgets are circular widgets and from
 * a user perspective behave like circular widgets (both paint event
 * and mouse cursor reactions/usage feeling), though from an application
 * programmer (that uses this library) perspective, they are of course
 * rectangular in the layout system. And: Post the results on
 * <a href="https://forum.qt.io/topic/118547/accept-reject-focus-coming-by-mouse-click-based-on-coordinates">
 * this Qt Forum thread</a>.
 *
 * @todo Support more of Qt Style Sheets, for example allow
 * customizing the neutral-gray background of diagrams? If
 * so, @ref PerceptualColor::drawQWidgetStyleSheetAware()
 * is available. Otherwise, remove the currently not used
 * @ref PerceptualColor::drawQWidgetStyleSheetAware() from
 * this library.
 *
 * @todo Unit tests for endianess. Maybe QtEndian can help…
 *
 * @todo General library properties:
 * - test cross-platform support and different byte-orders
 * - Could we integrate more with QStyle? Apparently
 *   <a href="https://api.kde.org/frameworks/frameworkintegration/html/classKStyle.html">
 *   KStyle</a> is a QCommonStyle-based class that provides
 *   support for QString-based query for custom style hints,
 *   control elements and sub elements. There is also
 *   <a href="https://api.kde.org/frameworks/kwidgetsaddons/html/namespaceKStyleExtensions.html">
 *   KStyleExtensions</a> that allows apparently custom widgets to query
 *   for these QString-based custom support, which allows to make the same
 *   query independently of the actual style, without the need to hard-code
 *   individual custom enum values for QStyle::ControlElement (and similar
 *   enum) for each individual style. KStyleExtensions works for all
 *   styles, also for these that are <em>not</em> subclasses of
 *   <tt>KStyle</tt>. It reports if a given query is supported or not
 *   by the underlying style. However, even Breeze, KDE’s default style,
 *   seems not to inherit from KStyle, so the question is if KStyle is
 *   not rather deprecated yet. An alternative would be if a style
 *   has special functions just for PerceptualColor rendering, like
 *   <tt>renderColorWheel</tt>. Than, we could cast the current QStyle
 *   of the application to this style (if the actual current
 *   style <em>is</em> this style), and call these functions. Big
 *   disadvantage: We would have to <em>link</em> against all styles
 *   that we want to support, which makes our library <em>depend</em>
 *   on them, which is not reasonable.
 * - More work on accessibility. [This includes to work well with bigger
 *   fonts. Should then the gradient be thicker and the marker
 *   thicker? setAccessibleName().] The application Accerciser provides
 *   inspection possibilities.
 *
 * @todo From KDE’s binary compatibility info page: In order to make a class
 * to extend in the future you should follow these rules:
 * - add non-inline virtual destructor even if the body is empty.
 * - re-implement event in QObject-derived classes, even if the body for
 *   the function is just calling the base class' implementation. This is
 *   specifically to avoid problems caused by adding a reimplemented virtual
 *   function as discussed below.
 *
 * @todo Following the recommendation of the C++ core guidelines, all
 * destructors should be noexcept.
 *
 * @todo Make genereatescreenshots and the unit tests run on hardware
 * without graphic card. This would be good for Continuous Integration.
 * The XVFB Virtual framebuffer (https://de.m.wikipedia.org/wiki/Xvfb)
 * can do this for X apps. Also, it is possible to start X apps on
 * terminal without a window manager, see
 * https://linuxconfig.org/how-to-run-x-applications-without-a-desktop-or-a-wm
 * but I suppose an X server is still required? There seem to exist also
 * possibilities for Wayland
 * https://unix.stackexchange.com/questions/653672/virtual-wayland-display-server-possible
 * Also, there seems to be a Qt Platform Abstraction called “minimal”
 * (https://doc.qt.io/qt-6/qpa.html) for testing purposes. Elsewhere,
 * if I remember correctly, it was described as useful for testing without X.
 *
 * @todo The missing 3rd diagram (hue-lightness? But: Impossible to model
 * the circular behaviour of the LCH color space: It cannot be a cut through
 * the gamut body, but has to be a curve within the gamut body – not so
 * nice. And: The diagram width has to change with the selected hue if we want
 * to have correct scaling between x axis and y axis…
 *
 * @todo In https://phabricator.kde.org/T12359 is recommended to provide
 * RESET statements for all properties for better compatibility with QML.
 * As we provide widgets, this should not be too important. Are there also
 * good arguments for widgets to provide RESET?
 *
 * @todo Provide an init() function that calls qRegisterMetaType() for
 * all our types?
 *
 * @todo We prevent division by 0 in
 * @ref PerceptualColor::ChromaLightnessDiagramPrivate::fromWidgetPixelPositionToColor().
 * We should make sure this happens also in the other diagram widgets!
 *
 * @todo Add a @ref PerceptualColor::ConstPropagatingUniquePointer to
 * all classes, including the non-pimpl classes, to allow for later
 * enhancements.
 *
 * @todo Remove setDevicePixelRatioF from all *Image classes. (It is
 * confusing, and at the same time there is no real need/benefit.)
 * Complete list: @ref PerceptualColor::ChromaHueImageParameters,
 * @ref PerceptualColor::ColorWheelImage,
 * @ref PerceptualColor::GradientImageParameters.
 *
 * @todo Test also on Windows. (Does it work well with VisualStudio?)
 *
 * @todo Test also Big-Endian compatibility using s390x Linux via Qemu?
 * KDE Invent does not support this out-of-the-box, but with a custom
 * script?
 *
 * @todo Test opaque RGB color space object with a non-export-all version
 * of this library to make sure it actually works for third-party developers.
 *
 * @todo Sometimes, on dual-screen setup, one screen has another DPI than
 * the other screen. Does this library behave correctly in these situations?
 *
 * @todo Would it make sense for @ref PerceptualColor::ChromaHueDiagram and
 * @ref PerceptualColor::ChromaLightnessDiagram to split up their property
 * <tt>currentColor</tt> into two properties: A two-dimensional property
 * for what the user can change, and a one-dimensional property
 * for what only the programmer can change? Or at least provide
 * a Q_INVOKABLE getter and maybe also setter support? So
 * @ref PerceptualColor::WheelColorPicker could use this
 * instead of a lambda expression to set the hue of the
 * @ref PerceptualColor::ChromaLightnessDiagram. And: Also when we don’t do
 * that: When setting <tt>currentColor</tt> to an out-of-gamut color,
 * what happens? Does @ref PerceptualColor::ChromaHueDiagram preserve
 * lightness, while @ref PerceptualColor::ChromaLightnessDiagram preserves
 * hue? Would this make sense?
 *
 * @todo Paint grayed-out handles for all widgets when setEnabled(false)
 * is used! For example 25% lightness instead of black. And 75% lightness
 * instead of white. But: Provide this information
 * in @ref PerceptualColor::AbstractDiagram!
 *
 * @todo It might be interesting to use <tt>QStyle::PM_FocusFrameHMargin</tt>
 * <em>(Horizontal margin that the focus frame will outset the widget
 * by.)</em> Or: <tt>QStyle::PM_FocusFrameVMargin</tt>. Using this for the
 * distance between the focus indicator and the actual content of the widget
 * maybe give a more <tt>QStyle</tt> compliant look. But: If using this,
 * ensurePolished() must be called before!
 *
 * @todo Use <tt>explicit</tt> on all constructors?
 *
 * @todo Screen picker with magnifier glass in two steps
 * similar to https://colorsnapper.com ? Or like in Firefox
 * (Menu → Weitere Werkzeuge → Farbpalette)?
 *
 * @todo Multi-licensing? Add Boost licence and Unlicense as an additional
 * choice?
 *
 * @todo The image cache for the gamut widgets should be updated
 * asynchronously (in its own thread or even various own threads
 * in parallel). While waiting for the result, an empty image could be used.
 * Or it might be useful to provide first a low-resolution version, and only
 * later-on a high-resolution version. Anyway, KDE provides an interesting
 * recommendation: <tt>int Units::humanMoment = 2000;</tt> <em>Time in
 * milliseconds equivalent to the theoretical human moment, which can be
 * used to determine whether how long to wait until the user should be
 * informed of something, or can be used as the limit for how long something
 * should wait before being automatically initiated. / Some examples: /
 * When the user types text in a search field, wait no longer than this
 * duration after the user completes typing before starting the search /
 * When loading data which would commonly arrive rapidly enough to not
 * require interaction, wait this long before showing a spinner</em> See
 * https://api.kde.org/frameworks/plasma-framework/html/classUnits.html#ab22ad7033b2e3d00a862650e82f5ba5e
 * for details.
 *
 * @todo HLC @ref PerceptualColor::MultiSpinBox Allow entering (on the
 * keyboard) of too big hues (361°), negative hues (-1°), negative chroma (-20)
 * and too big chroma (201 or 256) – but do not allow this with the arrows
 * (and how do the arrows react when currently one of these values is
 * shown?). Does this make sense? Anyway do <em>not</em> allow this for
 * lightness, because the lightness is <em>by definition</em> bound
 * to <tt>[0, 100]</tt>.
 *
 * @todo Multi-threaded application of color transforms. It seems okay to
 * create the color transforms in one thread and use the same color
 * transform (once created) from various other threads at the same time
 * as long as the flag <tt>cmsFLAGS_NOCACHE</tt> is used to create the
 * transform.
 *
 * @todo Automatically scale the thickness of the wheel (and maybe even the
 * handle) with varying widget size?
 *
 * @todo Support more color spaces? https://pypi.org/project/colorio/ for
 * example supports a lot of (also perceptually uniform) color spaces…
 *
 * @todo Export less symbols?
 *
 * @todo Check in all classes that take a @ref PerceptualColor::RgbColorSpace
 * that the shared pointer is actually not a <tt>nullptr</tt>. If is
 * <em>is</em> a <tt>nullptr</tt> than throw an exception. Throwing the
 * exception early might make error detection easier for users of the library.
 *
 * * @todo Avoid default arguments like <tt>void test(int i = 0)</tt> in
 * public headers, as changes require re-compilation of the client application
 * to take effect, which might lead to a miss-match of behaviour between
 * application and library, if  compile-time and run-time version of the
 * library are not the same. Is the problem  for default constructors
 * like <tt>ClassName() = default</tt> similar?
 *
 * @todo mark all public non-slot functions with Q_INVOKABLE (except property
 * setters and getters)
 *
 * @todo A good widget library should also be touchscreen-ready. Find
 * an alternative to @ref PerceptualColor::MultiSpinBox? How, for up
 * to 360 values (degrees in step by 1)? Or should the steps simply be bigger?
 *
 * @todo KDE Frameworks / https://marketplace.qt.io/ ?
 * https://community.kde.org/Incubator
 *
 * @todo Provide property bindings as described in
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
 * @todo Provide QML support so that for
 * https://doc.qt.io/qt-5/qml-qtquick-dialogs-colordialog.html (or its
 * Qt6 counterpart) we provide a source compatible alternative, like for
 * QColorWidget? Split the library in three parts (Common, Widgets, QML)?
 * Support <a href="https://mauikit.org/">MauiKit</a>?
 *
 * @todo Apparently QWidget cannot be used from QML. (Though there is
 * https://www.kdab.com/declarative-widgets/ – how does that work?) Is it
 * therefore worth to have complete support for signals in all our QWidget
 * code if this is not really necessary for QWidget (for example for
 * properties that can only be changed by the library user and not by the
 * end user)?
 *
 * @todo Comply with <a href="https://community.kde.org/Policies">KDE
 * policies</a>.
 *
 * @todo Remove all qDebug calls from the source
 *
 * @todo Qt Designer support for the widgets. Quote from a blog from Viking
 * about Qt Designer plugins:
 * The problem is that you have to build it with exactly the same compiler
 * tool chain as designer was built with, and you have to do it in release
 * mode. Unless your Qt is built in debug, then your plugin needs to be
 * built in debug mode as well. So you can’t just always use the same
 * compiler as you build the application with, if you use the system Qt or
 * a downloaded Qt version.
 *
 * @todo Use <a href="https://lvc.github.io/abi-compliance-checker/">
 * abi-compliance-checker</a> to control ABI compatibility.
 *
 * @todo Follow KDE’s <a href="https://hig.kde.org/index.html">HIG</a>
 *
 * @todo Test linking against lcms.h in version 2.0.0 for compatibility
 * (or require more recent version?)
 *
 * @todo Require (by static cast additional to CMake conditions) a minimum
 * Qt version?
 *
 * @todo Would it be a good idea to implement Q_PROPERTY RESET overall? See
 * also https://phabricator.kde.org/T12359
 *
 * @todo Better design on small widget sizes for the whole library.
 *
 * @todo Anti-aliasing the gamut diagrams? Wouldn't this be bad for
 * performance?
 *
 * @todo Use a cross-hair cursor on @ref PerceptualColor::ChromaHueDiagram
 * and @ref PerceptualColor::ChromaLightnessDiagram when the mouse is
 * hovering over the gamut, to show that this surface can be clicked?
 *
 * @todo Touch-friendly interface: Would it be good to have buttons for
 * plus and minus on the various LCH axis which would be companions
 * for @ref PerceptualColor::ChromaHueDiagram and
 * @ref PerceptualColor::ChromaLightnessDiagram and would allow
 * more exactly choose colors also on touch devices?
 *
 * @todo Would it be a good idea to have plus and minus buttons that
 * manipulate the current color along the depth and vividness axis
 * as proposed in “Extending CIELAB - Vividness, V, depth, D, and clarity, T”
 * by Roy S. Berns?
 *
 * @todo Spell checking for the documentation, if possible also grammar
 * checking with LanguageTool */

/** @page hidpisupport High DPI support
 * This library supports High DPI out of the box. You do not need to do
 * much to use it. The widgets provide High DPI support automatically.
 *
 * The only problem are icons. Icons are used for
 * @ref PerceptualColor::MultiSpinBox::addActionButton and for
 * the “refresh” icon and (on some widget styles) for the “Ok”
 * button and the “Cancel” button in @ref PerceptualColor::ColorDialog.
 *
 * @section loadicons Load icons
 *
 * This library uses by default a possibly existing icon theme
 * if available in Qt.
 *
 * - Windows and Mac do not provide icon themes by default, while Linux
 *   usually provides them.
 *
 * - You might bundle icons (as resource) with your application.
 *
 * There are different file formats for icon themes:
 *
 * - Loading raster image icons is supported out-of-the-box by Qt.
 *
 * - Loading SVG icons is supported by Qt’s SVG icon
 *   support plugin. (On Linux this is the file
 *   <tt>plugins/iconengines/libqsvgicon.so</tt>). This
 *   plugin is loaded by Qt automatically if present.
 *
 * SVG is pretty much the standard nowadays and the only
 * reliably way to have crisp icons also on desktop scales like 1.25 or 1.5.
 * Make sure that the plugin is present if you want that SVG icons
 * can be loaded. (On Linux, it seems possible to enforce this by linking
 * dynamically to the plugin itself, if you want to. This forces Linux
 * package managers to produce packages of your application that depend
 * not only on Qt base, but also on the SVG plugin.)
 *
 * If no external icon can be loaded by Qt, this library uses hard-coded
 * fallback icon where necessary.
 *
 * @section rendericons Render icons
 *
 * - <a href="https://bugreports.qt.io/browse/QTBUG-89279">Qt6 renders icons
 *   always with high-DPI.</a>
 * - Qt5 renders icons by default in low resolution. This applies even
 *   for SVG icons on high-DPI displays! Application developers have to enable
 *   high-DPI icon rendering manually with the following code (which should be
 *   put by convention <em>before</em> creating the <tt>QCoreApplication</tt>
 *   object):
 *   <br/><tt>QCoreApplication::setAttribute(Qt::AA_UseHighDpiPixmaps);</tt>
 *
 * Exception: The hard-coded fallback icons of this library render <em>always
 * and on all Qt versions (even if no SVG support is available at all
 * in Qt)</em> at high-DPI! */

/** @page howtogetstarted How to get started
 *
 * How to get started? @ref PerceptualColor::ColorDialog provides a
 * perceptual replacement for QColorDialog:
 * @snippet testcolordialog.cpp ColorDialog Get color
 *
 * This is a minimal, but complete example project showing how to use
 * this library:
 *
 * CMakeLists.txt:
 * @include examples/CMakeLists.txt
 *
 * example.cpp:
 * @include examples/example.cpp */

/** @page i18nl10n Internationalization and localization
 *
 * @section Internationalization
 *
 * This library is internationalized (i18n). This include also support
 * for right-to-left layouts in the widgets.
 *
 * @section Localization
 *
 * This library is also localized (l10n). The localization is divided
 * into two separate areas, which behave differently and independently
 * of each other.
 *
 * 1. Translation.
 * 2. Everything else.
 *
 * @subsection localizationtranslation Translation
 *
 * The translation of user-visible strings is a global setting for the whole
 * library. The language for the translation is auto-detected depending on
 * the settings of the current computer. You can specify the translation
 * explicitly with @ref PerceptualColor::setTranslation(), which can also
 * be used to change the translation dynamically (during program execution).
 * The various translations are build directly into the library binary;
 * no external files need to be available or loaded.
 *
 * @subsection localizationeverythingelse Everything else
 *
 * All other localization settings (like which decimal separator to use or
 * which date format to use) are individual per widget, depending on the
 * <tt><a href="https://doc.qt.io/qt-6/qwidget.html#locale-prop">
 * QWidget::locale()</a></tt> property. Changing the localization dynamically
 * (during program execution) is currently not supported.
 *
 * @internal
 *
 * @todo Support changing the localization dynamically (during program
 * execution). This affects also @ref PerceptualColor::MultiSpinBox and
 * the <tt>QSpinBox</tt> in @ref PerceptualColor::ColorDialog that is
 * used for the opacity and maybe also the RGB-Hex-LineEdit.
 *
 * @todo Provide more localizations! */

/** @page licenseinfo License
 *
 * @copyright
 * - We follow the <a href="https://reuse.software/">“Reuse”
 *   specification</a>. The source code of the library itself (and also this
 *   documentation itself) are dual-licensed. You can use it (at your option)
 *   either under BSD-2-Clause or MIT. Deviating from this, some CMake files
 *   are exclusively BSD-3-Clause.
 * - Other parts of the codebase (which will
 *   <em>not</em> be installed by CMake, examples include <em>autotests</em>
 *   and <em>utils</em>) might have different licenses and/or include
 *   compiled-in resources that have different licenses. */

/** @internal
 *
 * @page measurementdetails Measurement details
 *
 * When this library deals with raster graphics, it simultaneously uses
 * concepts concerning measurement. This page describes the terminology
 * used within the documentation of this library.
 *
 * @section introduction Introduction
 * Today’s displays have a wide range of physical pixel density (pixels
 * per length). Displays with a high physical pixel density are called
 * <b>High-DPI displays</b> or <b>HiDPI displays</b> or <b>Retina displays</b>.
 *
 * @section unitsofmeasurement Units of measurement
 * As Qt documentation says:
 *      “<em>Qt uses a model where the application coordinate system is
 *      independent of the display device resolution. The application
 *      operates in </em>device-independent pixels<em>, which are then
 *      mapped to the physical pixels of the display via a scale
 *      factor, known as the </em>device pixel ratio<em>.</em>”
 *
 * So when rendering widgets, there are two different units of measurement
 * to consider:
 * - <b>Device-independent pixels</b> are the  unit of measurement for
 *   widgets, windows, screens, mouse events and so on in Qt.
 * - <b>Physical pixels</b> are the unit that measures actual physical
 *   display pixels.
 *
 * The conversion factor between these two units of measurement is
 * <tt>QPaintDevice::devicePixelRatioF()</tt>, a floating point number.
 * It is usually <tt>1.00</tt> on classic low-resolution screens. It could be
 * for example <tt>1.25</tt> or <tt>2.00</tt> on displays with a higher
 * pixel density.
 *
 * @section coordinatepointsversuspixelpositions Coordinate points versus pixel positions
 *
 * - <b>Coordinate points</b> are points in the mathematical sense, that
 *   means they have zero surface. Coordinate points should be stored as
 *   <em>floating point numbers</em>.
 * - <b>Pixel positions</b> describe the position of a particular pixel
 *   within the pixel grid. Pixels are surfaces, not points. A pixel is a
 *   square of the width and length <tt>1</tt>. The pixel at position
 *   <tt>QPoint(x, y)</tt> is the square with the top-left edge at coordinate
 *   point <tt>QPoint(x, y)</tt> and the bottom-right edge at coordinate
 *   point <tt>QPoint(x+1, y+1)</tt>. Pixel positions should be stored
 *   as <em>integer numbers</em>.
 *
 * Some functions (like mouse events) work with pixel positions, other
 * functions (like antialiased floating-point drawing operations) work
 * with coordinate points. It’s important to always distinguish correctly
 * these two different concepts. See https://doc.qt.io/qt-6/coordsys.html
 * for more details about integer precision vs floating point precision
 * on drawing operations. */

/** @page namespacepollution Name⁠space pollution
 *
 * This library avoids namespace pollution and collisions:
 *
 * - Macros are prefixed with <tt>PERCEPTUALCOLOR_</tt>.
 * - Symbols that have external linkage are within the
 *   namespace <tt>PerceptualColor</tt>. (Exception: The
 *   <a href="https://doc.qt.io/qt-5/resources.html">
 *   Qt resource system</a> generates functions prefixed
 *   with <tt>qInitResources_</tt> and <tt>qCleanupResources_</tt>
 *   that are not within the previously mentioned namespace.)
 * - Resources within the <a href="https://doc.qt.io/qt-5/resources.html">Qt
 *   resource system</a> are within the folder <tt>:/PerceptualColor/</tt>.  */

/** @internal
 *
 * @page pimpl Pointer to implementation idiom
 *
 * This library uses the <em>pointer to implementation</em> idiom
 * (also known as pimpl idiom, d-pointer idiom or opaque-pointer idiom)
 * in almost all classes that are part of the public API, and also in
 * some classes that are part of the private API. The idiom is described
 * in detail in the Internet. The following flavour of the idiom is used
 * in our library:
 *
 * - The pointer to the implementation is called <tt>d_pointer</tt>. It’s of
 *   type @ref PerceptualColor::ConstPropagatingUniquePointer which provides
 *   const-correctness.
 * - The back pointer is called <tt>q_pointer</tt>. (A “q” is just a “d”
 *   pointing in a different direction, get it?). It’s
 *   of type @ref PerceptualColor::ConstPropagatingRawPointer which provides
 *   const-correctness.
 * - <a href="https://euroquis.nl//kde/2022/01/31/dptr.html"> The
 *   <tt>q_pointer</tt> <em>must not</em> ever be used in the destructor
 *   of the private implementation.</a> Rationale: All functions of the
 *   public class could potentially use the d_pointer. However, at the moment
 *   the destructor of the private class has started, the use of the d_pointer
 *   is already undefined behaviour. With some compilers this leads to an
 *   immediate crash, with other compilers it leads to
 *   silent undefined behaviour.
 *
 * The private classes are <em>not</em> nested
 * classes of their public counterpart. This is also
 * <a href="https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B#Using_a_d-Pointer">
 * what the the KDE community recommends</a>:
 * > It is also possible (but not recommended) to declare the private
 * > class […] as a nested private class (e.g. Foo::Private). […] remember
 * > that the nested private class will inherit the public symbol visibility
 * > of the containing exported class. This will cause the functions
 * > of the private class to be named in the dynamic library's symbol
 * > table. […] Other downsides […] include […] the fact that it can't be
 * > forward-declared in unrelated headers anymore (which can be useful to
 * > declare it as a friend class).
 *
 * @note This idiom is also used by Qt itself, and Qt even provides some macros
 * and extension points (<tt>Q_DECLARE_PRIVATE</tt>, <tt>Q_D</tt>, a protected
 * member called <tt>d_ptr</tt> in almost all classes…), that help dealing
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
 * <tt>const</tt> propagating access to the private implementation
 * thanks to @ref PerceptualColor::ConstPropagatingUniquePointer and
 * @ref PerceptualColor::ConstPropagatingRawPointer. And, at difference
 * to Qt’s pimpl idiom, it keeps private code strictly private.
 * Note however, that switching later from our current pimpl idiom to
 * the polymorphic Qt pimpl idiom would break the binary
 * compatibility. See also the document <em>
 * <a href="https://accu.org/journals/overload/18/100/love_1718/">Interface
 * Versioning in C++</a></em> and KDE’s information document <em>
 * <a
 * href="https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B">
 * Binary Compatibility Issues With C++</a></em> and for details.
 *
 * @note It would be nice to have the d_pointer and q_pointer
 * be themselves be declared <tt>const</tt>, because this would
 * clearly communicate that those pointers are not expected to change
 * the address they point to. Unfortunately, apparently this does not
 * work with neither @ref PerceptualColor::ConstPropagatingUniquePointer nor
 * @ref PerceptualColor::ConstPropagatingRawPointer as it would change also
 * all the access rights to the pointed object to always <tt>const</tt>. */

/** @page qtstylesheetssupport Qt Style Sheets support
 *
 * The widget of this library supports the Qt Style Sheet
 * <a href="https://doc.qt.io/qt-6/stylesheet-reference.html#list-of-stylable-widgets">
 * properties of the Qt class they are derived from</a> only where it
 * makes sense. So you set the <tt>background-color</tt> of
 * a @ref PerceptualColor::MultiSpinBox. But you should not set
 * it for a @ref PerceptualColor::GradientSlider because the point
 * of this widget is to always use the gradient as the background;
 * the same applies for most widgets that showcase colors.
 *
 * When using class names of this library as
 * selectors in Qt Style Sheets, you have to
 * <a href="https://doc.qt.io/Qt-6/stylesheet-syntax.html#widgets-inside-c-namespaces">
 * substitute the namespace separator <tt>::</tt> by <tt>\--</tt></a>
 * to get a working selector: To select the class
 * @ref PerceptualColor::ColorDialog, use the selector
 * <tt>PerceptualColor\--ColorDialog</tt>. */

/** @page lchrange Range of LCH values
 *
 *
 * The LCH values in this library are implemented with the following range:
 *
 * |        |    L     |    C     |    H     |
 * | :----- | :------: |:-------: | :------: |
 * | CIELCh | [0, 100] | [0, 255] | [0, 360[ |
 * | Oklch  |  [0, 1]  |  [0, 2]  | [0, 360[ |
 *
 * This range is enough to cover the hole range of human perception. (Note
 * that the actual range of human perception has an irregular shape and
 * covers only parts of all possible combinations of LCH values. And
 * the actual gamut of real-word output devices is smaller than the
 * human perception.)
 *
 * @internal
 *
 * @section lchrangerationale Rationale
 *
 * The gamut of actual human perception within the LAB color model (and
 * its alternative representation LCH) has an irregular shape. Its maximum
 * extensions:
 *
 * <b>Lightness (L)</b>
 * The maximum range for LAB/LCH lightness is limited by
 * definition: <tt>[0, 100]</tt> for CIELch and <tt>[0, 1]</tt> for Oklch.
 *
 * <b>Hue (H)</b>
 * The maximum range for LCH hue is limited by definition to
 * the full circle: <tt>[0°, 360°[</tt>.
 *
 * <b>a, b, Chroma (C)</b>
 * The maximum range for a, b and Chroma (C) is complex. It is <em>not</em>
 * limited by definition. A useful limit is the actual human perception.
 *
 * | CIELab/CIELCh                 |        a          |         b         |      C      |
 * | :---------------------------- |:----------------: | :---------------: | :---------: |
 * | Usual implementation¹         |    [−128, 127]    |    [−128, 127]    |             |
 * | Human perception (Wikipedia)² |    [−170, 100]    |    [−100, 150]    |             |
 * | Human perception (2° D50)³    | [−165.39, 129.05] | [−132.62, 146.69] | [0, 183.42] |
 * | Human perception (2° D65)³    | [−170.84, 147.84] | [−129.66, 146.78] | [0, 194.84] |
 * | Human perception (10° D65)³   | [−164.29, 115.14] | [−116.10, 145.53] | [0, 186.17] |
 *
 * 1. The range of  <tt>[−128, 127]</tt> is in C++ a signed 8‑bit integer. But
 *    this data type usually used in software implementations is (as the table
 *    clearly shows) not enough to cover the hole range of actual human
 *    color perception.
 * 2. Ranges of CIELAB coordinates according to the
 *    <a href="https://de.wikipedia.org/w/index.php?title=Lab-Farbraum&oldid=197156292">
 *    German Wikipedia</a>.
 * 3. The German association <em>Freie Farbe e. V.</em> has
 *    published a calculation of the
 *    <a href="https://www.freiefarbe.de/artikel/grenzen-des-cielab-farbraums/">
 *    shape of actual human perception</a> for various observation angles
 *    and illuminants. This data contains only the CIELAB coordinates
 *    (L, a, b). From this data, the chroma (C) component can be calculated
 *    easily as Pythagoras of the a axis and b axis value pairs:
 *    √(a² + b²) = C.
 *
 * Following these tables, the maximum chroma in human perception in CIELCh
 * is <tt>194.84</tt>. As apparently this depends on viewing  conditions,
 * it might be a good idea to use a slightly higher limit, to be sure that the
 * value will never be too small. Here, <tt>200</tt> might be a good candidate
 * However, some gamuts are wider. The <em>LargeRGB-elle-V2-g22.icc</em>
 * profile goes up to a chroma value of 245. Finally, we have fixed the valid
 * range to 255, because this is for sure enough to cover the human
 * perception, and it will cover almost all existing profiles. Furthermore,
 * [0, 255] is quite standard in other color models.
 *
 * For Oklch we have observed up to 1.52 as chroma values when using the
 * <em>LargeRGB-elle-V2-g22.icc</em> profile. As with CIELCh chroma, we have
 * added a safety margin, rounded up to the next integer, and finally
 * chosen 2 as maximum chroma.
 *
 * @internal
 *
 * @sa @ref PerceptualColor::CielchD50Values::maximumChroma
 * @sa @ref PerceptualColor::OklchValues::maximumChroma
 *
 * @todo Why is the exact extend of non-imaginary colors unknown? Could it be
 * deduced from the <a href="https://en.m.wikipedia.org/wiki/CIE_1931_color_space#CIE_xy_chromaticity_diagram_and_the_CIE_xyY_color_space">
 * CIE xy chromacity diagram</a>? And: Is 255 enough even for large color
 * spaces like <a href="https://en.m.wikipedia.org/wiki/Rec._2020">
 * Rec. 2020</a> or <a href="https://en.m.wikipedia.org/wiki/DCI-P3">
 * DCI-P3</a>?
 */

/** @page versioninfo Version information at compiletime and runtime
 *
 * This library uses
 * <a href="https://semver.org/">Semantic Versioning 2.0.0</a>.
 *
 * Version information is provided by the header <tt>version.h</tt>
 *
 * To know against which version of this library you are <em>running</em>, use
 * - @ref PerceptualColor::perceptualColorRunTimeVersion
 *
 * To know against which version of this library you are <em>compiling</em>,
 * use
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MAJOR
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION_MINOR
 * - @ref PERCEPTUALCOLOR_COMPILE_TIME_VERSION_PATCH */

/** @brief The namespace of this library.
 *
 * All symbols that are provided in this library are encapsulated within this
 * namespace. */
namespace PerceptualColor
{
} // namespace PerceptualColor
