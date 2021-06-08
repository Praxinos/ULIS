// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerFolder.cpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the FLayerFolder class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include "Layer/Layer/LayerFolder.h"
#include "Layer/Common/LayerUtils.h"
#include "Context/Context.h"

ULIS_NAMESPACE_BEGIN
FLayerFolder::~FLayerFolder()
{
}

FLayerFolder::FLayerFolder(
      const FString& iName
    , bool iLocked
    , bool iVisible
    , const FColor& iColor
    , uint16 iWidth
    , uint16 iHeight
    , eFormat iFormat
    , eBlendMode iBlendMode
    , eAlphaMode iAlphaMode
    , ufloat iOpacity
    , bool iAlphaLocked
    , bool iCollapsed
    , tParent* iParent
)
    : tSuperClass(
          iName
        , iLocked
        , iVisible
        , iColor
        , iWidth
        , iHeight
        , iFormat
        , iBlendMode
        , iAlphaMode
        , iOpacity
        , iAlphaLocked
        , iParent
    )
    , mCollapsed( iCollapsed )
{
}

FLayerFolder::FLayerFolder(
      FBlock* iBlock
    , const FString& iName
    , bool iLocked
    , bool iVisible
    , const FColor& iColor
    , eBlendMode iBlendMode
    , eAlphaMode iAlphaMode
    , ufloat iOpacity
    , bool iAlphaLocked
    , bool iCollapsed
    , tParent* iParent
)
    : tSuperClass(
          iBlock
        , iName
        , iLocked
        , iVisible
        , iColor
        , iBlendMode
        , iAlphaMode
        , iOpacity
        , iAlphaLocked
        , iParent
    )
    , mCollapsed( iCollapsed )
{
}

bool
FLayerFolder::IsCollapsed() const {
    return  mCollapsed;
}

void
FLayerFolder::SetCollapsed( bool iCollapsed ) {
    mCollapsed = iCollapsed;
}

ILayer&
FLayerFolder::operator[]( const FString& iName ) {
    return  FindLayerByFuzzyNameInContainer( iName, *this );
}

const ILayer&
FLayerFolder::operator[]( const FString& iName ) const {
    return  FindLayerByFuzzyNameInContainer( iName, *this );
}

FEvent
FLayerFolder::RenderCache( FContext& iCtx ) {
    if( IsImageCacheValid() )
        return  FEvent::NoOP();

    FEvent ev;
    iCtx.Clear( Block(), FRectI::Auto, FSchedulePolicy::CacheEfficient, 0, nullptr, &ev );
    for( int i = static_cast< int >( Children().Size() ) - 1; i >= 0; --i )
        ev = Children()[i]->Self().RenderImage( iCtx, Block(), FRectI::Auto, FVec2I( 0 ), FSchedulePolicy::MultiScanlines, 1, &ev );

    ValidateImageCache();
    return  ev;
}

FEvent
FLayerFolder::RenderImage(
      FContext& iCtx
    , FBlock& ioBlock
    , const FRectI& iRect
    , const FVec2I& iPos
    , const FSchedulePolicy& iPolicy
    , uint32 iNumWait
    , const FEvent* iWaitList
)
{
    if( ( iRect & Block().Rect() ) == Block().Rect() )
        RenderCache( iCtx );

    FEvent ev;
    ulError err = iCtx.Blend(
          Block()
        , ioBlock
        , iRect
        , iPos
        , BlendMode()
        , AlphaMode()
        , Opacity()
        , iPolicy
        , iNumWait
        , iWaitList
        , &ev
    );
    ULIS_ASSERT( !err, "Error during layer image blend" );
    return  ev;
}

ULIS_NAMESPACE_END
