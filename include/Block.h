// Copyright � 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         Block.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FBlock class.
* @copyright    Copyright � 2018-2019 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core.h"
#include "Pixel.h"
#include "Color.h"

ULIS2_NAMESPACE_BEGIN

ULIS2_API void OnCleanup_FreeMemory( tByte* iData, void* iInfo );
ULIS2_API void OnCleanup_DoNothing(  tByte* iData, void* iInfo );

/////////////////////////////////////////////////////
/// @class      FBlock
/// @brief      The FBlock class provides a mean of storing and manipulating digital images in various formats.
/// @details    The block format, type, layout, and other informations can be retrieved with dedicated functions.
class ULIS2_API FBlock
{
public:
    // Construction / Destruction
    ~FBlock();

    FBlock( tSize iWidth
          , tSize iHeight
          , tFormat iFormat
          , const FOnInvalid& iOnInvalid = FOnInvalid()
          , const FOnCleanup& iOnCleanup = FOnCleanup( &OnCleanup_FreeMemory ) );

    FBlock( tSize iWidth
          , tSize iHeight
          , tFormat iFormat
          , FColorProfile* iProfile = nullptr
          , const FOnInvalid& iOnInvalid = FOnInvalid()
          , const FOnCleanup& iOnCleanup = FOnCleanup( &OnCleanup_FreeMemory ) );

    FBlock( tByte* iData
          , tSize iWidth
          , tSize iHeight
          , tFormat iFormat
          , const FOnInvalid& iOnInvalid = FOnInvalid()
          , const FOnCleanup& iOnCleanup = FOnCleanup( &OnCleanup_DoNothing ) );

    FBlock( tByte* iData
          , tSize iWidth
          , tSize iHeight
          , tFormat iFormat
          , FColorProfile* iProfile = nullptr
          , const FOnInvalid& iOnInvalid = FOnInvalid()
          , const FOnCleanup& iOnCleanup = FOnCleanup( &OnCleanup_DoNothing ) );

public:
    // Public API
    tByte*                  DataPtr();
    tByte*                  PixelPtr( tIndex iX, tIndex iY );
    tByte*                  ScanlinePtr( tIndex iRow );
    void                    AssignProfile( FColorProfile* iProfile );
    const tByte*            DataPtr()                                   const;
    const tByte*            PixelPtr( tIndex iX, tIndex iY )            const;
    const tByte*            ScanlinePtr( tIndex iRow )                  const;
    tSize                   Width()                                     const;
    tSize                   Height()                                    const;
    tSize                   BytesPerSample()                            const;
    tSize                   BytesPerPixel()                             const;
    tSize                   BytesPerScanLine()                          const;
    tSize                   BytesTotal()                                const;
    tFormat                 Format()                                    const;
    eModelSig               Model()                                     const;
    eType                   Type()                                      const;
    bool                    HasAlpha()                                  const;
    bool                    Swapped()                                   const;
    bool                    Reversed()                                  const;
    uint8                   SamplesPerPixel()                           const;
    uint8                   NumColorChannels()                          const;
    FColorProfile*          Profile()                                   const;
    void                    Invalidate()                                const;
    void                    Invalidate( const FRect& iRect )            const;
    FPixelValue             PixelValue( tIndex iX, tIndex iY )          const;
    FPixelProxy             PixelProxy( tIndex iX, tIndex iY );
    FColor                  PixelColor( tIndex iX, tIndex iY )          const;

private:
    // Private Data Members
    tByte*          mData;
    tSize           mWidth;
    tSize           mHeight;
    tFormat         mFormat;
    FOnInvalid      mOnInvalid;
    FOnCleanup      mOnCleanup;
    FColorProfile*  mProfile;
};

ULIS2_NAMESPACE_END

