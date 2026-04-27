// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_WHEELCOLORPICKER_P_H
#define PERCEPTUALCOLOR_WHEELCOLORPICKER_P_H

// Include the header of the public class of this private implementation.
// #include "wheelcolorpicker.h"

#include "constpropagatingrawpointer.h"
#include "helperconversion.h"
#include "lchvalues.h"
#include <qglobal.h>
#include <qobject.h>
#include <qpointer.h>
#include <qsharedpointer.h>
#include <qsize.h>
#include <qtmetamacros.h>
class QWidget;

namespace PerceptualColor
{
class ChromaLightnessDiagram;
class ColorWheel;
class WheelColorPicker;

/** @internal
 *
 *  @brief Private implementation within the <em>Pointer to
 *  implementation</em> idiom */
class WheelColorPickerPrivate : public QObject
{
    Q_OBJECT

public:
    // Constructors and destructors
    explicit WheelColorPickerPrivate(WheelColorPicker *backLink, const LchSpace projectionSpace);
    /** @brief Default destructor */
    virtual ~WheelColorPickerPrivate() noexcept override = default;

    // Member methods
    [[nodiscard]] QSizeF optimalChromaLightnessDiagramSize() const;
    void resizeChildWidgets();

    // Data members
    /** @brief A pointer to the @ref ChromaLightnessDiagram child widget. */
    QPointer<ChromaLightnessDiagram> m_chromaLightnessDiagram;
    /** @brief A pointer to the @ref ColorWheel child widget. */
    QPointer<ColorWheel> m_colorWheel;
    /**
     * @brief Geometry of the current Lch color space.
     * */
    const LchValues m_lchValues;
    /**
     * @brief The color space into which the gamut will be projected.
     */
    const LchSpace m_projectionSpace;

public Q_SLOTS:
    void handleFocusChanged(QWidget *old, QWidget *now);

private:
    Q_DISABLE_COPY(WheelColorPickerPrivate)

    /** @brief Pointer to the object from which <em>this</em> object
     *  is the private implementation. */
    ConstPropagatingRawPointer<WheelColorPicker> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_WHEELCOLORPICKER_P_H
