// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         ParallelFor.h
* @author       Clement Berthaud
* @brief        This file provides the declarations for the ParallelFor utilities.
* @copyright    Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Base/Core.h"
#include <functional>

ULIS2_NAMESPACE_BEGIN
class FThreadPool;
/////////////////////////////////////////////////////
// ParallelFor
#define ULIS2_PF_CALL [=]( int32 iLine )
#define ULIS2_PF_CALL_CAPTURE_VALUE_SAFE_NONBLOCKING( ... ) [ __VA_ARGS__ ]( int32 iLine )
ULIS2_API void ParallelFor( FThreadPool& iPool, bool iBlocking, const FPerf& iPerf, int32 iNum, const std::function< void( int32 ) >& iFun );

ULIS2_NAMESPACE_END

