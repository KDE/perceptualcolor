// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef GRADIENTSLIDER_H
#define GRADIENTSLIDER_H

#include "abstractdiagram.h"
#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include "importexport.h"
#include <qglobal.h>
#include <qmetatype.h>
#include <qnamespace.h>
#include <qsharedpointer.h>
#include <qsize.h>
class QKeyEvent;
class QMouseEvent;
class QPaintEvent;
class QResizeEvent;
class QWheelEvent;
class QWidget;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class GradientSliderPrivate;

class RgbColorSpace;

/** @brief A slider who’s groove displays an LCH color gradient.
 *
 * @image html GradientSlider.png "GradientSlider"
 *
 * The groove of this slider that displays a gradient between two LCH
 * colors. The gradient is an equal gradient calculated independently
 * for each of the four components (lightness, chroma, hue, alpha).
 *
 * The hue component is the only one that is circular (0° = 360°): Here,
 * Here, the path via the shorter side is always chosen. Examples:
 * @li If the first hue is 182° and the second hue is 1°, than
 *     the hue will increase from 182° up to 359°, than 0° and then 1°.
 * @li If the first hue is 169° and the second hue is 359°, than
 *     the hue will decrease from 169° down to 0°, and then 359°.
 *
 * This widget considers the alpha channel, using a background
 * of gray squares behind the (semi-)transparent colors.
 *
 * Example:
 * |                  |   L |  C |   h  | alpha |
 * | :--------------- | --: | -: | ---: | ----: |
 * | @ref firstColorCieLchD50A  | 80% |  5 |  15° |   0.7 |
 * |                  | 70% |  7 |   5° |   0.8 |
 * |                  | 60% |  9 | 355° |   0.9 |
 * | @ref secondColorCieLchD50A | 50% | 11 | 345° |   1.0 |
 *
 * Note that due to this mathematical model, there might be out-of-gamut colors
 * within the slider even if both, the first and the second color are in-gamut
 * colors. Out-of-gamut colors are rendered as nearby in-gamut colors.
 *
 * - In the case of vertical @ref orientation, @ref firstColorCieLchD50A is the colour
 *   at the bottom of the widget and @ref secondColorCieLchD50A is the colour at the
 *   top of the widget.
 * - In the case of horizontal @ref orientation, @ref firstColorCieLchD50A is the
 *   colour on the left of the widget and @ref secondColorCieLchD50A is the colour
 *   on the right of the widget in LTR layout. In RTL layout it is the
 *   other way round.
 *
 * @internal
 *
 * @todo NICETOHAVE A better handle for the slider.
 * Currently, the handle is just a
 * line. At a handle position at the very beginning or end of the slider,
 * furthermore only half of the line thickness is visible. It might be better
 * to have arrows outside the slider to mark the position. (On the other
 * hand, this would be different to the slider handles of the color
 * wheels…)
 *
 * @todo NICETOHAVE A better focus indicator.
 *
 * @todo SHOULDHAVE This class @ref GradientSlider and also the class
 * @ref ColorWheel could be subclasses of QAbstractSlider. This
 * might integrate better with the user’s Qt code. On the other hand,
 * this would mean a lot of work in this library to implement the
 * complete interface of QAbstractSlider, and probably we would
 * also need multiple inheritance because this class also depends
 * on @ref AbstractDiagram which is itself yet a subclass of QWidget.
 */
// The API is roughly orientated on QSlider/QAbstractSlider and on
// KSelecter/KGradientSelector where applicable. Our API is however
// less complete, and of course also a little bit different, as
// both, QAbstractSlider and KGradientSelector are not directly
// comparable to this class.
class PERCEPTUALCOLOR_IMPORTEXPORT GradientSlider : public AbstractDiagram
{
    Q_OBJECT

    /** @brief First color (the one corresponding to a low @ref value)
     *
     * @sa READ @ref firstColorCieLchD50A() const
     * @sa WRITE @ref setFirstColorCieLchD50A()
     * @sa NOTIFY @ref firstColorCieLchD50AChanged()
     * @sa @ref secondColorCieLchD50A */
    Q_PROPERTY(PerceptualColor::GenericColor firstColorCieLchD50A READ firstColorCieLchD50A WRITE setFirstColorCieLchD50A NOTIFY firstColorCieLchD50AChanged)

    /** @brief Orientation of the widget.
     *
     * By default, the orientation is horizontal. The possible
     * orientations are <tt>Qt::Horizontal</tt> and <tt>Qt::Vertical</tt>.
     *
     * Also, <tt>Qt::Orientation</tt> is declared in this header as type to
     * Qt’s type system: <tt>Q_DECLARE_METATYPE(Qt::Orientation)</tt>. This
     * is done because Qt itself does not declare this type as meta type.
     * Because we use it here in a property including a signal, we have to
     * declare this type. Depending on your use case (for example if you
     * want to use it reliably in Qt’s signals and slots), you might consider
     * calling <tt>qRegisterMetaType()</tt> for this type, once you have
     * a QApplication object.
     *
     * @sa READ @ref orientation() const
     * @sa WRITE @ref setOrientation()
     * @sa NOTIFY @ref orientationChanged() */
    Q_PROPERTY(Qt::Orientation orientation READ orientation WRITE setOrientation NOTIFY orientationChanged)

    /** @brief This property holds the page step.
     *
     * The larger of two natural steps this widget provides.
     * Corresponds to the user pressing PageUp or PageDown.
     *
     * The valid range is <tt>[0, 1]</tt>.
     *
     * @sa READ @ref pageStep() const
     * @sa WRITE @ref setPageStep()
     * @sa NOTIFY @ref pageStepChanged()
     * @sa @ref singleStep */
    Q_PROPERTY(qreal pageStep READ pageStep WRITE setPageStep NOTIFY pageStepChanged)

    /** @brief Second color (the one corresponding to a high @ref value)
     *
     * @sa READ @ref secondColorCieLchD50A() const
     * @sa WRITE @ref setSecondColorCieLchD50A()
     * @sa NOTIFY @ref secondColorCieLchD50AChanged()
     * @sa @ref firstColorCieLchD50A */
    // The Q_PROPERTY macro must be on a single line for correct compilation.
    // clang-format is disabled here to prevent automatic line breaks.
    // clang-format off
    Q_PROPERTY(PerceptualColor::GenericColor secondColorCieLchD50A READ secondColorCieLchD50A WRITE setSecondColorCieLchD50A NOTIFY secondColorCieLchD50AChanged)
    // clang-format on

    /** @brief This property holds the single step.
     *
     * The smaller of two natural steps this widget provides.
     * Corresponds to the user pressing an arrow key.
     *
     * The valid range is <tt>[0, 1]</tt>.
     *
     * @sa READ @ref singleStep() const
     * @sa WRITE @ref setSingleStep()
     * @sa NOTIFY @ref singleStepChanged()
     * @sa @ref pageStep */
    Q_PROPERTY(qreal singleStep READ singleStep WRITE setSingleStep NOTIFY singleStepChanged)

    /** @brief The slider’s current value.
     *
     *
     * The valid range is <tt>[0, 1]</tt>.
     * The slider forces the value to be within the valid range:
     * <tt>0 <= value <= 1</tt>.
     * - <tt>0</tt> means: totally firstColorCieLchD50A()
     * - <tt>1</tt> means: totally secondColorCieLchD50A()
     *
     * @sa READ @ref value() const
     * @sa WRITE @ref setValue()
     * @sa NOTIFY @ref valueChanged() */
    Q_PROPERTY(qreal value READ value WRITE setValue NOTIFY valueChanged USER true)

public:
    Q_INVOKABLE explicit GradientSlider(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace, QWidget *parent = nullptr);
    Q_INVOKABLE explicit GradientSlider(const QSharedPointer<PerceptualColor::RgbColorSpace> &colorSpace,
                                        Qt::Orientation orientation,
                                        QWidget *parent = nullptr);
    virtual ~GradientSlider() noexcept override;
    /** @brief Getter for property @ref firstColorCieLchD50A
     *  @returns the property */
    [[nodiscard]] PerceptualColor::GenericColor firstColorCieLchD50A() const;
    [[nodiscard]] virtual QSize minimumSizeHint() const override;
    /** @brief Getter for property @ref orientation
     *  @returns the property */
    [[nodiscard]] Qt::Orientation orientation() const;
    /** @brief Getter for property @ref pageStep
     *  @returns the property */
    [[nodiscard]] qreal pageStep() const;
    /** @brief Getter for property @ref secondColorCieLchD50A
     *  @returns the property */
    [[nodiscard]] PerceptualColor::GenericColor secondColorCieLchD50A() const;
    /** @brief Getter for property @ref singleStep
     *  @returns the property */
    [[nodiscard]] qreal singleStep() const;
    [[nodiscard]] virtual QSize sizeHint() const override;
    /** @brief Getter for property @ref value
     *  @returns the property */
    [[nodiscard]] qreal value() const;

Q_SIGNALS:
    /** @brief Signal for @ref firstColorCieLchD50A property.
     * @param newFirstColorCieLchD50A the new @ref firstColorCieLchD50A */
    void firstColorCieLchD50AChanged(const PerceptualColor::GenericColor &newFirstColorCieLchD50A);
    /** @brief Signal for @ref orientation property.
     * @param newOrientation the new @ref orientation */
    void orientationChanged(const Qt::Orientation newOrientation);
    /** @brief Signal for @ref pageStep property.
     * @param newPageStep the new @ref pageStep */
    void pageStepChanged(const qreal newPageStep);
    /** @brief Signal for @ref secondColorCieLchD50A property.
     * @param newSecondColorCieLchD50A the new @ref secondColorCieLchD50A */
    void secondColorCieLchD50AChanged(const PerceptualColor::GenericColor &newSecondColorCieLchD50A);
    /** @brief Signal for @ref singleStep property.
     * @param newSingleStep the new @ref singleStep */
    void singleStepChanged(const qreal newSingleStep);
    /** @brief Signal for @ref value property.
     * @param newValue the new @ref value */
    void valueChanged(const qreal newValue);

public Q_SLOTS:
    void setColors(const PerceptualColor::GenericColor &newFirstColorCieLchD50A, const PerceptualColor::GenericColor &newSecondColorCieLchD50A);
    void setFirstColorCieLchD50A(const PerceptualColor::GenericColor &newFirstColorCieLchD50A);
    void setOrientation(const Qt::Orientation newOrientation);
    void setPageStep(const qreal newPageStep);
    void setSecondColorCieLchD50A(const PerceptualColor::GenericColor &newSecondColorCieLchD50A);
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

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class GradientSliderPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<GradientSliderPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestGradientSlider;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(Qt::Orientation)

#endif // GRADIENTSLIDER_H
