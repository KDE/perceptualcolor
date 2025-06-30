// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERIMAGE_H
#define HELPERIMAGE_H

#include <functional>
#include <qglobal.h>
#include <qlist.h>
#include <qrgb.h>
#include <qthread.h>
class QImage;
class QPoint;
class QRect;
class QThreadPool;

/** @internal
 *
 * @file
 *
 * Provides functions for image processing. */

namespace PerceptualColor
{

void doAntialias(QImage &image, const QList<QPoint> &antiAliasCoordinates, const std::function<QRgb(const double x, const double y)> &colorFunction);

void fillRect(uchar *const bytesPtr, const qsizetype bytesPerLine, const QRect rectangle, const QRgb color);

[[nodiscard]] QList<QPoint> findBoundary(const QImage &image);

QThreadPool &getLibraryQThreadPoolInstance();

/**
 * @brief Thread priority for calculating images in the background.
 */
constexpr QThread::Priority imageThreadPriority = QThread::Priority::LowPriority;

/**
 * @brief A transparent value, compatible with both the premultiplied and the
 * non-premultiplied format.
 */
constexpr QRgb qRgbTransparent = 0;

} // namespace PerceptualColor

#endif // HELPERIMAGE_H
