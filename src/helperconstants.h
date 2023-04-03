// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERCONSTANTS_H
#define HELPERCONSTANTS_H

#include <qglobal.h>
#include <qstring.h>

/** @internal
 *
 * @file
 *
 * Provides global constant values. */

namespace PerceptualColor
{

/** @internal
 *
 * @brief Precision for gamut boundary search
 *
 * We have to search sometimes for the gamut boundary. This value defines
 * the precision of the search:  Smaller values mean better precision and
 * slower processing. */
constexpr qreal gamutPrecisionCielab = 0.001;

/** @internal
 *
 * @brief Precision for gamut boundary search
 *
 * We have to search sometimes for the gamut boundary. This value defines
 * the precision of the search:  Smaller values mean better precision and
 * slower processing. */
constexpr qreal gamutPrecisionOklab = gamutPrecisionCielab / 100;

/** @internal
 *
 * @brief The overlap is a recommended tolerance value, measured in physical
 * pixels.
 *
 * It can be used during the painting process to paint “a little bit more
 * surface than requested”, just to be sure no pixel is missing. We
 * choose <tt>2</tt> because this value might also be used for radius
 * values, and at 45°, going from one edge of a pixel to the opposite edge
 * yet has a distance of √2 ≈ 1,41, which we round up to <tt>2</tt> just
 * to be sure. */
constexpr int overlap = 2;

/** @internal
 *
 * @brief Proposed scale factor for gradients
 *
 * Widgets provide a <tt>minimumSizeHint</tt> and a <tt>sizeHint</tt>.
 * This value provides a scale factor that is multiplied with
 * <tt>minimumSizeHint</tt> to get an appropriate <tt>sizeHint</tt>.
 * This scale factor is meant for gradient-based widgets. */
// This value is somewhat arbitrary…
constexpr qreal scaleFromMinumumSizeHintToSizeHint = 1.2;

/** @internal
 *
 * @brief Amount of single step for alpha.
 *
 * Measured for an alpha range from 0 (transparent) to 1 (opaque).
 *
 * The smaller of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * @sa @ref pageStepAlpha */
constexpr qreal singleStepAlpha = 0.01;

/** @internal
 *
 * @brief Amount of single step for chroma.
 *
 * Measured in LCH chroma units.
 *
 * The smaller of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * @sa @ref pageStepChroma */
constexpr int singleStepChroma = 1;

/** @internal
 *
 * @brief Amount of single step for hue.
 *
 * Measured in degree.
 *
 * The smaller of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * @sa @ref pageStepHue
 *
 * @todo What would be a good value for this? Its effect depends on
 * chroma: On higher chroma, the same step in hue means a bigger visual
 * color difference. We could even calculate that, but it does not seem to
 * be very intuitive if the reaction on mouse wheel events are different
 * depending on chroma - that would not be easy to understand for the
 * user. And it might be better that the user this way also notices
 * intuitively that hue changes are not linear across chroma. Anyway:
 * What would be a sensible default step? */
constexpr int singleStepHue = 360 / 100;

/** @internal
 *
 * @brief Amount of single step for lightness.
 *
 * Measured in LCH lightness units.
 *
 * The smaller of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * @sa @ref pageStepLightness */
constexpr int singleStepLightness = 1;

/** @internal
 *
 * @brief Amount of single step for Ok lightness, a, b, chroma (but
 * <em>not</em> hue).
 *
 * Measured in Ok units.
 *
 * The smaller of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value. */
constexpr double singleStepOklabc = 0.01;

/** @internal
 *
 * @brief Amount of page step for alpha.
 *
 * Measured for an alpha range from 0 (transparent) to 1 (opaque).
 *
 * The larger of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * The value is 10 times @ref singleStepChroma. This behavior
 * corresponds to QAbstractSlider, who’s page step is also 10 times bigger than
 * its single step. */
constexpr qreal pageStepAlpha = 10 * singleStepAlpha;

/** @internal
 *
 * @brief Amount of page step for chroma.
 *
 * Measured in LCH chroma units.
 *
 * The larger of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * The value is 10 times @ref singleStepChroma. This behavior
 * corresponds to QAbstractSlider, who’s page step is also 10 times bigger than
 * its single step. */
constexpr int pageStepChroma = 10 * singleStepChroma;

/** @internal
 *
 * @brief Amount of page step for hue.
 *
 * Measured in degree.
 *
 * The larger of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * The value is 10 times @ref singleStepHue. This behavior
 * corresponds to QAbstractSlider, who’s page step is also 10 times bigger than
 * its single step. */
constexpr int pageStepHue = 10 * singleStepHue;

/** @internal
 *
 * @brief Amount of page step for lightness.
 *
 * Measured in LCH lightness units.
 *
 * The larger of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * The value is 10 times @ref singleStepLightness. This behavior
 * corresponds to QAbstractSlider, who’s page step is also 10 times bigger than
 * its single step. */
constexpr int pageStepLightness = 10 * singleStepLightness;

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
 * @snippet testhelperconstants.cpp richTextMarkerExample */
inline const QString richTextMarker = QStringLiteral(u"<a/>");

} // namespace PerceptualColor

#endif // HELPERCONSTANTS_H
