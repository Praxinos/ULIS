// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedDrawable.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TAnimatedDrawable class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Rectangle.h"
#include "Math/Geometry/Vector.h"
#include "Scheduling/Event.h"
#include "Scheduling/SchedulePolicy.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TAnimatedDrawable
/// @brief      The TAnimatedDrawable class provides a base abstract interface to
///             perform cached renders of contents for layers
class ULIS_API TAnimatedDrawable
{
protected:
    TAnimatedDrawable();

public:
    virtual FEvent RenderCache( FContext& iCtx );
    virtual FEvent RenderImage(
          FContext& iCtx
        , FBlock& ioBlock
        , uint32 iFrame
        , const FRectI& iRect = FRectI::Auto
        , const FVec2I& iPos = FVec2I( 0 )
        , const FSchedulePolicy& iPolicy = FSchedulePolicy()
        , uint32 iNumWait = 0
        , const FEvent* iWaitList = nullptr
    ) = 0;


    bool IsImageCacheValid() const;
    virtual void InvalidImageCache();

protected:
    void ValidateImageCache();

private:
    bool mImageCacheValid;
};

ULIS_NAMESPACE_END

#include "Layer/Components/AnimatedDrawable.tpp"

