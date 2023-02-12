// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef INITIALIZETRANSLATION_H
#define INITIALIZETRANSLATION_H

#include <optional>

#include <qglobal.h>
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
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
 * @todo Investigate automatic initialization. We definitely
 * need to call the @ref PerceptualColor::initializeTranslation()
 * function, but having to call it manually might be
 * cumbersome. It would be more comfortable if the initialization
 * would happen automatically. Apparently, if we want to support both, SHARED
 * <a href="https://www.volkerkrause.eu/2018/10/13/kf5-static-builds.html">
 * and STATIC libraries, we have to</a>
 * - either <a href="https://phabricator.kde.org/D13816">request the library
 *   user to call manually</a> @ref PerceptualColor::initializeTranslation()
 *   on both, STATIC and SHARED libraries. This gives us uniform behavior
 *   between STATIC and SHARED builds and makes bug-tracking easier.
 * - or use <tt>
 *   <a href="http://doc.qt.io/qt-5/qcoreapplication.html#Q_COREAPP_STARTUP_FUNCTION">
 *   Q_COREAPP_STARTUP_FUNCTION</a></tt> to call
 *   @ref PerceptualColor::initializeTranslation() on SHARED
 *   libraries automatically when QCoreApplication starts (works
 *   only for SHARED libraries, and not for STATIC libraries, and
 *   and <a href="https://stackoverflow.com/questions/43333151"> there
 *   seems to be no workaround</a>) and request the library user to
 *   call @ref PerceptualColor::initializeTranslation() manually
 *   only on STATIC libraries.
 * - or make @ref PerceptualColor::initializeTranslation() private and call it
 *   <a href="https://github.com/KDE/kitinerary/commit/72326ed">
 *   in our library code whenever our code is about to do
 *   something that requires previous initialization</a>. To not call it
 *   too often or call it within functions that are executed often, it
 *   might be better to call it in the constructor of <em>all</em> our classes
 *   that need previous initialization. This approach would be more comfortable
 *   for the library user. The disadvantage is that we have to pay attention
 *   to never use <tt>tr()</tt> without calling the initialization first, and
 *   forgetting about this might introduce subtile and difficult-to-discover
 *   bugs.
 * - or make @ref PerceptualColor::initializeTranslation() private and
 *   use the solution that <a href="https://stackoverflow.com/a/1420261">
 *   instantiates a special class as global variable in every translation
 *   unit</a>, and this class makes sure that the initialization
 *   happens also for static libraries. But wouldn’t this mean that
 *   @ref PerceptualColor::initializeTranslation() is called at program
 *   startup, while still no QApplication object is available, and
 *   therefore our @ref PerceptualColor::initializeTranslation()
 *   function will crash? And anyway, when the QApplication object is
 *   destroyed and re-created, this pattern will not help. (Might
 *   https://doc.qt.io/qt-5/qglobalstatic.html#Q_GLOBAL_STATIC help?) See
 *   also https://isocpp.org/wiki/faq/ctors#static-init-order for useful
 *   information about static initialization.
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
