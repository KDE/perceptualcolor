// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// First included header is the public header of the class we are testing;
// this forces the header to be self-contained.
#include "multispinboxsection.h"
// Second, the private implementation.
#include "multispinboxsection_p.h" // IWYU pragma: keep

#include <qglobal.h>
#include <qobject.h>
#include <qspinbox.h>
#include <qstringliteral.h>
#include <qtest.h>
#include <qtestcase.h>
#include <qvariant.h>
#include <utility>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class TestMultiSpinBoxSection : public QObject
{
    Q_OBJECT

public:
    explicit TestMultiSpinBoxSection(QObject *parent = nullptr)
        : QObject(parent)
    {
    }

private:
    MultiSpinBoxSection helperGetUnusualConfig()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(1);
        myConfig.setWrapping(true);
        myConfig.setMaximum(3);
        myConfig.setMinimum(2);
        myConfig.setFormatString(QStringLiteral("a%1b"));
        myConfig.setSingleStep(4);
        return myConfig;
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

    void testDefaultValues()
    {
        // The default values should be the same as for QDoubleSpinBox
        MultiSpinBoxSection myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Test default values of the configuration object:
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());
        QCOMPARE(myConfig.isWrapping(), myDoubleSpinBox.wrapping());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.prefix(), myDoubleSpinBox.prefix());
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());
        QCOMPARE(myConfig.suffix(), myDoubleSpinBox.suffix());
    }

    void testCopy()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setDecimals(1);
        myConfig.setWrapping(true);
        myConfig.setMaximum(3);
        myConfig.setMinimum(2);
        myConfig.setFormatString(QStringLiteral("a%1b"));
        myConfig.setSingleStep(4);

        // Test copy constructor
        MultiSpinBoxSection copyContructed(myConfig);
        QCOMPARE(copyContructed.decimals(), 1);
        QCOMPARE(copyContructed.isWrapping(), true);
        QCOMPARE(copyContructed.maximum(), 3);
        QCOMPARE(copyContructed.minimum(), 2);
        QCOMPARE(copyContructed.formatString(), QStringLiteral("a%1b"));
        QCOMPARE(copyContructed.prefix(), QStringLiteral("a"));
        QCOMPARE(copyContructed.suffix(), QStringLiteral("b"));
        QCOMPARE(copyContructed.singleStep(), 4);

        // Test copy assignment operator
        MultiSpinBoxSection copyAssigned;
        copyAssigned.setMaximum(9); // Change arbitrary some value
        copyAssigned = myConfig;
        QCOMPARE(copyAssigned.decimals(), 1);
        QCOMPARE(copyAssigned.isWrapping(), true);
        QCOMPARE(copyAssigned.maximum(), 3);
        QCOMPARE(copyAssigned.minimum(), 2);
        QCOMPARE(copyAssigned.formatString(), QStringLiteral("a%1b"));
        QCOMPARE(copyAssigned.prefix(), QStringLiteral("a"));
        QCOMPARE(copyAssigned.suffix(), QStringLiteral("b"));
        QCOMPARE(copyAssigned.singleStep(), 4);
    }

    void testMoveConstructor()
    {
        MultiSpinBoxSection testObjectToMove;
        testObjectToMove.setDecimals(1);
        testObjectToMove.setWrapping(true);
        testObjectToMove.setMaximum(3);
        testObjectToMove.setMinimum(2);
        testObjectToMove.setFormatString(QStringLiteral("a%1b"));
        testObjectToMove.setSingleStep(4);
        MultiSpinBoxSection myConfig(
            // Trigger the move constructor
            std::move(testObjectToMove));
        QCOMPARE(myConfig.decimals(), 1);
        QCOMPARE(myConfig.isWrapping(), true);
        QCOMPARE(myConfig.maximum(), 3);
        QCOMPARE(myConfig.minimum(), 2);
        QCOMPARE(myConfig.formatString(), QStringLiteral("a%1b"));
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
        QCOMPARE(myConfig.suffix(), QStringLiteral("b"));
        QCOMPARE(myConfig.singleStep(), 4);
    }

    void testMoveAssignment()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setMaximum(9); // change arbitrarily some value

        // Create a test object
        MultiSpinBoxSection testObjectToMove;
        testObjectToMove.setDecimals(1);
        testObjectToMove.setWrapping(true);
        testObjectToMove.setMaximum(3);
        testObjectToMove.setMinimum(2);
        testObjectToMove.setFormatString(QStringLiteral("a%1b"));
        testObjectToMove.setSingleStep(4);

        // Move assignment:
        myConfig = std::move(testObjectToMove);

        QCOMPARE(myConfig.decimals(), 1);
        QCOMPARE(myConfig.isWrapping(), true);
        QCOMPARE(myConfig.maximum(), 3);
        QCOMPARE(myConfig.minimum(), 2);
        QCOMPARE(myConfig.formatString(), QStringLiteral("a%1b"));
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
        QCOMPARE(myConfig.suffix(), QStringLiteral("b"));
        QCOMPARE(myConfig.singleStep(), 4);
    }

    void testDecimals()
    {
        MultiSpinBoxSection myConfig;
        // decimals() is 2 by default, analogous to QDoubleSpinBox
        QCOMPARE(myConfig.decimals(), 2);
        myConfig.setDecimals(5);
        QCOMPARE(myConfig.decimals(), 5);
    }

    void testIsWrapping()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setWrapping(true);
        QCOMPARE(myConfig.isWrapping(), true);
        myConfig.setWrapping(false);
        QCOMPARE(myConfig.isWrapping(), false);
    }

    void testMaximum()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setMaximum(5);
        QCOMPARE(myConfig.maximum(), 5);
    }

    void testMinimum()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setMinimum(5);
        QCOMPARE(myConfig.minimum(), 5);
    }

    void testPrefix()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setFormatString(QStringLiteral("a%1b"));
        QCOMPARE(myConfig.prefix(), QStringLiteral("a"));
    }

    void testSingleStep()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setSingleStep(5);
        QCOMPARE(myConfig.singleStep(), 5);
    }

    void testSuffix()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setFormatString(QStringLiteral("a%1b"));
        QCOMPARE(myConfig.suffix(), QStringLiteral("b"));
    }

    void testFormatString()
    {
        MultiSpinBoxSection myConfig;
        myConfig.setFormatString(QStringLiteral("a%1b"));
        QCOMPARE(myConfig.formatString(), QStringLiteral("a%1b"));
    }

    void testComplianceDecimals()
    {
        MultiSpinBoxSection myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        myConfig.setDecimals(5);
        myDoubleSpinBox.setDecimals(5);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(50);
        myDoubleSpinBox.setDecimals(50);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(500);
        myDoubleSpinBox.setDecimals(500);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(0);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());

        myConfig.setDecimals(-1);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myConfig.decimals(), myDoubleSpinBox.decimals());
    }

    void testComplianceSingleStep()
    {
        MultiSpinBoxSection myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        myConfig.setSingleStep(5);
        myDoubleSpinBox.setSingleStep(5);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(50);
        myDoubleSpinBox.setSingleStep(50);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(500);
        myDoubleSpinBox.setSingleStep(500);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(0);
        myDoubleSpinBox.setSingleStep(0);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());

        myConfig.setSingleStep(-1);
        myDoubleSpinBox.setSingleStep(-1);
        QCOMPARE(myConfig.singleStep(), myDoubleSpinBox.singleStep());
    }

    void testComplianceMinimumTooBig()
    {
        MultiSpinBoxSection myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a minimum that is bigger than the current maximum
        myConfig.setMaximum(50);
        myDoubleSpinBox.setMaximum(50);
        myConfig.setMinimum(60);
        myDoubleSpinBox.setMinimum(60);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());

        // Afterwards, change back the maximum to a value that would
        // have allowed to the original minimum
        myConfig.setMaximum(70);
        myDoubleSpinBox.setMaximum(70);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
    }

    void testComplianceMaximumTooSmall()
    {
        MultiSpinBoxSection myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a maximum that is smaller than the current minimum
        myConfig.setMinimum(50);
        myDoubleSpinBox.setMinimum(50);
        myConfig.setMaximum(40);
        myDoubleSpinBox.setMaximum(40);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());

        // Afterwards, change back the minimum to a value that would
        // have allowed to the original maximum
        myConfig.setMinimum(30);
        myDoubleSpinBox.setMinimum(30);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
    }

    void testComplianceMinimumDecimals()
    {
        MultiSpinBoxSection myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a minimum with more decimals than configured.
        // Check compliance of the rounding behaviour.
        myConfig.setDecimals(1);
        myConfig.setMinimum(12.3456);
        myDoubleSpinBox.setDecimals(1);
        myDoubleSpinBox.setMinimum(12.3456);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());

        // Check compliance when changing the decimals _after_ setting
        // the minimum value. This seems to allow accessing to the
        // original value of minimum in QDoubleSpinBox!
        myConfig.setDecimals(2);
        myDoubleSpinBox.setDecimals(2);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(3);
        myDoubleSpinBox.setDecimals(3);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(4);
        myDoubleSpinBox.setDecimals(4);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(5);
        myDoubleSpinBox.setDecimals(5);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(6);
        myDoubleSpinBox.setDecimals(6);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(0);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
        myConfig.setDecimals(-1);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myConfig.minimum(), myDoubleSpinBox.minimum());
    }

    void testComplianceMaximumDecimals()
    {
        MultiSpinBoxSection myConfig;
        QDoubleSpinBox myDoubleSpinBox;

        // Set a maximum with more decimals than configured.
        // Check compliance of the rounding behaviour.
        myConfig.setDecimals(1);
        myConfig.setMaximum(12.3456);
        myDoubleSpinBox.setDecimals(1);
        myDoubleSpinBox.setMaximum(12.3456);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());

        // Check compliance when changing the decimals _after_ setting
        // the maximum value. This seems to allow accessing to the
        // original value of maximum in QDoubleSpinBox!
        myConfig.setDecimals(2);
        myDoubleSpinBox.setDecimals(2);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(3);
        myDoubleSpinBox.setDecimals(3);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(4);
        myDoubleSpinBox.setDecimals(4);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(5);
        myDoubleSpinBox.setDecimals(5);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(6);
        myDoubleSpinBox.setDecimals(6);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(0);
        myDoubleSpinBox.setDecimals(0);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
        myConfig.setDecimals(-1);
        myDoubleSpinBox.setDecimals(-1);
        QCOMPARE(myConfig.maximum(), myDoubleSpinBox.maximum());
    }

    void testMetaTypeDeclaration()
    {
        QVariant test;
        // The next line should produce a compiler error if the
        // type is not declared to Qt’s Meta Object System.
        test.setValue(MultiSpinBoxSection());
    }
};

} // namespace PerceptualColor

QTEST_MAIN(PerceptualColor::TestMultiSpinBoxSection)

// The following “include” is necessary because we do not use a header file:
#include "testmultispinboxsection.moc"
