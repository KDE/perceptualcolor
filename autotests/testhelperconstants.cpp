﻿// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "helperconstants.h"

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
#endif

static bool snippet01()
{
    const QString myRichText = QStringLiteral(u"abc");
    const QString myPlainText = QStringLiteral(u"abc");
    QScopedPointer<QWidget> myWidget1(new QWidget());
    QScopedPointer<QWidget> myWidget2(new QWidget());
    //! [richTextMarkerExample]
    myWidget1->setToolTip( // Make sure rich text is treated as such:
        PerceptualColor::richTextMarker + myRichText);

    myWidget2->setToolTip( // Make sure plain text is treated as such:
        PerceptualColor::richTextMarker + myPlainText.toHtmlEscaped());
    //! [richTextMarkerExample]
    return Qt::mightBeRichText(myWidget1->toolTip()) //
        && Qt::mightBeRichText(myWidget2->toolTip());
}

namespace PerceptualColor
{

class TestHelperConstants : public QObject
{
    Q_OBJECT

public:
    explicit TestHelperConstants(QObject *parent = nullptr)
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

    void testCielabGamutPrecision()
    {
        // The value is somewhat arbitrary.
        // Make sure that at least it is not too high.
        static_assert(PerceptualColor::gamutPrecisionCielab < 1, //
                      "Verify that gamut precision value is not too high");
    }

    void testOklabPrecision()
    {
        // The value is somewhat arbitrary.
        // Make sure that at least it is not too high.
        static_assert(PerceptualColor::gamutPrecisionCielab < 0.01, //
                      "Verify that gamut precision value is not too high");
    }

    void testSteps()
    {
        static_assert(pageStepChroma > singleStepChroma, //
                      "Chroma page step is bigger than single step.");
        static_assert(singleStepChroma > 0, //
                      "Chroma single step is positive.");
        static_assert(pageStepHue > singleStepHue, //
                      "Hue page step is bigger than single step.");
        static_assert(singleStepHue > 0, //
                      "Hue single step is positive.");
    }

    void testRichTextMarkerIsRecognized()
    {
        const QString myMarker = richTextMarker;
        QVERIFY(myMarker.size() > 0);
        QVERIFY(Qt::mightBeRichText(myMarker));

        const QString myText = QStringLiteral(u"abc");
        QVERIFY(!Qt::mightBeRichText(myText)); // Assertion
        QVERIFY(Qt::mightBeRichText(myMarker + myText));
    }

    void testRichTextMarkerSnippet()
    {
        QVERIFY(snippet01());
    }

    void testRichTextMarkerIsInvisible()
    {
        QTextDocument myDocument;
        const QString myRichText = QStringLiteral(u"This <em>is</em> a test.");
        myDocument.setHtml(myRichText);
        QCOMPARE(myDocument.toRawText(), // Assertion
                 QStringLiteral(u"This is a test."));
        // Now, test if the rich text marker is actually invisible:
        myDocument.setHtml(richTextMarker + myRichText);
        QCOMPARE(myDocument.toRawText(), QStringLiteral(u"This is a test."));
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestHelperConstants)
// The following “include” is necessary because we do not use a header file:
#include "testhelperconstants.moc"
