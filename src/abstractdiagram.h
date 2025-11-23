// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_ABSTRACTDIAGRAM_H
#define PERCEPTUALCOLOR_ABSTRACTDIAGRAM_H

#include "constpropagatinguniquepointer.h"
#include "importexport.h"
#include <qcolor.h>
#include <qglobal.h>
#include <qimage.h>
#include <qsize.h>
#include <qwidget.h>
class QHideEvent;
class QShowEvent;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

namespace PerceptualColor
{
class AbstractDiagramPrivate;

/** @brief Base class for LCH diagrams.
 *
 * Provides some elements that are common for all LCH diagrams in this
 * library.
 *
 * @internal
 *
 * @note Qt provides some possibilities to declare that a certain widget
 * has a fixed ration between width and height. You can reimplement
 * <tt>QWidget::hasHeightForWidth()</tt> (indicates that the widget’s preferred
 * height depends on its width) and <tt>QWidget::heightForWidth()</tt>
 * (returns the preferred height for this widget, given the width <tt>w</tt>).
 * However, Qt’s layout management makes only very limited use of this
 * information. It is ignored, when the surrounding window is resized by
 * grabbing the window border with the mouse. It is however considered when
 * the surrounding window is resized by grabbing a <tt>QSizeGrip</tt>
 * widget. This behavior is inconsistent and would be surprising for the
 * user. Furthermore, if the widget is yet touching the border of the
 * screen, then the policy cannot be honored anyway; but it causes
 * flickering. Another possibility is QSizePolicy::setHeightForWidth or
 * QSizePolicy::setWidthForHeight which seem both to be “only supported for
 * QGraphicsLayout’s subclasses”. Therefore, it’s better not to use at all
 * these features; that’s the only way to provide a consistent and good
 * user experience.
 *
 * @todo NICETOHAVE @ref AbstractDiagram::actualVisibilityToggledEvent and
 * @ref AbstractDiagram::isActuallyVisible are not actually used.
 * Either use them or remove them?
 */
class PERCEPTUALCOLOR_IMPORTEXPORT AbstractDiagram : public QWidget
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AbstractDiagram(QWidget *parent = nullptr);
    /** @brief Default destructor */
    virtual ~AbstractDiagram() noexcept override;

protected:
    virtual void actualVisibilityToggledEvent();
    void callUpdate();
    [[nodiscard]] QColor focusIndicatorColor() const;
    [[nodiscard]] int gradientMinimumLength() const;
    [[nodiscard]] int gradientThickness() const;
    virtual void hideEvent(QHideEvent *event) override;
    [[nodiscard]] bool isActuallyVisible() const;
    [[nodiscard]] int maximumPhysicalSquareSize() const;
    [[nodiscard]] qreal maximumWidgetSquareSize() const;
    [[nodiscard]] QSize physicalPixelSize() const;
    [[nodiscard]] QColor handleColorFromBackgroundLightness(qreal lightness) const;
    [[nodiscard]] int handleOutlineThickness() const;
    [[nodiscard]] qreal handleRadius() const;
    virtual void showEvent(QShowEvent *event) override;
    [[nodiscard]] int spaceForFocusIndicator() const;
    [[nodiscard]] QImage transparencyBackground() const;

private:
    Q_DISABLE_COPY(AbstractDiagram)

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class AbstractDiagramPrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<AbstractDiagramPrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestAbstractDiagram;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_ABSTRACTDIAGRAM_H
