// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "rgbcolorspace.h"
// Second, the private implementation.
#include "rgbcolorspace_p.h" // IWYU pragma: associated

#include "constpropagatingrawpointer.h"
#include "constpropagatinguniquepointer.h"
#include "helperconstants.h"
#include "helperconversion.h"
#include "helpermath.h"
#include "helperqttypes.h"
#include "initializetranslation.h"
#include "iohandlerfactory.h"
#include "lchdouble.h"
#include "polarpointf.h"
#include "rgbdouble.h"
#include <limits>
#include <memory>
#include <optional>
#include <qbytearray.h>
#include <qcolor.h>
#include <qcoreapplication.h>
#include <qfileinfo.h>
#include <qlocale.h>
#include <qmath.h>
#include <qnamespace.h>
#include <qrgba64.h>
#include <qstringliteral.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qlist.h>
#else
#include <qstringlist.h>
#endif

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
    result->d_pointer->m_profileMaximumCielchChroma = 132;

    // Return:
    return result;
}

/** @brief Try to create a color space object for a given ICC file.
 *
 * @note This function may fail to create the color space object when it
 * cannot open the given file, or when the file cannot be interpreted.
 *
 * @pre This function is called from the main thread.
 *
 * @param fileName The file name. See <tt>QFile</tt> documentation
 * for what are valid file names. The file is only used during the
 * execution of this function and it is closed again at the end of
 * this function. The created object does not need the file anymore,
 * because all necessary information has already been loaded into
 * memory. Accepted are RGB-based ICC profiles up to version 4.
 *
 * @returns A shared pointer to a newly created color space object on success.
 * A shared pointer to <tt>nullptr</tt> on fail.
 *
 * @sa @ref RgbColorSpaceFactory::createFromFile()
 *
 * @internal
 *
 * @todo The value for @ref profileMaximumCielchChroma should be the actual maximum
 * chroma value of the profile, and not a fallback default value as currently.
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
QSharedPointer<PerceptualColor::RgbColorSpace> RgbColorSpace::createFromFile(const QString &fileName)
{
    // TODO xxx Only accept Display Class profiles

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
 * @todo This function is used in @ref RgbColorSpace::createSrgb()
 * and @ref RgbColorSpace::createFromFile(), but some of the initialization
 * is changed afterwards (file name, file size, profile name, maximum chroma).
 * Is it possible to find a more elegant design? */
bool RgbColorSpacePrivate::initialize(cmsHPROFILE rgbProfileHandle)
{
    constexpr auto renderingIntent = INTENT_ABSOLUTE_COLORIMETRIC;

    m_profileClass = cmsGetDeviceClass(rgbProfileHandle);
    m_profileColorModel = cmsGetColorSpace(rgbProfileHandle);
    m_profileCopyright = getInformationFromProfile(rgbProfileHandle, //
                                                   cmsInfoCopyright);
    m_profileCreationDateTime = //
        getCreationDateTimeFromProfile(rgbProfileHandle);
    const bool inputUsesCLUT = cmsIsCLUT(rgbProfileHandle, //
                                         renderingIntent, //
                                         LCMS_USED_AS_INPUT);
    const bool outputUsesCLUT = cmsIsCLUT(rgbProfileHandle, //
                                          renderingIntent, //
                                          LCMS_USED_AS_OUTPUT);
    // There is a third value, LCMS_USED_AS_PROOF. This value seem to return
    // always true, even for the sRGB built-in profile. Not sure if this is
    // a bug? Anyway, as we do not actually use the profile in proof mode,
    // we can discard this information.
    m_profileHasClut = inputUsesCLUT || outputUsesCLUT;
    m_profileHasMatrixShaper = cmsIsMatrixShaper(rgbProfileHandle);
    m_profileIccVersion = getIccVersionFromProfile(rgbProfileHandle);
    m_profileManufacturer = getInformationFromProfile(rgbProfileHandle, //
                                                      cmsInfoManufacturer);
    m_profileModel = getInformationFromProfile(rgbProfileHandle, //
                                               cmsInfoModel);
    m_profileName = getInformationFromProfile(rgbProfileHandle, //
                                              cmsInfoDescription);
    m_profilePcsColorModel = cmsGetPCS(rgbProfileHandle);

    {
        // Create an ICC v4 profile object for the Lab color space.
        cmsHPROFILE labProfileHandle = cmsCreateLab4Profile(
            // nullptr means: Default white point (D50)
            // TODO Does this make sense? sRGB, for example, has
            // D65 as whitepoint…
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
        m_transformLabToRgbHandle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            labProfileHandle, // input profile handle
            TYPE_Lab_DBL, // input buffer format
            rgbProfileHandle, // output profile handle
            TYPE_RGB_DBL, // output buffer format
            renderingIntent,
            flags);
        m_transformLabToRgb16Handle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            labProfileHandle, // input profile handle
            TYPE_Lab_DBL, // input buffer format
            rgbProfileHandle, // output profile handle
            TYPE_RGB_16, // output buffer format
            renderingIntent,
            flags);
        m_transformRgbToLabHandle = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            rgbProfileHandle, // input profile handle
            TYPE_RGB_DBL, // input buffer format
            labProfileHandle, // output profile handle
            TYPE_Lab_DBL, // output buffer format
            renderingIntent,
            flags);
        // It is mandatory to close the profiles to prevent memory leaks:
        cmsCloseProfile(labProfileHandle);
    }

    // After having closed the profiles, we can now return
    // (if appropriate) without having memory leaks:
    if ((m_transformLabToRgbHandle == nullptr) //
        || (m_transformLabToRgb16Handle == nullptr) //
        || (m_transformRgbToLabHandle == nullptr) //
    ) {
        return false;
    }

    // Maximum chroma:
    // TODO Detect an appropriate value for m_profileMaximumCielchChroma.

    // Find blackpoint and whitepoint. We have to make sure that:
    // 0 <= blackbpoint < whitepoint <= 100
    LchDouble candidate;
    candidate.l = 0;
    candidate.c = 0;
    candidate.h = 0;
    while (!q_pointer->isInGamut(candidate)) {
        candidate.l += gamutPrecision;
        if (candidate.l >= 100) {
            return false;
        }
    }
    m_blackpointL = candidate.l;
    candidate.l = 100;
    while (!q_pointer->isInGamut(candidate)) {
        candidate.l -= gamutPrecision;
        if (candidate.l <= m_blackpointL) {
            return false;
        }
    }
    m_whitepointL = candidate.l;

    // Now, calculate the properties who’s calculation depends on a fully
    // initialized object.
    m_profileMaximumCielchChroma = detectMaximumCielchChroma();
    m_profileMaximumOklchChroma = detectMaximumOklchChroma();

    return true;
}

/** @brief Destructor */
RgbColorSpace::~RgbColorSpace() noexcept
{
    RgbColorSpacePrivate::deleteTransform( //
        &d_pointer->m_transformLabToRgb16Handle);
    RgbColorSpacePrivate::deleteTransform( //
        &d_pointer->m_transformLabToRgbHandle);
    RgbColorSpacePrivate::deleteTransform( //
        &d_pointer->m_transformRgbToLabHandle);
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
QString RgbColorSpace::profileManufacturer() const
{
    return d_pointer->m_profileManufacturer;
}

// No documentation here (documentation of properties
// and its getters are in the header)
double RgbColorSpace::profileMaximumCielchChroma() const
{
    return d_pointer->m_profileMaximumCielchChroma;
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

/** @brief Get information from an ICC profile via LittleCMS
 *
 * @param profileHandle handle to the ICC profile in which will be searched
 * @param infoType the type of information that is searched
 * @returns A QString with the information. It searches the
 * information in the current locale (language code and country code as
 * provided currently by <tt>QLocale</tt>). If the information is not
 * available in this locale, LittleCMS silently falls back to another available
 * localization. Note that the returned <tt>QString</tt> might be empty if the
 * requested information is not available in the ICC profile. */
QString RgbColorSpacePrivate::getInformationFromProfile(cmsHPROFILE profileHandle, cmsInfoType infoType)
{
    QByteArray languageCode;
    QByteArray countryCode;
    // Update languageCode and countryCode to the actual locale (if possible)
    const QStringList list = QLocale().name().split(QStringLiteral(u"_"));
    // The list of locale codes should be ASCII only.
    // Therefore QString::toUtf8() should return ASCII-only valid results.
    // (We do not know what character encoding LittleCMS expects,
    // but ASCII seems a safe choice.)
    if (list.count() == 2) {
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
    // wchar_t might have diffferent sizes (either 16 bit or 32 bit) on
    // different systems, and LittleCMS’ treatment of this situation is
    // not well documented. Therefore, we interpret the buffer length
    // as number of necessary wchart_t, which creates a greater buffer,
    // which might possibly be waste of space, but it’s just a little bit
    // of text, so that’s not so much space that is wasted finally.

    // TODO For security reasons (you never know what surprise a foreign ICC
    // file might have for us), it would be better to have a maximum
    // length for the buffer, so that insane big buffer will not be
    // actually created, and instead an empty string is returned.

    // Allocate the buffer
    wchar_t *buffer = new wchar_t[bufferLength];
    // Initialize the buffer with 0
    for (cmsUInt32Number i = 0; i < bufferLength; ++i) {
        *(buffer + i) = 0;
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
        buffer,
        // the buffer size as previously calculated by cmsGetProfileInfo
        resultLength);
    // Make absolutely sure the buffer is null-terminated by marking its last
    // element (the one that was the +1 "extra" element) as null.
    *(buffer + (bufferLength - 1)) = 0;

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
    const QString result = QString::fromWCharArray(
        // Convert to string with these parameters:
        buffer, // read from this buffer
        -1 // read until the first null element
    );

    // Free allocated memory of the buffer
    delete[] buffer;

    // Return
    return result;
}

/** @brief Get ICC version from profile via LittleCMS
 *
 * @param profileHandle handle to the ICC profile
 * @returns The version number of the ICC format used in the profile. */
QVersionNumber RgbColorSpacePrivate::getIccVersionFromProfile(cmsHPROFILE profileHandle)
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
QDateTime RgbColorSpacePrivate::getCreationDateTimeFromProfile(cmsHPROFILE profileHandle)
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
        // that will not change arbitrary.
        Qt::TimeSpec::UTC);
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
 * @param color The color that will be adapted.
 *
 * @returns An @ref isInGamut color. */
PerceptualColor::LchDouble RgbColorSpace::reduceChromaToFitIntoGamut(const PerceptualColor::LchDouble &color) const
{
    // Normalize the LCH coordinates
    LchDouble referenceColor = color;
    {
        const PolarPointF temp(referenceColor.c, referenceColor.h);
        referenceColor.c = temp.radius();
        referenceColor.h = temp.angleDegree();
    }

    // Bound to valid range:
    referenceColor.c = qMin<decltype(referenceColor.c)>(referenceColor.c, //
                                                        profileMaximumCielchChroma());
    referenceColor.l = qBound(d_pointer->m_blackpointL, //
                              referenceColor.l, //
                              d_pointer->m_whitepointL);

    // Test special case: If we are yet in-gamut…
    if (isInGamut(referenceColor)) {
        return referenceColor;
    }

    // Now we know: We are out-of-gamut.
    LchDouble temp;

    // TODO Decide which one of the algorithms will be used (and remove
    // the other one).
    // Create an in-gamut point on the gray axis:
    LchDouble lowerChroma{referenceColor.l, 0, referenceColor.h};
    if (!isInGamut(lowerChroma)) {
        // This is quite strange because every point between the blackpoint
        // and the whitepoint on the gray axis should be in-gamut on
        // normally shaped gamuts. But as we never know, we need a fallback,
        // which is guaranteed to be in-gamut:
        referenceColor.l = d_pointer->m_blackpointL;
        lowerChroma.l = d_pointer->m_blackpointL;
    }
    constexpr bool quickApproximate = true;
    if constexpr (quickApproximate) {
        // Do a quick-approximate search:
        LchDouble upperChroma{referenceColor};
        // Now we know for sure that lowerChroma is in-gamut
        // and upperChroma is out-of-gamut…
        temp = upperChroma;
        while (upperChroma.c - lowerChroma.c > gamutPrecision) {
            // Our test candidate is half the way between lowerChroma
            // and upperChroma:
            temp.c = ((lowerChroma.c + upperChroma.c) / 2);
            if (isInGamut(temp)) {
                lowerChroma = temp;
            } else {
                upperChroma = temp;
            }
        }
        return lowerChroma;

    } else {
        // Do a slow-thorough search:
        temp = referenceColor;
        while (temp.c > 0) {
            if (isInGamut(temp)) {
                break;
            } else {
                temp.c -= gamutPrecision;
            }
        }
        if (temp.c < 0) {
            temp.c = 0;
        }
        return temp;
    }
}

/** @brief Conversion to CIELab.
 *
 * @param rgbColor The original color.
 * @returns The corresponding (opaque) CIELab color.
 *
 * @note By definition, each RGB color in a given color space is an in-gamut
 * color in this very same color space. Nevertheless, because of rounding
 * errors, when converting colors that are near to the outer hull of the
 * gamut/color space, than @ref isInGamut() might return <tt>false</tt> for
 * a return value of <em>this</em> function. */
cmsCIELab RgbColorSpace::toCielab(const QRgba64 rgbColor) const
{
    constexpr qreal maximum = //
        std::numeric_limits<decltype(rgbColor.red())>::max();
    const RgbDouble my_rgb{rgbColor.red() / maximum, //
                           rgbColor.green() / maximum, //
                           rgbColor.blue() / maximum};
    cmsCIELab lab;
    cmsDoTransform(d_pointer->m_transformRgbToLabHandle, // handle to transform
                   &my_rgb, // input
                   &lab, // output
                   1 // convert exactly 1 value
    );
    return lab;
}

/** @brief Conversion to CIELCh.
 *
 * @param rgbColor The original color.
 * @returns The corresponding (opaque) CIELCh color.
 *
 * @note By definition, each RGB color in a given color space is an in-gamut
 * color in this very same color space. Nevertheless, because of rounding
 * errors, when converting colors that are near to the outer hull of the
 * gamut/color space, than @ref isInGamut() might return <tt>false</tt> for
 * a return value of <em>this</em> function. */
PerceptualColor::LchDouble RgbColorSpace::toCielchDouble(const QRgba64 rgbColor) const
{
    constexpr qreal maximum = //
        std::numeric_limits<decltype(rgbColor.red())>::max();
    const RgbDouble my_rgb{rgbColor.red() / maximum, //
                           rgbColor.green() / maximum, //
                           rgbColor.blue() / maximum};
    cmsCIELab lab;
    cmsDoTransform(d_pointer->m_transformRgbToLabHandle, // handle to transform
                   &my_rgb, // input
                   &lab, // output
                   1 // convert exactly 1 value
    );
    cmsCIELCh lch;
    cmsLab2LCh(&lch, // output
               &lab // input
    );
    return LchDouble{lch.L, lch.C, lch.h};
}

/** @brief Conversion to QRgb.
 *
 * @param lch The original color.
 *
 * @returns If the original color is in-gamut, the corresponding
 * (opaque) in-range RGB value. If the original color is out-of-gamut,
 * a more or less similar (opaque) in-range RGB value.
 *
 * @note There is no guarantee <em>which</em> specific algorithm is used
 * to fit out-of-gamut colors into the gamut.
 *
 * @sa @ref toQRgbOrTransparent */
QRgb RgbColorSpace::toQRgbBound(const LchDouble &lch) const
{
    const cmsCIELCh myCmsCieLch = toCmsCieLch(lch);
    cmsCIELab lab; // uses cmsFloat64Number internally
    cmsLCh2Lab(&lab, // output
               &myCmsCieLch // input
    );
    cmsUInt16Number rgb_int[3];
    cmsDoTransform(d_pointer->m_transformLabToRgb16Handle, // transform
                   &lab, // input
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
bool RgbColorSpace::isInGamut(const LchDouble &lch) const
{
    if (!isInRange<decltype(lch.l)>(0, lch.l, 100)) {
        return false;
    }
    if (!isInRange<decltype(lch.l)>( //
            (-1) * d_pointer->m_profileMaximumCielchChroma, //
            lch.c, //
            d_pointer->m_profileMaximumCielchChroma //
            )) {
        return false;
    }
    cmsCIELab lab; // uses cmsFloat64Number internally
    const cmsCIELCh myCmsCieLch = toCmsCieLch(lch);
    cmsLCh2Lab(&lab, &myCmsCieLch);
    return qAlpha(toQRgbOrTransparent(lab)) != 0;
}

/** @brief Check if a color is within the gamut.
 * @param lab the color
 * @returns <tt>true</tt> if the color is in the gamut.
 * <tt>false</tt> otherwise. */
bool RgbColorSpace::isInGamut(const cmsCIELab &lab) const
{
    if (!isInRange<decltype(lab.L)>(0, lab.L, 100)) {
        return false;
    }
    const auto chromaSquare = lab.a * lab.a + lab.b * lab.b;
    const auto maximumChromaSquare = qPow(d_pointer->m_profileMaximumCielchChroma, 2);
    if (chromaSquare > maximumChromaSquare) {
        return false;
    }
    return qAlpha(toQRgbOrTransparent(lab)) != 0;
}

/** @brief Conversion to QRgb.
 *
 * @pre
 * - Input Lightness: 0 ≤ lightness ≤ 100
 * @pre
 * - Input Chroma: - @ref RgbColorSpace::profileMaximumCielchChroma ≤ chroma ≤
 *   @ref RgbColorSpace::profileMaximumCielchChroma
 *
 * @param lab the original color
 *
 * @returns The corresponding opaque color if the original color is in-gamut.
 * A transparent color otherwise.
 *
 * @sa @ref toQRgbBound */
QRgb RgbColorSpace::toQRgbOrTransparent(const cmsCIELab &lab) const
{
    constexpr QRgb transparentValue = 0;
    static_assert(qAlpha(transparentValue) == 0, //
                  "The alpha value of a transparent QRgb must be 0.");

    RgbDouble rgb;
    cmsDoTransform(
        // Parameters:
        d_pointer->m_transformLabToRgbHandle, // handle to transform function
        &lab, // input
        &rgb, // output
        1 // convert exactly 1 value
    );

    // Detect if valid:
    const bool colorIsValid = //
        isInRange<decltype(rgb.red)>(0, rgb.red, 1) //
        && isInRange<decltype(rgb.green)>(0, rgb.green, 1) //
        && isInRange<decltype(rgb.blue)>(0, rgb.blue, 1);
    if (!colorIsValid) {
        return transparentValue;
    }

    // Detect deviation:
    cmsCIELab roundtripLab;
    cmsDoTransform(
        // Parameters:
        d_pointer->m_transformRgbToLabHandle, // handle to transform function
        &rgb, // input
        &roundtripLab, // output
        1 // convert exactly 1 value
    );
    const qreal actualDeviationSquare = //
        qPow(lab.L - roundtripLab.L, 2) //
        + qPow(lab.a - roundtripLab.a, 2) //
        + qPow(lab.b - roundtripLab.b, 2);
    constexpr auto cielabDeviationLimitSquare = //
        RgbColorSpacePrivate::cielabDeviationLimit //
        * RgbColorSpacePrivate::cielabDeviationLimit;
    const bool actualDeviationIsOkay = //
        actualDeviationSquare <= cielabDeviationLimitSquare;

    // If deviation is too big, return a transparent color.
    if (!actualDeviationIsOkay) {
        return transparentValue;
    }

    // If in-gamut, return an opaque color.
    QColor temp = QColor::fromRgbF(static_cast<QColorFloatType>(rgb.red), //
                                   static_cast<QColorFloatType>(rgb.green), //
                                   static_cast<QColorFloatType>(rgb.blue));
    return temp.rgb();
}

/** @brief Conversion to @ref RgbDouble.
 *
 * @param lch The original color.
 *
 * @returns If the original color is in-gamut, it returns the corresponding
 * in-range RGB color. If the original color is out-of-gamut, it returns an
 * RGB value which might be in-range or out-of range. */
PerceptualColor::RgbDouble RgbColorSpace::toRgbDoubleUnbound(const PerceptualColor::LchDouble &lch) const
{
    const cmsCIELCh myCmsCieLch = toCmsCieLch(lch);
    cmsCIELab lab; // uses cmsFloat64Number internally
    cmsLCh2Lab(&lab, // output
               &myCmsCieLch // input
    );
    RgbDouble rgb;
    cmsDoTransform(
        // Parameters:
        d_pointer->m_transformLabToRgbHandle, // handle to transform function
        &lab, // input
        &rgb, // output
        1 // convert exactly 1 value
    );
    return rgb;
}

/** @brief Calculation of @ref RgbColorSpace::profileMaximumCielchChroma
 *
 * @returns Calculation of @ref RgbColorSpace::profileMaximumCielchChroma */
double RgbColorSpacePrivate::detectMaximumCielchChroma() const
{
    // Make sure chromaDetectionPrecision is big enough to make a difference
    // when being added to floating point variable “hue” used in loop later.
    static_assert(0. + chromaDetectionPrecision > 0.);
    static_assert(360. + chromaDetectionPrecision > 360.);

    // Implementation
    double result = 0;
    double hue = 0;
    while (hue < 360) {
        const auto qColorHue = static_cast<QColorFloatType>(hue / 360.);
        const auto color = QColor::fromHsvF(qColorHue, 1, 1).rgba64();
        result = qMax(result, q_pointer->toCielchDouble(color).c);
        hue += chromaDetectionPrecision;
    }
    return result * chromaDetectionIncrementFactor + cielabDeviationLimit;
}

/** @brief Calculation of @ref RgbColorSpace::profileMaximumOklchChroma
 *
 * @returns Calculation of @ref RgbColorSpace::profileMaximumOklchChroma */
double RgbColorSpacePrivate::detectMaximumOklchChroma() const
{
    // Make sure chromaDetectionPrecision is big enough to make a difference
    // when being added to floating point variable “hue” used in loop later.
    static_assert(0. + chromaDetectionPrecision > 0.);
    static_assert(360. + chromaDetectionPrecision > 360.);

    double chromaSquare = 0;
    double hue = 0;
    while (hue < 360) {
        const auto qColorHue = static_cast<QColorFloatType>(hue / 360.);
        const auto rgbColor = QColor::fromHsvF(qColorHue, 1, 1).rgba64();
        const auto labColor = q_pointer->toCielab(rgbColor);
        const auto oklab = fromCmscielabD50ToOklab(labColor);
        chromaSquare = qMax(chromaSquare, oklab.a * oklab.a + oklab.b * oklab.b);
        hue += chromaDetectionPrecision;
    }
    return qSqrt(chromaSquare) * chromaDetectionIncrementFactor //
        + oklabDeviationLimit;
}

/** @brief Gets the rendering intents supported by the LittleCMS library.
 *
 * @returns The rendering intents supported by the LittleCMS library.
 *
 * @note Do not use this function. Instead, use @ref intentList. */
QMap<cmsUInt32Number, QString> RgbColorSpacePrivate::getIntentList()
{
    // TODO xxx Actually use this (for translation, for example), or remove it…
    QMap<cmsUInt32Number, QString> result;
    const cmsUInt32Number intentCount = //
        cmsGetSupportedIntents(0, nullptr, nullptr);
    cmsUInt32Number *codeArray = new cmsUInt32Number[intentCount];
    char **descriptionArray = new char *[intentCount];
    cmsGetSupportedIntents(intentCount, codeArray, descriptionArray);
    for (cmsUInt32Number i = 0; i < intentCount; ++i) {
        result.insert(codeArray[i], QString::fromUtf8(descriptionArray[i]));
    }
    delete[] codeArray;
    delete[] descriptionArray;
    return result;
}

} // namespace PerceptualColor
