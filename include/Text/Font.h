// Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         Font.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FFont class.
* @copyright    Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Base/Core.h"

#include <ft2build.h>
#include FT_FREETYPE_H

ULIS2_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FFont
/// @brief      The FFont class provides a thin wrapper around the freetype face type.
class ULIS2_API FFont
{
public:
    // Construction / Destruction
    ~FFont();
    FFont( const FFontRegistry& iFontRegistry, const std::string& iFamily = "Arial", const std::string& iStyle = "Regular" );
    FFont( FFont& iOther );

public:
    // Public API
    FT_Face Handle() const;
    const FFontEngine& FontEngine() const;
    const FFontRegistry& FontRegistry() const;
private:
    // Private Data Members
    FT_Face mHandle;
    const FFontEngine& mFontEngine;
    const FFontRegistry& mFontRegistry;
};
ULIS2_NAMESPACE_END

