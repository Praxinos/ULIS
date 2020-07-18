// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Draw.cpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the raster draw entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include "Raster/Draw.h"
#include "Data/Block.h"
#include "Conv/ConvBuffer.h"

ULIS3_NAMESPACE_BEGIN
/*
void DotNoAA( FBlock* iDst, const FColor& iColor, const FVec2I iPos ) {
    uint8* ptr = iDst->PixelBits( iPos.x, iPos.y );
    fpConversionInvocation fptr = QueryDispatchedConversionInvocation( iColor.Format(), iDst->Format() );
    fptr( &iColor.FormatInfo(), iColor.Bits(), &iDst->FormatInfo(), ptr, 1 );
}
*/

ULIS3_NAMESPACE_END

