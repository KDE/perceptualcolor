// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "refreshiconengine.h"

#include <QtCore/qglobal.h>
#include <qbytearray.h>
#include <qicon.h>
#include <qiconengine.h>
#include <qimage.h>
#include <qlist.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qrect.h>
#include <qscopedpointer.h>
#include <qsize.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qtestdata.h>
#include <qwidget.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

Q_DECLARE_METATYPE(QIcon::Mode)
Q_DECLARE_METATYPE(QIcon::State)

namespace PerceptualColor
{
class TestRefreshIconEngine : public QObject
{
    Q_OBJECT

public:
    explicit TestRefreshIconEngine(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    static void voidMessageHandler(QtMsgType, const QMessageLogContext &, const QString &)
    {
        // dummy message handler that does not print messages
    }

    void helperProvideTestData()
    {
        QTest::addColumn<QRect>("rect");
        QTest::addColumn<QIcon::Mode>("mode");
        QTest::addColumn<QIcon::State>("state");

        QList<QPoint> rectPointList;
        rectPointList.append(QPoint(0, 0));
        rectPointList.append(QPoint(0, 10));
        rectPointList.append(QPoint(10, 0));
        rectPointList.append(QPoint(10, 10));
        rectPointList.append(QPoint(-10, 0));
        rectPointList.append(QPoint(0, -10));
        rectPointList.append(QPoint(-10, -10));

        QList<QSize> rectSizeList;
        rectSizeList.append(QSize(0, 0));
        rectSizeList.append(QSize(1, 1));
        rectSizeList.append(QSize(10, 10));
        rectSizeList.append(QSize(10, 20));
        rectSizeList.append(QSize(-1, -1));
        rectSizeList.append(QSize(0, 10));
        rectSizeList.append(QSize(10, 0));

        QList<QIcon::Mode> modeList;
        modeList.append(QIcon::Mode::Active);
        modeList.append(QIcon::Mode::Disabled);
        modeList.append(QIcon::Mode::Normal);
        modeList.append(QIcon::Mode::Selected);

        QList<QIcon::State> stateList;
        stateList.append(QIcon::State::Off);
        stateList.append(QIcon::State::On);

        for (int i = 0; i < rectPointList.size(); ++i) {
            for (int j = 0; j < rectSizeList.size(); ++j) {
                for (int k = 0; k < modeList.size(); ++k) {
                    for (int l = 0; l < stateList.size(); ++l) {
                        QTest::newRow(QStringLiteral("QRect(QPoint(%1, %2), QSize(%3, %4)) %5 %6")
                                          .arg(rectPointList.at(i).x())
                                          .arg(rectPointList.at(i).y())
                                          .arg(rectSizeList.at(j).width())
                                          .arg(rectSizeList.at(j).height())
                                          .arg(modeList.at(k))
                                          .arg(stateList.at(l))
                                          .toUtf8()
                                          .data())
                            << QRect(rectPointList.at(i), rectSizeList.at(j)) << modeList.at(k) << stateList.at(l);
                    }
                }
            }
        }
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

    void testConstructorDestructorOnStack()
    {
        // The class should instantiate on the stack without crash.
        RefreshIconEngine test;
    }

    void testConstructorDestructorOnHeap()
    {
        // The class should instantiate on the heap without crash.
        QScopedPointer<RefreshIconEngine> test{new RefreshIconEngine};
    }

    void testClone()
    {
        QScopedPointer<RefreshIconEngine> test1{new RefreshIconEngine};
        QWidget testWidget;
        test1->setReferenceWidget(&testWidget);
        // The clone function should not crash.
        QScopedPointer<QIconEngine> test2{test1->clone()};
        RefreshIconEngine *test2a = dynamic_cast<RefreshIconEngine *>(test2.data());
        QVERIFY2(test2a != nullptr,
                 "The clone should correctly cast dynamically "
                 "to RefreshIconEngine*.");
        // Test if the reference widget was copied correctly.
        QCOMPARE(test2a->m_referenceWidget, &testWidget);
    }

    void testPaint_data()
    {
        helperProvideTestData();
    }

    void testPaint()
    {
        QFETCH(QRect, rect);
        QFETCH(QIcon::Mode, mode);
        QFETCH(QIcon::State, state);
        QImage myImage(10, 10, QImage::Format::Format_ARGB32_Premultiplied);
        QPainter myPainter(&myImage);
        RefreshIconEngine myEngine;

        // Test if there is no crash also on strange values

        // suppress warning on invalid sizes
        qInstallMessageHandler(voidMessageHandler);

        myEngine.paint(&myPainter, rect, mode, state);

        // do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testPaintFallbackIcon_data()
    {
        helperProvideTestData();
    }

    void testPaintFallbackIcon()
    {
        QFETCH(QRect, rect);
        QFETCH(QIcon::Mode, mode);
        QImage myImage(10, 10, QImage::Format::Format_ARGB32_Premultiplied);
        QPainter myPainter(&myImage);
        RefreshIconEngine myEngine;

        // Test if there is no crash also on strange values

        // suppress warning on invalid sizes
        qInstallMessageHandler(voidMessageHandler);

        myEngine.paintFallbackIcon(&myPainter, rect, mode);

        // do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testPixmap_data()
    {
        helperProvideTestData();
    }

    void testPixmap()
    {
        QFETCH(QRect, rect);
        QFETCH(QIcon::Mode, mode);
        QFETCH(QIcon::State, state);
        RefreshIconEngine myEngine;

        // Test if there is no crash also on strange values

        // suppress warning on invalid sizes
        qInstallMessageHandler(voidMessageHandler);

        Q_UNUSED(myEngine.pixmap(rect.size(), mode, state));

        // do not suppress warnings anymore
        qInstallMessageHandler(nullptr);
    }

    void testSetReferenceWidget()
    {
        RefreshIconEngine myEngine;
        QScopedPointer<QWidget> myWidget{new QWidget};
        // Setting reference widget shall not crash.
        myEngine.setReferenceWidget(myWidget.data());
        QCOMPARE(myEngine.m_referenceWidget, myWidget.data());
        QImage myImage(10, 10, QImage::Format::Format_ARGB32_Premultiplied);
        QPainter myPainter(&myImage);
        // Should not crash
        myEngine.paint(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active, QIcon::State::On);
        // Should not crash
        myEngine.paintFallbackIcon(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active);
        // Should not crash
        Q_UNUSED(myEngine.pixmap(QSize(11, 11), QIcon::Mode::Active, QIcon::State::On));
        // Now delete the widget, then test again for crash
        myWidget.reset();
        // Should not crash
        myEngine.paint(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active, QIcon::State::On);
        // Should not crash
        myEngine.paintFallbackIcon(&myPainter, QRect(1, 1, 11, 11), QIcon::Mode::Active);
        // Should not crash
        Q_UNUSED(myEngine.pixmap(QSize(11, 11), QIcon::Mode::Active, QIcon::State::On));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestRefreshIconEngine)

// The following “include” is necessary because we do not use a header file:
#include "testrefreshiconengine.moc"
