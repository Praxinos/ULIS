
/*************************************************************************
*
*   ULIS
*__________________
*
* ULIS.Interface.Paint.h
* Thomas Schmitt
* Please refer to LICENSE.md
*/

#pragma once

#include "ULIS/Base/ULIS.Base.BaseTypes.h"
#include "ULIS/Base/ULIS.Base.PerfStrat.h"
#include "ULIS/Color/ULIS.Color.CColor.h"
#include "ULIS/Maths/ULIS.Maths.Geometry.h"

namespace ULIS { class IBlock; }

namespace ULIS {
/////////////////////////////////////////////////////
// FClearFillContext
class FPainterContext
{
public:
    static void DrawLine( IBlock* iBlock, const FPoint p0, const FPoint p1, const CColor& iColor, const FPerfStrat& iPerfStrat = FPerfStrat(), bool callInvalidCB = true );
    static void DrawCircle( IBlock* iBlock, const FPoint iCenter, const int iRadius, const CColor& iColor, const FPerfStrat& iPerfStrat = FPerfStrat(), bool callInvalidCB = true );
};

} // namespace ULIS

