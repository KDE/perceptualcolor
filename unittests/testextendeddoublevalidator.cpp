// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "extendeddoublevalidator.h"

#include <qobject.h>
#include <qobjectdefs.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvalidator.h>

namespace PerceptualColor
{
class TestExtendedDoubleValidator : public QObject
{
    Q_OBJECT

public:
    TestExtendedDoubleValidator(QObject *parent = nullptr)
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

    void testConstructor()
    {
        PerceptualColor::ExtendedDoubleValidator myValidator;
    }

    void testPrefix()
    {
        PerceptualColor::ExtendedDoubleValidator myValidator;
        QCOMPARE(myValidator.prefix(), QString());
        const QString myString = QStringLiteral("abc");
        myValidator.setPrefix(myString);
        QCOMPARE(myValidator.prefix(), myString);
        myValidator.setRange(0, 1000);
        QString testString;
        int testPosition = 0;
        testString = QStringLiteral("abc1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("abc1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("ab1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("ab1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QString();
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
    }

    void testSuffix()
    {
        PerceptualColor::ExtendedDoubleValidator myValidator;
        QCOMPARE(myValidator.suffix(), QString());
        const QString myString = QStringLiteral("abc");
        myValidator.setSuffix(myString);
        QCOMPARE(myValidator.suffix(), myString);
        myValidator.setRange(0, 1000);
        QString testString;
        int testPosition = 0;
        testString = QStringLiteral("1abc");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("1000abc");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Acceptable);
        testString = QStringLiteral("1ab");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1000ab");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QStringLiteral("1000");
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
        testString = QString();
        QCOMPARE(myValidator.validate(testString, testPosition), QValidator::State::Invalid);
    }

    void testValidate()
    {
        ExtendedDoubleValidator myValidator;
        myValidator.setPrefix(QStringLiteral("abc"));
        myValidator.setSuffix(QStringLiteral("def"));
        myValidator.setRange(0, 1000);
        const QString originalInput = QStringLiteral("abc123def");
        QString myInput = originalInput;
        const int originalPos = 5;
        int myPos = originalPos;
        QValidator::State result = myValidator.validate(myInput, myPos);
        // The input should be considered valid.
        QCOMPARE(result, QValidator::State::Acceptable);
        // On simple cases of valid input, the string should not change.
        QCOMPARE(myInput, originalInput);
        // On simple cases of valid input, the position should not change.
        QCOMPARE(myPos, originalPos);
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestExtendedDoubleValidator)

// The following “include” is necessary because we do not use a header file:
#include "testextendeddoublevalidator.moc"
