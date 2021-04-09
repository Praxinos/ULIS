// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         LayerText.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FLayerText class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Layer.h"
#include "Layer/LayerRoot.h"
#include "Layer/LayerImage.h"
#include "Font/Font.h"
#include "Image/Color.h"
#include "Math/Geometry/Matrix.h"

ULIS_NAMESPACE_BEGIN
#pragma warning(push)
#pragma warning(disable : 4251) // Shut warning C4251 dll export of stl classes
/////////////////////////////////////////////////////
/// @class      FLayerText
/// @brief      The FLayerText class provides a class to store text in a layer
///             of a layer stack for painting applications.
class ULIS_API FLayerText final
    : public FLayerImage
{
public:
    ~FLayerText() override final;
    FLayerText(
          const FString& iName
        , uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat
        , const FFont& iFont
        , eBlendMode iBlendMode = eBlendMode::Blend_Normal
        , eAlphaMode iAlphaMode = eAlphaMode::Alpha_Normal
        , ufloat iOpacity = 1.f
        , const std::wstring& iText = L""
        , uint32 iFontSize = 12
        , const FMat3F& iTransform = FMat3F()
        , const ISample& iColor = FColor::Black
        , bool iAA = true
        , FLayerRoot* iParent = nullptr
    );

    FLayerText(
          FBlock* iBlock
        , const FString& iName
        , uint16 iWidth
        , uint16 iHeight
        , eFormat iFormat
        , const FFont& iFont
        , eBlendMode iBlendMode = eBlendMode::Blend_Normal
        , eAlphaMode iAlphaMode = eAlphaMode::Alpha_Normal
        , ufloat iOpacity = 1.f
        , const std::wstring& iText = L""
        , uint32 iFontSize = 12
        , const FMat3F& iTransform = FMat3F()
        , const ISample& iColor = FColor::Black
        , bool iAA = true
        , FLayerRoot* iParent = nullptr
    );

    FLayerText( const FLayerText& ) = delete;
    FLayerText& operator=( const FLayerText& ) = delete;

public:
    eLayerType Type() const override;
    const std::wstring& Text() const;
    const FFont& Font() const;
    uint32 FontSize() const;
    const FMat3F& Transform() const;
    const FColor& Color() const;
    bool AA() const;

    void SetText( const std::wstring& iValue );
    void SetFont( const FFont& iValue );
    void SetFontSize( uint32 iValue );
    void SetTransform( const FMat3F& iValue );
    void SetColor( const ISample& iValue );
    void SetAA( bool iValue );

private:
    std::wstring mText;
    FFont mFont;
    uint32 mFontSize;
    FMat3F mTransform;
    FColor mColor;
    bool mAA;
};
#pragma warning(pop)
ULIS_NAMESPACE_END

