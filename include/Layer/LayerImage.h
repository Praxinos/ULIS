// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerImage.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FLayerImage class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Layer.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FLayerImage
/// @brief      The FLayerImage class provides a class to store an image in a
///             layer stack for painting applications.
class ULIS_API FLayerImage
    : public virtual ILayer
{
public:
    virtual ~FLayerImage() override;
    FLayerImage(
          const FString& iName
        , uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat
        , eBlendMode iBlendMode
        , eAlphaMode iAlphaMode
        , ufloat iOpacity
        , FLayerRoot* iParent
    );

    FLayerImage(
          FBlock* iBlock
        , const FString& iName
        , uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat
        , eBlendMode iBlendMode
        , eAlphaMode iAlphaMode
        , ufloat iOpacity
        , FLayerRoot* iParent
    );

    FLayerImage( const FLayerImage& ) = delete;
    FLayerImage& operator=( const FLayerImage& ) = delete;

public:
    virtual eLayerType Type() const override;
    FBlock& Block();
    const FBlock& Block() const;
    bool IsAlphaLocked() const;
    void SetAlphaLocked( bool iValue );
    eBlendMode BlendMode() const;
    eAlphaMode AlphaMode() const;
    ufloat Opacity() const;
    void SetBlendMode( eBlendMode iValue );
    void SetAlphaMode( eAlphaMode iValue );
    void SetOpacity( ufloat iValue );
    bool IsCacheValid() const;
    void InvalidateCache();
    void ValidateCache();
private:
    bool mAlphaLock;
    FBlock* mBlock;
    eBlendMode mBlendMode;
    eAlphaMode mAlphaMode;
    ufloat mOpacity;
    bool bIsCacheValid;
};

ULIS_NAMESPACE_END

