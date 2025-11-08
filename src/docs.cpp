// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// This file contains the following documentation:
// – All the @page documentation. Putting all @page documentation in this
//   single file allows that they show up in the documentation in
//   alphabetical order.
// – The namespace documentation

/** @page build Build instructions and requirements
 *
 * Build-time dependencies:
 * - LittleCMS 2 (minimum version: 2.0)
 * - Qt 6 (minimum version: 6.0.0).
 *   Components: Core, Gui, Widgets, DBus, Test, Svg.
 * <!--
 *     This library officially supports only Qt 6. Qt 5.15 builds are
 *     maintained solely for internal testing purposes and are not part of the
 *     public API contract. Qt 5.15 provides an API that is yet largely similar
 *     to Qt 6 and includes many of the newer functions, allowing us to avoid
 *     deprecated or removed legacy calls in Qt 6 production code. Note that
 *     Qt 5 support is minimal, intended only for internal use.  For Qt 6, we
 *     aim to keep compiler warnings to a minimum. For Qt 5, compiler warnings
 *     are not a concern.
 *
 *     We keep internal builds working with Qt 5.15 because it offers a
 *     significantly wider range of QStyle implementations than Qt 6. This is
 *     a widget UI library, and thorough testing across diverse styles is
 *     essential.
 * -->
 * - CMake
 * - ECM (Extra CMake Modules from KDE)
 * - C++17
 * - Both, the input character set and the execution character set, have
 *   to be UTF8. (See @ref compilercharacterset for more details.)
 * <!--
 *      Qt 5.6 only required C++03. Only starting
 *      with Qt 5.7, Qt itself requires C++11. Source:
 *      https://doc.qt.io/qt-5.9/cmake-manual.html#using-qt-5-with-cmake-older-than-3-1-0
 *
 *      Qt 6 requires minimum C++17, as
 *      https://doc-snapshots.qt.io/qt6-dev/cmake-get-started.html
 *      explains.
 *
 *      In the CMakeLists.txt file, we set -std=c++17 and we set
 *      also -Wpedantic and -pedantic-errors to enforce it.
 * -->
 * - Optional: There is also a LittleCMS plugin called
 *   <em>fast_float plug-in</em> that you can include into the
 *   source code of your application and load it in your main function
 *   before using this library. This can make color management faster.
 *   (Note that this plugin has a different license than LittleCMS itself.)
 *
 * Additional mandatory run-time dependencies:
 * - QSvgIconEnginePlugin. Available plugins are loaded
 *   automatically by Qt. Therefore, just make sure that this plugin is
 *   present. On Linux, it seems possible to enforce this by linking
 *   dynamically to the plugin itself, if you want to. This forces Linux
 *   package managers to produce packages of your application that depend
 *   not only on Qt base, but also on the SVG plugin. A typical file name of
 *   the plugin is <tt>plugins/iconengines/libqsvgicon.so</tt>.
 *
 * Please make sure that you comply with the licences of used libraries.
 *
 * To prepare the build, run cmake. We provide plenty of CMake options
 * that control the build type (shared/dynamic vs. static), IPO/LPO and
 * much more. The options are self-documenting. When you run CMake,
 * the options are listed with their values and a description of them.
 *
 * Then, to build and install the library:
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
 * When using <tt>target_link_libraries()</tt>, always specify
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
 * @page generallist General to-do list with ideas or issues
 *
 * @todo SHOULDHAVE When using the ITUR profiles, the relation between the
 * diagrams and the numeric values seems strange. And there are artefacts
 * in the diagrams.
 *
 * @todo NICETOHAVE Static codecheck: The doxygen command (at)sa must always
 * be followed by (at)ref, because (at)sa fails silently, but a following
 * (at)ref makes sure we get an error message.
 *
 * @todo NICETOHAVE Currently, <tt>testcolordialog.cpp</tt> modifies the
 * normal configuration file of the library, which interferes with normal
 * usage of the library. It would be nice if <tt>testcolordialog.cpp</tt>
 * could work independently from the normal configuration file, similar to
 * <tt>testperceptualsettings.cpp</tt> which uses a custon identifier to
 * prevent modifications of the normal ini configuration file.
 *
 * @todo SHOULDHAVE Check against Q_NAMESPACE and Q_ENUM_NS because they cannot
 * work reliably when namespaces do accross header files (a double declaration
 * would break Q_NAMESPACE), and the gain isn't work the problems.
 *
 * @todo SHOULDHAVE <tt>/usr/share/color/icc/sRGB_v4_ICC_preference.icc</tt>
 * has a strange blackpoint in chroma-lightness diagram (1/10 above the
 * 0 line)
 *
 *
 * @todo SHOULDHAVE Full-featured accessibility support
 *
 * @todo NCIETOHAVE Add <tt>QToolTip</tt> value explaining the accepted keys
 * and mouse movements?
 *
 * @todo SHOULDHAVE Circular diagrams should be right-aligned on RTL layouts.
 *
 * @todo NICETOHAVE Could we get better rendering performance? Even online
 * tools like <a href="https://bottosson.github.io/misc/colorpicker/#ff2a00">
 * this</a> or <a href="https://oklch.evilmartians.io/#65.4,0.136,146.7,100">
 * this</a> get quite good rendering performance. How do they do that?
 *
 * @todo NICETOHAVE Use words as hints for color ranges? Muted/dull colors have
 * a low chroma value. The dark ones (getrübte/gebrochene Farben) are created by
 * adding black (and possibly a bit of white) and include both, warm tones (the
 * browns) and cool tones (the olives). The light ones are called Pastel
 * colors and are created by adding white (and possibly a bit of
 * black) and also include both warm tones (like baby pink) and cool
 * tones (like baby blue). Warm colors are located at a color angle of
 * about 45°, cool colors at about 225°. Could we mark this in the diagrams?
 * Could we provide an option to enable labels for the
 * <a href="https://www.handprint.com/HP/WCL/color12.html">warm-cool</a>?
 * Cold and warm could be marked by a text outside the color wheel at the
 * given position. The other ones seem to be more complicated: These specific
 * color terms do not have a translation in all languages. (For “muted colors”,
 * there is no good German translation, and for “getrübte Farben”, there is
 * no good English translation.
 * Another idea might be to mark the wavelength of the corresponding spectral
 * color outside around the color wheel, respectively the term
 * “Line of purples” for the non-spectral colors.
 *
 * @todo NICETOHAVE
 * https://invent.kde.org/plasma/kdeplasma-addons/-/merge_requests/249
 * allows the user to drag and drop an image file. The image’s average color
 * is calculated and set as current color of Plasma’s color picker widget.
 * Furthermore, it seems that Plasma’s color picker widget also accepts
 * color codes for drag-and-drop. (Which ones? Maybe the #128945 style?)
 * Would this make sense also for our library?
 *
 * @todo SHOULDHAVE
 * ITUR profile: Minimum widget size must be smaller! On high sizes, the
 * inner focus indicator of color wheel too narrow to hue circle.
 * On RGB 255 0 0 no value indicator is visible. The high-chroma values
 * are empty in the diagram!
 *
 * @todo NICETOHAVE
 * All scripts (both, local and CI scripts) should break and stop
 * on every error. When implementing this, be beware of side effects
 * (some local scripts are also called from the CI and so on…).
 *
 * @todo NICETOHAVE A design question: Should we use
 * <a href="https://doc.qt.io/qt-6/qt.html#CursorShape-enum"><tt>
 * Qt::UpArrowCursor</tt></a> for one-dimensional selections like
 * @ref PerceptualColor::GradientSlider?
 *
 * @todo NICETOHAVE Most widgets of this library allocate in each paint event
 * a new buffer to paint on, before painting on the widget. This is also done
 * because only <tt>QImage</tt> guarantees the same result on all platforms,
 * while <tt>QPixmap</tt> is platform-dependent and Qt does not guarantee that
 * for example <tt>QPainter::Antialiasing</tt> is available on all platforms.
 * Could we at least instantiate only one single buffer per
 * application, that is than shared between all the widgets of our library?
 * This buffer would never be freed, so it will always occupy memory. But
 * this avoids the time-consuming memory allocations at each paint event!
 *
 * @todo NICETOHAVE Use <tt>QCache</tt> where is makes sense. Maybe
 * @ref PerceptualColor::RgbColorSpace::reduceCielchD50ChromaToFitIntoGamut() or
 * @ref PerceptualColor::RgbColorSpace::isCielchD50InGamut() or
 * @ref PerceptualColor::RgbColorSpace::isCielabD50InGamut() or
 * @ref PerceptualColor::ChromaLightnessDiagramPrivate::nearestInGamutCielchD50ByAdjustingChromaLightness(().
 *
 * @todo SHOULDHAVE If using the Motif style (only available in Qt 5, not
 * in Qt 6), the @ref PerceptualColor::ChromaHueDiagram
 * widget, which has circular look-and-feel, has a rectangular focus
 * indicator corresponding the rectangular widget geometry, which looks
 * quite ugly. On the other hand, @ref PerceptualColor::WheelColorPicker
 * has also circular  look-and-feel, but no rectangular focus indicator
 * corresponding the rectangular widget geometry, which looks better.
 * Why doesn’t @ref PerceptualColor::ChromaHueDiagram also behave
 * like @ref PerceptualColor::WheelColorPicker? And
 * how does @ref PerceptualColor::ColorWheel behave?
 *
 * @todo SHOULDHAVE We do some hacks to get circle-like (instead of rectangular)
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
 * @todo SHOULDHAVE Support more of Qt Style Sheets, for example allow
 * customizing the neutral-gray background of diagrams? If
 * so, @ref PerceptualColor::drawQWidgetStyleSheetAware()
 * is available. Otherwise, remove the currently not used
 * @ref PerceptualColor::drawQWidgetStyleSheetAware() from
 * this library.
 *
 * @todo NICETOHAVE Could we integrate more with QStyle? Apparently
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
 *
 * @todo SHOULDHAVE Full-featured
 *   <a href="https://doc.qt.io/qt-6/accessible-qwidget.html">Qt Widget
 *   accessibility support</a>. And:
 *   More work on accessibility. [This includes to work well with bigger
 *   fonts. Should then the gradient be thicker and the marker
 *   thicker? setAccessibleName().] The application Accerciser provides
 *   inspection possibilities.
 *
 * @todo NICETOHAVE
 * In https://phabricator.kde.org/T12359 is recommended to provide
 * RESET statements for all properties for better compatibility with QML.
 * As we provide widgets, this should not be too important. Are there also
 * good arguments for widgets to provide RESET?
 *
 * @todo SHOULDHAVE We prevent division by 0 in
 * @ref PerceptualColor::ChromaLightnessDiagramPrivate::fromWidgetPixelPositionToCielchD50().
 * We should make sure this happens also in the other diagram widgets!
 *
 * @todo NICETOHAVE Remove setDevicePixelRatioF from all *Image classes. (It is
 * confusing, and at the same time there is no real need/benefit.)
 * Complete list: @ref PerceptualColor::ChromaHueImageParameters,
 * @ref PerceptualColor::ColorWheelImage,
 * @ref PerceptualColor::GradientImageParameters.
 *
 * @todo SHOWSTOPPER
 * Sometimes, on dual-screen setup, one screen has another DPI than
 * the other screen. Does this library behave correctly in these situations?
 * Especially the rounding for the transparency background image: The size of
 * the squares of the transparency background should not change across scale
 * factors! Instead: No pixel alignment anymore, use floating-point coordinates
 * on all scale factors instead!
 *
 * @todo SHOULDHAVE When setting <tt>currentColor</tt> to an out-of-gamut color,
 * what happens? Does @ref PerceptualColor::ChromaHueDiagram preserve
 * lightness, while @ref PerceptualColor::ChromaLightnessDiagram preserves
 * hue? Would this make sense?
 *
 * @todo NICETOHAVE Paint grayed-out
 * handles for all widgets when <tt>setReadOnly(false)</tt>
 * is used! For example 25% lightness instead of black. And 75% lightness
 * instead of white. But: Provide this information
 * in @ref PerceptualColor::AbstractDiagram!
 * And: Gray out the hole diagram, making the diagram itself grayscale and
 * maybe even the gamut itself invisible when <tt>setEnabled(false)</tt>
 * is used.
 *
 * @todo SHOULDHAVE Switch AbstractDiagram::handleOutlineThickness() and
 * handleRadius() and spaceForFocusIndicator() to use PM_DefaultFrameWidth.
 * (PM_DefaultFrameWidth seems to be used yet in ColorPatch.)
 *
 * @todo SHOULDHAVE
 * It might be interesting to use <tt>QStyle::PM_FocusFrameHMargin</tt>
 * <em>(Horizontal margin that the focus frame will outset the widget
 * by.)</em> Or: <tt>QStyle::PM_FocusFrameVMargin</tt>. Using this for the
 * distance between the focus indicator and the actual content of the widget
 * maybe give a more <tt>QStyle</tt> compliant look. But: If using this,
 * ensurePolished() must be called before!
 *
 * @todo SHOULDHAVE Screen picker with magnifier glass in two steps
 * similar to https://colorsnapper.com which has a normal-sized magnifying
 * glass, and also a giant magnifying glass occupying almost the hole screen?
 * How does its UI work? Is there the normal magnifying glass, then a first
 * click, then the giant magnifying glass? Advantage: Users are always aware
 * of the function. Disadvantage: It always requires two clicks, what might
 * be cumbersome. Or do you get the giant magnifying glass only on demand,
 * for example by pushing the Ctrl key? Or should we do it more simply
 * <a href="https://web.archive.org/web/20250626170102/https://www.firefox.com/en-US/features/eyedropper/">
 * like in Firefox</a> (Application Menu → More tools → Eyedropper)?
 * Also Spectacle (the new version in Plasma 6.4)
 * <a href="https://kde.org/announcements/plasma/6/6.4.0/#screenshots--screen-recording">
 * has a magnifying glass with haircross</a>, but is this only on X11 or also
 * on Wayland? And how does it work, because Wayland is very strict about
 * security?
 *
 * @todo SHOULDHAVE KDE is switching from
 * Doxygen to QDoc, at least for Frameworks. The
 * <a href="https://mail.kde.org/pipermail/kde-devel/2025-June/003710.html">
 * QDoc-based documentation it built within CMake targets</a> and requires
 * <a href="https://invent.kde.org/-/snippets/3206">quite some syntax
 * changes</a> compared to Doxygen.
 * <a href="https://doc.qt.io/qt-6/qdoc-index.html">QDoc has also its own
 * manual.</a> Is it possible to support both, Doxygen and QDoc, at the same
 * time? If not, can we get with QDoc as much error tracking in
 * Continious Integration as we get currently with Doxygen?
 *
 * @todo NICETOHAVE KDE provides an interesting
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
 * for details. Use this instead of interlacing big images?
 *
 * @todo NICETOHAVE
 * Support more color spaces? https://pypi.org/project/colorio/ for
 * example supports a lot of (also perceptually uniform) color spaces…
 *
 * @todo SHOULDHAVE
 * Check in all classes that take a @ref PerceptualColor::RgbColorSpace
 * that the shared pointer is actually not a <tt>nullptr</tt>. If is
 * <em>is</em> a <tt>nullptr</tt> than throw an exception. Throwing the
 * exception early might make error detection easier for users of the library.
 *
 * @todo SHOULDHAVE
 * Avoid default arguments like <tt>void test(int i = 0)</tt> in
 * public headers, as changes require re-compilation of the client application
 * to take effect, which might lead to a miss-match of behaviour between
 * application and library, if  compile-time and run-time version of the
 * library are not the same. Is the problem  for default constructors
 * like <tt>ClassName() = default</tt> similar?
 *
 * @todo SHOULDHAVE mark all public
 * non-slot functions with Q_INVOKABLE (except property
 * setters and getters)
 *
 * @todo SHOULDHAVE KDE Frameworks / https://marketplace.qt.io/ ?
 *
 * @todo SHOULDHAVE Property bindings: Can a Q_PROPERTY declaration be changed
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
 * @todo NICETOHAVE Provide QML support so that for
 * https://doc.qt.io/qt-5/qml-qtquick-dialogs-colordialog.html (or its
 * Qt6 counterpart) we provide a source compatible alternative, like for
 * QColorWidget? Split the library in three parts (Common, Widgets, QML)?
 * Support <a href="https://mauikit.org/">MauiKit</a>?
 * Apparently QWidget cannot be used from QML. (Though there is
 * https://www.kdab.com/declarative-widgets/ – how does that work?)
 *
 * @todo SHOULDHAVE Comply with <a href="https://community.kde.org/Policies">KDE
 * policies</a>.
 *
 * @todo SHOULDHAVE Remove all qDebug calls from the source
 *
 * @todo NICETOHAVE
 * Qt Designer support for the widgets. Quote from a blog from Viking
 * about Qt Designer plugins:
 * The problem is that you have to build it with exactly the same compiler
 * tool chain as designer was built with, and you have to do it in release
 * mode. Unless your Qt is built in debug, then your plugin needs to be
 * built in debug mode as well. So you can’t just always use the same
 * compiler as you build the application with, if you use the system Qt or
 * a downloaded Qt version.
 *
 * @todo SHOULDHAVE
 * Use <a href="https://lvc.github.io/abi-compliance-checker/">
 * abi-compliance-checker</a> to control ABI compatibility.
 *
 * @todo SHOULDHAVE
 * Test linking against lcms.h in version 2.0.0 for compatibility
 * (or require more recent version?)
 *
 * @todo NICETOHAVE Would it be a good idea to have plus and minus buttons that
 * manipulate the current color along the depth and vividness axis
 * as proposed in “Extending CIELAB - Vividness, V, depth, D, and clarity, T”
 * by Roy S. Berns?
 *
 * @todo SHOULDHAVE Move diagrams from Cielch to Oklch
 *
 * @todo NICETOHAVE In our custom CI, test (and fail) not only on clang
 * warnings but also on gcc warnings.
 *
 * @todo SHOULDHAVE Test RTL functionality and text layout, using the yet
 * available Arabic translation.
 *
 * @todo NICETOHAVE How can the diagrams actually output 16 bits per channel on
 * the screen instead of the current 8 bits? There isn’t any Qt API for that,
 * isn’t it?
 *
 * @todo SHOWSTOPPER Which type of sRGB does Wayland assume? Real sRGB with
 * the defined gamma functions per section, or simplified 2.2-overall gamma,
 * or even linear?
 */

/** @page hidpisupport High DPI support
 *
 * This library provides native support for High DPI displays.
 *
 * It uses vector-based drawing, which scales seamlessly and delivers crisp
 * rendering, even at fractional scale factors such as 1.25 or 1.5.
 *
 * The only aspect that may require special consideration is icon rendering.
 * <a href="https://bugreports.qt.io/browse/QTBUG-89279">As of Qt6</a>,
 * icons are always rendered in high DPI if available.
 * By default, the library leverages any available icon
 * theme supported by Qt. On Linux, such themes are typically
 * present. Support for Windows, macOS, iOS, and Android was
 * <a href="https://doc.qt.io/qt-6/whatsnew67.html#qt-gui-module">
 * introduced only in Qt 6.7</a> via a
 * <a href="https://doc.qt.io/qt-6/qicon.html#ThemeIcon-enum">new
 * enum</a> and by <a href="https://www.qt.io/blog/qt-6.7-released">
 * mapping XDG icon names to platform-native symbols</a>. Icon themes
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
 * dependency (see @ref build for details).
 */

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
 * @section localizationtranslation Localization – Translation Component
 *
 * The translation of user-visible strings is a global setting for the whole
 * library. The language for the translation is auto-detected depending on
 * the settings of the current computer. You can specify the translation
 * explicitly with @ref PerceptualColor::setTranslation(), which can also
 * be used to change the translation dynamically (during program execution).
 * The various translations are build directly into the library binary;
 * no external files need to be available or loaded.
 *
 * @section localizationnontranslation Localization – Non-Translation Components
 *
 * All other localization settings (like which decimal separator to use or
 * which date format to use) are individual per widget, depending on the
 * <tt><a href="https://doc.qt.io/qt-6/qwidget.html#locale-prop">
 * QWidget::locale()</a></tt> property. Changing the localization dynamically
 * (during program execution) is currently not supported.
 *
 * @internal
 *
 * @todo NICETOHAVE Support changing the localization  (like which decimal
 * separator to use or which date format to use) dynamically (during program
 * execution). This affects also @ref PerceptualColor::MultiSpinBox and
 * the <tt>QSpinBox</tt> in @ref PerceptualColor::ColorDialog that is
 * used for the opacity and maybe also the RGB-Hex-LineEdit.
 */

/** @page licenseinfo License
 *
 * @copyright
 * - We follow the <a href="https://reuse.software/">“Reuse”
 *   specification</a>.
 * - The files from which the library (and this documentation as well)
 *   are generated do not all have the same license; instead, each file
 *   is subject to one of the following permissive licenses:
 *   - BSD-2-Clause OR MIT (for example, some C++ source code files)
 *   - MIT (for example, some icons)
 *   - BSD-3-Clause (for example, some CMake files)
 *   - CC0-1.0 (for example, some color profiles)
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

/** @page rangeoflchvalues Range of LCH values
 *
 *
 * The LCH values in this library are implemented with the following range:
 *
 * |               |    l     |    a     |    b     |    c     |    h     |
 * | :------------ | :------: | :------: | :------: |:-------: | :------: |
 * | CIELab/CIELCh | [0, 100] | [0, 255] | [0, 255] | [0, 255] | [0, 360[ |
 * | Oklab/Oklch   |  [0, 1]  |  [0, 2]  |  [0, 2]  |  [0, 2]  | [0, 360[ |
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
 * Logically, the chroma value can reach higher values than a and b, however,
 * for simplicity it seems appropriate to use the same range for chroma, a and
 * b. Following these tables, the maximum chroma in human perception in CIELCh
 * is <tt>194.84</tt>. As apparently this depends on viewing  conditions,
 * it might be a good idea to use a slightly higher limit, to be sure that the
 * value will never be too small. Here, <tt>200</tt> might be a good candidate.
 * However, some gamuts are wider. The <em>LargeRGB-elle-V2-g22.icc</em>
 * profile goes up to a chroma value of 245. Finally, we have fixed the valid
 * range to 255, because this is for sure enough to cover the human
 * perception, and it will cover almost all existing profiles.
 *
 * For Oklch we have observed up to 1.52 as chroma values when using the
 * <em>LargeRGB-elle-V2-g22.icc</em> profile. As with CIELCh chroma, we have
 * added a safety margin, rounded up to the next integer, and finally
 * chosen 2 as maximum.
 *
 * @internal
 *
 * @sa @ref PerceptualColor::CielchD50Values::maximumChroma
 * @sa @ref PerceptualColor::OklchValues::maximumChroma
 *
 * @todo NICETOHAVE Why is the exact extend of non-imaginary
 * colors unknown? Could it be deduced from the
 * <a href="https://en.m.wikipedia.org/wiki/CIE_1931_color_space#CIE_xy_chromaticity_diagram_and_the_CIE_xyY_color_space">
 * CIE xy chromacity diagram</a>? And: Are CIELCh chroma 255 and
 * Oklch chroma 2.00 enough even for large color
 * spaces like <a href="https://en.m.wikipedia.org/wiki/Rec._2020">
 * Rec. 2020</a> or <a href="https://en.m.wikipedia.org/wiki/DCI-P3">
 * DCI-P3</a>?
 */

/** @internal
 *
 * @page releasechecklist Release checklist
 *
 * @todo SHOULDHAVE https://keepachangelog.com/en/1.1.0/
 *
 * @todo SHOULDHAVE Add missing friend declarations for unit
 * tests in Public API.
 *
 * @todo SHOULDHAVE Execute all scritps
 *
 * @todo SHOULDHAVE Update screenshots
 *
 * @todo SHOULDHAVE Control compile warnings in default CI jobs
 *
 * @todo SHOULDHAVE Manually control that only actual Public API appears in the
 * Public API documentation.
 *
 * @todo SHOULDHAVE Increase version number in CMakeLists.txt (MAJOR_VERSION,
 * MINOR_VERSION, PATCH_VERSION)
 *
 * @todo SHOULDHAVE <tt>operator&lt;&lt;</tt> and <tt>operator==</tt> actually
 * cover all data members.
 *
 * @todo SHOULDHAVE From
 * <a href="https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B">
 * KDE’s binary compatibility info page</a>:
 * In order to make a class to extend in the future you should follow these
 * rules:
 * - Add d-pointer. See below.
 * - Add non-inline virtual destructor even if the body is empty.
 * - Reimplement <tt>event</tt> in QObject-derived classes, even if the body
 *   for the function is just calling the base class' implementation. This is
 *   specifically to avoid problems caused by adding a reimplemented virtual
 *   function as discussed below.
 * - Make all constructors non-inline.
 * - Write non-inline implementations of the copy constructor and assignment
 *   operator unless the class cannot be copied by value. (E.g. classes
 *   inherited from QObject can't be.)
 *
 * @todo SHOULDHAVE
 * Add d-pointer using @ref PerceptualColor::ConstPropagatingUniquePointer to
 * all public classes, including the non-pimpl classes, to allow for later
 * enhancements.
 *
 * @todo SHOULDHAVE Following
 * <a href="https://community.kde.org/Policies/Binary_Compatibility_Issues_With_C%2B%2B">
 * KDE’s binary compatibility info page</a> you cannot:
 * “Add a virtual function to a class that doesn't have any virtual functions
 * or virtual bases.” Therefore, make sure all Public API classes have either
 * a virtual function or a virtual base class, if appropriate (though even so,
 * it seems to be impossible to add new virtual functions on Windows while
 * staying binary compatible.)
 *
 * @todo SHOULDHAVE
 * <a href="https://github.com/isocpp/CppCoreGuidelines/blob/master/CppCoreGuidelines.md#c37-make-destructors-noexcept">
 * The C++ core guidelines recommand that destructors should
 * be <tt>noexcept</tt>, either implicitly or explicitly.</a>. Make sure
 * that in our Public API, every destructor is explicitly <tt>noexcept</tt>.
 *
 * @todo SHOULDHAVE Remove things form the public API, leaving only the
 * absolutely minimal API that is required by the user, then remove this
 * item from the release checklist.
 *
 * @todo SHOULDHAVE Test on MacOS and Big-Endian, which are both not in the
 * normal CI pipeline.
 *
 * @todo SHOULDHAVE Ensure full accessibility for all functions via both mouse
 * and keyboard. Every function in the dialog should be operable using only the
 * mouse, and likewise using only the keyboard. Currently, at least two issues
 * remain: Adding and removing colors from the swatchbook is not possible via
 * keyboard. The hexadecimal RGB value cannot be modified without using the
 * keyboard. A potential solution for the hexadecimal RGB input might be to
 * replace the current widget with @ref PerceptualColor::MultiSpinBox. However,
 * this could interfere with copy-and-paste of the entire value. Can we extend
 * @ref PerceptualColor::MultiSpinBox to support full-value copy-paste? If such
 * support introduces binary or source incompatibilities, it must be
 * implemented before any eventual release.
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
