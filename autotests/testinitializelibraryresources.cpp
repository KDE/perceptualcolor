// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "initializelibraryresources.h"

#include <qdiriterator.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
class TestIinitializeLibraryResources : public QObject
{
    Q_OBJECT

public:
    explicit TestIinitializeLibraryResources(QObject *parent = nullptr)
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

    void testDoesNotCrash()
    {
        initializeLibraryResources();
    }

    void testMultipleCalls()
    {
        initializeLibraryResources();
        initializeLibraryResources();
    }

    void testResourcesActuallyAvailable()
    {
        initializeLibraryResources();

        // Resources are added in two steps in CMakeLists.txt. One step for the translation files and another one for the icons. Therefore, we test both here.

        // Test translation files
        static const QString qmFile = //
            QStringLiteral(":/PerceptualColor/i18n/localization.de.qm");
        QVERIFY(QFile::exists(qmFile));

        // Test icons
        static const QString svgFile = //
            QStringLiteral(":/PerceptualColor/icons/lighttheme/color-swatch.svg");
        QVERIFY(QFile::exists(svgFile));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestIinitializeLibraryResources)

// The following “include” is necessary because we do not use a header file:
#include "testinitializelibraryresources.moc"
