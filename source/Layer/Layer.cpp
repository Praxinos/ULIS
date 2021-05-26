// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Layer.cpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the ILayer class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include "Layer/Layer.h"
#include "Layer/LayerRoot.h"

ULIS_NAMESPACE_BEGIN
ILayer::~ILayer()
{
}

ILayer::ILayer()
    : mName( "Untitled" )
    , mLocked( false )
    , mVisible( true )
    , mParent( nullptr )
{
}

ILayer::ILayer( const FString& iName, ILayerRoot* iParent )
    : mName( iName )
    , mLocked( false )
    , mVisible( true )
    , mParent( iParent )
{}

eLayerType
ILayer::Type() const
{
    return  Layer_Invalid;
}

const FString&
ILayer::Name() const
{
    return  mName;
}

void
ILayer::SetName( const FString& iName )
{
    mName = iName;
}

bool
ILayer::Locked() const
{
    return  mLocked;
}

bool
ILayer::Visible() const
{
    return  mVisible;
}

void
ILayer::SetLocked( bool iValue )
{
    mLocked = iValue;
}

void
ILayer::SetVisible( bool iValue )
{
    mVisible = iValue;
}

ILayerRoot*
ILayer::Parent()
{
    return  mParent;
}

const ILayerRoot*
ILayer::Parent() const
{
    return  mParent;
}

void
ILayer::SetParent( ILayerRoot* iParent )
{
    mParent = iParent;
}

ULIS_NAMESPACE_END

