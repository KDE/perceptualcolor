// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "languagechangeeventfilter.h"

#include <qcoreevent.h>

namespace PerceptualColor
{

/** @brief Constructor
 *
 * @param parent The parent of the widget, if any */
LanguageChangeEventFilter::LanguageChangeEventFilter(QObject *parent)
    : QObject(parent)
{
}

/** @brief Destructor */
LanguageChangeEventFilter::~LanguageChangeEventFilter()
{
}

/** @brief Filters events if this object has been installed as an event filter
 * for the watched object.
 *
 * Reimplemented from base class.
 *
 * @param obj Pointer to the object for which we are filtering events
 * @param event The event that we are filtering
 *
 * @returns <tt>true</tt> (which means “discard”) for
 * <tt>QEvent::LanguageChange</tt> events. <tt>false</tt> otherwise. */
bool LanguageChangeEventFilter::eventFilter(QObject *obj, QEvent *event)
{
    Q_UNUSED(obj)
    if (event->type() == QEvent::LanguageChange) {
        return true;
    }
    return false;
}

} // namespace PerceptualColor
