// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_PERCEPTUALCOLORNAMESPACE_H
#define PERCEPTUALCOLOR_PERCEPTUALCOLORNAMESPACE_H

// For Q_NAMESPACE and Q_ENUM_NS, including just qtmetamacros.h is not enough.
// The build would fail it qobject.h is not included also. IWYU does not detect
// this dependency and issues a false positive.
#include <qobject.h> // IWYU pragma: keep
#include <qtmetamacros.h>

// Declares the namespace \l PerceptualColor with the Q_NAMESPACE
// macro and centralizes all enums that belong directly to this namespace
// (i.e. enums not defined inside a class).
//
// This setup is required due to the specific behavior of Q_NAMESPACE.
// The macro Q_ENUM can only be used for enums that are members of a class.
// For enums defined directly in a namespace, Q_ENUM_NS must be used instead.
// However, Q_ENUM_NS requires the presence of Q_NAMESPACE, and Q_NAMESPACE
// may only be declared once within the entire program or library.
//
// While C++ allows a namespace to be opened and closed multiple times,
// Q_ENUM_NS must reside in the same namespace block as the single Q_NAMESPACE
// declaration. Consequently, Q_NAMESPACE and all Q_ENUM_NS declarations
// must be grouped together in one dedicated header file. This header must
// be protected by include guards to prevent multiple inclusion and can then
// be safely included in other headers or source files.

/*!
 * \namespace PerceptualColor
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Namespace that
 * encapsulates all \l {ModulePublicApi}{public symbols} and
 * all private symbols
 * of this library.
 *
 * This namespace is declared once with Q_NAMESPACE
 * to enable Qt’s meta‑object system to recognize enums defined directly
 * in the namespace.
 *
 * \section1 Other public symbols
 *
 * \generatelist GroupPublicSymbols
 *
 * \section1 General public documentation
 *
 * \generatelist GroupPublicPages
 *
 * \section1 General private documentation
 *
 * \generatelist GroupPrivatePages
 */
namespace PerceptualColor
{

Q_NAMESPACE

/*!
 *
 *
 * \brief Identifiers for color models.
 *
 * \note Maybe
 * \l {https://doc-snapshots.qt.io/qt6-dev/qcolorspace.html#ColorModel-enum}
 * {enum QColorSpace::NamedColorSpace} might also be useful in
 * the future.
 *
 * \value SRgb_1
 * The absolute sRGB color space, which by
 * definition always and exclusively uses a D65 illuminant.
 * R: [0, 1]. G: [0, 1]. B: [0, 1].
 *
 * \value LinearSRgb_1
 * The absolute Linear sRGB color space, which by
 * definition al ways and exclusively uses a D65 illuminant.
 * R: [0, 1]. G: [0, 1]. B: [0, 1].
 *
 * \value CielabD50
 * The absolute Cielab color space using a D50
 * illuminant.
 * Lightness: [0, 100]. a: unbound. b: unbound.
 *
 * \value CielchD50
 * The absolute Cielch color space using a D50
 * illuminant.
 * Lightness: [0, 100]. Chroma: unbound. Hue: [0, 360[.
 *
 * \value Hsl_360_1_1
 * Any color space using the HSL color model, which
 * is a transformation of the RGB color model. Color values only have a
 * well-defined absolute meaning when associated with a corresponding
 * RGB color profile.
 * Hue: [0, 360[. Saturation: [0, 1]. Lightness: [0, 1].
 *
 * \value Hwb_360_1_1
 * Any color space using the HSL color model, which
 * is a transformation of the HWB color model. Color values only have a
 * well-defined absolute meaning when associated with a corresponding
 * RGB color profile.
 * Hue: [0, 360[. Whiteness: [0, 1]. Blackness: [0, 1].
 *
 * \value Invalid
 * Represents invalid data.
 *
 * \value OklabD65
 * The absolute Oklab color space, which by
 * definition always and exclusively uses a D65 illuminant.
 * Lightness: [0, 1]. a: unbound. b: unbound.
 *
 * \value OklchD65
 * The absolute Oklch color space, which by
 * definition always and exclusively uses a D65 illuminant.
 * Lightness: [0, 1]. Chroma: unbound. Hue: [0, 360[.
 *
 * \value Rgb_1
 * Any color space using the RGB color model. Color
 * values only have a well-defined absolute meaning when associated with a
 * corresponding RGB color profile.
 * R: [0, 1]. G: [0, 1]. B: [0, 1].
 *
 * \value XyzD50_1
 * The absolute XYZ color space assuming chromatic
 * adaption for the D50 illuminant.
 * X: unbound. Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0. Z: unbound.
 *
 * \value XyzD65_1
 * The absolute XYZ color space assuming chromatic
 * adaption for the D65 illuminant.
 * X: unbound. Y: [0, 1]. Diffuse white has a luminance (Y) of 1.0.  Z: unbound.
 */
enum class ColorModel {
    SRgb_1,
    LinearSRgb_1,
    CielabD50,
    CielchD50,
    Hsl_360_1_1,
    Hwb_360_1_1,
    Invalid,
    OklabD65,
    OklchD65,
    Rgb_1,
    XyzD50_1,
    XyzD65_1
};
Q_ENUM_NS(ColorModel)

/*!
 *
 *
 * \brief Enum class representing possible color spaces in the Lch color
 * models.
 *
 * \value Oklch
 * The Oklch color space, which uses by definition
 * always a D65 whitepoint.
 *
 * \value CielchD50
 * The CielchD50 color space, assuming a chromatic
 * adaption to the D50 whitepoint.
 */
enum class LchSpace {
    Oklch,
    CielchD50
};
Q_ENUM_NS(LchSpace)

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_PERCEPTUALCOLORNAMESPACE_H
