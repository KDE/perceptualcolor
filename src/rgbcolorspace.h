// SPDX-FileCopyrightText: Lukas Sommer <sommerluk@gmail.com>
// SPDX-License-Identifier: BSD-2-Clause OR MIT

#ifndef PERCEPTUALCOLOR_RGBCOLORSPACE_H
#define PERCEPTUALCOLOR_RGBCOLORSPACE_H

#include "constpropagatinguniquepointer.h"
#include "genericcolor.h"
#include <lcms2.h>
#include <optional>
#include <qcolor.h>
#include <qcontainerfwd.h>
#include <qdatetime.h>
#include <qglobal.h>
#include <qmap.h>
#include <qmetatype.h>
#include <qobject.h>
#include <qrgb.h>
#include <qsharedpointer.h>
#include <qstring.h>
#include <qversionnumber.h>
class QRgba64;

#if (QT_VERSION >= QT_VERSION_CHECK(6, 0, 0))
#include <qtmetamacros.h>
#endif

Q_DECLARE_METATYPE(cmsColorSpaceSignature)
Q_DECLARE_METATYPE(cmsProfileClassSignature)

namespace PerceptualColor
{
class RgbColorSpacePrivate;

/** @internal
 *
 * @brief Provides access to LittleCMS color management
 *
 * This class has no public constructor. Objects can be generated
 * with the static factory functions.
 *
 * @note The maximum accepted Cielch-D50/Cielab-D50 lightness range is
 * 0 to 100, and the maximum Cielch-D50 chroma is
 * @ref CielchD50Values::maximumChroma. Values outside of this
 * range are considered out-of-gamut, even if the profile
 * itself would accept them.
 *
 * This class is reentrant. Furthermore, it is thread-save to use only
 * <tt>const</tt> functions simultaneously from various threads.
 *
 * @note Major operating systems assume sRGB (with the
 * standard-conformant piecewise gamma function) as the default color
 * space for application-drawn content. This is either because the OS
 * performs active color management and expects sRGB input, or because no
 * color management is applied and most monitors are designed to interpret
 * incoming data as sRGB. Consequently, when drawing in QWidget, colors
 * should be specified in sRGB. Support for Wide Color Gamut (WCG) or High
 * Dynamic Range (HDR) rendering is only achievable by using
 * <a href="https://doc.qt.io/qt-6/qopenglwidget.html">QOpenGLWidget</a> and
 * leveraging OpenGL directly. (<a href="https://doc.qt.io/qt-6/qrhi.html">
 * QRHI</a> is a half-private API with limited stability guaranties and seems
 * less appropriate than QOpenGLWidget.) Since sRGB remains the most widely
 * supported and reliable color space, WCG and HDR are typically of limited
 * practical importance in this context, and support for output color spaces
 * different from sRGB is not a priority.
 *
 * @todo SHOULDHAVE Decouple Gamut and Output Profiles. Problem: This class
 * assumes that the working gamut (used for the color values) and the screen
 * profiles (the profile of the display device) are always identical. This
 * assumption is incorrect. Example: While sRGB for both is the most likely
 * case (sRGB is kind of standard), it's well possible that a Display-P3
 * working gamut is requested, but the output must be mapped/clamped to an
 * sRGB-only display (or the other way around). Action Items: Decide whether
 * this class represents only the gamut profile, only the screen profile or the
 * combination of both. Refactor the class’ interface and rename the class to
 * accurately reflect this change in scope.
 *
 * @todo SHOULDHAVE
 *       Unit tests for @ref RgbColorSpace, especially the to…() functions.
 *
 * @todo SHOULDHAVE
 *       Unit tests for @ref profileMaximumCielchD50Chroma and
 *       @ref profileMaximumOklchChroma with all profiles that are available
 *       in the testbed.
 *
 * @todo SHOWSTOPPER Use (only or alternatively) Oklab
 * instead of CIELAB. But not less common
 * <a href="https://en.wikipedia.org/wiki/Uniform_color_space">
 * perceptually uniform color spaces</a> like CAM16 or
 * <a href="https://de.wikipedia.org/wiki/DIN99-Farbraum">DIN99</a> or Googles
 * <a href="https://github.com/material-foundation/material-color-utilities">
 * HCT</a> (they also use
 * <a href="https://m3.material.io/blog/science-of-color-design">HCT in their
 * Material design system</a>). Also the @ref profileMaximumCielchD50Chroma
 * would need to have companion functions provided for all the new color
 * spaces individually.
 *
 * @todo NICETOHAVE
 *       The sRGB colour space object should be implemented as a singleton.
 *       This is possible because it is thread-safe, and therefore it does
 *       not make sense to have more than one object of this class. At the
 *       same time, it is necessary that it implements the common interface
 *       of the colour space objects that are created on-the-fly from ICC
 *       profile files, therefore it cannot be static.
 *       As a consequence, translations within sRGB objects should always
 *       be dynamic instead of being done only once at instantiation time,
 *       because now the instantiation time is out of control of the library
 *       user. (And maybe even for ICC profiles we could provide ALL
 *       translations, be reading ALL possible translations at creating time
 *       and guarding them? Or would this be overkill?)
 *       The singleton pattern has special requirements
 *       for: 1) thread-safety. 2)  dynamic libraries. See Wikipedia
 *       for details!
 *
 * @todo NICETOHAVE
 *       Is it possible to split this into an interface and into
 *       various implementations (a slow but safe implementation for
 *       all valid ICC files, and a fast optimized implementation for sRGB
 *       only? If so, is it possible to get rid of the dependency from
 *       LittleCMS by implementing sRGB ourselves, and providing ICC support
 *       via an optional header-only header that would link against LittleCMS
 *       without injecting this dependency into our shared library? And
 *       this might be faster! The web page
 *       https://bottosson.github.io/misc/colorpicker/#91a7ee is only
 *       JavaScript and works faster than this library! See
 *       https://en.wikipedia.org/wiki/SRGB#From_sRGB_to_CIE_XYZ and
 *       http://www.brucelindbloom.com/index.html?Math.html and
 *       http://www.brucelindbloom.com/index.html?Eqn_RGB_XYZ_Matrix.html
 *       for implementation details.
 *
 * @todo SHOULDHAVE
 *       In the API of this class, clarify the precision. If more than
 *       8 bit per channel, we have to switch from QRgb to QRgb64. But
 *       probably all OS APIs only accept 8 bit anyway? Is it worth the
 *       pain just because @ref ColorDialog can return <tt>QColor</tt>
 *       which provides 16 bit support?
 *
 * @todo NICETOHAVE If almost all RGB profiles are matrix-based and not
 * LUT-based (look-up table based), may there exist an algorithmic way to
 * calculate the gamut boundary in Oklab or CIELab projections?
 *
 * @todo SHOULDHAVE Restructure this class? Use two different classes instead,
 * one for the working color space (defines the gamut available in the color
 * dialog) and another one that represents the output screen color space?
 *
 * @todo SHOULDHAVE To determine whether a specific value is in-gamut or
 * out-of-gamut, this class converts Oklch or CIELch values to RGB using an
 * RGB ICC color profile. If the resulting RGB values are out of range, the
 * original color is considered out-of-gamut. However, we know that this
 * behavior in LittleCMS works with some RGB memory formats, but fails with
 * others. Is it also possible that this entire approach may not work with
 * certain (unusual) ICC profiles? Could we test this during profile creation?
 * For example, by converting a clearly out-of-gamut Oklch or CIELch value and
 * rejecting the profile creation if the resulting RGB value appears in-gamut?
 *
 * @todo SHOULDHAVE When loading an ICC profile from a file, load first the
 * file into a <tt>QByteArray</tt>, then keep it permanently in memory.
 * Document this behaviour and warn the library user about the memory
 * consumtion. Provide on-the-fly translated ICC fields (for on-the-fly
 * application language changes), based on the <tt>QByteArray</tt>. Add
 * functions that accept directly a <tt>QByteArray</tt> instead of a file
 * path.
 *
 * @todo NCETOHAVE Use cmsHPROFILE cmsCreate_OkLabProfile(cmsContext ctx)
 * instead fo calculating Oklab values ourself. For performance reasons.
 * But it's available sind LittleCMS 2.16 only!
 *
 * @todo NICETOHAVE
 *       Find more efficient ways of in-gamut detection. Maybe provide
 *       a subclass with optimized algorithms just for sRGB-build-in? */
class RgbColorSpace : public QObject
{
    Q_OBJECT

public: // enums and flags
    /**
     * @brief Enum class representing the possible roles of an ICC profile in
     * color management transforms.
     *
     * This enum class defines the directions in which a profile can be used
     * for creating color transforms. Each flag represents a specific role that
     * the profile can play.
     *
     * This enum is declared to the meta-object system with <tt>Q_ENUM</tt>.
     * This happens automatically. You do not need to make any manual calls.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     */
    enum class ProfileRole {
        Input = 0x01, /**< The profile can be used as input profile. */
        Output = 0x02, /**< The profile can be used as input profile. */
        Proof = 0x04 /**< The profile can be used as input profile. */
    };
    Q_ENUM(ProfileRole)
    /**
     * @brief <tt>Q_DECLARE_FLAGS</tt> for @ref ProfileRole.
     */
    Q_DECLARE_FLAGS(ProfileRoles, ProfileRole)
    /**
     * @brief Type for property @ref profileRenderingIntentDirections
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     */
    using RenderingIntentDirections = QMap<cmsUInt32Number, RgbColorSpace::ProfileRoles>;

private:
    /** @brief The absolute file path of the profile.
     *
     * @note This is empty for build-in profiles.
     *
     * @sa READ @ref profileAbsoluteFilePath() const */
    Q_PROPERTY(QString profileAbsoluteFilePath READ profileAbsoluteFilePath CONSTANT)

    /** @brief The class of the profile.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @sa READ @ref profileClass() const */
    Q_PROPERTY(cmsProfileClassSignature profileClass READ profileClass CONSTANT)

    /** @brief The color model of the color space which is described by
     * this profile.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @sa READ @ref profileColorModel() const */
    Q_PROPERTY(cmsColorSpaceSignature profileColorModel READ profileColorModel CONSTANT)

    /** @brief The copyright information of the profile.
     *
     * If supported by the underlying profile, this property is localized
     * to the current locale <em>at the moment of the constructor call</em>.
     *
     * @note This is empty if the information is not available.
     *
     * @sa READ @ref profileCopyright() const */
    Q_PROPERTY(QString profileCopyright READ profileCopyright CONSTANT)

    /** @brief The date and time of the creation of the profile.
     *
     * @note This is null if the information is not available.
     *
     * @sa READ @ref profileCreationDateTime() const */
    Q_PROPERTY(QDateTime profileCreationDateTime READ profileCreationDateTime CONSTANT)

    /** @brief The file size of the profile, measured in byte.
     *
     * @note This is <tt>-1</tt> for build-in profiles.
     *
     * @sa READ @ref profileFileSize() const */
    Q_PROPERTY(qint64 profileFileSize READ profileFileSize CONSTANT)

    /** @brief Wether or not the profile has a color lookup table (CLUT).
     *
     * @sa READ @ref profileHasClut() const */
    Q_PROPERTY(bool profileHasClut READ profileHasClut CONSTANT)

    /** @brief Wether or not the profile has a matrix shaper.
     *
     * @sa READ @ref profileHasMatrixShaper() const */
    Q_PROPERTY(bool profileHasMatrixShaper READ profileHasMatrixShaper CONSTANT)

    /**
     * @brief Available transform directions of rendering intents.
     *
     * A mapping of all rendering intents supported by LittleCMS, indicating
     * whether they are supported by the given profile and specifying
     * the direction of support.
     * - key: The code corresponding to the rendering intent. Refer
     *   to @ref lcmsIntentList() for additional details.
     * - value: The transformation directions available for each rendering
     *   intent.
     */
    Q_PROPERTY(RenderingIntentDirections profileRenderingIntentDirections READ profileRenderingIntentDirections CONSTANT)

    /** @brief The ICC version of the profile.
     *
     * @sa READ @ref profileIccVersion() const */
    Q_PROPERTY(QVersionNumber profileIccVersion READ profileIccVersion CONSTANT)

    /** @brief The manufacturer information of the profile.
     *
     * If supported by the underlying profile, this property is localized
     * to the current locale <em>at the moment of the constructor call</em>.
     *
     * @note This is empty if the information is not available.
     *
     * @sa READ @ref profileManufacturer() const */
    Q_PROPERTY(QString profileManufacturer READ profileManufacturer CONSTANT)

    /** @brief The maximum CIELch-D50 chroma of the profile.
     *
     * This value is equal or slightly bigger than the actual maximum chroma.
     *
     * @note This is the result of an auto-detection, which might theoretically
     * in very rare cases return a value that is smaller than the actual
     * maximum chroma.
     *
     * @sa READ @ref profileMaximumCielchD50Chroma() const */
    Q_PROPERTY(double profileMaximumCielchD50Chroma READ profileMaximumCielchD50Chroma CONSTANT)

    /** @brief The maximum Oklch chroma of the profile.
     *
     * This value is equal or slightly bigger than the actual maximum chroma.
     *
     * @note This is the result of an auto-detection, which might theoretically
     * in very rare cases return a value that is smaller than the actual
     * maximum chroma.
     *
     * @sa READ @ref profileMaximumOklchChroma() const */
    Q_PROPERTY(double profileMaximumOklchChroma READ profileMaximumOklchChroma CONSTANT)

    /** @brief The model information of the profile.
     *
     * If supported by the underlying profile, this property is localized
     * to the current locale <em>at the moment of the constructor call</em>.
     *
     * @note This is empty if the information is not available.
     *
     * @sa READ @ref profileModel() const */
    Q_PROPERTY(QString profileModel READ profileModel CONSTANT)

    /** @brief The name of the profile.
     *
     * If supported by the underlying profile, this property is localized
     * to the current locale <em>at the moment of the constructor call</em>.
     *
     * Note that this string might be very long in some profiles. On some
     * UI elements, maybe it should be elided (truncate it and put “…” at
     * the end).
     *
     * @note This is empty if the information is not available.
     *
     * @sa READ @ref profileName() const */
    Q_PROPERTY(QString profileName READ profileName CONSTANT)

    /** @brief The name of the profile.
     *
     * If supported by the underlying profile, this property is localized
     * to the current locale <em>at the moment of the constructor call</em>.
     *
     * This type is declared as type to Qt’s type system via
     * <tt>Q_DECLARE_METATYPE</tt>. Depending on your use case (for
     * example if you want to use for <em>queued</em> signal-slot connections),
     * you might consider calling <tt>qRegisterMetaType()</tt> for
     * this type, once you have a QApplication object.
     *
     * @sa READ @ref profilePcsColorModel() const */
    Q_PROPERTY(cmsColorSpaceSignature profilePcsColorModel READ profilePcsColorModel CONSTANT)

    /** @brief Value of the tag <tt>bkpt</tt> if present in the ICC profile,
     *
     * Is <tt>std::nullopt</tt> if the tag is not present in the ICC profile.
     *
     * @sa READ @ref profileTagBlackpoint() const */
    Q_PROPERTY(std::optional<cmsCIEXYZ> profileTagBlackpoint READ profileTagBlackpoint CONSTANT)

    /** @brief Value of the tag <tt>bXYZ</tt> if present in the ICC profile,
     *
     * Is <tt>std::nullopt</tt> if the tag is not present in the ICC profile.
     *
     * @sa READ @ref profileTagBluePrimary() const */
    Q_PROPERTY(std::optional<cmsCIEXYZ> profileTagBluePrimary READ profileTagBluePrimary CONSTANT)

    /** @brief Value of the tag <tt>gXYZ</tt> if present in the ICC profile,
     *
     * Is <tt>std::nullopt</tt> if the tag is not present in the ICC profile.
     *
     * @sa READ @ref profileTagGreenPrimary() const */
    Q_PROPERTY(std::optional<cmsCIEXYZ> profileTagGreenPrimary READ profileTagGreenPrimary CONSTANT)

    /** @brief Value of the tag <tt>rXYZ</tt> if present in the ICC profile,
     *
     * Is <tt>std::nullopt</tt> if the tag is not present in the ICC profile.
     *
     * @sa READ @ref profileTagRedPrimary() const */
    Q_PROPERTY(std::optional<cmsCIEXYZ> profileTagRedPrimary READ profileTagRedPrimary CONSTANT)

    /** @brief The signatures of all tags actually present in the ICC profile.
     *
     * This contains both, “public tags” mentioned in the
     * <a href="https://www.color.org/icc_specs2.xalter">ICC specification</a>
     * itself, and “private tags” which should be registered at the
     * <a href="https://www.color.org/signatures2.xalter">ICC Signature
     * Registry</a>.
     *
     * @sa READ @ref profileTagSignatures() const */
    Q_PROPERTY(QStringList profileTagSignatures READ profileTagSignatures CONSTANT)

    /** @brief Value of the tag <tt>wtpt</tt> if present in the ICC profile,
     *
     * Is <tt>std::nullopt</tt> if the tag is not present in the ICC profile.
     *
     * @sa READ @ref profileTagWhitepoint() const */
    Q_PROPERTY(std::optional<cmsCIEXYZ> profileTagWhitepoint READ profileTagWhitepoint CONSTANT)

public: // Static factory functions
    [[nodiscard]] Q_INVOKABLE static QSharedPointer<PerceptualColor::RgbColorSpace> tryCreateFromFile(const QString &fileName);
    [[nodiscard]] Q_INVOKABLE static QSharedPointer<PerceptualColor::RgbColorSpace> createSrgb();

public:
    virtual ~RgbColorSpace() noexcept override;
    [[nodiscard]] Q_INVOKABLE virtual bool isCielabD50InGamut(const cmsCIELab &lab) const;
    [[nodiscard]] Q_INVOKABLE virtual bool isCielchD50InGamut(const PerceptualColor::GenericColor &lch) const;
    [[nodiscard]] Q_INVOKABLE virtual bool isOklchInGamut(const PerceptualColor::GenericColor &lch) const;
    [[nodiscard]] Q_INVOKABLE QColor maxChromaColorByCielchD50Hue360(double oklabHue360) const;
    [[nodiscard]] Q_INVOKABLE QColor maxChromaColorByOklabHue360(double oklabHue360) const;
    /** @brief Getter for property @ref profileAbsoluteFilePath
     *  @returns the property @ref profileAbsoluteFilePath */
    [[nodiscard]] QString profileAbsoluteFilePath() const;
    /** @brief Getter for property @ref profileClass
     *  @returns the property @ref profileClass */
    [[nodiscard]] cmsProfileClassSignature profileClass() const;
    /** @brief Getter for property @ref profileColorModel
     *  @returns the property @ref profileColorModel */
    [[nodiscard]] cmsColorSpaceSignature profileColorModel() const;
    /** @brief Getter for property @ref profileCopyright
     *  @returns the property @ref profileCopyright */
    [[nodiscard]] QString profileCopyright() const;
    /** @brief Getter for property @ref profileCreationDateTime
     *  @returns the property @ref profileCreationDateTime */
    [[nodiscard]] QDateTime profileCreationDateTime() const;
    /** @brief Getter for property @ref profileFileSize
     *  @returns the property @ref profileFileSize */
    [[nodiscard]] qint64 profileFileSize() const;
    /** @brief Getter for property @ref profileHasClut
     *  @returns the property @ref profileHasClut */
    [[nodiscard]] bool profileHasClut() const;
    /** @brief Getter for property @ref profileHasMatrixShaper
     *  @returns the property @ref profileHasMatrixShaper */
    [[nodiscard]] bool profileHasMatrixShaper() const;
    /** @brief Getter for property @ref profileIccVersion
     *  @returns the property @ref profileIccVersion */
    [[nodiscard]] QVersionNumber profileIccVersion() const;
    /** @brief Getter for property @ref profileRenderingIntentDirections
     *  @returns the property @ref profileRenderingIntentDirections */
    [[nodiscard]] RenderingIntentDirections profileRenderingIntentDirections() const;
    /** @brief Getter for property @ref profileManufacturer
     *  @returns the property @ref profileManufacturer */
    [[nodiscard]] QString profileManufacturer() const;
    /** @brief Getter for property @ref profileMaximumCielchD50Chroma
     *  @returns the property @ref profileMaximumCielchD50Chroma */
    [[nodiscard]] double profileMaximumCielchD50Chroma() const;
    /** @brief Getter for property @ref profileMaximumOklchChroma
     *  @returns the property @ref profileMaximumOklchChroma */
    [[nodiscard]] double profileMaximumOklchChroma() const;
    /** @brief Getter for property @ref profileModel
     *  @returns the property @ref profileModel */
    [[nodiscard]] QString profileModel() const;
    /** @brief Getter for property @ref profileName
     *  @returns the property @ref profileName */
    [[nodiscard]] QString profileName() const;
    /** @brief Getter for property @ref profilePcsColorModel
     *  @returns the property @ref profilePcsColorModel */
    [[nodiscard]] cmsColorSpaceSignature profilePcsColorModel() const;
    /** @brief Getter for property @ref profileTagBlackpoint
     *  @returns the property @ref profileTagBlackpoint */
    [[nodiscard]] std::optional<cmsCIEXYZ> profileTagBlackpoint() const;
    /** @brief Getter for property @ref profileTagBluePrimary
     *  @returns the property @ref profileTagBluePrimary */
    [[nodiscard]] std::optional<cmsCIEXYZ> profileTagBluePrimary() const;
    /** @brief Getter for property @ref profileTagGreenPrimary
     *  @returns the property @ref profileTagGreenPrimary */
    [[nodiscard]] std::optional<cmsCIEXYZ> profileTagGreenPrimary() const;
    /** @brief Getter for property @ref profileTagRedPrimary
     *  @returns the property @ref profileTagRedPrimary */
    [[nodiscard]] std::optional<cmsCIEXYZ> profileTagRedPrimary() const;
    /** @brief Getter for property @ref profileTagSignatures
     *  @returns the property @ref profileTagSignatures */
    [[nodiscard]] QStringList profileTagSignatures() const;
    /** @brief Getter for property @ref profileTagWhitepoint
     *  @returns the property @ref profileTagWhitepoint */
    [[nodiscard]] std::optional<cmsCIEXYZ> profileTagWhitepoint() const;
    // The function declaration is kept on a single line to prevent issues
    // with Doxygen parsing.
    // clang-format is disabled here to prevent automatic line breaks.
    // clang-format off
    [[nodiscard]] Q_INVOKABLE virtual PerceptualColor::GenericColor reduceCielchD50ChromaToFitIntoGamut(const PerceptualColor::GenericColor &cielchD50color) const;
    // clang-format on
    [[nodiscard]] Q_INVOKABLE virtual PerceptualColor::GenericColor reduceOklchChromaToFitIntoGamut(const PerceptualColor::GenericColor &oklchColor) const;
    [[nodiscard]] Q_INVOKABLE virtual cmsCIELab toCielabD50(const QRgba64 rgbColor) const;
    [[nodiscard]] Q_INVOKABLE virtual PerceptualColor::GenericColor toCielchD50(const QRgba64 rgbColor) const;
    [[nodiscard]] Q_INVOKABLE static cmsCIELab fromLchToCmsCIELab(const PerceptualColor::GenericColor &lch);
    [[nodiscard]] Q_INVOKABLE virtual QRgb fromCielchD50ToQRgbBound(const PerceptualColor::GenericColor &cielchD50) const;
    [[nodiscard]] Q_INVOKABLE virtual QRgb fromCielabD50ToQRgbOrTransparent(const cmsCIELab &lab) const;
    [[nodiscard]] Q_INVOKABLE virtual PerceptualColor::GenericColor fromCielchD50ToRgb1(const PerceptualColor::GenericColor &lch) const;

private:
    Q_DISABLE_COPY(RgbColorSpace)

    /** @internal
     *
     * @brief Private constructor.
     *
     * @param parent The widget’s parent widget. This parameter will be
     * passed to the base class’s constructor. */
    explicit RgbColorSpace(QObject *parent = nullptr);

    /** @internal
     *
     * @brief Declare the private implementation as friend class.
     *
     * This allows the private class to access the protected members and
     * functions of instances of <em>this</em> class. */
    friend class RgbColorSpacePrivate;
    /** @brief Pointer to implementation (pimpl) */
    ConstPropagatingUniquePointer<RgbColorSpacePrivate> d_pointer;

    /** @internal @brief Only for unit tests. */
    friend class TestRgbColorSpace;
};

} // namespace PerceptualColor

Q_DECLARE_METATYPE(PerceptualColor::RgbColorSpace::ProfileRole)
Q_DECLARE_METATYPE(PerceptualColor::RgbColorSpace::RenderingIntentDirections)

#endif // PERCEPTUALCOLOR_RGBCOLORSPACE_H
