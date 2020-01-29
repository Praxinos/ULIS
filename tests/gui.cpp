// Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         gui.cpp
* @author       Clement Berthaud
* @brief        Gui application for ULIS2.
* @copyright    Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include <ULIS2>
#include <QApplication>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <vectorclass.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <Windows.h>
#include <filesystem>
using namespace ::ul2;

bool replace(std::string& str, const std::string& from, const std::string& to) {
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

int
main( int argc, char *argv[] )
{
    // Windows
    // On Windows newer than 3.1, the font directory is located in %WINDIR%\fonts.
    // 
    // Mac OS X
    // /System/Library/Fonts - Fonts necessary for the system. Do not touch these.
    // /Library/Fonts - Additional fonts that can be used by all users. This is generally where fonts go if they are to be used by other applications.
    // ~/Library/Fonts - Fonts specific to each user.
    // /Network/Library/Fonts - Fonts shared for users on a network.
    // 
    // Linux
    // /usr/share/fonts, /usr/local/share/fonts, and user-specific ~/.fonts
    // /etc/fonts/fonts.conf or /etc/fonts/local.conf.

    std::string font_path;

    #ifdef ULIS2_WIN
        CHAR windir[MAX_PATH];
        GetWindowsDirectoryA( windir, MAX_PATH );
        font_path = std::string( windir );
    #elif defined ULIS2_MACOS
        font_path = "";
    #elif defined ULIS2_LINUX
        font_path = "";
    #endif

    while( replace( font_path, "\\", "/" ) ) {}

    FT_Library  library;
    FT_Error error = FT_Init_FreeType( &library );
    if( error )
    {
      std::cout << "an error occurred during freetype library initialization ..." << std::endl;
    }


    //8bit to float
    /*
    {
        Vec16uc src( 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 );
        Vec16us ex16 = extend( src );
        Vec16ui ex32 = extend( ex16 );
        Vec16f  dst = to_float( ex32 );
        auto dummy = 0;
    }
    */

    //8bit mul
    /*
    {
        Vec16uc a( 0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF );
        Vec16uc b( 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF );
        Vec16us mul = extend( a ) * extend( b );
        Vec16uc res = compress( ( mul + 1 + ( mul >> 8 ) ) >> 8 );
        auto dummy = 0;
    }
    */

    int width   = 512;
    int height  = 512;
    FBlock blockA( width, height, ULIS2_FORMAT_RGBA8 );
    FBlock blockB( width, height, ULIS2_FORMAT_RGBA8 );

    FThreadPool pool;
    FPerf perf_low( Perf_Lowest );
    FPerf perf_best( Perf_Best_CPU );
    FCPU cpu_info;
    FPixel white( ULIS2_FORMAT_RGB8, { 255, 255, 255 } );
    FPixel black( ULIS2_FORMAT_RGB8, { 0, 0, 0 } );
    Fill( &pool,  ULIS2_BLOCKING, perf_best, cpu_info, &blockA, white, ULIS2_NOCB );
    Fill( &pool,  ULIS2_BLOCKING, perf_best, cpu_info, &blockB, black, ULIS2_NOCB );

    for( int x = 0; x < 512; ++x )
    {
        float t = x / 512.f;
        for( int y = 0; y < 512; ++y )
        {
            blockB.PixelProxy( x, y ).SetAlpha( t );
        }
    }

    BlendRect( &pool, ULIS2_BLOCKING, perf_low, cpu_info, ULIS2_SUBPIXEL, &blockB, &blockA, FRect( 0, 0, 256, 256 ), glm::vec2( 32.5f, 32.5f ), BM_BAYERDITHER8x8, AM_NORMAL, 1.f, ULIS2_CALLCB );

    // Qt Window
    QApplication app( argc, argv );
    QWidget* widget = new  QWidget();
    QImage*  image  = new  QImage( blockA.DataPtr(), blockA.Width(), blockA.Height(), blockA.BytesPerScanLine(), QImage::Format::Format_RGBA8888 );
    QPixmap  pixmap = QPixmap::fromImage( *image );
    QLabel*  label  = new  QLabel( widget );
    label->setPixmap( pixmap );
    widget->resize( pixmap.size() );
    widget->show();

    int exit_code = app.exec();

    delete  label;
    delete  image;
    delete  widget;

    return  exit_code;
}

