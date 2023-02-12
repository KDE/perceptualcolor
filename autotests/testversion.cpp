// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "version.h"

#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qversionnumber.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

static void snippet01()
{
//! [Version Macro usage]
// Either with macros:
#if (PERCEPTUALCOLOR_COMPILE_TIME_VERSION >= QT_VERSION_CHECK(1, 2, 3))
    // Some code…
#else
    // Some different code…
#endif

    // Or with C++ “if constexpr”. Works only if both(!) code paths can
    // potentially compile without errors at all versions:
    if constexpr (PERCEPTUALCOLOR_COMPILE_TIME_VERSION >= QT_VERSION_CHECK(1, 2, 3)) {
        // Some code…
    } else {
        // Some different code…
    }
    //! [Version Macro usage]
}

namespace PerceptualColor
{
class TestVersion : public QObject
{
    Q_OBJECT

public:
    explicit TestVersion(QObject *parent = nullptr)
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

    void testSnippet01()
    {
        snippet01();
    }
};

} // flags

QTEST_MAIN(PerceptualColor::TestVersion)

// The following “include” is necessary because we do not use a header file:
#include "testversion.moc"
