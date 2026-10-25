// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_ASYNCIMAGEPROVIDERBASE_H
#define PERCEPTUALCOLOR_ASYNCIMAGEPROVIDERBASE_H

#include <qglobal.h>
#include <qobject.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{
/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Base class for \l AsyncImageProvider.
 *
 *
 *
 * \note \l {https://stackoverflow.com/a/63021891} {The Q_OBJECT
 * macro and templates cannot be combined.} Therefore,
 * \l AsyncImageProviderBase serves as a base class to provide
 * signals for \l AsyncImageProvider. */
class AsyncImageProviderBase : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AsyncImageProviderBase(QObject *parent = nullptr);
    virtual ~AsyncImageProviderBase() noexcept override;

Q_SIGNALS:
    /*!
     * \brief Signals that the background rendering has completed an
     * interlacing pass.
     *
     * New image data is available now at \l AsyncImageProvider::getCache().
     *
     * \note Even after changing image parameters with
     * \l AsyncImageProvider::setImageParameters() a possibly
     * running render process might not stop immediately and continue
     * to deliver data, therefore also emitting this signal.
     *
     * \sa AsyncImageProvider::refreshAsync()
     */
    void interlacingPassCompleted();

private:
    Q_DISABLE_COPY(AsyncImageProviderBase)

    friend class TestAsyncImageProviderBase;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_ASYNCIMAGEPROVIDERBASE_H
