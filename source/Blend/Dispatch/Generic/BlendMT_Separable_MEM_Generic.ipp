// Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         BlendMT_Separable_MEM_Generic.ipp
* @author       Clement Berthaud
* @brief        This file provides the declaration for the generic Blend entry point functions.
* @copyright    Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Blend/Modes.h"
#include "Blend/Dispatch/BlendInfo.h"
#include "Blend/Func/AlphaFuncF.ipp"
#include "Blend/Func/CompositingHelpers.ipp"
#include "Blend/Func/SeparableBlendFuncF.ipp"
#include "Maths/Geometry.h"
#include "Thread/ThreadPool.h"

ULIS2_NAMESPACE_BEGIN
template< typename T >
void
InvokeBlendMTProcessScanline_Separable_MEM_Generic_Subpixel( const tByte* iSrc, tByte* iBdp, int32 iLine, const tSize iSrcBps, std::shared_ptr< const _FBlendInfoPrivate > iInfo ) {
    const _FBlendInfoPrivate&   info    = *iInfo;
    const FFormatInfo&          fmt     = info.source->FormatInfo();
    const tByte*                src     = iSrc;
    tByte*                      bdp     = iBdp;
    const bool notLastLine  = iLine < info.backdropCoverage.y;
    const bool notFirstLine = iLine > 0;
    const bool onLeftBorder = info.backdropWorkingRect.x == 0;
    const bool hasLeftData  = info.sourceRect.x + info.shift.x > 0;
    const bool hasTopData   = info.sourceRect.y + info.shift.y > 0;

    float m11, m01, m10, m00, vv0, vv1, res;
    m11 = ( notLastLine && onLeftBorder && hasLeftData )    ? TYPE2FLOAT( src - fmt.BPP,              fmt.AID ) : 0.f;
    m10 = ( hasLeftData && ( notFirstLine || hasTopData ) ) ? TYPE2FLOAT( src - iSrcBps - fmt.BPP,    fmt.AID ) : 0.f;
    vv1 = m10 * info.subpixelComponent.y + m11 * info.buspixelComponent.y;

    for( int x = 0; x < info.backdropWorkingRect.w; ++x ) {
        const bool notLastCol = x < info.backdropCoverage.x;
        m00 = m10;
        m01 = m11;
        vv0 = vv1;
        SampleSubpixelAlpha( res );
        const float alpha_bdp   = fmt.HEA ? TYPE2FLOAT( bdp, fmt.AID ) : 1.f;
        const float alpha_src   = res * info.opacityValue;
        const float alpha_comp  = AlphaNormalF( alpha_src, alpha_bdp );
        const float var         = alpha_comp == 0.f ? 0.f : alpha_src / alpha_comp;
        float alpha_result;
        ULIS2_ASSIGN_ALPHAF( info.alphaMode, alpha_result, alpha_src, alpha_bdp );

        for( uint8 j = 0; j < fmt.NCC; ++j ) {
            uint8 r = fmt.IDT[j];
            float s11, s01, s10, s00, v1, v2, srcvf;
            SampleSubpixelChannel( srcvf, r );
            float bdpvf = TYPE2FLOAT( bdp, r );
            #define TMP_ASSIGN( _BM, _E1, _E2, _E3 ) FLOAT2TYPE( bdp, r, SeparableCompOpF< _BM >( srcvf, bdpvf, alpha_bdp, var ) );
            ULIS2_SWITCH_FOR_ALL_COMP_OP_DO( info.blendingMode, ULIS2_FOR_ALL_SEPARABLE_BM_DO, TMP_ASSIGN, 0, 0, 0 )
            #undef TMP_ASSIGN
        }

        if( fmt.HEA ) FLOAT2TYPE( bdp, fmt.AID, alpha_result );
        src += fmt.BPP;
        bdp += fmt.BPP;
    }
}

template< typename T >
void
BlendMT_Separable_MEM_Generic_Subpixel( std::shared_ptr< const _FBlendInfoPrivate > iInfo ) {
    const _FBlendInfoPrivate&   info        = *iInfo;
    const tByte*                src         = info.source->DataPtr();
    tByte*                      bdp         = info.backdrop->DataPtr();
    const tSize                 src_bps     = info.source->BytesPerScanLine();
    const tSize                 bdp_bps     = info.backdrop->BytesPerScanLine();
    const tSize                 src_decal_y = info.shift.y + info.sourceRect.y;
    const tSize                 src_decal_x = ( info.shift.x + info.sourceRect.x )  * info.source->BytesPerPixel();
    const tSize                 bdp_decal_x = ( info.backdropWorkingRect.x )        * info.source->BytesPerPixel();
    ULIS2_MACRO_INLINE_PARALLEL_FOR( info.perfIntent, info.pool, info.blocking
                                   , info.backdropWorkingRect.h
                                   , InvokeBlendMTProcessScanline_Separable_MEM_Generic_Subpixel< T >
                                   , src + ( ( src_decal_y + pLINE )                * src_bps ) + src_decal_x
                                   , bdp + ( ( info.backdropWorkingRect.y + pLINE ) * bdp_bps ) + bdp_decal_x
                                   , pLINE , src_bps, iInfo );
}

template< typename T >
void
InvokeBlendMTProcessScanline_Separable_MEM_Generic( const tByte* iSrc, tByte* iBdp, int32 iLine, std::shared_ptr< const _FBlendInfoPrivate > iInfo ) {
    const _FBlendInfoPrivate&   info    = *iInfo;
    const FFormatInfo&          fmt     = info.source->FormatInfo();
    const tByte*                src     = iSrc;
    tByte*                      bdp     = iBdp;

    for( int x = 0; x < info.backdropWorkingRect.w; ++x ) {
        const float alpha_src  = fmt.HEA ? TYPE2FLOAT( src, fmt.AID ) * info.opacityValue : info.opacityValue;
        const float alpha_bdp  = fmt.HEA ? TYPE2FLOAT( bdp, fmt.AID ) : 1.f;
        const float alpha_comp = AlphaNormalF( alpha_src, alpha_bdp );
        const float var        = alpha_comp == 0.f ? 0.f : alpha_src / alpha_comp;
        float alpha_result;
        ULIS2_ASSIGN_ALPHAF( info.alphaMode, alpha_result, alpha_src, alpha_bdp );
        for( uint8 j = 0; j < fmt.NCC; ++j ) {
            uint8 r = fmt.IDT[j];
            float srcvf = TYPE2FLOAT( src, r );
            float bdpvf = TYPE2FLOAT( bdp, r );
            #define TMP_ASSIGN( _BM, _E1, _E2, _E3 ) FLOAT2TYPE( bdp, r, SeparableCompOpF< _BM >( srcvf, bdpvf, alpha_bdp, var ) );
            ULIS2_SWITCH_FOR_ALL_COMP_OP_DO( info.blendingMode, ULIS2_FOR_ALL_SEPARABLE_BM_DO, TMP_ASSIGN, 0, 0, 0 )
            #undef TMP_ASSIGN
        }
        if( fmt.HEA ) FLOAT2TYPE( bdp, fmt.AID, alpha_result );
        src += fmt.BPP;
        bdp += fmt.BPP;
    }
}

template< typename T >
void
BlendMT_Separable_MEM_Generic( std::shared_ptr< const _FBlendInfoPrivate > iInfo ) {
    const _FBlendInfoPrivate&   info        = *iInfo;
    const tByte*                src         = info.source->DataPtr();
    tByte*                      bdp         = info.backdrop->DataPtr();
    const tSize                 src_bps     = info.source->BytesPerScanLine();
    const tSize                 bdp_bps     = info.backdrop->BytesPerScanLine();
    const tSize                 src_decal_y = info.shift.y + info.sourceRect.y;
    const tSize                 src_decal_x = ( info.shift.x + info.sourceRect.x )  * info.source->BytesPerPixel();
    const tSize                 bdp_decal_x = ( info.backdropWorkingRect.x )        * info.source->BytesPerPixel();
    ULIS2_MACRO_INLINE_PARALLEL_FOR( info.perfIntent, info.pool, info.blocking
                                   , info.backdropWorkingRect.h
                                   , InvokeBlendMTProcessScanline_Separable_MEM_Generic< T >
                                   , src + ( ( src_decal_y + pLINE )                * src_bps ) + src_decal_x
                                   , bdp + ( ( info.backdropWorkingRect.y + pLINE ) * bdp_bps ) + bdp_decal_x
                                   , pLINE , iInfo );
}

ULIS2_NAMESPACE_END

