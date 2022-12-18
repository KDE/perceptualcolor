// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// Own header
#include "helperconstants.h"

#include <qstringliteral.h>

namespace PerceptualColor
{
/** @internal
 *
 * @brief Invisible marker for rich text
 *
 * Some parts of Qt accept both, plain text and rich text, within the same
 * property. Example: <tt>QToolTip</tt> uses <tt>Qt::mightBeRichText()</tt>
 * to decide if a text is treated as rich text or as plain text. But
 * <tt>Qt::mightBeRichText()</tt> is only a raw guess. This situation
 * is not comfortable: You never really know in advance if text will be
 * treated as rich text or as plain text.
 *
 * This function provides a solution. It provides a rich text marker. If
 * your text starts with this marker, it will always be treated
 * as rich text. The marker itself will not be visible in the rendered
 * rich text.
 *
 * Usage example:
 * @snippet testhelperconstants.cpp richTextMarkerExample
 *
 * @returns Invisible marker for rich text */
QString richTextMarker()
{
    return QStringLiteral(u"<a/>");
}

} // namespace PerceptualColor
