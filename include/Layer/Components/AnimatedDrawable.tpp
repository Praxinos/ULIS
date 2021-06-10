// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedDrawable.tpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the TAnimatedDrawable class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Layer/Components/AnimatedDrawable.h"

ULIS_NAMESPACE_BEGIN
template< class BlockType >
TAnimatedDrawable< BlockType >::TAnimatedDrawable()
    : mCacheValid( false )
{}

template< class BlockType >
bool
TAnimatedDrawable< BlockType >::IsCacheValid() const {
    return  mCacheValid;
}

template< class BlockType >
FEvent
TAnimatedDrawable< BlockType >::RenderCache( FContext& iCtx ) {
    if( IsCacheValid() )
        return  FEvent::NoOP();

    ValidateCache();
    return  FEvent::NoOP();
}

template< class BlockType >
FEvent
TAnimatedDrawable< BlockType >::RenderImage(
      FContext& iCtx
    , BlockType& ioBlock
    , uint32 iFrame
    , const FRectI& iRect
    , const FVec2I& iPos
    , const FSchedulePolicy& iPolicy
    , uint32 iNumWait
    , const FEvent* iWaitList
)
{
    RenderCache( iCtx );
    return  FEvent::NoOP();
}

template< class BlockType >
void
TAnimatedDrawable< BlockType >::InvalidCache() {
    mCacheValid = false;
}

template< class BlockType >
void
TAnimatedDrawable< BlockType >::ValidateCache() {
    mCacheValid = true;
}


ULIS_NAMESPACE_END

