// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         CRC32.h
* @author       Clement Berthaud
* @brief        This file provides declaration for CRC32 function.
* @copyright    Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core.h"

ULIS2_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @fn         CRC32( const uint8* iData, int iLen )
/// @brief      Simple CRC32 computation for hashing purposes.
/// @details    Hashes the data of with specified size in bytes. The implementation is not optimal in terms of efficiency, but is simple and straightforward.
/// @param      iData The data to hash, cast as pointer to const uint8.
/// @param      iLen The size of the data to hash in bytes.
/// @return     An uint32 representing the CRC32 hash.
ULIS2_API uint32 CRC32( const uint8* iData, int iLen );

ULIS2_NAMESPACE_END

