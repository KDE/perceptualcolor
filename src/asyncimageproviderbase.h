// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef ASYNCIMAGEPROVIDERBASE_H
#define ASYNCIMAGEPROVIDERBASE_H

#include <qglobal.h>
#include <qobject.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#else
#include <qobjectdefs.h>
#include <qstring.h>
#endif

namespace PerceptualColor
{
/** @internal
 *
 * @brief Base class for @ref AsyncImageProvider.
 *
 * @internal
 *
 * @note <a href="https://stackoverflow.com/a/63021891">The <tt>Q_OBJECT</tt>
 * macro and templates cannot be combined.</a> Therefore,
 * @ref AsyncImageProviderBase serves as a base class to provide
 * signals for @ref AsyncImageProvider. */
class AsyncImageProviderBase : public QObject
{
    Q_OBJECT

public:
    Q_INVOKABLE explicit AsyncImageProviderBase(QObject *parent = nullptr);
    virtual ~AsyncImageProviderBase() noexcept override;

Q_SIGNALS:
    /** @brief Signals that the background rendering has completed an
     * interlacing pass.
     *
     * New image data is available now at @ref AsyncImageProvider::getCache().
     *
     * @note Even after changing image parameters with
     * @ref AsyncImageProvider::setImageParameters() a possibly
     * running render process might not stop immediately and continue
     * to deliver data, therefore also emitting this signal.
     *
     * @sa @ref AsyncImageProvider::refreshAsync() */
    void interlacingPassCompleted();

private:
    Q_DISABLE_COPY(AsyncImageProviderBase)

    /** @internal @brief Only for unit tests. */
    friend class TestAsyncImageProviderBase;
};

} // namespace PerceptualColor

#endif // ASYNCIMAGEPROVIDERBASE_H
