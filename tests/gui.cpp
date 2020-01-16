// Copyright � 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         gui.cpp
* @author       Clement Berthaud
* @brief        Gui application for ULIS2.
* @copyright    Copyright � 2018-2019 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include <ULIS2>
#include <CL/cl.hpp>
#include <QApplication>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QLabel>
using namespace ::ul2;

int
main( int argc, char *argv[] )
{
    QApplication app( argc, argv );

    int width   = 1024;
    int height  = 1024;
    FBlock blockA( width, height, ULIS2_FORMAT_RGBA8 );
    FBlock blockB( width, height, ULIS2_FORMAT_RGBA8 );
    FThreadPool pool;
    FPerf perf_best( Perf_Best_CPU );
    FPerf perf_low( Perf_Lowest );
    Clear( pool, &blockA, perf_best );
    Clear( pool, &blockB, perf_best );
    FPixel color( ULIS2_FORMAT_RGB8, { 255, 0, 0 } );
    Fill( pool, &blockA, color, perf_best );
    Swap( pool, &blockA, 0, 2,  perf_best );
    Swap( pool, &blockA, 1, 2,  perf_best );
    Fill( pool, &blockB, color, perf_best );
    Blend( pool, &blockB, &blockA, FPoint(), BM_NORMAL, AM_NORMAL, 1.0f, perf_low );
    QWidget* widget = new  QWidget();
    QImage*  image  = new  QImage( blockA.DataPtr(), blockA.Width(), blockA.Height(), blockA.BytesPerScanLine(), QImage::Format::Format_RGBA8888 );
    QPixmap  pixmap = QPixmap::fromImage( *image );
    QLabel*  label  = new  QLabel( widget );
    label->setPixmap( pixmap );

    widget->resize( pixmap.size() );
    widget->show();

    int exit_code = app.exec();

    delete label;
    delete image;
    delete widget;

    return  exit_code;

    return  0;
}

