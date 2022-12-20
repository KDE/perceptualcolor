// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: MIT

#ifndef WHEELCOLORPICKER_P_H
#define WHEELCOLORPICKER_P_H

// Include the header of the public class of this private implementation.
// #include "wheelcolorpicker.h"

#include "constpropagatingrawpointer.h"
#include <QtCore/qsharedpointer.h>
#include <qglobal.h>
#include <qobject.h>
#include <qobjectdefs.h>
#include <qpointer.h>
#include <qsize.h>
#include <qstring.h>
class QWidget;

namespace PerceptualColor
{
class ChromaLightnessDiagram;
class ColorWheel;
class RgbColorSpace;
class WheelColorPicker;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class WheelColorPickerPrivate final : public QObject
{
    Q_OBJECT

public:
    // Constructors and destructors
    WheelColorPickerPrivate(WheelColorPicker *backLink);
    /** @brief Default destructor */
    virtual ~WheelColorPickerPrivate() noexcept override = default;

    // Member methods
    [[nodiscard]] QSizeF optimalChromaLightnessDiagramSize() const;
    void resizeChildWidgets();

    // Data members
    /** @brief A pointer to the @ref ChromaLightnessDiagram child widget. */
    QPointer<ChromaLightnessDiagram> m_chromaLightnessDiagram;
    /** @brief A pointer to the color space. */
    QSharedPointer<PerceptualColor::RgbColorSpace> m_rgbColorSpace;
    /** @brief A pointer to the @ref ColorWheel child widget. */
    QPointer<ColorWheel> m_colorWheel;

public Q_SLOTS:
    void handleFocusChanged(QWidget *old, QWidget *now);

private:
    Q_DISABLE_COPY(WheelColorPickerPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<WheelColorPicker> q_pointer;
};

} // namespace PerceptualColor

#endif // WHEELCOLORPICKER_P_H
