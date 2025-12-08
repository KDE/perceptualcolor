// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "colorengine.h"
// Second, the private implementation.
#include "colorengine_p.h" // IWYU pragma: keep

#include "cielchd50values.h"
#include "colorenginefactory.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "helpermath.h"
#include "helperposixmath.h"
#include <lcms2.h>
#include <map>
#include <optional>
#include <qbytearray.h>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qglobal.h>
#include <qlist.h>
#include <qobject.h>
#include <qrgb.h>
#include <qrgba64.h>
#include <qscopedpointer.h>
#include <qsharedpointer.h>
#include <qstring.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qtemporarydir.h>
#include <qtemporaryfile.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtimezone.h>
#include <qversionnumber.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestColorEngine : public QObject
{
    Q_OBJECT

public:
    explicit TestColorEngine(QObject *parent = nullptr)
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
        ColorEngine myColorEngine;
    }

    void testCreateSrgb()
    {
        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine;

        myColorEngine = ColorEngine::createSrgb();
        QCOMPARE(myColorEngine.isNull(), false);

        QVERIFY(isInRange<qreal>(0, myColorEngine->d_pointer->m_cielabD50BlackpointL, 1));
        QVERIFY( //
            isInRange<qreal>(99, myColorEngine->d_pointer->m_cielabD50WhitepointL, 100));

        QVERIFY(isInRange<qreal>(0.00, myColorEngine->d_pointer->m_oklabBlackpointL, 0.01));
        QVERIFY( //
            isInRange<qreal>(0.99, myColorEngine->d_pointer->m_oklabWhitepointL, 1.00));
    }

    void testTryCreateFromFile()
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

        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine;

        // Invalid file
        QVERIFY(QFileInfo::exists(invalidFile->fileName())); // assertion
        myColorEngine = ColorEngine::tryCreateFromFile( //
            invalidFile->fileName());
        QCOMPARE(myColorEngine.isNull(), true);

        // Non-existing file/directory name
        QString nonexistingfilename = //
            QStringLiteral("/nonexistingfilename.txt");
        QCOMPARE(QFileInfo::exists(nonexistingfilename), false); // assertion
        QCOMPARE(QDir{nonexistingfilename}.exists(), false); // assertion
        myColorEngine = ColorEngine::tryCreateFromFile( //
            nonexistingfilename);
        QCOMPARE(myColorEngine.isNull(), true);

        // Existing folder with trailing slash
        myColorEngine = ColorEngine::tryCreateFromFile( //
            existingDirectoryWithoutTrailingSlash.path() + QStringLiteral("/"));
        QCOMPARE(myColorEngine.isNull(), true);

        // Existing folder without trailing slash
        myColorEngine = ColorEngine::tryCreateFromFile( //
            existingDirectoryWithoutTrailingSlash.path());
        QCOMPARE(myColorEngine.isNull(), true);

        // Valid RGB profile (should load correctly)
        QVERIFY(QFileInfo::exists(validRgbFile->fileName())); // assertion
        myColorEngine = ColorEngine::tryCreateFromFile( //
            validRgbFile->fileName());
        QCOMPARE(myColorEngine.isNull(), false);
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

        auto myColorEngine = ColorEngine::tryCreateFromFile( //
            wideGamutFile->fileName());
        QCOMPARE(myColorEngine.isNull(), false); // assertion
        // assertion that maximum lightness is out-of-gamut for this profile:
        QCOMPARE(myColorEngine->isCielchD50InGamut(GenericColor{100, 0, 0}), false);
        QCOMPARE(myColorEngine->isOklchInGamut(GenericColor{1, 0, 0}), false);

        // Actual test:
        QVERIFY(isInRange<qreal>(0, myColorEngine->d_pointer->m_cielabD50BlackpointL, 1));
        QVERIFY( //
            isInRange<qreal>(99, myColorEngine->d_pointer->m_cielabD50WhitepointL, 100));

        QVERIFY(isInRange<qreal>(0.00, myColorEngine->d_pointer->m_oklabBlackpointL, 0.01));
        QVERIFY( //
            isInRange<qreal>(0.99, myColorEngine->d_pointer->m_oklabWhitepointL, 1.00));
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

        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine;
        myColorEngine = ColorEngine::tryCreateFromFile( //
            wideGamutFile->fileName());
        QCOMPARE(myColorEngine.isNull(), false); // assertion
        const GenericColor referenceColor{100, 50, 0};
        QCOMPARE(myColorEngine->isCielchD50InGamut(referenceColor), false); // assertion
        // The value referenceColor is out-of-gamut because WideGamutRGB stops
        // just a little bit before the lightness of 100.

        // Now, test how this special situation is handled:
        const GenericColor modifiedColor = //
            myColorEngine->reduceCielchD50ChromaToFitIntoGamut(referenceColor);
        QVERIFY(modifiedColor.second <= referenceColor.second);
        QCOMPARE(modifiedColor.third, referenceColor.third);
        QVERIFY(isInRange<qreal>(99, modifiedColor.first, 100));
        QVERIFY(modifiedColor.first < 100);
        QVERIFY(myColorEngine->isCielchD50InGamut(modifiedColor));
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
        auto myColorEngine = ColorEngine::tryCreateFromFile( //
            wideGamutFile->fileName());
        GenericColor temp{100, 50, 0};
        QVERIFY(myColorEngine->reduceCielchD50ChromaToFitIntoGamut(temp).first > 95);
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

        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine;
        myColorEngine = ColorEngine::tryCreateFromFile( //
            wideGamutFile->fileName());
        QCOMPARE(myColorEngine.isNull(), false); // assertion
        const GenericColor referenceColor{1, 0.151189, 359.374};
        QCOMPARE(myColorEngine->isOklchInGamut(referenceColor), false); // assertion
        // The value referenceColor is out-of-gamut because WideGamutRGB stops
        // just a little bit before the lightness of 100.

        // Now, test how this special situation is handled:
        const GenericColor modifiedColor = //
            myColorEngine->reduceOklchChromaToFitIntoGamut(referenceColor);
        QVERIFY(modifiedColor.second <= referenceColor.second);
        QCOMPARE(modifiedColor.third, referenceColor.third);
        QVERIFY(isInRange<qreal>(0.99, modifiedColor.first, 1));
        QVERIFY(modifiedColor.first < 1);
        QVERIFY(myColorEngine->isOklchInGamut(modifiedColor));
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
        auto myColorEngine = ColorEngine::tryCreateFromFile( //
            wideGamutFile->fileName());
        const GenericColor temp{1, 0.151189, 359.374};
        QVERIFY(myColorEngine->reduceOklchChromaToFitIntoGamut(temp).first > 0.95);
    }

    void testDeleteTransformThatIsNull()
    {
        cmsHTRANSFORM myTransform = nullptr;
        ColorEnginePrivate::deleteTransform(&myTransform);
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
        ColorEnginePrivate::deleteTransform(&myTransform);
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

        auto srgb = ColorEngine::createSrgb();
        QCOMPARE(srgb.isNull(), false); // assertion
        auto widegamutrgb = ColorEngine::tryCreateFromFile( //
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
        QCOMPARE( //
            widegamutrgb->profileCreationDateTime(), //
            QDateTime(QDate(2021, 04, 27), QTime(10, 27, 00), QTimeZone(0)));

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
        auto temp = ColorEngine::createSrgb();
        GenericColor color;

        // Test if profileMaximumCielchD50Chroma is big enough
        qreal precisionDegreeMaxSrgbChroma = //
            0.1 / 360 * 2 * pi * temp->profileMaximumCielchD50Chroma();
        color.second = temp->profileMaximumCielchD50Chroma();
        qreal hue = 0;
        qreal lightness;
        qreal cielabPresicion = 0.1;
        while (hue <= 360) {
            color.third = hue;
            lightness = 0;
            while (lightness <= 100) {
                color.first = lightness;
                QVERIFY2(!temp->isCielchD50InGamut(color), "Test if profileMaximumCielchD50Chroma is big enough");
                lightness += cielabPresicion;
            }
            hue += precisionDegreeMaxSrgbChroma;
        }

        // Test if profileMaximumCielchD50Chroma is as small as possible
        color.second = temp->profileMaximumCielchD50Chroma() * 0.97;
        bool inGamutValueFound = false;
        hue = 0;
        while (hue <= 360) {
            color.third = hue;
            lightness = 0;
            while (lightness <= 100) {
                color.first = lightness;
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
        auto temp = ColorEngine::createSrgb();
        GenericColor color;

        // Test if profileMaximumOklchChroma is big enough
        qreal precisionDegreeMaxSrgbChroma = //
            0.1 / 360 * 2 * pi * temp->profileMaximumOklchChroma() * 100;
        color.second = temp->profileMaximumOklchChroma();
        qreal hue = 0;
        qreal lightness;
        qreal oklabPrecision = 0.001;
        while (hue <= 360) {
            color.third = hue;
            lightness = 0;
            while (lightness <= 1) {
                color.first = lightness;
                QVERIFY2(!temp->isOklchInGamut(color), "Test if profileMaximumOklchChroma is big enough");
                lightness += oklabPrecision;
            }
            hue += precisionDegreeMaxSrgbChroma;
        }

        // Test if profileMaximumOklchChroma is as small as possible
        color.second = temp->profileMaximumOklchChroma() * 0.97;
        bool inGamutValueFound = false;
        hue = 0;
        while (hue <= 360) {
            color.third = hue;
            lightness = 0;
            while (lightness <= 1) {
                color.first = lightness;
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

    void testProfileTagSignatures()
    {
        auto temp = ColorEngine::createSrgb();

        const QStringList signatures = temp->profileTagSignatures();

        QVERIFY2(signatures.contains(QStringLiteral("wtpt")), //
                 "Test if wtpt tag of build-in profile is in tag list.");
    }

    void testProfileTagWhitepoint()
    {
        auto temp = ColorEngine::createSrgb();

        const std::optional<cmsCIEXYZ> maybeWhitepoint = temp->profileTagWhitepoint();

        QVERIFY2(maybeWhitepoint.has_value(), //
                 "Test if wtpt tag of build-in profile is available.");

        const cmsCIEXYZ whitepoint = maybeWhitepoint.value();

        cmsCIEXYZ referenceWhitepoint;
        referenceWhitepoint.X = 0.9642;
        referenceWhitepoint.Y = 1;
        referenceWhitepoint.Z = 0.8249;

        QCOMPARE(whitepoint.X, referenceWhitepoint.X);
        QCOMPARE(whitepoint.Y, referenceWhitepoint.Y);
        QCOMPARE(whitepoint.Z, referenceWhitepoint.Z);
    }

    void testToCielchD50Double()
    {
        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        // Testing
        const QRgba64 white = QColor{255, 255, 255}.rgba64();
        const auto convertedWhite = myColorEngine->toCielchD50(white);
        const auto bufferWhiteL = QStringLiteral("convertedWhite.first: %1") //
                                      .arg(convertedWhite.first, 0, 'e') //
                                      .toUtf8();
        QVERIFY2(convertedWhite.first >= 99, bufferWhiteL.constData());
        QVERIFY2(convertedWhite.first <= 100, bufferWhiteL.constData());
        const auto bufferWhiteC = QStringLiteral("convertedWhite.second: %1") //
                                      .arg(convertedWhite.second, 0, 'e') //
                                      .toUtf8();
        QVERIFY2(convertedWhite.second >= -1, bufferWhiteC.constData());
        QVERIFY2(convertedWhite.second <= 1, bufferWhiteC.constData());
        // No test for hue because it would be meaningless.

        // Testing
        const QRgba64 black = QColor{0, 0, 0}.rgba64();
        const auto convertedBlack = myColorEngine->toCielchD50(black);
        const auto bufferBlackL = QStringLiteral("convertedBlack.first: %1") //
                                      .arg(convertedBlack.first, 0, 'e') //
                                      .toUtf8();
        QVERIFY2(convertedBlack.first >= 0, bufferBlackL.constData());
        QVERIFY2(convertedBlack.first <= 1, bufferBlackL.constData());
        const auto bufferBlackC = QStringLiteral("convertedBlack.second: %1") //
                                      .arg(convertedBlack.second, 0, 'e') //
                                      .toUtf8();
        QVERIFY2(convertedBlack.second >= -1, bufferBlackC.constData());
        QVERIFY2(convertedBlack.second <= 1, bufferBlackC.constData());
        // No test for hue because it would be meaningless.
    }

    void testToQRgbForce()
    {
        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        // Variables
        GenericColor color;

        // In-gamut colors should work:
        color.first = 50;
        color.second = 20;
        color.third = 10;
        auto result = myColorEngine->fromCielchD50ToQRgbBound(color);
        QCOMPARE(qAlpha(result), 255); // opaque

        // Out-of-gamut colors should work:
        color.first = 100;
        color.second = 200;
        color.third = 10;
        result = myColorEngine->fromCielchD50ToQRgbBound(color);
        QCOMPARE(qAlpha(result), 255); // opaque

        // Out-of-boundary colors should work:
        color.first = 200;
        color.second = 300;
        color.third = 400;
        result = myColorEngine->fromCielchD50ToQRgbBound(color);
        QCOMPARE(qAlpha(result), 255); // opaque
    }

    void testIsCielchD50InGamut()
    {
        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        // Variables
        GenericColor color;

        // In-gamut colors should work:
        color.first = 50;
        color.second = 20;
        color.third = 10;
        QCOMPARE(myColorEngine->isCielchD50InGamut(color), true);

        // Out-of-gamut colors should work:
        color.first = 100;
        color.second = 200;
        color.third = 10;
        QCOMPARE(myColorEngine->isCielchD50InGamut(color), false);

        // Out-of-boundary colors should work:
        color.first = 200;
        color.second = 300;
        color.third = 400;
        QCOMPARE(myColorEngine->isCielchD50InGamut(color), false);
    }

    void testIsOklchInGamut()
    {
        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        // Variables
        GenericColor color;

        // In-gamut colors should work:
        color.first = 0.5;
        color.second = 0.10;
        color.third = 10;
        QCOMPARE(myColorEngine->isOklchInGamut(color), true);

        // Out-of-gamut colors should work:
        color.first = 1;
        color.second = 0.3;
        color.third = 10;
        QCOMPARE(myColorEngine->isOklchInGamut(color), false);

        // Out-of-boundary colors should work:
        color.first = 200;
        color.second = 300;
        color.third = 400;
        QCOMPARE(myColorEngine->isOklchInGamut(color), false);
    }

    void testIsCielabD50InGamut()
    {
        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        // Variables
        cmsCIELab color;

        // In-gamut colors should work:
        color.L = 50;
        color.a = 10;
        color.b = 10;
        QCOMPARE(myColorEngine->isCielabD50InGamut(color), true);

        // Out-of-gamut colors should work:
        color.L = 100;
        color.a = 100;
        color.b = 100;
        QCOMPARE(myColorEngine->isCielabD50InGamut(color), false);

        // Out-of-boundary colors should work:
        color.L = 200;
        color.a = 300;
        color.b = 300;
        QCOMPARE(myColorEngine->isCielabD50InGamut(color), false);
    }

    void testToQRgbOrTransparent()
    {
        QSharedPointer<PerceptualColor::ColorEngine> myColorEngine =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        // Variables
        cmsCIELab color;

        // In-gamut colors should work:
        color.L = 50;
        color.a = 10;
        color.b = 10;
        QVERIFY(qAlpha(myColorEngine->fromCielabD50ToQRgbOrTransparent(color)) == 255);

        // Out-of-gamut colors should work:
        color.L = 100;
        color.a = 100;
        color.b = 100;
        QVERIFY(qAlpha(myColorEngine->fromCielabD50ToQRgbOrTransparent(color)) == 0);

        // Out-of-boundary colors should work:
        color.L = 200;
        color.a = 300;
        color.b = 300;
        QVERIFY(qAlpha(myColorEngine->fromCielabD50ToQRgbOrTransparent(color)) == 0);
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
            *(buffer + 0) = static_cast<wchar_t>(0x1F58C);
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

    void testChromaticityBoundaryByCielchD50Hue360()
    {
        QSharedPointer<PerceptualColor::ColorEngine> temp =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        const auto colorCount = //
            temp->d_pointer->m_chromaticityBoundaryByCielchD50Hue360.size();
        // Six 8-bit color blocks, minus 6 duplicates where the blocks touch
        // each other, plus 2 duplicates at the lower and upper range.
        QCOMPARE(colorCount, 256 * 6 - 6 + 2);
    }

    void testChromaticityBoundaryByOklabHue360()
    {
        QSharedPointer<PerceptualColor::ColorEngine> temp =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        const auto colorCount = //
            temp->d_pointer->m_chromaticityBoundaryByOklabHue360.size();
        // Six 8-bit color blocks, minus 6 duplicates where the blocks touch
        // each other, plus 2 duplicates at the lower and upper range.
        QCOMPARE(colorCount, 256 * 6 - 6 + 2);
    }

    void testMaxChromaColorBy()
    {
        QSharedPointer<PerceptualColor::ColorEngine> tmp =
            // Create sRGB which is pretty much standard.
            PerceptualColor::createSrgbColorEngine();

        auto onChromaBoundary = [](const QColor &c) -> bool {
            const bool has0 = //
                (c.red() == 0) || (c.green() == 0) || (c.blue() == 0);
            const bool has255 = //
                (c.red() == 255) || (c.green() == 255) || (c.blue() == 255);
            return has0 && has255;
        };
        QVERIFY2(onChromaBoundary(tmp->maxChromaColorByCielchD50Hue360(0)), //
                 "Has to return a color on the chromaticity boundary.");
        QVERIFY2(onChromaBoundary(tmp->maxChromaColorByCielchD50Hue360(180)), //
                 "Has to return a color on the chromaticity boundary.");
        QVERIFY2(onChromaBoundary(tmp->maxChromaColorByCielchD50Hue360(360)), //
                 "Has to return a color on the chromaticity boundary.");

        QVERIFY2(onChromaBoundary(tmp->maxChromaColorByOklabHue360(0)), //
                 "Has to return a color on the chromaticity boundary.");
        QVERIFY2(onChromaBoundary(tmp->maxChromaColorByOklabHue360(180)), //
                 "Has to return a color on the chromaticity boundary.");
        QVERIFY2(onChromaBoundary(tmp->maxChromaColorByOklabHue360(360)), //
                 "Has to return a color on the chromaticity boundary.");
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorEngine)

// The following “include” is necessary because we do not use a header file:
#include "testcolorengine.moc"
