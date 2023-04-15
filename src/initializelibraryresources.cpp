// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "initializelibraryresources.h"

#include <qglobal.h>

/** @internal @file
 *
 * Provides the @ref PerceptualColor::initializeLibraryResources() function. */

/** @internal
 *
 * @brief Helper function for @ref PerceptualColor::initializeTranslation().
 *
 * <tt>Q_INIT_RESOURCE</tt> may not be used within a namespace.
 * Therefore exists this function which is not part of any namespace. To avoid
 * pollution of the global namespace, it is declared <tt>static</tt> which
 * gives here internal linkage, so it is not visible outside of this
 * compilation unit. */
static void helperInitializeLibraryResources()
{
    // Resource initialization is necessary on some systems for static
    // libraries, but also recommended for dynamic libraries. For details see
    // https://doc.qt.io/qt-6/resources.html#using-resources-in-a-library
    // The argument of this macro is the name of the .qrc file (without
    // the .qrc suffix) just as in the corresponding CMakeLists.txt.
    // This macro relies on qRegisterResourceData() which checks if
    // the resource is yet loaded: If so, it is not loaded again.
    Q_INIT_RESOURCE(resourcelist);
}

namespace PerceptualColor
{

/** @internal
 *
 * @brief Initializes the resources of the <tt>Qt resource system</tt> that
 * come with this library.
 *
 * It is safe to call this function multiple times: If the resources are
 * yet initialized, nothing happens.
 *
 * @post The resources are initialized. */
void initializeLibraryResources()
{
    helperInitializeLibraryResources();
}

} // namespace PerceptualColor
