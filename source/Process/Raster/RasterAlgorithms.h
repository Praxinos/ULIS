// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         RasterAlgorithms.h
* @author       Thomas Schmitt
* @brief        This file provides the actual functions and structs used in the Raster API
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/

#pragma once
#include "Core/Core.h"
#include "Math/Geometry/Rectangle.h"
#include "Image/Block.h"

#include <map>

ULIS_NAMESPACE_BEGIN
namespace eClippingZone
{
const uint8 kInside   = 0b0000;
const uint8 kLeft     = 0b0001;
const uint8 kRight    = 0b0010;
const uint8 kBot      = 0b0100;
const uint8 kTop      = 0b1000;
};
typedef uint8 tClippingCode;

static const int InternalComputeCodeForPoint (const FVec2I& iPoint,const FRectI& iClippingRect)
{
    // initialized as being inside
    int code = eClippingZone::kInside;

    if(iPoint.x < iClippingRect.x)       // to the left of rectangle
        code |= eClippingZone::kLeft;
    else if(iPoint.x > (iClippingRect.x + iClippingRect.w))  // to the right of rectangle
        code |= eClippingZone::kRight;
    if(iPoint.y < iClippingRect.y)       // above the rectangle
        code |= eClippingZone::kTop;
    else if(iPoint.y > (iClippingRect.y + iClippingRect.h))  // below the rectangle
        code |= eClippingZone::kBot;

    return code;
}

// Returns true if ioP0 and ioP1 are different at the end of the algorithm
static const bool InternalCropLineToRectangle(FVec2I& ioP0,FVec2I& ioP1,const FRectI& iClippingRect)
{
    int xMax = iClippingRect.x + iClippingRect.w;
    int yMax = iClippingRect.y + iClippingRect.h;
    int xMin = iClippingRect.x;
    int yMin = iClippingRect.y;

    tClippingCode codeP0 = InternalComputeCodeForPoint(ioP0,iClippingRect);
    tClippingCode codeP1 = InternalComputeCodeForPoint(ioP1,iClippingRect);
    bool accept = false;

    while(true)
    {
        if((codeP0 == 0) && (codeP1 == 0))
        {
            // If both endpoints lie within rectangle
            accept = true;
            break;
        } 
        else if(codeP0 & codeP1)
        {
            // If both endpoints are outside rectangle,
            // in same region
            break;
        } 
        else
        {
            // Some segment of line lies within the
            // rectangle
            tClippingCode code_out;
            int x,y;

            // At least one endpoint is outside the
            // rectangle, pick it.
            if(codeP0 != 0)
                code_out = codeP0;
            else
                code_out = codeP1;

            // Find intersection point;
            // using formulas y = y1 + slope * (x - x1),
            // x = x1 + (1 / slope) * (y - y1)
            if(code_out & eClippingZone::kBot)
            {
                // point is above the clip rectangle
                x = ioP0.x + (ioP1.x - ioP0.x) * (yMax - ioP0.y) / (ioP1.y - ioP0.y);
                y = yMax;
            } 
            else if(code_out & eClippingZone::kTop)
            {
                // point is below the rectangle
                x = ioP0.x + (ioP1.x - ioP0.x) * (yMin - ioP0.y) / (ioP1.y - ioP0.y);
                y = yMin;
            } 
            else if(code_out & eClippingZone::kRight)
            {
                // point is to the right of rectangle
                y = ioP0.y + (ioP1.y - ioP0.y) * (xMax - ioP0.x) / (ioP1.x - ioP0.x);
                x = xMax;
            } 
            else if(code_out & eClippingZone::kLeft)
            {
                // point is to the left of rectangle
                y = ioP0.y + (ioP1.y - ioP0.y) * (xMin - ioP0.x) / (ioP1.x - ioP0.x);
                x = xMin;
            }

            // Now intersection point x,y is found
            // We replace point outside rectangle
            // by intersection point
            if(code_out == codeP0)
            {
                ioP0.x = x;
                ioP0.y = y;
                codeP0 = InternalComputeCodeForPoint(ioP0,iClippingRect);
            } 
            else
            {
                ioP1.x = x;
                ioP1.y = y;
                codeP1 = InternalComputeCodeForPoint(ioP1,iClippingRect);
            }
        }
    }
    return accept;
}

static const float InternalGetPixelBaseAlphaFromCoord( FVec2F& iPt )
{
    float xDec = iPt.x - int( iPt.x );
    float xAlpha = 0.f;

    if( xDec <= 0.5f )
        xAlpha = xDec + (0.5f - xDec) / 2;
    else
        xAlpha = 1.f - xDec + (xDec - 0.5f) / 2;

    float yDec = iPt.y - int(iPt.y);
    float yAlpha = 0.f;

    if(yDec <= 0.5f)
        yAlpha = yDec + (0.5f - yDec) / 2;
    else
        yAlpha = 1.f - yDec + (yDec - 0.5f) / 2;

    return ( xAlpha + yAlpha );
}


static void DrawLine( FBlock&       iBlock
                    , const FVec2I& iP0
                    , const FVec2I& iP1
                    , const FColor& iColor 
                    , const FRectI& iClippingRect )
{
    //Clipping ----
    
    FVec2I p0 = iP0;
    FVec2I p1 = iP1;
    
    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;
    
    if(clippingRect.Area() == 0)
    {
        clippingRect = FRectI::FromXYWH(0,0, iBlock.Width() - 1, iBlock.Height() - 1);
    }

    if(!InternalCropLineToRectangle(p0,p1,clippingRect))
        return; //Nothing to draw

    //Drawing ----
    FColor val = iColor;
    
    if(::ULIS::FMath::Abs(p1.y - p0.y) < ::ULIS::FMath::Abs(p1.x - p0.x)) // x slope > y slope
    {
        if(p1.x < p0.x)
        {
            FVec2I temp = p0;
            p0 = p1;
            p1 = temp;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        int yStep = 1;

        if(dy < 0)
        {
            yStep = -1;
            dy = -dy;
        }

        int slopeDifferential = 2 * dy - dx;
        int y = p0.y;

        for(int x = p0.x; x <= p1.x; x++)
        {
            iBlock.SetPixel( x, y, val );

            if(slopeDifferential > 0)
            {
                y += yStep;
                slopeDifferential-=(2 * dx);
            }
            slopeDifferential+=(2 * dy);
        }
    } 
    else //y slope > x slope
    {
        if(p1.y < p0.y)
        {
            FVec2I temp = p0;
            p0 = p1;
            p1 = temp;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        int xStep = 1;

        if(dx < 0)
        {
            xStep = -1;
            dx = -dx;
        }

        int slopeDifferential = 2 * dx - dy;
        int x = p0.x;

        for(int y = p0.y; y <= p1.y; y++)
        {
            iBlock.SetPixel(x,y,val);

            if(slopeDifferential > 0)
            {
                x += xStep;
                slopeDifferential-=(2 * dy);
            }
            slopeDifferential+=(2 * dx);
        }
    }
}

template< typename T >
void DrawLineAA(      FBlock&     iBlock
                    , const FVec2I& iP0
                    , const FVec2I& iP1
                    , const FColor& iColor 
                    , const FRectI& iClippingRect )
{
    //Clipping ----
    FVec2I p0 = iP0;
    FVec2I p1 = iP1;
    
    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;
    
    if( clippingRect.Area() == 0 )
    {
        clippingRect = FRectI::FromXYWH(0, 0, iBlock.Width() - 1, iBlock.Height() - 1);
    }
    
    int xMax = clippingRect.x + clippingRect.w;
    int yMax = clippingRect.y + clippingRect.h;
    int xMin = clippingRect.x;
    int yMin = clippingRect.y;
    
    int codeP0 = InternalComputeCodeForPoint( p0, clippingRect );
    int codeP1 = InternalComputeCodeForPoint( p1, clippingRect );
    
    bool accept = false;

    while (true)
    {
        if ((codeP0 == 0) && (codeP1 == 0))
        {
            // If both endpoints lie within rectangle
            accept = true;
            break;
        }
        else if (codeP0 & codeP1)
        {
            // If both endpoints are outside rectangle,
            // in same region
            break;
        }
        else
        {
            // Some segment of line lies within the
            // rectangle
            int code_out;
            int x, y;
  
            // At least one endpoint is outside the
            // rectangle, pick it.
            if (codeP0 != 0)
                code_out = codeP0;
            else
                code_out = codeP1;
  
            // Find intersection point;
            // using formulas y = y1 + slope * (x - x1),
            // x = x1 + (1 / slope) * (y - y1)
            if (code_out & eClippingZone::kBot)
            {
                // point is above the clip rectangle
                x = p0.x + (p1.x - p0.x) * (yMax - p0.y) / (p1.y - p0.y);
                y = yMax;
            }
            else if (code_out & eClippingZone::kTop)
            {
                // point is below the rectangle
                x = p0.x + (p1.x - p0.x) * (yMin - p0.y) / (p1.y - p0.y);
                y = yMin;
            }
            else if (code_out & eClippingZone::kRight)
            {
                // point is to the right of rectangle
                y = p0.y + (p1.y - p0.y) * (xMax - p0.x) / (p1.x - p0.x);
                x = xMax;
            }
            else if (code_out & eClippingZone::kLeft)
            {
                // point is to the left of rectangle
                y = p0.y + (p1.y - p0.y) * (xMin - p0.x) / (p1.x - p0.x);
                x = xMin;
            }
  
            // Now intersection point x,y is found
            // We replace point outside rectangle
            // by intersection point
            if (code_out == codeP0)
            {
                p0.x = x;
                p0.y = y;
                codeP0 = InternalComputeCodeForPoint(p0, clippingRect);
            }
            else
            {
                p1.x = x;
                p1.y = y;
                codeP1 = InternalComputeCodeForPoint(p1, clippingRect);
            }
        }
    }
    
    if( !accept )
        return; //Nothing to draw
    
    //Drawing
    FColor val = iColor;
    T maxAlpha = val.AlphaT<T>();

    if( ::ULIS::FMath::Abs( p1.y - p0.y ) < ::ULIS::FMath::Abs( p1.x - p0.x )) //x slope > y slope
    {
        if( p1.x < p0.x )
        {
            ULIS::FVec2I temp = p0;
            p0 = p1;
            p1 = temp;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        int yStep = 1;

        if( dy < 0)
        {
            yStep = -1;
            dy = -dy;
        }

        int errMax = -2 * dx + 2 * dy + 1;
        int errMin = 2 * dy - 1;
        int slopeDifferential = 2 * dy - dx;
        int y = p0.y;

        for( int x = p0.x; x <= p1.x; x++)
        {
            float alphaTop = float(1 - FMath::Abs( ( float( slopeDifferential - errMax ) / float( errMin - errMax ) ) - 0.5 ) ); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            iBlock.SetPixel( x, y, val );

            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop)) );
            iBlock.SetPixel( x, y + yStep, val );

            if( slopeDifferential >= dx )
            {
                y += yStep;
                slopeDifferential-=(2 * dx);
            }
            slopeDifferential+=(2 * dy);
        }
    }
    else //y slope > x slope
    {
        if( p1.y < p0.y )
        {
            ULIS::FVec2I temp = p0;
            p0 = p1;
            p1 = temp;
        }

        int dx = p1.x - p0.x;
        int dy = p1.y - p0.y;
        int xStep = 1;

        if( dx < 0)
        {
            xStep = -1;
            dx = -dx;
        }

        int errMax = -2 * dy + 2 * dx + 1;
        int errMin = 2 * dx - 1;
        int slopeDifferential = 2 * dx - dy;
        int x = p0.x;

        for( int y = p0.y; y <= p1.y; y++)
        {
            float alphaTop = float(1 - FMath::Abs( ( float( slopeDifferential - errMax ) / float( errMin - errMax ) ) - 0.5 ) ); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            iBlock.SetPixel( x, y, val );

            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            iBlock.SetPixel( x + xStep, y, val );

            if( slopeDifferential >= dy )
            {
                x += xStep;
                slopeDifferential-=(2 * dy);
            }
            slopeDifferential+=(2 * dx);
        }
    }
}

template< typename T >
void DrawLineSP(      FBlock&     iBlock
                    , const FVec2F& iP0
                    , const FVec2F& iP1
                    , const FColor& iColor 
                    , const FRectI& iClippingRect )
{
    //Clipping ----
    FVec2F p0 = iP0;
    FVec2F p1 = iP1;
    
    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;
    
    if( clippingRect.Area() == 0 )
    {
        clippingRect = FRectI::FromXYWH(0, 0, iBlock.Width() - 1, iBlock.Height() - 1);
    }
    
    int xMax = clippingRect.x + clippingRect.w;
    int yMax = clippingRect.y + clippingRect.h;
    int xMin = clippingRect.x;
    int yMin = clippingRect.y;
    
    int codeP0 = InternalComputeCodeForPoint( p0, clippingRect );
    int codeP1 = InternalComputeCodeForPoint( p1, clippingRect );
    
    bool accept = false;

    while (true)
    {
        if ((codeP0 == 0) && (codeP1 == 0))
        {
            // If both endpoints lie within rectangle
            accept = true;
            break;
        }
        else if (codeP0 & codeP1)
        {
            // If both endpoints are outside rectangle,
            // in same region
            break;
        }
        else
        {
            // Some segment of line lies within the
            // rectangle
            int code_out;
            int x, y;
  
            // At least one endpoint is outside the
            // rectangle, pick it.
            if (codeP0 != 0)
                code_out = codeP0;
            else
                code_out = codeP1;
  
            // Find intersection point;
            // using formulas y = y1 + slope * (x - x1),
            // x = x1 + (1 / slope) * (y - y1)
            if (code_out & eClippingZone::kBot)
            {
                // point is above the clip rectangle
                x = int(p0.x + (p1.x - p0.x) * (yMax - p0.y) / (p1.y - p0.y));
                y = yMax;
            }
            else if (code_out & eClippingZone::kTop)
            {
                // point is below the rectangle
                x = int(p0.x + (p1.x - p0.x) * (yMin - p0.y) / (p1.y - p0.y));
                y = yMin;
            }
            else if (code_out & eClippingZone::kRight)
            {
                // point is to the right of rectangle
                y = int(p0.y + (p1.y - p0.y) * (xMax - p0.x) / (p1.x - p0.x));
                x = xMax;
            }
            else if (code_out & eClippingZone::kLeft)
            {
                // point is to the left of rectangle
                y = int(p0.y + (p1.y - p0.y) * (xMin - p0.x) / (p1.x - p0.x));
                x = xMin;
            }
  
            // Now intersection point x,y is found
            // We replace point outside rectangle
            // by intersection point
            if (code_out == codeP0)
            {
                p0.x = float(x);
                p0.y = float(y);
                codeP0 = InternalComputeCodeForPoint(p0, clippingRect);
            }
            else
            {
                p1.x = float(x);
                p1.y = float(y);
                codeP1 = InternalComputeCodeForPoint(p1, clippingRect);
            }
        }
    }
    
    if( !accept )
        return; //Nothing to draw
    
    //Drawing
    FColor val = iColor;
    T maxAlpha = val.AlphaT<T>();

    if( ::ULIS::FMath::Abs( p1.y - p0.y ) < ::ULIS::FMath::Abs( p1.x - p0.x )) //x slope > y slope
    {
        if( p1.x < p0.x )
        {
            ULIS::FVec2F temp = p0;
            p0 = p1;
            p1 = temp;
        }

        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        int yStep = 1;

        if( dy < 0)
        {
            yStep = -1;
            dy = -dy;
        }

        float errMax = -2 * dx + 2 * dy + 1;
        float errMin = 2 * dy - 1;
        float slopeDifferential = 2 * dy - (2*dx)*InternalGetPixelBaseAlphaFromCoord(p0);
        float y = p0.y;
        
        for( float x = p0.x; x <= p1.x; x++)
        {
            float alphaTop = float(1 - FMath::Abs( ( float( slopeDifferential - errMax ) / float( errMin - errMax ) ) ) ); //Interpolation of slopedifferential between errMin and errMax

            if (alphaTop <= 0)
            {
                y += yStep;
                slopeDifferential -= (2 * dx);
                alphaTop += 1;
            }

            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            iBlock.SetPixel( int(x), int(y), val );

            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop)) );
            iBlock.SetPixel( int(x), int(y + yStep), val );

            slopeDifferential+=(2 * dy);
        }
    }
    else //y slope > x slope
    {
        if( p1.y < p0.y )
        {
            ULIS::FVec2F temp = p0;
            p0 = p1;
            p1 = temp;
        }

        float dx = p1.x - p0.x;
        float dy = p1.y - p0.y;
        int xStep = 1;

        if( dx < 0)
        {
            xStep = -1;
            dx = -dx;
        }

        float errMax = -2 * dy + 2 * dx + 1;
        float errMin = 2 * dx - 1;
        float slopeDifferential = 2 * dx - (2 * dy)*InternalGetPixelBaseAlphaFromCoord(p0);
        float x = p0.x;

        for( float y = p0.y; y <= p1.y; y++)
        {
            float alphaTop = float(1 - FMath::Abs( ( float( slopeDifferential - errMax ) / float( errMin - errMax ) ) ) ); //Interpolation of slopedifferential between errMin and errMax

            if (alphaTop <= 0)
            {
                x += xStep;
                slopeDifferential -= (2 * dy);
                alphaTop+=1;
            }

            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            iBlock.SetPixel( int(x), int(y), val );

            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            iBlock.SetPixel( int(x + xStep), int(y), val );


            slopeDifferential+=(2 * dx);
        }
    }
}

static void InternalDrawQuadRationalBezierSeg( FBlock& iBlock
                                             , int x0
                                             , int y0
                                             , int x1
                                             , int y1
                                             , int x2
                                             , int y2
                                             , float w
                                             , const FColor& iColor
                                             , const FRectI& iClippingRect
                                             , std::map< int,std::vector< int > >* iStoragePoints = NULL)
{
    FColor val = iColor;

    int sx = x2-x1,sy = y2-y1;
    double dx = x0-x2,dy = y0-y2,xx = x0-x1,yy = y0-y1;
    double xy = xx*sy+yy*sx,cur = xx*sy-yy*sx,err;

    if(xx*sx > 0.0 || yy*sy > 0.0)
    {
        return;
    }

    if(cur != 0.0 && w > 0.0) {
        if(sx*(long)sx+sy*(long)sy > xx*xx+yy*yy) {
            x2 = x0; x0 -= int(dx); y2 = y0; y0 -= int(dy); cur = -cur;
        }
        xx = 2.0*(4.0*w*sx*xx+dx*dx);
        yy = 2.0*(4.0*w*sy*yy+dy*dy);
        sx = x0 < x2 ? 1 : -1;
        sy = y0 < y2 ? 1 : -1;
        xy = -2.0*sx*sy*(2.0*w*xy+dx*dy);
        if(cur*sx*sy < 0.0)
        {
            xx = -xx; yy = -yy; xy = -xy; cur = -cur;
        }
        dx = 4.0*w*(x1-x0)*sy*cur+xx/2.0+xy;
        dy = 4.0*w*(y0-y1)*sx*cur+yy/2.0+xy;
        if(w < 0.5 && dy > dx)
        {
            cur = -(w+1.0)/2.0;
            w = sqrt(w);
            xy = 1.0/(w+1.0);
            sx = int(floor((x0+2.0*w*x1+x2)*xy/2.0+0.5));
            sy = int(floor((y0+2.0*w*y1+y2)*xy/2.0+0.5));
            dx = floor((w*x1+x0)*xy+0.5);
            dy = floor((y1*w+y0)*xy+0.5);
            InternalDrawQuadRationalBezierSeg(iBlock,x0,y0,int(dx),int(dy),sx,sy,float(cur),iColor,iClippingRect,iStoragePoints);
            dx = floor((w*x1+x2)*xy+0.5);
            dy = floor((y1*w+y2)*xy+0.5);
            InternalDrawQuadRationalBezierSeg(iBlock,sx,sy,int(dx),int(dy),x2,y2,float(cur),iColor,iClippingRect,iStoragePoints);
            return;
        }
        err = dx+dy-xy;

        // Clipping
        FRectI clippingRect = iClippingRect;
        clippingRect.w--;
        clippingRect.h--;

        if(clippingRect.Area() == 0)
        {
            clippingRect = FRectI::FromXYWH(0,0,iBlock.Width() - 1,iBlock.Height() - 1);
        }

        FRectI bezierRect = FRectI::FromMinMax(FMath::Min3(x0,x1,x2),FMath::Min3(y0,y1,y2),FMath::Max3(x0,x1,x2),FMath::Max3(y0,y1,y2));

        clippingRect = clippingRect & bezierRect;

        while(dy <= xy && dx >= xy)
        {
            if( x0 >= clippingRect.x && x0 <= (clippingRect.x + clippingRect.w) && y0 >= clippingRect.y && y0 <= (clippingRect.y + clippingRect.h) )
                iBlock.SetPixel(x0,y0,val);

            if(iStoragePoints)
            {
                (*iStoragePoints)[x0 - (*iStoragePoints)[0][0]].push_back(y0 - (*iStoragePoints)[0][1]);
            }
            if(x0 == x2 && y0 == y2)
                return;
            x1 = 2 * err > dy; y1 = 2*(err + yy) < -dy;
            if(2 * err < dx || y1) { y0 += sy; dy += xy; err += dx += xx; }
            if(2 * err > dx || x1) { x0 += sx; dx += xy; err += dy += yy; }
        }
    }
    DrawLine(iBlock,FVec2I(x0,y0),FVec2I(x2,y2),iColor,iClippingRect);

    if(iStoragePoints) //We go here only when we draw an ellipse with beziers
    {
        if(x0 == x2 && y0 == y2) //Corners where we draw a single pixel
        {
            (*iStoragePoints)[x0 - (*iStoragePoints)[0][0]].push_back(y0 - (*iStoragePoints)[0][1]);
        } else if(y0 == y2) //Horizontal lines
        {
            for(int i = x0; i < x2; i++)
                (*iStoragePoints)[i - (*iStoragePoints)[0][0]].push_back(y0 - (*iStoragePoints)[0][1]);
        }
        //We don't need to take care of vertical lines, since storagePoints is used to fill an ellipse using the exact same type of vertical lines
    }
}

template< typename T >
static void InternalDrawQuadRationalBezierSegAA(FBlock& iBlock
                                              ,int x0
                                              ,int y0
                                              ,int x1
                                              ,int y1
                                              ,int x2
                                              ,int y2
                                              ,float w
                                              ,const FColor& iColor
                                              ,const FRectI& iClippingRect
                                              ,std::map< int,std::vector< int > >* iStoragePoints = NULL)
{
    FColor val = iColor;
    T maxAlpha = val.AlphaT<T>();

    int sx = x2-x1,sy = y2-y1;
    double dx = x0-x2,dy = y0-y2,xx = x0-x1,yy = y0-y1;
    double xy = xx*sy+yy*sx,cur = xx*sy-yy*sx,err,ed;
    bool f;

    if(xx*sx > 0.0 || yy*sy > 0.0)
    {
        return;
    }

    if(cur != 0.0 && w > 0.0)
    {
        if(sx*(long)sx+sy*(long)sy > xx*xx+yy*yy)
        {
            x2 = x0; x0 -= int(dx); y2 = y0; y0 -= int(dy); cur = -cur;
        }
        xx = 2.0*(4.0*w*sx*xx+dx*dx);
        yy = 2.0*(4.0*w*sy*yy+dy*dy);
        sx = x0 < x2 ? 1 : -1;
        sy = y0 < y2 ? 1 : -1;
        xy = -2.0*sx*sy*(2.0*w*xy+dx*dy);

        if(cur*sx*sy < 0.0)
        {
            xx = -xx; yy = -yy; xy = -xy; cur = -cur;
        }

        dx = 4.0*w*(x1-x0)*sy*cur+xx/2.0+xy;
        dy = 4.0*w*(y0-y1)*sx*cur+yy/2.0+xy;

        if(w < 0.5 && dy > dx)
        {
            cur = -(w+1.0)/2.0;
            w = sqrt(w);
            xy = 1.0/(w+1.0);
            sx = int(floor((x0+2.0*w*x1+x2)*xy/2.0+0.5));
            sy = int(floor((y0+2.0*w*y1+y2)*xy/2.0+0.5));
            dx = floor((w*x1+x0)*xy+0.5);
            dy = floor((y1*w+y0)*xy+0.5);
            InternalDrawQuadRationalBezierSegAA<T>(iBlock,x0,y0,int(dx),int(dy),sx,sy,float(cur),iColor,iClippingRect,iStoragePoints);
            dx = floor((w*x1+x2)*xy+0.5);
            dy = floor((y1*w+y2)*xy+0.5);
            InternalDrawQuadRationalBezierSegAA<T>(iBlock,sx,sy,int(dx),int(dy),x2,y2,float(cur),iColor,iClippingRect,iStoragePoints);
            return;
        }
        err = dx+dy-xy;

        // Clipping
        FRectI clippingRect = iClippingRect;
        clippingRect.w--;
        clippingRect.h--;

        if(clippingRect.Area() == 0)
        {
            clippingRect = FRectI::FromXYWH(0,0,iBlock.Width() - 1,iBlock.Height() - 1);
        }

        while(dy < dx)
        {
            cur = FMath::Min(dx - xy,xy - dy);
            ed = FMath::Max(dx - xy,xy - dy);
            ed += (2 * ed * cur * cur / (4.0 * ed * ed + cur * cur));

            float errorRatio = float((err - dx - dy + xy) / ed);

            T alphaTop = T(maxAlpha * (1 - FMath::Abs(errorRatio)));
            f = (2 * err + dy) < 0;

            if(alphaTop <= maxAlpha)
            {
                if( x0 >= clippingRect.x && x0 <= (clippingRect.x + clippingRect.w) && y0 >= clippingRect.y && y0 <= (clippingRect.y + clippingRect.h) )
                {
                    val.SetAlphaT<T>(alphaTop);
                    iBlock.SetPixel(x0,y0,val);
                }

                if(iStoragePoints && errorRatio >= 0)
                {
                    (*iStoragePoints)[x0 - (*iStoragePoints)[0][0]].push_back(y0 - (*iStoragePoints)[0][1]);
                }
            }

            if(f)
            {
                if(y0 == y2)
                    return;
                if((dx - err) < ed)
                {
                    float errorRatio = float(1 - (dx - err) / ed);
                    float alpha = FMath::Abs(errorRatio);

                    if(x0 > clippingRect.x && x0 < (clippingRect.x + clippingRect.w) && y0 >= clippingRect.y && y0 <= (clippingRect.y + clippingRect.h))
                    {
                        val.SetAlphaT<T>(T(maxAlpha * alpha));
                        iBlock.SetPixel(x0 + sx,y0,val);
                    }

                    if(iStoragePoints && errorRatio <= 0)
                    {
                        (*iStoragePoints)[x0 + sx - (*iStoragePoints)[0][0]].push_back(y0 - (*iStoragePoints)[0][1]);
                    }
                }
            }

            if(2 * err + dx > 0)
            {
                if(x0 == x2)
                    return;
                if((err - dy) < ed)
                {
                    float errorRatio = float(1 - (err - dy) / ed);
                    float alpha = FMath::Abs(errorRatio);

                    if(x0 >= clippingRect.x && x0 <= (clippingRect.x + clippingRect.w) && y0 > clippingRect.y && y0 < (clippingRect.y + clippingRect.h))
                    {
                        val.SetAlphaT<T>(T(maxAlpha * alpha));
                        iBlock.SetPixel(x0,y0 + sy,val);
                    }

                    if(iStoragePoints && errorRatio >= 0)
                    {
                        (*iStoragePoints)[x0 - (*iStoragePoints)[0][0]].push_back(y0 + sy - (*iStoragePoints)[0][1]);
                    }
                }
                x0 += sx;
                dx += xy;
                err += dy += yy;
            }

            if(f)
            {
                y0 += sy;
                dy += xy;
                err += dx += xx;
            }
        }
    }
    DrawLineAA<T>(iBlock,FVec2I(x0,y0),FVec2I(x2,y2),iColor,iClippingRect);

    if(iStoragePoints) //We go here only when we draw an ellipse with beziers
    {
        if(x0 == x2 && y0 == y2) //Corners where we draw a single pixel
        {
            (*iStoragePoints)[x0 - (*iStoragePoints)[0][0]].push_back(y0 - (*iStoragePoints)[0][1]);
        } else if(y0 == y2) //Horizontal lines
        {
            for(int i = x0; i < x2; i++)
                (*iStoragePoints)[i - (*iStoragePoints)[0][0]].push_back(y0 - (*iStoragePoints)[0][1]);
        }
        //We don't need to take care of vertical lines, since storagePoints is used to fill an ellipse using the exact same type of vertical lines
    }
}

void DrawCircleAndres(            FBlock&                  iBlock
                                , const FVec2I&            iCenter
                                , const int                iRadius
                                , const FColor&            iColor
                                , const bool               iFilled
                                , const FRectI&            iClippingRect );

template< typename T >
static void DrawCircleAndresAA(   FBlock&                  iBlock
                                , const FVec2I&            iCenter
                                , const int                iRadius
                                , const FColor&            iColor
                                , const bool               iFilled
                                , const FRectI&            iClippingRect )
{
    //Clipping -----
    int x = 0;
    int y = iRadius; //We start from the top of the circle for the first octant
    
    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;
    
    if( clippingRect.Area() == 0 )
    {
        clippingRect = FRectI::FromXYWH(0, 0, iBlock.Width() - 1, iBlock.Height() - 1);
    }
    
    int shift45 = int(std::sin( FMath::DegToRad( 45 ) ) * iRadius);
    FVec2I point0 = FVec2I( iCenter.x, iCenter.y - iRadius );
    FVec2I point45 = FVec2I( iCenter.x + shift45, iCenter.y - shift45 );
    FVec2I point90 = FVec2I( iCenter.x + iRadius, iCenter.y );
    FVec2I point135 = FVec2I( iCenter.x + shift45, iCenter.y + shift45 );
    FVec2I point180 = FVec2I( iCenter.x, iCenter.y + iRadius );
    FVec2I point225 = FVec2I( iCenter.x - shift45, iCenter.y + shift45 );
    FVec2I point270 = FVec2I( iCenter.x - iRadius, iCenter.y );
    FVec2I point315 = FVec2I( iCenter.x - shift45, iCenter.y - shift45 );
    
    FRectI rectOctant1 = FRectI( point0.x, point0.y - 1, point45.x - point0.x, point45.y - point0.y );
    FRectI rectOctant2 = FRectI( point45.x + 1, point45.y, point90.x - point45.x, point90.y - point45.y );
    FRectI rectOctant3 = FRectI( point135.x + 1, point90.y, point90.x - point135.x, point135.y - point90.y );
    FRectI rectOctant4 = FRectI( point180.x, point135.y + 1, point135.x - point180.x, point180.y - point135.y );
    FRectI rectOctant5 = FRectI( point225.x, point225.y + 1, point180.x - point225.x, point180.y - point225.y );
    FRectI rectOctant6 = FRectI( point270.x - 1, point270.y, point225.x - point270.x, point225.y - point270.y );
    FRectI rectOctant7 = FRectI( point270.x - 1, point315.y, point315.x - point270.x, point270.y - point315.y );
    FRectI rectOctant8 = FRectI( point315.x, point0.y - 1, point0.x - point315.x, point315.y - point0.y );
    
    FRectI rectOctant1Clipped = rectOctant1 & clippingRect;
    FRectI rectOctant2Clipped = rectOctant2 & clippingRect;
    FRectI rectOctant3Clipped = rectOctant3 & clippingRect;
    FRectI rectOctant4Clipped = rectOctant4 & clippingRect;
    FRectI rectOctant5Clipped = rectOctant5 & clippingRect;
    FRectI rectOctant6Clipped = rectOctant6 & clippingRect;
    FRectI rectOctant7Clipped = rectOctant7 & clippingRect;
    FRectI rectOctant8Clipped = rectOctant8 & clippingRect;

    // 0 : Don't draw the octant. 1: Draw fully the octant. 2: Partial draw of the octant (complex case)
    int drawRectOctant1 = rectOctant1Clipped == rectOctant1 ? 1 : rectOctant1Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant2 = rectOctant2Clipped == rectOctant2 ? 1 : rectOctant2Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant3 = rectOctant3Clipped == rectOctant3 ? 1 : rectOctant3Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant4 = rectOctant4Clipped == rectOctant4 ? 1 : rectOctant4Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant5 = rectOctant5Clipped == rectOctant5 ? 1 : rectOctant5Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant6 = rectOctant6Clipped == rectOctant6 ? 1 : rectOctant6Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant7 = rectOctant7Clipped == rectOctant7 ? 1 : rectOctant7Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant8 = rectOctant8Clipped == rectOctant8 ? 1 : rectOctant8Clipped.Area() != 0 ? 2 : 0;
    
    
    //Drawing ------
    FColor val = iColor;
    T maxAlpha = val.AlphaT<T>();

    //0� is on top and we turn clockwise
    //Octant 1 ------
    if( drawRectOctant1 == 1)
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y - y, val ); // 0� to 45�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y - y - 1, val ); // 0� to 45�
            
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
    else if( drawRectOctant1 == 2 )
    {
        int xx = rectOctant1.x;
        int yy = rectOctant1.y;
        int limitX = rectOctant1Clipped.w + rectOctant1Clipped.x;
        int limitY = rectOctant1Clipped.h + rectOctant1Clipped.y;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Left and top clip
        while ( xx < rectOctant1Clipped.x || yy < rectOctant1Clipped.y )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy++;
                x++; xx++;
            }
        }
        
        // Base for drawing the aliased part of the circle on the sides on the clipping rect
        /*if( xx == rectOctant1Clipped.x || yy == rectOctant1Clipped.y ) //We draw only the bottom line of AA until we draw the next y
        {
            while ( 1 )
            {
                float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
                
                val.SetAlphaT<T>( MaxAlpha * alphaTop );
                
                iBlock.SetPixel( iCenter.x + x, iCenter.y - y, val ); // 0� to 45�
                
                if( diff >= ( 2 * x ) )
                {
                    diff -= ( 2 * x + 1 );
                    x++; xx++;
                }
                else if ( diff < ( 2 * ( iRadius - y ) ) )
                {
                    diff += ( 2 * y - 1 );
                    y--; yy++;
                    break;
                }
                else
                {
                    diff += (2 * ( y - x - 1 ) );
                    y--; yy++;
                    x++; xx++;
                    break;
                }
            }
        }*/
        
        //Right and bottom clip
        while (xx <= limitX && yy <= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y - y, val ); // 0� to 45�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y - y - 1, val ); // 0� to 45�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy++;
                x++; xx++;
            }
        }
    }
    
    //Octant 2 ------
    x = 0;
    y = iRadius;
    if( drawRectOctant2 == 1)
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + y, iCenter.y - x, val ); // 90� to 45�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + y + 1, iCenter.y - x, val ); // 90� to 45�
            
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
    else if( drawRectOctant2 == 2 )
    {
        int xx = rectOctant2.x + rectOctant2.w;
        int yy = rectOctant2.y + rectOctant2.h;
        int limitX = rectOctant2Clipped.x;
        int limitY = rectOctant2Clipped.y;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Right and bottom clip
        while ( xx > rectOctant2Clipped.x + rectOctant2Clipped.w || yy > rectOctant2Clipped.y + rectOctant2Clipped.h )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx--;
                x++; yy--;
            }
        }
        
        //Top and left clip
        while (xx >= limitX && yy >= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + y, iCenter.y - x, val ); // 90� to 45�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + y + 1, iCenter.y - x, val ); // 90� to 45�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx--;
                x++; yy--;
            }
        }
    }
    
    //Octant 3 ------
    x = 0;
    y = iRadius;
    if( drawRectOctant3 == 1)
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + y, iCenter.y + x, val ); // 90� to 135�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + y + 1, iCenter.y + x, val ); // 90� to 135�
            
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
    else if( drawRectOctant3 == 2 )
    {
        int xx = rectOctant3.x + rectOctant3.w;
        int yy = rectOctant3.y;
        int limitX = rectOctant3Clipped.x;
        int limitY = rectOctant3Clipped.y + rectOctant3Clipped.h;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Right and top clip
        while ( xx > rectOctant3Clipped.x + rectOctant3Clipped.w || yy < rectOctant3Clipped.y )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx--;
                x++; yy++;
            }
        }
        
        //Top and left clip
        while (xx >= limitX && yy <= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + y, iCenter.y + x, val ); // 90� to 135�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + y + 1, iCenter.y + x, val ); // 90� to 135�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx--;
                x++; yy++;
            }
        }
    }
    
    //Octant 4 ------
    x = 0;
    y = iRadius;
    if( drawRectOctant4 == 1)
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y + y, val ); // 180� to 135�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y + y + 1, val ); // 180� to 135�
            
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
    else if( drawRectOctant4 == 2 )
    {
        int xx = rectOctant4.x;
        int yy = rectOctant4.y + rectOctant4.h;
        int limitX = rectOctant4Clipped.x + rectOctant4Clipped.w;
        int limitY = rectOctant4Clipped.y;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Right and top clip
        while ( xx < rectOctant4Clipped.x || yy > rectOctant4Clipped.y + rectOctant4Clipped.h )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy--;
                x++; xx++;
            }
        }
        
        //Bottom and left clip
        while (xx <= limitX && yy >= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y + y, val ); // 180� to 135�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x + x, iCenter.y + y + 1, val ); // 180� to 135�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy--;
                x++; xx++;
            }
        }
    }
    
    //Octant 5 ------
    x = 0;
    y = iRadius;
    if( drawRectOctant5 == 1)
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y + y, val ); // 180� to 225�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y + y + 1, val ); // 180� to 225�
            
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
    else if( drawRectOctant5 == 2 )
    {
        int xx = rectOctant5.x + rectOctant5.w;
        int yy = rectOctant5.y + rectOctant5.h;
        int limitX = rectOctant5Clipped.x;
        int limitY = rectOctant5Clipped.y;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Left and bottom clip
        while ( xx > rectOctant5Clipped.x + rectOctant5Clipped.w || yy > rectOctant5Clipped.y + rectOctant5Clipped.h )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy--;
                x++; xx--;
            }
        }
        
        //Bottom and left clip
        while (xx >= limitX && yy >= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y + y, val ); // 180� to 225�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y + y + 1, val ); // 180� to 225�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy--;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy--;
                x++; xx--;
            }
        }
    }
    
    
    //Octant 6 ------
    x = 0;
    y = iRadius;
    if( drawRectOctant6 == 1)
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - y, iCenter.y + x, val );  // 270� to 225�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - y - 1, iCenter.y + x, val );  // 270� to 225�
            
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
    else if( drawRectOctant6 == 2 )
    {
        int xx = rectOctant6.x;
        int yy = rectOctant6.y;
        int limitX = rectOctant6Clipped.x + rectOctant6Clipped.w;
        int limitY = rectOctant6Clipped.y + rectOctant6Clipped.h;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Left and bottom clip
        while ( xx < rectOctant6Clipped.x || yy < rectOctant6Clipped.y )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx++;
                x++; yy++;
            }
        }
        
        //Bottom and left clip
        while (xx <= limitX && yy <= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - y, iCenter.y + x, val );  // 270� to 225�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - y - 1, iCenter.y + x, val );  // 270� to 225�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy++;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx++;
                x++; yy++;
            }
        }
    }
    
    
    //Octant 7------
    x = 0;
    y = iRadius;
    if( drawRectOctant7 == 1 )
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - y, iCenter.y - x, val ); // 270� to 315�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - y - 1, iCenter.y - x, val ); // 270� to 315�
            
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
    else if( drawRectOctant7 == 2 )
    {
        int xx = rectOctant7.x;
        int yy = rectOctant7.y + rectOctant7.h;
        int limitX = rectOctant7Clipped.x + rectOctant7Clipped.w;
        int limitY = rectOctant7Clipped.y;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Left and bottom clip
        while ( xx < rectOctant7Clipped.x || yy > rectOctant7Clipped.y + rectOctant7Clipped.h )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx++;
                x++; yy--;
            }
        }
        
        //Bottom and left clip
        while (xx <= limitX && yy >= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - y, iCenter.y - x, val ); // 270� to 315�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - y - 1, iCenter.y - x, val ); // 270� to 315�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; yy--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; xx++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; xx++;
                x++; yy--;
            }
        }
    }
    
    
    //Octant 8------
    x = 0;
    y = iRadius;
    if( drawRectOctant8 == 1)
    {
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        while (y >= x)
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y - y, val ); // 0� to 315�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y - y - 1, val ); // 0� to 315�
            
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
    else if( drawRectOctant8 == 2 )
    {
        int xx = rectOctant8.x + rectOctant8.w;
        int yy = rectOctant8.y;
        int limitX = rectOctant8Clipped.x;
        int limitY = rectOctant8Clipped.y + rectOctant8Clipped.h;
        
        int diff = iRadius - 1;
        int errMax = 2 * (iRadius - 1);
        int errMin = 0;
        
        //Left and bottom clip
        while ( xx > rectOctant8Clipped.x + rectOctant8Clipped.w || yy < rectOctant8Clipped.y )
        {
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy++;
                x++; xx--;
            }
        }
        
        //Bottom and left clip
        while (xx >= limitX && yy <= limitY )
        {
            float alphaTop = FMath::Abs( ( float( diff - errMax ) / float( errMin - errMax ) ) ); //Interpolation of slopedifferential between errMin and errMax
            
            val.SetAlphaT<T>( T(maxAlpha * alphaTop) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y - y, val ); // 0� to 315�
            
            val.SetAlphaT<T>( T(maxAlpha * (1 - alphaTop )) );
            
            iBlock.SetPixel( iCenter.x - x, iCenter.y - y - 1, val ); // 0� to 315�
            
            if( diff >= ( 2 * x ) )
            {
                diff -= ( 2 * x + 1 );
                x++; xx--;
            }
            else if ( diff < ( 2 * ( iRadius - y ) ) )
            {
                diff += ( 2 * y - 1 );
                y--; yy++;
            }
            else
            {
                diff += (2 * ( y - x - 1 ) );
                y--; yy++;
                x++; xx--;
            }
        }
    }

    //Filled
    if( iFilled )
    {
        x = 0;
        y = iRadius;
        int diff = iRadius - 1;

        while (y >= x)
        {
            if( diff >= ( 2 * x ) )
            {
                DrawLine( iBlock, FVec2I( iCenter.x + x, iCenter.y - y ), FVec2I( iCenter.x + x, iCenter.y + y ), iColor, iClippingRect );
                DrawLine( iBlock, FVec2I( iCenter.x - x, iCenter.y - y ), FVec2I( iCenter.x - x, iCenter.y + y ), iColor, iClippingRect );
                DrawLine( iBlock, FVec2I( iCenter.x + y, iCenter.y - x ), FVec2I( iCenter.x + y, iCenter.y + x ), iColor, iClippingRect );
                DrawLine( iBlock, FVec2I( iCenter.x - y, iCenter.y - x ), FVec2I( iCenter.x - y, iCenter.y + x ), iColor, iClippingRect );
                
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
                DrawLine( iBlock, FVec2I( iCenter.x + x, iCenter.y - y ), FVec2I( iCenter.x + x, iCenter.y + y ), iColor, iClippingRect );
                DrawLine( iBlock, FVec2I( iCenter.x - x, iCenter.y - y ), FVec2I( iCenter.x - x, iCenter.y + y ), iColor, iClippingRect );
                DrawLine( iBlock, FVec2I( iCenter.x + y, iCenter.y - x ), FVec2I( iCenter.x + y, iCenter.y + x ), iColor, iClippingRect );
                DrawLine( iBlock, FVec2I( iCenter.x - y, iCenter.y - x ), FVec2I( iCenter.x - y, iCenter.y + x ), iColor, iClippingRect );
                
                diff += (2 * ( y - x - 1 ) );
                y--;
                x++;
            }
        }
    }
}


void DrawCircleBresenham(         FBlock&                  iBlock
                                , const FVec2I&            iCenter
                                , const int                iRadius
                                , const FColor&            iColor
                                , const bool               iFilled
                                , const FRectI&            iClippingRect );

template< typename T >
static void DrawCircleBresenhamAA(    FBlock&                  iBlock
                                    , const FVec2I&            iCenter
                                    , const int                iRadius
                                    , const FColor&            iColor
                                    , const bool               iFilled
                                    , const FRectI&            iClippingRect )
{
    //Clipping -----
    int x = 0;
    int y = iRadius;

    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;

    if(clippingRect.Area() == 0)
    {
        clippingRect = FRectI::FromXYWH(0,0,iBlock.Width() - 1,iBlock.Height() - 1);
    }

    int shift45 = int(std::sin(FMath::DegToRad(45)) * iRadius);
    FVec2I point0 = FVec2I(iCenter.x,iCenter.y - iRadius);
    FVec2I point45 = FVec2I(iCenter.x + shift45,iCenter.y - shift45);
    FVec2I point90 = FVec2I(iCenter.x + iRadius,iCenter.y);
    FVec2I point135 = FVec2I(iCenter.x + shift45,iCenter.y + shift45);
    FVec2I point180 = FVec2I(iCenter.x,iCenter.y + iRadius);
    FVec2I point225 = FVec2I(iCenter.x - shift45,iCenter.y + shift45);
    FVec2I point270 = FVec2I(iCenter.x - iRadius,iCenter.y);
    FVec2I point315 = FVec2I(iCenter.x - shift45,iCenter.y - shift45);

    FRectI rectOctant1 = FRectI(point0.x,point0.y - 1,point45.x - point0.x,point45.y - point0.y);
    FRectI rectOctant2 = FRectI(point45.x + 1,point45.y,point90.x - point45.x,point90.y - point45.y);
    FRectI rectOctant3 = FRectI(point135.x + 1,point90.y,point90.x - point135.x,point135.y - point90.y);
    FRectI rectOctant4 = FRectI(point180.x,point135.y + 1,point135.x - point180.x,point180.y - point135.y);
    FRectI rectOctant5 = FRectI(point225.x,point225.y + 1,point180.x - point225.x,point180.y - point225.y);
    FRectI rectOctant6 = FRectI(point270.x - 1,point270.y,point225.x - point270.x,point225.y - point270.y);
    FRectI rectOctant7 = FRectI(point270.x - 1,point315.y,point315.x - point270.x,point270.y - point315.y);
    FRectI rectOctant8 = FRectI(point315.x,point0.y - 1,point0.x - point315.x,point315.y - point0.y);

    FRectI rectOctant1Clipped = rectOctant1 & clippingRect;
    FRectI rectOctant2Clipped = rectOctant2 & clippingRect;
    FRectI rectOctant3Clipped = rectOctant3 & clippingRect;
    FRectI rectOctant4Clipped = rectOctant4 & clippingRect;
    FRectI rectOctant5Clipped = rectOctant5 & clippingRect;
    FRectI rectOctant6Clipped = rectOctant6 & clippingRect;
    FRectI rectOctant7Clipped = rectOctant7 & clippingRect;
    FRectI rectOctant8Clipped = rectOctant8 & clippingRect;

    // 0 : Don't draw the octant. 1: Draw fully the octant. 2: Partial draw of the octant (complex case)
    int drawRectOctant1 = rectOctant1Clipped == rectOctant1 ? 1 : rectOctant1Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant2 = rectOctant2Clipped == rectOctant2 ? 1 : rectOctant2Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant3 = rectOctant3Clipped == rectOctant3 ? 1 : rectOctant3Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant4 = rectOctant4Clipped == rectOctant4 ? 1 : rectOctant4Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant5 = rectOctant5Clipped == rectOctant5 ? 1 : rectOctant5Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant6 = rectOctant6Clipped == rectOctant6 ? 1 : rectOctant6Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant7 = rectOctant7Clipped == rectOctant7 ? 1 : rectOctant7Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant8 = rectOctant8Clipped == rectOctant8 ? 1 : rectOctant8Clipped.Area() != 0 ? 2 : 0;

    // Drawing ------
    FColor val = iColor;

    T maxAlpha = val.AlphaT<T>();

    //0� is on top and we turn clockwise
    //Octant 1 ------
    if(drawRectOctant1 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0� to 45�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val); // 0� to 45�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant1 == 2)
    {
        int xx = rectOctant1.x;
        int yy = rectOctant1.y;
        int limitX = rectOctant1Clipped.w + rectOctant1Clipped.x;
        int limitY = rectOctant1Clipped.h + rectOctant1Clipped.y;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx < rectOctant1Clipped.x || yy < rectOctant1Clipped.y)
        {
            if(diff > 0)
            {
                y--; yy++;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; xx++;
            diff = diff + 8 * x + 4;
        }

        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; yy++;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0� to 45�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val); // 0� to 45�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 2 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant2 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val); // 90� to 45�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val); // 90� to 45�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant2 == 2)
    {
        int xx = rectOctant2.x + rectOctant2.w;
        int yy = rectOctant2.y + rectOctant2.h;
        int limitX = rectOctant2Clipped.x;
        int limitY = rectOctant2Clipped.y;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx > rectOctant2Clipped.x + rectOctant2Clipped.w || yy > rectOctant2Clipped.y + rectOctant2Clipped.h)
        {
            if(diff > 0)
            {
                y--; xx--;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; yy--;
            diff = diff + 8 * x + 4;
        }

        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; xx--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val); // 90� to 45�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val); // 90� to 45�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy--;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 3 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant3 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val); // 90� to 135�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val); // 90� to 135�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant3 == 2)
    {
        int xx = rectOctant3.x + rectOctant3.w;
        int yy = rectOctant3.y;
        int limitX = rectOctant3Clipped.x;
        int limitY = rectOctant3Clipped.y + rectOctant3Clipped.h;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx > rectOctant3Clipped.x + rectOctant3Clipped.w || yy < rectOctant3Clipped.y)
        {
            if(diff > 0)
            {
                y--; xx--;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; yy++;
            diff = diff + 8 * x + 4;
        }

        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; xx--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val); // 90� to 135�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val); // 90� to 135�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 4 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant4 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val); // 180� to 135�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val); // 180� to 135�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant4 == 2)
    {
        int xx = rectOctant4.x;
        int yy = rectOctant4.y + rectOctant4.h;
        int limitX = rectOctant4Clipped.x + rectOctant4Clipped.w;
        int limitY = rectOctant4Clipped.y;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx < rectOctant4Clipped.x || yy > rectOctant4Clipped.y + rectOctant4Clipped.h)
        {
            if(diff > 0)
            {
                y--; yy--;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; xx++;
            diff = diff + 8 * x + 4;
        }

        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; yy--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val); // 180� to 135�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val); // 180� to 135�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 5 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant5 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val); // 180� to 225�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val); // 180� to 225�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant5 == 2)
    {
        int xx = rectOctant5.x + rectOctant5.w;
        int yy = rectOctant5.y + rectOctant5.h;
        int limitX = rectOctant5Clipped.x;
        int limitY = rectOctant5Clipped.y;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx > rectOctant5Clipped.x + rectOctant5Clipped.w || yy > rectOctant5Clipped.y + rectOctant5Clipped.h)
        {
            if(diff > 0)
            {
                y--; yy--;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; xx--;
            diff = diff + 8 * x + 4;
        }

        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; yy--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val); // 180� to 225�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val); // 180� to 225�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx--;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 6 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant6 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val); // 270� to 225�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val); // 270� to 225�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant6 == 2)
    {
        int xx = rectOctant6.x;
        int yy = rectOctant6.y;
        int limitX = rectOctant6Clipped.x + rectOctant6Clipped.w;
        int limitY = rectOctant6Clipped.y + rectOctant6Clipped.h;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx < rectOctant6Clipped.x || yy < rectOctant6Clipped.y)
        {
            if(diff > 0)
            {
                y--; xx++;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; yy++;
            diff = diff + 8 * x + 4;
        }

        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; xx++;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val); // 270� to 225�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val); // 270� to 225�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 7 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant7 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val); // 270� to 315�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val); // 270� to 315�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant7 == 2)
    {
        int xx = rectOctant7.x;
        int yy = rectOctant7.y + rectOctant7.h;
        int limitX = rectOctant7Clipped.x + rectOctant7Clipped.w;
        int limitY = rectOctant7Clipped.y;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx < rectOctant7Clipped.x || yy > rectOctant7Clipped.y + rectOctant7Clipped.h)
        {
            if(diff > 0)
            {
                y--; xx++;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; yy--;
            diff = diff + 8 * x + 4;
        }

        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; xx++;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val); // 270� to 315�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val); // 270� to 315�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy--;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 8 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant8 == 1)
    {
        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val); // 0� to 315�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val); // 0� to 315�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant8 == 2)
    {
        int xx = rectOctant8.x + rectOctant8.w;
        int yy = rectOctant8.y;
        int limitX = rectOctant8Clipped.x;
        int limitY = rectOctant8Clipped.y + rectOctant8Clipped.h;

        int diff = 5 - 4 * iRadius;
        int errMax = 2 * (4 * iRadius - 5);
        int errMin = 0;

        while(xx > rectOctant8Clipped.x + rectOctant8Clipped.w || yy < rectOctant8Clipped.y)
        {
            if(diff > 0)
            {
                y--; yy++;
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }

            x++; xx--;
            diff = diff + 8 * x + 4;
        }

        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if( alphaTop < 0.f )
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--; yy++;
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val); // 0� to 315�

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val); // 0� to 315�

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx--;
            diff = diff + 8 * x + 4;
        }
    }

    //Filled
    if(iFilled)
    {
        x = 0;
        y = iRadius;
        int diff = 5 - 4 * iRadius;
        while(x <= y)
        {
            if(diff > 0)
            {
                y--;
            }

            DrawLine(iBlock,FVec2I(iCenter.x + x,iCenter.y - y),FVec2I(iCenter.x + x,iCenter.y + y),iColor,iClippingRect);
            DrawLine(iBlock,FVec2I(iCenter.x - x,iCenter.y - y),FVec2I(iCenter.x - x,iCenter.y + y),iColor,iClippingRect);
            DrawLine(iBlock,FVec2I(iCenter.x + y,iCenter.y - x),FVec2I(iCenter.x + y,iCenter.y + x),iColor,iClippingRect);
            DrawLine(iBlock,FVec2I(iCenter.x - y,iCenter.y - x),FVec2I(iCenter.x - y,iCenter.y + x),iColor,iClippingRect);

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    }
}

void DrawArcAndres(           FBlock&                   iBlock
                            , const FVec2I&             iCenter
                            , const int                 iRadius
                            , const int                 iStartDegree
                            , const int                 iEndDegree
                            , const FColor&             iColor
                            , const FRectI&             iClippingRect );

template< typename T >
static void DrawArcAndresAA(  FBlock&                   iBlock
                            , const FVec2I&             iCenter
                            , const int                 iRadius
                            , const int                 iStartDegree
                            , const int                 iEndDegree
                            , const FColor&             iColor
                            , const FRectI&             iClippingRect )
{
    if(iRadius == 0)
        return;

    //Clipping -----
    int x = 0;
    int y = iRadius; //We start from the top of the circle for the first octant
    int errMax = 2 * (iRadius - 1);
    int errMin = 0;

    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;

    if(clippingRect.Area() == 0)
    {
        clippingRect = FRectI::FromXYWH(0,0,iBlock.Width() - 1,iBlock.Height() - 1);
    }

    int shift45 = int(std::sin(FMath::DegToRad(45)) * iRadius);
    FVec2I point0 = FVec2I(iCenter.x,iCenter.y - iRadius);
    FVec2I point45 = FVec2I(iCenter.x + shift45,iCenter.y - shift45);
    FVec2I point90 = FVec2I(iCenter.x + iRadius,iCenter.y);
    FVec2I point135 = FVec2I(iCenter.x + shift45,iCenter.y + shift45);
    FVec2I point180 = FVec2I(iCenter.x,iCenter.y + iRadius);
    FVec2I point225 = FVec2I(iCenter.x - shift45,iCenter.y + shift45);
    FVec2I point270 = FVec2I(iCenter.x - iRadius,iCenter.y);
    FVec2I point315 = FVec2I(iCenter.x - shift45,iCenter.y - shift45);

    FRectI rectOctant1 = FRectI(point0.x,point0.y,point45.x - point0.x,point45.y - point0.y);
    FRectI rectOctant2 = FRectI(point45.x,point45.y,point90.x - point45.x,point90.y - point45.y);
    FRectI rectOctant3 = FRectI(point135.x,point90.y,point90.x - point135.x,point135.y - point90.y);
    FRectI rectOctant4 = FRectI(point180.x,point135.y,point135.x - point180.x,point180.y - point135.y);
    FRectI rectOctant5 = FRectI(point225.x,point225.y,point180.x - point225.x,point180.y - point225.y);
    FRectI rectOctant6 = FRectI(point270.x,point270.y,point225.x - point270.x,point225.y - point270.y);
    FRectI rectOctant7 = FRectI(point270.x,point315.y,point315.x - point270.x,point270.y - point315.y);
    FRectI rectOctant8 = FRectI(point315.x,point0.y,point0.x - point315.x,point315.y - point0.y);

    FRectI rectOctant1Clipped = rectOctant1 & clippingRect;
    FRectI rectOctant2Clipped = rectOctant2 & clippingRect;
    FRectI rectOctant3Clipped = rectOctant3 & clippingRect;
    FRectI rectOctant4Clipped = rectOctant4 & clippingRect;
    FRectI rectOctant5Clipped = rectOctant5 & clippingRect;
    FRectI rectOctant6Clipped = rectOctant6 & clippingRect;
    FRectI rectOctant7Clipped = rectOctant7 & clippingRect;
    FRectI rectOctant8Clipped = rectOctant8 & clippingRect;

    // 0 : Don't draw the octant. 1: Draw fully the octant. 2: Partial draw of the octant (complex case)
    int drawRectOctant1 = rectOctant1Clipped == rectOctant1 ? 1 : rectOctant1Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant2 = rectOctant2Clipped == rectOctant2 ? 1 : rectOctant2Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant3 = rectOctant3Clipped == rectOctant3 ? 1 : rectOctant3Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant4 = rectOctant4Clipped == rectOctant4 ? 1 : rectOctant4Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant5 = rectOctant5Clipped == rectOctant5 ? 1 : rectOctant5Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant6 = rectOctant6Clipped == rectOctant6 ? 1 : rectOctant6Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant7 = rectOctant7Clipped == rectOctant7 ? 1 : rectOctant7Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant8 = rectOctant8Clipped == rectOctant8 ? 1 : rectOctant8Clipped.Area() != 0 ? 2 : 0;

    //Drawing ------
    FColor val = iColor;

    T maxAlpha = val.AlphaT<T>();

    int sizeAngleToDraw = (iEndDegree - iStartDegree + 360) % 360; //Positive modulo
    int currentAngle = iStartDegree;

    int octantsToDraw[8] ={0,0,0,0,0,0,0,0}; // 0: Don't draw the octant. 1: draw fully the octant. 2: draw part of the octant
    int directionToDraw[8][2] ={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; // 1 clockwise, -1 anti-clockwise, 2 both 0 irrelevant, second entry is angle to draw on octant

    if(sizeAngleToDraw < 45)
    {
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = 2;
        directionToDraw[currentAngle / 45][1] = sizeAngleToDraw;
    } else if(currentAngle % 45 == 0)
        octantsToDraw[currentAngle / 45] = 1;
    else
    {
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = -1;
        directionToDraw[currentAngle / 45][1] = 45 - (currentAngle % 45);
    }

    sizeAngleToDraw -= (45 - (iStartDegree % 45));

    while(sizeAngleToDraw >= 45)
    {
        currentAngle = (currentAngle + 45) % 360;
        octantsToDraw[currentAngle / 45] = 1;
        sizeAngleToDraw -= 45;
    }

    if(sizeAngleToDraw > 0)
    {
        currentAngle = (currentAngle + 45) % 360;
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = 1;
        directionToDraw[currentAngle / 45][1] = sizeAngleToDraw;
    }

    //0� is on top and we turn clockwise
    //Octant 1 ------
    if(drawRectOctant1 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant1 == 2)
    {
        int xx = rectOctant1.x;
        int yy = rectOctant1.y;
        int limitX = rectOctant1Clipped.w + rectOctant1Clipped.x;
        int limitY = rectOctant1Clipped.h + rectOctant1Clipped.y;

        int diff = iRadius - 1;

        while(xx < rectOctant1Clipped.x || yy < rectOctant1Clipped.y)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy++;
                x++; xx++;
            }
        }

        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy++;
                x++; xx++;
            }
        }
    }

    //Octant 2 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant2 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant2 == 2)
    {
        int xx = rectOctant2.x + rectOctant2.w;
        int yy = rectOctant2.y + rectOctant2.h;
        int limitX = rectOctant2Clipped.x;
        int limitY = rectOctant2Clipped.y;

        int diff = iRadius - 1;

        while(xx > rectOctant2Clipped.x + rectOctant2Clipped.w || yy > rectOctant2Clipped.y + rectOctant2Clipped.h)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx--;
                x++; yy--;
            }
        }

        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx--;
                x++; yy--;
            }
        }
    }

    //Octant 3 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant3 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
            }


            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant3 == 2)
    {
        int xx = rectOctant3.x + rectOctant3.w;
        int yy = rectOctant3.y;
        int limitX = rectOctant3Clipped.x;
        int limitY = rectOctant3Clipped.y + rectOctant3Clipped.h;

        int diff = iRadius - 1;

        //Right and top clip
        while(xx > rectOctant3Clipped.x + rectOctant3Clipped.w || yy < rectOctant3Clipped.y)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx--;
                x++; yy++;
            }
        }

        //Bottom and left clip
        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx--;
                x++; yy++;
            }
        }
    }

    //Octant 4 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant4 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant4 == 2)
    {
        int xx = rectOctant4.x;
        int yy = rectOctant4.y + rectOctant4.h;
        int limitX = rectOctant4Clipped.x + rectOctant4Clipped.w;
        int limitY = rectOctant4Clipped.y;

        int diff = iRadius - 1;

        //Left and bottom clip
        while(xx < rectOctant4Clipped.x || yy > rectOctant4Clipped.y + rectOctant4Clipped.h)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy--;
                x++; xx++;
            }
        }

        //Bottom and left clip
        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy--;
                x++; xx++;
            }
        }
    }


    //Octant 5 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant5 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant5 == 2)
    {
        int xx = rectOctant5.x + rectOctant5.w;
        int yy = rectOctant5.y + rectOctant5.h;
        int limitX = rectOctant5Clipped.x;
        int limitY = rectOctant5Clipped.y;

        int diff = iRadius - 1;

        //Left and bottom clip
        while(xx > rectOctant5Clipped.x + rectOctant5Clipped.w || yy > rectOctant5Clipped.y + rectOctant5Clipped.h)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy--;
                x++; xx--;
            }
        }

        //Bottom and left clip
        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy--;
                x++; xx--;
            }
        }
    }


    //Octant 6 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant6 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant6 == 2)
    {
        int xx = rectOctant6.x;
        int yy = rectOctant6.y;
        int limitX = rectOctant6Clipped.x + rectOctant6Clipped.w;
        int limitY = rectOctant6Clipped.y + rectOctant6Clipped.h;

        int diff = iRadius - 1;

        //Left and bottom clip
        while(xx < rectOctant6Clipped.x || yy < rectOctant6Clipped.y)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx++;
                x++; yy++;
            }
        }

        //Bottom and left clip
        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx++;
                x++; yy++;
            }
        }
    }


    //Octant 7 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant7 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant7 == 2)
    {
        int xx = rectOctant7.x;
        int yy = rectOctant7.y + rectOctant7.h;
        int limitX = rectOctant7Clipped.x + rectOctant7Clipped.w;
        int limitY = rectOctant7Clipped.y;

        int diff = iRadius - 1;

        //Left and bottom clip
        while(xx < rectOctant7Clipped.x || yy > rectOctant7Clipped.y + rectOctant7Clipped.h)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx++;
                x++; yy--;
            }
        }

        //Bottom and left clip
        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; yy--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; xx++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; xx++;
                x++; yy--;
            }
        }
    }


    //Octant 8 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant8 == 1)
    {
        int diff = iRadius - 1;

        while(y >= x)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--;
            } else
            {
                diff += (2 * (y - x - 1));
                y--;
                x++;
            }
        }
    } else if(drawRectOctant8 == 2)
    {
        int xx = rectOctant8.x + rectOctant8.w;
        int yy = rectOctant8.y;
        int limitX = rectOctant8Clipped.x;
        int limitY = rectOctant8Clipped.y + rectOctant8Clipped.h;

        int diff = iRadius - 1;

        //Left and bottom clip
        while(xx > rectOctant8Clipped.x + rectOctant8Clipped.w || yy < rectOctant8Clipped.y)
        {
            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy++;
                x++; xx--;
            }
        }

        //Bottom and left clip
        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = FMath::Abs((float(diff - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
            }

            if(diff >= (2 * x))
            {
                diff -= (2 * x + 1);
                x++; xx--;
            } else if(diff < (2 * (iRadius - y)))
            {
                diff += (2 * y - 1);
                y--; yy++;
            } else
            {
                diff += (2 * (y - x - 1));
                y--; yy++;
                x++; xx--;
            }
        }
    }
}

void DrawArcBresenham(        FBlock&                   iBlock
                            , const FVec2I&             iCenter
                            , const int                 iRadius
                            , const int                 iStartDegree
                            , const int                 iEndDegree
                            , const FColor&             iColor
                            , const FRectI&             iClippingRect );

template< typename T >
static void DrawArcBresenhamAA(  FBlock&                   iBlock
                               , const FVec2I&             iCenter
                               , const int                 iRadius
                               , const int                 iStartDegree
                               , const int                 iEndDegree
                               , const FColor&             iColor
                               , const FRectI&             iClippingRect)
{
    if(iRadius == 0)
        return;

    //Clipping -----
    int x = 0;
    int y = iRadius; //We start from the top of the circle for the first octant
    int errMax = 2 * (4 * iRadius - 5);
    int errMin = 0;

    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;

    if(clippingRect.Area() == 0)
    {
        clippingRect = FRectI::FromXYWH(0,0,iBlock.Width() - 1,iBlock.Height() - 1);
    }

    int shift45 = int(std::sin(FMath::DegToRad(45)) * iRadius);
    FVec2I point0 = FVec2I(iCenter.x,iCenter.y - iRadius);
    FVec2I point45 = FVec2I(iCenter.x + shift45,iCenter.y - shift45);
    FVec2I point90 = FVec2I(iCenter.x + iRadius,iCenter.y);
    FVec2I point135 = FVec2I(iCenter.x + shift45,iCenter.y + shift45);
    FVec2I point180 = FVec2I(iCenter.x,iCenter.y + iRadius);
    FVec2I point225 = FVec2I(iCenter.x - shift45,iCenter.y + shift45);
    FVec2I point270 = FVec2I(iCenter.x - iRadius,iCenter.y);
    FVec2I point315 = FVec2I(iCenter.x - shift45,iCenter.y - shift45);

    FRectI rectOctant1 = FRectI(point0.x,point0.y,point45.x - point0.x,point45.y - point0.y);
    FRectI rectOctant2 = FRectI(point45.x,point45.y,point90.x - point45.x,point90.y - point45.y);
    FRectI rectOctant3 = FRectI(point135.x,point90.y,point90.x - point135.x,point135.y - point90.y);
    FRectI rectOctant4 = FRectI(point180.x,point135.y,point135.x - point180.x,point180.y - point135.y);
    FRectI rectOctant5 = FRectI(point225.x,point225.y,point180.x - point225.x,point180.y - point225.y);
    FRectI rectOctant6 = FRectI(point270.x,point270.y,point225.x - point270.x,point225.y - point270.y);
    FRectI rectOctant7 = FRectI(point270.x,point315.y,point315.x - point270.x,point270.y - point315.y);
    FRectI rectOctant8 = FRectI(point315.x,point0.y,point0.x - point315.x,point315.y - point0.y);

    FRectI rectOctant1Clipped = rectOctant1 & clippingRect;
    FRectI rectOctant2Clipped = rectOctant2 & clippingRect;
    FRectI rectOctant3Clipped = rectOctant3 & clippingRect;
    FRectI rectOctant4Clipped = rectOctant4 & clippingRect;
    FRectI rectOctant5Clipped = rectOctant5 & clippingRect;
    FRectI rectOctant6Clipped = rectOctant6 & clippingRect;
    FRectI rectOctant7Clipped = rectOctant7 & clippingRect;
    FRectI rectOctant8Clipped = rectOctant8 & clippingRect;

    // 0 : Don't draw the octant. 1: Draw fully the octant. 2: Partial draw of the octant (complex case)
    int drawRectOctant1 = rectOctant1Clipped == rectOctant1 ? 1 : rectOctant1Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant2 = rectOctant2Clipped == rectOctant2 ? 1 : rectOctant2Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant3 = rectOctant3Clipped == rectOctant3 ? 1 : rectOctant3Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant4 = rectOctant4Clipped == rectOctant4 ? 1 : rectOctant4Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant5 = rectOctant5Clipped == rectOctant5 ? 1 : rectOctant5Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant6 = rectOctant6Clipped == rectOctant6 ? 1 : rectOctant6Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant7 = rectOctant7Clipped == rectOctant7 ? 1 : rectOctant7Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant8 = rectOctant8Clipped == rectOctant8 ? 1 : rectOctant8Clipped.Area() != 0 ? 2 : 0;

    //Drawing ------
    FColor val = iColor;

    T maxAlpha = val.AlphaT<T>();

    int sizeAngleToDraw = (iEndDegree - iStartDegree + 360) % 360; //Positive modulo
    int currentAngle = iStartDegree;

    int octantsToDraw[8] ={0,0,0,0,0,0,0,0}; // 0: Don't draw the octant. 1: draw fully the octant. 2: draw part of the octant
    int directionToDraw[8][2] ={{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0},{0,0}}; // 1 clockwise, -1 anti-clockwise, 2 both 0 irrelevant, second entry is angle to draw on octant

    if(sizeAngleToDraw < 45)
    {
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = 2;
        directionToDraw[currentAngle / 45][1] = sizeAngleToDraw;
    } else if(currentAngle % 45 == 0)
        octantsToDraw[currentAngle / 45] = 1;
    else
    {
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = -1;
        directionToDraw[currentAngle / 45][1] = 45 - (currentAngle % 45);
    }

    sizeAngleToDraw -= (45 - (iStartDegree % 45));

    while(sizeAngleToDraw >= 45)
    {
        currentAngle = (currentAngle + 45) % 360;
        octantsToDraw[currentAngle / 45] = 1;
        sizeAngleToDraw -= 45;
    }

    if(sizeAngleToDraw > 0)
    {
        currentAngle = (currentAngle + 45) % 360;
        octantsToDraw[currentAngle / 45] = 2;
        directionToDraw[currentAngle / 45][0] = 1;
        directionToDraw[currentAngle / 45][1] = sizeAngleToDraw;
    }

    //0� is on top and we turn clockwise
    //Octant 1 ------
    if(drawRectOctant1 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant1 == 2)
    {
        int xx = rectOctant1.x;
        int yy = rectOctant1.y;
        int limitX = rectOctant1Clipped.w + rectOctant1Clipped.x;
        int limitY = rectOctant1Clipped.h + rectOctant1Clipped.y;

        int diff = 5 - 4 * iRadius;

        while(xx < rectOctant1Clipped.x || yy < rectOctant1Clipped.y)
        {
            if(diff > 0)
            {
                y--; yy++;
                diff = diff - 8 * y;
            }
            x++; xx++;
            diff = diff + 8 * x + 4;
        }

        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;yy++;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[0] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val); // 0� to 45�

            // Complex cases
            if(octantsToDraw[0] == 2)
            {
                if(directionToDraw[0][0] == 1 && currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[0][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
                else if(directionToDraw[0][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y - y - 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 2 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant2 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant2 == 2)
    {
        int xx = rectOctant2.x + rectOctant2.w;
        int yy = rectOctant2.y + rectOctant2.h;
        int limitX = rectOctant2Clipped.x;
        int limitY = rectOctant2Clipped.y;

        int diff = 5 - 4 * iRadius;

        while(xx > rectOctant2Clipped.x + rectOctant2Clipped.w || yy > rectOctant2Clipped.y + rectOctant2Clipped.h)
        {
            if(diff > 0)
            {
                y--; xx--;
                diff = diff - 8 * y;
            }
            x++; yy--;
            diff = diff + 8 * x + 4;
        }

        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;xx--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[1] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val); // 90� to 45�

            if(octantsToDraw[1] == 2)
            {
                if(directionToDraw[1][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == -1 && currentAngleOnFirstOctant < directionToDraw[1][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
                else if(directionToDraw[1][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y - x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy--;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 3 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant3 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant3 == 2)
    {
        int xx = rectOctant3.x + rectOctant3.w;
        int yy = rectOctant3.y;
        int limitX = rectOctant3Clipped.x;
        int limitY = rectOctant3Clipped.y + rectOctant3Clipped.h;

        int diff = 5 - 4 * iRadius;

        //Right and top clip
        while(xx > rectOctant3Clipped.x + rectOctant3Clipped.w || yy < rectOctant3Clipped.y)
        {
            if(diff > 0)
            {
                y--; xx--;
                diff = diff - 8 * y;
            }
            x++; yy++;
            diff = diff + 8 * x + 4;
        }

        //Bottom and left clip
        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;xx--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[2] == 1) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val); // 90� to 135�

            if(octantsToDraw[2] == 2)
            {
                if(directionToDraw[2][0] == 1 && currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[2][1]) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
                else if(directionToDraw[2][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + y + 1,iCenter.y + x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 4 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant4 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant4 == 2)
    {
        int xx = rectOctant4.x;
        int yy = rectOctant4.y + rectOctant4.h;
        int limitX = rectOctant4Clipped.x + rectOctant4Clipped.w;
        int limitY = rectOctant4Clipped.y;

        int diff = 5 - 4 * iRadius;

        //Left and bottom clip
        while(xx < rectOctant4Clipped.x || yy > rectOctant4Clipped.y + rectOctant4Clipped.h)
        {
            if(diff > 0)
            {
                y--; yy--;
                diff = diff - 8 * y;
            }
            x++; xx++;
            diff = diff + 8 * x + 4;
        }

        //Bottom and left clip
        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;yy--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[3] == 1) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val); // 180� to 135�

            if(octantsToDraw[3] == 2)
            {
                if(directionToDraw[3][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == -1 && currentAngleOnFirstOctant < directionToDraw[3][1]) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
                else if(directionToDraw[3][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x + x,iCenter.y + y + 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 5 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant5 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant5 == 2)
    {
        int xx = rectOctant5.x + rectOctant5.w;
        int yy = rectOctant5.y + rectOctant5.h;
        int limitX = rectOctant5Clipped.x;
        int limitY = rectOctant5Clipped.y;

        int diff = 5 - 4 * iRadius;

        //Left and bottom clip
        while(xx > rectOctant5Clipped.x + rectOctant5Clipped.w || yy > rectOctant5Clipped.y + rectOctant5Clipped.h)
        {
            if(diff > 0)
            {
                y--; yy--;
                diff = diff - 8 * y;
            }
            x++; xx--;
            diff = diff + 8 * x + 4;
        }

        //Bottom and left clip
        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;yy--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[4] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val); // 180� to 225�

            if(octantsToDraw[4] == 2)
            {
                if(directionToDraw[4][0] == 1 && currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[4][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
                else if(directionToDraw[4][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y + y + 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx--;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 6 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant6 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant6 == 2)
    {
        int xx = rectOctant6.x;
        int yy = rectOctant6.y;
        int limitX = rectOctant6Clipped.x + rectOctant6Clipped.w;
        int limitY = rectOctant6Clipped.y + rectOctant6Clipped.h;

        int diff = 5 - 4 * iRadius;

        //Left and bottom clip
        while(xx < rectOctant6Clipped.x || yy < rectOctant6Clipped.y)
        {
            if(diff > 0)
            {
                y--; xx++;
                diff = diff - 8 * y;
            }
            x++; yy++;
            diff = diff + 8 * x + 4;
        }

        //Bottom and left clip
        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;xx++;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y + x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[5] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);  // 270� to 225�

            if(octantsToDraw[5] == 2)
            {
                if(directionToDraw[5][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == -1 && currentAngleOnFirstOctant < directionToDraw[5][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
                else if(directionToDraw[5][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y + x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy++;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 7 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant7 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant7 == 2)
    {
        int xx = rectOctant7.x;
        int yy = rectOctant7.y + rectOctant7.h;
        int limitX = rectOctant7Clipped.x + rectOctant7Clipped.w;
        int limitY = rectOctant7Clipped.y;

        int diff = 5 - 4 * iRadius;

        //Left and bottom clip
        while(xx < rectOctant7Clipped.x || yy > rectOctant7Clipped.y + rectOctant7Clipped.h)
        {
            if(diff > 0)
            {
                y--; xx++;
                diff = diff - 8 * y;
            }
            x++; yy--;
            diff = diff + 8 * x + 4;
        }

        //Bottom and left clip
        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;xx++;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y,iCenter.y - x,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[6] == 1) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val); // 270� to 315�

            if(octantsToDraw[6] == 2)
            {
                if(directionToDraw[6][0] == 1 && currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == -1 && 45 - currentAngleOnFirstOctant < directionToDraw[6][1]) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
                else if(directionToDraw[6][0] == 2 && currentAngleOnFirstOctant > (iStartDegree % 45) && currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - y - 1,iCenter.y - x,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; yy--;
            diff = diff + 8 * x + 4;
        }
    }

    //Octant 8 ------
    x = 0;
    y = iRadius;
    if(drawRectOctant8 == 1)
    {
        int diff = 5 - 4 * iRadius;

        while(y >= x)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++;
            diff = diff + 8 * x + 4;
        }
    } else if(drawRectOctant8 == 2)
    {
        int xx = rectOctant8.x + rectOctant8.w;
        int yy = rectOctant8.y;
        int limitX = rectOctant8Clipped.x;
        int limitY = rectOctant8Clipped.y + rectOctant8Clipped.h;

        int diff = 5 - 4 * iRadius;

        //Left and bottom clip
        while(xx > rectOctant8Clipped.x + rectOctant8Clipped.w || yy < rectOctant8Clipped.y)
        {
            if(diff > 0)
            {
                y--; yy++;
                diff = diff - 8 * y;
            }
            x++; xx--;
            diff = diff + 8 * x + 4;
        }

        //Bottom and left clip
        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = (float(diff - errMax) / float(errMin - errMax)) - 1; //Interpolation of slopedifferential between errMin and errMax
            if(alphaTop < 0.f)
            {
                alphaTop = 1 - FMath::Abs(alphaTop);
                y--;yy++;
            }
            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            double currentAngleOnFirstOctant = -::ULIS::FMath::RadToDeg(std::acos(double(x) / double(iRadius)) - (FMath::kPId / 2));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);
            }

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            if(octantsToDraw[7] == 1) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val); // 0� to 315�

            if(octantsToDraw[7] == 2)
            {
                if(directionToDraw[7][0] == 1 && 45 - currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == -1 && currentAngleOnFirstOctant < directionToDraw[7][1]) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
                else if(directionToDraw[7][0] == 2 && 45 - currentAngleOnFirstOctant > (iStartDegree % 45) && 45 - currentAngleOnFirstOctant < (iEndDegree % 45)) iBlock.SetPixel(iCenter.x - x,iCenter.y - y - 1,val);
            }

            if(diff > 0)
            {
                diff = diff - 8 * y;
            }
            x++; xx--;
            diff = diff + 8 * x + 4;
        }
    }
}


void DrawEllipse(         FBlock&                  iBlock
                        , const FVec2I&            iCenter
                        , const int                iA
                        , const int                iB
                        , const FColor&            iColor
                        , const bool               iFilled
                        , const FRectI&            iClippingRect );

template< typename T >
static void DrawEllipseAA( FBlock&                  iBlock
                         , const FVec2I&            iCenter
                         , const int                iA
                         , const int                iB
                         , const FColor&            iColor
                         , const bool               iFilled
                         , const FRectI&            iClippingRect )
{
    FRectI clippingRect = iClippingRect;
    clippingRect.w--;
    clippingRect.h--;

    if(clippingRect.Area() == 0)
    {
        clippingRect = FRectI::FromXYWH(0,0,iBlock.Width() - 1,iBlock.Height() - 1);
    }

    float angleTo45 = std::atan(- float(iB) / float(iA));
    int shift45x = int(FMath::Abs(std::cos(angleTo45) * iA));
    int shift45y = int(FMath::Abs(std::sin(angleTo45) * iB));

    FVec2I point0 = FVec2I(iCenter.x,iCenter.y - iB);
    FVec2I point45 = FVec2I(iCenter.x + shift45x,iCenter.y - shift45y);
    FVec2I point90 = FVec2I(iCenter.x + iA,iCenter.y);
    FVec2I point135 = FVec2I(iCenter.x + shift45x,iCenter.y + shift45y);
    FVec2I point180 = FVec2I(iCenter.x,iCenter.y + iB);
    FVec2I point225 = FVec2I(iCenter.x - shift45x,iCenter.y + shift45y);
    FVec2I point270 = FVec2I(iCenter.x - iA,iCenter.y);
    FVec2I point315 = FVec2I(iCenter.x - shift45x,iCenter.y - shift45y);

    FRectI rectOctant1 = FRectI(point0.x,point0.y - 1,point45.x - point0.x,point45.y - point0.y);
    FRectI rectOctant2 = FRectI(point45.x + 1,point45.y,point90.x - point45.x,point90.y - point45.y);
    FRectI rectOctant3 = FRectI(point135.x + 1,point90.y,point90.x - point135.x,point135.y - point90.y);
    FRectI rectOctant4 = FRectI(point180.x,point135.y + 1,point135.x - point180.x,point180.y - point135.y);
    FRectI rectOctant5 = FRectI(point225.x,point225.y + 1,point180.x - point225.x,point180.y - point225.y);
    FRectI rectOctant6 = FRectI(point270.x - 1,point270.y,point225.x - point270.x,point225.y - point270.y);
    FRectI rectOctant7 = FRectI(point270.x - 1,point315.y,point315.x - point270.x,point270.y - point315.y);
    FRectI rectOctant8 = FRectI(point315.x,point0.y - 1,point0.x - point315.x,point315.y - point0.y);

    FRectI rectOctant1Clipped = rectOctant1 & clippingRect;
    FRectI rectOctant2Clipped = rectOctant2 & clippingRect;
    FRectI rectOctant3Clipped = rectOctant3 & clippingRect;
    FRectI rectOctant4Clipped = rectOctant4 & clippingRect;
    FRectI rectOctant5Clipped = rectOctant5 & clippingRect;
    FRectI rectOctant6Clipped = rectOctant6 & clippingRect;
    FRectI rectOctant7Clipped = rectOctant7 & clippingRect;
    FRectI rectOctant8Clipped = rectOctant8 & clippingRect;

    // 0 : Don't draw the octant. 1: Draw fully the octant. 2: Partial draw of the octant (complex case)
    int drawRectOctant1 = rectOctant1Clipped == rectOctant1 ? 1 : rectOctant1Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant2 = rectOctant2Clipped == rectOctant2 ? 1 : rectOctant2Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant3 = rectOctant3Clipped == rectOctant3 ? 1 : rectOctant3Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant4 = rectOctant4Clipped == rectOctant4 ? 1 : rectOctant4Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant5 = rectOctant5Clipped == rectOctant5 ? 1 : rectOctant5Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant6 = rectOctant6Clipped == rectOctant6 ? 1 : rectOctant6Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant7 = rectOctant7Clipped == rectOctant7 ? 1 : rectOctant7Clipped.Area() != 0 ? 2 : 0;
    int drawRectOctant8 = rectOctant8Clipped == rectOctant8 ? 1 : rectOctant8Clipped.Area() != 0 ? 2 : 0;

    //Drawing ------
    FColor val = iColor;

    T maxAlpha = val.AlphaT<T>();

    int a2 = iA * iA;
    int b2 = iB * iB;
    int fa2 = 4 * a2;
    int fb2 = 4 * b2;
    int x = 0;
    int y = iB;
    int sigma = 2*b2+a2*(1-2*iB);

    int errMax = 0;
    int errMin =  2 * (2*b2+a2*(1-2*iB));

    //0� is on top and we turn clockwise
    //Octant 1 ------
    if(drawRectOctant1 == 1)
    {
        while(b2 * x <= a2 * y)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0 to 45 degrees

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y - step,val); // 0 to 45 degrees

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--;
            }
            sigma += b2*(4 * x + 6);
            x++;
        }
    } else if(drawRectOctant1 == 2)
    {
        int xx = rectOctant1.x;
        int yy = rectOctant1.y;
        int limitX = rectOctant1Clipped.w + rectOctant1Clipped.x;
        int limitY = rectOctant1Clipped.h + rectOctant1Clipped.y;

        while(xx < rectOctant1Clipped.x || yy < rectOctant1Clipped.y)
        {
            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy++;
            }
            sigma += b2*(4 * x + 6);
            x++; xx++;
        }

        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val); // 0 to 45 degrees

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y - step,val); // 0 to 45 degrees

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy++;
            }
            sigma += b2*(4 * x + 6);
            x++; xx++;
        }
    }

    //Octant 2 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(1-2*iA);
    errMin =  2 * (2*a2+b2*(1-2*iA));
    if(drawRectOctant2 == 1)
    {
        while(a2 * y <= b2 * x)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x + step,iCenter.y - y,val); // 90 to 45 degrees

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--;
            }
            sigma += a2*(4 * y + 6);
            y++;
        }
    } else if(drawRectOctant2 == 2)
    {
        int xx = rectOctant2.x + rectOctant2.w;
        int yy = rectOctant2.y + rectOctant2.h;
        int limitX = rectOctant2Clipped.x;
        int limitY = rectOctant2Clipped.y;

        while(xx > rectOctant2Clipped.x + rectOctant2Clipped.w || yy > rectOctant2Clipped.y + rectOctant2Clipped.h)
        {
            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx--;
            }
            sigma += a2*(4 * y + 6);
            y++; yy--;
        }

        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y - y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x + step,iCenter.y - y,val); // 90 to 45 degrees

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx--;
            }
            sigma += a2*(4 * y + 6);
            y++; yy--;
        }
    }


    //Octant 3 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(1-2*iA);
    errMin =  2 * (2*a2+b2*(1-2*iA));
    if(drawRectOctant3 == 1)
    {
        while(a2 * y <= b2 * x)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x + step,iCenter.y + y,val);

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--;
            }
            sigma += a2*(4 * y + 6);
            y++;
        }
    } else if(drawRectOctant3 == 2)
    {
        int xx = rectOctant3.x + rectOctant3.w;
        int yy = rectOctant3.y;
        int limitX = rectOctant3Clipped.x;
        int limitY = rectOctant3Clipped.y + rectOctant3Clipped.h;

        while(xx > rectOctant3Clipped.x + rectOctant3Clipped.w || yy < rectOctant3Clipped.y)
        {
            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx--;
            }
            sigma += a2*(4 * y + 6);
            y++; yy++;
        }

        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x + step,iCenter.y + y,val);

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx--;
            }
            sigma += a2*(4 * y + 6);
            y++; yy++;
        }
    }


    //Octant 4 ------
    x = 0;
    y = iB;
    sigma = 2*b2+a2*(1-2*iB);
    errMin =  2 * (2*b2+a2*(1-2*iB));
    if(drawRectOctant4 == 1)
    {
        while(b2 * x <= a2 * y)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y + step,val); // 180 to 135 degrees

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--;
            }
            sigma += b2*(4 * x + 6);
            x++;
        }
    } else if(drawRectOctant4 == 2)
    {
        int xx = rectOctant4.x;
        int yy = rectOctant4.y + rectOctant4.h;
        int limitX = rectOctant4Clipped.x + rectOctant4Clipped.w;
        int limitY = rectOctant4Clipped.y;

        while(xx < rectOctant4Clipped.x || yy > rectOctant4Clipped.y + rectOctant4Clipped.h)
        {
            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy--;
            }
            sigma += b2*(4 * x + 6);
            x++; xx++;
        }

        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x + x,iCenter.y + y + step,val); // 180 to 135 degrees

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy--;
            }
            sigma += b2*(4 * x + 6);
            x++; xx++;
        }
    }

    //Octant 5 ------
    x = 0;
    y = iB;
    sigma = 2*b2+a2*(1-2*iB);
    errMin =  2 * (2*b2+a2*(1-2*iB));
    if(drawRectOctant5 == 1)
    {
        while(b2 * x <= a2 * y)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y + step,val);

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--;
            }
            sigma += b2*(4 * x + 6);
            x++;
        }
    } else if(drawRectOctant5 == 2)
    {
        int xx = rectOctant5.x + rectOctant5.w;
        int yy = rectOctant5.y + rectOctant5.h;
        int limitX = rectOctant5Clipped.x;
        int limitY = rectOctant5Clipped.y;

        while(xx > rectOctant5Clipped.x + rectOctant5Clipped.w || yy > rectOctant5Clipped.y + rectOctant5Clipped.h)
        {
            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy--;
            }
            sigma += b2*(4 * x + 6);
            x++; xx--;
        }

        while(xx >= limitX && yy >= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y + step,val);

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy--;
            }
            sigma += b2*(4 * x + 6);
            x++; xx--;
        }
    }

    //Octant 6 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(1-2*iA);
    errMin =  2 * (2*a2+b2*(1-2*iA));
    if(drawRectOctant6 == 1)
    {
        while(a2 * y <= b2 * x)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x - step,iCenter.y + y,val);

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--;
            }
            sigma += a2*(4 * y + 6);
            y++;
        }
    } else if(drawRectOctant6 == 2)
    {
        int xx = rectOctant6.x;
        int yy = rectOctant6.y;
        int limitX = rectOctant6Clipped.x + rectOctant6Clipped.w;
        int limitY = rectOctant6Clipped.y + rectOctant6Clipped.h;

        while(xx < rectOctant6Clipped.x || yy < rectOctant6Clipped.y)
        {
            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx++;
            }
            sigma += a2*(4 * y + 6);
            y++; yy++;
        }

        while(xx <= limitX && yy <= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y + y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x - step,iCenter.y + y,val);

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx++;
            }
            sigma += a2*(4 * y + 6);
            y++; yy++;
        }
    }

    //Octant 7 ------
    x = iA;
    y = 0;
    sigma = 2*a2+b2*(1-2*iA);
    errMin =  2 * (2*a2+b2*(1-2*iA));
    if(drawRectOctant7 == 1)
    {
        while(a2 * y <= b2 * x)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x - step,iCenter.y - y,val);

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--;
            }
            sigma += a2*(4 * y + 6);
            y++;
        }
    } else if(drawRectOctant7 == 2)
    {
        int xx = rectOctant7.x;
        int yy = rectOctant7.y + rectOctant7.h;
        int limitX = rectOctant7Clipped.x + rectOctant7Clipped.w;
        int limitY = rectOctant7Clipped.y;

        while(xx < rectOctant7Clipped.x || yy > rectOctant7Clipped.y + rectOctant7Clipped.h)
        {
            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx++;
            }
            sigma += a2*(4 * y + 6);
            y++; yy--;
        }

        while(xx <= limitX && yy >= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            int step = sigma < 0 ? 1 : -1;

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x - step,iCenter.y - y,val);

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--; xx++;
            }
            sigma += a2*(4 * y + 6);
            y++; yy--;
        }
    }

    //Octant 8 ------
    x = 0;
    y = iB;
    sigma = 2*b2+a2*(1-2*iB);
    errMin =  2 * (2*b2+a2*(1-2*iB));
    if(drawRectOctant8 == 1)
    {
        while(b2 * x <= a2 * y)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y - step,val);

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--;
            }
            sigma += b2*(4 * x + 6);
            x++;
        }
    } else if(drawRectOctant8 == 2)
    {
        int xx = rectOctant8.x + rectOctant8.w;
        int yy = rectOctant8.y;
        int limitX = rectOctant8Clipped.x;
        int limitY = rectOctant8Clipped.y + rectOctant8Clipped.h;

        while(xx > rectOctant8Clipped.x + rectOctant8Clipped.w || yy < rectOctant8Clipped.y)
        {
            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy++;
            }
            sigma += b2*(4 * x + 6);
            x++; xx--;
        }

        while(xx >= limitX && yy <= limitY)
        {
            float alphaTop = 1 - FMath::Abs((float(sigma - errMax) / float(errMin - errMax))); //Interpolation of slopedifferential between errMin and errMax

            val.SetAlphaT<T>(T(maxAlpha * alphaTop));

            int step = sigma <= 0 ? 1 : -1;

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y,val);

            val.SetAlphaT<T>(T(maxAlpha * (1 - alphaTop)));

            iBlock.SetPixel(iCenter.x - x,iCenter.y - y - step,val);

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--; yy++;
            }
            sigma += b2*(4 * x + 6);
            x++; xx--;
        }
    }

    //Fill

    if(iFilled)
    {
        x = 0;
        y = iB;
        sigma = 2*b2+a2*(1-2*iB);

        while(b2 * x <= a2 * y)
        {
            int step = sigma <= 0 ? 1 : -1;

            if(step == 1)
            {
                DrawLine(iBlock,FVec2I(iCenter.x + x,iCenter.y - y),FVec2I(iCenter.x + x,iCenter.y + y),iColor,iClippingRect);
                DrawLine(iBlock,FVec2I(iCenter.x - x,iCenter.y - y),FVec2I(iCenter.x - x,iCenter.y + y),iColor,iClippingRect);
            } else //step = -1, we draw the aliasing on the inside of the ellipse, so we colorize one pixel less
            {
                DrawLine(iBlock,FVec2I(iCenter.x + x,iCenter.y - y + 1),FVec2I(iCenter.x + x,iCenter.y + y - 1),iColor,iClippingRect);
                DrawLine(iBlock,FVec2I(iCenter.x - x,iCenter.y - y + 1),FVec2I(iCenter.x - x,iCenter.y + y - 1),iColor,iClippingRect);
            }

            if(sigma >= 0)
            {
                sigma += fa2 * (1 - y);
                y--;
            }
            sigma += b2*(4 * x + 6);
            x++;
        }

        x = iA;
        y = 0;
        sigma = 2*a2+b2*(1-2*iA);

        while(a2 * y <= b2 * x)
        {
            int step = sigma < 0 ? 1 : -1;

            if(step == 1)
            {
                DrawLine(iBlock,FVec2I(iCenter.x + x,iCenter.y - y),FVec2I(iCenter.x + x,iCenter.y + y),iColor,iClippingRect);
                DrawLine(iBlock,FVec2I(iCenter.x - x,iCenter.y - y),FVec2I(iCenter.x - x,iCenter.y + y),iColor,iClippingRect);
            } else //step = -1, we draw the aliasing on the inside of the ellipse, so we colorize one pixel less
            {
                DrawLine(iBlock,FVec2I(iCenter.x + x,iCenter.y - y + 1),FVec2I(iCenter.x + x,iCenter.y + y - 1),iColor,iClippingRect);
                DrawLine(iBlock,FVec2I(iCenter.x - x,iCenter.y - y + 1),FVec2I(iCenter.x - x,iCenter.y + y - 1),iColor,iClippingRect);
            }

            if(sigma >= 0)
            {
                sigma += fb2 * (1 - x);
                x--;
            }
            sigma += a2*(4 * y + 6);
            y++;
        }
    }
}


void DrawRotatedEllipse(        FBlock&                  iBlock
                              , const FVec2I&            iCenter
                              , const int                iA
                              , const int                iB
                              , const int                iRotationDegrees
                              , const FColor&            iColor
                              , const bool               iFilled
                              , const FRectI&            iClippingRect );

template< typename T >
static void DrawRotatedEllipseAA( FBlock&                  iBlock
                                , const FVec2I&            iCenter
                                , const int                iA
                                , const int                iB
                                , const int                iRotationDegrees
                                , const FColor&            iColor
                                , const bool               iFilled
                                , const FRectI&            iClippingRect )
{
    if(iRotationDegrees % 180 == 0)
    {
        DrawEllipseAA<T>(iBlock,iCenter,iA,iB,iColor,iFilled,iClippingRect); //Don't bother to use the rotated ellipse algorithm if the ellipse is not rotated
        return;
    }
    if(iRotationDegrees % 90 == 0)
    {
        DrawEllipseAA<T>(iBlock,iCenter,iB,iA,iColor,iFilled,iClippingRect); //Don't bother to use the rotated ellipse algorithm if the ellipse is not rotated
        return;
    }

    int a = 0;
    int b = 0;
    int rotation = 0;
    int rotationDegreesBase = iRotationDegrees;
    if( iA > iB )
    {
        a = iA;
        b = iB;
        rotation = ((iRotationDegrees % 180) + 180) % 180; //Positive modulo
    }
    else
    {
        rotationDegreesBase+=90;
        a = iB;
        b = iA;
        rotation = ((rotationDegreesBase % 180) + 180 ) % 180; //Positive modulo
    }

    float dx = float(a*a);
    float dy = float(b*b);
    float s = float(std::sin(FMath::DegToRad(rotation)));
    float dz = (dx - dy) * s;
    dx = std::sqrt(dx - dz * s);
    dy = std::sqrt(dy + dz * s);
    a = int(dx + 0.5);
    b = int(dy + 0.5);
    dz = dz * a * b / (dx * dy);

    int x0 = iCenter.x - a;
    int y0 = iCenter.y - b;
    int x1 = iCenter.x + a;
    int y1 = iCenter.y + b;
    dz = float(4 * dz * std::cos(FMath::DegToRad(rotation)));


    dx = float(x1 - x0);
    dy = float(y1 - y0);
    float w = dx * dy;
    if(w != 0.0)
        w = (w - dz) / (w + w);

    if(w > 1 || w < 0)
        return;

    dx = float(std::floor(dx * w + 0.5));
    dy = float(std::floor(dy * w + 0.5));



    if(!iFilled)
    {
        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x0,int(y0 + dy),x0,y0,int(x0 + dx),y0,1 - w,iColor,iClippingRect); //top left
        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x0,int(y0 + dy),x0,y1,int(x1 - dx),y1,w,iColor,iClippingRect); //bottom left
        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x1,int(y1 - dy),x1,y1,int(x1 - dx),y1,1 - w,iColor,iClippingRect); //bottom right
        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x1,int(y1 - dy),x1,y0,int(x0 + dx),y0,w,iColor,iClippingRect); //top right
    } else //Filled Ellipse
    {
        //               x  y
        std::map< int,std::vector< int > > storagePoints; // storagePoints[x][0]  We have n points for each x on the ellipse: p1(x, y0), p2(x, y1) ... pn(x, yn)
                                                            //                 [1]
                                                            //                 [...]
                                                            //                 [n]
        storagePoints[0].push_back(iCenter.x);           // In order to only pass storagePoints in parameter to InternalDrawQuadRationalBezierSeg
        storagePoints[0].push_back(iCenter.y);           // we store the center (useful in this function) at index 0 (we'll know it's there)

        std::map< int,std::vector< int > > pointsForFill; // same structure, but with only the top and bottom points to draw a line on each x to fill the ellipse


        // Lambda ---
        auto fillPointsForFill = [&](bool isTop,int shift)
        {
            //Removing the coordinates of the center we stored
            storagePoints[0].erase(storagePoints[0].begin());
            storagePoints[0].erase(storagePoints[0].begin());

            for(std::map< int,std::vector< int > >::iterator it=storagePoints.begin(); it!=storagePoints.end(); ++it)
            {
                if(it->second.size() > 0)
                {
                    int extremum = it->second[0];
                    for(int i = 1; i < it->second.size() ; i++)
                    {
                        if((isTop && shift == 0) || (shift == 1 && !isTop)) //We get the minimum of it->second
                        {
                            if(extremum > it->second[i])
                                extremum = it->second[i];
                        } else //We get the maximum of it->second
                        {
                            if(extremum < it->second[i])
                                extremum = it->second[i];
                        }
                    }
                    pointsForFill[it->first].push_back(extremum + shift * (isTop ? 1 : -1));
                }
            }

            storagePoints.clear();
            storagePoints[0].push_back(iCenter.x);
            storagePoints[0].push_back(iCenter.y);
        };
        //Lambda end ----


        // Depending of the angle of the ellipse, we either get the outer antialiased pixels or the inner antialiased pixels of the ellipse. If we get the outer pixels, we need to draw from the less outer one on top to the less outer one on bottom, and shift them one pixel inwards so we don't draw on outer pixels at all
        // If we get the inner pixels, we draw from the less inner one on top to the less inner one on bottom, no need to shift the pixels, we're already inside the ellipse, so we can fill from there.

        int shift = ((((rotationDegreesBase + 45) % 180) + 180) % 180) < 90 ? 0 : 1;

        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x0,int(y0 + dy),x0,y0,int(x0 + dx),y0,1 - w,iColor,iClippingRect,&storagePoints); //top left
        fillPointsForFill(true,shift);

        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x0,int(y0 + dy),x0,y1,int(x1 - dx),y1,w,iColor,iClippingRect,&storagePoints); //bottom left
        fillPointsForFill(false,shift);

        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x1,int(y1 - dy),x1,y1,int(x1 - dx),y1,1 - w,iColor,iClippingRect,&storagePoints); //bottom right
        fillPointsForFill(false,shift);

        InternalDrawQuadRationalBezierSegAA<T>(iBlock,x1,int(y1 - dy),x1,y0,int(x0 + dx),y0,w,iColor,iClippingRect,&storagePoints); //top right
        fillPointsForFill(true,shift);

        for(std::map< int,std::vector< int > >::iterator it=pointsForFill.begin(); it!=pointsForFill.end(); ++it)
        {
            if(it->second.size() == 2)
            {
                DrawLine(iBlock,FVec2I(iCenter.x + it->first,iCenter.y + it->second[0]),FVec2I(iCenter.x + it->first,iCenter.y + it->second[1]),iColor,iClippingRect);
            }
            if(it->second.size() > 2) // where we draw more than 2 pixels for a said y (it happens at the junctions between beziers)
            {
                int minY = it->second[0];
                int maxY = it->second[0];
                for(int i = 1; i < it->second.size(); i++)
                {
                    if(minY > it->second[i])
                        minY = it->second[i];

                    if(maxY < it->second[i])
                        maxY = it->second[i];
                }
                DrawLine(iBlock,FVec2I(iCenter.x + it->first,iCenter.y + minY),FVec2I(iCenter.x + it->first,iCenter.y + maxY),iColor,iClippingRect);
            }
        }
    }
}


void DrawRectangle(        FBlock&                        iBlock
                         , const FVec2I&                  iTopLeft
                         , const FVec2I&                  iBottomRight
                         , const FColor&                  iColor
                         //, const bool                     iFilled // Done in invocation
                         , const FRectI&                  iClippingRect );


void DrawPolygon(        FBlock&                      iBlock
                       , const std::vector< FVec2I >& iPoints
                       , const FColor&                iColor
                       , const bool                   iFilled
                       , const FRectI&                iClippingRect );


template< typename T >
static void DrawPolygonAA( FBlock&                      iBlock
                         , const std::vector< FVec2I >& iPoints
                         , const FColor&                iColor
                         , const bool                   iFilled
                         , const FRectI&                iClippingRect )
{
    if(iPoints.size() < 3)
        return;

    int j = int(iPoints.size() - 1);
    for(int i = 0; i < iPoints.size(); i++)  //TODO ? Drawing can be cleaner by blending the lines together instead of drawing them one by one
    {
        DrawLineAA<T>(iBlock,iPoints.at(i),iPoints.at(j),iColor,iClippingRect);
        j = i;
    }

    if(iFilled)
    {
        int maxX = 0;
        int maxY = 0;
        int minX = INT_MAX;
        int minY = INT_MAX;

        //Initialization of useful variables
        for(int i = 0; i < iPoints.size(); i++)
        {
            if(maxX < iPoints[i].x)
                maxX = iPoints[i].x;
            if(maxY < iPoints[i].y)
                maxY = iPoints[i].y;
            if(minX > iPoints[i].x)
                minX = iPoints[i].x;
            if(minY > iPoints[i].y)
                minY = iPoints[i].y;
        }
        minX++;
        maxX--;
        minY++;
        maxY--;

        //We go through the polygon by scanning it top to bottom
        for(int y = minY; y <= maxY; y++)
        {
            std::vector< int > nodesX;
            int j = int(iPoints.size() - 1);

            for(int i = 0; i < iPoints.size(); i++)
            {
                if((iPoints[i].y < y && iPoints[j].y >= y) || (iPoints[j].y < y && iPoints[i].y >= y))
                {
                    nodesX.push_back(int(iPoints[i].x  + double(y - iPoints[i].y) / double(iPoints[j].y - iPoints[i].y) * (iPoints[j].x - iPoints[i].x)));
                }
                j = i;
            }

            //Sorting the nodes on X
            int i = 0;
            int size = int(nodesX.size() - 1);
            while(i < size)
            {
                if(nodesX[i] > nodesX[i+1])
                {
                    int temp = nodesX[i];
                    nodesX[i]=nodesX[i+1];
                    nodesX[i+1] = temp;
                    if(i > 0)
                        i--;
                } else
                {
                    i++;
                }
            }

            //Filling the polygon on line y
            for(i = 0; i < nodesX.size(); i+= 2)
            {
                if(nodesX[i] > maxX) break;
                if(nodesX[i+1] > minX)
                {
                    if(nodesX[i] < minX)
                        nodesX[i] = minX;
                    if(nodesX[i+1] > maxX)
                        nodesX[i+1] = maxX;

                    DrawLine(iBlock,FVec2I(nodesX[i] + 1,y),FVec2I(nodesX[i+1],y),iColor,iClippingRect);
                }
            }
        }
    }
}


void DrawQuadraticBezier(        FBlock&                         iBlock
                               , const FVec2I&                   iCtrlPt0
                               , const FVec2I&                   iCtrlPt1
                               , const FVec2I&                   iCtrlPt2
                               , const float                     iWeight
                               , const FColor&                   iColor
                               , const FRectI&                   iClippingRect );



template< typename T >
static void DrawQuadraticBezierAA( FBlock&                         iBlock
                                 , const FVec2I&                   iCtrlPt0
                                 , const FVec2I&                   iCtrlPt1
                                 , const FVec2I&                   iCtrlPt2
                                 , const float                     iWeight
                                 , const FColor&                   iColor
                                 , const FRectI&                   iClippingRect )
{
    int x = iCtrlPt0.x - 2 * iCtrlPt1.x + iCtrlPt2.x;
    int y = iCtrlPt0.y - 2 * iCtrlPt1.y + iCtrlPt2.y;
    double dx = iCtrlPt0.x - iCtrlPt1.x;
    double dy = iCtrlPt0.y - iCtrlPt1.y;
    double dWeight;
    double dt;
    double dq;

    FVec2I pt0 = iCtrlPt0;
    FVec2I pt1 = iCtrlPt1;
    FVec2I pt2 = iCtrlPt2;

    float weight = iWeight;

    if(weight < 0) //Can't draw a bezier curve with a weight < 0
        return;

    if(dx * (pt2.x - pt1.x) > 0)
    {
        if(dy * (pt2.y - pt1.y) > 0)
        {
            if(FMath::Abs(dx * y) > FMath::Abs(dy * x))
            {
                pt0.x = pt2.x;
                pt2.x = int(dx + pt1.x);
                pt0.y = pt2.y;
                pt2.y = int(dy + pt1.y);
            }
        }
        if(pt0.x == pt2.x || weight == 1.0)
        {
            dt = (pt0.x - pt1.x) / (double)x;
        } else
        {
            dq = std::sqrt(4.0 * weight * weight * (pt0.x - pt1.x) * (pt2.x - pt1.x) + (pt2.x - pt0.x) * (long)(pt2.x - pt0.x));

            if(pt1.x < pt0.x)
                dq = -dq;

            dt = (2.0 * weight * (pt0.x - pt1.x) - pt0.x + pt2.x + dq) / (2.0 * (1.0 - weight) * (pt2.x - pt0.x));
        }
        dq = 1.0 / (2.0 * dt * (1.0 - dt) * (weight - 1.0) + 1.0);
        dx = (dt * dt * (pt0.x - 2.0 * weight * pt1.x + pt2.x) + 2.0 * dt * (weight * pt1.x - pt0.x) + pt0.x) * dq;
        dy = (dt * dt * (pt0.y - 2.0 * weight * pt1.y + pt2.y) + 2.0 * dt * (weight * pt1.y - pt0.y) + pt0.y) * dq;
        dWeight = dt * (weight - 1.0) + 1.0;
        dWeight *= (dWeight * dq);
        weight = float(((1.0 - dt) * (weight - 1.0) + 1.0) * std::sqrt(dq));
        x = int(std::floor(dx + 0.5));
        y = int(std::floor(dy + 0.5));
        dy = (dx - pt0.x) * (pt1.y - pt0.y) / (pt1.x - pt0.x) + pt0.y;
        InternalDrawQuadRationalBezierSegAA<T>(iBlock,pt0.x,pt0.y,x,int(std::floor(dy + 0.5)),x,y,float(dWeight),iColor,iClippingRect);
        dy = (dx - pt2.x) * (pt1.y - pt2.y) / (pt1.x - pt2.x) + pt2.y;
        pt1.y = int(std::floor(dy + 0.5));
        pt0.x = pt1.x = x;
        pt0.y = y;
    }

    if((pt0.y - pt1.y) * (long)(pt2.y - pt1.y) > 0)
    {
        if(pt0.y == pt2.y || iWeight == 1.0)
        {
            dt = (pt0.y - pt1.y) / (pt0.y - 2.0 * pt1.y + pt2.y);
        } else
        {
            dq = std::sqrt(4.0 * weight * weight * (pt0.y - pt1.y) * (pt2.y - pt1.y) + (pt2.y - pt0.y) * (long)(pt2.y - pt0.y));

            if(pt1.y < pt0.y)
                dq = -dq;

            dt = (2.0 * weight * (pt0.y - pt1.y) - pt0.y + pt2.y + dq) / (2.0 * (1.0 - weight) * (pt2.y - pt0.y));
        }
        dq = 1.0 / (2.0 * dt * (1.0 - dt) * (weight - 1.0) + 1.0);
        dx = (dt * dt * (pt0.x - 2.0 * weight * pt1.x + pt2.x) + 2.0 * dt * (weight * pt1.x - pt0.x) + pt0.x) * dq;
        dy = (dt * dt * (pt0.y - 2.0 * weight * pt1.y + pt2.y) + 2.0 * dt * (weight * pt1.y - pt0.y) + pt0.y) * dq;
        dWeight = dt * (weight - 1.0) + 1.0;
        dWeight *= (dWeight * dq);
        weight = float(((1.0 - dt) * (weight - 1.0) + 1.0) * std::sqrt(dq));
        x = int(std::floor(dx + 0.5));
        y = int(std::floor(dy + 0.5));
        dx = (pt1.x - pt0.x) * (dy - pt0.y) / (pt1.y - pt0.y) + pt0.x;
        InternalDrawQuadRationalBezierSegAA<T>(iBlock,pt0.x,pt0.y,int(std::floor(dx + 0.5)),y,x,y,float(dWeight),iColor,iClippingRect);

        dx = (pt1.x - pt2.x) * (dy - pt2.y) / (pt1.y - pt2.y) + pt2.x;
        pt1.x = int(std::floor(dx + 0.5));
        pt0.x = x;
        pt0.y = pt1.y = y;
    }
    InternalDrawQuadRationalBezierSegAA<T>(iBlock,pt0.x,pt0.y,pt1.x,pt1.y,pt2.x,pt2.y,weight * weight,iColor,iClippingRect);
}

ULIS_NAMESPACE_END

