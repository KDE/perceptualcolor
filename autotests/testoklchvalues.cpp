// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "oklchvalues.h"

#include "rgbcolorspace.h"
#include "rgbcolorspacefactory.h"
#include <qglobal.h>
#include <qobject.h>
#include <qsharedpointer.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestOklchValues : public QObject
{
    Q_OBJECT

public:
    explicit TestOklchValues(QObject *parent = nullptr)
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

        const auto mySpace = RgbColorSpaceFactory::createSrgb();
        const auto maxOkChroma = mySpace->profileMaximumOklchChroma();
        QVERIFY(OklchValues::maximumChroma > maxOkChroma);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestOklchValues)
// The following “include” is necessary because we do not use a header file:
#include "testoklchvalues.moc"
