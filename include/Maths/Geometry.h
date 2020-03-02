// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         Geometry.h
* @author       Clement Berthaud
* @brief        This file provides geometry class declarations.
* @copyright    Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Base/Core.h"
#include <glm/vec2.hpp>

ULIS2_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// TVec2
template< typename T >
struct ULIS2_API TVec2
{
    T x;
    T y;

    TVec2() : x( 0 ), y( 0 ) {}
    TVec2( T iX, T iY ) : x( iX ), y( iY ) {};
};
typedef TVec2< unsigned int >   FVec2U;
typedef TVec2< int >            FVec2I;
typedef TVec2< float >          FVec2F;
typedef TVec2< double >         FVec2D;
typedef TVec2< uint8 >          FVec2U8;
typedef TVec2< uint16 >         FVec2U16;
typedef TVec2< uint32 >         FVec2U32;
/////////////////////////////////////////////////////
// FRect
struct ULIS2_API FRect
{
    int x;
    int y;
    int w;
    int h;

    FRect();
    FRect( int iX, int iY, int iW, int iH );
    static FRect FromXYWH( int iX, int iY, int iW, int iH );
    static FRect FromMinMax( int iXMin, int iYMin, int iXMax, int iYMax );
    FRect operator&( const FRect& iOther ) const;
    FRect operator|( const FRect& iOther ) const;
    bool operator==( const FRect& iOther ) const;
    int Area() const;
    void Transform( const FTransform2D& iTransform );
    FRect Transformed( const FTransform2D& iTransform ) const;
    void FitInPositiveRange();
    void Shift( const FVec2I& iVec );
    FVec2I GetShift() const;
};

ULIS2_NAMESPACE_END

