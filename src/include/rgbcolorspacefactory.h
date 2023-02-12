// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef RGBCOLORSPACEFACTORY_H
#define RGBCOLORSPACEFACTORY_H

#include "importexport.h"
#include <qglobal.h>
#include <qsharedpointer.h>
#include <qstring.h>

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qcontainerfwd.h>
#else
#include <qstringlist.h>
#endif

namespace PerceptualColor
{
class RgbColorSpace;

/** @brief Factory for color space objects.
 *
 * These color space objects are needed in the constructors of various classes
 * of this library.
 *
 * Creating color space objects can be slow. But once created, they can be
 * used simultaneously on various widgets. Thanks to the QSharedPointer, you
 * can easily create a color space object, pass it to the widget constructors
 * you like, and then forget about it – it will be deleted automatically when
 * the last widget that used it has been deleted. And passing the shared
 * pointer to widget constructors is fast! Usage example:
 *
 * @snippet testrgbcolorspacefactory.cpp Create */
class PERCEPTUALCOLOR_IMPORTEXPORT RgbColorSpaceFactory
{
public:
    // No Q_INVOKABLE here because the class does not inherit QObject:
    [[nodiscard]] static QSharedPointer<PerceptualColor::RgbColorSpace> createSrgb();
    [[nodiscard]] static QSharedPointer<PerceptualColor::RgbColorSpace> createFromFile(const QString &fileName);
    [[nodiscard]] static QStringList colorProfileDirectories();

private:
    /** @internal
     *
     * @brief Private default constructor.
     *
     * This class should have no instances, therefore the constructor
     * is private. */
    RgbColorSpaceFactory() = default;

    /** @internal
     *
     * @brief Only for unit tests. */
    friend class TestRgbColorSpaceFactory;
};

} // namespace PerceptualColor

#endif // RGBCOLORSPACEFACTORY_H
