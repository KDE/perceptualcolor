// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "screencolorpicker.h"

#include <optional>
#include <qglobal.h>
#include <qobject.h>
#include <qpointer.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#include <qmap.h>
#include <qtmetamacros.h>
#else
#include <qmetatype.h>
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
class TestScreenColorPicker : public QObject
{
    Q_OBJECT

public:
    explicit TestScreenColorPicker(QObject *parent = nullptr)
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

    void testIsAvailable()
    {
        ScreenColorPicker picker(nullptr);
        // Result depends on the platform. Make sure that at least is
        // does not crash.
        const bool result = picker.isAvailable();
        Q_UNUSED(result)
    }

    void testGetPortalResponse()
    {
        ScreenColorPicker picker(nullptr);
        // Difficult to test. Make sure that at least it does not crash.
        picker.getPortalResponse(1, QVariantMap());
    }

    void testInitializeQColorDialogSupport()
    {
        ScreenColorPicker picker(nullptr);
        // Difficult to test. Make sure that at least it does not crash.
        picker.initializeQColorDialogSupport();
        // There has to be at least a result (even if we do not know which)
        QVERIFY(picker.m_hasQColorDialogSupport.has_value());
        if (picker.m_hasQColorDialogSupport.has_value()) {
            QVERIFY(!picker.m_qColorDialogScreenButton.isNull());
            QVERIFY(!picker.m_qColorDialog.isNull());
        }
    }

    void testHasPortalSupport()
    {
        // Difficult to test. Make sure that at least it does not crash.
        const bool result = ScreenColorPicker::hasPortalSupport();
        Q_UNUSED(result)
    }

    void testQueryPortalSupport()
    {
        // Difficult to test. Make sure that at least it does not crash.
        const bool result = ScreenColorPicker::queryPortalSupport();
        Q_UNUSED(result)
    }

    void testTranslateViaQColorDialog()
    {
        // Difficult to test. Make sure that at least it does not crash.
        const auto result = ScreenColorPicker::translateViaQColorDialog("abcdefghijkl");
        Q_UNUSED(result)
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestScreenColorPicker)

// The following “include” is necessary because we do not use a header file:
#include "testscreencolorpicker.moc"
