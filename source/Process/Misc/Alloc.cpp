// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Alloc.cpp
* @author       Clement Berthaud
* @brief        This file provides the definition for the Alloc API.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Process/Misc/Alloc.h"
#include "Image/Block.h"
#include <new>

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// Invocations
void
InvokeAlloc(
      const FSimpleBufferJobArgs* jargs
    , const FAllocCommandArgs* cargs
)
{
    ULIS_ASSERT( cargs->size.x  > 0, "Width must be greater than zero" );
    ULIS_ASSERT( cargs->size.y > 0, "Height must be greater than zero" );
    uint16 width = cargs->size.x;
    uint16 height = cargs->size.y;
    FFormatMetrics fmt_metrics( cargs->format );
    uint32 bytesPerScanline = width * fmt_metrics.BPP;
    uint64 bytesTotal = height * bytesPerScanline;
    ULIS_ASSERT( bytesTotal != 0, "Cannot allocate a buffer of size 0" );

    uint8* bitmap = new  ( std::nothrow )  uint8[ bytesTotal ];
    ULIS_ASSERT( bitmap, "Allocation failed with requested size: " << bytesTotal << " bytes" );

    cargs->dst.LoadFromData( bitmap, width, height, cargs->format, cargs->colorspace, cargs->onInvalid, cargs->onCleanup );
}

void
InvokeDealloc(
      const FSimpleBufferJobArgs* jargs
    , const FSimpleBufferCommandArgs* cargs
)
{
    FBlock& block = cargs->dst;
    auto w = block.Width();
    auto h = block.Height();
    auto fmt = block.Format();
    auto cs = block.ColorSpace();
    cargs->dst.LoadFromData( nullptr, w, h, fmt, nullptr, FOnInvalidBlock(), FOnCleanupData( &OnCleanup_FreeMemory ) );
}

/////////////////////////////////////////////////////
// Dispatch / Schedule
ULIS_DEFINE_COMMAND_SCHEDULER_FORWARD_SIMPLE( ScheduleAlloc, FSimpleBufferJobArgs, FAllocCommandArgs, &InvokeAlloc )
ULIS_DEFINE_COMMAND_SCHEDULER_FORWARD_SIMPLE( ScheduleDealloc, FSimpleBufferJobArgs, FSimpleBufferCommandArgs, &InvokeDealloc )

ULIS_NAMESPACE_END

