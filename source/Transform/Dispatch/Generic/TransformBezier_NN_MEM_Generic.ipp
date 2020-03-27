// Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         TransformBezier_NN_MEM_Generic.ipp
* @author       Clement Berthaud
* @brief        This file provides the declaration for the generic transform entry point functions.
* @copyright    Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Maths/Geometry.h"
#include "Transform/Dispatch/TransformInfo.h"
#include "Maths/Geometry.h"
#include "Thread/ThreadPool.h"

ULIS2_NAMESPACE_BEGIN
template< typename T > void
InvokeTransformBezierMTProcessScanline_NN_MEM_Generic( tByte* iDst, int32 iLine, std::shared_ptr< const _FTransformInfoPrivate > iInfo, std::shared_ptr< const FBlock > iField, std::shared_ptr< const FBlock > iMask ) {
    const _FTransformInfoPrivate&   info    = *iInfo;
    const FFormatInfo&              fmt     = info.destination->FormatInfo();
    tByte*                          dst     = iDst;
    const float*                    field   = reinterpret_cast< const float* >( iField->ScanlinePtr( iLine ) );
    const uint8*                    mask    = reinterpret_cast< const uint8* >( iMask->ScanlinePtr( iLine ) );
    const int rangex = info.src_roi.w - 1;
    const int rangey = info.src_roi.h - 1;
    for( int x = 0; x < info.dst_roi.w; ++x ) {
        if( *mask & 0xFF ) {
            int src_x = static_cast< int >( field[0] * rangex );
            int src_y = static_cast< int >( field[1] * rangey );
            memcpy( dst, info.source->PixelPtr( src_x, src_y ), fmt.BPP );
        }

        dst += fmt.BPP;
        field += 2;
        ++mask;
    }
}

template< typename T > void
TransformBezierMT_NN_MEM_Generic( std::shared_ptr< const _FTransformInfoPrivate > iInfo, std::shared_ptr< const FBlock > iField, std::shared_ptr< const FBlock > iMask ) {
    const _FTransformInfoPrivate&   info        = *iInfo;
    tByte*                          dst         = info.destination->DataPtr();
    const tSize                     dst_bps     = info.destination->BytesPerScanLine();
    const tSize                     dst_decal_y = info.dst_roi.y;
    const tSize                     dst_decal_x = info.dst_roi.x * info.destination->BytesPerPixel();
    ULIS2_MACRO_INLINE_PARALLEL_FOR( info.perfIntent, info.pool, info.blocking
                                   , info.dst_roi.h
                                   , InvokeTransformBezierMTProcessScanline_NN_MEM_Generic< T >
                                   , dst + ( ( dst_decal_y + pLINE ) * dst_bps ) + dst_decal_x, pLINE, iInfo, iField, iMask );
}

ULIS2_NAMESPACE_END
