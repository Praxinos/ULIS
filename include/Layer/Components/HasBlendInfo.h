// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         HasBlendInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the IHasBlendInfo class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Layer/Components/BlendInfo.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      IHasBlendInfo
/// @brief      Simple Blendable class.
struct ULIS_API IHasBlendInfo
{
protected:
    IHasBlendInfo( eBlendMode iBlendMode = Blend_Normal, eAlphaMode iAlphaMode = Alpha_Normal, ufloat iOpacity = 1.f );

public:
    eBlendMode BlendMode() const;
    eAlphaMode AlphaMode() const;
    ufloat Opacity() const;

    void SetBlendMode( eBlendMode iValue );
    void SetAlphaMode( eAlphaMode iValue );
    void SetOpacity( ufloat iValue );

private:
    FBlendInfo mInfo;
};

ULIS_NAMESPACE_END

