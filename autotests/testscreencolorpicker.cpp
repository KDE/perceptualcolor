// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "screencolorpicker.h"

#include <qcontainerfwd.h>
#include <qglobal.h>
#include <qguiapplication.h>
#include <qmap.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpointer.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

#ifdef PERCEPTUALCOLORLIB_STATIC
#include <optional>
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

#ifdef PERCEPTUALCOLORLIB_STATIC
        // NOTE The variable m_hasQColorDialogSupport is defined as “static
        // inline … = …;”. This decision has caused issues when
        // perceptualcolorinternal is used as a shared/dynamic
        // library, and the value of m_hasQColorDialogSupport is
        // accessed from  outside. Specifically, on MSVC, incorrect
        // values are returned during variable access from outside, likely
        // due to MSVC creating another  concurrent variable within the unit
        // test executable, while initializeQColorDialogSupport modifies
        // the variable within the dll. However, in general, having “static
        // inline … = …;” variables is acceptable if they are
        // private class members, as nobody can access them. Consequently,
        // this part of the unit test is intended for static builds only.

        // There has to be at least a result (even if we do not know which)
        QVERIFY(picker.m_hasQColorDialogSupport.has_value());

        // Tough future code changes in Qt could break our
        // QColorDialog support, it is a good idea to check
        // here if the QColorDialog support does actually work,
        // so we might get at least alerts by failing unit tests.
        if (picker.m_hasQColorDialogSupport.has_value()) {
            QCOMPARE(picker.m_hasQColorDialogSupport.value(), true);
        }
#endif

        const QString name = QGuiApplication::platformName();
        if (!name.contains(QStringLiteral("wayland"), Qt::CaseInsensitive)) {
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
