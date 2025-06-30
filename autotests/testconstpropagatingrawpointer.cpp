// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "constpropagatingrawpointer.h"

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
    // Assuming you have a member variable in your class:
    PerceptualColor::ConstPropagatingRawPointer<QRectF> pointerToQRectF{new QRectF};

    // Now, you access this member variable from a method within your class:

    // Helper variables
    QRectF myRectF;
    qreal myHeight;
    QRectF *normalCppPointerToQRectF;

    // The following code works within both, const and non-const contexts:
    myHeight = pointerToQRectF->height();
    myRectF = *pointerToQRectF;

    // The following code works only within non-const contexts.
    // Within cost contexts, you will get an error at compile time.
    pointerToQRectF->setHeight(5);
    *pointerToQRectF = myRectF;
    normalCppPointerToQRectF = pointerToQRectF;
    delete pointerToQRectF;
    //! [example]
    Q_UNUSED(myRectF)
    Q_UNUSED(myHeight)
    Q_UNUSED(normalCppPointerToQRectF)
}

namespace PerceptualColor
{
class TestConstPropagatingRawPointer : public QObject
{
    Q_OBJECT

public:
    explicit TestConstPropagatingRawPointer(QObject *parent = nullptr)
        : QObject(parent)
        , pointerToQRectF(new QRectF)
        , pointerToTestDelete(new QRectF)
    {
    }

private:
    ConstPropagatingRawPointer<QRectF> pointerToQRectF;
    ConstPropagatingRawPointer<QRectF> pointerToTestDelete;

private Q_SLOTS:

    void initTestCase()
    {
        // Called before the first test function is executed
    }

    void cleanupTestCase()
    {
        // Called after the last test function was executed
        delete pointerToQRectF;
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
        ConstPropagatingRawPointer<QObject> test;
    }

    void testDefaultConstructor()
    {
        ConstPropagatingRawPointer<QObject> test;
        QVERIFY2(!test, "Verify that default constructor produced an invalid pointer.");
    }

    // NOTE Should break on compile time if the method is const.
    void testDelete()
    {
        delete pointerToTestDelete;
    }

    // NOTE Should break on compile time if the method is const.
    void testNonConstAccess()
    {
        pointerToQRectF->setHeight(5);
    }

    // NOTE Should break on compile time if the method is const.
    void testBackCopy01()
    {
        QRectF temp = QRectF(1, 2, 3, 4);
        *pointerToQRectF = temp;
    }

    // NOTE Should break on compile time if the method is const.
    void testCastToNormalRawPointer()
    {
        QRectF *temp;
        temp = pointerToQRectF;
        Q_UNUSED(temp)
    }

    void testCastToNormalRawPointerToConstObjectInConstContext() const
    {
        const QRectF *temp;
        temp = pointerToQRectF.toPointerToConstObject();
        Q_UNUSED(temp)
    }

    void testCastToNormalRawPointerToConstObjectInNonConstContext()
    {
        const QRectF *temp;
        temp = pointerToQRectF.toPointerToConstObject();
        Q_UNUSED(temp)
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

QTEST_MAIN(PerceptualColor::TestConstPropagatingRawPointer)
// The following “include” is necessary because we do not use a header file:
#include "testconstpropagatingrawpointer.moc"
