// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "PerceptualColor/constpropagatinguniquepointer.h"

#include <memory>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qrect.h>
#include <qstring.h>
#include <qtest.h>
#include <qtestcase.h>

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
    TestConstPropagatingUniquePointer(QObject *parent = nullptr)
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
