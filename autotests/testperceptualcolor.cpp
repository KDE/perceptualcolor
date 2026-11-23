// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor"

#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
class TestPerceptualColor : public QObject
{
    Q_OBJECT

public:
    explicit TestPerceptualColor(QObject *parent = nullptr)
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

    // NOTE No actual unit tests here. By compiling this test, we just make
    // sure that the module header "PerceptualColor" compiles correctly.
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestPerceptualColor)

// The following “include” is necessary because we do not use a header file:
#include "testperceptualcolor.moc"
