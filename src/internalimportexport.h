// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_INTERNALIMPORTEXPORT_H
#define PERCEPTUALCOLOR_INTERNALIMPORTEXPORT_H

#include <qglobal.h>

/*!
 *
 *
 * \macro PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT
 *
 * \relates PerceptualColor
 *
 * \brief Import/export macro for internal builds.
 *
 * To use this macro, include \c {internalimportexport.h}
 *
 * This library can be built in two modes:
 *
 * \list
 *     \li Regular build: Only the symbols of the public API are exported.
 *     \li Internal build: All symbols are exported. This mode is used for
 *         white‑box unit testing and internal tools such as the screenshot
 *         generator.
 * \endlist
 *
 * Exporting all symbols can be handled automatically by certain CMake options.
 * However, on Windows (unlike on Linux or BSD), these options only export the
 * class itself and not the Qt‑generated code created by the
 * \l {https://doc.qt.io/qt-6/qobject.html#Q_OBJECT} {Q_OBJECT} macro.
 * In such cases, this macro ensures that the complete set of symbols is
 * exported. It works like \l PERCEPTUALCOLOR\_IMPORTEXPORT but only
 * exports on internal builds, not on regular builds.
 */

#ifndef PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT

#ifdef PERCEPTUALCOLOR_INTERNAL
#ifdef PERCEPTUALCOLOR_STATIC
#define PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT
#else
#ifdef PERCEPTUALCOLOR_BUILD_DYNAMIC_LIBRARY
#define PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT Q_DECL_EXPORT
#else
#define PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT Q_DECL_IMPORT
#endif
#endif
#else
#define PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT
#endif

#endif // #ifndef PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT

#endif // PERCEPTUALCOLOR_INTERNALIMPORTEXPORT_H
