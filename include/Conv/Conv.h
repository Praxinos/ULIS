// Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         Conv.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Conv entry point functions.
* @copyright    Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Base/Core.h"

ULIS2_NAMESPACE_BEGIN
class FProfileRegistry;
/// @fn         void ProfileConv( const IPixel& iSrc, IPixel& iDst, const FProfileRegistry& iProfileRegistry, uint32 iIntent = INTENT_PERCEPTUAL )
/// @brief      Conv a pixel from source to destination with color space and intent.
/// @param      iSrc                A const reference to the source Pixel.
/// @param      iDst                A reference to the destination Pixel.
/// @param      iProfileRegistry    A reference to the profile registry used as backup if input pixels do not have profiles associated with them.
/// @param      iIntent             The profile conversion intent.
ULIS2_API void ProfileConv( const IPixel& iSrc, IPixel& iDst, const FProfileRegistry& iProfileRegistry, uint32 iIntent = 0 /* INTENT_PERCEPTUAL */ );

/// @fn         void Conv( const IPixel& iSrc, IPixel& iDst )
/// @brief      Conv a pixel from source to destination with basic model conversion.
/// @param      iSrc                A const reference to the source Pixel.
/// @param      iDst                A reference to the destination Pixel.
ULIS2_API void Conv( const IPixel& iSrc, IPixel& iDst );

// Pixel Template implementations
template< typename T1, typename T2 > void ConvT( const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToGrey(   const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToRGB(    const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToHSV(    const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToHSL(    const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToCMY(    const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToCMYK(   const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToYUV(    const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToLab(    const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToXYZ(    const IPixel& iSrc, IPixel& iDst );
template< typename T1, typename T2 > void ConvToYxy(    const IPixel& iSrc, IPixel& iDst );

ULIS2_NAMESPACE_END

