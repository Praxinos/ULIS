// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         MonacoWidget.cpp
* @author       Clement Berthaud
* @brief        pyULIS_Interactive application for testing pyULIS.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#include  "MonacoWidget.h"
#include <QWebEngineView>
#include <QWebEnginePage>

//--------------------------------------------------------------------------------------
//--------------------------------------------------- Default values & Utilities defines
#define  DEFAULT_BACKGROUND_COLOR  QColor( 20, 20, 20 )
#define  WEB_CONTEXT    mEmbeddedBrowser->page()


//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Prefix API


#define  MONACO_HEADER  "file://"
#define  MONACO_SCRIPT  "/monaco-editor/browser-script-editor/index.html"

static QString monaco_prefix    = "/resources";
static QString monaco_path      = MONACO_HEADER + monaco_prefix + MONACO_SCRIPT;

void
SetMonacoPrefix( const  QString&  iPrefix )
{
    monaco_prefix = iPrefix;
    monaco_path = MONACO_HEADER + monaco_prefix + MONACO_SCRIPT;
}


QString
GetMonacoPrefix()
{
    return  monaco_prefix;
}


QString
GetFullMonacoPath()
{
    return  monaco_path;
}


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction


MonacoWidget::~MonacoWidget()
{
    Destroy();
}


MonacoWidget::MonacoWidget( QWidget* iParent ) :
    tSuperClass( iParent ),
    mEmbeddedBrowser( NULL )
{
    Init();
    Build();
    Compose();
}


//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Editor API Exec


void
MonacoWidget::Layout()
{
    if( !ASyncJS_IsReady )  return;

    QVariant unused;
    ASyncJS_Run( WEB_CONTEXT, "editor.layout();", unused )
    return;
}


QString
MonacoWidget::GetText()
{
    if( !ASyncJS_IsReady )  return  QString();

    QVariant result;
    ASyncJS_Run( WEB_CONTEXT, "editor.getValue();", result )
    return  result.toString();
}


void
MonacoWidget::SetText( const  QString&  iStr )
{
    if( !ASyncJS_IsReady )  return;

    QString ct = iStr;
    // Escape quotes
    ct.replace( "'", "\\\'" );
    // Split string @ linebreaks & turn it into a List.
    auto list = ct.split( '\n' );
    // Enclose each substring in simple quotes
    for( auto& it: list ) it = QString( "'%1'" ).arg( it );
    // Enclose the whole into a js array
    auto param = QString( "[%1]" ).arg( list.join( "," ) );

    QVariant unused;
    ASyncJS_Run( WEB_CONTEXT, "setText( "+param+" );", unused )
    return;
}


void
MonacoWidget::ClearText()
{
    SetText( "" );
}


void
MonacoWidget::AppendText( const  QString&  iStr )
{
    SetText( GetText() + iStr );
}


QString
MonacoWidget::GetLanguage()
{
    if( !ASyncJS_IsReady )  return  QString();

    QVariant result;
    ASyncJS_Run( WEB_CONTEXT, "monaco.editor.getModels()[0].getModeId();", result )
    return  result.toString();
}


void
MonacoWidget::SetLanguage( const  QString&  iStr )
{
    if( !ASyncJS_IsReady )  return;

    QVariant unused;
    ASyncJS_Run( WEB_CONTEXT, "monaco.editor.setModelLanguage(monaco.editor.getModels()[0],'"+iStr+"');", unused )
    return;
}


//--------------------------------------------------------------------------------------
//--------------------------------------------------------- Protected Qt events override


void
MonacoWidget::resizeEvent( QResizeEvent*  event )
{
    tSuperClass::resizeEvent( event );
    Compose();
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------ Private Internal GUI Utilitites


void
MonacoWidget::Init()
{
    mEmbeddedBrowser = new QWebEngineView( this );
}


void
MonacoWidget::Build()
{
    mEmbeddedBrowser->setUrl( QUrl( monaco_path ) );
    WEB_CONTEXT->setBackgroundColor( DEFAULT_BACKGROUND_COLOR );
    mEmbeddedBrowser->show();

    QObject::connect( mEmbeddedBrowser, SIGNAL( loadFinished( bool ) ), this, SLOT( ASyncJS_SetReady( bool ) ) );
}


void
MonacoWidget::Compose()
{
    mEmbeddedBrowser->resize( size() );
}


void
MonacoWidget::Destroy()
{
    emit  ASyncJS_UnlockSignal_Name();

    delete  mEmbeddedBrowser;
}


//--------------------------------------------------------------------------------------
//------------------------------------------------------------------- Async JS Framework


ASyncJS_Impl( MonacoWidget )

