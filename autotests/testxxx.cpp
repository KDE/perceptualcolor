// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "xxx.h"
// Second, the private implementation.
#include "xxx_p.h" // IWYU pragma: keep

#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestXXX : public QObject
{
    Q_OBJECT

public:
    explicit TestXXX(QObject *parent = nullptr)
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

        // It is necessary to show the widget and make it active
        // to make focus and widget events working within unit tests.
        myWidget->show();
        QApplication::setActiveWindow(myWidget.data());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestXXX)

// The following “include” is necessary because we do not use a header file:
#include "testxxx.moc"
