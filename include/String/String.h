// Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/*
*
*   ULIS3
*__________________
*
* @file         String.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the FString class.
* @copyright    Copyright 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Core/Core.h"

ULIS_NAMESPACE_BEGIN
/////////////////////////////////////////////////////
/// @class      FString
/// @brief      The FString class provides a simple dynamic string class
/// @details    The need of a custom FString class rose from the fact that std::
///             classes cannot be exported easily accross dll boundaries when
///             ULIS is compiled as a shared library.
///
///             \sa FWString
class ULIS_API FString
{
public:
    typedef char char_type;

public:
    /* Destructor. */
    ~FString();

    /* Default constructor. */
    FString();

    /* Constructor from C string. */
    FString( const char_type* iStr );

    /* Copy constructor. */
    FString( const FString& iStr );

    /* Move constructor. */
    FString( FString&& iStr );

    /* Conversion constructor from WString. */
    FString( const FWString& iWStr );

    /* Copy assignment operator. */
    FString& operator=( const FString& iStr );

    /* Move assignment operator. */
    FString& operator=( FString&& iStr );

    /* Request explicit copy of string */
    FString Copy() const;

    /* Request substring of string */
    FString SubString( uint64 iPos, uint64 iCount ) const;

    /*! Returns the string capacity. */
    uint64 Capacity() const;

    /*! Returns the string size. */
    uint64 Size() const;

    /*! Returns the const raw storage. */
    const char_type* Data() const;

    /*! Access char_type at index. */
    char_type& operator[]( uint64 iIndex );

    /*! Access const char_type at index. */
    const char_type& operator[]( uint64 iIndex ) const;

    /*! Access char_type at index. */
    char_type& At( uint64 iIndex );

    /*! Access const char_type at index. */
    const char_type& At( uint64 iIndex ) const;

    /*! Access char_type at front. */
    char_type& Front();

    /*! Access const char_type at front. */
    const char_type& Front() const;

    /*! Access char_type at back. */
    char_type& Back();

    /*! Access const char_type at back. */
    const char_type& Back() const;

    /*! Convert to WString. */
    FWString ToWString() const;

    /*! Append a string. */
    FString& Append( const FString& iStr );

    /*! Append a string. */
    FString& Append( const char_type* iStr );

    /*! Append a char. */
    FString& Append( char_type iChar );

    /*! Prepend a string. */
    FString& Prepend( const FString& iStr );

    /*! Prepend a string. */
    FString& Prepend( const char_type* iStr );

    /*! Prepend a char. */
    FString& Prepend( char_type iChar );

    /* Equality operator */
    bool operator==( const FString& iOther );

    /* Inequality operator */
    bool operator!=( const FString& iOther );

    /* To Int */
    int ToInt() const;

    /* To Unsigned Int */
    unsigned int ToUnsignedInt() const;

    /* To Float */
    float ToFloat() const;

    /* To Double */
    double ToDouble() const;

    /* Static Maker from Int */
    static FString FromInt( int iValue );

    /* Static Maker from Unsigned Int */
    static FString FromUnsignedInt( unsigned int iValue );

    /* Static Maker from Float */
    static FString FromFloat( float iValue );

    /* Static Maker from Double */
    static FString FromDouble( double iValue );

    /* Static Maker from WString */
    static FString FromWString( const FWString& iValue );

    /* Conversion to uppercase */
    FString ToUpper() const;

    /* Conversion to lowercase */
    FString ToLower() const;

    /*! Returns wether the array is empty or not. */
    bool IsEmpty() const;

    /*!
        Reserve some capacity for future usage, if iCapacity is bigger than the
        current capacity. If iCapacity is lower or equal to capacity, this
        function does nothing.
        This function might invalid the underlying buffer.
        The size doesn't change.
    */
    void Reserve( uint64 iCapacity );

    /*!
        Shrink the capacity to fit the size.
        If the capacity is already the same as the size, the function does
        nothing.
        This function might invalid the underlying buffer.
        The size doesn't change.
    */
    void Shrink();

    /*!
        Resize the actual usage size of the container.
        This function might invalid the underlying buffer.
    */
    void Resize( uint64 iSize );

    /*! Clear the string, deallocating everything. */
    void Clear();

    /*! Split the string. */
    TArray< FString > Split( const FString& iSep ) const;

    /*! Split the string. */
    TArray< FString > Split( char_type iSep ) const;

    /* Erase part of string */
    void Erase( uint64 iPos, uint64 iCount ) const;

    /* Insert part of string */
    void Insert( uint64 iPos, const FString& iStr ) const;

    /* Find First occurence */
    int64 FindFirst( const FString& iStr ) const;

    /* Find Last occurence */
    int64 FindLast( const FString& iStr ) const;

    /* Find occurence starting for position */
    int64 FindFrom( uint64 iPos, const FString& iStr ) const;

    /* Replace First occurence */
    bool ReplaceFirst( const FString& iA, const FString& iB ) const;

    /* Replace Last occurence */
    bool ReplaceLast( const FString& iA, const FString& iB ) const;

    /* Replace occurence starting for position */
    bool ReplaceFrom( uint64 iPos, const FString& iA, const FString& iB ) const;

    /* Replace all occurences */
    uint64 ReplaceAll( const FString& iA, const FString& iB ) const;

    /* Compute MD5 for this string. */
    FString MD5() const;

    /* Compute CRC32 for this string. */
    uint32 CRC32() const;

private:
    void CleanupBulk();
    FString( uint64 iSize );

private:
    char_type* mBulk; ///< The main raw string buffer storage.
    uint64 mCapacity; ///< The string capacity, may be bigger than size.
    uint64 mSize; ///< The string usage size.
};

ULIS_NAMESPACE_END

