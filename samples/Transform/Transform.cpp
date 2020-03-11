// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         Transform.cpp
* @author       Clement Berthaud
* @brief        Transform application for ULIS2.
* @copyright    Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include <ULIS2>

#include <QApplication>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QLabel>

using namespace ::ul2;

int
main( int argc, char *argv[] ) {
    FThreadPool  threadPool;
    FHostDeviceInfo host = FHostDeviceInfo::Detect();
    uint32 perfIntent = ULIS2_PERF_MT | ULIS2_PERF_TSPEC | ULIS2_PERF_SSE42 | ULIS2_PERF_AVX2;

    std::string path = "C:/Users/PRAXINOS/Documents/work/base_160.png";
    FBlock* blockSRC = XLoadFromFile( &threadPool, ULIS2_BLOCKING, perfIntent, host, ULIS2_NOCB, path, ULIS2_FORMAT_RGBA8 );
    FBlock* blockDST = new  FBlock( 800, 600, ULIS2_FORMAT_RGBA8 );

    Transform( &threadPool, ULIS2_BLOCKING, perfIntent, host, ULIS2_NOCB, blockSRC, blockDST, blockSRC->Rect(), FTransform2D( ComposeMatrix( MakeRotationMatrix( 1.12 ), MakeTranslationMatrix( 500, 50 ) ) ), INTERP_NN );

    delete  blockSRC;

    QApplication    app( argc, argv );
    QWidget*        widget  = new QWidget();
    QImage*         image   = new QImage( blockDST->DataPtr()
                                        , blockDST->Width()
                                        , blockDST->Height()
                                        , blockDST->BytesPerScanLine()
                                        , QImage::Format_RGBA8888 );
    QPixmap         pixmap  = QPixmap::fromImage( *image );
    QLabel*         label   = new QLabel( widget );
    label->setPixmap( pixmap );
    widget->resize( pixmap.size() );
    widget->show();

    int exit_code = app.exec();

    delete  label;
    delete  image;
    delete  widget;

    delete  blockDST;

    return  exit_code;
}

