// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "colorwheelimageprovider.h"

#include <qtest.h>
#include <qtestcase.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
class TestColorWheelImageProvider : public QObject
{
    Q_OBJECT

public:
    explicit TestColorWheelImageProvider(QObject *parent = nullptr)
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

#ifndef MSVC_DLL

    // The automatic export of otherwise private symbols on MSVC
    // shared libraries via CMake's WINDOWS_EXPORT_ALL_SYMBOLS property
    // does not work well for Qt meta objects. AsyncImageProvider inherits
    // from AsyncImageProviderBase, which relies on Qt meta object
    // functionality and whose API is private. Therefore, instantiation of
    // AsyncImageProvider is not possible, so the following unit tests cannot
    // be built for MSVC shared libraries.

    void testConstructorDestructorCielchD50()
    {
        // Make sure that constructor and destructor do not crash:
        ColorWheelImageProvider<LchSpace::CielchD50> test;
        Q_UNUSED(test)
    }

    void testConstructorDestructorOklch()
    {
        // Make sure that constructor and destructor do not crash:
        ColorWheelImageProvider<LchSpace::Oklch> test;
        Q_UNUSED(test)
    }

    void testDrawColorWheel()
    {
        QImage image(100, 100, QImage::Format_ARGB32_Premultiplied);
        QPainter painter(&image);
        // Make sure that constructor and destructor do not crash:
        ColorWheelImageProvider<LchSpace::Oklch>::drawColorWheel( //
            painter,
            1.5,
            QPointF(50, 50),
            45,
            10);
    }

    void testConnectPaintEvent()
    {
        AbstractDiagram diagram;
        ColorWheelImageProvider<LchSpace::Oklch>::connectPaintEvent(&diagram);
    }

#endif
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestColorWheelImageProvider)

// The following “include” is necessary because we do not use a header file:
#include "testcolorwheelimageprovider.moc"
