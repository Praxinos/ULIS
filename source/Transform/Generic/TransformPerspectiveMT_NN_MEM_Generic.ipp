// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         TransformPerspectiveMT_NN_MEM_Generic.ipp
* @author       Clement Berthaud
* @brief        This file provides the implementation for a Transform specialization as described in the title.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Data/Block.h"
#include "Maths/Geometry.h"
#include "Maths/Transform2D_Private.h"
#include "Transform/TransformArgs.h"
#include "Transform/TransformHelpers.h"
#include "Thread/ThreadPool.h"

ULIS3_NAMESPACE_BEGIN
template< typename T > void
InvokeTransformPerspectiveMTProcessScanline_NN_MEM_Generic( tByte* iDst, int32 iLine, std::shared_ptr< const FTransformArgs > iInfo ) {
    const FTransformArgs&   info    = *iInfo;
    const FFormatInfo&      fmt     = info.destination->FormatInfo();
    tByte*                  dst     = iDst;

    FVec2F pointInDst( static_cast< float >( info.dst_roi.x ), static_cast< float >( info.dst_roi.y + iLine ) );

    const int minx = info.src_roi.x;
    const int miny = info.src_roi.y;
    const int maxx = minx + info.src_roi.w;
    const int maxy = miny + info.src_roi.h;
    for( int x = 0; x < info.dst_roi.w; ++x ) {
        FVec2F pointInSrc = HomographyTransform( pointInDst, info.inverseTransform );
        int src_x = static_cast< int >( pointInSrc.x );
        int src_y = static_cast< int >( pointInSrc.y );
        if( src_x >= minx && src_y >= miny && src_x < maxx && src_y < maxy )
            memcpy( dst, info.source->PixelPtr( src_x, src_y ), fmt.BPP );

        dst += fmt.BPP;
        pointInDst.x += 1;
    }
}

template< typename T > void
TransformPerspectiveMT_NN_MEM_Generic( std::shared_ptr< const FTransformArgs > iInfo ) {
    const FTransformArgs&   info        = *iInfo;
    tByte*                  dst         = info.destination->DataPtr();
    const tSize             dst_bps     = info.destination->BytesPerScanLine();
    const tSize             dst_decal_y = info.dst_roi.y;
    const tSize             dst_decal_x = info.dst_roi.x * info.destination->BytesPerPixel();
    ULIS3_MACRO_INLINE_PARALLEL_FOR( info.perfIntent, info.pool, info.blocking
                                   , info.dst_roi.h
                                   , InvokeTransformPerspectiveMTProcessScanline_NN_MEM_Generic< T >
                                   , dst + ( ( dst_decal_y + pLINE ) * dst_bps ) + dst_decal_x, pLINE, iInfo );
}

ULIS3_NAMESPACE_END
