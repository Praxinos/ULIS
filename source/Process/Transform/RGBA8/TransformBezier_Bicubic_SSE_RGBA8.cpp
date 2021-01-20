// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         TransformBezier_Bicubic_SSE_RGBA8.cpp
* @author       Clement Berthaud
* @brief        This file provides the implementation for a Transform specialization as described in the title.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Process/Transform/RGBA8/TransformBezier_Bicubic_SSE_RGBA8.h"
#include "Process/Transform/TransformHelpers.h"
#include "Image/Block.h"
#include <vectorclass.h>

ULIS_NAMESPACE_BEGIN
void
InvokeTransformBezierMT_Bicubic_SSE_RGBA8(
      const FTransformJobArgs* jargs
    , const FTransformCommandArgs* cargs
)
{
    /*
    const FFormatMetrics& fmt = cargs->dst.FormatMetrics();
    uint8* ULIS_RESTRICT dst = jargs->dst;
    Vec4i _idt( fmt.IDT[0], fmt.IDT[1], fmt.IDT[2], fmt.IDT[3] );
    const float*            field   = reinterpret_cast< const float* >( iField->ScanlineBits( jargs->line ) );
    const uint8*            mask    = reinterpret_cast< const uint8* >( iMask->ScanlineBits( jargs->line ) );
    const int rangex = cargs->srcRect.w - 1;
    const int rangey = cargs->srcRect.h - 1;

    Vec4f p00, p10, p20, p30;
    Vec4f p01, p11, p21, p31;
    Vec4f p02, p12, p22, p32;
    Vec4f p03, p13, p23, p33;
    Vec4f hh0, hh1, hh2, hh3;
    Vec4f res, alp;

    const int minx = cargs->srcRect.x;
    const int miny = cargs->srcRect.y;
    const int maxx = minx + cargs->srcRect.w;
    const int maxy = miny + cargs->srcRect.h;
    for( int x = 0; x < cargs->dstRect.w; ++x ) {
        if( *mask & 0xFF ) {
            float srcxf = field[0] * rangex;
            float srcyf = field[1] * rangey;
            const int   src_x = static_cast< int >( floor( srcxf ) );
            const int   src_y = static_cast< int >( floor( srcyf ) );
            const Vec4f tx      = srcxf - src_x;
            const Vec4f ty      = srcyf - src_y;

            #define LOAD( X )   _mm_cvtepi32_ps( _mm_cvtepu8_epi32( _mm_loadu_si128( reinterpret_cast< const __m128i* >( X ) ) ) )
            #define GETPIXEL( _C, _X, _Y )                                                                                                                      \
                if( _X >= minx && _Y >= miny && _X < maxx && _Y < maxy ) {                                                                                      \
                    const uint8* pptr = cargs->src.PixelBits( _X, _Y );                                                                                        \
                    Vec4f _ch = LOAD( pptr );                                                                                                                   \
                    Vec4f _al = _mm_set_ps1( pptr[ fmt.AID ] );                                                                                                 \
                    _C = lookup8( _idt, ( _ch * _al ) / 255.f, _al );                                                                                           \
                } else {                                                                                                                                        \
                    _C = _mm_setzero_ps();                                                                                                                      \
                }
            GETPIXEL( p00, src_x - 1, src_y - 1 );  GETPIXEL( p01, src_x - 1, src_y + 0 );  GETPIXEL( p02, src_x - 1, src_y + 1 );  GETPIXEL( p03, src_x - 1, src_y + 2 );
            GETPIXEL( p10, src_x + 0, src_y - 1 );  GETPIXEL( p11, src_x + 0, src_y + 0 );  GETPIXEL( p12, src_x + 0, src_y + 1 );  GETPIXEL( p13, src_x + 0, src_y + 2 );
            GETPIXEL( p20, src_x + 1, src_y - 1 );  GETPIXEL( p21, src_x + 1, src_y + 0 );  GETPIXEL( p22, src_x + 1, src_y + 1 );  GETPIXEL( p23, src_x + 1, src_y + 2 );
            GETPIXEL( p30, src_x + 2, src_y - 1 );  GETPIXEL( p31, src_x + 2, src_y + 0 );  GETPIXEL( p32, src_x + 2, src_y + 1 );  GETPIXEL( p33, src_x + 2, src_y + 2 );
            #undef GETPIXEL
            #undef LOAD

            hh0 = InterpCubic( p00, p10, p20, p30, tx );
            hh1 = InterpCubic( p01, p11, p21, p31, tx );
            hh2 = InterpCubic( p02, p12, p22, p32, tx );
            hh3 = InterpCubic( p03, p13, p23, p33, tx );
            res = InterpCubic( hh0, hh1, hh2, hh3, ty );
            alp = lookup4( fmt.AID, res );
            alp.insert( fmt.AID, 255.f );
            res = ( res * 255.f ) / alp;

            auto _pack = _mm_cvtps_epi32( res );
            _pack = _mm_packus_epi32( _pack, _pack );
            _pack = _mm_packus_epi16( _pack, _pack );
            *( uint32* )dst = static_cast< uint32 >( _mm_cvtsi128_si32( _pack ) );
        }

        dst += fmt.BPP;
        field += 2;
        ++mask;
    }
    */
}

ULIS_DEFINE_TRANSFORM_COMMAND_SPECIALIZATION( TransformBezierMT_Bicubic_SSE_RGBA8 )

ULIS_NAMESPACE_END

