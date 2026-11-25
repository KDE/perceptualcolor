// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CUSTOMCOLORPATCH_H
#define PERCEPTUALCOLOR_CUSTOMCOLORPATCH_H

#include <qtclasshelpermacros.h>
#include <qtmetamacros.h>

#include "colorpatch.h"
#include "completecolor.h"
#include "constpropagatinguniquepointer.h"
#include "internalimportexport.h"

class QWidget;

namespace PerceptualColor
{
class CustomColorPatchPrivate;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief A specialized \l ColorPatch providing copy-and-paste and
 * drag-and-drop support for \l ColorDialog.
 *
 * Provides Copy&Paste and Drag&Drop functionallity.
 *
 * This subclass isolates interaction logic with \l ColorDialog internals from
 * the base \l ColorPatch. This keeps the public \l ColorPatch clean and
 * lightweight while fully inheriting its rendering implementation here, paving
 * the way for future \c QStyle-based rendering support in the base class.
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT CustomColorPatch : public ColorPatch
{
    Q_OBJECT

    /*!
     * \property PerceptualColor::CustomColorPatch::colorData
     *
     * \brief More detailed version of \l ColorPatch::color.
     */
    Q_PROPERTY(PerceptualColor::CompleteColor colorData READ colorData WRITE setColorData NOTIFY colorDataChanged)

public:
    Q_INVOKABLE explicit CustomColorPatch(QWidget *parent = nullptr);
    virtual ~CustomColorPatch() noexcept override;
    CompleteColor colorData() const;

Q_SIGNALS:
    /*!
     * \brief Signal that is emitted when the color data has changed.
     */
    void colorDataChanged();

public Q_SLOTS:
    virtual void setColor(const QColor &newColor) override;
    void setColorData(const PerceptualColor::CompleteColor &newColor);
    void setExtraPrecision(const int newExtraPrecision);
    virtual void setTransparencyAllowed(const bool newValue);

protected:
    virtual void contextMenuEvent(QContextMenuEvent *eventParameter) override;
    virtual void dragEnterEvent(QDragEnterEvent *eventParameter) override;
    virtual void dropEvent(QDropEvent *eventParameter) override;
    virtual void keyPressEvent(QKeyEvent *eventParameter) override;
    virtual void mouseMoveEvent(QMouseEvent *eventParameter) override;
    virtual void mousePressEvent(QMouseEvent *eventParameter) override;

private:
    Q_DISABLE_COPY(CustomColorPatch)

    friend class CustomColorPatchPrivate;
    /*!
     * \brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<CustomColorPatchPrivate> d_pointer;

    friend class TestCustomColorPatch;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CUSTOMCOLORPATCH_H
