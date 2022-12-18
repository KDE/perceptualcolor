// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "perceptualcolor-0/settranslation.h"

#include "initializetranslation.h"
#include <optional>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qthread.h>

/** @file
 *
 * Provides the @ref PerceptualColor::setTranslation() function. */

namespace PerceptualColor
{

/** @brief Set the translation for the whole library.
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
 * You might call this function again after a change of
 * <tt>QLocale()</tt> to change the translation. Also, call this
 * function again after destroying the <tt>QCoreApplication</tt>
 * object and creating a new one.
 *
 * It is safe to call this function multiple times.
 *
 * @pre There exists exactly <em>one</em> instance of <tt>QCoreApplication</tt>
 * to which the parameter points. This function is called from the same thread
 * in which the <tt>QCoreApplication</tt> instance lives.
 *
 * @param instance A pointer to the <tt>QCoreApplication</tt> instance for
 * which the initialization will be done.
 * @param newUiLanguages List of translations, ordered by priority,
 * most important ones first, like in <tt>QLocale::uiLanguages()</tt>. */
void setTranslation(QCoreApplication *instance, const QStringList &newUiLanguages)
{
    // Check of pre-conditions
    // The mutex lowers the risk when using QCoreApplication::instance()
    // and QThread::currentThread(), which are not explicitly documented
    // as thread-safe.
    if (instance == nullptr) {
        qWarning() //
            << __func__ //
            << "must not be called without a QCoreApplication object.";
        return;
    }
    if (QThread::currentThread() != QCoreApplication::instance()->thread()) {
        qWarning() //
            << __func__ //
            << "must not be called by any other thread "
               "except the QCoreApplication thread.";
        return;
    }

    initializeTranslation(instance, std::optional<QStringList>(newUiLanguages));
}

} // namespace PerceptualColor
