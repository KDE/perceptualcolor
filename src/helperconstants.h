// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_HELPERCONSTANTS_H
#define PERCEPTUALCOLOR_HELPERCONSTANTS_H

#include <qglobal.h>
#include <qstring.h>
#include <qstringliteral.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \brief Precision for gamut boundary search
 *
 * We have to search sometimes for the gamut boundary. This value defines
 * the precision of the search:  Smaller values mean better precision and
 * slower processing. */
inline constexpr qreal gamutPrecisionCielab = 0.001;

/*!
 *
 *
 * \brief Precision for gamut boundary search
 *
 * We have to search sometimes for the gamut boundary. This value defines
 * the precision of the search:  Smaller values mean better precision and
 * slower processing. */
inline constexpr qreal gamutPrecisionOklab = gamutPrecisionCielab / 100;

/*!
 *
 *
 * \brief The overlap is a recommended tolerance value, measured in physical
 * pixels.
 *
 * It can be used during the painting process to paint “a little bit more
 * surface than requested”, just to be sure no pixel is missing. We
 * choose \c {2} because this value might also be used for radius
 * values, and at 45°, going from one edge of a pixel to the opposite edge
 * yet has a distance of √2 ≈ 1,41, which we round up to \c {2} just
 * to be sure. */
inline constexpr int overlap = 2;

// This value is somewhat arbitrary…
/*!
 *
 *
 * \brief Proposed scale factor for gradients
 *
 * Widgets provide a \c {minimumSizeHint} and a \c {sizeHint}.
 * This value provides a scale factor that is multiplied with
 * \c {minimumSizeHint} to get an appropriate \c {sizeHint}.
 * This scale factor is meant for gradient-based widgets. */
inline constexpr qreal scaleFromMinumumSizeHintToSizeHint = 1.2;

/*!
 *
 *
 * \brief Amount of single step for alpha.
 *
 * Measured for an alpha range from 0 (transparent) to 1 (opaque).
 *
 * The smaller of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * Use \l pageStepFactor to calculate corresponding page step (the
 * larger of two natural steps).
 */
inline constexpr qreal singleStepAlpha = 0.01;

/*!
 *
 *
 * \brief Amount of single step for hue.
 *
 * Measured in degree.
 *
 * The smaller of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * Use \l pageStepFactor to calculate corresponding page step (the
 * larger of two natural steps).
 */
inline constexpr int singleStepHue = 360 / 100;

/*!
 *
 *
 * \brief Factor for converting from single step to page step.
 *
 * The single step is the smaller and the page step is the larger
 * of two natural steps that a widget provides and
 * typically corresponds to the user pressing a key or using the mouse
 * wheel: The value will be incremented/decremented by the amount of this
 * value.
 *
 * Multiply the single step by this factor to get the page step.
 *
 * The value is 10. This behavior
 * corresponds to QAbstractSlider, who’s page step is also 10 times bigger than
 * its single step.
 *
 * \sa LchValues::singleStepLabc
 * \sa singleStepHue
 * \sa singleStepAlpha
 */
inline constexpr quint8 pageStepFactor = 10;

/*!
 *
 *
 * \brief Invisible marker for rich text
 *
 * Some parts of Qt accept both, plain text and rich text, within the same
 * property. Example: \c {QToolTip} uses \c {Qt::mightBeRichText()}
 * to decide if a text is treated as rich text or as plain text. But
 * \c {Qt::mightBeRichText()} is only a raw guess. This situation
 * is not comfortable: You never really know in advance if text will be
 * treated as rich text or as plain text.
 *
 * This function provides a solution. It provides a rich text marker. If
 * your text starts with this marker, it will always be treated
 * as rich text. The marker itself will not be visible in the rendered
 * rich text.
 *
 * Usage example:
 * \snippet testhelperconstants.cpp richTextMarkerExample
 */
inline constexpr QStringView richTextMarker{u"<a/>"};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_HELPERCONSTANTS_H
