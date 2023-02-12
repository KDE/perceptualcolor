﻿// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef IOHANDLERFACTORY_H
#define IOHANDLERFACTORY_H

#include "lcms2.h"
#include <qglobal.h>
#include <qstring.h>

namespace PerceptualColor
{
/** @internal
 *
 * @brief Portable and Unicode-enabled file access for
 * LittleCMS IO handlers.
 *
 * @internal
 *
 * This class is necessary because LittleCMS does not provide portable
 * support for Unicode file names.
 *
 * - LittleCMS allows to open profiles directly from file names, but with
 *   a <tt>char*</tt> as argument for the file name. This leads to non-portable
 *   behaviour because the file name encoding differs between operation
 *   systems. And on Windows, apparently, you even need to use wide-characters
 *   to get support for Unicode file names. This does not work for us because
 *   LittleCMS requires narrow-characters for the file name. For the same
 *   reason, QFile::decodeName() and QFile::encodeName() would not help us
 *   either doing this conversion. And we would have to use either <tt>/</tt>
 *   or <tt>\\</tt> as directory separator, depending on the OS. C itself
 *   does not seem to provide any portable methods for working with  Unicode
 *   file names. So this solution is both: not portable and not comfortable.
 *
 * - LittleCMS allows to open profiles directly from <tt>FILE *</tt>. We
 *   could open a file in <tt>QFile</tt>, call <tt>QFile::handle()</tt>
 *   to get a file descriptor, and use <tt>dup</tt> to create a duplicate.
 *   Then, close the <tt>QFile</tt>, and use <tt>fdopen</tt> to create a
 *   <tt>FILE *</tt> from the duplicate file descriptor. (We need to create
 *   a duplicate of the file handle because otherwise, during the following
 *   calls, fclose() might be called by LittleCMS before QFile::close is
 *   called. This leads to undefined behaviour in some rare cases:
 *   https://stackoverflow.com/questions/9465727/convert-qfile-to-file)
 *   However, <tt>fdopen()</tt> and <tt>dup()</tt> are not part of C, but
 *   of Posix; this might not be portable. Windows, for example, has
 *   no <tt>fdopen()</tt> function but a <tt>_fdopen()</tt> function.
 *   Other systems might not have them at all. So this solution is
 *   not portable.
 *
 * - C++17 has <tt>filesystem::path</tt>, which seams to open files with
 *   Unicode file names. But it does not allow to generate C-Standard
 *   <tt>FILE</tt> handles, which would be necessary to connect with
 *   LittleCMS, which has a C-only interface.
 * - Using an external library (maybe the boost library) for portable
 *   file name handling would introduce another external dependency,
 *   what we do not want.

 * - LittleCMS allows to open profiles directly from a memory buffer.
 *   We could use <tt>QFile</tt> for portability, load the hole file into
 *   a memory buffer and let LittleCMS work with the memory buffer.
 *   The disadvantage would be that it raises our memory usage. This might
 *   be problematic if the ICC profile has various megabytes. It might
 *   produce a crash, when by mistake the file name points to a 10-GB
 *   video file, which would have to load completely into the buffer before
 *   being rejected.
 *
 * Therefore, this class provides a custom LittleCMS IO handler which
 * internally (but invisible for LittleCMS) relies on QFile. This gives
 * us Qt’s portability and avoids the above-mentioned disadvantages. */
class IOHandlerFactory
{
public:
    [[nodiscard]] static cmsIOHANDLER *createReadOnly(cmsContext ContextID, const QString &fileName);

private:
    Q_DISABLE_COPY(IOHandlerFactory)

    /** @internal
     *
     * @brief No constructor, because no objects of this class are
     * necessary. */
    IOHandlerFactory() = delete;

    static cmsBool close(cmsIOHANDLER *iohandler);
    static cmsUInt32Number read(cmsIOHANDLER *iohandler, void *Buffer, cmsUInt32Number size, cmsUInt32Number count);
    static cmsBool seek(cmsIOHANDLER *iohandler, cmsUInt32Number offset);
    [[nodiscard]] static cmsUInt32Number tell(cmsIOHANDLER *iohandler);
    static cmsBool write(cmsIOHANDLER *iohandler, cmsUInt32Number size, const void *Buffer);
};

} // namespace PerceptualColor

#endif // IOHANDLERFACTORY_H
