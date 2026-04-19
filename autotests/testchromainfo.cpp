// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "chromainfo.h"

#include <qtest.h>
#include <qtestcase.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestChromaInfo : public QObject
{
    Q_OBJECT

public:
    explicit TestChromaInfo(QObject *parent = nullptr)
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

    void testConstructor()
    {
        const ChromaInfo temp;
        Q_UNUSED(temp);
    }

    void testMaxOklchChroma()
    {
        QVERIFY(ChromaInfo::maxOklchChroma() > 0.3);
        QVERIFY(ChromaInfo::maxOklchChroma() < 0.4);
    }

    void testMaxCielchD50Chroma()
    {
        QVERIFY(ChromaInfo::maxCielchD50Chroma() > 130);
        QVERIFY(ChromaInfo::maxCielchD50Chroma() < 140);
    }

    void testMaxChromaColorByCielchD50Hue360()
    {
        const auto temp = ChromaInfo::maxChromaColorByCielchD50Hue360(42);
        Q_UNUSED(temp);
    }

    void testMaxChromaColorByOklabHue360()
    {
        const auto temp = ChromaInfo::maxChromaColorByOklabHue360(42);
        Q_UNUSED(temp);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestChromaInfo)

// The following “include” is necessary because we do not use a header file:
#include "testchromainfo.moc"
