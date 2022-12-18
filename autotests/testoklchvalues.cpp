// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "oklchvalues.h"

#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>

namespace PerceptualColor
{
class TestOklchValues : public QObject
{
    Q_OBJECT

public:
    TestOklchValues(QObject *parent = nullptr)
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

    void testMaximumChroma()
    {
        QVERIFY(OklchValues::maximumChroma > 0);
        // TODO xxx Load all profiles to see if none has a higher chroma!
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestOklchValues)
// The following “include” is necessary because we do not use a header file:
#include "testoklchvalues.moc"
