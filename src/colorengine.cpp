// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "colorengine.h"
// Second, the private implementation.
#include "colorengine_p.h" // IWYU pragma: associated

#include "absolutecolor.h"
#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "helper.h"
#include "helperconstants.h"
#include "helperconversion.h"
#include "helperimage.h"
#include "helpermath.h"
#include "initializetranslation.h"
#include "iohandlerfactory.h"
#include <algorithm>
#include <limits>
#include <optional>
#include <qbytearray.h>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qlist.h>
#include <qlocale.h>
#include <qmath.h>
#include <qrgba64.h>
#include <qsharedpointer.h>
#include <qstringliteral.h>
#include <qtimezone.h>
#include <type_traits>
#include <utility>
#include <vector>

// Include the type “tm” as defined in the C standard (time.h), as LittleCMS
// expects, preventing IWYU < 0.19 to produce false-positives.
#include <time.h> // IWYU pragma: keep
// IWYU pragma: no_include <bits/types/struct_tm.h>

namespace PerceptualColor
{

/** @internal
 *
 * @brief Constructor
 *
 * @param parent Parent object
 *
 * @attention Creates an uninitialised object. You have to call
 * @ref ColorEnginePrivate::initialize() <em>successfully</em>
 * before actually use object. */
ColorEngine::ColorEngine(QObject *parent)
    : QObject(parent)
    , d_pointer(new ColorEnginePrivate(this))
{
}

/** @brief Create a color engine object with sRGB working gamut.
 *
 * This is build-in, no external ICC file is used.
 *
 * @pre This function must be called from the main thread.
 *
 * @returns A shared pointer to the newly created color engine object.
 *
 * @sa @ref createSrgb()
 *
 * @internal
 *
 * @note This function has to be called from the main thread because
 * <a href="https://doc.qt.io/qt-6/qobject.html#tr">it is not save to use
 * <tt>QObject::tr()</tt> while a new translation is loaded into
 * QCoreApplication</a>, which should happen within the main thread. Therefore,
 * if this function is also called within the main thread, we can use
 * QObject::tr() safely because there will be not be executed simultaneously
 * with loading a translation. */
QSharedPointer<PerceptualColor::ColorEngine> ColorEngine::createSrgb()
{
    // Create an invalid object:
    QSharedPointer<PerceptualColor::ColorEngine> result{new ColorEngine()};

    // Transform it into a valid object:
    cmsHPROFILE srgb = cmsCreate_sRGBProfile(); // Use build-in profile
    const bool success = result->d_pointer->initialize(srgb);
    cmsCloseProfile(srgb);

    if (!success) {
        // This should never fail. If it fails anyway, that’s a
        // programming error and we throw an exception.
        throw 0;
    }

    initializeTranslation(QCoreApplication::instance(),
                          // An empty std::optional means: If in initialization
                          // had been done yet, repeat this initialization.
                          // If not, do a new initialization now with default
                          // values.
                          std::optional<QStringList>());

    // Return:
    return result;
}

/** @brief Basic initialization.
 *
 * This function is meant to be called when constructing the object.
 *
 * @param rgbProfileHandle Handle for the RGB profile
 *
 * @pre rgbProfileHandle is valid.
 *
 * @returns <tt>true</tt> on success. <tt>false</tt> otherwise (for example
 * when it’s not an RGB profile but an CMYK profile). When <tt>false</tt>
 * is returned, the object is still in an undefined state; it cannot
 * be used, but only be destroyed. This should happen as soon as
 * possible to reduce memory usage.
 *
 * @note rgbProfileHandle is <em>not</em> deleted in this function.
 * Remember to delete it manually.
 *
 * @internal
 *
 * @todo SHOULDHAVE
 * LUT profiles should be detected and refused, as the actual diagram
 * results are currently bad. (LUT profiles for RGB are not common among
 * the usual standard profile files. But they might be more common among
 * individually calibrated monitors? -> The color.org reference sRGB profile
 * named sRGB_v4_ICC_preference.icc also uses them, and it works fine in
 * our library.)
 *
 * @todo SHOULDHAVE
 * This function creates a transforms from CIELabD50 to RGB. And if RGB
 * is sRGB, it is D65 (and other RGB gamuts are likely to be also D65).
 * However, CIELabD50 is D50! We do no whitepoint compensation manuelly, and
 * https://sourceforge.net/p/lcms/mailman/lcms-user/?viewmonth=202510 says
 * we do not need to when using perceptual or relative-colormetric rendering
 * intents. However, we use absolut rendering intent. Also, we have these
 * changes between D50 and D65 in a lot of other, different contexts. What
 * would be the general solution here?
 *
 * @todo SHOULDHAVE Decide wether to require specific rendering intents to be
 * present in the ICC file or not.
 *
 * @todo NICETOHAVE Add support the profiles containing a VCGT? Currently,
 * these profiles are rejected.
 */
bool ColorEnginePrivate::initialize(cmsHPROFILE rgbProfileHandle)
{
    constexpr auto renderingIntent = INTENT_ABSOLUTE_COLORIMETRIC;

    {
        // Create an ICC v4 profile object for the CielabD50 color space.
        cmsHPROFILE cielabD50ProfileHandle = cmsCreateLab4Profile(
            // nullptr means: Default white point (D50)
            nullptr);

        // Create an ICC profile object for the XYZD50 color space.
        cmsHPROFILE xyzD50ProfileHandle = cmsCreateXYZProfile();

        // Create the transforms.
        // We use the flag cmsFLAGS_NOCACHE which disables the 1-pixel-cache
        // which is normally used in the transforms. We do this because
        // transforms that use the 1-pixel-cache are not thread-safe. And
        // disabling it should not have negative impacts as we usually work
        // with gradients, so anyway it is not likely to have two consecutive
        // pixels with the same color, which is the only situation where the
        // 1-pixel-cache makes processing faster.
        constexpr auto flags = cmsFLAGS_NOCACHE;
        m_transformCielabD50ToRgbHandle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            cielabD50ProfileHandle, // input profile handle
            TYPE_Lab_DBL, // input buffer format
            rgbProfileHandle, // output profile handle
            TYPE_RGB_DBL, // output buffer format
            renderingIntent,
            flags);
        m_transformXyzD50ToRgbHandle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            xyzD50ProfileHandle, // input profile handle
            TYPE_XYZ_DBL, // input buffer format
            rgbProfileHandle, // output profile handle
            TYPE_RGB_DBL, // output buffer format
            renderingIntent,
            flags);
        m_transformCielabD50ToRgb16Handle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            cielabD50ProfileHandle, // input profile handle
            TYPE_Lab_DBL, // input buffer format
            rgbProfileHandle, // output profile handle
            TYPE_RGB_16, // output buffer format
            renderingIntent,
            flags);
        m_transformRgbToCielabD50Handle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            rgbProfileHandle, // input profile handle
            TYPE_RGB_DBL, // input buffer format
            cielabD50ProfileHandle, // output profile handle
            TYPE_Lab_DBL, // output buffer format
            renderingIntent,
            flags);
        m_transformRgbToXyzD50Handle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            rgbProfileHandle, // input profile handle
            TYPE_RGB_DBL, // input buffer format
            xyzD50ProfileHandle, // output profile handle
            TYPE_XYZ_DBL, // output buffer format
            renderingIntent,
            flags);
        // It is mandatory to close the profiles to prevent memory leaks:
        cmsCloseProfile(cielabD50ProfileHandle);
        cmsCloseProfile(xyzD50ProfileHandle);
    }

    // After having closed the profiles, we can now return
    // (if appropriate) without having memory leaks:
    if ((m_transformCielabD50ToRgbHandle == nullptr) //
        || (m_transformCielabD50ToRgb16Handle == nullptr) //
        || (m_transformRgbToCielabD50Handle == nullptr) //
        || (m_transformRgbToXyzD50Handle == nullptr) //
        || (m_transformXyzD50ToRgbHandle == nullptr) //
    ) {
        return false;
    }

    // Find blackpoint and whitepoint.
    // For CielabD50 make sure that: 0 <= blackpoint < whitepoint <= 100
    GenericColor candidate;
    candidate.second = 0;
    candidate.third = 0;
    candidate.first = 0;
    while (!q_pointer->isCielchD50InGamut(candidate)) {
        candidate.first += gamutPrecisionCielab;
        if (candidate.first >= 100) {
            return false;
        }
    }
    m_cielabD50BlackpointL = candidate.first;
    candidate.first = 100;
    while (!q_pointer->isCielchD50InGamut(candidate)) {
        candidate.first -= gamutPrecisionCielab;
        if (candidate.first <= m_cielabD50BlackpointL) {
            return false;
        }
    }
    m_cielabD50WhitepointL = candidate.first;
    // For Oklab make sure that: 0 <= blackbpoint < whitepoint <= 1
    candidate.first = 0;
    while (!q_pointer->isOklchInGamut(candidate)) {
        candidate.first += gamutPrecisionOklab;
        if (candidate.first >= 1) {
            return false;
        }
    }
    m_oklabBlackpointL = candidate.first;
    candidate.first = 1;
    while (!q_pointer->isOklchInGamut(candidate)) {
        candidate.first -= gamutPrecisionOklab;
        if (candidate.first <= m_oklabBlackpointL) {
            return false;
        }
    }
    m_oklabWhitepointL = candidate.first;

    // Now, calculate the properties who’s calculation depends on a fully
    // initialized object.
    initializeChromaticityBoundaries();

    return true;
}

/** @brief Destructor */
ColorEngine::~ColorEngine() noexcept
{
    ColorEnginePrivate::deleteTransform( //
        &d_pointer->m_transformCielabD50ToRgb16Handle);
    ColorEnginePrivate::deleteTransform( //
        &d_pointer->m_transformCielabD50ToRgbHandle);
    ColorEnginePrivate::deleteTransform( //
        &d_pointer->m_transformRgbToCielabD50Handle);
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
ColorEnginePrivate::ColorEnginePrivate(ColorEngine *backLink)
    : q_pointer(backLink)
{
}

/**
 * @brief Default destructor
 */
ColorEnginePrivate::~ColorEnginePrivate() noexcept = default;

/** @brief Convenience function for deleting LittleCMS transforms
 *
 * <tt>cmsDeleteTransform()</tt> is not comfortable. Calling it on a
 * <tt>nullptr</tt> crashes. If called on a valid handle, it does not
 * reset the handle to <tt>nullptr</tt>. Calling it again on the now
 * invalid handle crashes. This convenience function can be used instead
 * of <tt>cmsDeleteTransform()</tt>: It provides some more comfort,
 * by adding support for <tt>nullptr</tt> checks.
 *
 * @param transformHandle handle of the transform
 *
 * @post If the handle is <tt>nullptr</tt>, nothing happens. Otherwise,
 * <tt>cmsDeleteTransform()</tt> is called, and afterwards the handle is set
 * to <tt>nullptr</tt>. */
void ColorEnginePrivate::deleteTransform(cmsHTRANSFORM *transformHandle)
{
    if ((*transformHandle) != nullptr) {
        cmsDeleteTransform(*transformHandle);
        (*transformHandle) = nullptr;
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
double ColorEngine::profileMaximumCielchD50Chroma() const
{
    return d_pointer->m_profileMaximumCielchD50Chroma;
}

// No documentation here (documentation of properties
// and its getters are in the header)
double ColorEngine::profileMaximumOklchChroma() const
{
    return d_pointer->m_profileMaximumOklchChroma;
}

/** @brief Reduces the chroma until the color fits into the gamut.
 *
 * It always preserves the hue. It preservers the lightness whenever
 * possible.
 *
 * @note In some cases with very curvy color spaces, the nearest in-gamut
 * color (with the same lightness and hue) might be at <em>higher</em>
 * chroma. As this function always <em>reduces</em> the chroma,
 * in this case the result is not the nearest in-gamut color.
 *
 * @param cielchD50color The color that will be adapted.
 *
 * @returns An @ref isCielchD50InGamut color. */
PerceptualColor::GenericColor ColorEngine::reduceCielchD50ChromaToFitIntoGamut(const PerceptualColor::GenericColor &cielchD50color) const
{
    GenericColor referenceColor = cielchD50color;

    // Normalize the LCH coordinates
    normalizePolar360(referenceColor.second, referenceColor.third);

    // Bound to valid range:
    referenceColor.second = qMin<decltype(referenceColor.second)>( //
        referenceColor.second, //
        profileMaximumCielchD50Chroma());
    referenceColor.first = qBound(d_pointer->m_cielabD50BlackpointL, //
                                  referenceColor.first, //
                                  d_pointer->m_cielabD50WhitepointL);

    // Test special case: If we are yet in-gamut…
    if (isCielchD50InGamut(referenceColor)) {
        return referenceColor;
    }

    // Now we know: We are out-of-gamut.
    GenericColor temp;

    // Create an in-gamut point on the gray axis:
    GenericColor lowerChroma{referenceColor.first, 0, referenceColor.third};
    if (!isCielchD50InGamut(lowerChroma)) {
        // This is quite strange because every point between the blackpoint
        // and the whitepoint on the gray axis should be in-gamut on
        // normally shaped gamuts. But as we never know, we need a fallback,
        // which is guaranteed to be in-gamut:
        referenceColor.first = d_pointer->m_cielabD50BlackpointL;
        lowerChroma.first = d_pointer->m_cielabD50BlackpointL;
    }
    // Do a quick-approximate search:
    GenericColor upperChroma{referenceColor};
    // Now we know for sure that lowerChroma is in-gamut
    // and upperChroma is out-of-gamut…
    temp = upperChroma;
    while (upperChroma.second - lowerChroma.second > gamutPrecisionCielab) {
        // Our test candidate is half the way between lowerChroma
        // and upperChroma:
        temp.second = ((lowerChroma.second + upperChroma.second) / 2);
        if (isCielchD50InGamut(temp)) {
            lowerChroma = temp;
        } else {
            upperChroma = temp;
        }
    }
    return lowerChroma;
}

/** @brief Reduces the chroma until the color fits into the gamut.
 *
 * It always preserves the hue. It preservers the lightness whenever
 * possible.
 *
 * @note In some cases with very curvy color spaces, the nearest in-gamut
 * color (with the same lightness and hue) might be at <em>higher</em>
 * chroma. As this function always <em>reduces</em> the chroma,
 * in this case the result is not the nearest in-gamut color.
 *
 * @param oklchColor The color that will be adapted.
 *
 * @returns An @ref isOklchInGamut color. */
PerceptualColor::GenericColor ColorEngine::reduceOklchChromaToFitIntoGamut(const PerceptualColor::GenericColor &oklchColor) const
{
    GenericColor referenceColor = oklchColor;

    // Normalize the LCH coordinates
    normalizePolar360(referenceColor.second, referenceColor.third);

    // Bound to valid range:
    referenceColor.second = qMin<decltype(referenceColor.second)>( //
        referenceColor.second, //
        profileMaximumOklchChroma());
    referenceColor.first = qBound(d_pointer->m_oklabBlackpointL,
                                  referenceColor.first, //
                                  d_pointer->m_oklabWhitepointL);

    // Test special case: If we are yet in-gamut…
    if (isOklchInGamut(referenceColor)) {
        return referenceColor;
    }

    // Now we know: We are out-of-gamut.
    GenericColor temp;

    // Create an in-gamut point on the gray axis:
    GenericColor lowerChroma{referenceColor.first, 0, referenceColor.third};
    if (!isOklchInGamut(lowerChroma)) {
        // This is quite strange because every point between the blackpoint
        // and the whitepoint on the gray axis should be in-gamut on
        // normally shaped gamuts. But as we never know, we need a fallback,
        // which is guaranteed to be in-gamut:
        referenceColor.first = d_pointer->m_oklabBlackpointL;
        lowerChroma.first = d_pointer->m_oklabBlackpointL;
    }
    // Do a quick-approximate search:
    GenericColor upperChroma{referenceColor};
    // Now we know for sure that lowerChroma is in-gamut
    // and upperChroma is out-of-gamut…
    temp = upperChroma;
    while (upperChroma.second - lowerChroma.second > gamutPrecisionOklab) {
        // Our test candidate is half the way between lowerChroma
        // and upperChroma:
        temp.second = ((lowerChroma.second + upperChroma.second) / 2);
        if (isOklchInGamut(temp)) {
            lowerChroma = temp;
        } else {
            upperChroma = temp;
        }
    }
    return lowerChroma;
}

/** @brief Conversion to CIELab.
 *
 * @param rgbColor The original color.
 * @returns The corresponding (opaque) CIELab color.
 *
 * @note By definition, each RGB color in a given color space is an in-gamut
 * color in this very same color space. Nevertheless, because of rounding
 * errors, when converting colors that are near to the outer hull of the
 * gamut/color space, than @ref isCielabD50InGamut() might return <tt>false</tt> for
 * a return value of <em>this</em> function.
 *
 * @todo NICETOHAVE Write a unit test for this function.
 */
cmsCIELab ColorEngine::toCielabD50(const QRgba64 rgbColor) const
{
    constexpr qreal maximum = //
        std::numeric_limits<decltype(rgbColor.red())>::max();
    const double my_rgb[]{rgbColor.red() / maximum, //
                          rgbColor.green() / maximum, //
                          rgbColor.blue() / maximum};
    cmsCIELab cielabD50;
    cmsDoTransform(d_pointer->m_transformRgbToCielabD50Handle, // handle to transform
                   &my_rgb, // input
                   &cielabD50, // output
                   1 // convert exactly 1 value
    );
    if (cielabD50.L < 0) {
        // Workaround for https://github.com/mm2/Little-CMS/issues/395
        cielabD50.L = 0;
    }
    return cielabD50;
}

/** @brief Conversion to CIELCh-D50.
 *
 * @param rgbColor The original color.
 * @returns The corresponding (opaque) CIELCh-D50 color.
 *
 * @note By definition, each RGB color in a given color space is an in-gamut
 * color in this very same color space. Nevertheless, because of rounding
 * errors, when converting colors that are near to the outer hull of the
 * gamut/color space, than @ref isCielchD50InGamut() might return
 * <tt>false</tt> for a return value of <em>this</em> function.
 */
PerceptualColor::GenericColor ColorEngine::toCielchD50(const QRgba64 rgbColor) const
{
    constexpr qreal maximum = //
        std::numeric_limits<decltype(rgbColor.red())>::max();
    const double my_rgb[]{rgbColor.red() / maximum, //
                          rgbColor.green() / maximum, //
                          rgbColor.blue() / maximum};
    cmsCIELab cielabD50;
    cmsDoTransform(d_pointer->m_transformRgbToCielabD50Handle, // handle to transform
                   &my_rgb, // input
                   &cielabD50, // output
                   1 // convert exactly 1 value
    );
    if (cielabD50.L < 0) {
        // Workaround for https://github.com/mm2/Little-CMS/issues/395
        cielabD50.L = 0;
    }
    cmsCIELCh cielchD50;
    cmsLab2LCh(&cielchD50, // output
               &cielabD50 // input
    );
    return GenericColor{cielchD50.L, cielchD50.C, cielchD50.h};
}

/**
 * @brief Conversion LCh polar coordinates to corresponding Lab Cartesian
 * coordinates.
 *
 * @param lch The original LCh polar coordinates.
 *
 * @returns The corresponding Lab Cartesian coordinates.
 *
 * @note This function can convert both, from @ref ColorModel::CielchD50 to
 * @ref ColorModel::CielabD50, and from @ref ColorModel::OklchD65 to
 * @ref ColorModel::OklabD65.
 */
cmsCIELab ColorEngine::fromLchToCmsCIELab(const GenericColor &lch)
{
    const cmsCIELCh myCmsCieLch = lch.reinterpretAsLchToCmscielch();
    cmsCIELab lab; // uses cmsFloat64Number internally
    cmsLCh2Lab(&lab, // output
               &myCmsCieLch // input
    );
    return lab;
}

/** @brief Conversion to QRgb.
 *
 * @param cielchD50 The original color.
 *
 * @returns If the original color is in-gamut, the corresponding
 * (opaque) in-range RGB value. If the original color is out-of-gamut,
 * a more or less similar (opaque) in-range RGB value.
 *
 * @note There is no guarantee <em>which</em> specific algorithm is used
 * to fit out-of-gamut colors into the gamut.
 *
 * @sa @ref fromCielabD50ToQRgbOrTransparent */
QRgb ColorEngine::fromCielchD50ToQRgbBound(const GenericColor &cielchD50) const
{
    const auto cielabD50 = fromLchToCmsCIELab(cielchD50);
    cmsUInt16Number rgb_int[3];
    cmsDoTransform(d_pointer->m_transformCielabD50ToRgb16Handle, // transform
                   &cielabD50, // input
                   rgb_int, // output
                   1 // number of values to convert
    );
    constexpr qreal channelMaximumQReal = //
        std::numeric_limits<cmsUInt16Number>::max();
    constexpr quint8 rgbMaximum = 255;
    return qRgb(qRound(rgb_int[0] / channelMaximumQReal * rgbMaximum), //
                qRound(rgb_int[1] / channelMaximumQReal * rgbMaximum), //
                qRound(rgb_int[2] / channelMaximumQReal * rgbMaximum));
}

/** @brief Check if a color is within the gamut.
 * @param lch the color
 * @returns <tt>true</tt> if the color is in the gamut.
 * <tt>false</tt> otherwise. */
bool ColorEngine::isCielchD50InGamut(const GenericColor &lch) const
{
    if (!isInRange<decltype(lch.first)>(0, lch.first, 100)) {
        return false;
    }
    if (!isInRange<decltype(lch.first)>( //
            (-1) * d_pointer->m_profileMaximumCielchD50Chroma, //
            lch.second, //
            d_pointer->m_profileMaximumCielchD50Chroma //
            )) {
        return false;
    }
    const auto cielabD50 = fromLchToCmsCIELab(lch);
    return qAlpha(fromCielabD50ToQRgbOrTransparent(cielabD50)) != 0;
}

/** @brief Check if a color is within the gamut.
 * @param lch the color
 * @returns <tt>true</tt> if the color is in the gamut.
 * <tt>false</tt> otherwise. */
bool ColorEngine::isOklchInGamut(const GenericColor &lch) const
{
    if (!isInRange<decltype(lch.first)>(0, lch.first, 1)) {
        return false;
    }
    if (!isInRange<decltype(lch.first)>( //
            (-1) * d_pointer->m_profileMaximumOklchChroma, //
            lch.second, //
            d_pointer->m_profileMaximumOklchChroma //
            )) {
        return false;
    }
    const auto oklab = AbsoluteColor::fromPolarToCartesian(GenericColor(lch));
    const auto xyzD65 = AbsoluteColor::fromOklabToXyzD65(oklab);
    const auto xyzD50 = AbsoluteColor::fromXyzD65ToXyzD50(xyzD65);
    const auto cielabD50 = AbsoluteColor::fromXyzD50ToCielabD50(xyzD50);
    const auto cielabD50cms = cielabD50.reinterpretAsLabToCmscielab();
    const auto rgb = fromCielabD50ToQRgbOrTransparent(cielabD50cms);
    return (qAlpha(rgb) != 0);
}

/** @brief Check if a color is within the gamut.
 *
 * @param oklab the color
 *
 * @returns <tt>true</tt> if the color is in the gamut.
 * <tt>false</tt> otherwise. */
bool ColorEngine::isOklabInGamut(const PerceptualColor::GenericColor &oklab) const
{
    if (!isInRange<decltype(oklab.first)>(0, oklab.first, 1)) {
        return false;
    }
    const auto xyzD65 = AbsoluteColor::fromOklabToXyzD65(oklab);
    const auto xyzD50 = AbsoluteColor::fromXyzD65ToXyzD50(xyzD65);
    const auto cielabD50 = AbsoluteColor::fromXyzD50ToCielabD50(xyzD50);
    const auto cielabD50cms = cielabD50.reinterpretAsLabToCmscielab();
    const auto rgb = fromCielabD50ToQRgbOrTransparent(cielabD50cms);
    return (qAlpha(rgb) != 0);
}

/** @brief Check if a color is within the gamut.
 * @param lab the color
 * @returns <tt>true</tt> if the color is in the gamut.
 * <tt>false</tt> otherwise. */
bool ColorEngine::isCielabD50InGamut(const cmsCIELab &lab) const
{
    if (!isInRange<decltype(lab.L)>(0, lab.L, 100)) {
        return false;
    }
    const auto chromaSquare = lab.a * lab.a + lab.b * lab.b;
    const auto maximumChromaSquare = qPow(d_pointer->m_profileMaximumCielchD50Chroma, 2);
    if (chromaSquare > maximumChromaSquare) {
        return false;
    }
    return qAlpha(fromCielabD50ToQRgbOrTransparent(lab)) != 0;
}

/** @brief Conversion to QRgb.
 *
 * @pre
 * - Input Lightness: 0 ≤ lightness ≤ 100
 * @pre
 * - Input Chroma: − @ref ColorEngine::profileMaximumCielchD50Chroma ≤ chroma ≤
 *   @ref ColorEngine::profileMaximumCielchD50Chroma
 *
 * @param lab the original color
 *
 * @returns An opaque color matching the original if it is within the gamut.
 *          Otherwise, returns a fully transparent color (alpha and RGB
 *          channels set to 0 to ensure ).
 *
 * @sa @ref fromCielchD50ToQRgbBound */
QRgb ColorEngine::fromCielabD50ToQRgbOrTransparent(const cmsCIELab &lab) const
{
    double rgb[3];
    cmsDoTransform(
        // Parameters:
        d_pointer->m_transformCielabD50ToRgbHandle, // handle to transform function
        &lab, // input
        &rgb, // output
        1 // convert exactly 1 value
    );

    // Detect if valid:
    const bool colorIsValid = //
        isInRange<double>(0, rgb[0], 1) //
        && isInRange<double>(0, rgb[1], 1) //
        && isInRange<double>(0, rgb[2], 1);
    if (!colorIsValid) {
        return qRgbTransparent;
    }

    // Detect deviation:
    cmsCIELab roundtripCielabD50;
    cmsDoTransform(
        // Parameters:
        d_pointer->m_transformRgbToCielabD50Handle, // handle to transform function
        &rgb, // input
        &roundtripCielabD50, // output
        1 // convert exactly 1 value
    );
    const qreal actualDeviationSquare = //
        qPow(lab.L - roundtripCielabD50.L, 2) //
        + qPow(lab.a - roundtripCielabD50.a, 2) //
        + qPow(lab.b - roundtripCielabD50.b, 2);
    constexpr auto cielabDeviationLimitSquare = //
        ColorEnginePrivate::cielabDeviationLimit //
        * ColorEnginePrivate::cielabDeviationLimit;
    const bool actualDeviationIsOkay = //
        actualDeviationSquare <= cielabDeviationLimitSquare;

    // If deviation is too big, return a transparent color.
    if (!actualDeviationIsOkay) {
        return qRgbTransparent;
    }

    // If in-gamut, return an opaque color.
    QColor temp = qColorFromRgbDouble(rgb[0], rgb[1], rgb[2]);
    return temp.rgb();
}

/** @brief Conversion to RGB.
 *
 * @param lch The original color.
 *
 * @returns If the original color is in-gamut, it returns the corresponding
 * in-range RGB color. If the original color is out-of-gamut, it returns an
 * RGB value which might be in-range or out-of range. The RGB value range
 * is [0, 1]. */
PerceptualColor::GenericColor ColorEngine::fromCielchD50ToRgb1(const PerceptualColor::GenericColor &lch) const
{
    const auto cielabD50 = fromLchToCmsCIELab(lch);
    double rgb[3];
    cmsDoTransform(
        // Parameters:
        d_pointer->m_transformCielabD50ToRgbHandle, // handle to transform function
        &cielabD50, // input
        &rgb, // output
        1 // convert exactly 1 value
    );
    return GenericColor(rgb[0], rgb[1], rgb[2]);
}

/**
 * @brief Initialization for various data items related to the chromatic
 * boundary.
 */
void ColorEnginePrivate::initializeChromaticityBoundaries()
{
    QList<QColor> chromaticityBoundaryQColor;
    chromaticityBoundaryQColor.reserve(256 * 6);
    for (int value = 0; value <= 255; ++value) {
        // This will create six duplicate values (at the borders between
        // the six value groups). These will be filtered out later
        // automatically, because std::map does not allow duplicates.

        // Red = 255
        chromaticityBoundaryQColor.append(QColor(255, value, 0)); // Vary green
        chromaticityBoundaryQColor.append(QColor(255, 0, value)); // Vary blue

        // Green = 255
        chromaticityBoundaryQColor.append(QColor(value, 255, 0)); // Vary red
        chromaticityBoundaryQColor.append(QColor(0, 255, value)); // Vary blue

        // Blue = 255
        chromaticityBoundaryQColor.append(QColor(value, 0, 255)); // Vary red
        chromaticityBoundaryQColor.append(QColor(0, value, 255)); // Vary green
    }

    m_profileMaximumCielchD50Chroma = 0;
    m_profileMaximumOklchChroma = 0;
    for (auto &color : chromaticityBoundaryQColor) {
        const auto rgb = color.rgba64();
        const auto cielabD50 = GenericColor(q_pointer->toCielabD50(rgb));

        const auto cielchD50 = AbsoluteColor::fromCartesianToPolar(cielabD50);
        m_profileMaximumCielchD50Chroma = qMax( //
            m_profileMaximumCielchD50Chroma, //
            cielchD50.second);
        m_chromaticityBoundaryByCielchD50Hue360[cielchD50.third] = color;

        const auto xyzD50 = AbsoluteColor::fromCielabD50ToXyzD50(cielabD50);
        const auto xyzD65 = AbsoluteColor::fromXyzD50ToXyzD65(xyzD50);
        const auto oklab = AbsoluteColor::fromXyzD65ToOklab(xyzD65);
        const auto oklch = AbsoluteColor::fromCartesianToPolar(oklab);
        m_profileMaximumOklchChroma = qMax( //
            m_profileMaximumOklchChroma, //
            oklch.second);
        m_chromaticityBoundaryByOklabHue360[oklch.third] = color;
    }

    auto addDuplicates = [](auto &boundaryMap) {
        const auto firstKey = boundaryMap.begin()->first;
        const auto firstValue = boundaryMap.begin()->second;
        const auto lastKey = boundaryMap.rbegin()->first;
        const auto lastValue = boundaryMap.rbegin()->second;
        // In our circle, we create duplicates for the lowest and highest
        // angles beyond the [0, 360] boundary on the opposite side of the
        // circle. For example, the lowest original key is 2° and its duplicate
        // is placed at 362°, while the highest original key might be 357°,
        // with its duplicate at -3°.
        boundaryMap[firstKey + 360] = firstValue;
        boundaryMap[lastKey - 360] = lastValue;
    };
    addDuplicates(m_chromaticityBoundaryByCielchD50Hue360);
    addDuplicates(m_chromaticityBoundaryByOklabHue360);

    m_profileMaximumCielchD50Chroma = std::min<double>( //
        m_profileMaximumCielchD50Chroma, //
        cielchD50Values.maximumChroma);

    m_profileMaximumOklchChroma = std::min<double>( //
        m_profileMaximumOklchChroma, //
        oklchValues.maximumChroma);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param hue360 hue in the range [0, 360]
 *
 * @returns the most chromatic color for the given hue in the current
 * RGB gamut.
 */
QColor ColorEngine::maxChromaColorByOklabHue360(double hue360) const
{
    return d_pointer->maxChromaColorByHue360( //
        hue360, //
        LchSpace::Oklch);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param hue360 hue in the range [0, 360]
 *
 * @returns the most chromatic color for the given hue in the current
 * RGB gamut.
 */
QColor ColorEngine::maxChromaColorByCielchD50Hue360(double hue360) const
{
    return d_pointer->maxChromaColorByHue360( //
        hue360, //
        LchSpace::CielchD50);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param hue360 Oklab hue in the range [0, 360]
 * @param type The type of Lch color space.
 *
 * @returns the most chromatic color for the given Oklab hue in the current
 * RGB gamut.
 */
QColor ColorEnginePrivate::maxChromaColorByHue360(double hue360, PerceptualColor::LchSpace type) const
{
    const auto &table = (type == LchSpace::CielchD50) //
        ? m_chromaticityBoundaryByCielchD50Hue360 //
        : m_chromaticityBoundaryByOklabHue360;

    // begin() points to the actual first key-value pair.
    // end() points to a virtual key-value pair after the last actual
    // key-value pair. Dereferencing is not allowed.

    // lower_bound: Returns an iterator pointing to the first element that
    // is not less than (i.e. greater or equal to) key.
    auto greaterOrEqual = //
        table.lower_bound(hue360);

    if (greaterOrEqual == table.begin()) {
        // All available keys are greater than the search key. So the key
        // at the begin is the closest match:
        return greaterOrEqual->second; // "second" returns the value of
                                       // the key-value pair.
        // NOTE If the map is empty, begin() == end(), so we would get a
        // crash. Therefore, we have to make sure that the map is initialized
        // in the constructor.
    }

    auto lower = --greaterOrEqual; // Move to the lower key

    if (greaterOrEqual == table.end()) {
        // We are at the end of the map. greaterOrEqual is not a valid
        // key-value pair. Return the value of the previous key-value pair.
        return lower->second;
    }

    // Compare distances to find the closest key
    const auto distanceToLower = qAbs(hue360 - lower->first);
    const auto distanceToHigher = qAbs(hue360 - greaterOrEqual->first);
    if (distanceToLower <= distanceToHigher) {
        return lower->second;
    } else {
        return greaterOrEqual->second;
    }
}

} // namespace PerceptualColor
