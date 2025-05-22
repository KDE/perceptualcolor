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

[[nodiscard]] QList<QPoint> findBoundary(const QImage &image);

/**
 * @brief Thread priority for calculating images in the background.
 */
constexpr QThread::Priority imageThreadPriority = QThread::Priority::LowPriority;

QThreadPool &getLibraryQThreadPoolInstance();

} // namespace PerceptualColor

#endif // HELPERIMAGE_H
