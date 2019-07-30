/*************************************************************************
*
*   ULIS
*__________________
*
* ULIS.Paint.PainterContext.h
* Thomas Schmitt
* Please refer to LICENSE.md
*/

#pragma once

#include <assert.h>
#include "ULIS/Base/ULIS.Base.BaseTypes.h"
#include "ULIS/Base/ULIS.Base.PerfStrat.h"
#include "ULIS/Data/ULIS.Data.Block.h"

namespace ULIS {
/////////////////////////////////////////////////////
// Defines
#define tSpec TBlockInfo< _SH >

/////////////////////////////////////////////////////
// TPainterContext
template< uint32 _SH >
class TPainterContext
{
public:
    static void DrawLine( TBlock< _SH >*            iBlock
                         , const FPoint             iP0
                         , const FPoint             iP1
                         , const CColor&            iColor
                         , const FPerfStrat&        iPerfStrat
                         , bool                     callInvalidCB )
    {
        TPixelValue< _SH > val = iBlock->PixelValueForColor( iColor );

        FPoint p0;
        FPoint p1;
        

        if( ::ULIS::FMath::Abs( iP1.y - iP0.y ) < ::ULIS::FMath::Abs( iP1.x - iP0.x )) //x slope > y slope
        {
            if( iP1.x > iP0.x )
            {
                p0 = iP0;
                p1 = iP1;
            }
            else
            {
                p0 = iP1;
                p1 = iP0;
            }
            
            int dx = p1.x - p0.x;
            int dy = p1.y - p0.y;
            int yStep = 1;
        
            if( dy < 0)
            {
                yStep = -1;
                dy = -dy;
            }
        
            int slopeDifferential = 2 * dy - dx;
            int y = p0.y;
        
            for( int x = p0.x; x < p1.x; x++)
            {
                iBlock->SetPixelValue( x, y, val );
                
                if( slopeDifferential > 0 )
                {
                    y += yStep;
                    slopeDifferential-=(2 * dx);
                }
                slopeDifferential+=(2 * dy);
            }
        }
        else //y slope > x slope
        {
            if( iP1.y > iP0.y )
            {
                p0 = iP0;
                p1 = iP1;
            }
            else
            {
                p0 = iP1;
                p1 = iP0;
            }
            
            int dx = p1.x - p0.x;
            int dy = p1.y - p0.y;
            int xStep = 1;
        
            if( dx < 0)
            {
                xStep = -1;
                dx = -dx;
            }
        
            int slopeDifferential = 2 * dx - dy;
            int x = p0.x;
        
            for( int y = p0.y; y < p1.y; y++)
            {
                iBlock->SetPixelValue( x, y, val );
                
                if( slopeDifferential > 0 )
                {
                    x += xStep;
                    slopeDifferential-=(2 * dy);
                }
                slopeDifferential+=(2 * dx);
            }
        }
    }
    
static void DrawCircle( TBlock< _SH >*            iBlock
                       , const FPoint             iCenter
                       , const int                iRadius
                       , const CColor&            iColor
                       , const FPerfStrat&        iPerfStrat
                       , bool                     callInvalidCB )
{
    TPixelValue< _SH > val = iBlock->PixelValueForColor( iColor );

    int x = 0;
    int y = iRadius; //We start from the top of the circle
    int diff = iRadius - 1;
    while (y >= x) //We draw one octant, 8 times
    {
        iBlock->SetPixelValue( iCenter.x + x, iCenter.y + y, val ); //First
        iBlock->SetPixelValue( iCenter.x + y, iCenter.y + x, val ); //Fifth
        iBlock->SetPixelValue( iCenter.x - x, iCenter.y + y, val );
        iBlock->SetPixelValue( iCenter.x - y, iCenter.y + x, val );
        iBlock->SetPixelValue( iCenter.x + x, iCenter.y - y, val );
        iBlock->SetPixelValue( iCenter.x + y, iCenter.y - x, val );
        iBlock->SetPixelValue( iCenter.x - x, iCenter.y - y, val );
        iBlock->SetPixelValue( iCenter.x - y, iCenter.y - x, val );
        if( diff >= ( 2 * x ) )
        {
            diff -= ( 2 * x + 1 );
            x++;
        }
        else if ( diff < ( 2 * ( iRadius - y ) ) )
        {
            diff += ( 2 * y - 1 );
            y--;
        }
        else
        {
            diff += (2 * ( y - x - 1 ) );
            y--;
            x++;
        }
    }
}
    
};

/////////////////////////////////////////////////////
// Undefines
#undef tSpec

} // namespace ULIS
