// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Disk.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the disk IO entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN

enum eImageFormat {
      IM_PNG
    , IM_BMP
    , IM_TGA
    , IM_JPG
    , IM_HDR
};

static const char* kwImageFormat[] = {
      "png"
    , "bmp"
    , "tga"
    , "jpg"
    , "hdr"
};

ULIS_API FRasterImage2D* XLoadFromFile( FThreadPool*           iThreadPool
                               , bool                   iBlocking
                               , uint32                 iPerfIntent
                               , const FHostDeviceInfo& iHostDeviceInfo
                               , bool                   iCallCB
                               , const std::string&     iPath
                               , eFormat                iDesiredFormat );

ULIS_API void SaveToFile( FThreadPool*             iThreadPool
                         , bool                     iBlocking
                         , uint32                   iPerfIntent
                         , const FHostDeviceInfo&   iHostDeviceInfo
                         , bool                     iCallCB
                         , const FRasterImage2D*            iSource
                         , const std::string&       iPath
                         , eImageFormat             iImageFormat
                         , int                      iQuality );

ULIS_NAMESPACE_END

