// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/xxx.h"
// Second, the private implementation.
#include "xxx_p.h" // IWYU pragma: keep

#include <QtTest>

namespace PerceptualColor
{
class TestXXX : public QObject
{
    Q_OBJECT

public:
    TestXXX(QObject *parent = nullptr)
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

    void testDef()
    {
        // XXX Implement me!
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestXXX)

// The following “include” is necessary because we do not use a header file:
#include "testxxx.moc"
