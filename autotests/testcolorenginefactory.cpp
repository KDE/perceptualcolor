// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "colorenginefactory.h"

#include "chromahuediagram.h"
#include "colorengine.h"
#include "colorwheel.h"
#include "settranslation.h"
#include <qcontainerfwd.h>
#include <qcoreapplication.h>
#include <qdebug.h>
#include <qglobal.h>
#include <qlist.h>
#include <qlocale.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

static void snippet01()
{
    //! [Create]
    QSharedPointer<PerceptualColor::ColorEngine> myColorEngine =
        // Create the color engine object with the factory class.
        // This call might be slow.
        PerceptualColor::createSrgbColorEngine();

    // These calls are fast:

    PerceptualColor::ChromaHueDiagram *myDiagram =
        // Create a widget that uses the color engine:
        new PerceptualColor::ChromaHueDiagram(myColorEngine);

    PerceptualColor::ColorWheel *myWheel =
        // Create another widget that uses the very same color engine:
        new PerceptualColor::ColorWheel(myColorEngine);
    //! [Create]

    delete myDiagram;
    delete myWheel;
}

namespace PerceptualColor
{
class TestColorEngineFactory : public QObject
{
    Q_OBJECT

public:
    explicit TestColorEngineFactory(QObject *parent = nullptr)
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
        QSharedPointer<PerceptualColor::ColorEngine> temp = //
            createSrgbColorEngine();
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
            colorProfileDirectories();
        Q_UNUSED(temp)
    }

    void testColorProfileDirectoriesQInfo()
    {
        qInfo() << colorProfileDirectories();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorEngineFactory)

// The following “include” is necessary because we do not use a header file:
#include "testcolorenginefactory.moc"
