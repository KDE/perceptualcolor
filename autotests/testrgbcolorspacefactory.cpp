// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "rgbcolorspacefactory.h"

#include "chromahuediagram.h"
#include "colorwheel.h"
#include "rgbcolorspace.h"
#include "settranslation.h"
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlocale.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qlist.h>
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#include <qstringlist.h>
#endif

static void snippet01()
{
    //! [Create]
    QSharedPointer<PerceptualColor::RgbColorSpace> myColorSpace =
        // Create the color space object with the factory class.
        // This call might be slow.
        PerceptualColor::RgbColorSpaceFactory::createSrgb();

    // These calls are fast:

    PerceptualColor::ChromaHueDiagram *myDiagram =
        // Create a widget with the color space:
        new PerceptualColor::ChromaHueDiagram(myColorSpace);

    PerceptualColor::ColorWheel *myWheel =
        // Create another widget with the very same color space:
        new PerceptualColor::ColorWheel(myColorSpace);
    //! [Create]

    delete myDiagram;
    delete myWheel;
}

namespace PerceptualColor
{
class TestRgbColorSpaceFactory : public QObject
{
    Q_OBJECT

public:
    explicit TestRgbColorSpaceFactory(QObject *parent = nullptr)
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

    void testCreate()
    {
        setTranslation(QCoreApplication::instance(), //
                       QLocale(QLocale::English).uiLanguages());
        QSharedPointer<PerceptualColor::RgbColorSpace> temp = //
            RgbColorSpaceFactory::createSrgb();
        QCOMPARE(temp.isNull(), false); // This is no nullptr!
        // Make a random call, just to be sure that a method call won’t crash:
        Q_UNUSED(temp->profileName());
        // Make sure the returned value is actually the sRGB gamut.
        QCOMPARE(temp->profileName(), QStringLiteral("sRGB color space"));
    }

    void testSnipped01()
    {
        snippet01();
    }

    void testColorProfileDirectories()
    {
        // colorProfileDirectories() must not throw an exception:
        const QStringList temp = //
            RgbColorSpaceFactory::colorProfileDirectories();
        Q_UNUSED(temp)
    }

    void testColorProfileDirectoriesQInfo()
    {
        qInfo() << RgbColorSpaceFactory::colorProfileDirectories();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestRgbColorSpaceFactory)

// The following “include” is necessary because we do not use a header file:
#include "testrgbcolorspacefactory.moc"
