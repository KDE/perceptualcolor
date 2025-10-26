// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helperconversion.h"
#include <lcms2.h>
#include <qglobal.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qtest.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

Q_DECLARE_METATYPE(cmsCIELab)

namespace PerceptualColor
{

class TestHelperConversion : public QObject
{
    Q_OBJECT

public:
    explicit TestHelperConversion(QObject *parent = nullptr)
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

    void testFromFloatingToEightBit()
    {
        constexpr quint8 value = fromFloatingToEightBit(1.0);
        static_assert(value == 255);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperConversion)
// The following “include” is necessary because we do not use a header file:
#include "testhelperconversion.moc"
