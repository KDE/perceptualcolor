// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helperimage.h"

#include <qglobal.h>
#include <qobject.h>
#include <qscopedpointer.h>
#include <qstring.h>
#include <qstringbuilder.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtextdocument.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

static void snippet01()
{
    // cppcheck-suppress constVariableReference // snippet for documentation
    //! [PerceptualSettings Instance]
    auto &poolReference = PerceptualColor::getLibraryQThreadPoolInstance();
    //! [PerceptualSettings Instance]
    Q_UNUSED(poolReference)
}

namespace PerceptualColor
{

class TestHelperImage : public QObject
{
    Q_OBJECT

public:
    explicit TestHelperImage(QObject *parent = nullptr)
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

    void testFindBoundary()
    {
        // Does not crash on empty image
        const auto boundary = findBoundary(QImage());
        Q_UNUSED(boundary)
    }

    void testDoAntialias()
    {
        const auto myColorFunction = [](const double, const double) -> QRgb {
            // Mock-up
            return QColor(Qt::black).rgba();
        };
        QImage myImage;
        // Does not crash on empty image
        doAntialias(myImage, QList<QPoint>(), myColorFunction);
    }

    void testSnippet01()
    {
        snippet01();
    }

    void testQRgbTransparent()
    {
        // The alpha value of a transparent QRgb must be 0."
        QCOMPARE(qAlpha(qRgbTransparent), 0);

        // Also all RGB channels must be 0 in order to be compatible with both
        // premultiplied and non-premultiplied (straight) QRgb values.
        QCOMPARE(qRed(qRgbTransparent), 0);
        QCOMPARE(qGreen(qRgbTransparent), 0);
        QCOMPARE(qBlue(qRgbTransparent), 0);
    }

    void testFillRect()
    {
        QImage image{QSize(3, 3), QImage::Format_ARGB32_Premultiplied};
        image.fill(Qt::red);

        QCOMPARE(image.pixelColor(2, 2), Qt::red);

        fillRect(image.bits(), image.bytesPerLine(), QRect(2, 2, 1, 1), qRgbTransparent);

        QCOMPARE(image.pixelColor(2, 2).alpha(), 0);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperImage)
// The following “include” is necessary because we do not use a header file:
#include "testhelperimage.moc"
