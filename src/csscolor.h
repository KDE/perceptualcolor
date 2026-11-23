// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CSSCOLOR_H
#define PERCEPTUALCOLOR_CSSCOLOR_H

#include "completecolor.h"
#include "genericcolor.h"
#include "internalimportexport.h"
#include "perceptualcolornamespace.h"
#include <optional>
#include <qcontainerfwd.h>
#include <qhash.h>
#include <qrgb.h>
#include <qstring.h>
#include <qtmetamacros.h>

namespace PerceptualColor
{

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 * \brief Toolbox for CSS colors. */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT CssColor
{
public:
    [[nodiscard]] static QString cssRgbHex(const double red255, const double green255, const double blue255);
    [[nodiscard]] static QString cssRgbHex(const double red255, const double green255, const double blue255, const double alpha255);
    [[nodiscard]] static CompleteColor parseToCompleteColor(const QString &string);
    [[nodiscard]] static QStringList generateCss(const CompleteColor &color, const int extraPrecision);

    friend class TestCssColor;
};

} // namespace PerceptualColor

#endif // PERCEPTUALCOLOR_CSSCOLOR_H
