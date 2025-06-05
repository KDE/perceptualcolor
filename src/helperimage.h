// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef HELPERIMAGE_H
#define HELPERIMAGE_H

#include "genericcolor.h"
#include "helperqttypes.h"
#include <lcms2.h>
#include <qcolor.h>
#include <qglobal.h>
#include <qthread.h>
#include <qthreadpool.h>
#include <type_traits>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

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
