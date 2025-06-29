// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "extendeddoublevalidator.h"

#include <qglobal.h>
#include <qobject.h>
#include <qstring.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvalidator.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#endif

namespace PerceptualColor
{
class TestExtendedDoubleValidator : public QObject
{
    Q_OBJECT

public:
    explicit TestExtendedDoubleValidator(QObject *parent = nullptr)
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
        // NOTE decimals(): QDoubleValidator’s default behavior changed in
        // Qt 6.3. In Qt ≤ 6.2, QDoubleValidator::decimals() returned 1000 by
        // default. From Qt 6.3 onward, the default was changed to -1, breaking
        // API stability. To guarantee consistent behaviour across all
        // Qt versions, this class explicitly sets the default decimals value
        // to -1 during initialization.
        QCOMPARE(myValidator.decimals(), -1);
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
