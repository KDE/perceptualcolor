// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// We actually delete the PIMPL's copy constructor in the private header using
// Q_DISABLE_COPY(ColorPatchPrivate)
// Nevertheless, cppcheck worries about a default copy constructor:
//     "Class 'CustomColorPatchPrivate' does not have a copy constructor which
//     is recommended since it has dynamic memory/resource allocation(s).
//     (CWE-398)"
// So we suppress this warning:
// cppcheck-suppress-file noCopyConstructor

// Own headers
// First the interface, which forces the header to be self-contained.
#include "customcolorpatch.h"
// Second, the private implementation.
#include "customcolorpatch_p.h" // IWYU pragma: associated

#include "absolutecolor.h"
#include "colorpatch_p.h"
#include "constpropagatinguniquepointer.h"
#include "csscolor.h"
#include "helper.h"
#include "helperconversion.h"
#include <algorithm>
#include <qapplication.h>
#include <qbrush.h>
#include <qclipboard.h>
#include <qdrag.h>
#include <qevent.h>
#include <qfont.h>
#include <qframe.h>
#include <qglobal.h>
#include <qimage.h>
#include <qlabel.h>
#include <qmath.h>
#include <qmenu.h>
#include <qmimedata.h>
#include <qnamespace.h>
#include <qpainter.h>
#include <qpalette.h>
#include <qpen.h>
#include <qpixmap.h>
#include <qpoint.h>
#include <qrect.h>
#include <qsizepolicy.h>
#include <qstyle.h>
#include <qstyleoption.h>
#include <qvariant.h>
#include <tuple>
class QWidget;

namespace PerceptualColor
{

/*!
 * \brief Constructor
 *
 * \a parent The parent of the widget, if any
 */
CustomColorPatch::CustomColorPatch(QWidget *parent)
    : ColorPatch(parent)
    , d_pointer(new CustomColorPatchPrivate(this))
{
    setAcceptDrops(true);
    setFocusPolicy(Qt::StrongFocus);

    CustomColorPatch::setColor(color());
}

/*!
 * \brief Destructor
 */
CustomColorPatch::~CustomColorPatch() noexcept
{
}

/*!
 * \brief Constructor
 *
 * \a backLink Pointer to the object from which \e {this} object
 * is the private implementation.
 */
CustomColorPatchPrivate::CustomColorPatchPrivate(CustomColorPatch *backLink)
    : q_pointer(backLink)
{
}

/*!
 * \brief Destructor
 */
CustomColorPatchPrivate::~CustomColorPatchPrivate() noexcept
{
}

/*!
 * \brief Sets wether an alpha channel is enabled.
 *
 * \a newValue The new value.
 *
 * If \c false then every color is opaque, without any transparency.
 */
void CustomColorPatch::setTransparencyAllowed(const bool newValue)
{
    d_pointer->m_isTransparencyAllowed = newValue;

    if (!newValue) {
        d_pointer->m_colorData.alpha = 1;
        QColor temp = color();
        temp.setAlpha(255);
        ColorPatch::setColor(temp);
    }
}

/*!
 * \brief Returns the complete color data.
 */
CompleteColor CustomColorPatch::colorData() const
{
    return d_pointer->m_colorData;
}

/*!
 * \brief Updates color data in both this class and the base class, emitting
 * notification signals as appropriate.
 *
 * The base class manages QColor color data, while this class adds
 * \l CompleteColor color data. Each maintains its own storage.
 *
 * This function updates both data stores prior to emitting any notification
 * signals, ensuring external subscribers see a fully consistent state when
 * receiving signal updates.
 *
 * \a newColor The new \l CompleteColor value to apply.
 */
void CustomColorPatch::setColorData(const PerceptualColor::CompleteColor &newColor)
{
    CompleteColor correctedColor = newColor;
    if (!d_pointer->m_isTransparencyAllowed) {
        correctedColor.alpha = 1;
    }

    bool hasCompleteColorChanged = false;

    // Update internal state first to guarantee consistency before emitting
    // signals.
    if (d_pointer->m_colorData != correctedColor) {
        d_pointer->m_colorData = correctedColor;
        hasCompleteColorChanged = true;
    }

    // Update the base class color data. Note that setColor() may trigger the
    // base class's notifier signal immediately; at that point, both base and
    // derived class data are already up to date.
    QColor newBaseClassColor = d_pointer->m_colorData.opaqueColorSRgb.rgb_QColor;
    newBaseClassColor.setAlphaF(static_cast<float>(correctedColor.alpha));
    ColorPatch::setColor(newBaseClassColor);

    // Emit the change notification for this class if the color was modified.
    if (hasCompleteColorChanged) {
        Q_EMIT colorDataChanged();
    }
}

/*!
 * \brief Setter for the color property.
 *
 * \a newColor the new color
 */
void CustomColorPatch::setColor(const QColor &newColor)
{
    const auto newRgbColor = RgbColor::fromRgb_QColor(newColor);
    const double transparency = //
        (d_pointer->m_isTransparencyAllowed) //
        ? static_cast<double>(newColor.alphaF()) //
        : 1;
    const auto newCompleteColor = //
        CompleteColor::fromSRgbColor(newRgbColor, transparency);
    setColorData(newCompleteColor);
}

/*!
 * \brief React on a mouse move event.
 *
 * Reimplemented from base class.
 *
 * \note When overriding this function in a subclass, make sure to
 * call the base class implementation (AbstractDiagram::mousePressEvent).
 *
 * \a eventParameter The corresponding mouse event
 */
void CustomColorPatch::mousePressEvent(QMouseEvent *eventParameter)
{
    eventParameter->accept();
    if (eventParameter->button() == Qt::LeftButton)
        d_pointer->m_dragStartPosition = eventParameter->pos();

    if (eventParameter->button() == Qt::MiddleButton) {
        const auto mimeDataPtr = //
            QGuiApplication::clipboard()->mimeData(QClipboard::Selection);
        const auto color = d_pointer->fromMimeToCompleteColor(mimeDataPtr);
        if (color.isValid) {
            setColorData(color);
        }
    }
}

/*!
 * \brief Called when a context menu is requested, e.g. by a right mouse click
 * or the context-menu key on the keyboard.
 *
 * \a eventParameter The event.
 */
void CustomColorPatch::contextMenuEvent(QContextMenuEvent *eventParameter)
{
    QMenu menu(this);

    const QIcon copyIcon = qIconFromTheme( //
        QIcon::ThemeIcon::EditCopy, //
        QStringList(QStringLiteral("edit-copy")), //
        QString(QStringLiteral("copy")));

    const auto strings = CssColor::generateCss(d_pointer->m_colorData, //
                                               d_pointer->m_extraPrecision);
    for (const auto &string : std::as_const(strings)) {
        QAction *copyAction = menu.addAction(copyIcon, string);
        connect(copyAction, //
                &QAction::triggered, //
                this, //
                [string]() {
                    QGuiApplication::clipboard()->setText(string);
                });
    }

    menu.addSeparator();

    const QIcon pasteIcon = qIconFromTheme( //
        QIcon::ThemeIcon::EditPaste, //
        QStringList(QStringLiteral("edit-paste")), //
        QString(QStringLiteral("clipboard")));
    QAction *pasteAction = menu.addAction(pasteIcon, tr("&Paste"));
    const auto pasteColor = d_pointer->fromMimeToCompleteColor( //
        QGuiApplication::clipboard()->mimeData());
    if (pasteColor.isValid) {
        connect(pasteAction, //
                &QAction::triggered, //
                this, //
                [this, pasteColor]() {
                    setColorData(pasteColor);
                });
    } else {
        pasteAction->setEnabled(false);
    };

    menu.exec(eventParameter->globalPos());
}

/*!
 * \brief React on a mouse press event.
 *
 * Reimplemented from base class.
 *
 * \note When overriding this function in a subclass, make sure to
 * call the base class implementation (AbstractDiagram::mouseMoveEvent).
 *
 * \a eventParameter The corresponding mouse event
 */
void CustomColorPatch::mouseMoveEvent(QMouseEvent *eventParameter)
{
    eventParameter->accept();
    if (eventParameter->buttons() & Qt::LeftButton) {
        // Distance since the left mouse buttons was originally clicked.
        const auto vector = eventParameter->pos() - d_pointer->m_dragStartPosition;
        const auto distanceSquare = //
            vector.x() * vector.x() + vector.y() * vector.y();
        const auto refSquare = QApplication::startDragDistance() //
            * QApplication::startDragDistance();
        if (distanceSquare >= refSquare) {
            // NOTE Calling execDrag() here does not trigger a new drag
            // operation on every minor mouse movement. Once QDrag::exec()
            // has been invoked, Qt suspends further mouse move events until
            // the drag-and-drop sequence has finished. Only after the drag
            // completes will normal mouse events resume.
            d_pointer->execDrag();
        }
    }
    QWidget::mouseMoveEvent(eventParameter);
}

/*!
 * \brief Creates a new \c QMimeData object on the heap representing
 * a given color.
 *
 * This method allocates a new \c QMimeData instance. Ownership of the
 * returned object is explicitly transferred to the caller, who is responsible
 * for deleting it (e.g., via \c delete).
 *
 * \note When passed to some Qt API like
 * \c {QGuiApplication::clipboard()->setMimeData()}, Qt assumes
 * ownership and will automatically delete the object when appropriate.
 *
 * \a color The color for which the MIME data is created.
 *
 * Returns a pointer to the newly allocated \c QMimeData object on the
 * heap. Never returns \c nullptr.
 *
 * \note When passed to a Qt API that takes ownership, such as
 * \c QClipboard::setMimeData() or \c QDrag::setMimeData(), Qt
 * assumes ownership and will automatically delete the object when appropriate.
 */
QMimeData *CustomColorPatchPrivate::createMimeData(const QColor color)
{
    QMimeData *mimeData = new QMimeData;
    if (color.isValid()) {
        mimeData->setColorData(color);

        if (color.alpha() >= 255) {
            mimeData->setText(CssColor::cssRgbHex(color.red(), //
                                                  color.green(), //
                                                  color.blue()));
        } else {
            mimeData->setText(CssColor::cssRgbHex(color.red(), //
                                                  color.green(), //
                                                  color.blue(), //
                                                  color.alpha()));
        }

        // The explicit length parameter ensures that the first null
        // terminator ('\0') is included as part of the data size (payload),
        // which the GEGL binary format requires. A second trailing '\0' is
        // appended to the actual payload out of caution to safely terminate
        // the underlying string literal.
        constexpr auto geglHeaderU8 = QByteArrayView("R'G'B'A u8\0\0", 11);
        // Always use RGB as the data format. While GEGL also offers support
        // for CIELch and HSV, it lacks support for other color models
        // supported by our library, such as Oklch and Hwb. Using GEGL's
        // native models for only some formats would lead to inconsistent
        // behavior. To ensure a predictable and uniform user experience across
        // all color models, RGB is consistently used as the universal
        // underlying format.
        QByteArray gegl;
        gegl.append(geglHeaderU8);
        gegl.append(static_cast<char>(color.red()));
        gegl.append(static_cast<char>(color.green()));
        gegl.append(static_cast<char>(color.blue()));
        gegl.append(static_cast<char>(color.alpha()));
        mimeData->setData(mimeTypeGegl.toString(), gegl);
    }
    return mimeData;
}

/*!
 * \brief Initiates an outgoing drag operation.
 *
 * This virtual function simplifies support for drag-and-drop.
 * It is called when the user presses the left mouse button and
 * moves the cursor beyond the platform-defined drag threshold.
 *
 * It will:
 *
 * \list
 *     \li Create a QDrag object on the heap, using the widget as parent.
 *     \li Configure the drag object (e.g. set mime data, pixmap).
 *     \li Call QDrag::exec() to start the drag. Qt will take ownership
 *         of the QDrag object; do not delete it manually.
 * \endlist
 */
void CustomColorPatchPrivate::execDrag()
{
    const auto color = q_pointer->color();
    if (color.isValid()) {
        QDrag *drag = new QDrag(q_pointer); // Mandatory on heap and with parent
        // QDrag::setMimeData will take ownership of mime data:
        drag->setMimeData(createMimeData(color));
        drag->setPixmap(q_pointer->dragDropPixmap());
        drag->exec(Qt::CopyAction);
        // Not deleting the objects on the heap. Qt takes ownership and
        // will take are of the cleanup.
    }
}

/*!
 * \brief Decides whether a incoming drag operation may enter this widget.
 *
 * This event is triggered when the mouse, while holding a drag payload,
 * enters the widget's area.
 * The decision directly affects the cursor feedback: if accepted,
 * the cursor shows that dropping is possible; if ignored, the cursor
 * indicates that dropping is forbidden.
 *
 * Reimplemented from base class.
 *
 * \a eventParameter The corresponding event
 */
void CustomColorPatch::dragEnterEvent(QDragEnterEvent *eventParameter)
{
    if (d_pointer->fromMimeToCompleteColor(eventParameter->mimeData()).isValid) {
        eventParameter->acceptProposedAction();
        return;
    }

    eventParameter->ignore();
    return;
}

/*!
 * \brief Handles the actual drop operation.
 *
 * This event is triggered when the user releases the mouse button
 * inside the widget while dragging data. At this point the widget
 * should process the dropped payload and perform the intended action.
 *
 * Reimplemented from base class.
 *
 * \a eventParameter The corresponding event
 */
void CustomColorPatch::dropEvent(QDropEvent *eventParameter)
{
    const auto dropColor = //
        d_pointer->fromMimeToCompleteColor(eventParameter->mimeData());
    if (dropColor.isValid) {
        setColorData(dropColor);
        eventParameter->acceptProposedAction();
        return;
    }

    eventParameter->ignore();
    return;
}

/*!
 * \brief Convert mime data to QColor
 *
 * \a mimeData The mime data
 *
 * Returns a corresponding valid QColor if the mime data contains
 * color data that can be processed.
 * An invalid QColor otherwise.
 */
CompleteColor CustomColorPatchPrivate::fromMimeToCompleteColor(const QMimeData *mimeData)
{
    // Qt color mime type
    if (mimeData->hasColor()) {
        const auto m_qColor = qvariant_cast<QColor>(mimeData->colorData());
        if (m_qColor.isValid()) {
            const auto m_rgbColor = RgbColor::fromRgb_QColor(m_qColor);
            return CompleteColor::fromSRgbColor( //
                m_rgbColor, //
                static_cast<double>(m_qColor.alphaF()));
        }
    }

    // Gimp color drag-and-drop
    const auto myMimeTypeGegl = mimeTypeGegl.toString();
    if (mimeData->hasFormat(myMimeTypeGegl)) {
        const auto raw = //
            mimeData->data(myMimeTypeGegl);
        const auto geglColor = fromGegelToCompleteColor(raw);
        if (geglColor.isValid) {
            return geglColor;
        }
    }

    // text-based CSS colors
    const auto string = mimeData->text(); // Should also cover CSS and HTML.
    const auto cssParsedColor = CssColor::parseToCompleteColor(string);
    if (cssParsedColor.isValid) {
        return cssParsedColor;
    }

    // Fall-through
    return CompleteColor(); // invalid color
}

/*!
 * \brief Converts raw GEGL color data into a \c QColor object.
 *
 * Parses raw binary data formatted in GEGL's MIME transfer format.
 *
 * \a data A \c QByteArrayView referencing the raw GEGL data.
 *
 * Returns a valid \c QColor object matching the decoded color channels
 * if the payload format is supported and valid; otherwise, an invalid
 * \c QColor.
 *
 * \note Limitations: RGB data and data in color models based on RGB (HSV,
 * etc.) are interpreted using the sRGB color profile. For the CMYK model, due
 * to the lack of a widespread default color profile, the conversion falls back
 * to QColor’s internal CMYK transformation, which is quite primitive.
 */
CompleteColor CustomColorPatchPrivate::fromGegelToCompleteColor(const QByteArrayView data)
{
    if (data.size() > 100) {
        // Do not parse byte arrays that are far too big to be valid GEGL data.
        return CompleteColor();
    }

    const qsizetype nullPos = data.indexOf('\0');
    if (nullPos == -1) {
        return CompleteColor();
    }

    const QByteArrayView headerText = data.first(nullPos); // without \0
    const qsizetype lastSpace = headerText.lastIndexOf(' ');
    if (lastSpace == -1) {
        return CompleteColor();
    }

    const QByteArrayView model = headerText.first(lastSpace);
    const QByteArrayView format = headerText.sliced(lastSpace + 1);
    const QByteArray values = data.sliced(nullPos + 1).toByteArray();

    // NOTE It is not documented whether Gimp/GEGL writes doubles
    // in host-native byte order or in a fixed byte order. This implementation
    // assumes host-native layout, which works on x86/x86-64 (little-endian).
    // On other architectures or when exchanging data across hosts, explicit
    // endian handling would be required.
    if (model == "R'G'B'A") {
        constexpr uchar count = 4;
        if (format == "u8") {
            if (values.size() == count) {
                QDataStream stream(values);
                quint8 r;
                quint8 g;
                quint8 b;
                quint8 a;
                stream >> r >> g >> b >> a;
                const GenericColor rgb(r, g, b);
                const RgbColor myRgbColor = RgbColor::fromRgb_255(rgb);
                return CompleteColor::fromSRgbColor(myRgbColor, a / 255.);
            }
        } else if (format == "float") {
            constexpr qsizetype fieldSize = sizeof(float);
            if (values.size() == count * fieldSize) {
                QDataStream stream(values);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
                float r;
                float g;
                float b;
                float a;
                stream >> r >> g >> b >> a;
                const GenericColor rgb(static_cast<double>(r), //
                                       static_cast<double>(g), //
                                       static_cast<double>(b));
                const RgbColor myRgbColor = RgbColor::fromRgb_1(rgb);
                return CompleteColor::fromSRgbColor(myRgbColor, //
                                                    static_cast<double>(a));
            }
        } else if (format == "double") {
            constexpr qsizetype fieldSize = sizeof(double);
            if (values.size() == count * fieldSize) {
                QDataStream stream(values);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
                double r;
                double g;
                double b;
                double a;
                stream >> r >> g >> b >> a;
                const GenericColor rgb(r, g, b);
                const RgbColor myRgbColor = RgbColor::fromRgb_1(rgb);
                return CompleteColor::fromSRgbColor(myRgbColor, a);
            }
        }
    }

    if (model == "HSVA") {
        constexpr uchar count = 4;
        if (format == "u8") {
            if (values.size() == count) {
                QDataStream stream(values);
                quint8 h;
                quint8 s;
                quint8 v;
                quint8 a;
                stream >> h >> s >> v >> a;
                const GenericColor hsv(h, s, v);
                const RgbColor myRgbColor = RgbColor::fromHsv_360_100_100(hsv);
                return CompleteColor::fromSRgbColor(myRgbColor, a / 255.);
            }
        } else if (format == "float") {
            constexpr qsizetype fieldSize = sizeof(float);
            if (values.size() == count * fieldSize) {
                QDataStream stream(values);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
                float h;
                float s;
                float v;
                float a;
                stream >> h >> s >> v >> a;
                const GenericColor hsv(static_cast<double>(h) * 360., //
                                       static_cast<double>(s) * 100., //
                                       static_cast<double>(v) * 100.);
                const RgbColor myRgbColor = RgbColor::fromHsv_360_100_100(hsv);
                return CompleteColor::fromSRgbColor(myRgbColor, //
                                                    static_cast<double>(a));
            }
        } else if (format == "double") {
            constexpr qsizetype fieldSize = sizeof(double);
            if (values.size() == count * fieldSize) {
                QDataStream stream(values);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
                double h;
                double s;
                double v;
                double a;
                stream >> h >> s >> v >> a;
                const GenericColor hsv(h * 360., s * 100., v * 100.);
                const RgbColor myRgbColor = RgbColor::fromHsv_360_100_100(hsv);
                return CompleteColor::fromSRgbColor(myRgbColor, a);
            }
        }
    }

    if (model == "CIE LCH(ab) alpha") {
        constexpr uchar count = 4;
        if (format == "float") {
            constexpr qsizetype fieldSize = sizeof(float);
            if (values.size() == count * fieldSize) {
                QDataStream stream(values);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream.setFloatingPointPrecision(QDataStream::SinglePrecision);
                float l;
                float c;
                float h;
                float a;
                stream >> l >> c >> h >> a;
                const GenericColor lch(static_cast<double>(l), //
                                       static_cast<double>(c), //
                                       static_cast<double>(h));
                if (AbsoluteColor::isLchInSRgbGamut(lch, LchSpace::CielchD50)) {
                    const auto abs = //
                        AbsoluteColor::allConversions(ColorModel::CielchD50, //
                                                      lch);
                    return CompleteColor::fromAbsColor(abs, //
                                                       static_cast<double>(a));
                }
            }
        } else if (format == "double") {
            constexpr qsizetype fieldSize = sizeof(double);
            if (values.size() == count * fieldSize) {
                QDataStream stream(values);
                stream.setByteOrder(QDataStream::LittleEndian);
                stream.setFloatingPointPrecision(QDataStream::DoublePrecision);
                double l;
                double c;
                double h;
                double a;
                stream >> l >> c >> h >> a;
                const GenericColor lch(l, c, h);
                if (AbsoluteColor::isLchInSRgbGamut(lch, LchSpace::CielchD50)) {
                    const auto abs = //
                        AbsoluteColor::allConversions(ColorModel::CielchD50, //
                                                      lch);
                    return CompleteColor::fromAbsColor(abs, a);
                }
            }
        }
    }

    return CompleteColor();
}

/*!
 * \brief Controls how many additional fractional digits are displayed
 * beyond the property's default precision.
 *
 * \a newExtraPrecision Valid range: [0..9]
 *
 * This affects only how the values are presented visually to the user.
 * It does not alter the interal precision of the calculations.
 *
 * \sa ColorDialogPrivate::m_extraPrecision
 */
void CustomColorPatch::setExtraPrecision(const int newExtraPrecision)
{
    const int effectiveNewExtraPrecision = std::clamp(newExtraPrecision, 0, 9);
    d_pointer->m_extraPrecision = effectiveNewExtraPrecision;
}

/*!
 * \brief React on key press events: Implements Copy&Paste.
 *
 * \a eventParameter The key event.
 *
 * \note Also the mouse selection (middle mouse click on Linux) is supported,
 * see \l mousePressEvent.
 */
void CustomColorPatch::keyPressEvent(QKeyEvent *eventParameter)
{
    if (eventParameter->matches(QKeySequence::Copy)) {
        if (color().isValid()) {
            // QClipboard::setMimeData will take ownership of mime data and is
            // responsible for deleting it:
            QGuiApplication::clipboard()->setMimeData( //
                CustomColorPatchPrivate::createMimeData(color()));
            eventParameter->accept();
            return;
        }
    }

    if (eventParameter->matches(QKeySequence::Paste)) {
        const auto pasteColor = d_pointer->fromMimeToCompleteColor( //
            QGuiApplication::clipboard()->mimeData());
        if (pasteColor.isValid) {
            setColorData(pasteColor);
        };
        eventParameter->accept();
    }

    QWidget::keyPressEvent(eventParameter);
}

} // namespace PerceptualColor
