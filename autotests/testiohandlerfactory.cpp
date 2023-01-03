// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "iohandlerfactory.h"

#include "lcms2_plugin.h"
#include <lcms2.h>
#include <qbytearray.h>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qscopedpointer.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtemporaryfile.h>
#include <qtest.h>
#include <qtestcase.h>

namespace PerceptualColor
{
class TestIOHandlerFactory : public QObject
{
    Q_OBJECT

public:
    explicit TestIOHandlerFactory(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
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

    void testExistingFile()
    {
        QScopedPointer<QTemporaryFile> testFile(
            // Create a temporary actual file…
            QTemporaryFile::createNativeFile(
                // …from the content of this resource:
                QStringLiteral(":/testbed/ascii-abcd.txt")));
        if (testFile.isNull()) {
            throw 0;
        }
        cmsIOHANDLER *myHandler = IOHandlerFactory::createReadOnly( //
            nullptr,
            testFile->fileName());

        QVERIFY(myHandler != nullptr);
        QCOMPARE(myHandler->ContextID, nullptr);
        QCOMPARE(myHandler->ReportedSize, 4);
        QCOMPARE(myHandler->UsedSpace, 0);
        // We do not check neither “stream” (implicitly covered by other
        // tests) nor ”PhysicalFile” (an unused implementation detail).

        QByteArray myByteArray(5, ' '); // Array of 5 bytes, each with value 0x00
        cmsUInt32Number readResult;
        cmsBool seekResult;
        cmsBool closeResult;
        cmsBool writeResult;

        myByteArray.fill(' ');
        readResult = myHandler->Read(myHandler, myByteArray.data(), 1, 2);
        QCOMPARE(readResult, 2);
        QCOMPARE(myByteArray, QByteArrayLiteral("ab   "));
        QCOMPARE(myHandler->Tell(myHandler), 2);

        myByteArray.fill(' ');
        readResult = myHandler->Read(myHandler, myByteArray.data(), 1, 2);
        QCOMPARE(readResult, 2);
        QCOMPARE(myByteArray, QByteArrayLiteral("cd   "));
        QCOMPARE(myHandler->Tell(myHandler), 4);

        myByteArray.fill(' ');
        QCOMPARE(myHandler->Tell(myHandler), 4); // Assertion
        // We are at the end of the file. The following read should not work:
        qInstallMessageHandler(voidMessageHandler); // suppress warnings
        readResult = myHandler->Read(myHandler, myByteArray.data(), 1, 2);
        qInstallMessageHandler(nullptr); // do not suppress warning anymore
        QCOMPARE(readResult, 0);
        QCOMPARE(myByteArray, QByteArrayLiteral("     "));
        QCOMPARE(myHandler->Tell(myHandler), 4);

        myByteArray.fill(' ');
        seekResult = myHandler->Seek(myHandler, 1);
        QCOMPARE(seekResult, true);
        readResult = myHandler->Read(myHandler, myByteArray.data(), 1, 2);
        QCOMPARE(readResult, 2);
        QCOMPARE(myByteArray, QByteArrayLiteral("bc   "));
        QCOMPARE(myHandler->Tell(myHandler), 3);

        myByteArray.fill(' ');
        seekResult = myHandler->Seek(myHandler, 1);
        QCOMPARE(seekResult, true);
        readResult = myHandler->Read(myHandler, myByteArray.data(), 1, 2);
        QCOMPARE(readResult, 2);
        QCOMPARE(myByteArray, QByteArrayLiteral("bc   "));
        QCOMPARE(myHandler->Tell(myHandler), 3);

        myByteArray.fill(' ');
        seekResult = myHandler->Seek(myHandler, 1);
        QCOMPARE(seekResult, true);
        QCOMPARE(myHandler->Tell(myHandler), 1);
        myHandler->Seek(myHandler, 8); // Out-of-range
        qInstallMessageHandler(voidMessageHandler); // suppress warnings
        readResult = myHandler->Read(myHandler, myByteArray.data(), 1, 2);
        qInstallMessageHandler(nullptr); // do not suppress warning anymore
        QCOMPARE(readResult, 0);
        QCOMPARE(myByteArray, QByteArrayLiteral("     "));

        myByteArray.fill('x');
        myHandler->Seek(myHandler, 1);
        QCOMPARE(myHandler->Tell(myHandler), 1); // Assertion
        writeResult = myHandler->Write(myHandler, 2, myByteArray.data());
        QCOMPARE(writeResult, false);
        QCOMPARE(myHandler->Tell(myHandler), 1);

        closeResult = myHandler->Close(myHandler);
        QCOMPARE(closeResult, true);
    }

    void testNonExisting()
    {
        cmsIOHANDLER *myHandler = IOHandlerFactory::createReadOnly( //
            nullptr,
            QStringLiteral("../testbed/nonexistingname"));
        QVERIFY(myHandler == nullptr);
    }

    void testDirectory1()
    {
        cmsIOHANDLER *myHandler = IOHandlerFactory::createReadOnly( //
            nullptr,
            // Try the name of a directory with trailing /
            QStringLiteral("../testbed/"));
        QVERIFY(myHandler == nullptr);
    }

    void testDirectory2()
    {
        cmsIOHANDLER *myHandler = IOHandlerFactory::createReadOnly( //
            nullptr,
            // Try the name of a directory without trailing /
            QStringLiteral("../testbed"));
        QVERIFY(myHandler == nullptr);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestIOHandlerFactory)

// The following “include” is necessary because we do not use a header file:
#include "testiohandlerfactory.moc"
