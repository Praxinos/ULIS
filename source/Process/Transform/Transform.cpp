// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         Transform.cpp
* @author       Clement Berthaud
* @brief        This file provides the definition for Transform API.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include "Process/Transform/Transform.h"

// Include SSE RGBA8 Implementation
#ifdef ULIS_COMPILETIME_SSE_SUPPORT
#include "Process/Transform/RGBA8/ResizeMT_NN_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/ResizeMT_Bilinear_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/ResizeMT_Bicubic_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/ResizeMT_Area_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformAffineMT_NN_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformAffineMT_Bilinear_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformAffineMT_Bicubic_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformAffineTiledMT_NN_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformAffineTiledMT_Bilinear_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformAffineTiledMT_Bicubic_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformPerspectiveMT_NN_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformPerspectiveMT_Bilinear_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformPerspectiveMT_Bicubic_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformBezier_NN_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformBezier_Bilinear_SSE_RGBA8.h"
#include "Process/Transform/RGBA8/TransformBezier_Bicubic_SSE_RGBA8.h"
#endif // ULIS_COMPILETIME_SSE_SUPPORT

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
// Resize Area
ULIS_BEGIN_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeAreaInvocationSchedulerSelector )
    ULIS_DEFINE_DISPATCHER_SPECIALIZATION(
          &DispatchTestIsUnorderedRGBA8
        , &ScheduleResizeMT_Area_SSE_RGBA8
        , &ScheduleResizeMT_Area_SSE_RGBA8
        , &ScheduleResizeMT_Area_MEM_Generic< uint8 > )
ULIS_END_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeAreaInvocationSchedulerSelector )
/////////////////////////////////////////////////////
// Resize Bicubic
ULIS_BEGIN_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeBicubicInvocationSchedulerSelector )
    ULIS_DEFINE_DISPATCHER_SPECIALIZATION(
          &DispatchTestIsUnorderedRGBA8
        , &ScheduleResizeMT_Bicubic_SSE_RGBA8
        , &ScheduleResizeMT_Bicubic_SSE_RGBA8
        , &ScheduleResizeMT_Bicubic_MEM_Generic< uint8 > )
ULIS_END_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeBicubicInvocationSchedulerSelector )
/////////////////////////////////////////////////////
// Resize Bilinear
ULIS_BEGIN_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeBilinearInvocationSchedulerSelector )
    ULIS_DEFINE_DISPATCHER_SPECIALIZATION(
          &DispatchTestIsUnorderedRGBA8
        , &ScheduleResizeMT_Bilinear_SSE_RGBA8
        , &ScheduleResizeMT_Bilinear_SSE_RGBA8
        , &ScheduleResizeMT_Bilinear_MEM_Generic< uint8 > )
ULIS_END_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeBilinearInvocationSchedulerSelector )
/////////////////////////////////////////////////////
// Resize NN
ULIS_BEGIN_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeNNInvocationSchedulerSelector )
    ULIS_DEFINE_DISPATCHER_SPECIALIZATION(
          &DispatchTestIsUnorderedRGBA8
        , &ScheduleResizeMT_NN_SSE_RGBA8
        , &ScheduleResizeMT_NN_SSE_RGBA8
        , &ScheduleResizeMT_NN_MEM_Generic< uint8 > )
ULIS_END_DISPATCHER_SPECIALIZATION_DEFINITION( FDispatchedResizeNNInvocationSchedulerSelector )

ULIS_NAMESPACE_END

