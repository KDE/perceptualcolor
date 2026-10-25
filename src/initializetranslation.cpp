// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "initializetranslation.h"

#include "initializelibraryresources.h"
#include "logging.h"
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlist.h>
#include <qlocale.h>
#include <qloggingcategory.h>
#include <qmutex.h>
#include <qpointer.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qthread.h>
#include <qtranslator.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \brief Set the translation for the whole library.
 *
 * After calling this function, all objects of this library that are created
 * from now on are translated according to translation that was set.
 *
 * Objects that were yet existing when calling are \e {not always}
 * automatically updated: When calling this function, Qt sends
 * a \c {QEvent::LanguageChange} event only to top-level widgets,
 * and these will get updated then. You can send the event yourself
 * to non-top-level widgets to update those widgets also.
 *
 * If you create objects that use translations \e {before} a translation
 * has been set explicitly, than automatically an environment-dependant
 * translation is loaded.
 *
 * It is safe to call this function multiple times.
 *
 * \b Precondition: There exists exactly \e {one} instance
 * of \c {QCoreApplication}
 * to which the parameter points. This function is called from the same thread
 * in which the \c {QCoreApplication} instance lives.
 *
 * \a instance A pointer to the \c {QCoreApplication} instance for
 *        which the initialization will be done.
 *
 * \a newUiLanguages List of translations, ordered by priority, most
 *        important ones first, like in \c {QLocale::uiLanguages()}. If
 *        \c {std::optional::has_value()} than the translation is
 *        initialized for this value, and previously loaded translations are
 *        removed. If \c {std::optional::has_value()} is \c {false},
 *        than it depends: If no translation has been initialized so far, than
 *        the translation is initialized to an environment-dependent default
 *        value; otherwise there last initialization is simply repeated.
 *
 *  The translation is initialized, even if a previous initialization
 * had been destroyed by deleting the previous QCoreApplication object.
 *
 *
 *
 * We definitely
 * need to call this
 * function, but having to call it manually might be
 * cumbersome. It would be more comfortable if the initialization
 * would happen automatically. Apparently, if we want to support both, SHARED
 * \l {https://www.volkerkrause.eu/2018/10/13/kf5-static-builds.html}
 * {and STATIC libraries, we have a couple of options.}
 * We could
 *
 * \list
 * \li either \l {https://phabricator.kde.org/D13816} {request the library
 *   user to call manually} this function
 *   on both, STATIC and SHARED libraries. This gives us uniform behavior
 *   between STATIC and SHARED builds and makes bug-tracking easier.
 * \li or use
 *   \l {http://doc.qt.io/qt-5/qcoreapplication.html#Q_COREAPP_STARTUP_FUNCTION}
 *   {Q_COREAPP_STARTUP_FUNCTION} to call this function on SHARED
 *   libraries automatically when QCoreApplication starts (works
 *   only for SHARED libraries, and not for STATIC libraries, and
 *   and \l {https://stackoverflow.com/questions/43333151} { there
 *   seems to be no workaround}) and request the library user to
 *   call this function manually
 *   only on STATIC libraries.
 * \li or make this function private and
 *   use the solution that \l {https://stackoverflow.com/a/1420261}
 *   {instantiates a special class as global variable in every translation
 *   unit}, and this class makes sure that the initialization
 *   happens also for static libraries. But wouldn’t this mean that
 *   this function is called at program
 *   startup, while still no QApplication object is available, and
 *   therefore our
 *   function will crash? And anyway, when the QApplication object is
 *   destroyed and re-created, this pattern will not help. (Might
 *   https://doc.qt.io/qt-5/qglobalstatic.html#Q_GLOBAL_STATIC help?) See
 *   also https://isocpp.org/wiki/faq/ctors#static-init-order for useful
 *   information about static initialization.
 * \li or make this function private and call it
 *   \l {https://github.com/KDE/kitinerary/commit/72326ed}
 *   {in our library code whenever our code is about to do
 *   something that requires previous initialization}. To not call it
 *   too often or call it within functions that are executed often, it
 *   might be better to call it in the constructor of \e {all} our classes
 *   that need previous initialization. This approach would be more comfortable
 *   for the library user. The disadvantage is that we have to pay attention
 *   to never use \c {tr()} without calling the initialization first, and
 *   forgetting about this might introduce subtile and difficult-to-discover
 *   bugs.
 * \endlist
 *
 * We opt for calling this function
 * in our library code whenever our code is about to do
 * something that requires previous initialization.
 * Requiering the API user to do this manually would not be comfortable for the
 * API user, and the global-variable solution seems too dificult in maintaince.
 */
void initializeTranslation(QCoreApplication *instance, std::optional<QStringList> newUiLanguages)
{
    // Mutex protection
    static QMutex mutex;
    QMutexLocker<QMutex> mutexLocker(&mutex);

    // Check of pre-conditions
    // The mutex lowers the risk when using QCoreApplication::instance()
    // and QThread::currentThread(), which are not explicitly documented
    // as thread-safe.
    if (instance == nullptr) {
        qCWarning(logging) //
            << __func__ //
            << "must not be called without a QCoreApplication object.";
    }
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        qCWarning(logging) //
            << __func__ //
            << "must not be called by any other thread "
               "except the QCoreApplication thread.";
    }

    // Static variables
    static QTranslator translator;
    // The last UI language list that was loaded:
    static std::optional<QStringList> translatorUiLanguages;
    // A guarded pointer to the QCoreApplication object for which
    // the translation is initialized. In the (strange) use case
    // that a library user deletes his QCoreApplication (and maybe
    // create a new one), this guarded pointer is set to nullptr.
    // We provide support for this use case because Q_COREAPP_STARTUP_FUNCTION
    // also does, and we want to provide a full-featured alternative
    // to Q_COREAPP_STARTUP_FUNCTION.
    static QPointer<QCoreApplication> instanceWhereTranslationIsInstalled;

    // Actual function implementation…

    if (!newUiLanguages.has_value()) {
        if (translatorUiLanguages.has_value()) {
            newUiLanguages = translatorUiLanguages;
        } else {
            newUiLanguages = //
                QLocale().uiLanguages() + QLocale::system().uiLanguages();
        }
    }

    // NOTE Currently, this library does not use any plural forms in the
    // original English user-visible strings of the form "%1 color(s)".
    // If it becomes necessary to have these strings later, we have to
    // adapt this function: It has to install unconditionally first a
    // QTranslator for English, which resolves to "1 color" or "2 colors"
    // and so on. Then, a further QTranslator is installed for the target
    // language (but only if the target language is not English?). A
    // QTranslator that is installed later has higher priority. This makes
    // sure that, if a string is not translated to the target language,
    // the user does not see something like "1 color(s)" in the English
    // fallback, but instead "1 color".

    // QTranslator::load() will generate a QEvent::LanguageChange event
    // even if it loads the same translation file that was loaded anyway.
    // To avoid unnecessary events, we check if the new locale is really
    // different from the old locale: only than, we try to load the new
    // locale.
    //
    // Still, this will generate unnecessary events if in the previous call
    // of this function, we had tried to load a non-existing translation, so
    // that the QTranslator has an empty file path now. If we try to load now
    // another non-existing translation, we get an unnecessary event. But
    // to try to filter this out would be overkill…
    if (translatorUiLanguages != newUiLanguages) {
        // Resources can be loaded, and they can also be unloaded. Therefore,
        // here we make sure that our resources are actually currently loaded.
        // It is safe to call this function various times, and the overhead
        // should not be big.
        initializeLibraryResources();

        if (newUiLanguages.value().size() <= 0) {
            // QTranslator::load() will always delete the currently loaded
            // translation. After that, it will try to load the new one.
            // With this trick, we can delete the existing translation:
            Q_UNUSED( // We expect load() to fail, so we discard return value.
                translator.load(
                    // Filename of the binary translation file (.qm):
                    QStringLiteral("nonexistingfilename"),
                    // Directory within which filename is searched
                    // if filename is a relative path:
                    QStringLiteral(":/PerceptualColor/i18n")));
        } else {
            bool loaded = false;
            int i = 0;
            // NOTE We will load the first translation among the translation
            // list for which we can find an actual translation file (qm file).
            // Example: The list is "fr", "es", "de". The qm file for "fr" does
            // not exist, but the "qm" filed for "es" and "de" exist. Only the
            // "es" translation is loaded. If a specific string is missing in
            // "es", but exists in "de", than the system will nevertheless
            // fallback to the original source code language ("en"). Of course,
            // it would be better to fallback to "de", but this would require
            // to load various QTranslator and this might be a overkill. While
            // KDE’s internationalization library explicitly supports this use
            // case, Qt doesn’t. And we do not have too many strings to
            // translate anyway. If we find out later that we have many
            // incomplete translations, we can still implement this feature…
            while (!loaded && i < newUiLanguages.value().size()) {
                loaded = translator.load(
                    // The locale. From this locale are generated BCP47 codes.
                    // Various versions (upper-case and lower-case) are tried
                    // to load. If for more specific codes like "en-US" it
                    // does not succeed, than less specific variants like
                    // "en" are also tried.
                    QLocale(newUiLanguages.value().value(i)),
                    // First part of the filename
                    QStringLiteral("localization"),
                    // Separator after the first part of the filename
                    // (Intentionally NOT "_" or "-" to avoid confusion
                    // with the separators in BCP47 codes
                    QStringLiteral("."),
                    // Directory within which filename is searched
                    // if filename is a relative path:
                    QStringLiteral(":/PerceptualColor/i18n"));
                ++i;
            }
        }
        translatorUiLanguages = newUiLanguages;
    }

    // Make sure that the translator is installed into the current
    // QCoreApplication instance. The library user cannot uninstall it
    // because this is only possible when you have a pointer to the
    // QTranslator object, but the pointer is kept a private information
    // of this function. So we can be confident that, if once installed
    // on a QCoreApplication object, the QTranslation object will stay
    // available. However, the library user could delete the existing
    // QCoreApplication object and create a new one. In this case,
    // our guarded pointer instanceWhereTranslationIsInstalled will
    // be set to nullptr, so we can detect this case:
    if (instanceWhereTranslationIsInstalled != instance) {
        if (instance->installTranslator(&translator)) {
            instanceWhereTranslationIsInstalled = instance;
        } else {
            instanceWhereTranslationIsInstalled = nullptr;
        }
    }
}

} // namespace PerceptualColor
