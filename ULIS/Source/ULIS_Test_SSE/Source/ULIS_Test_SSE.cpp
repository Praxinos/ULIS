/*************************************************************************
*
*   ULIS
*__________________
*
* ULIS_Test_SSE.cpp
* Clement Berthaud - Layl
* Please refer to LICENSE.md
*/

#include <ULIS_Core>
using namespace ::ULIS;
#define PRINTV( i ) printv( #i, i )

inline __m128i _mm_mullo_epi8(__m128i a, __m128i b)
{
    __m128i zero    = _mm_setzero_si128();
    __m128i Alo     = _mm_unpacklo_epi8(a, zero);
    __m128i Ahi     = _mm_unpackhi_epi8(a, zero);
    __m128i Blo     = _mm_unpacklo_epi8(b, zero);
    __m128i Bhi     = _mm_unpackhi_epi8(b, zero);
    __m128i Clo     = _mm_mullo_epi16(Alo, Blo);
    __m128i Chi     = _mm_mullo_epi16(Ahi, Bhi);
    __m128i maskLo  = _mm_set_epi8(0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 14, 12, 10, 8, 6, 4, 2, 0);
    __m128i maskHi  = _mm_set_epi8(14, 12, 10, 8, 6, 4, 2, 0, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80, 0x80);
    __m128i C       = _mm_or_si128(_mm_shuffle_epi8(Clo, maskLo), _mm_shuffle_epi8(Chi, maskHi));
     return C;
}


void printv( const char* title, const ::ULIS::FVectorSIMD128& iVec )
{
    std::cout << title << ": [";
    for( int i = 0; i < 16; ++i )
        std::cout << (int)iVec.u8[i] << (i == 15 ? "" : ",");
    std::cout << "]" << std::endl;
}

void printv( const char* title, const ::ULIS::FDualVectorSIMD128& iVec )
{
    std::cout << "Dual " << title << std::endl;
    printv( "lo", iVec.lo );
    printv( "hi", iVec.hi );
}


int main()
{

    FVectorSIMD128 vec;
    for( int i = 0; i < 16; ++i ) vec.u8[i] = i;

    FVectorSIMD128 mul;
    mul.m128i = _mm_set1_epi8( 16 );

    FVectorSIMD128 res;
    res.m128i = _mm_mullo_epi8( vec.m128i, mul.m128i );

    FDualVectorSIMD128 dual;
    dual.lo.m128i = vec.m128i;
    dual.hi.m128i = vec.m128i;

    PRINTV( vec );
    PRINTV( dual );

    return 0;
}

