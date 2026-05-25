// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own header
#include "helperconversion.h"

#include "helper.h"
#include <qcolor.h>
#include <qimage.h>
#include <qpoint.h>
#include <qrect.h>
#include <qrgb.h>
#include <qrunnable.h>
#include <qsemaphore.h>
#include <qthreadpool.h>
#include <type_traits>

namespace PerceptualColor
{

/**
 * @brief Alternative to <tt>QColor::toRgb()</tt> with full precision.
 *
 * Unlike <tt>QColor::toRgb()</tt>, which converts a <tt>QColor</tt> to
 * <tt>QColor::Spec::Rgb</tt> and rounds each channel to 8‑bit integers in the
 * range [0..255], this function calculates with full precision.
 * Colors with <tt>QColor::Spec::ExtendedRgb</tt> are clipped to
 * <tt>QColor::Spec::Rgb</tt>.
 *
 * @param color The color to convert.
 * @return A QColor in  <tt>QColor::Spec::Rgb</tt> space.
 */
QColor toRgbExact(const QColor &color)
{
    if (!color.isValid() || color.spec() == QColor::Spec::Rgb) {
        return color;
    }

    return QColor::fromRgbF( //
        qBound<float>(0, color.redF(), 1),
        qBound<float>(0, color.greenF(), 1),
        qBound<float>(0, color.blueF(), 1),
        qBound<float>(0, color.alphaF(), 1));
}

} // namespace PerceptualColor
