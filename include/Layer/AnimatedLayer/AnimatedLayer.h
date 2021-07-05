// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedLayer.h
* @author       Clement Berthaud, Eric Scholl
* @brief        This file provides the declaration for the IAnimatedLayer class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Memory/Tree.h"
#include "Core/Callback.h"
#include "Layer/Components/HasLock.h"
#include "Layer/Components/HasName.h"
#include "Layer/Components/HasPrettyColor.h"
#include "Layer/Components/HasUserData.h"
#include "Layer/Components/HasVisibility.h"
#include "Layer/Components/TypeIdentifiable.h"

ULIS_NAMESPACE_BEGIN
// Forward Declarations
class IAnimatedLayer;

// Exports
template class ULIS_API TLambdaCallback< void, const TNode< IAnimatedLayer >*, const TRoot< IAnimatedLayer >* >;
template class ULIS_API TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >* >;
template class ULIS_API TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >*, bool >;
template class ULIS_API TLambdaCallback< void, const TNode< IAnimatedLayer >* >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >* >, 0 >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TRoot< IAnimatedLayer >*, const TNode< IAnimatedLayer >*, bool >, 1 >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TNode< IAnimatedLayer >*, const TRoot< IAnimatedLayer >* >, 2 >;
template class ULIS_API TCallbackCapable< TLambdaCallback< void, const TNode< IAnimatedLayer >* >, 3 >;
template class ULIS_API TNode< IAnimatedLayer >;
template class ULIS_API TArray< TNode< IAnimatedLayer >* >;
template class ULIS_API TRoot< IAnimatedLayer >;
typedef FOn_bool_Changed FOnBoolChanged;
typedef TOnParentChanged< IAnimatedLayer > FOnAnimatedLayerParentChanged;
typedef TOnSelfChanged< IAnimatedLayer > FOnAnimatedLayerSelfChanged;
typedef TOnNodeAdded< IAnimatedLayer > FOnAnimatedLayerNodeAdded;
typedef TOnNodeRemoved< IAnimatedLayer > FOnAnimatedLayerNodeRemoved;

/////////////////////////////////////////////////////
/// @class      IAnimatedLayer
/// @brief      The IAnimatedLayer class provides a base abstract class to
///             store an animated layer in an animated  layer stack for
///             animation applications.
class ULIS_API IAnimatedLayer
    : public virtual TNode< IAnimatedLayer >
    , public ITypeIdentifiable
    , public IHasUserData
    , public IHasName
    , public IHasLock
    , public IHasVisibility
    , public IHasPrettyColor
{
public:
    // DTor
    virtual ~IAnimatedLayer() override = 0;

    // CTor
    IAnimatedLayer(
          const FString& iName = "Untitled"
        , bool iLocked = false
        , bool iVisible = true
        , const FColor& iPrettyColor = FColor::Transparent
        , const TRoot< IAnimatedLayer >* iParent = nullptr

        , const FOnNameChanged& iOnNameChanged = FOnNameChanged()
        , const FOnBoolChanged& iOnLockChanged = FOnBoolChanged()
        , const FOnBoolChanged& iOnVisibleChanged = FOnBoolChanged()
        , const FOnColorChanged& iOnColorChanged = FOnColorChanged()
        , const FOnUserDataAdded& iOnUserDataAdded = FOnUserDataAdded()
        , const FOnUserDataChanged& iOnUserDataChanged = FOnUserDataChanged()
        , const FOnUserDataRemoved& iOnUserDataRemoved = FOnUserDataRemoved()
        , const FOnAnimatedLayerParentChanged& iOnParentChanged = FOnAnimatedLayerParentChanged()
        , const FOnAnimatedLayerSelfChanged& iOnSelfChanged = FOnAnimatedLayerSelfChanged()
    );
};

ULIS_NAMESPACE_END

