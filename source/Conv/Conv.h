// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*
*   ULIS
*__________________
*
* @file         Conv.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Conv entry point functions.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Conv/ConvArgs.h"

ULIS_NAMESPACE_BEGIN
ULIS_API void Conv( const ISample& iSrc, ISample& iDst );
ULIS_API FColor Conv( const ISample& iSrc, eFormat iDst );

void
InvokeConvertFormat(
      const FConvJobArgs* jargs
    , const FConvCommandArgs* cargs
);

void
ScheduleConvertFormat(
      FCommand* iCommand
    , const FSchedulePolicy& iPolicy
);

ULIS_NAMESPACE_END

