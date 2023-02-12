// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "refreshiconengine.h"

#include <qapplication.h>
#include <qcoreapplication.h>
#include <qglobal.h>
#include <qguiapplication.h>
#include <qimage.h>
#include <qnamespace.h>
#include <qobject.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpen.h>
#include <qpoint.h>
#include <qrect.h>
#include <qstringliteral.h>
#include <qstyle.h>
#include <qwidget.h>
class QSize;

namespace PerceptualColor
{
/** @brief Constructor.
 *
 * @pre Instantiating this class requires an existing <tt>QApplication</tt>
 * object. */
RefreshIconEngine::RefreshIconEngine()
{
    // We test if a QApplication object exists. While the constructor
    // would work without problems also when there is no QApplication
    // object, this is not true for the member functions: Each instantiation
    // of QPixmap and each call to the (static!) QGuiApplication::palette()
    // would lead to a crash. And the paint function also requires a
    // QApplication object. This is confusing, so it seems better to
    // crash directly here, with a useful error message. (Note that also
    // QWidget crashes in the constructor when no QApplication object is
    // available.)
    if (qobject_cast<QApplication *>(QCoreApplication::instance()) == nullptr) {
        // From Qt documentation about qFatal():
        //
        //     “If you are using the default message handler
        //      this function will abort to create a core dump.”
        //
        // So, it should not be necessary to throw an exception after
        // calling qFatal, because qFatal itself aborts the program.
        qFatal(
            "RefreshIconEngine: Must construct a QApplication "
            "before a RefreshIconEngine");
    }
}

/** @brief Sets a reference widget.
 *
 * The reference widget is the widget who’s color palette is used when
 * drawing the fallback icon. Using this function is optional. When
 * set, the fallback icon colors follow the palette of this specific
 * widget rather than the application’s color palette. (This only makes
 * sense when using the icon with a widget that uses a modified color
 * palette.) If it is not set, the default palette of the application
 * is used instead.
 *
 * @param referenceWidget A pointer to the reference widget, or
 * <tt>nullptr</tt> to not use a reference widget. It is allowed that
 * the widget may be deleted in the future; if so, @ref RefreshIconEngine
 * will not crash, but simply fall back the the default palette of the
 * application.
 *
 * @note
 * - The reference widget is used only for the fallback icon; it has
 *   no influence if the icon-theme’s icon or the widget-style’s icon are
 *   used.
 * - Intentionally, there is no constructor that takes this as argument.
 *   This would be highly confusing, because normally constructors are taking
 *   pointers to widgets to use them as parent. Therefore, it’s better to
 *   avoid this confusion.
 *
 * @sa @ref m_referenceWidget */
void RefreshIconEngine::setReferenceWidget(QWidget *referenceWidget)
{
    m_referenceWidget = referenceWidget;
}

/** @brief Returns the icon as a pixmap with the required size,
 * mode, and state.
 *
 * Reimplemented from base class.
 *
 * Relies on @ref paint() to do the heavy work.
 *
 * @param size The size of the icon.
 * @param mode The mode of the icon.
 * @param state The state of the icon.
 * @returns The icon as a pixmap with the required size,
 * mode, and state. */
QPixmap RefreshIconEngine::pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state)
{
    QImage iconImage{size, QImage::Format_ARGB32_Premultiplied};
    iconImage.fill(Qt::transparent);
    {
        // Putting this in an own block, as the QPainter object
        // might be in an undefined state after paint() has been
        // called. So the QPainter object should not be used
        // anymore after the paint() call.
        QPainter painter(&iconImage);
        paint(&painter, QRect(QPoint(0, 0), size), mode, state);
    }
    return QPixmap::fromImage(iconImage);
}

/** @brief Paints the fallback “refresh” icon.
 *
 * This function provides a freely scalable icon. It does not rely
 * on QSvg, but only on the QPainter class.
 *
 * @todo Provide a real design for this icon, and not just a circle.
 *
 * @param painter Uses the given painter to paint the icon
 * @param rect into the rectangle rect
 * @param mode with the required mode. */
void RefreshIconEngine::paintFallbackIcon(QPainter *painter, const QRect rect, QIcon::Mode mode)
{
    if (rect.isEmpty()) {
        // Return on empty rectangles. This avoids bad calculations with
        // formulas that require a non-empty, non-negative (!) size.
        return;
    }

    // It might be possible that rect.width() and rect.height are not
    // identical. Probably the best thing to do would be to center
    // the (square) icon within this rectangle. On the other side,
    // Qt’s own implementation doesn't to that either, but simply
    // aligns to the center of the coordinate system (top left corner),
    // also on right-to-left locales. So, for simplicity, we do the same
    // thing here.
    constexpr int unscaledPenWidth = 1;
    constexpr int designSpace = 16;
    const int destinationSpace = qMin(rect.width(), rect.height());
    const qreal halfDestinationSpace = destinationSpace / 2.0;
    const qreal scaleFactor = static_cast<qreal>(destinationSpace) / designSpace;
    const qreal radius = halfDestinationSpace - 0.5 * unscaledPenWidth * scaleFactor;
    painter->setRenderHint(QPainter::RenderHint::Antialiasing);
    painter->setCompositionMode(
        // While CompositionMode_SourceOver is the default value
        // anyway, it’s important to set it explicitly, as the
        // painter that we received might currently be set to
        // another CompositionMode.
        QPainter::CompositionMode::CompositionMode_SourceOver);
    QPen pen;
    pen.setWidthF(unscaledPenWidth * scaleFactor);
    QPalette::ColorGroup paletteColorGroup;
    switch (mode) {
    case QIcon::Mode::Normal:
        paletteColorGroup = QPalette::ColorGroup::Normal;
        break;
    case QIcon::Mode::Disabled:
        paletteColorGroup = QPalette::ColorGroup::Disabled;
        break;
    case QIcon::Mode::Active:
        paletteColorGroup = QPalette::ColorGroup::Active;
        break;
    case QIcon::Mode::Selected:
        paletteColorGroup = QPalette::ColorGroup::Normal;
        break;
    }
    QPalette referencePalette;
    if (m_referenceWidget.isNull()) {
        referencePalette = QGuiApplication::palette();
    } else {
        referencePalette = m_referenceWidget->palette();
    }
    pen.setColor(referencePalette.color(paletteColorGroup, QPalette::ColorRole::Text));
    painter->setPen(pen);
    painter->drawEllipse(QPointF(halfDestinationSpace + rect.left(), halfDestinationSpace + rect.top()), radius, radius);
}

/** @brief Paints the icon.
 *
 * Reimplemented from base class.
 *
 * @param painter Uses the given painter to paint the icon
 * @param rect into the rectangle rect
 * @param mode with the required mode
 * @param state and state. */
void RefreshIconEngine::paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state)
{
    // Initialize
    QIcon myIcon;

    // First, try to load an icon from the current icon theme.
    myIcon = QIcon::fromTheme(QStringLiteral("dialog-apply"));
    if (!myIcon.isNull()) {
        myIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
        return;
    }
    myIcon = QIcon::fromTheme(QStringLiteral("gtk-apply"));
    if (!myIcon.isNull()) {
        myIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
        return;
    }
    myIcon = QIcon::fromTheme(QStringLiteral("dialog-ok-apply"));
    if (!myIcon.isNull()) {
        myIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
        return;
    }
    myIcon = QIcon::fromTheme(QStringLiteral("gtk-ok-apply"));
    if (!myIcon.isNull()) {
        myIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
        return;
    }
    myIcon = QIcon::fromTheme(QStringLiteral("dialog-ok"));
    if (!myIcon.isNull()) {
        myIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
        return;
    }
    myIcon = QIcon::fromTheme(QStringLiteral("gtk-ok"));
    if (!myIcon.isNull()) {
        myIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
        return;
    }

    // Second, if the current icon theme has no icon (not all platforms
    // provide icon themes, and even those who provide it might be incomplete)
    // then try to use a build-in icon from the current widget style.
    // The q_pointer’s object is still not fully initialized at
    // this point, but it’s base class constructor has fully run;
    // this should be enough to use functionality based on QWidget.
    QStyle *referenceStyle;
    if (m_referenceWidget.isNull()) {
        // We can use QApplication::style(), which requires (in spite of
        // being static), that a QApplication object exists. For an existing
        // QApplication object is checked in the constructor of this class.
        referenceStyle = QApplication::style();
    } else {
        referenceStyle = m_referenceWidget->style();
    }
    myIcon = referenceStyle->standardIcon(QStyle::StandardPixmap::SP_DialogApplyButton);
    if (!myIcon.isNull()) {
        myIcon.paint(painter, rect, Qt::AlignCenter, mode, state);
        return;
    }

    // Third, if no other option has returned a valid icon, we use
    // our build-in icon.
    paintFallbackIcon(painter, rect, mode);
}

/** @brief Returns a clone of this icon engine.
 *
 * Reimplemented from base class, where this function is abstract.
 *
 * This idiom provides a sort of virtual copy constructor that can be
 * called by pointers to the base class of @ref RefreshIconEngine.
 *
 * The implementation basically calls the private copy constructor
 * @ref RefreshIconEngine(const RefreshIconEngine &other) and
 * returns the result.
 *
 * @returns a clone of this icon engine. */
QIconEngine *RefreshIconEngine::clone() const
{
    return new RefreshIconEngine(*this);
}

/** @brief Copy constructor.
 *
 * The base class of <em>this</em> class has an abstract <tt>clone</tt>
 * function. The implementation of @ref clone() here in <em>this</em> class
 * relies on an existing copy constructor.
 *
 * This copy constructor calls the base class’s copy constructor. That
 * means, that this copy constructor cannot be defined as default, which means
 * that we have to implement the function body explicitly and keep track
 * of newly added data members and support them one-by-one here within this
 * copy constructor.
 *
 * To avoid slicing, this copy constructor is private.
 *
 * @param other The other object that shall be copied to this one. */
RefreshIconEngine::RefreshIconEngine(const RefreshIconEngine &other)
    : QIconEngine(other)
    , m_referenceWidget(other.m_referenceWidget)
{
}

} // namespace PerceptualColor
