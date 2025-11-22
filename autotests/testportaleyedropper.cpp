// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "portaleyedropper.h"

#include <qcontainerfwd.h>
#include <qglobal.h>
#include <qmap.h>
#include <qobject.h>
#include <qtest.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

#ifdef PERCEPTUALCOLORLIB_STATIC
#include <optional>
#endif

namespace PerceptualColor
{
class TestPortalEyedropper : public QObject
{
    Q_OBJECT

public:
    explicit TestPortalEyedropper(QObject *parent = nullptr)
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

    void testGetInstance()
    {
        // This should not crash.
        const auto &myEyedropper = PortalEyedropper::getInstance();
        Q_UNUSED(myEyedropper)
    }

    void testIsAvailable()
    {
        // Result depends on the platform. Make sure that at least is
        // does not crash.
        auto &myEyedropper = PortalEyedropper::getInstance();
        const auto result = myEyedropper.isAvailable();
        Q_UNUSED(result)
    }

    void testGetPortalResponse()
    {
        auto &myEyedropper = PortalEyedropper::getInstance();
        // Difficult to test. Make sure that at least it does not crash.
        myEyedropper.getPortalResponse(1, QVariantMap());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestPortalEyedropper)

// The following “include” is necessary because we do not use a header file:
#include "testportaleyedropper.moc"
