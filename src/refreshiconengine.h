// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef REFRESHICONENGINE_H
#define REFRESHICONENGINE_H

#include <qicon.h>
#include <qiconengine.h>
#include <qpixmap.h>
#include <qpointer.h>
#include <qwidget.h>
class QPainter;
class QRect;
class QSize;

namespace PerceptualColor
{
/** @internal
 *
 * @brief An fail-safe icon engine that provides a <em>refresh</em> icon.
 *
 * This icon engine provides a <em>refresh</em> icon. It is fail-safe,
 * which means that it will never fail to provide an icon: This icon
 * engine will always return a valid, non-null icon. It does a best
 * effort to find an icon that integrates well with the current icon
 * theme and widget style. It searches in various places to provide
 * this icon, eventually using the first one that it finds:
 * 1. The <tt>view-refresh</tt> icon provided by the current FreeDesktop
 *    icon theme, either on your system or bundled as resource with
 *    the application that uses this library. On Linux, it is common
 *    that an icon theme is provided. Which icon formats (SVG, PNG …)
 *    are supported depends on your Qt installation. On
 *    <a href="https://kate-editor.org/post/2021/2021-03-07-cross-platform-light-dark-themes-and-icons/">
 *    some platforms like KDE</a> the icons get automatically adapted to
 *    dark and light mode, on others not.
 * 2. The <tt>QStyle::StandardPixmap::SP_BrowserReload</tt> icon provided
 *    by the current <tt>QStyle</tt>.
 * 3. The fallback build-in icon that is hard-coded within this class.
 *    This icon is resolution-independent with High-DPI support (and
 *    is does <em>not</em> require SVG support in Qt for this) and
 *    adapts automatically to the current palette, thus providing
 *    automatically appropriate icon colors for dark mode and light
 *    mode. With the @ref setReferenceWidget() function it can
 *    integrate with a specific widget’s color palette (rather than
 *    the default color palette).
 *
 * This icon engine does not use a cache. That means, the icon will
 * be recalculated each time again. This is less efficient, but it
 * makes sure the icon is always up-to-date, also immediately after
 * the widget style or the icon theme or both have changed. */
class RefreshIconEngine : public QIconEngine
{
public:
    explicit RefreshIconEngine();
    /** @brief Default destructor. */
    virtual ~RefreshIconEngine() override = default;
    [[nodiscard]] virtual QIconEngine *clone() const override;
    virtual void paint(QPainter *painter, const QRect &rect, QIcon::Mode mode, QIcon::State state) override;
    [[nodiscard]] virtual QPixmap pixmap(const QSize &size, QIcon::Mode mode, QIcon::State state) override;
    void setReferenceWidget(QWidget *referenceWidget);

private:
    // Functions
    explicit RefreshIconEngine(const RefreshIconEngine &other);
    void paintFallbackIcon(QPainter *painter, const QRect rect, QIcon::Mode mode);

    // Data members
    /** @brief Holds a guarded pointer to the reference widget.
     * @sa @ref setReferenceWidget() */
    QPointer<QWidget> m_referenceWidget = nullptr;

    /** @internal @brief Only for unit tests. */
    friend class TestRefreshIconEngine;
};

} // namespace PerceptualColor

#endif // REFRESHICONENGINE_H
