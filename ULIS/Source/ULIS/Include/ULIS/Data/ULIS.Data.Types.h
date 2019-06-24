/*************************************************************************
*
*   ULIS
*__________________
*
* ULIS.Data.Types.h
* Clement Berthaud - Layl
* Please refer to LICENSE.md
*/

#pragma once


#include "ULIS/Data/ULIS.Data.Block.h"


namespace ULIS {
/////////////////////////////////////////////////////
// Reg baking
/* we spec the name of the build reg */
#define ULIS_REG_BUILDER ulis_reg_builder
/* we create the reg with a first invalid entry (0) */
ULIS_CREATE_REG( ULIS_REG_BUILDER, 0 )

/////////////////////////////////////////////////////
// Types declspec
/* Actual types specializations */
ULIS_DECLSPEC(uint8,GREY,hasAlpha,GA,typeLimits)
ULIS_DECLSPEC(uint8,GREY,hasAlpha,AG,typeLimits)
ULIS_DECLSPEC(uint8,GREY,noAlpha,G,typeLimits)
ULIS_DECLSPEC(uint16,GREY,hasAlpha,GA,typeLimits)
ULIS_DECLSPEC(uint16,GREY,hasAlpha,AG,typeLimits)
ULIS_DECLSPEC(uint16,GREY,noAlpha,G,typeLimits)
ULIS_DECLSPEC(uint32,GREY,hasAlpha,GA,typeLimits)
ULIS_DECLSPEC(uint32,GREY,hasAlpha,AG,typeLimits)
ULIS_DECLSPEC(uint32,GREY,noAlpha,G,typeLimits)
ULIS_DECLSPEC(float,GREY,hasAlpha,GA,normalized)
ULIS_DECLSPEC(float,GREY,hasAlpha,AG,normalized)
ULIS_DECLSPEC(float,GREY,hasAlpha,GA,typeLimits)
ULIS_DECLSPEC(float,GREY,hasAlpha,AG,typeLimits)
ULIS_DECLSPEC(float,GREY,noAlpha,G,normalized)
ULIS_DECLSPEC(float,GREY,noAlpha,G,typeLimits)
ULIS_DECLSPEC(double,GREY,hasAlpha,GA,normalized)
ULIS_DECLSPEC(double,GREY,hasAlpha,AG,normalized)
ULIS_DECLSPEC(double,GREY,hasAlpha,GA,typeLimits)
ULIS_DECLSPEC(double,GREY,hasAlpha,AG,typeLimits)
ULIS_DECLSPEC(double,GREY,noAlpha,G,normalized)
ULIS_DECLSPEC(double,GREY,noAlpha,G,typeLimits)
ULIS_DECLSPEC(uint8,RGB,hasAlpha,ARGB,typeLimits)
ULIS_DECLSPEC(uint8,RGB,hasAlpha,ABGR,typeLimits)
ULIS_DECLSPEC(uint8,RGB,hasAlpha,RGBA,typeLimits)
ULIS_DECLSPEC(uint8,RGB,hasAlpha,BGRA,typeLimits)
ULIS_DECLSPEC(uint8,RGB,noAlpha,RGB,typeLimits)
ULIS_DECLSPEC(uint8,RGB,noAlpha,BGR,typeLimits)
ULIS_DECLSPEC(uint16,RGB,hasAlpha,ARGB,typeLimits)
ULIS_DECLSPEC(uint16,RGB,hasAlpha,ABGR,typeLimits)
ULIS_DECLSPEC(uint16,RGB,hasAlpha,RGBA,typeLimits)
ULIS_DECLSPEC(uint16,RGB,hasAlpha,BGRA,typeLimits)
ULIS_DECLSPEC(uint16,RGB,noAlpha,RGB,typeLimits)
ULIS_DECLSPEC(uint16,RGB,noAlpha,BGR,typeLimits)
ULIS_DECLSPEC(uint32,RGB,hasAlpha,ARGB,typeLimits)
ULIS_DECLSPEC(uint32,RGB,hasAlpha,ABGR,typeLimits)
ULIS_DECLSPEC(uint32,RGB,hasAlpha,RGBA,typeLimits)
ULIS_DECLSPEC(uint32,RGB,hasAlpha,BGRA,typeLimits)
ULIS_DECLSPEC(uint32,RGB,noAlpha,RGB,typeLimits)
ULIS_DECLSPEC(uint32,RGB,noAlpha,BGR,typeLimits)
ULIS_DECLSPEC(float,RGB,hasAlpha,ARGB,normalized)
ULIS_DECLSPEC(float,RGB,hasAlpha,ABGR,normalized)
ULIS_DECLSPEC(float,RGB,hasAlpha,RGBA,normalized)
ULIS_DECLSPEC(float,RGB,hasAlpha,BGRA,normalized)
ULIS_DECLSPEC(float,RGB,hasAlpha,ARGB,typeLimits)
ULIS_DECLSPEC(float,RGB,hasAlpha,ABGR,typeLimits)
ULIS_DECLSPEC(float,RGB,hasAlpha,RGBA,typeLimits)
ULIS_DECLSPEC(float,RGB,hasAlpha,BGRA,typeLimits)
ULIS_DECLSPEC(float,RGB,noAlpha,RGB,normalized)
ULIS_DECLSPEC(float,RGB,noAlpha,BGR,normalized)
ULIS_DECLSPEC(float,RGB,noAlpha,RGB,typeLimits)
ULIS_DECLSPEC(float,RGB,noAlpha,BGR,typeLimits)
ULIS_DECLSPEC(double,RGB,hasAlpha,ARGB,normalized)
ULIS_DECLSPEC(double,RGB,hasAlpha,ABGR,normalized)
ULIS_DECLSPEC(double,RGB,hasAlpha,RGBA,normalized)
ULIS_DECLSPEC(double,RGB,hasAlpha,BGRA,normalized)
ULIS_DECLSPEC(double,RGB,hasAlpha,ARGB,typeLimits)
ULIS_DECLSPEC(double,RGB,hasAlpha,ABGR,typeLimits)
ULIS_DECLSPEC(double,RGB,hasAlpha,RGBA,typeLimits)
ULIS_DECLSPEC(double,RGB,hasAlpha,BGRA,typeLimits)
ULIS_DECLSPEC(double,RGB,noAlpha,RGB,normalized)
ULIS_DECLSPEC(double,RGB,noAlpha,BGR,normalized)
ULIS_DECLSPEC(double,RGB,noAlpha,RGB,typeLimits)
ULIS_DECLSPEC(double,RGB,noAlpha,BGR,typeLimits)
ULIS_DECLSPEC(uint8,HSL,hasAlpha,HSLA,typeLimits)
ULIS_DECLSPEC(uint8,HSL,hasAlpha,AHSL,typeLimits)
ULIS_DECLSPEC(uint8,HSL,noAlpha,HSL,typeLimits)
ULIS_DECLSPEC(uint16,HSL,hasAlpha,HSLA,typeLimits)
ULIS_DECLSPEC(uint16,HSL,hasAlpha,AHSL,typeLimits)
ULIS_DECLSPEC(uint16,HSL,noAlpha,HSL,typeLimits)
ULIS_DECLSPEC(uint32,HSL,hasAlpha,HSLA,typeLimits)
ULIS_DECLSPEC(uint32,HSL,hasAlpha,AHSL,typeLimits)
ULIS_DECLSPEC(uint32,HSL,noAlpha,HSL,typeLimits)
ULIS_DECLSPEC(float,HSL,hasAlpha,HSLA,normalized)
ULIS_DECLSPEC(float,HSL,hasAlpha,AHSL,normalized)
ULIS_DECLSPEC(float,HSL,hasAlpha,HSLA,typeLimits)
ULIS_DECLSPEC(float,HSL,hasAlpha,AHSL,typeLimits)
ULIS_DECLSPEC(float,HSL,noAlpha,HSL,normalized)
ULIS_DECLSPEC(float,HSL,noAlpha,HSL,typeLimits)
ULIS_DECLSPEC(double,HSL,hasAlpha,HSLA,normalized)
ULIS_DECLSPEC(double,HSL,hasAlpha,AHSL,normalized)
ULIS_DECLSPEC(double,HSL,hasAlpha,HSLA,typeLimits)
ULIS_DECLSPEC(double,HSL,hasAlpha,AHSL,typeLimits)
ULIS_DECLSPEC(double,HSL,noAlpha,HSL,normalized)
ULIS_DECLSPEC(double,HSL,noAlpha,HSL,typeLimits)
ULIS_DECLSPEC(uint8,HSV,hasAlpha,HSVA,typeLimits)
ULIS_DECLSPEC(uint8,HSV,hasAlpha,AHSV,typeLimits)
ULIS_DECLSPEC(uint8,HSV,noAlpha,HSV,typeLimits)
ULIS_DECLSPEC(uint16,HSV,hasAlpha,HSVA,typeLimits)
ULIS_DECLSPEC(uint16,HSV,hasAlpha,AHSV,typeLimits)
ULIS_DECLSPEC(uint16,HSV,noAlpha,HSV,typeLimits)
ULIS_DECLSPEC(uint32,HSV,hasAlpha,HSVA,typeLimits)
ULIS_DECLSPEC(uint32,HSV,hasAlpha,AHSV,typeLimits)
ULIS_DECLSPEC(uint32,HSV,noAlpha,HSV,typeLimits)
ULIS_DECLSPEC(float,HSV,hasAlpha,HSVA,normalized)
ULIS_DECLSPEC(float,HSV,hasAlpha,AHSV,normalized)
ULIS_DECLSPEC(float,HSV,hasAlpha,HSVA,typeLimits)
ULIS_DECLSPEC(float,HSV,hasAlpha,AHSV,typeLimits)
ULIS_DECLSPEC(float,HSV,noAlpha,HSV,normalized)
ULIS_DECLSPEC(float,HSV,noAlpha,HSV,typeLimits)
ULIS_DECLSPEC(double,HSV,hasAlpha,HSVA,normalized)
ULIS_DECLSPEC(double,HSV,hasAlpha,AHSV,normalized)
ULIS_DECLSPEC(double,HSV,hasAlpha,HSVA,typeLimits)
ULIS_DECLSPEC(double,HSV,hasAlpha,AHSV,typeLimits)
ULIS_DECLSPEC(double,HSV,noAlpha,HSV,normalized)
ULIS_DECLSPEC(double,HSV,noAlpha,HSV,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,CMYKA,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,KCMYA,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,YMCKA,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,KYMCA,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,ACMYK,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,AKCMY,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,AYMCK,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,hasAlpha,AKYMC,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,noAlpha,CMYK,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,noAlpha,KCMY,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,noAlpha,YMCK,typeLimits)
ULIS_DECLSPEC(uint8,CMYK,noAlpha,KYMC,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,CMYKA,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,KCMYA,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,YMCKA,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,KYMCA,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,ACMYK,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,AKCMY,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,AYMCK,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,hasAlpha,AKYMC,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,noAlpha,CMYK,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,noAlpha,KCMY,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,noAlpha,YMCK,typeLimits)
ULIS_DECLSPEC(uint16,CMYK,noAlpha,KYMC,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,CMYKA,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,KCMYA,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,YMCKA,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,KYMCA,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,ACMYK,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,AKCMY,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,AYMCK,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,hasAlpha,AKYMC,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,noAlpha,CMYK,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,noAlpha,KCMY,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,noAlpha,YMCK,typeLimits)
ULIS_DECLSPEC(uint32,CMYK,noAlpha,KYMC,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,CMYKA,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,KCMYA,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,YMCKA,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,KYMCA,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,ACMYK,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,AKCMY,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,AYMCK,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,AKYMC,normalized)
ULIS_DECLSPEC(float,CMYK,hasAlpha,CMYKA,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,KCMYA,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,YMCKA,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,KYMCA,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,ACMYK,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,AKCMY,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,AYMCK,typeLimits)
ULIS_DECLSPEC(float,CMYK,hasAlpha,AKYMC,typeLimits)
ULIS_DECLSPEC(float,CMYK,noAlpha,CMYK,normalized)
ULIS_DECLSPEC(float,CMYK,noAlpha,KCMY,normalized)
ULIS_DECLSPEC(float,CMYK,noAlpha,YMCK,normalized)
ULIS_DECLSPEC(float,CMYK,noAlpha,KYMC,normalized)
ULIS_DECLSPEC(float,CMYK,noAlpha,CMYK,typeLimits)
ULIS_DECLSPEC(float,CMYK,noAlpha,KCMY,typeLimits)
ULIS_DECLSPEC(float,CMYK,noAlpha,YMCK,typeLimits)
ULIS_DECLSPEC(float,CMYK,noAlpha,KYMC,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,CMYKA,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,KCMYA,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,YMCKA,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,KYMCA,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,ACMYK,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,AKCMY,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,AYMCK,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,AKYMC,normalized)
ULIS_DECLSPEC(double,CMYK,hasAlpha,CMYKA,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,KCMYA,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,YMCKA,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,KYMCA,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,ACMYK,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,AKCMY,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,AYMCK,typeLimits)
ULIS_DECLSPEC(double,CMYK,hasAlpha,AKYMC,typeLimits)
ULIS_DECLSPEC(double,CMYK,noAlpha,CMYK,normalized)
ULIS_DECLSPEC(double,CMYK,noAlpha,KCMY,normalized)
ULIS_DECLSPEC(double,CMYK,noAlpha,YMCK,normalized)
ULIS_DECLSPEC(double,CMYK,noAlpha,KYMC,normalized)
ULIS_DECLSPEC(double,CMYK,noAlpha,CMYK,typeLimits)
ULIS_DECLSPEC(double,CMYK,noAlpha,KCMY,typeLimits)
ULIS_DECLSPEC(double,CMYK,noAlpha,YMCK,typeLimits)
ULIS_DECLSPEC(double,CMYK,noAlpha,KYMC,typeLimits)
ULIS_DECLSPEC(uint8,YUV,hasAlpha,YUVA,typeLimits)
ULIS_DECLSPEC(uint8,YUV,hasAlpha,AYUV,typeLimits)
ULIS_DECLSPEC(uint8,YUV,noAlpha,YUV,typeLimits)
ULIS_DECLSPEC(uint16,YUV,hasAlpha,YUVA,typeLimits)
ULIS_DECLSPEC(uint16,YUV,hasAlpha,AYUV,typeLimits)
ULIS_DECLSPEC(uint16,YUV,noAlpha,YUV,typeLimits)
ULIS_DECLSPEC(uint32,YUV,hasAlpha,YUVA,typeLimits)
ULIS_DECLSPEC(uint32,YUV,hasAlpha,AYUV,typeLimits)
ULIS_DECLSPEC(uint32,YUV,noAlpha,YUV,typeLimits)
ULIS_DECLSPEC(float,YUV,hasAlpha,YUVA,normalized)
ULIS_DECLSPEC(float,YUV,hasAlpha,AYUV,normalized)
ULIS_DECLSPEC(float,YUV,hasAlpha,YUVA,typeLimits)
ULIS_DECLSPEC(float,YUV,hasAlpha,AYUV,typeLimits)
ULIS_DECLSPEC(float,YUV,noAlpha,YUV,normalized)
ULIS_DECLSPEC(float,YUV,noAlpha,YUV,typeLimits)
ULIS_DECLSPEC(double,YUV,hasAlpha,YUVA,normalized)
ULIS_DECLSPEC(double,YUV,hasAlpha,AYUV,normalized)
ULIS_DECLSPEC(double,YUV,hasAlpha,YUVA,typeLimits)
ULIS_DECLSPEC(double,YUV,hasAlpha,AYUV,typeLimits)
ULIS_DECLSPEC(double,YUV,noAlpha,YUV,normalized)
ULIS_DECLSPEC(double,YUV,noAlpha,YUV,typeLimits)
ULIS_DECLSPEC(uint8,LAB,hasAlpha,LABA,typeLimits)
ULIS_DECLSPEC(uint8,LAB,hasAlpha,ALAB,typeLimits)
ULIS_DECLSPEC(uint8,LAB,noAlpha,LAB,typeLimits)
ULIS_DECLSPEC(uint16,LAB,hasAlpha,LABA,typeLimits)
ULIS_DECLSPEC(uint16,LAB,hasAlpha,ALAB,typeLimits)
ULIS_DECLSPEC(uint16,LAB,noAlpha,LAB,typeLimits)
ULIS_DECLSPEC(uint32,LAB,hasAlpha,LABA,typeLimits)
ULIS_DECLSPEC(uint32,LAB,hasAlpha,ALAB,typeLimits)
ULIS_DECLSPEC(uint32,LAB,noAlpha,LAB,typeLimits)
ULIS_DECLSPEC(float,LAB,hasAlpha,LABA,normalized)
ULIS_DECLSPEC(float,LAB,hasAlpha,ALAB,normalized)
ULIS_DECLSPEC(float,LAB,hasAlpha,LABA,typeLimits)
ULIS_DECLSPEC(float,LAB,hasAlpha,ALAB,typeLimits)
ULIS_DECLSPEC(float,LAB,noAlpha,LAB,normalized)
ULIS_DECLSPEC(float,LAB,noAlpha,LAB,typeLimits)
ULIS_DECLSPEC(double,LAB,hasAlpha,LABA,normalized)
ULIS_DECLSPEC(double,LAB,hasAlpha,ALAB,normalized)
ULIS_DECLSPEC(double,LAB,hasAlpha,LABA,typeLimits)
ULIS_DECLSPEC(double,LAB,hasAlpha,ALAB,typeLimits)
ULIS_DECLSPEC(double,LAB,noAlpha,LAB,normalized)
ULIS_DECLSPEC(double,LAB,noAlpha,LAB,typeLimits)
ULIS_DECLSPEC(uint8,XYZ,hasAlpha,XYZA,typeLimits)
ULIS_DECLSPEC(uint8,XYZ,hasAlpha,AXYZ,typeLimits)
ULIS_DECLSPEC(uint8,XYZ,noAlpha,XYZ,typeLimits)
ULIS_DECLSPEC(uint16,XYZ,hasAlpha,XYZA,typeLimits)
ULIS_DECLSPEC(uint16,XYZ,hasAlpha,AXYZ,typeLimits)
ULIS_DECLSPEC(uint16,XYZ,noAlpha,XYZ,typeLimits)
ULIS_DECLSPEC(uint32,XYZ,hasAlpha,XYZA,typeLimits)
ULIS_DECLSPEC(uint32,XYZ,hasAlpha,AXYZ,typeLimits)
ULIS_DECLSPEC(uint32,XYZ,noAlpha,XYZ,typeLimits)
ULIS_DECLSPEC(float,XYZ,hasAlpha,XYZA,normalized)
ULIS_DECLSPEC(float,XYZ,hasAlpha,AXYZ,normalized)
ULIS_DECLSPEC(float,XYZ,hasAlpha,XYZA,typeLimits)
ULIS_DECLSPEC(float,XYZ,hasAlpha,AXYZ,typeLimits)
ULIS_DECLSPEC(float,XYZ,noAlpha,XYZ,normalized)
ULIS_DECLSPEC(float,XYZ,noAlpha,XYZ,typeLimits)
ULIS_DECLSPEC(double,XYZ,hasAlpha,XYZA,normalized)
ULIS_DECLSPEC(double,XYZ,hasAlpha,AXYZ,normalized)
ULIS_DECLSPEC(double,XYZ,hasAlpha,XYZA,typeLimits)
ULIS_DECLSPEC(double,XYZ,hasAlpha,AXYZ,typeLimits)
ULIS_DECLSPEC(double,XYZ,noAlpha,XYZ,normalized)
ULIS_DECLSPEC(double,XYZ,noAlpha,XYZ,typeLimits)

/////////////////////////////////////////////////////
// Types assign
/* we assign the result reg */
ULIS_REG_TYPE ulis_types_reg = ULIS_ASSIGN_REG( ULIS_REG_BUILDER );
/* we spec the name of the result reg for easy usage */
#define ULIS_REG ::ULIS::ulis_types_reg

/////////////////////////////////////////////////////
// Reg Size
/* temp assign reg size ( workaround ) */
constexpr auto temp_reg_size = ulis_types_reg.Size();
/* Reg size macro for usage during preprocessor ( workaround ) */
#define ULIS_REG_SIZE 253

/////////////////////////////////////////////////////
// Mainstream typedefs
/* easy typedefs for the most common block types defined earlier */
typedef ::ULIS::FBlockuint8RGBhasAlphaRGBAtypeLimits FBlockRGBA8;
typedef ::ULIS::FBlockuint8RGBhasAlphaBGRAtypeLimits FBlockBGRA8;
typedef ::ULIS::FBlockuint8RGBhasAlphaARGBtypeLimits FBlockARGB8;
typedef ::ULIS::FBlockuint8RGBhasAlphaABGRtypeLimits FBlockABGR8;

} // namespace ULIS
