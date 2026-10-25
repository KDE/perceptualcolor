// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "initializelibraryresources.h"

#include <qglobal.h>

// Helper function for PerceptualColor::initializeLibraryResources().
//
// Q_INIT_RESOURCE cannot be used within a namespace. As a workaround,
// this function resides in the global namespace. To prevent namespace
// pollution and external linkage, it is declared static exclusively
// within this translation unit without any corresponding header declaration.
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

/*!
 *
 *
 * \brief Initializes the resources of the \c {Qt resource system} that
 * come with this library.
 *
 * It is safe to call this function multiple times: If the resources are
 * yet initialized, nothing happens.
 *
 *  The resources are initialized.
 */
void initializeLibraryResources()
{
    helperInitializeLibraryResources();
}

} // namespace PerceptualColor
