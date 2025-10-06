// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helperaccessibility.h"

#include <qaccessiblewidget.h>
#include <qglobal.h>
#include <qobject.h>
#include <qscopedpointer.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

class QAccessibleInterface;

class MockupWidget : public QWidget
{
    Q_OBJECT
};

class MockupAccessibleWidget : public QAccessibleWidget
{
public:
    // clang-format off
    explicit MockupAccessibleWidget(QWidget *widget)
        : QAccessibleWidget(widget) { };
    // clang-format on
    virtual ~MockupAccessibleWidget() = default;
};

namespace PerceptualColor
{

class MockupWidgetInNamespace : public QWidget
{
    Q_OBJECT
};

class MockupAccessibleWidgetInNamespace : public QAccessibleWidget
{
public:
    // clang-format off
    explicit MockupAccessibleWidgetInNamespace(QWidget *widget)
        : QAccessibleWidget(widget) { };
    // clang-format on
    virtual ~MockupAccessibleWidgetInNamespace() = default;
};

class TestHelperAccessibility : public QObject
{
    Q_OBJECT

public:
    explicit TestHelperAccessibility(QObject *parent = nullptr)
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

    void testAccessibleFactory()
    {
        QScopedPointer<MockupWidget> myMockupWidget(new MockupWidget);
        QScopedPointer<QWidget> myWidget(new QWidget);

        // NOTE: The remainder of this function contains memory leaks.
        // The function under test returns a pointer to a QAccessibleInterface
        // object, which has a protected destructor and therefore cannot be
        // deleted. In real-world usage, this function is not called by library
        // users directly; it is used internally by Qt, which likely creates
        // these objects only once and manages their lifetime appropriately.
        // Despite the leak, we include this unit test to verify behavior.
        // Since this is test code and not production code, the leak is
        // acceptable. We explicitly acknowledge it here and suppress static
        // analysis warnings.

        // NOLINTBEGIN

        QAccessibleInterface const *const pointer01 = //
            accessibleFactory<MockupWidget, MockupAccessibleWidget>( //
                QStringLiteral("MockupWidget"), //
                myMockupWidget.data());
        QVERIFY(pointer01 != nullptr);

        QAccessibleInterface const *const pointer02 = //
            accessibleFactory<MockupWidget, MockupAccessibleWidget>( //
                QStringLiteral("abc"), //
                myMockupWidget.data());
        QVERIFY(pointer02 == nullptr);

        QAccessibleInterface const *const pointer03 = //
            accessibleFactory<MockupWidget, MockupAccessibleWidget>( //
                QStringLiteral("MockupWidget"), //
                myWidget.data());
        QVERIFY(pointer03 == nullptr);

        // NOLINTEND
    }

    void testAccessibleFactoryInNamespace()
    {
        QScopedPointer<MockupWidgetInNamespace> myMockupWidget(new MockupWidgetInNamespace);
        QScopedPointer<QWidget> myWidget(new QWidget);

        // NOTE: The remainder of this function contains memory leaks.
        // The function under test returns a pointer to a QAccessibleInterface
        // object, which has a protected destructor and therefore cannot be
        // deleted. In real-world usage, this function is not called by library
        // users directly; it is used internally by Qt, which likely creates
        // these objects only once and manages their lifetime appropriately.
        // Despite the leak, we include this unit test to verify behavior.
        // Since this is test code and not production code, the leak is
        // acceptable. We explicitly acknowledge it here and suppress static
        // analysis warnings.

        // NOLINTBEGIN

        QAccessibleInterface const *const pointer01 = //
            accessibleFactory<MockupWidgetInNamespace, MockupAccessibleWidgetInNamespace>( //
                QStringLiteral("PerceptualColor::MockupWidgetInNamespace"), //
                myMockupWidget.data());
        QVERIFY(pointer01 != nullptr);

        QAccessibleInterface const *const pointer02 = //
            accessibleFactory<MockupWidgetInNamespace, MockupAccessibleWidgetInNamespace>( //
                QStringLiteral("abc"), //
                myMockupWidget.data());
        QVERIFY(pointer02 == nullptr);

        QAccessibleInterface const *const pointer03 = //
            accessibleFactory<MockupWidgetInNamespace, MockupAccessibleWidgetInNamespace>( //
                QStringLiteral("PerceptualColor::MockupWidgetInNamespace"), //
                myWidget.data());
        QVERIFY(pointer03 == nullptr);

        // NOLINTEND
    }

    void testInstallAccessibleFactory()
    {
        installAccessibleFactory<MockupWidget, MockupAccessibleWidget>();

        // Calling it again should do nothing.
        installAccessibleFactory<MockupWidget, MockupAccessibleWidget>();
    }

    void testInstallAccessibleFactoryInNamespace()
    {
        installAccessibleFactory<MockupWidgetInNamespace, MockupAccessibleWidgetInNamespace>();

        // Calling it again should do nothing.
        installAccessibleFactory<MockupWidgetInNamespace, MockupAccessibleWidgetInNamespace>();
    }
};
} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperAccessibility)

// The following “include” is necessary because we do not use a header file:
#include "testhelperaccessibility.moc"
