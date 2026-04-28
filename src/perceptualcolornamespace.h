// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_PERCEPTUALCOLORNAMESPACE_H
#define PERCEPTUALCOLOR_PERCEPTUALCOLORNAMESPACE_H

#include "genericcolor.h"
#include "mat3.h"
#include <array>
#include <lcms2.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qtmetamacros.h>
#include <type_traits>

/**
 * @internal
 *
 * @file
 *
 * Declares the namespace @ref PerceptualColor with the Q_NAMESPACE
 * macro and centralizes all enums that belong directly to this namespace
 * (i.e. enums not defined inside a class).
 *
 * This setup is required due to the specific behavior of Q_NAMESPACE.
 * The macro Q_ENUM can only be used for enums that are members of a class.
 * For enums defined directly in a namespace, Q_ENUM_NS must be used instead.
 * However, Q_ENUM_NS requires the presence of Q_NAMESPACE, and Q_NAMESPACE
 * may only be declared once within the entire program or library.
 *
 * While C++ allows a namespace to be opened and closed multiple times,
 * Q_ENUM_NS must reside in the same namespace block as the single Q_NAMESPACE
 * declaration. Consequently, Q_NAMESPACE and all Q_ENUM_NS declarations
 * must be grouped together in one dedicated header file. This header must
 * be protected by include guards to prevent multiple inclusion and can then
 * be safely included in other headers or source files.
 */

/**
 * @brief Namespace for all library symbols.
 *
 * The @ref PerceptualColor namespace encapsulates every public
 * symbol provided by this library. It is declared once with Q_NAMESPACE
 * to enable Qt’s meta‑object system to recognize enums defined directly
 * in the namespace.
 */
namespace PerceptualColor
{

Q_NAMESPACE

/**
 * @internal
 *
 * @brief Identifiers for color models.
 *
 * @internal
 *
 * @note Maybe
 * <a href="https://doc-snapshots.qt.io/qt6-dev/qcolorspace.html#ColorModel-enum">
 * <tt>enum QColorSpace::NamedColorSpace</tt></a> might also be useful in
 * the future.
 *
 * @todo NICETOHAVE Write unit tests for all functions and function templates
 * in this header.
 */
enum class ColorModel {
    SRgb_1, /**< @internal The absolute sRGB color space, which by
    definition always and exclusively uses a D65 illuminant.

    R: [0, 1].<br/>
    G: [0, 1].<br/>
    B: [0, 1]. */
    LinearSRgb_1, /**< @internal The absolute Linear sRGB color space, which by
    definition always and exclusively uses a D65 illuminant.

    R: [0, 1].<br/>
    G: [0, 1].<br/>
    B: [0, 1]. */
    CielabD50, /**< @internal The absolute Cielab color space using a D50
    illuminant.

    Lightness: [0, 100].<br/>
    a: unbound.<br/>
    b: unbound. */
    CielchD50, /**< @internal The absolute Cielch color space using a D50
    illuminant.

    Lightness: [0, 100].<br/>
    Chroma: unbound.<br/>
    Hue: [0, 360[. */
    Hsl_360_1_1, /**< @internal Any color space using the HSL color model, which
    is a transformation of the RGB color model. Color values only have a
    well-defined absolute meaning when associated with a corresponding
    RGB color profile.

    Hue: [0, 360[.<br/>
    Saturation: [0, 1].<br/>
    Lightness: [0, 1]. */
    Hwb_360_1_1, /**< @internal Any color space using the HSL color model, which
    is a transformation of the HWB color model. Color values only have a
    well-defined absolute meaning when associated with a corresponding
    RGB color profile.

    Hue: [0, 360[.<br/>
    Whiteness: [0, 1].<br/>
    Blackness: [0, 1]. */
    Invalid, /**< @internal Represents invalid data. */
    OklabD65, /**< @internal The absolute Oklab color space, which by
    definition always and exclusively uses a D65 illuminant.

    Lightness: [0, 1].<br/>
    a: unbound.<br/>
    b: unbound. */
    OklchD65, /**< @internal The absolute Oklch color space, which by
    definition always and exclusively uses a D65 illuminant.

    Lightness: [0, 1].<br/>
    Chroma: unbound.<br/>
    Hue: [0, 360[. */
    Rgb_1, /**< @internal Any color space using the RGB color model. Color
    values only have a well-defined absolute meaning when associated with a
    corresponding RGB color profile.

    R: [0, 1].<br/>
    G: [0, 1].<br/>
    B: [0, 1]. */
    XyzD50_1, /**< @internal The absolute XYZ color space assuming chromatic
    adaption for the D50 illuminant.

    X: unbound.<br/>
    Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0<br/>
    Z: unbound. */
    XyzD65_1 /**< @internal The absolute XYZ color space assuming chromatic
    adaption for the D65 illuminant.

    X: unbound.<br/>
    Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0<br/>
    Z: unbound. */
};
Q_ENUM_NS(ColorModel)

/**
 * @internal
 *
 * @brief Enum class representing possible color spaces in the Lch color
 * models.
 */
enum class LchSpace {
    Oklch, /**< @internal The Oklch color space, which uses by definition
    always a D65 whitepoint. */
    CielchD50 /**< @internal The CielchD50 color space, assuming a chromatic
    adaption to the D50 whitepoint. */
};
Q_ENUM_NS(LchSpace)

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_PERCEPTUALCOLORNAMESPACE_H
