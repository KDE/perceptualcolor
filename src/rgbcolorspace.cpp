// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "rgbcolorspace.h"
// Second, the private implementation.
#include "rgbcolorspace_p.h" // IWYU pragma: associated

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
 * @attention Creates an uninitialised object. You have to call
 * @ref RgbColorSpacePrivate::initialize() <em>successfully</em>
 * before actually use object. */
RgbColorSpace::RgbColorSpace(QObject *parent)
    : QObject(parent)
    , d_pointer(new RgbColorSpacePrivate(this))
{
}

/** @brief Create an sRGB color space object.
 *
 * This is build-in, no external ICC file is used.
 *
 * @pre This function is called from the main thread.
 *
 * @returns A shared pointer to the newly created color space object.
 *
 * @sa @ref RgbColorSpaceFactory::createSrgb()
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
QSharedPointer<PerceptualColor::RgbColorSpace> RgbColorSpace::createSrgb()
{
    // Create an invalid object:
    QSharedPointer<PerceptualColor::RgbColorSpace> result{new RgbColorSpace()};

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

    // Fine-tuning (and localization) for this build-in profile:
    result->d_pointer->m_profileCreationDateTime = QDateTime();
    /*: @item Manufacturer information for the built-in sRGB color. */
    result->d_pointer->m_profileManufacturer = tr("LittleCMS");
    result->d_pointer->m_profileModel = QString();
    /*: @item Name of the built-in sRGB color space. */
    result->d_pointer->m_profileName = tr("sRGB color space");
    result->d_pointer->m_gamutIdentifier = QStringLiteral("builtinsrgb");
    result->d_pointer->m_profileMaximumCielchD50Chroma = 132;

    // Return:
    return result;
}

/** @brief Try to create a color space object for a given ICC file.
 *
 * This function may fail to create the color space object when it
 * cannot open the given file, or when the file cannot be interpreted.
 *
 * @pre This function is called from the main thread.
 *
 * @param fileName The file name. See <tt>QFile</tt> documentation
 * for what are valid file names. The file is only used during the
 * execution of this function and it is closed again at the end of
 * this function. The created object does not need the file anymore,
 * because all necessary information has already been loaded into
 * memory. Accepted are most RGB-based ICC profiles up to version 4.
 *
 * @param identifier Identifier for @ref PerceptualSettings. Must comply
 * with the conditions documented in @ref PerceptualSettings.
 *
 * @returns A shared pointer to a newly created color space object on success.
 * A shared pointer to <tt>nullptr</tt> on fail.
 *
 * @note Opening unknown or untrusted files may pose security risks. For
 * instance, an unusually large file could exhaust system memory potentially
 * leading to crashes.
 *
 * @sa @ref RgbColorSpaceFactory::tryCreateFromFile()
 *
 * @internal
 *
 * @todo SHOULDHAVE Only accept “Display Class“ profiles?
 *
 * @note Currently, there is no function that loads a profile from a memory
 * buffer instead of a file. However it would easily be possible to implement
 * this if necessary, because LittleCMS allows loading from a memory buffer.
 *
 * @note While it is not strictly necessary to call this function within
 * the main thread, we put it nevertheless as precondition because of
 * consistency with @ref createSrgb().
 *
 * @note The new <a href="https://www.color.org/iccmax/index.xalter">version 5
 * (iccMax)</a> is <em>not</em> accepted. <a href="https://www.littlecms.com/">
 * LittleCMS</a> does not support ICC version 5, but only
 * up to version 4. The ICC organization itself provides
 * a <a href="https://github.com/InternationalColorConsortium/DemoIccMAX">demo
 * implementation</a>, but this does not seem to be a complete color
 * management system. */
QSharedPointer<PerceptualColor::RgbColorSpace> RgbColorSpace::tryCreateFromFile(const QString &fileName, const QString &identifier)
{
    // Definitions
    constexpr auto myContextID = nullptr;

    // Create an IO handler for the file
    cmsIOHANDLER *myIOHandler = //
        IOHandlerFactory::createReadOnly(myContextID, fileName);
    if (myIOHandler == nullptr) {
        return nullptr;
    }

    // Create a handle to a LittleCMS profile representation
    cmsHPROFILE myProfileHandle = //
        cmsOpenProfileFromIOhandlerTHR(myContextID, myIOHandler);
    if (myProfileHandle == nullptr) {
        // If cmsOpenProfileFromIOhandlerTHR fails to create a profile
        // handle, it deletes the IO handler. Therefore,  we do not
        // have to delete the underlying IO handler manually.
        return nullptr;
    }

    // Create an invalid object:
    QSharedPointer<PerceptualColor::RgbColorSpace> newObject{new RgbColorSpace()};

    // Try to transform it into a valid object:
    const QFileInfo myFileInfo{fileName};
    newObject->d_pointer->m_profileAbsoluteFilePath = //
        myFileInfo.absoluteFilePath();
    newObject->d_pointer->m_profileFileSize = myFileInfo.size();
    const bool success = newObject->d_pointer->initialize(myProfileHandle);

    // Clean up
    cmsCloseProfile(myProfileHandle); // Also deletes the underlying IO handler

    newObject->d_pointer->m_gamutIdentifier = identifier;

    // Return
    if (success) {
        return newObject;
    }
    return nullptr;
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
bool RgbColorSpacePrivate::initialize(cmsHPROFILE rgbProfileHandle)
{
    constexpr auto renderingIntent = INTENT_ABSOLUTE_COLORIMETRIC;

    m_profileClass = cmsGetDeviceClass(rgbProfileHandle);
    m_profileColorModel = cmsGetColorSpace(rgbProfileHandle);
    // If we kept a copy of the original ICC file in a QByteArray, we
    // could provide support for on-the-fly language changes. However, it seems
    // that most ICC files do not provide different locales anyway.
    const QString defaultLocaleName = QLocale().name();
    m_profileCopyright = profileInformation(rgbProfileHandle, //
                                            cmsInfoCopyright,
                                            defaultLocaleName);
    m_profileCreationDateTime = //
        profileCreationDateTime(rgbProfileHandle);
    const auto renderingIntentIds = lcmsIntentList().keys();
    for (cmsUInt32Number id : renderingIntentIds) {
        RgbColorSpace::ProfileRoles directions;
        directions.setFlag( //
            RgbColorSpace::ProfileRole::Input, //
            cmsIsIntentSupported(rgbProfileHandle, id, LCMS_USED_AS_INPUT));
        directions.setFlag( //
            RgbColorSpace::ProfileRole::Output, //
            cmsIsIntentSupported(rgbProfileHandle, id, LCMS_USED_AS_OUTPUT));
        directions.setFlag( //
            RgbColorSpace::ProfileRole::Proof, //
            cmsIsIntentSupported(rgbProfileHandle, id, LCMS_USED_AS_PROOF));
        m_profileRenderingIntentDirections.insert(id, directions);
    }
    m_profileHasClut = false;
    const auto intents = m_profileRenderingIntentDirections.keys();
    for (auto intent : intents) {
        const auto directions = m_profileRenderingIntentDirections.value(intent);
        if (directions.testFlag(RgbColorSpace::ProfileRole::Input)) {
            m_profileHasClut = cmsIsCLUT(rgbProfileHandle, //
                                         renderingIntent, //
                                         LCMS_USED_AS_INPUT);
            if (m_profileHasClut) {
                break;
            }
        }
        if (directions.testFlag(RgbColorSpace::ProfileRole::Output)) {
            m_profileHasClut = cmsIsCLUT(rgbProfileHandle, //
                                         renderingIntent, //
                                         LCMS_USED_AS_OUTPUT);
            if (m_profileHasClut) {
                break;
            }
        }
        if (directions.testFlag(RgbColorSpace::ProfileRole::Proof)) {
            m_profileHasClut = cmsIsCLUT(rgbProfileHandle, //
                                         renderingIntent, //
                                         LCMS_USED_AS_PROOF);
            if (m_profileHasClut) {
                break;
            }
        }
    }
    m_profileHasMatrixShaper = cmsIsMatrixShaper(rgbProfileHandle);
    m_profileIccVersion = profileIccVersion(rgbProfileHandle);
    m_profileManufacturer = profileInformation(rgbProfileHandle, //
                                               cmsInfoManufacturer,
                                               defaultLocaleName);
    m_profileModel = profileInformation(rgbProfileHandle, //
                                        cmsInfoModel,
                                        defaultLocaleName);
    m_profileName = profileInformation(rgbProfileHandle, //
                                       cmsInfoDescription,
                                       defaultLocaleName);
    m_profilePcsColorModel = cmsGetPCS(rgbProfileHandle);
    m_profileTagSignatures = profileTagSignatures(rgbProfileHandle);
    // Gamma Correction Overview:
    //
    // Modern display systems, which consist of a video card and a screen, have
    // a gamma curve that determines how colors are rendered. Historically,
    // CRT (Cathode Ray Tube) screens had a gamma curve inherently defined by
    // their hardware properties. Contemporary LCD and LED screens often
    // emulate this behavior, typically using the sRGB gamma curve, which was
    // designed to closely match the natural gamma curve of CRT screens.
    //
    // ICC (International Color Consortium) profiles define color
    // transformations that assume a specific gamma curve for the display
    // system (the combination of video card and screen). For correct color
    // reproduction, the display system's gamma curve must match the one
    // expected by the ICC profile. Today, this usually means the sRGB gamma
    // curve.
    //
    // However, in some cases, for example when a custom ICC profile is created
    // using a colorimeter for screen calibration, it may assume a non-standard
    // gamma curve. This  custom gamma curve is often embedded within the
    // profile using the private “vcgt”  (Video Card Gamma Table) tag. While
    // “vcgt” is registered as a private tag in the ICC Signature Registry, it
    // is not a standard tag defined in the core ICC  specification.  The
    // operating system is responsible for ensuring that the gamma curve
    // specified in the ICC profile is applied, typically by loading it into
    // the video card hardware. However, whether the operating system actually
    // applies this gamma adjustment is not always guaranteed.
    //
    // Note: Our current codebase does not support the “vcgt” tag. If an
    // ICC profile containing a “vcgt” tag is encountered, it will be rejected.
    if (m_profileTagSignatures.contains(QStringLiteral("vcgt"))) {
        return false;
    }
    m_profileTagWhitepoint = profileReadCmsciexyzTag(rgbProfileHandle, //
                                                     cmsSigMediaWhitePointTag);
    m_profileTagBlackpoint = profileReadCmsciexyzTag(rgbProfileHandle, //
                                                     cmsSigMediaBlackPointTag);
    m_profileTagRedPrimary = profileReadCmsciexyzTag(rgbProfileHandle, //
                                                     cmsSigRedColorantTag);
    m_profileTagGreenPrimary = profileReadCmsciexyzTag(rgbProfileHandle, //
                                                       cmsSigGreenColorantTag);
    m_profileTagBluePrimary = profileReadCmsciexyzTag(rgbProfileHandle, //
                                                      cmsSigBlueColorantTag);

    {
        // Create an ICC v4 profile object for the CielabD50 color space.
        cmsHPROFILE cielabD50ProfileHandle = cmsCreateLab4Profile(
            // nullptr means: Default white point (D50)
            nullptr);

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
        // It is mandatory to close the profiles to prevent memory leaks:
        cmsCloseProfile(cielabD50ProfileHandle);
    }

    // After having closed the profiles, we can now return
    // (if appropriate) without having memory leaks:
    if ((m_transformCielabD50ToRgbHandle == nullptr) //
        || (m_transformCielabD50ToRgb16Handle == nullptr) //
        || (m_transformRgbToCielabD50Handle == nullptr) //
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
RgbColorSpace::~RgbColorSpace() noexcept
{
    RgbColorSpacePrivate::deleteTransform( //
        &d_pointer->m_transformCielabD50ToRgb16Handle);
    RgbColorSpacePrivate::deleteTransform( //
        &d_pointer->m_transformCielabD50ToRgbHandle);
    RgbColorSpacePrivate::deleteTransform( //
        &d_pointer->m_transformRgbToCielabD50Handle);
}

/** @brief Constructor
 *
 * @param backLink Pointer to the object from which <em>this</em> object
 * is the private implementation. */
RgbColorSpacePrivate::RgbColorSpacePrivate(RgbColorSpace *backLink)
    : q_pointer(backLink)
{
}

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
void RgbColorSpacePrivate::deleteTransform(cmsHTRANSFORM *transformHandle)
{
    if ((*transformHandle) != nullptr) {
        cmsDeleteTransform(*transformHandle);
        (*transformHandle) = nullptr;
    }
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString RgbColorSpace::profileAbsoluteFilePath() const
{
    return d_pointer->m_profileAbsoluteFilePath;
}

// No documentation here (documentation of properties
// and its getters are in the header)
cmsProfileClassSignature RgbColorSpace::profileClass() const
{
    return d_pointer->m_profileClass;
}

// No documentation here (documentation of properties
// and its getters are in the header)
cmsColorSpaceSignature RgbColorSpace::profileColorModel() const
{
    return d_pointer->m_profileColorModel;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString RgbColorSpace::profileCopyright() const
{
    return d_pointer->m_profileCopyright;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QDateTime RgbColorSpace::profileCreationDateTime() const
{
    return d_pointer->m_profileCreationDateTime;
}

// No documentation here (documentation of properties
// and its getters are in the header)
qint64 RgbColorSpace::profileFileSize() const
{
    return d_pointer->m_profileFileSize;
}

// No documentation here (documentation of properties
// and its getters are in the header)
bool RgbColorSpace::profileHasClut() const
{
    return d_pointer->m_profileHasClut;
}

// No documentation here (documentation of properties
// and its getters are in the header)
bool RgbColorSpace::profileHasMatrixShaper() const
{
    return d_pointer->m_profileHasMatrixShaper;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QVersionNumber RgbColorSpace::profileIccVersion() const
{
    return d_pointer->m_profileIccVersion;
}

// No documentation here (documentation of properties
// and its getters are in the header)
RgbColorSpace::RenderingIntentDirections RgbColorSpace::profileRenderingIntentDirections() const
{
    return d_pointer->m_profileRenderingIntentDirections;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString RgbColorSpace::profileManufacturer() const
{
    return d_pointer->m_profileManufacturer;
}

// No documentation here (documentation of properties
// and its getters are in the header)
double RgbColorSpace::profileMaximumCielchD50Chroma() const
{
    return d_pointer->m_profileMaximumCielchD50Chroma;
}

// No documentation here (documentation of properties
// and its getters are in the header)
double RgbColorSpace::profileMaximumOklchChroma() const
{
    return d_pointer->m_profileMaximumOklchChroma;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString RgbColorSpace::profileModel() const
{
    return d_pointer->m_profileModel;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QString RgbColorSpace::profileName() const
{
    return d_pointer->m_profileName;
}

// No documentation here (documentation of properties
// and its getters are in the header)
cmsColorSpaceSignature RgbColorSpace::profilePcsColorModel() const
{
    return d_pointer->m_profilePcsColorModel;
}

// No documentation here (documentation of properties
// and its getters are in the header)
std::optional<cmsCIEXYZ> RgbColorSpace::profileTagBlackpoint() const
{
    return d_pointer->m_profileTagBlackpoint;
}

// No documentation here (documentation of properties
// and its getters are in the header)
std::optional<cmsCIEXYZ> RgbColorSpace::profileTagBluePrimary() const
{
    return d_pointer->m_profileTagBluePrimary;
}

// No documentation here (documentation of properties
// and its getters are in the header)
std::optional<cmsCIEXYZ> RgbColorSpace::profileTagGreenPrimary() const
{
    return d_pointer->m_profileTagGreenPrimary;
}

// No documentation here (documentation of properties
// and its getters are in the header)
std::optional<cmsCIEXYZ> RgbColorSpace::profileTagRedPrimary() const
{
    return d_pointer->m_profileTagRedPrimary;
}

// No documentation here (documentation of properties
// and its getters are in the header)
QStringList RgbColorSpace::profileTagSignatures() const
{
    return d_pointer->m_profileTagSignatures;
}

// No documentation here (documentation of properties
// and its getters are in the header)
std::optional<cmsCIEXYZ> RgbColorSpace::profileTagWhitepoint() const
{
    return d_pointer->m_profileTagWhitepoint;
}

/** @brief Get information from an ICC profile via LittleCMS
 *
 * @param profileHandle handle to the ICC profile in which will be searched
 * @param infoType the type of information that is searched
 * @param languageTerritory A string of the form "language_territory", where
 * language is a lowercase, two-letter ISO 639 language code, and territory is
 * an uppercase, two- or three-letter ISO 3166 territory code. If the locale
 * has no specified territory, only the language name is required. Leave empty
 * to use the default locale of the profile.
 * @returns A QString with the information. It searches the
 * information in the current locale (language code and country code as
 * provided currently by <tt>QLocale</tt>). If the information is not
 * available in this locale, LittleCMS silently falls back to another available
 * localization. Note that the returned <tt>QString</tt> might be empty if the
 * requested information is not available in the ICC profile. */
QString RgbColorSpacePrivate::profileInformation(cmsHPROFILE profileHandle, cmsInfoType infoType, const QString &languageTerritory)
{
    QByteArray languageCode;
    QByteArray countryCode;
    // Update languageCode and countryCode to the actual locale (if possible)
    const QStringList list = languageTerritory.split(QStringLiteral(u"_"));
    // The list of locale codes should be ASCII only.
    // Therefore QString::toUtf8() should return ASCII-only valid results.
    // (We do not know what character encoding LittleCMS expects,
    // but ASCII seems a safe choice.)
    if (list.size() == 2) {
        languageCode = list.at(0).toUtf8();
        countryCode = list.at(1).toUtf8();
    }
    // Fallback for missing (empty) values to the default value recommended
    // by LittleCMS documentation: “en” and “US”.
    if (languageCode.size() != 2) {
        // Encoding of C++ string literals is UTF8 (we have static_assert
        // for this):
        languageCode = QByteArrayLiteral("en");
    }
    if (countryCode.size() != 2) {
        // Encoding of C++ string literals is UTF8 (we have a static_assert
        // for this):
        countryCode = QByteArrayLiteral("US");
    }
    // NOTE Since LittleCMS ≥ 2.16, cmsNoLanguage and cmsNoCountry could be
    // used instead of "en" and "US" and would return simply the first language
    // in the profile, but that seems less predictable and less reliably than
    // "en" and "US".
    //
    // NOTE Do only v4 profiles provide internationalization, while v2 profiles
    // don’t? This seems to be implied in LittleCMS documentation:
    //
    //     “Since 2.16, a special setting for the lenguage and country allows
    //      to access the unicode variant on V2 profiles.
    //
    //      For the language and country:
    //
    //      cmsV2Unicode
    //
    //      Many V2 profiles have this field empty or filled with bogus values.
    //      Previous versions of Little CMS were ignoring it, but with
    //      this additional setting, correct V2 profiles with two variants
    //      can be honored now. By default, the ASCII variant is returned on
    //      V2 profiles unless you specify this special setting. If you decide
    //      to use it, check the result for empty strings and if this is the
    //      case, repeat reading by using the normal path.”
    //
    // So maybe v2 profiles have just one ASCII and one Unicode string, and
    // that’s all? If so, our approach seems fine: Our locale will be honored
    // on v4 profiles, and it will be ignored on v2 profiles because we do not
    // use cmsV2Unicode. This seems a wise choice, because otherwise we would
    // need different code paths for v2 and v4 profiles, which would be even
    // even more complex than the current code, and still potentially return
    // “bogus values” (as LittleCMS the documentation states), so the result
    // would be worse than the current code.

    // Calculate the expected maximum size of the return value that we have
    // to provide for cmsGetProfileInfo later on in order to return an
    // actual value.
    const cmsUInt32Number resultLength = cmsGetProfileInfo(
        // Profile in which we search:
        profileHandle,
        // The type of information we search:
        infoType,
        // The preferred language in which we want to get the information:
        languageCode.constData(),
        // The preferred country for which we want to get the information:
        countryCode.constData(),
        // Do not actually provide the information,
        // just return the required buffer size:
        nullptr,
        // Do not actually provide the information,
        // just return the required buffer size:
        0);
    // For the actual buffer size, increment by 1. This helps us to
    // guarantee a null-terminated string later on.
    const cmsUInt32Number bufferLength = resultLength + 1;

    // NOTE According to the documentation, it seems that cmsGetProfileInfo()
    // calculates the buffer length in bytes and not in wchar_t. However,
    // the documentation (as of LittleCMS 2.9) is not clear about the
    // used encoding, and the buffer type must be wchar_t anyway, and
    // wchar_t might have different sizes (either 16 bit or 32 bit) on
    // different systems, and LittleCMS’ treatment of this situation is
    // not well documented. Therefore, we interpret the buffer length
    // as number of necessary wchart_t, which creates a greater buffer,
    // which might possibly be waste of space, but it’s just a little bit
    // of text, so that’s not so much space that is wasted finally.

    // Allocate the buffer
    std::vector<wchar_t> buffer(bufferLength);
    // Initialize the buffer with 0
    for (cmsUInt32Number i = 0; i < bufferLength; ++i) {
        buffer[i] = 0;
    }

    // Write the actual information to the buffer
    cmsGetProfileInfo(
        // profile in which we search
        profileHandle,
        // the type of information we search
        infoType,
        // the preferred language in which we want to get the information
        languageCode.constData(),
        // the preferred country for which we want to get the information
        countryCode.constData(),
        // the buffer into which the requested information will be written
        buffer.data(),
        // the buffer size as previously calculated by cmsGetProfileInfo
        resultLength);
    // Make absolutely sure the buffer is null-terminated by marking its last
    // element (the one that was the +1 "extra" element) as null.
    buffer[bufferLength - 1] = 0;

    // Create a QString() from the from the buffer
    //
    // cmsGetProfileInfo returns often strings that are smaller than the
    // previously calculated buffer size. But we had initialized the buffer
    // with null, so actually we get a null-terminated string even if LittleCMS
    // would not provide the final null.  So we read only up to the first null
    // value.
    //
    // LittleCMS returns wchar_t. This type might have different sizes:
    // Depending on the operating system either 16 bit or 32 bit.
    // LittleCMS does not specify the encoding in its documentation for
    // cmsGetProfileInfo() as of LittleCMS 2.9. It only says “Strings are
    // returned as wide chars.” So this is likely either UTF-16 or UTF-32.
    // According to github.com/mm2/Little-CMS/issues/180#issue-421837278
    // it is even UTF-16 when the size of wchar_t is 32 bit! And according
    // to github.com/mm2/Little-CMS/issues/180#issuecomment-1007490587
    // in LittleCMS versions after 2.13 it might be UTF-32 when the size
    // of wchar_t is 32 bit. So the behaviour of LittleCMS changes between
    // various versions. Conclusion: It’s either UTF-16 or UTF-32, but we
    // never know which it is and have to be prepared for all possible
    // combinations between UTF-16/UTF-32 and a wchar_t size of
    // 16 bit/32 bit.
    //
    // QString::fromWCharArray can create a QString from this data. It
    // accepts arrays of wchar_t. As Qt’s documentation of
    // QString::fromWCharArray() says:
    //
    //     “If wchar is 4 bytes, the string is interpreted as UCS-4,
    //      if wchar is 2 bytes it is interpreted as UTF-16.”
    //
    // However, apparently this is not exact: When wchar is 4 bytes,
    // surrogate pairs in the code unit array are interpreted like UTF-16:
    // The surrogate pair is recognized as such, which is not strictly
    // UTF-32 conform, but enhances the compatibility. Single surrogates
    // cannot be interpreted correctly, but there will be no crash:
    // QString::fromWCharArray will continue to read, also the part
    // after the first UTF error. So QString::fromWCharArray is quite
    // error-tolerant, which is great as we do not exactly know the
    // encoding of the buffer that LittleCMS returns. However, this is
    // undocumented behaviour of QString::fromWCharArray which means
    // it could change over time. Therefore, in the unit tests of this
    // class, we test if QString::fromWCharArray actually behaves as we want.
    //
    // NOTE Instead of cmsGetProfileInfo(), we could also use
    // cmsGetProfileInfoUTF8() which returns directly an UTF-8 encoded
    // string. We were no longer required to guess the encoding, but we
    // would have a return value in a well-defined encoding. However,
    // this would  also require LittleCMS ≥ 2.16, and we would still
    // need the buffer.
    const QString result = QString::fromWCharArray(
        // Convert to string with these parameters:
        buffer.data(), // read from this buffer
        -1 // read until the first null element
    );

    // Return
    return result;
}

/** @brief Get ICC version from profile via LittleCMS
 *
 * @param profileHandle handle to the ICC profile
 * @returns The version number of the ICC format used in the profile. */
QVersionNumber RgbColorSpacePrivate::profileIccVersion(cmsHPROFILE profileHandle)
{
    // cmsGetProfileVersion returns a floating point number. Apparently
    // the digits before the decimal separator are the major version,
    // and the digits after the decimal separator are the minor version.
    // So, the version number strings “2.1” (major version 2, minor version 1)
    // and “2.10” (major version 2, minor version 10) both get the same
    // representation as floating point number 2.1 because floating
    // point numbers do not have memory about how many trailing zeros
    // exist. So we have to assume minor versions higher than 9 are not
    // supported by cmsGetProfileVersion anyway. A positive side effect
    // of this assumption is that is makes the conversion to QVersionNumber
    // easier: We use a fixed width of exactly one digit for the
    // part after the decimal separator. This makes also sure that
    // the floating point number 2 is interpreted as “2.0” (and not
    // simply as “2”).

    // QString::number() ignores the locale and uses always a “.”
    // as separator, which is exactly what we need to create
    // a QVersionNumber from.
    const QString versionString = QString::number( //
        cmsGetProfileVersion(profileHandle), // floating point
        'f', // use normal rendering format (no exponents)
        1 // number of digits after the decimal point
    );
    return QVersionNumber::fromString(versionString);
}

/** @brief Date and time of creation of a profile via LittleCMS
 *
 * @param profileHandle handle to the ICC profile
 * @returns Date and time of creation of the profile, if available. An invalid
 * date and time otherwise. */
QDateTime RgbColorSpacePrivate::profileCreationDateTime(cmsHPROFILE profileHandle)
{
    tm myDateTime; // The type “tm” as defined in C (time.h), as LittleCMS expects.
    const bool success = cmsGetHeaderCreationDateTime(profileHandle, &myDateTime);
    if (!success) {
        // Return invalid QDateTime object
        return QDateTime();
    }
    const QDate myDate(myDateTime.tm_year + 1900, // tm_year means: years since 1900
                       myDateTime.tm_mon + 1, // tm_mon ranges fromm 0 to 11
                       myDateTime.tm_mday // tm_mday ranges from 1 to 31
    );
    // “tm” allows seconds higher than 59: It allows up to 60 seconds: The
    // “supplement” second is for leap seconds. However, QTime does not
    // accept seconds beyond 59. Therefore, this has to be corrected:
    const QTime myTime(myDateTime.tm_hour, //
                       myDateTime.tm_min, //
                       qBound(0, myDateTime.tm_sec, 59));
    return QDateTime(
        // Date:
        myDate,
        // Time:
        myTime,
        // Assuming UTC for the QDateTime because it’s the only choice
        // that will not change arbitrary. QTimeZone(0) constructs a time
        // zone with 0 seconds offset to UTC.
        QTimeZone(0));
}

/** @brief List of tag signatures that are actually present in the profile.
 *
 * @param profileHandle handle to the ICC profile
 * @returns A list of tag signatures actually present in the profile. Contains
 * both, public and private signatures. See @ref profileTagSignatures for
 * details. */
QStringList RgbColorSpacePrivate::profileTagSignatures(cmsHPROFILE profileHandle)
{
    const cmsInt32Number count = cmsGetTagCount(profileHandle);
    if (count < 0) {
        return QStringList();
    }
    QStringList returnValue;
    returnValue.reserve(count);
    const cmsUInt32Number countUnsigned = static_cast<cmsUInt32Number>(count);
    using underlyingType = std::underlying_type<cmsTagSignature>::type;
    for (cmsUInt32Number i = 0; i < countUnsigned; ++i) {
        const underlyingType value = cmsGetTagSignature(profileHandle, i);
        QByteArray byteArray;
        byteArray.reserve(4);
        // Extract the 4 lowest bytes
        static_assert( //
            sizeof(underlyingType) == 4, //
            "cmsTagSignature must have 4 bytes for this code to work.");
        byteArray.append(static_cast<char>((value >> 24) & 0xFF));
        byteArray.append(static_cast<char>((value >> 16) & 0xFF));
        byteArray.append(static_cast<char>((value >> 8) & 0xFF));
        byteArray.append(static_cast<char>(value & 0xFF));
        // Convert QByteArray to QString
        returnValue.append(QString::fromLatin1(byteArray));
    }
    return returnValue;
}

/** @brief Reads a tag from a profile and converts to cmsCIEXYZ.
 *
 * @pre signature is a tag signature for which LittleCMS will return a
 * pointer to an cmsCIEXYZ value (see LittleCMS documentation).
 *
 * @warning If the precondition is not fulfilled, this will produce undefined
 * behaviour and possibly a segmentation fault.
 *
 * @param profileHandle handle to the ICC profile
 * @param signature signature of the tag to search for
 * @returns The value of the requested tag if present in the profile.
 * An <tt>std::nullopt</tt> otherwise. */
std::optional<cmsCIEXYZ> RgbColorSpacePrivate::profileReadCmsciexyzTag(cmsHPROFILE profileHandle, cmsTagSignature signature)
{
    if (!cmsIsTag(profileHandle, signature)) {
        return std::nullopt;
    }

    void *voidPointer = cmsReadTag(profileHandle, signature);

    if (voidPointer == nullptr) {
        return std::nullopt;
    }

    const cmsCIEXYZ result = *static_cast<cmsCIEXYZ *>(voidPointer);

    return result;
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
PerceptualColor::GenericColor RgbColorSpace::reduceCielchD50ChromaToFitIntoGamut(const PerceptualColor::GenericColor &cielchD50color) const
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
PerceptualColor::GenericColor RgbColorSpace::reduceOklchChromaToFitIntoGamut(const PerceptualColor::GenericColor &oklchColor) const
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
cmsCIELab RgbColorSpace::toCielabD50(const QRgba64 rgbColor) const
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
PerceptualColor::GenericColor RgbColorSpace::toCielchD50(const QRgba64 rgbColor) const
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
cmsCIELab RgbColorSpace::fromLchToCmsCIELab(const GenericColor &lch)
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
QRgb RgbColorSpace::fromCielchD50ToQRgbBound(const GenericColor &cielchD50) const
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
bool RgbColorSpace::isCielchD50InGamut(const GenericColor &lch) const
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
bool RgbColorSpace::isOklchInGamut(const GenericColor &lch) const
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
 * @param lab the color
 * @returns <tt>true</tt> if the color is in the gamut.
 * <tt>false</tt> otherwise. */
bool RgbColorSpace::isCielabD50InGamut(const cmsCIELab &lab) const
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
 * - Input Chroma: − @ref RgbColorSpace::profileMaximumCielchD50Chroma ≤ chroma ≤
 *   @ref RgbColorSpace::profileMaximumCielchD50Chroma
 *
 * @param lab the original color
 *
 * @returns An opaque color matching the original if it is within the gamut.
 *          Otherwise, returns a fully transparent color (alpha and RGB
 *          channels set to 0 to ensure ).
 *
 * @sa @ref fromCielchD50ToQRgbBound */
QRgb RgbColorSpace::fromCielabD50ToQRgbOrTransparent(const cmsCIELab &lab) const
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
        RgbColorSpacePrivate::cielabDeviationLimit //
        * RgbColorSpacePrivate::cielabDeviationLimit;
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
PerceptualColor::GenericColor RgbColorSpace::fromCielchD50ToRgb1(const PerceptualColor::GenericColor &lch) const
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
void RgbColorSpacePrivate::initializeChromaticityBoundaries()
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

    m_profileMaximumCielchD50Chroma *= chromaDetectionIncrementFactor;
    m_profileMaximumCielchD50Chroma += cielabDeviationLimit;
    m_profileMaximumCielchD50Chroma = std::min<double>( //
        m_profileMaximumCielchD50Chroma, //
        CielchD50Values::maximumChroma);

    m_profileMaximumOklchChroma *= chromaDetectionIncrementFactor;
    m_profileMaximumOklchChroma += oklabDeviationLimit;
    m_profileMaximumOklchChroma = std::min<double>( //
        m_profileMaximumOklchChroma, //
        OklchValues::maximumChroma);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param hue360 hue in the range [0, 360]
 *
 * @returns the most chromatic color for the given hue in the current
 * RGB gamut.
 */
QColor RgbColorSpace::maxChromaColorByOklabHue360(double hue360) const
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
QColor RgbColorSpace::maxChromaColorByCielchD50Hue360(double hue360) const
{
    return d_pointer->maxChromaColorByHue360( //
        hue360, //
        LchSpace::CielchD50);
}

/**
 * @brief Returns the most chromatic color for the given hue.
 *
 * @param oklabHue360 Oklab hue in the range [0, 360]
 * @param type The type of Lch color space.
 *
 * @returns the most chromatic color for the given Oklab hue in the current
 * RGB gamut.
 */
QColor RgbColorSpacePrivate::maxChromaColorByHue360(double oklabHue360, PerceptualColor::LchSpace type) const
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
        table.lower_bound(oklabHue360);

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
    const auto distanceToLower = qAbs(oklabHue360 - lower->first);
    const auto distanceToHigher = qAbs(oklabHue360 - greaterOrEqual->first);
    if (distanceToLower <= distanceToHigher) {
        return lower->second;
    } else {
        return greaterOrEqual->second;
    }
}

/**
 * @brief Identifier for the gamut (working color space).
 *
 * @returns Identifier for @ref PerceptualSettings.
 */
QString RgbColorSpace::gamutIdentifier() const
{
    return d_pointer->m_gamutIdentifier;
}

} // namespace PerceptualColor
