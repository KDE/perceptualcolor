// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "constpropagatinguniquepointer.h"

#include <qglobal.h>
#include <qobject.h>
#include <qrect.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

static void snippet01()
{
    //! [example]
    // A ConstPropagatingUniquePointer pointing to a new QObject
    PerceptualColor::ConstPropagatingUniquePointer<QObject> myPointer(new QObject());
    //! [example]
}

namespace PerceptualColor
{
class TestConstPropagatingUniquePointer : public QObject
{
    Q_OBJECT

public:
    explicit TestConstPropagatingUniquePointer(QObject *parent = nullptr)
        : QObject(parent)
        , pointerToQRectF(new QRectF)
    {
    }

private:
    ConstPropagatingUniquePointer<QRectF> pointerToQRectF;

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
        ConstPropagatingUniquePointer<QObject> test;
    }

    void testDefaultConstructor()
    {
        ConstPropagatingUniquePointer<QObject> test;
        QCOMPARE(test.get(), nullptr);
    }

    // NOTE Should break on compile time when the function is const.
    void testNonConstAccess()
    {
        // The following line should not break
        pointerToQRectF->setHeight(5);
    }

    // NOTE Should break on compile time when the function is const.
    void testBackCopy01()
    {
        QRectF temp = QRectF(1, 2, 3, 4);
        *pointerToQRectF = temp;
    }

    void testConstAccess01() const
    {
        // The following line should not break
        qreal height = pointerToQRectF->height();
        Q_UNUSED(height)
    }

    void testConstAccess02()
    {
        // The following line should not break
        qreal height = pointerToQRectF->height();
        Q_UNUSED(height)
    }

    void testCopy01() const
    {
        QRectF temp = *pointerToQRectF;
        Q_UNUSED(temp)
    }

    void testCopy02()
    {
        QRectF temp = *pointerToQRectF;
        Q_UNUSED(temp)
    }

    void testReset()
    {
        ConstPropagatingUniquePointer<int> ptr(new int(42));
        QCOMPARE(*ptr, 42);

        ptr.reset(new int(23));
        QCOMPARE(*ptr, 23);

        ptr.reset();
        QCOMPARE(ptr.operator->(), nullptr);
        QCOMPARE(ptr.get(), nullptr);
    }

    void testSwap1()
    {
        ConstPropagatingUniquePointer<int> ptr1(new int(42));
        ConstPropagatingUniquePointer<int> ptr2(new int(23));
        QCOMPARE(*ptr1, 42);
        QCOMPARE(*ptr2, 23);

        ptr1.swap(ptr2);
        QCOMPARE(*ptr1, 23);
        QCOMPARE(*ptr2, 42);
    }

    void testSwap2()
    {
        ConstPropagatingUniquePointer<int> ptr1(new int(42));
        ConstPropagatingUniquePointer<int> ptr2;
        QCOMPARE(*ptr1, 42);
        QCOMPARE(ptr2.operator->(), nullptr);

        ptr1.swap(ptr2);
        QCOMPARE(ptr1.operator->(), nullptr);
        QCOMPARE(*ptr2, 42);
    }

    void testGet()
    {
        ConstPropagatingUniquePointer<int> ptr(new int(42));
        QCOMPARE(*ptr.get(), 42);
        QVERIFY(ptr.get() != nullptr);
    }

    void testGet2()
    {
        ConstPropagatingUniquePointer<int> ptr;
        QCOMPARE(ptr.get(), nullptr);
    }

    void testSnippet01()
    {
        snippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestConstPropagatingUniquePointer)
// The following “include” is necessary because we do not use a header file:
#include "testconstpropagatinguniquepointer.moc"
