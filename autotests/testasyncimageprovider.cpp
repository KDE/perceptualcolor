// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "asyncimageprovider.h"

#include "asyncimagerendercallback.h"
#include <qglobal.h>
#include <qimage.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvariant.h>

//! [How to create]
// Provide a data type that contains
// all necessary data to render the image,
// and a rendering function.
struct MyImageParameters {
public:
    // Data members, that contain all necessary
    // information to render the image.
    int myFirstValue = 0; // Example
    int mySecondValue = 0; // Example

    // A public constructor, copy constructor
    // and destructor are required:
    MyImageParameters() = default;
    MyImageParameters(const MyImageParameters &) = default;
    ~MyImageParameters() = default;

    // Define also these functions recommended by “rule of five”:
    MyImageParameters &operator=(const MyImageParameters &) = default; // clazy:exclude=function-args-by-value
    MyImageParameters &operator=(MyImageParameters &&) = default;
    MyImageParameters(MyImageParameters &&) = default;

    // An “equal” operator is required:
    [[nodiscard]] bool operator==(const MyImageParameters other) const;

    // A thread-safe render function with exactly the
    // following signature is required:
    static void render(
        // A QVariant that will contain an object
        // of this very same type:
        const QVariant &variantParameters,
        // A callback object:
        PerceptualColor::AsyncImageRenderCallback &callbackObject);
};

// Use Q_DECLARE_METATYPE with this data type.
// Attention: This must be done outside of all name-spaces.
Q_DECLARE_METATYPE(MyImageParameters)

// Now you are ready to use the image provider:
class MyClass
{
    PerceptualColor::AsyncImageProvider<MyImageParameters> myImageProvider;
};
//! [How to create]

void MyImageParameters::render( //
    const QVariant &variantParameters, //
    PerceptualColor::AsyncImageRenderCallback &callbackObject)
{
    Q_UNUSED(variantParameters)
    Q_UNUSED(callbackObject.shouldAbort())
}

bool MyImageParameters::operator==(const MyImageParameters other) const
{
    return ((myFirstValue == other.myFirstValue) //
            && (mySecondValue == other.mySecondValue));
}

struct MockupParameters {
public:
    // Some values:
    int imageWidth = 1;
    bool produceCorrectImage = true;
    int arbitraryNumber = 0;
    // Equal operator is required:
    [[nodiscard]] bool operator==(const MockupParameters other) const
    {
        return ((imageWidth == other.imageWidth) //
                && (arbitraryNumber == other.arbitraryNumber) //
                && (produceCorrectImage == other.produceCorrectImage));
    }
    static void render( //
        const QVariant &variantParameters, //
        PerceptualColor::AsyncImageRenderCallback &callbackObject)
    {
        Q_UNUSED(variantParameters)
        Q_UNUSED(callbackObject)
    }
};
Q_DECLARE_METATYPE(MockupParameters)

namespace PerceptualColor
{
class TestAsyncImageProvider : public QObject
{
    Q_OBJECT

public:
    explicit TestAsyncImageProvider(QObject *parent = nullptr)
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

    void testMakeSureTheSnippetCorrectlyInstanciatesTheTemplate()
    {
        MyClass temp;
    }

    void testConstructorDestructor()
    {
        // Make sure that constructor and destructor do not crash:
        AsyncImageProvider<MockupParameters> test;
    }

    void testOnExampleImplementationNoCrashGetCache()
    {
        AsyncImageProvider<MockupParameters> image;
        Q_UNUSED(image.getCache());
    }

    void testOnExampleImplementationNoCrashImageParameters()
    {
        AsyncImageProvider<MockupParameters> image;
        Q_UNUSED(image.imageParameters());
    }

    void testOnExampleImplementationNoCrashRefreshAsync()
    {
        AsyncImageProvider<MockupParameters> image;
        image.refreshAsync();
    }

    void testOnExampleImplementationNoCrashRefreshSync()
    {
        AsyncImageProvider<MockupParameters> image;
        image.refreshSync();
    }

    void testOnExampleImplementationNoCrashRefreshAsynchSyncMix()
    {
        AsyncImageProvider<MockupParameters> image;
        image.refreshAsync();
        image.refreshSync();
    }

    void testOnExampleImplementationNoCrashRefreshAsynchSyncMixMultiple()
    {
        AsyncImageProvider<MockupParameters> image;
        image.refreshAsync();
        image.refreshSync();
        image.refreshAsync();
        image.refreshSync();
    }

    void testOnExampleImplementationNoCrashSetImageParameters()
    {
        AsyncImageProvider<MockupParameters> image;
        MockupParameters parameters;
        image.setImageParameters(parameters);
    }

    void testOnExampleImplementationNoCrashProcessInterlacingPassResult()
    {
        AsyncImageProvider<MockupParameters> image;
        image.processInterlacingPassResult(QImage{});
    }

    void testImageParameters()
    {
        AsyncImageProvider<MockupParameters> image;
        MockupParameters parameters;
        parameters.imageWidth = 3;
        image.setImageParameters(parameters);
        QCOMPARE(image.imageParameters(), parameters);
        parameters.imageWidth = 4;
        image.setImageParameters(parameters);
        QCOMPARE(image.imageParameters(), parameters);
    }

    void testDefaultCacheContent()
    {
        AsyncImageProvider<MockupParameters> image;
        // Cache is expected to be empty at startup:
        QVERIFY(image.getCache().isNull());
    }

    void testGetCache()
    {
        AsyncImageProvider<MockupParameters> test;
        Q_UNUSED(test.getCache())
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestAsyncImageProvider)

// The following “include” is necessary because we do not use a header file:
#include "testasyncimageprovider.moc"
