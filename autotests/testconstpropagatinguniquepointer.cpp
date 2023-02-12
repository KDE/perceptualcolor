// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "constpropagatinguniquepointer.h"

#include <memory>
#include <qglobal.h>
#include <qobject.h>
#include <qrect.h>
#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
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
        QCOMPARE(test, nullptr);
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
        QRectF temp;
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

    void testSnippet01()
    {
        snippet01();
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestConstPropagatingUniquePointer)
// The following “include” is necessary because we do not use a header file:
#include "testconstpropagatinguniquepointer.moc"
