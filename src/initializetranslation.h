// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef INITIALIZETRANSLATION_H
#define INITIALIZETRANSLATION_H

#include <optional>

#include <qglobal.h>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qstringlist.h>
#else
class QStringList;
#endif

class QCoreApplication;

/** @internal @file
 *
 * Provides the @ref PerceptualColor::initializeTranslation() function.
 *
 * @internal
 *
 * @todo Call <tt>
 * <a href="https://doc.qt.io/qt-5/qmetatype.html#qRegisterMetaType-1">
 * qRegisterMetaType()</a></tt> for all our data types during initialization!?
 *
 * @todo Provide an own documentation page for initialization and localization.
 *
 * @todo Investigate automatic initialization. We need an
 * @ref initializeTranslation() function, but having to call the
 * initialization manually is cumbersome. It would be more comfortable if the
 * initialization would happen automatically. How could this work? With <tt>
 * <a href="https://doc.qt.io/qt-5/qcoreapplication.html#Q_COREAPP_STARTUP_FUNCTION">
 * Q_COREAPP_STARTUP_FUNCTION</a></tt> this function can be called
 * automatically when QCoreApplication starts – but only for
 * <em>dynamic</em> libraries, and not for <em>static</em> libraries – and
 * <a href="https://stackoverflow.com/questions/43333151">
 * there seems to be no workaround</a>. Apparently,
 * <a href="https://www.volkerkrause.eu/2018/10/13/kf5-static-builds.html">
 * for static libraries we have to</a>
 * - either <a href="https://phabricator.kde.org/D13816">request the library user
 *   to call the function manually</a>
 * - or <a href="https://github.com/KDE/kitinerary/commit/72326ed62b7d32965821f2008368783830a76049">
 *   call it in our library code whenever our code is about to do
 *   something that requires previous initialization</a>. To not call it
 *   too often or call it within functions that are executed often, it
 *   might be better to call it in the constructor of <em>all</em> our
 *   classes that need previous initialization. This would be more
 *   comfortable for the library user, and the overhead should be
 *   minimal (at least at subsequent calls the function should memorize
 *   via a static variable that the initialization has happened yet and
 *   return immediately). The disadvantage is that we have to pay attention
 *   to never use <tt>tr()</tt> without calling the initialization first, and
 *   forgetting about this might introduce subtile and difficult-to-discover
 *   bugs that only happen on static builds. (Or we could drop <tt>
 *   <a href="https://doc.qt.io/qt-5/qcoreapplication.html#Q_COREAPP_STARTUP_FUNCTION">
 *   Q_COREAPP_STARTUP_FUNCTION</a></tt> completely, which would give use
 *   uniform behavior between static and dynamic builds and makes
 *   bug-tracking easier.
 * - or use the solution that <a href="https://stackoverflow.com/a/1420261">
 *   instantiates a special class as global variable in every translation
 *   unit</a>, and this class makes sure that the initialization
 *   happens also for static libraries. But wouldn’t this mean that
 *   @ref PerceptualColor::initializeTranslation is called at program
 *   startup, while still no QApplication object is available, and
 *   therefore our @ref PerceptualColor::initializeTranslation
 *   function will crash? And anyway, when the QApplication object is
 *   destroyed and re-created, this pattern will not help. (Might
 *   https://doc.qt.io/qt-5/qglobalstatic.html#Q_GLOBAL_STATIC help?) See
 *   also https://isocpp.org/wiki/faq/ctors#static-init-order for useful
 *   information about static initialization.
 *
 * @todo Provide support for building a static library. This is the only
 * way to test if everything in the initialization actually works fine.
 *
 * @todo Do not reload translations in one thread while another thread
 * uses tr(). But how to make this sure, also because the library user
 * could also reload its own translations while we are using tr(). At least,
 * we could make sure our classes that use translations (widget classes but
 * also @ref PerceptualColor::RgbColorSpace) cannot be instantiated outside
 * of the main thread. But @ref PerceptualColor::RgbColorSpace is
 * meant as thread-safe, so it would be strange if its constructor
 * (and maybe the properties that return translated text) is
 * the own function that is not thread-safe…
 *
 * @todo LocaleChange could be important for spin boxes to accept the new
 * QLocale’s number format.
 *
 * @todo Catch QEvent::LayoutDirectionChange in all widgets? Or the signal
 * QGuiApplication::layoutDirectionChanged()? */

namespace PerceptualColor
{

void initializeTranslation(QCoreApplication *instance, std::optional<QStringList> newUiLanguages);

} // namespace PerceptualColor

#endif // INITIALIZETRANSLATION_H
