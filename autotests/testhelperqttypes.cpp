// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helperqttypes.h"

#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>
#include <type_traits>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{

class TestHelperQtTypes : public QObject
{
    Q_OBJECT

public:
    explicit TestHelperQtTypes(QObject *parent = nullptr)
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

    void testQColorFloatType()
    {
        QVERIFY(std::is_floating_point_v<QColorFloatType>);
    }

    void testQListSizeType()
    {
        QVERIFY(std::is_integral_v<QListSizeType>);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperQtTypes)
// The following “include” is necessary because we do not use a header file:
#include "testhelperqttypes.moc"
