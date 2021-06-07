// IDDN FR.001.250001.004.S.X.2019.000.00000
// ULIS is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
/*
*   ULIS
*__________________
* @file         AnimatedLayerBuilder.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the TAnimatedLayerBuilder class.
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include <functional>

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TAnimatedLayerBuilder
/// @brief      The TAnimatedLayerBuilder class provides a tool for building layerstacks
///             from code.
template< typename T >
class TAnimatedLayerBuilder {
public:

    TAnimatedLayerBuilder< T >( T* iElem )
        : m( iElem )
    {
    }

    template< class ... Args >
    static
    TAnimatedLayerBuilder
    Assign( T** ioElem, Args&& ... args ) {
        T* ptr = new T( std::forward< Args >( args ) ... );
        *ioElem = ptr;
        return  TAnimatedLayerBuilder( ptr );
    }

    template< class ... Args >
    static
    TAnimatedLayerBuilder
    Create( Args&& ... args ) {
        T* ptr = new T( std::forward< Args >( args ) ... );
        return  TAnimatedLayerBuilder( ptr );
    }

    TAnimatedLayerBuilder< T >& Def( std::function< void( T* ) > iFunc ) {
        iFunc( m );
        return  *this;
    }

    template< typename U >
    TAnimatedLayerBuilder< T >& AddChild( TAnimatedLayerBuilder< U >& iObj ) {
        m->AddChild( iObj.m );
        return  *this;
    }

    template< typename U >
    TAnimatedLayerBuilder< T >& operator[]( TAnimatedLayerBuilder< U >& iObj ) {
        m->AddChild( iObj.m );
        return  *this;
    }

public:
    T* m;
};

#define ULAssociateAnimatedStack( _Elem_ )              TAnimatedLayerBuilder< FLayerStack >( _Elem_ )
#define ULAssignAnimatedStack( _Elem_, ... )            TAnimatedLayerBuilder< FLayerStack >::Assign( & _Elem_, __VA_ARGS__ )
#define ULCreateAnimatedChild( _Class_, ... )           TAnimatedLayerBuilder< _Class_ >::Create( __VA_ARGS__ )
#define ULAssignAnimatedChild( _Class_, _Elem_, ... )   TAnimatedLayerBuilder< _Class_ >::Assign( & _Elem_, __VA_ARGS__ )
#define ULAddAnimatedLayer( _Elem_ )                    .AddChild( _Elem_ )
#define ULAnimatedDef( ... )                            .Def( [&]( auto i ){ i-> __VA_ARGS__ ; } )

ULIS_NAMESPACE_END

