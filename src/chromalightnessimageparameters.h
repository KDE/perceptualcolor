// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_CHROMALIGHTNESSIMAGEPARAMETERS_H
#define PERCEPTUALCOLOR_CHROMALIGHTNESSIMAGEPARAMETERS_H

#include "internalimportexport.h"
#include "perceptualcolornamespace.h"
#include <qglobal.h>
#include <qmetatype.h>
#include <qsize.h>
#include <qvariant.h>

namespace PerceptualColor
{

class AsyncImageRenderCallback;

/*!
 *
 *
 * \inmodule ModulePrivateApi
 *
 *  \brief An image of a chroma-lightness plane.
 *
 * This is a cut through the gamut body at a given hue.
 *
 * For the y axis, its height covers the lightness range [0, 100].
 * Coordinate point \c {(0)} corresponds to value 100.
 * Coordinate point \c {height} corresponds to value 0.
 * Its x axis uses always the same scale as the y axis. So if the size
 * is a square, both x range and y range are from 0 to 100. If the
 * width is larger than the height, the x range goes beyond 100. The
 * image paints all the LCH values that are within the gamut and x/y range.
 * Each pixel show the color of the coordinate point at its center. So
 * the pixel at pixel position \c {(2, 3)} shows the color corresponding
 * to coordinate point \c {(2.5, 3.5)}.
 */
class PERCEPTUALCOLOR_INTERNAL_IMPORTEXPORT ChromaLightnessImageParameters
{
public:
    /*!
     * \brief Equal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if equal, \c {false} otherwise.
     */
    [[nodiscard]] bool operator==(const ChromaLightnessImageParameters &other) const = default;

    /*!
     * \brief Unequal operator
     *
     * \a other The object to compare with.
     *
     * Returns \c {true} if unequal, \c {false} otherwise.
     */
    [[nodiscard]] bool operator!=(const ChromaLightnessImageParameters &other) const = default;

    static void render(const QVariant &variantParameters, AsyncImageRenderCallback &callbackObject);

    /*!
     * \brief The LCH-hue.
     *
     * Valid range: 0° ≤ value < 360° */
    qreal hue = 0;
    /*!
     *\brief Image size, measured in physical pixels. */
    QSize imageSizePhysical;
    /*!
     * \brief The color space into which the working space is projected.
     */
    LchSpace projectionSpace = LchSpace::CielchD50;

private:
    friend class TestChromaLightnessImageParameters;

    /*!
     * \brief Calculate one-dimensional index for given \c {x} and
     * \c {y} coordinates.
     *
     * \a x The \c {x} coordinate
     *
     * \a y The \c {y} coordinate
     *
     * \a imageSizePhysical The image size
     *
     * Returns the corresponding index, assuming a one-dimensional array
     * that contains one element for each pixel, starting with the elements
     * \c {(0, 0)}, than \c {(0, 1)} and so on, line by line. */
    [[nodiscard]] static constexpr int maskIndex(const int x, const int y, const QSize imageSizePhysical)
    {
        return x + y * imageSizePhysical.width();
    }

    static void
    renderByRow(uchar *const bytesPtr, const qsizetype bytesPerLine, const ChromaLightnessImageParameters parameters, const int firstRow, const int lastRow);
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::ChromaLightnessImageParameters)

#endif // PERCEPTUALCOLOR_CHROMALIGHTNESSIMAGEPARAMETERS_H
