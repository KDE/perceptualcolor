// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_GRADIENTSLIDER_H
#define PERCEPTUALCOLOR_GRADIENTSLIDER_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "internalimportexport.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qmetatype.h>
#include <qnamespace.h>
#include <qsize.h>
#include <qtmetamacros.h>
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWheelEvent;
class QWidget;

namespace PerceptualColor
{
class GradientSliderPrivate;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A slider who’s groove displays an LCH color gradient.
 *
 * \image GradientSlider.png "GradientSlider"
 *
 * The groove of this slider that displays a gradient between two LCH
 * colors. The gradient is an equal gradient calculated independently
 * for each of the four components (lightness, chroma, hue, alpha).
 *
 * The hue component is the only one that is circular (0° = 360°): Here,
 * Here, the path via the shorter side is always chosen. Examples:
 * \list
 * \li If the first hue is 182° and the second hue is 1°, than
 *     the hue will increase from 182° up to 359°, than 0° and then 1°.
 * \li If the first hue is 169° and the second hue is 359°, than
 *     the hue will decrease from 169° down to 0°, and then 359°.
 * \endlist
 *
 * This widget considers the alpha channel, using a background
 * of gray squares behind the (semi-)transparent colors.
 *
 * Example:
 * \table
 *     \header \li slider position \li L   \li C  \li h    \li alpha
 *     \row    \li first color     \li 80% \li  5 \li  15° \li   0.7
 *     \row    \li intermediate    \li 70% \li  7 \li   5° \li   0.8
 *     \row    \li intermediate    \li 60% \li  9 \li 355° \li   0.9
 *     \row    \li second color    \li 50% \li 11 \li 345° \li   1.0
 * \endtable
 *
 * Note that due to this mathematical model, there might be out-of-gamut colors
 * within the slider even if both, the first and the second color are in-gamut
 * colors. Out-of-gamut colors are rendered as nearby in-gamut colors.
 *
 * \list
 * \li In the case of vertical \l orientation, \l setFirstColorLchA is the
 *   colour at the bottom of the widget and \l setSecondColorLchA is the
 *   colour at the top of the widget.
 * \li In the case of horizontal \l orientation, \l setFirstColorLchA is the
 *   colour on the left of the widget and \l setSecondColorLchA is the colour
 *   on the right of the widget in LTR layout. In RTL layout it is the
 *   other way round.
 * \endlist
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT GradientSlider : public AbstractDiagram
// The API is roughly orientated on QSlider/QAbstractSlider and on
// KSelecter/KGradientSelector where applicable. Our API is however
// less complete, and of course also a little bit different, as
// both, QAbstractSlider and KGradientSelector are not directly
// comparable to this class.
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::GradientSlider::orientation
     *
     * \brief Orientation of the widget.
     *
     * By default, the orientation is horizontal. The possible
     * orientations are \c {Qt::Horizontal} and \c {Qt::Vertical}.
     *
     * Also, \c {Qt::Orientation} is declared in this header as type to
     * Qt’s type system: \c {Q_DECLARE_METATYPE(Qt::Orientation)}. This
     * is done because Qt itself does not declare this type as meta type.
     * Because we use it here in a property including a signal, we have to
     * declare this type. Depending on your use case (for example if you
     * want to use it reliably in Qt’s signals and slots), you might consider
     * calling \c {qRegisterMetaType()} for this type, once you have
     * a QApplication object.
     */
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

    /*!
     * \property PerceptualColor::GradientSlider::pageStep
     *
     * \brief This property holds the page step.
     *
     * The larger of two natural steps this widget provides.
     * Corresponds to the user pressing PageUp or PageDown.
     *
     * The valid range is \c {[0, 1]}.
     */
    Q_PROPERTY(qreal pageStep READ pageStep WRITE setPageStep NOTIFY pageStepChanged)

    /*!
     * \property PerceptualColor::GradientSlider::singleStep
     *
     * \brief This property holds the single step.
     *
     * The smaller of two natural steps this widget provides.
     * Corresponds to the user pressing an arrow key.
     *
     * The valid range is \c {[0, 1]}.
     */
    Q_PROPERTY(qreal singleStep READ singleStep WRITE setSingleStep NOTIFY singleStepChanged)

    /*!
     * \property PerceptualColor::GradientSlider::value
     *
     * \brief The slider’s current value.
     *
     *
     * The valid range is \c {[0, 1]}.
     * The slider forces the value to be within the valid range:
     * \c {0 <= value <= 1}.
     * A value of \c {0} means totally firstColorLchA(), whereas \c {1} means
     * totally secondColorLchA().
     */
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged USER true)

public:
    Q_INVOKABLE explicit GradientSlider(const PerceptualColor::LchSpace projectionSpace, QWidget *parent = nullptr);
    Q_INVOKABLE explicit GradientSlider(const PerceptualColor::LchSpace projectionSpace, Qt::Orientation orientation, QWidget *parent = nullptr);
    virtual ~GradientSlider() noexcept override;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    /*!
     * \brief Getter for property \l orientation
     */
    [[nodiscard]] Qt::Orientation orientation() const;
    /*!
     * \brief Getter for property \l pageStep
     */
    [[nodiscard]] qreal pageStep() const;
    /*!
     * \brief Getter for property \l singleStep
     */
    [[nodiscard]] qreal singleStep() const;
    [[nodiscard]] virtual QSize sizeHint() const override;
    /*!
     * \brief Getter for property \l value
     */
    [[nodiscard]] qreal value() const;

Q_SIGNALS:
    /*!
     * \brief Signal for \l orientation property.
     *
     * \a newOrientation the new \l orientation
     */
    void orientationChanged(const Qt::Orientation newOrientation);
    /*!
     * \brief Signal for \l pageStep property.
     *
     * \a newPageStep the new \l pageStep
     */
    void pageStepChanged(const qreal newPageStep);
    /*!
     * \brief Signal for \l singleStep property.
     *
     * \a newSingleStep the new \l singleStep
     */
    void singleStepChanged(const qreal newSingleStep);
    /*!
     * \brief Signal for \l value property.
     *
     * \a newValue the new value
     */
    void valueChanged(const qreal newValue);

public Q_SLOTS:
    void setColors(const PerceptualColor::GenericColor &newFirstColorLch,
                   const double newFirstColorAlpha,
                   const PerceptualColor::GenericColor &newSecondColorLch,
                   const double newSecondColorAlpha);
    void setFirstColorLchA(const PerceptualColor::GenericColor &newFirstColorLch, const double newFirstColorAlpha);
    void setOrientation(const Qt::Orientation newOrientation);
    void setPageStep(const qreal newPageStep);
    void setSecondColorLchA(const PerceptualColor::GenericColor &newSecondColorLch, const double newSecondColorAlpha);
    void setSingleStep(const qreal newSingleStep);
    void setValue(const qreal newValue);

protected:
    virtual void keyPressEvent(QKeyEvent *event) override;
    virtual void mouseMoveEvent(QMouseEvent *event) override;
    virtual void mousePressEvent(QMouseEvent *event) override;
    virtual void mouseReleaseEvent(QMouseEvent *event) override;
    virtual void paintEvent(QPaintEvent *event) override;
    virtual void resizeEvent(QResizeEvent *event) override;
    virtual void wheelEvent(QWheelEvent *event) override;

private:
    Q_DISABLE_COPY(GradientSlider)

    friend class GradientSliderPrivate;

    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<GradientSliderPrivate> d_pointer;

    friend class TestGradientSlider;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(Qt::Orientation)

#endif // PERCEPTUALCOLOR_GRADIENTSLIDER_H
