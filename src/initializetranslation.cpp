// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "initializetranslation.h"

#include <qcoreapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlocale.h>
#include <qmutex.h>
#include <qpointer.h>
#include <qstring.h>
#include <qstringlist.h>
#include <qstringliteral.h>
#include <qthread.h>
#include <qtranslator.h>

/** @internal @file
 *
 * Provides the @ref PerceptualColor::initializeTranslation() function. */

/** @internal
 *
 * @brief Initializes the resources of the <tt>Qt resource system</tt> that
 * come with this library.
 *
 * It is safe to call this function multiple times: If the resources are
 * yet initialized, nothing happens.
 *
 * @post The resources are initialized.
 *
 * @note This is a helper function for
 * @ref PerceptualColor::initializeTranslation() which is the only user of
 * the resources. <tt>Q_INIT_RESOURCE</tt> may not be used within a namespace.
 * Therefore exists this function which is not part of any namespace. To avoid
 * pollution of the global namespace, it is declared <tt>static</tt> which
 * gives here internal linkage, so it is not visible outside of this
 * compilation unit. */
static void helperInitializeLibraryResources()
{
    // Resource initialization is necessary on some systems for static
    // libraries, but also recommended for dynamic libraries. For details see
    // https://doc.qt.io/qt-6/resources.html#using-resources-in-a-library
    // The argument of this macro is the name of the .qrc file (without
    // the .qrc suffix) just as in the corresponding CMakeLists.txt.
    // This macro relies on qRegisterResourceData() which checks if
    // the resource is yet loaded: If so, it is not loaded again.
    Q_INIT_RESOURCE(resourcelist);
}

namespace PerceptualColor
{

/** @internal
 *
 * @brief Set the translation for the whole library.
 *
 * After calling this function, all objects of this library that are created
 * from now on are translated according to translation that was set.
 *
 * Objects that were yet existing when calling are <em>not always</em>
 * automatically updated: When calling this function, Qt sends
 * a <tt>QEvent::LanguageChange</tt> event only to top-level widgets,
 * and these will get updated then. You can send the event yourself
 * to non-top-level widgets to update those widgets also. Note that
 * also @ref RgbColorSpaceFactory generates objects that might have
 * localized properties; these objects do not support translation
 * updates.
 *
 * If you create objects that use translations <em>before</em> a translation
 * has been set explicitly, than automatically an environment-dependant
 * translation is loaded.
 *
 * It is safe to call this function multiple times.
 *
 * @pre There exists exactly <em>one</em> instance of <tt>QCoreApplication</tt>
 * to which the parameter points. This function is called from the same thread
 * in which the <tt>QCoreApplication</tt> instance lives.
 *
 * @param instance A pointer to the <tt>QCoreApplication</tt> instance for
 *        which the initialization will be done.
 *
 * @param newUiLanguages List of translations, ordered by priority, most
 *        important ones first, like in <tt>QLocale::uiLanguages()</tt>. If
 *        <tt>std::optional::has_value()</tt> than the translation is
 *        initialized for this value, and previously loaded translations are
 *        removed. If <tt>std::optional::has_value()</tt> is <tt>false</tt>,
 *        than it depends: If no translation has been initialized so far, than
 *        the translation is initialized to an environment-dependent default
 *        value; otherwise there last initialization is simply repeated.
 *
 * @post The translation is initialized, even if a previous initialization
 * had been destroyed by deleting the previous QCoreApplication object. */
void initializeTranslation(QCoreApplication *instance, std::optional<QStringList> newUiLanguages)
{
    // Mutex protection
    static QMutex mutex;
#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
    QMutexLocker<QMutex> mutexLocker(&mutex);
#else
    QMutexLocker mutexLocker(&mutex);
#endif

    // Check of pre-conditions
    // The mutex lowers the risk when using QCoreApplication::instance()
    // and QThread::currentThread(), which are not explicitly documented
    // as thread-safe.
    if (instance == nullptr) {
        qWarning() //
            << __func__ //
            << "must not be called without a QCoreApplication object.";
        throw 0;
    }
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        qWarning() //
            << __func__ //
            << "must not be called by any other thread "
               "except the QCoreApplication thread.";
        throw 0;
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
        helperInitializeLibraryResources();

        if (newUiLanguages.value().count() <= 0) {
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
            while (!loaded && i < newUiLanguages.value().count()) {
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
