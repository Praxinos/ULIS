// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         ClearFillCopy.cpp
* @author       Clement Berthaud
* @brief        ClearFillCopy application for ULIS.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include <ULIS>
#include <QApplication>
#include <QWidget>
#include <QImage>
#include <QPixmap>
#include <QLabel>
#include <chrono>

using namespace ::ULIS;

int
main( int argc, char *argv[] ) {
    // Start by defining pool thread to host workers
    FThreadPool pool;
    // Define the command queue, multiple queues can use the same pool, or they can use different pools
    FCommandQueue queue( pool );
    // Document format is RGBA8
    eFormat fmt = Format_RGBA8;
    // Define the context, multiple contexts can share a queue or get to each their own. Here we specifically specify our perf intent is to use AVX when available, for all image processing tasks.
    FContext ctx( queue, fmt, PerformanceIntent_AVX );
    // Finally, detect host device to get runtime information about support for SSE and AVX features.
    FHardwareMetrics hw;
    // Some common policies for our tasks
    FSchedulePolicy policy_cache_efficient( ScheduleTime_Sync, ScheduleRun_Multi,ScheduleMode_Chunks,ScheduleParameter_Length,hw.L1CacheLineSize());
    FSchedulePolicy policy_mono_chunk(ScheduleTime_Sync, ScheduleRun_Mono,ScheduleMode_Chunks,ScheduleParameter_Count,1);

    // Create one RGBA8 block to host the operations
    // Using Format_RGBA8 as desired format for display
    FBlock* blockCanvas = new FBlock( 900, 600, fmt );

    // Start processing the blocks
    // We will first fill a part of the block
    // Next we will clear a part of the block
    // Finally we will copy a part of the block
    // Let's first define the fill color for our block:
    FColor color = FColor::RGBA8( 255, 0, 0 );

    // Gather start time to output the time it took to perform the blend composition
    auto startTime = std::chrono::steady_clock::now();

    // Notice the two successive commands here: we don't need to fence because we work on different areas of the block for the first two operations.
    ctx.Fill( *blockCanvas, color, FRectI( 0, 0, 300, 600 ), policy_cache_efficient );
    ctx.Clear( *blockCanvas, FRectI( 300, 0, 300, 600 ), policy_cache_efficient );
    // Now we fence ( and flush at the same time, so finish ).
    ctx.Finish();
    ctx.Copy( *blockCanvas, *blockCanvas, FRectI( 0, 0, 300, 600 ), FVec2I( 600, 0 ), policy_cache_efficient );
    // Finish, again. Else the command is not issued
    ctx.Finish();

    // Now let's see something interesting:
    // We can fill a block in a specific format like RGBA8 with a color source that uses any model, and any format, without worries
    // For example, we can fill a RGB block from a Lab or HSV source
    FColor colorHSV = FColor::HSVAF( 0.5f, 1.f, 1.f );
    ctx.Fill( *blockCanvas, colorHSV, FRectI( 0, 300, 900, 300 ) );
    ctx.Finish();

    // Before displaying the window, gather the end time and delta to output the time it took to process all ULIS operations.
    // We are not interested in the time it took Qt to create the window.
    auto endTime = std::chrono::steady_clock::now();
    auto delta   = std::chrono::duration_cast< std::chrono::milliseconds >( endTime - startTime ).count();

    // Let's recap:
    // We performed a block allocation
    // We performed 1 fill
    // We performed 1 clears
    // We performed 1 copy
    // That makes:
    //      1 Alloc     ( 900*600 )
    //      1 Fill      ( 300*600 )
    //      1 Clear     ( 300*600 )
    //      1 Copy      ( 300*600 )
    // Average on my desktop setup: 14ms
    // Average on my laptop setup:  <unavailable>
    // Remember: everything is multithreaded, SSE and AVX are used whenever possible, everything is computed on CPU
    // Print out the result time.
    std::cout << "ULIS ClearFillCopy: Composition took " << delta << "ms." << std::endl;

    // Create a Qt application and a simple window to display the result block we computed.
    // We create a QImage from the blockCanvas data, QImage does not own the data, so it still lives in blockCanvas, so we don't delete it right now.
    // For Qt Interoperability, several formats are compatible with ULIS formats. Here we chose RGBA8888 which has the same memory layout as ULIS_FORMAT_RGBA8
    QApplication    app( argc, argv );
    QWidget*        widget  = new QWidget();
    QImage*         image   = new QImage( blockCanvas->Bits()
                                        , blockCanvas->Width()
                                        , blockCanvas->Height()
                                        , blockCanvas->BytesPerScanLine()
                                        , QImage::Format_RGBA8888 );
    QPixmap         pixmap  = QPixmap::fromImage( *image );
    QLabel*         label   = new QLabel( widget );
    label->setPixmap( pixmap );
    widget->resize( pixmap.size() );
    widget->show();

    // Get error code. This function returns when all Qt Windows are closed.
    int exit_code = app.exec();

    // Delete Qt Components.
    delete  label;
    delete  image;
    delete  widget;

    // Delete our block Canvas.
    delete  blockCanvas;

    // Return exit code.
    return  exit_code;
}

