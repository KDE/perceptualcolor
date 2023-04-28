// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "rgbcolorspace.h"
// Second, the private implementation.
#include "rgbcolorspace_p.h" // IWYU pragma: keep

#include "cielchd50values.h"
#include "constpropagatinguniquepointer.h"
#include "helpermath.h"
#include "helperposixmath.h"
#include "lchdouble.h"
#include "rgbcolorspacefactory.h"
#include <lcms2.h>
#include <qcolor.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qlist.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qrgb.h>
#include <qrgba64.h>
#include <qscopedpointer.h>
#include <qsharedpointer.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qtemporarydir.h>
#include <qtemporaryfile.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qversionnumber.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qstring.h>
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#include <qvector.h>
#endif

namespace PerceptualColor
{
class TestRgbColorSpace : public QObject
{
    Q_OBJECT

public:
    explicit TestRgbColorSpace(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private Q_SLOTS:
    void initTestCase()
    {
        // Called before the first test function is executed
    }

    void cleanupTestCase()
    {
        // Called after the last test function was executed
    }

    void init()
    {
        // Called before each test function is executed
    }

    void cleanup()
    {
        // Called after every test function
    }

    void testConstructorDestructorUninitialized()
    {
        RgbColorSpace myColorSpace;
    }

    void testCreateSrgb()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace;

        myColorSpace = RgbColorSpace::createSrgb();
        QCOMPARE(myColorSpace.isNull(), false);

        QVERIFY(isInRange<qreal>(0, myColorSpace->d_pointer->m_cielabD50BlackpointL, 1));
        QVERIFY( //
            isInRange<qreal>(99, myColorSpace->d_pointer->m_cielabD50WhitepointL, 100));

        QVERIFY(isInRange<qreal>(0.00, myColorSpace->d_pointer->m_oklabBlackpointL, 0.01));
        QVERIFY( //
            isInRange<qreal>(0.99, myColorSpace->d_pointer->m_oklabWhitepointL, 1.00));
    }

    void testCreateFromFile()
    {
        QScopedPointer<QTemporaryFile> invalidFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/ascii-abcd.txt")));
        if (invalidFile.isNull()) {
            throw 0;
        }
        QScopedPointer<QTemporaryFile> validRgbFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (validRgbFile.isNull()) {
            throw 0;
        }
        QTemporaryDir existingDirectoryWithoutTrailingSlash;
        if (!existingDirectoryWithoutTrailingSlash.isValid()) {
            throw 0;
        }
        if (existingDirectoryWithoutTrailingSlash.path().endsWith(QStringLiteral("/"))) {
            throw 0;
        }

        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace;

        // Invalid file
        QVERIFY(QFileInfo::exists(invalidFile->fileName())); // assertion
        myColorSpace = RgbColorSpace::createFromFile(invalidFile->fileName());
        QCOMPARE(myColorSpace.isNull(), true);

        // Non-existing file/directory name
        QString nonexistingfilename = //
            QStringLiteral("/nonexistingfilename.txt");
        QCOMPARE(QFileInfo::exists(nonexistingfilename), false); // assertion
        QCOMPARE(QDir{nonexistingfilename}.exists(), false); // assertion
        myColorSpace = RgbColorSpace::createFromFile(nonexistingfilename);
        QCOMPARE(myColorSpace.isNull(), true);

        // Existing folder with trailing slash
        myColorSpace = RgbColorSpace::createFromFile( //
            existingDirectoryWithoutTrailingSlash.path() + QStringLiteral("/"));
        QCOMPARE(myColorSpace.isNull(), true);

        // Existing folder without trailing slash
        myColorSpace = RgbColorSpace::createFromFile( //
            existingDirectoryWithoutTrailingSlash.path());
        QCOMPARE(myColorSpace.isNull(), true);

        // Valid RGB profile (should load correctly)
        QVERIFY(QFileInfo::exists(validRgbFile->fileName())); // assertion
        myColorSpace = RgbColorSpace::createFromFile(validRgbFile->fileName());
        QCOMPARE(myColorSpace.isNull(), false);
    }

    void testInitialize()
    {
        QScopedPointer<QTemporaryFile> wideGamutFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (wideGamutFile.isNull()) {
            throw 0;
        }

        auto myColorSpace = RgbColorSpace::createFromFile(wideGamutFile->fileName());
        QCOMPARE(myColorSpace.isNull(), false); // assertion
        // assertion that maximum lightness is out-of-gamut for this profile:
        QCOMPARE(myColorSpace->isCielchD50InGamut(LchDouble{100, 0, 0}), false);
        QCOMPARE(myColorSpace->isOklchInGamut(LchDouble{1, 0, 0}), false);

        // Actual test:
        QVERIFY(isInRange<qreal>(0, myColorSpace->d_pointer->m_cielabD50BlackpointL, 1));
        QVERIFY( //
            isInRange<qreal>(99, myColorSpace->d_pointer->m_cielabD50WhitepointL, 100));

        QVERIFY(isInRange<qreal>(0.00, myColorSpace->d_pointer->m_oklabBlackpointL, 0.01));
        QVERIFY( //
            isInRange<qreal>(0.99, myColorSpace->d_pointer->m_oklabWhitepointL, 1.00));
    }

    void testReduceCielchD50ChromaToFitIntoGamut()
    {
        QScopedPointer<QTemporaryFile> wideGamutFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (wideGamutFile.isNull()) {
            throw 0;
        }

        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace;
        myColorSpace = RgbColorSpace::createFromFile(wideGamutFile->fileName());
        QCOMPARE(myColorSpace.isNull(), false); // assertion
        const LchDouble referenceColor{100, 50, 0};
        QCOMPARE(myColorSpace->isCielchD50InGamut(referenceColor), false); // assertion
        // The value referenceColor is out-of-gamut because WideGamutRGB stops
        // just a little bit before the lightness of 100.

        // Now, test how this special situation is handled:
        const LchDouble modifiedColor = //
            myColorSpace->reduceCielchD50ChromaToFitIntoGamut(referenceColor);
        QVERIFY(modifiedColor.c <= referenceColor.c);
        QCOMPARE(modifiedColor.h, referenceColor.h);
        QVERIFY(isInRange<qreal>(99, modifiedColor.l, 100));
        QVERIFY(modifiedColor.l < 100);
        QVERIFY(myColorSpace->isCielchD50InGamut(modifiedColor));
    }

    void testBugReduceCielchD50ChromaToFitIntoGamut()
    {
        QScopedPointer<QTemporaryFile> wideGamutFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (wideGamutFile.isNull()) {
            throw 0;
        }

        // This test looks for a bug that was seen during development
        // phase. When using WideGamutRGB and raising the lightness
        // slider up to 100%: Bug behaviour: the color switches
        // to 0% lightness. Expected behaviour: the color has almost
        // 100% lightness.
        auto myColorSpace = RgbColorSpace::createFromFile(wideGamutFile->fileName());
        LchDouble temp{100, 50, 0};
        QVERIFY(myColorSpace->reduceCielchD50ChromaToFitIntoGamut(temp).l > 95);
    }

    void testReduceOklabChromaToFitIntoGamut()
    {
        QScopedPointer<QTemporaryFile> wideGamutFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (wideGamutFile.isNull()) {
            throw 0;
        }

        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace;
        myColorSpace = RgbColorSpace::createFromFile(wideGamutFile->fileName());
        QCOMPARE(myColorSpace.isNull(), false); // assertion
        const LchDouble referenceColor{1, 0.151189, 359.374};
        QCOMPARE(myColorSpace->isOklchInGamut(referenceColor), false); // assertion
        // The value referenceColor is out-of-gamut because WideGamutRGB stops
        // just a little bit before the lightness of 100.

        // Now, test how this special situation is handled:
        const LchDouble modifiedColor = //
            myColorSpace->reduceOklchChromaToFitIntoGamut(referenceColor);
        QVERIFY(modifiedColor.c <= referenceColor.c);
        QCOMPARE(modifiedColor.h, referenceColor.h);
        QVERIFY(isInRange<qreal>(0.99, modifiedColor.l, 1));
        QVERIFY(modifiedColor.l < 1);
        QVERIFY(myColorSpace->isOklchInGamut(modifiedColor));
    }

    void testBugReduceOklabChromaToFitIntoGamut()
    {
        QScopedPointer<QTemporaryFile> wideGamutFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (wideGamutFile.isNull()) {
            throw 0;
        }

        // This test looks for a bug that was seen during development
        // phase. When using WideGamutRGB and raising the lightness
        // slider up to 100%: Bug behaviour: the color switches
        // to 0% lightness. Expected behaviour: the color has almost
        // 100% lightness.
        auto myColorSpace = RgbColorSpace::createFromFile(wideGamutFile->fileName());
        const LchDouble temp{1, 0.151189, 359.374};
        QVERIFY(myColorSpace->reduceOklchChromaToFitIntoGamut(temp).l > 0.95);
    }

    void testDeleteTransformThatIsNull()
    {
        cmsHTRANSFORM myTransform = nullptr;
        RgbColorSpacePrivate::deleteTransform(&myTransform);
        QCOMPARE(myTransform, nullptr);
    }

    void testDeleteTransformThatIsValid()
    {
        // Initialization

        cmsHPROFILE myProfile = cmsCreate_sRGBProfile();
        QVERIFY(myProfile != nullptr); // assertion
        cmsHTRANSFORM myTransform = cmsCreateTransform(
            // Create a transform function and get a handle to this function:
            myProfile, // input profile handle
            TYPE_RGB_16, // output buffer format
            myProfile, // output profile handle
            TYPE_RGB_16, // output buffer format
            INTENT_ABSOLUTE_COLORIMETRIC, // rendering intent
            cmsFLAGS_NOCACHE // flags
        );
        QVERIFY(myTransform != nullptr); // assertion

        // Do the actual unit test
        RgbColorSpacePrivate::deleteTransform(&myTransform);
        QCOMPARE(myTransform, nullptr);

        // Clean-up
        if (myTransform != nullptr) {
            cmsDeleteTransform(myTransform);
        }
        if (myProfile != nullptr) {
            cmsCloseProfile(myProfile);
        }
    }

    void testProperties()
    {
        QScopedPointer<QTemporaryFile> wideGamutFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/Compact-ICC-Profiles/Compact-ICC-Profiles/profiles/WideGamutCompat-v4.icc")));
        if (wideGamutFile.isNull()) {
            throw 0;
        }

        auto srgb = RgbColorSpace::createSrgb();
        QCOMPARE(srgb.isNull(), false); // assertion
        auto widegamutrgb = RgbColorSpace::createFromFile( //
            wideGamutFile->fileName());
        QCOMPARE(widegamutrgb.isNull(), false); // assertion

        // Start testing

        QCOMPARE(srgb->profileAbsoluteFilePath(), QString());
        QVERIFY( //
            widegamutrgb->profileAbsoluteFilePath().endsWith( //
                wideGamutFile->fileName()));

        QCOMPARE(srgb->profileClass(), //
                 cmsProfileClassSignature::cmsSigDisplayClass);
        QCOMPARE(widegamutrgb->profileClass(), //
                 cmsProfileClassSignature::cmsSigDisplayClass);

        QCOMPARE(srgb->profileColorModel(), //
                 cmsColorSpaceSignature::cmsSigRgbData);
        QCOMPARE(widegamutrgb->profileColorModel(), //
                 cmsColorSpaceSignature::cmsSigRgbData);

        QCOMPARE(srgb->profileCopyright(), //
                 QStringLiteral("No copyright, use freely"));
        // No non-localized test data for widgegamutrgb

        QCOMPARE(srgb->profileCreationDateTime().isNull(), true);
        QCOMPARE(widegamutrgb->profileCreationDateTime(), //
                 QDateTime(QDate(2021, 04, 27), QTime(10, 27, 00), Qt::UTC));

        QVERIFY(srgb->profileFileSize() == -1);
        QCOMPARE(widegamutrgb->profileFileSize(), 464);

        QCOMPARE(srgb->profileHasMatrixShaper(), true);

        // No external test data for srgb profile
        QCOMPARE(widegamutrgb->profileIccVersion(), QVersionNumber(4, 2));

        // No external test data for srgb profile
        QCOMPARE(widegamutrgb->profileManufacturer(), QString());

        QVERIFY(isInRange<double>(0,
                                  widegamutrgb->profileMaximumCielchD50Chroma(), //
                                  CielchD50Values::maximumChroma));

        // The test for profileModel is missing, because
        // we have currently no external test data against which
        // we could test.

        // The test for profileName is missing, because
        // we have currently no external test data against which
        // we could test.

        // According to the ICC specification v4.4, only two color models
        // are allowed as PCS (for all profile classes except the device link
        // class):
        const QList validPcsModels = {cmsColorSpaceSignature::cmsSigLabData, //
                                      cmsColorSpaceSignature::cmsSigXYZData};
        // We have no reference data for the PCS color model of these
        // profiles. So instead, we test if it’s one of the allowed
        // values as described in the ICC specification.
        QVERIFY(validPcsModels.contains(srgb->profilePcsColorModel()));
        QVERIFY(validPcsModels.contains(widegamutrgb->profilePcsColorModel()));
    }

    void testProfileMaximumCielchD50Chroma()
    {
        auto temp = RgbColorSpace::createSrgb();
        LchDouble color;

        // Test if profileMaximumCielchD50Chroma is big enough
        qreal precisionDegreeMaxSrgbChroma = //
            0.1 / 360 * 2 * pi * temp->profileMaximumCielchD50Chroma();
        color.c = temp->profileMaximumCielchD50Chroma();
        qreal hue = 0;
        qreal lightness;
        qreal cielabPresicion = 0.1;
        while (hue <= 360) {
            color.h = hue;
            lightness = 0;
            while (lightness <= 100) {
                color.l = lightness;
                QVERIFY2(!temp->isCielchD50InGamut(color), "Test if profileMaximumCielchD50Chroma is big enough");
                lightness += cielabPresicion;
            }
            hue += precisionDegreeMaxSrgbChroma;
        }

        // Test if profileMaximumCielchD50Chroma is as small as possible
        color.c = temp->profileMaximumCielchD50Chroma() * 0.97;
        bool inGamutValueFound = false;
        hue = 0;
        while (hue <= 360) {
            color.h = hue;
            lightness = 0;
            while (lightness <= 100) {
                color.l = lightness;
                if (temp->isCielchD50InGamut(color)) {
                    inGamutValueFound = true;
                    break;
                }
                lightness += cielabPresicion;
            }
            if (inGamutValueFound) {
                break;
            }
            hue += precisionDegreeMaxSrgbChroma;
        }
        QVERIFY2(inGamutValueFound, //
                 "Test if profileMaximumCielchD50Chroma is as small as possible");
    }

    void testProfileMaximumOklchChroma()
    {
        auto temp = RgbColorSpace::createSrgb();
        LchDouble color;

        // Test if profileMaximumOklchChroma is big enough
        qreal precisionDegreeMaxSrgbChroma = //
            0.1 / 360 * 2 * pi * temp->profileMaximumOklchChroma() * 100;
        color.c = temp->profileMaximumOklchChroma();
        qreal hue = 0;
        qreal lightness;
        qreal oklabPrecision = 0.001;
        while (hue <= 360) {
            color.h = hue;
            lightness = 0;
            while (lightness <= 1) {
                color.l = lightness;
                QVERIFY2(!temp->isOklchInGamut(color), "Test if profileMaximumOklchChroma is big enough");
                lightness += oklabPrecision;
            }
            hue += precisionDegreeMaxSrgbChroma;
        }

        // Test if profileMaximumOklchChroma is as small as possible
        color.c = temp->profileMaximumOklchChroma() * 0.97;
        bool inGamutValueFound = false;
        hue = 0;
        while (hue <= 360) {
            color.h = hue;
            lightness = 0;
            while (lightness <= 1) {
                color.l = lightness;
                if (temp->isOklchInGamut(color)) {
                    inGamutValueFound = true;
                    break;
                }
                lightness += oklabPrecision;
            }
            if (inGamutValueFound) {
                break;
            }
            hue += precisionDegreeMaxSrgbChroma;
        }
        QVERIFY2(inGamutValueFound, //
                 "Test if profileMaximumOklchChroma is as small as possible");
    }

    void testToCielchD50Double()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
            // Create sRGB which is pretty much standard.
            PerceptualColor::RgbColorSpaceFactory::createSrgb();

        // Testing
        const QRgba64 white = QColor{255, 255, 255}.rgba64();
        const auto convertedWhite = myColorSpace->toCielchD50Double(white);
        QVERIFY(convertedWhite.l >= 99);
        QVERIFY(convertedWhite.l <= 100);
        QVERIFY(convertedWhite.c >= -1);
        QVERIFY(convertedWhite.c <= 1);
        // No test for hue because it would be meaningless.

        // Testing
        const QRgba64 black = QColor{0, 0, 0}.rgba64();
        const auto convertedBlack = myColorSpace->toCielchD50Double(black);
        QVERIFY(convertedBlack.l >= 0);
        QVERIFY(convertedBlack.l <= 1);
        QVERIFY(convertedBlack.c >= -1);
        QVERIFY(convertedBlack.c <= 1);
        // No test for hue because it would be meaningless.
    }

    void testToQRgbForce()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
            // Create sRGB which is pretty much standard.
            PerceptualColor::RgbColorSpaceFactory::createSrgb();

        // Variables
        LchDouble color;

        // In-gamut colors should work:
        color.l = 50;
        color.c = 20;
        color.h = 10;
        auto result = myColorSpace->fromCielchD50ToQRgbBound(color);
        QCOMPARE(qAlpha(result), 255); // opaque

        // Out-of-gamut colors should work:
        color.l = 100;
        color.c = 200;
        color.h = 10;
        result = myColorSpace->fromCielchD50ToQRgbBound(color);
        QCOMPARE(qAlpha(result), 255); // opaque

        // Out-of-boundary colors should work:
        color.l = 200;
        color.c = 300;
        color.h = 400;
        result = myColorSpace->fromCielchD50ToQRgbBound(color);
        QCOMPARE(qAlpha(result), 255); // opaque
    }

    void testIsCielchD50InGamut()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
            // Create sRGB which is pretty much standard.
            PerceptualColor::RgbColorSpaceFactory::createSrgb();

        // Variables
        LchDouble color;

        // In-gamut colors should work:
        color.l = 50;
        color.c = 20;
        color.h = 10;
        QCOMPARE(myColorSpace->isCielchD50InGamut(color), true);

        // Out-of-gamut colors should work:
        color.l = 100;
        color.c = 200;
        color.h = 10;
        QCOMPARE(myColorSpace->isCielchD50InGamut(color), false);

        // Out-of-boundary colors should work:
        color.l = 200;
        color.c = 300;
        color.h = 400;
        QCOMPARE(myColorSpace->isCielchD50InGamut(color), false);
    }

    void testIsOlkchInGamut()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
            // Create sRGB which is pretty much standard.
            PerceptualColor::RgbColorSpaceFactory::createSrgb();

        // Variables
        LchDouble color;

        // In-gamut colors should work:
        color.l = 0.5;
        color.c = 0.10;
        color.h = 10;
        QCOMPARE(myColorSpace->isOklchInGamut(color), true);

        // Out-of-gamut colors should work:
        color.l = 1;
        color.c = 0.3;
        color.h = 10;
        QCOMPARE(myColorSpace->isOklchInGamut(color), false);

        // Out-of-boundary colors should work:
        color.l = 200;
        color.c = 300;
        color.h = 400;
        QCOMPARE(myColorSpace->isOklchInGamut(color), false);
    }

    void testIsCielabD50InGamut()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
            // Create sRGB which is pretty much standard.
            PerceptualColor::RgbColorSpaceFactory::createSrgb();

        // Variables
        cmsCIELab color;

        // In-gamut colors should work:
        color.L = 50;
        color.a = 10;
        color.b = 10;
        QCOMPARE(myColorSpace->isCielabD50InGamut(color), true);

        // Out-of-gamut colors should work:
        color.L = 100;
        color.a = 100;
        color.b = 100;
        QCOMPARE(myColorSpace->isCielabD50InGamut(color), false);

        // Out-of-boundary colors should work:
        color.L = 200;
        color.a = 300;
        color.b = 300;
        QCOMPARE(myColorSpace->isCielabD50InGamut(color), false);
    }

    void testToQRgbOrTransparent()
    {
        QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
            // Create sRGB which is pretty much standard.
            PerceptualColor::RgbColorSpaceFactory::createSrgb();

        // Variables
        cmsCIELab color;

        // In-gamut colors should work:
        color.L = 50;
        color.a = 10;
        color.b = 10;
        QVERIFY(qAlpha(myColorSpace->fromCielabD50ToQRgbOrTransparent(color)) == 255);

        // Out-of-gamut colors should work:
        color.L = 100;
        color.a = 100;
        color.b = 100;
        QVERIFY(qAlpha(myColorSpace->fromCielabD50ToQRgbOrTransparent(color)) == 0);

        // Out-of-boundary colors should work:
        color.L = 200;
        color.a = 300;
        color.b = 300;
        QVERIFY(qAlpha(myColorSpace->fromCielabD50ToQRgbOrTransparent(color)) == 0);
    }

    // The following unit tests are a little bit special. They do not
    // actually test the functionality of getInformationFromProfile()
    // but rather if its character encoding converting approach works
    // reliable in all situations.
    //
    // LittleCMS returns wchar_t. This type might have different sizes,
    // depending on the  operating system either 16 bit or 32 bit.
    // LittleCMS does not specify the encoding in its documentation for
    // cmsGetProfileInfo() as of LittleCMS 2.9. It only says “Strings are
    // returned as wide chars.” So this is likely either UTF-16 or UTF-32.
    // According to github.com/mm2/Little-CMS/issues/180#issue-421837278
    // it is even UTF-16 when the size of wchar_t is 32 bit. And according
    // to github.com/mm2/Little-CMS/issues/180#issuecomment-1007490587
    // in LittleCMS versions after 2.13 it might be UTF-32 when the size
    // of wchar_t is 32 bit. So the behaviour of LittleCMS changes between
    // various versions. Conclusion: It’s either UTF-16 or UTF-32, but we
    // never know which it is and have to be prepared for all possible
    // combinations between UTF-16/UTF-32 and a wchar_t size of
    // 16 bit/32 bit.
    //
    // The code of getInformationFromProfile() relies on
    // QString::fromWCharArray() to handle also these non-standard encoding
    // situations, which it seems to do well, but this is unfortunately
    // not documented.
    //
    // Those unit tests can only test the behaviour for the wchar_t size
    // of the system on which it’s running. But for this wchar_t size
    // we test it well…

    void testGetInformationFromProfile1()
    {
        // Test UTF-16 single-code-unit codepoint
        // (identical to testing UTF-32 code points below U+10000)
        wchar_t *buffer = new wchar_t[2]; // Allocate the buffer
        // Initialize the buffer an UTF-16 encoding of “✂” who’s code point
        // is U+2702 and who’s UTF-16 representation is 0x2702,
        // followed by a null character.
        *(buffer + 0) = 0x2702;
        *(buffer + 1) = 0;
        const QString result = QString::fromWCharArray(
            // Convert to string with these parameters:
            buffer, // read from this buffer
            -1 // read until the first null element
        );
        // Free allocated memory of the buffer
        delete[] buffer;
        // Test if the resulting QString has valid data:
        QVERIFY(result.isValidUtf16());
        // Test if the count of UTF-16 code units is as expected:
        QCOMPARE(result.size(), 1);
        // Test if the content is exactly 1 code point (excluding
        // the null termination)
        QCOMPARE(result.toUcs4().size(), 1);
        // Test if the code point is correctly recognized:
        QCOMPARE(result.toUcs4().at(0), 0x2702);
    }

    void testGetInformationFromProfile2()
    {
        // Test UTF-16 surrogate pair
        wchar_t *buffer = new wchar_t[3]; // Allocate the buffer
        // Initialize the buffer an UTF-16 encoding of “🖌” who’s code point
        // is U+1F58C and who’s UTF-16 representation is 0xD83D 0xDD8C,
        // followed by a null character.
        *(buffer + 0) = 0xD83D;
        *(buffer + 1) = 0xDD8C;
        *(buffer + 2) = 0;
        const QString result = QString::fromWCharArray(
            // Convert to string with these parameters:
            buffer, // read from this buffer
            -1 // read until the first null element
        );
        // Free allocated memory of the buffer
        delete[] buffer;
        // Test if the resulting QString has valid data:
        QVERIFY(result.isValidUtf16());
        // Test if the count of UTF-16 code units is as expected:
        QCOMPARE(result.size(), 2);
        // Test if the content is exactly 1 code point (excluding
        // the null termination)
        QCOMPARE(result.toUcs4().size(), 1);
        // Test if the code point is correctly recognized:
        QCOMPARE(result.toUcs4().at(0), 0x1F58C);
    }

    void testGetInformationFromProfile3()
    {
        if constexpr (sizeof(wchar_t) >= 4) {
            // This test makes only sense when wchar_t has 32 bit (4 bytes).

            // Test UTF-32 value beyond U+10000
            wchar_t *buffer = new wchar_t[2]; // Allocate the buffer
            // Initialize the buffer an UTF-32 encoding of “🖌” who’s code point
            // is U+1F58C and who’s UTF-32 representation is 0x1F58C,
            // followed by a null character.
            *(buffer + 0) = 0x1F58C;
            *(buffer + 1) = 0;
            const QString result = QString::fromWCharArray(
                // Convert to string with these parameters:
                buffer, // read from this buffer
                -1 // read until the first null element
            );
            // Free allocated memory of the buffer
            delete[] buffer;
            // Test if the resulting QString has valid data:
            QVERIFY(result.isValidUtf16());
            // Test if the count of UTF-16 code units is as expected:
            QCOMPARE(result.size(), 2);
            // Test if the content is exactly 1 code point (excluding
            // the null termination)
            QCOMPARE(result.toUcs4().size(), 1);
            // Test if the code point is correctly recognized:
            QCOMPARE(result.toUcs4().at(0), 0x1F58C);
        }
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestRgbColorSpace)

// The following “include” is necessary because we do not use a header file:
#include "testrgbcolorspace.moc"
