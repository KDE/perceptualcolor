// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CUSTOMCOLORPATCH_P_H
#define PERCEPTUALCOLOR_CUSTOMCOLORPATCH_P_H

#include <qbytearrayview.h>
#include <qcolor.h>
#include <qpoint.h>
#include <qstringview.h>
#include <qtclasshelpermacros.h>

#include "completecolor.h"
#include "constpropagatingrawpointer.h"

class QMimeData;

namespace PerceptualColor
{

class CustomColorPatch;

/*!
 * \inmodule ModulePrivateApi
 *
 *  \brief Private implementation within the \e {Pointer to
 *  implementation} idiom. */
class CustomColorPatchPrivate
{
public:
    explicit CustomColorPatchPrivate(CustomColorPatch *backLink);
    virtual ~CustomColorPatchPrivate() noexcept;

    /*!
     * \brief MIME type used for exchanging color data with GEGL/GIMP via
     * drag-and-drop or clipboard.
     */
    static inline constexpr QStringView mimeTypeGegl{u"application/x-geglcolor"};

    [[nodiscard]] static QMimeData *createMimeData(const QColor color);

    void execDrag();

    [[nodiscard]] static CompleteColor fromGegelToCompleteColor(const QByteArrayView data);

    [[nodiscard]] static CompleteColor fromMimeToCompleteColor(const QMimeData *mimeData);

    /*!
     * \brief Internal storage for color data.
     */
    CompleteColor m_colorData;

    /*!
     * \brief Initial cursor position (at the time of the left-click)
     * where a drag-and-drop action starts.
     *
     * \note If \l ColorPatch::color is invalid, this value is undefined.
     *
     * \sa CustomColorPatch::mousePressEvent
     * \sa CustomColorPatch::mouseMoveEvent
     */
    QPoint m_dragStartPosition;

    /*!
     * \brief Internal storage for property extraPrecision
     */
    int m_extraPrecision = 0;

    /*!
     * \brief If colors with an alpha value different from 1 are allowed.
     */
    bool m_isTransparencyAllowed = true;

private:
    Q_DISABLE_COPY(CustomColorPatchPrivate)

    /*!
     * \brief Pointer to the object from which \e {this} object
     *  is the private implementation. */
    ConstPropagatingRawPointer<CustomColorPatch> q_pointer;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CUSTOMCOLORPATCH_P_H
