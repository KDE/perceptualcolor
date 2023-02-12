﻿// SPDX-FileCopyrightText: 2020-2023 Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

// Own headers
// First the interface, which forces the header to be self-contained.
#include "iohandlerfactory.h"

#include "helpermath.h"
#include <lcms2.h>
#include <lcms2_plugin.h>
#include <limits>
#include <qdebug.h>
#include <qfile.h>
#include <qglobal.h>
#include <qiodevice.h>
#include <qstringliteral.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qiodevicebase.h>
#endif

namespace PerceptualColor
{
/** @internal
 *
 * @brief Read from file.
 *
 * @param iohandler The <tt>cmsIOHANDLER</tt> on which to operate
 * @param Buffer Pointer to the buffer to which the data should be loaded
 * @param size Size of the chucks that should be loaded
 * @param count Number of elements that should be loaded
 * @returns On success, <tt>count</tt> is returned. If failing (either
 * because less elements have been read or because zero elements have
 * been read), <tt>0</tt> is returned. */
cmsUInt32Number IOHandlerFactory::read(cmsIOHANDLER *iohandler, void *Buffer, cmsUInt32Number size, cmsUInt32Number count)
{
    QFile *const myFile = static_cast<QFile *>(iohandler->stream);
    const cmsUInt32Number numberOfBytesRequested = size * count;
    const qint64 numberOfBytesRead = myFile->read( //
        static_cast<char *>(Buffer),
        size * count);
    if (numberOfBytesRead != numberOfBytesRequested) {
        return 0;
    }
    return count;
}

/** @internal
 *
 * @brief Sets the current position within the file.
 *
 * @param iohandler The <tt>cmsIOHANDLER</tt> on which to operate
 * @param offset Set the current position to this position
 * @returns <tt>true</tt> on success, or <tt>false</tt> if an error
 * occurred. */
cmsBool IOHandlerFactory::seek(cmsIOHANDLER *iohandler, cmsUInt32Number offset)
{
    QFile *const myFile = static_cast<QFile *>(iohandler->stream);
    const bool seekSucceeded = myFile->seek(offset);
    if (!seekSucceeded) {
        qDebug() << QStringLiteral("Seek error; probably corrupted file");
        return false;
    }
    return true;
}

/** @internal
 *
 * @brief The position that data is written to or read from.
 * @param iohandler The <tt>cmsIOHANDLER</tt> on which to operate
 * @returns The position that data is written to or read from. */
cmsUInt32Number IOHandlerFactory::tell(cmsIOHANDLER *iohandler)
{
    const QFile *const myFile = static_cast<QFile *>(iohandler->stream);
    return static_cast<cmsUInt32Number>(myFile->pos());
}

/** @internal
 *
 * @brief Writes data to stream.
 *
 * Also keeps used space for further reference.
 *
 * @param iohandler The <tt>cmsIOHANDLER</tt> on which to operate
 * @param size The size of the buffer that should be written
 * @param Buffer The buffer that should be written
 * @returns Returns <tt>true</tt> on success, <tt>false</tt> on error.
 *
 * @note Because @ref IOHandlerFactory only provides support for read-only
 * handlers, this function does nothing and returns always <tt>false</tt>. */
cmsBool IOHandlerFactory::write(cmsIOHANDLER *iohandler, cmsUInt32Number size, const void *Buffer)
{
    Q_UNUSED(iohandler)
    Q_UNUSED(size)
    Q_UNUSED(Buffer)
    return false;
}

/** @internal
 *
 * @brief Closes the file and deletes the file handler.
 * @param iohandler The <tt>cmsIOHANDLER</tt> on which to operate
 * @returns <tt>true</tt> on success. */
cmsBool IOHandlerFactory::close(cmsIOHANDLER *iohandler)
{
    QFile *const myFile = static_cast<QFile *>(iohandler->stream);
    delete myFile; // This will also close the file.
    iohandler->stream = nullptr;
    _cmsFree(iohandler->ContextID, iohandler);
    return true;
}

/** @brief Create a read-only LittleCMS IO handler for a file.
 *
 * The handler has to be deleted with <tt>cmsCloseIOhandler</tt>
 * to free memory once it is not used anymore.
 *
 * @param ContextID Handle to user-defined context, or <tt>nullptr</tt> for
 * the global context
 * @param fileName Name of the file. See QFile::setFileName() for
 * the valid format. This format is portable, has standardized directory
 * separators and supports Unicode file names on all platforms.
 * @returns On success, a pointer to a new IO handler. On fail,
 * <tt>nullptr</tt>. The function might fail when the file does not
 * exist or cannot be opened for reading.
 *
 * @internal
 *
 * @note The type of the return value is not fully defined
 * in <tt>lcms2.h</tt> but only in <tt>lcms2_plugin.h</tt>. This is
 * the expected behaviour for an opaque handle. */
cmsIOHANDLER *IOHandlerFactory::createReadOnly(cmsContext ContextID, const QString &fileName)
{
    cmsIOHANDLER *const result = static_cast<cmsIOHANDLER *>( //
        _cmsMallocZero(ContextID, sizeof(cmsIOHANDLER)) //
    );
    if (result == nullptr) {
        return nullptr;
    }

    QFile *const fileObject = new QFile(fileName);
    if (fileObject == nullptr) {
        return nullptr;
    }

    const bool openSucceeded = fileObject->open(QIODevice::ReadOnly);
    const qint64 fileSize = fileObject->size();
    // Check if the size is not negative (this might be an error indicator)
    // neither too big for LittleCMS’s data types:
    const bool isFileSizeOkay = PerceptualColor::isInRange<qint64>( //
        0,
        fileSize,
        std::numeric_limits<cmsInt32Number>::max());
    if ((!openSucceeded) || (!isFileSizeOkay)) {
        delete fileObject;
        _cmsFree(ContextID, result);
        return nullptr;
    }

    // Initialize data members
    result->ContextID = ContextID;
    // static_cast loses integer precision: 'qint64' to 'cmsInt32Number'.
    // This is okay because we have tested yet that the file is not that big.
    result->ReportedSize = static_cast<cmsUInt32Number>(fileSize);
    result->stream = static_cast<void *>(fileObject);
    result->UsedSpace = 0;
    result->PhysicalFile[0] = 0;

    // Initialize function pointers
    result->Read = read;
    result->Seek = seek;
    result->Close = close;
    result->Tell = tell;
    result->Write = write;

    return result;
}

} // namespace PerceptualColor
