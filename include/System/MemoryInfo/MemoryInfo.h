// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         MemoryInfo.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FMemoryInfo tools.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/// @class      FMemoryInfo
/// @brief      The FMemoryInfo class provides a mean of fetching informations
///             about the memory or RAM at runtime.
class ULIS_API FMemoryInfo
{
private:
    ~FMemoryInfo() = delete;
    FMemoryInfo() = delete;
    FMemoryInfo( const FMemoryInfo& ) = delete;
    FMemoryInfo( FMemoryInfo&& ) = delete;

public:
};

ULIS_NAMESPACE_END

