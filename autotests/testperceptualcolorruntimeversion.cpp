// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "perceptualcolorruntimeversion.h"

#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>
#include <qversionnumber.h>

namespace PerceptualColor
{
class TestPerceptualColorRunTimeVersion : public QObject
{
    Q_OBJECT

public:
    explicit TestPerceptualColorRunTimeVersion(QObject *parent = nullptr)
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

    void testPerceptualColorRunTimeVersion()
    {
        QVERIFY2(perceptualColorRunTimeVersion() >= QVersionNumber(0, 0, 1), "Verify that the version number is at least 0.0.1.");
        QVERIFY2(perceptualColorRunTimeVersion() < QVersionNumber(99, 0, 1), "Verify that the version number is not too big.");
    }
};

} // flags

QTEST_MAIN(PerceptualColor::TestPerceptualColorRunTimeVersion)

// The following “include” is necessary because we do not use a header file:
#include "testperceptualcolorruntimeversion.moc"
