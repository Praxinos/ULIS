/*************************************************************************
*
*   ULIS
*__________________
*
* ULIS_Test.cpp
* Clement Berthaud - Layl
* Please refer to LICENSE.md
*/

#include <iostream>
#include <cstring>
#include <ULIS_Core>
#include <array>

#include <boost/preprocessor/arithmetic/add.hpp>
#include <boost/preprocessor/arithmetic/sub.hpp>

using namespace ::ULIS;

/*
template< int S >
struct regt
{
    constexpr const int operator[]( int i ) const { return n[i]; }
    const uint32 n[S] = { 0, 1 };
    template< int N, typename T, T... Nums > constexpr const regt< N > push_back_impl( uint32 i, ::ULIS::integer_sequence< T, Nums... > ) const { return { n[Nums] ..., i }; }
    constexpr const regt< S + 1 > push_back( uint32 i ) const { return push_back_impl< S + 1 >( i, ::ULIS::make_integer_sequence< int, S >() ); }
};

constexpr regt< 1 > regbase = { 0 };
constexpr auto reg1 = regbase.push_back( 3244 );
constexpr auto reg2 = reg1.push_back( 4126 );

#define PREVIOUS( i ) BOOST_PP_SUB( i, 1 )
#define APPLY( a ) a
#define CAT( a, b ) APPLY( a ) ## APPLY( b )
#define CREATE_REG( i ) constexpr regt< 1 > CAT( reg,  APPLY(__LINE__) ) = { i };
#define APPEND_REG( i ) constexpr auto CAT( reg,  APPLY(__LINE__) ) = CAT( reg, PREVIOUS( __LINE__ ) ).push_back( i );
#define ASSIGN_REG CAT( reg, PREVIOUS( __LINE__ ) )

CREATE_REG( 0 )
APPEND_REG( 1 )
APPEND_REG( 2 )
APPEND_REG( 3 )
constexpr auto reg = ASSIGN_REG;

int Gen( int i )
{
    switch( i )
    {
        case reg[0]:
            return reg[0];

        case reg[1]:
            return reg[1];
    }
}
*/

//@todo: impl this index indirection
/*
static constexpr const uint8 SeekIndexForChar( const char* ilayout, const char* imodel, uint8 num ) { return ::ULIS::ct_charindex_without_digits( imodel[num], ilayout ); }
template <uint8 N, typename T, T... Nums> static constexpr const std::array<uint8, N-1> make_impl( const char* ilayout, const char (&imodel)[N], ::ULIS::integer_sequence<T, Nums...>) { return { SeekIndexForChar( ilayout, imodel, Nums ) ... }; }
template <uint8 N> static constexpr const std::array<uint8, N-1> make_index_from_string( const char* ilayout, const char (&imodel)[N]) { return make_impl( ilayout, imodel, ::ULIS::make_integer_sequence<uint8, N-1>()); }


static constexpr const char layout[] = "B8G8R8A8";
static constexpr const char model[] =  "RGBA";
static constexpr const int count = ::ULIS::strlen( model );
static constexpr const std::array< uint8, count > arr = make_index_from_string( layout, model );
*/


int main()
{
    constexpr auto c_fmt = ULIS_CONST_STR( "Spec__ml_interleaved__am_straight__tp_uint8" );
    constexpr auto a_fmt = ULIS_CONST_STR( "Spec__tp_uint8__am_straight__ml_interleaved" );
    constexpr const int imlstart    = ULIS_PARSE_KW_START( a_fmt, "_tp:" );
    constexpr const int imlnext     = ULIS_PARSE_KW_NEXT( a_fmt, "_tp:" );
    constexpr const int imlend      = ULIS_PARSE_KW_END( a_fmt, "_tp:" );
    constexpr const int imlstop     = ULIS_PARSE_KW_STOP( a_fmt, "_tp:" );
    constexpr const int dml         = ULIS_PARSE_KW_DELTA( a_fmt, "_tp:" );
    constexpr  auto tmp0            = ULIS_PARSE_KW_SUBSTR( a_fmt, "_tp:" );
    constexpr auto r_fmt = ULIS_CONST_STR( "Spec" ).Append< ::ULIS::_CT::strlen( "_tp:" ) + 1 >( "_tp:" ).Append< dml + 1 >( tmp0.s );

    constexpr auto g_fmt = ULIS_CONST_STR( "Spec" ).ULIS_PARSE_KW_APPEND_W( a_fmt, ml ).ULIS_PARSE_KW_APPEND_W( a_fmt, am ).ULIS_PARSE_KW_APPEND_W( a_fmt, tp );

    static_assert( g_fmt == c_fmt, "..." );
    static_assert( g_fmt.CRC32() == c_fmt.CRC32(), "..." );

    std::cout << a_fmt << std::endl;
    std::cout << c_fmt << std::endl;
    std::cout << g_fmt << std::endl;
    std::cout << a_fmt.CRC32() << std::endl;
    std::cout << a_fmt.CRC32() << std::endl;
    std::cout << c_fmt.CRC32() << std::endl;
    std::cout << c_fmt.CRC32() << std::endl;
    std::cout << g_fmt.CRC32() << std::endl;
    std::cout << g_fmt.CRC32() << std::endl;
    getchar();
    return 0;
}

