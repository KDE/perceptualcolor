// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_MULTISPINBOXSECTION_H
#define PERCEPTUALCOLOR_MULTISPINBOXSECTION_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qdebug.h>
#include <qmetatype.h>
#include <qstring.h>

namespace PerceptualColor
{
class MultiSpinBoxSectionPrivate;

// NOTE The design of this configuration class is similar to Qt’s
// QNetworkConfiguration: an fully copyable object that can be
// passed to other classes, which then interpret its settings.
// Internally, the class uses the pimpl idiom to hide implementation
// details and allow for future binary-compatible extensions.
/*!
 * \inmodule ModulePublicApi
 *
 * \brief Configuration of a single section
 * within a \l MultiSpinBox.
 *
 * This class encapsulates the configuration settings for an individual
 * section of a \l MultiSpinBox widget.
 *
 * This data type can be passed to QDebug thanks to
 * \l {operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSection &value)}
 *
 * This type is declared as type to Qt’s type system via
 * \c {Q_DECLARE_METATYPE}. Depending on your use case (for
 * example if you want to use for \e {queued} signal-slot connections),
 * you might consider calling \c {qRegisterMetaType()} for
 * this type, once you have a QApplication object.
 */
class PERCEPTUALCOLOR_IMPORTEXPORT MultiSpinBoxSection
{
public:
    MultiSpinBoxSection() noexcept;
    MultiSpinBoxSection(const MultiSpinBoxSection &other) noexcept;
    ~MultiSpinBoxSection() noexcept;
    MultiSpinBoxSection &operator=(const MultiSpinBoxSection &other) noexcept;
    MultiSpinBoxSection(MultiSpinBoxSection &&other) noexcept;
    MultiSpinBoxSection &operator=(MultiSpinBoxSection &&other) noexcept;

    [[nodiscard]] int decimals() const;
    [[nodiscard]] QString formatString() const;
    [[nodiscard]] bool isGroupSeparatorShown() const;
    [[nodiscard]] bool isWrapping() const;
    [[nodiscard]] double maximum() const;
    [[nodiscard]] double minimum() const;
    [[nodiscard]] QString prefix() const;
    void setDecimals(int newDecimals);
    void setFormatString(const QString &newFormatString);
    void setGroupSeparatorShown(bool shown);
    void setMaximum(double newMaximum);
    void setMinimum(double newMinimum);
    void setRange(double newMinimum, double newMaximum);
    void setSingleStep(double newSingleStep);
    void setWrapping(bool newIsWrapping);
    [[nodiscard]] double singleStep() const;
    [[nodiscard]] QString suffix() const;

private:
    friend class MultiSpinBoxSectionPrivate;
    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<MultiSpinBoxSectionPrivate> d_pointer;

    friend class TestMultiSpinBoxSection;

    // This class is used as configuration for  MultiSpinBox. Thought
    // currently there is no need for this friend declaration,
    // it is done nevertheless. (If it would become necessary later,
    // adding it would break the binary API, which we want to avoid.)
    friend class MultiSpinBox;
};

PERCEPTUALCOLOR_IMPORTEXPORT QDebug operator<<(QDebug dbg, const PerceptualColor::MultiSpinBoxSection &value);

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::MultiSpinBoxSection)

#endif // PERCEPTUALCOLOR_MULTISPINBOXSECTION_H
