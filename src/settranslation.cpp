// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "settranslation.h"

#include "initializetranslation.h"
#include "logging.h"
#include <optional>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlist.h>
#include <qloggingcategory.h>
#include <qthread.h>

namespace PerceptualColor
{

/*!
 * \relates settranslation.h
 *
 * \ingroup GroupPublicSymbols
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
 * You might call this function again after a change of
 * \c {QLocale()} to change the translation. Also, call this
 * function again after destroying the \c {QCoreApplication}
 * object and creating a new one.
 *
 * It is safe to call this function multiple times.
 *
 * \b Precondition: There exists exactly \e {one} instance
 * of \c {QCoreApplication}
 * to which the parameter points. This function is called from the same thread
 * in which the \c {QCoreApplication} instance lives.
 *
 * \a instance A pointer to the \c {QCoreApplication} instance for
 * which the initialization will be done.
 *
 * \a newUiLanguages List of translations, ordered by priority,
 * most important ones first, like in \c {QLocale::uiLanguages()}.
 */
void setTranslation(QCoreApplication *instance, const QStringList &newUiLanguages)
{
    // Check of pre-conditions
    // The mutex lowers the risk when using QCoreApplication::instance()
    // and QThread::currentThread(), which are not explicitly documented
    // as thread-safe.
    if (instance == nullptr) {
        qCWarning(logging) //
            << __func__ //
            << "must not be called without a QCoreApplication object.";
        return;
    }
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        qCWarning(logging) //
            << __func__ //
            << "must not be called by any other thread "
               "except the QCoreApplication thread.";
        return;
    }

    initializeTranslation(instance, std::optional<QStringList>(newUiLanguages));
}

} // namespace PerceptualColor
