// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "languagechangeeventfilter.h"

#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qglobal.h>
#include <qobject.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{

class ReceiverMockup : public QWidget
{
    Q_OBJECT
public:
    explicit ReceiverMockup(QWidget *parent = nullptr)
        : QWidget(parent){};
    bool languageChangeEventDetected = false;

protected:
    virtual void changeEvent(QEvent *event) override
    {
        if (event->type() == QEvent::LanguageChange) {
            languageChangeEventDetected = true;
        }
    };
};

class TestLanguageChangeEventFilter : public QObject
{
    Q_OBJECT

public:
    explicit TestLanguageChangeEventFilter(QObject *parent = nullptr)
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

    void testConstructorDestructor()
    {
        LanguageChangeEventFilter temp;
    }

    void testEventDiscarded()
    {
        // Assertions
        {
            ReceiverMockup receiverTest;
            QEvent languageChangeEventTest(QEvent::LanguageChange);
            QCoreApplication::sendEvent(&receiverTest, &languageChangeEventTest);
            QVERIFY(receiverTest.languageChangeEventDetected);
        }

        // Actual test
        ReceiverMockup receiver;
        LanguageChangeEventFilter filter;
        receiver.installEventFilter(&filter);
        QEvent languageChangeEvent(QEvent::LanguageChange);
        QCoreApplication::sendEvent(&receiver, &languageChangeEvent);
        QCOMPARE(receiver.languageChangeEventDetected, false);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestLanguageChangeEventFilter)

// The following “include” is necessary because we do not use a header file:
#include "testlanguagechangeeventfilter.moc"
