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

private:
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
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

    void testMessages()
    {
        // Suppress warnings
        qInstallMessageHandler(voidMessageHandler);

        // Do something …

        // Do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testWidget()
    {
        QVERIFY(false); // XXX Implement me!

        QScopedPointer<PerceptualColor::Widget> widget(new PerceptualColor::Widget());

        // It is necessary to show the widget and make it active
        // to make focus and widget events working within unit tests.
        widget->show();
        QApplication::setActiveWindow(widget.data());

        // Key clicks must go to the focus widget to work as expected.
        widget->setFocus();
        QTest::keyClick(QApplication::focusWidget(), //
                        Qt::Key::Key_T,
                        Qt::KeyboardModifier::AltModifier);

        // If focus is not important, send the event directly.
        QKeyEvent keyEvent(QEvent::KeyPress, //
                           Qt::Key::Key_T, //
                           Qt::KeyboardModifier::AltModifier);
        QCoreApplication::sendEvent(widget.data(), &keyEvent);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestXXX)

// The following “include” is necessary because we do not use a header file:
#include "testxxx.moc"
