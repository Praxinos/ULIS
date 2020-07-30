// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         Matrix2x2.h
* @author       Clement Berthaud
* @brief        This file provides the definition for the TMatrix2x2 class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"
#include "Maths/Geometry/MatrixFunc.h"
#include "Maths/Geometry/Vector2.h"
#include "Maths/Maths.h"

ULIS3_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      TMatrix2x2
/// @brief      The TMatrix2x2 class provides a simple 2x2 square matrix class for
///             geometry applications.
/// @details    The TMatrix2x2 class is a template class that provides basic matrix
///             functionnalities, it can be used with any numeric types,
///             ranging from integer values to floating point values.
template< typename T >
class TMatrix2x2
{
public:
    typedef TVector2< T > tRow;
    typedef TVector2< T > tColumn;

private:
    /*! The columns of the matrix. */
    tColumn mCols[2];

public:
    // Constructors
    /*! Default constructor ( identity matrix ). */
    ULIS3_MATRIX_FUNC TMatrix2x2();

    /*! Constructor from input diagonal value. */
    ULIS3_MATRIX_FUNC TMatrix2x2( T iValue );

    /*! Constructor from input cells values. */
    ULIS3_MATRIX_FUNC TMatrix2x2( T iM00, T iM10, T iM01, T iM11 );

    /*! Constructor from row vector values. */
    ULIS3_MATRIX_FUNC TMatrix2x2( const tColumn& iCol0, const tColumn& iCol1 );

    /*! Copy constructor. */
    ULIS3_MATRIX_FUNC TMatrix2x2( const TMatrix2x2& iOther );

    /*! Assignment operator from matrix. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator=( const TMatrix2x2& iOther );



    // Conversion
    /*! Conversion constructor from other matrix type. */
    template< typename U >
    ULIS3_MATRIX_FUNC TMatrix2x2( const TMatrix2x2< U >& iOther );



    // Named Functions
    /*! Obtain the inverse of the matrix. */
    ULIS3_MATRIX_FUNC TMatrix2x2 Inverse();

    /*! Obtain the transpose of the matrix. */
    ULIS3_MATRIX_FUNC TMatrix2x2 Transpose();

    /*! Obtain the determinant of the matrix. */
    ULIS3_MATRIX_FUNC T Determinant();



    // Boolean operators
    /*! Equality operator. */
    ULIS3_MATRIX_FUNC bool operator==( const TMatrix2x2& iOther );

    /*! Inequality operator. */
    ULIS3_MATRIX_FUNC bool operator!=( const TMatrix2x2& iOther );



    // Unary Negative Operator
    /*! Unary negative operator. */
    ULIS3_MATRIX_FUNC TMatrix2x2 operator-() const;



    // Unary arithmetic operator with scalar
    /*! Unary addition operator with value. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator+=( T iValue );

    /*! Unary substraction operator with value. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator-=( T iValue );

    /*! Unary multiplication operator with value. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator*=( T iValue );

    /*! Unary division operator with value. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator/=( T iValue );



    // Unary arithmetic operator with matrix
    /*! Unary addition operator with matrix. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator+=( const TMatrix2x2& iOther );

    /*! Unary substraction operator with matrix. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator-=( const TMatrix2x2& iOther );

    /*! Unary multiplication operator with matrix. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator*=( const TMatrix2x2& iOther );

    /*! Unary division operator with matrix. */
    ULIS3_MATRIX_FUNC TMatrix2x2& operator/=( const TMatrix2x2& iOther );



    // Other Operators
    /*! Subscript accessor for matrix columns. */
    ULIS3_MATRIX_FUNC tColumn& operator[]( int iIndex );

    /*! Subscript accessor for const matrix columns. */
    ULIS3_MATRIX_FUNC const tColumn& operator[]( int iIndex ) const;
};



// Constructors
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::TMatrix2x2()
    : mCols {
          tColumn( 1, 0 )
        , tColumn( 0, 1 )
    }
{}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::TMatrix2x2( T iValue )
    : mCols {
          tColumn( iValue, 0 )
        , tColumn( 0, iValue )
    }
{}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::TMatrix2x2( T iM00, T iM10, T iM01, T iM11 )
    : mCols {
          tColumn( iM00, iM01 )
        , tColumn( iM10, iM11 )
    }
{}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::TMatrix2x2( const tColumn& iCol0, const tColumn& iCol1 )
    : mCols {
          iCol0
        , iCol1
    }
{}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::TMatrix2x2( const TMatrix2x2< T >& iOther )
    : mCols {
          iOther.mCols[0]
        , iOther.mCols[1]
    }
{}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator=( const TMatrix2x2< T >& iOther ) {
    mCols[0] = iOther.mCols[0];
    mCols[1] = iOther.mCols[1];
}



// Conversion
template< typename T >
template< typename U >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::TMatrix2x2( const TMatrix2x2< U >& iOther )
    : mCols {
          tColumn( iOther.mCols[0] )
        , tColumn( iOther.mCols[1] )
    }
{}



// Named Functions
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > TMatrix2x2< T >::Inverse() {
    T inv_det = static_cast< T >( 1 ) / Determinant();
    return  TMatrix2x2< T >(
          + (*this)[1][1] * inv_det
        , - (*this)[1][0] * inv_det
        , - (*this)[0][1] * inv_det
        , + (*this)[0][0] * inv_det
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > TMatrix2x2< T >::Transpose() {
    return  TMatrix2x2< T >(
          (*this)[0][0]
        , (*this)[1][0]
        , (*this)[0][1]
        , (*this)[1][1]
    );
}

template< typename T >
ULIS3_MATRIX_FUNC T TMatrix2x2< T >::Determinant() {
    return  (*this)[0][0] * (*this)[1][1] - (*this)[1][0] * (*this)[0][1];
}



// Boolean operators
template< typename T >
ULIS3_MATRIX_FUNC bool TMatrix2x2< T >::operator==( const TMatrix2x2< T >& iOther ) {
    return  mCols[0] == iOther.mCols[0] && mCols[1] == iOther.mCols[1];
}

template< typename T >
ULIS3_MATRIX_FUNC bool TMatrix2x2< T >::operator!=( const TMatrix2x2< T >& iOther ) {
    return  !( *this == iOther );
}



// Unary Negative Operator
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > TMatrix2x2< T >::operator-() const {
    return  TMatrix2x2< T >( -mCols[0], -mCols[1] );
}



// Unary arithmetic operator with scalar
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator+=( T iValue ) {
    mCols[0] += iValue;
    mCols[1] += iValue;
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator-=( T iValue ) {
    mCols[0] -= iValue;
    mCols[1] -= iValue;
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator*=( T iValue ) {
    mCols[0] *= iValue;
    mCols[1] *= iValue;
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator/=( T iValue ) {
    mCols[0] /= iValue;
    mCols[1] /= iValue;
}



// Unary arithmetic operator with matrix
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator+=( const TMatrix2x2& iOther ) {
    mCols[0] += iOther.mCols[0];
    mCols[1] += iOther.mCols[1];
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator-=( const TMatrix2x2& iOther ) {
    mCols[0] -= iOther.mCols[0];
    mCols[1] -= iOther.mCols[1];
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator*=( const TMatrix2x2& iOther ) {
    return  ( *this = *this * iOther );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >& TMatrix2x2< T >::operator/=( const TMatrix2x2& iOther ) {
    return  ( *this *= iOther.Inverse() );
}



// Other Operators
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::tColumn& TMatrix2x2< T >::operator[]( int iIndex ) {
    ULIS3_ASSERT( iIndex >= 0 && iIndex < 2, "Bad Index" );
    return  mCols[ iIndex ];
}

template< typename T >
ULIS3_MATRIX_FUNC const TMatrix2x2< T >::tColumn& TMatrix2x2< T >::operator[]( int iIndex ) const {
    ULIS3_ASSERT( iIndex >= 0 && iIndex < 2, "Bad Index" );
    return  mCols[ iIndex ];
}



// Binary arithmetic operator with scalar on left hand side
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator+( T iValue, const TMatrix2x2< T >& iMat ) {
    return  TMatrix2x2< T >(
          iValue + iMat[0]
        , iValue + iMat[1]
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator-( T iValue, const TMatrix2x2< T >& iMat ) {
    return  TMatrix2x2< T >(
          iValue - iMat[0]
        , iValue - iMat[1]
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator*( T iValue, const TMatrix2x2< T >& iMat ) {
    return  TMatrix2x2< T >(
          iValue * iMat[0]
        , iValue * iMat[1]
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator/( T iValue, const TMatrix2x2< T >& iMat ) {
    return  TMatrix2x2< T >(
          iValue / iMat[0]
        , iValue / iMat[1]
    );
}



// Binary arithmetic operator with scalar on right hand side
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator+( const TMatrix2x2< T >& iMat, T iValue ) {
    return  TMatrix2x2< T >(
          iMat[0] + iValue
        , iMat[1] + iValue
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator-( const TMatrix2x2< T >& iMat, T iValue ) {
    return  TMatrix2x2< T >(
          iMat[0] - iValue
        , iMat[1] - iValue
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator*( const TMatrix2x2< T >& iMat, T iValue ) {
    return  TMatrix2x2< T >(
          iMat[0] * iValue
        , iMat[1] * iValue
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator/( const TMatrix2x2< T >& iMat, T iValue ) {
    return  TMatrix2x2< T >(
          iMat[0] / iValue
        , iMat[1] / iValue
    );
}



// Binary arithmetic operator with other matrix
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator+( const TMatrix2x2< T >& iMat, const TMatrix2x2< T >& iOther ) {
    return  TMatrix2x2< T >(
          iMat[0] + iOther[0]
        , iMat[1] + iOther[1]
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator-( const TMatrix2x2< T >& iMat, const TMatrix2x2< T >& iOther ) {
    return  TMatrix2x2< T >(
          iMat[0] - iOther[0]
        , iMat[1] - iOther[1]
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator*( const TMatrix2x2< T >& iMat, const TMatrix2x2< T >& iOther ) {
    return  TMatrix2x2< T >(
          iMat[0][0] * & iOther[0][0] + iMat[1][0] * & iOther[0][1]
        , iMat[0][0] * & iOther[1][0] + iMat[1][0] * & iOther[1][1]
        , iMat[0][1] * & iOther[0][0] + iMat[1][1] * & iOther[0][1]
        , iMat[0][1] * & iOther[1][0] + iMat[1][1] * & iOther[1][1]
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T > operator/( const TMatrix2x2< T >& iMat, const TMatrix2x2< T >& iOther ) {
    return  iMat * iOther.Inverse();
}



// Binary matrix vector multiplication operators.
template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::tColumn operator*( const TMatrix2x2< T >& iMat, const TMatrix2x2< T >::tRow& iRow ) {
    return  TVector2< T >(
          iMat[0][0] * iRow.x + iMat[1][0] * iRow.y
        , iMat[0][1] * iRow.x + iMat[1][1] * iRow.y
    );
}

template< typename T >
ULIS3_MATRIX_FUNC TMatrix2x2< T >::tRow operator*( const TMatrix2x2< T >::tColumn& iColum, const TMatrix2x2< T >& iMat ) {
    return  TVector2< T >(
          iColum.x * iMat[0][0] + iColum.y * iMat[0][1]
        , iColum.x * iMat[1][0] + iColum.y * iMat[1][1]
    );
}

ULIS3_NAMESPACE_END
