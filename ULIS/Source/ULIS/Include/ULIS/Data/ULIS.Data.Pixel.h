/*************************************************************************
*
*   ULIS
*__________________
*
* ULIS.Data.Pixel.h
* Clement Berthaud - Layl
* Please refer to LICENSE.md
*/

#pragma once

#include "ULIS/Data/ULIS.Data.Spec.h"
#include "ULIS/Data/ULIS.Data.Layout.h"
#include "ULIS/Maths/ULIS.Maths.Utility.h"
#include "ULIS/Color/ULIS.Color.CColor.h"
#include "ULIS/Color/ULIS.Color.ColorProfile.h"
#include "ULIS/Color/ULIS.Color.ProfileRegistry.h"
#include "ULIS/Global/ULIS.Global.GlobalProfileRegistry.h"

namespace ULIS {


/////////////////////////////////////////////////////
// Defines
#define tSpec       TBlockInfo<     _SH >
#define tLayout     TPixelLayout<   _SH >

/////////////////////////////////////////////////////
// TPixelTypeSelector
/* Shenanigans for type operations */
template< typename T > struct TNextPixelType { using _tUnderlyingNextPixelType = T;         };
template<> struct TNextPixelType< uint8     >{ using _tUnderlyingNextPixelType = uint16;    };
template<> struct TNextPixelType< uint16    >{ using _tUnderlyingNextPixelType = uint32;    };
template<> struct TNextPixelType< uint32    >{ using _tUnderlyingNextPixelType = uint64;    };
template<> struct TNextPixelType< float     >{ using _tUnderlyingNextPixelType = float;     };
template<> struct TNextPixelType< double    >{ using _tUnderlyingNextPixelType = float;     };
template< typename T > struct TPrevPixelType { using _tUnderlyingPrevPixelType = T;         };
template<> struct TPrevPixelType< uint8     >{ using _tUnderlyingPrevPixelType = uint8;     };
template<> struct TPrevPixelType< uint16    >{ using _tUnderlyingPrevPixelType = uint8;     };
template<> struct TPrevPixelType< uint32    >{ using _tUnderlyingPrevPixelType = uint16;    };
template<> struct TPrevPixelType< float     >{ using _tUnderlyingPrevPixelType = float;     };
template<> struct TPrevPixelType< double    >{ using _tUnderlyingPrevPixelType = float;     };
template< typename T > struct TPixelType { using _tUnderlyingPixelType = T; };
template< e_tp _TP > struct TPixelTypeSelector {};
#define ULIS_TYPE_SELECTOR_REPEAT( r, data, elem )  template<> class TPixelTypeSelector<  BOOST_PP_CAT( e_tp::k, elem ) > : public TPixelType< elem >, public TNextPixelType< elem >, public TPrevPixelType< elem > {};
BOOST_PP_SEQ_FOR_EACH( ULIS_TYPE_SELECTOR_REPEAT, void, ULIS_SEQ_TP )
#define ULIS_FORWARD_TYPE_SELECTOR                                                                      \
public:                                                                                                 \
    using tPixelType        = typename TPixelTypeSelector< tSpec::_nf._tp >::_tUnderlyingPixelType;     \
    using tNextPixelType    = typename TPixelTypeSelector< tSpec::_nf._tp >::_tUnderlyingNextPixelType; \
    using tPrevPixelType    = typename TPixelTypeSelector< tSpec::_nf._tp >::_tUnderlyingPrevPixelType;

/////////////////////////////////////////////////////
// TPixelInfo
/* Basically a wrapper around FSpec, adding type info for ease of use */
template< uint32_t _SH >
class TPixelInfo
{
    ULIS_FORWARD_TYPE_SELECTOR

public:
    // Public API
//  inline virtual type                         Name                ( params )                                      const   override    final   { body                                                                  }
    inline         tPixelType                   Max                 ()                                              const                       { return  (tPixelType)tSpec::_nf._tm;                                   }
    inline         tNextPixelType               Range               ()                                              const                       { return  (tNextPixelType)tSpec::_nf._rm;                               }
    inline         int                          Depth               ()                                              const                       { return tSpec::_nf._pd;                                                }
    inline         e_tp                         Type                ()                                              const                       { return tSpec::_nf._tp;                                                }
    inline         e_cm                         ColorModel          ()                                              const                       { return tSpec::_nf._cm;                                                }
    inline         e_ea                         ExtraAlpha          ()                                              const                       { return tSpec::_nf._ea;                                                }
    inline         bool                         HasAlpha            ()                                              const                       { return ExtraAlpha() == e_ea::khasAlpha;                               }
    inline         const char*                  ChannelLayout       ()                                              const                       { return tSpec::_nf._cl;                                                }
    inline         e_nm                         NormalMode          ()                                              const                       { return tSpec::_nf._nm;                                                }
    inline         bool                         IsNormalized        ()                                              const                       { return NormalMode() == e_nm::knormalized;                             }
    inline         bool                         IsDecimal           ()                                              const                       { return tSpec::_nf._dm;                                                }
    inline         int                          NumChannels         ()                                              const                       { return tSpec::_nf._rc;                                                }
    inline         int                          NumColorChannels    ()                                              const                       { return tSpec::_nf._nc;                                                }
    static inline  int                          RedirectedIndex     ( int i )                                                                   { return tLayout::red.arr[i];                                           }
};


/////////////////////////////////////////////////////
// TNormalizer
template< typename T, e_nm _nm > struct TNormalizer                          { static inline T Apply( T iVal ) { return iVal; }                                  };
template< typename T >           struct TNormalizer< T, e_nm::knormalized >  { static inline T Apply( T iVal ) { return FMath::Clamp< T >( iVal, T(0), T(1) ); } };


/////////////////////////////////////////////////////
// TPixelBase
template< uint32_t _SH >
class TPixelBase : public TPixelInfo< _SH >
{
    typedef TPixelInfo< _SH > tSuperClass;
    ULIS_FORWARD_TYPE_SELECTOR

public:
    // Destructor
    virtual ~TPixelBase() {}

public:
    // Public API
    inline         void*                        Ptr                 ()                                              const                       { return  (void*)d;                                                                                                                                                 }
    inline         tPixelType&                  GetRaw              ( uint8 i )                                                                 { return d[ i ];                                                                                                                                                    }
    inline const   tPixelType&                  GetRaw              ( uint8 i )                                     const                       { return d[ i ];                                                                                                                                                    }
    inline         tPixelType&                  GetComponent        ( uint8 i )                                                                 { return d[ tLayout::red.arr[i] ];                                                                                                                                  }
    inline const   tPixelType&                  GetComponent        ( uint8 i )                                     const                       { return d[ tLayout::red.arr[i] ];                                                                                                                                  }
    inline         void                         SetComponent        ( uint8 i, tPixelType iValue )                                              { d[ tLayout::red.arr[i] ] = TNormalizer< tPixelType, tSpec::_nf._nm >::Apply( iValue );                                                                            }
    inline         tPixelType&                  operator[]          ( uint8 i )                                                                 { return d[ tLayout::red.arr[i] ];                                                                                                                                  }
    inline const   tPixelType&                  operator[]          ( uint8 i )                                     const                       { return d[ tLayout::red.arr[i] ];                                                                                                                                  }
    inline         tPixelType                   GetAlpha            ()                                              const                       { return tSpec::_nf._ea == e_ea::khasAlpha ? d[ tLayout::red.arr[ tSpec::_nf._nc ] ] : tSpec::_nf._nm == e_nm::knormalized ? tPixelType( 1 ) : tSuperClass::Max();  }
    inline         void                         SetAlpha            ( tPixelType iValue )                                                       { if(    tSpec::_nf._ea == e_ea::khasAlpha ) d[ tLayout::red.arr[ tSpec::_nf._nc ] ] = TNormalizer< tPixelType, tSpec::_nf._nm >::Apply( iValue );                  }
    inline         TPixelBase< _SH >&           operator=           ( const TPixelBase< _SH >& iOther )                                         { memcpy( d, iOther.Ptr(), tSpec::_nf._pd ); return *this;                                                                                                          }

protected:
    // Protected Data
    tPixelType* d;
    FColorProfile* profile;
};


/////////////////////////////////////////////////////
// Conversion API
template< typename T1, typename T2 > T2 inline ConvType( T1 iValue ) { return iValue; }
template<> uint16   inline ConvType< uint8,  uint16 >( uint8 iValue  ) { return iValue * 0x101;                     }
template<> uint32   inline ConvType< uint8,  uint32 >( uint8 iValue  ) { return iValue * 0x1010101;                 }
template<> uint64   inline ConvType< uint8,  uint64 >( uint8 iValue  ) { return iValue * 0x10100000101;             }
template<> uint32   inline ConvType< uint16, uint32 >( uint16 iValue ) { return iValue * 0x10001;                   }
template<> uint64   inline ConvType< uint16, uint64 >( uint16 iValue ) { return iValue * 0x1000100010001;           }
template<> uint64   inline ConvType< uint32, uint64 >( uint32 iValue ) { return iValue * 0x100000001;               }
template<> uint8    inline ConvType< uint16, uint8  >( uint16 iValue ) { return ( iValue + 1 + ( iValue>>8 ) ) >> 8;}
template<> uint8    inline ConvType< uint32, uint8  >( uint32 iValue ) { return iValue >> 24;                       }
template<> uint8    inline ConvType< uint64, uint8  >( uint64 iValue ) { return iValue >> 56;                       }
template<> uint16   inline ConvType< uint32, uint16 >( uint32 iValue ) { return iValue >> 16;                       }
template<> uint16   inline ConvType< uint64, uint16 >( uint64 iValue ) { return iValue >> 48;                       }
template<> uint32   inline ConvType< uint64, uint32 >( uint64 iValue ) { return iValue >> 32;                       }
template<> float    inline ConvType< uint8,  float  >( uint8 iValue  ) { return iValue / (float)0xFF;               }
template<> float    inline ConvType< uint16, float  >( uint16 iValue ) { return iValue / (float)0xFFFF;             }
template<> float    inline ConvType< uint32, float  >( uint32 iValue ) { return iValue / (float)0xFFFFFFFF;         }
template<> float    inline ConvType< uint64, float  >( uint64 iValue ) { return iValue / (float)0xFFFFFFFFFFFFFFFF; }
template<> double   inline ConvType< uint8,  double >( uint8 iValue  ) { return iValue / (double)0xFF;              }
template<> double   inline ConvType< uint16, double >( uint16 iValue ) { return iValue / (double)0xFFFF;            }
template<> double   inline ConvType< uint32, double >( uint32 iValue ) { return iValue / (double)0xFFFFFFFF;        }
template<> double   inline ConvType< uint64, double >( uint64 iValue ) { return iValue / (double)0xFFFFFFFFFFFFFFFF;}
template<> uint8    inline ConvType< float,  uint8  >( float iValue  ) { return iValue * 0xFF;                      }
template<> uint16   inline ConvType< float,  uint16 >( float iValue  ) { return iValue * 0xFFFF;                    }
template<> uint32   inline ConvType< float,  uint32 >( float iValue  ) { return iValue * 0xFFFFFFFF;                }
template<> uint64   inline ConvType< float,  uint64 >( float iValue  ) { return iValue * 0xFFFFFFFFFFFFFFFF;        }
template<> uint8    inline ConvType< double, uint8  >( double iValue ) { return iValue * 0xFF;                      }
template<> uint16   inline ConvType< double, uint16 >( double iValue ) { return iValue * 0xFFFF;                    }
template<> uint32   inline ConvType< double, uint32 >( double iValue ) { return iValue * 0xFFFFFFFF;                }
template<> uint64   inline ConvType< double, uint64 >( double iValue ) { return iValue * 0xFFFFFFFFFFFFFFFF;        }


/////////////////////////////////////////////////////
// TPixelAcessor
template< uint32_t _SH, e_cm _CM > class TPixelAcessor : public TPixelBase< _SH > {};

#define ULIS_SPEC_PIXEL_ACCESSOR_START( iCm )                       \
    template< uint32_t _SH >                                        \
    class TPixelAcessor< _SH, BOOST_PP_CAT( e_cm::k, iCm ) >        \
        : public TPixelBase< _SH > {                                \
    typedef TPixelBase< _SH > tSuperClass;                          \
    ULIS_FORWARD_TYPE_SELECTOR                                      \
    public:
#define ULIS_SPEC_PIXEL_ACCESSOR_END    };

#define ULIS_SPEC_COMPONENT( iComp, iIndex )                                                                                \
    inline          tPixelType&     iComp       ()                      { return tSuperClass::GetComponent( iIndex );    }  \
    inline  const   tPixelType&     iComp       () const                { return tSuperClass::GetComponent( iIndex );    }  \
    inline          tPixelType      Get##iComp  ()                      { return tSuperClass::GetComponent( iIndex );    }  \
    inline  void                    Set##iComp  ( tPixelType iValue )   { tSuperClass::SetComponent( iIndex, iValue );   }

ULIS_SPEC_PIXEL_ACCESSOR_START( G )
    ULIS_SPEC_COMPONENT( G,     0           )
    ULIS_SPEC_COMPONENT( Gray,  0           )
    inline  CColor  GetColor()  const {
        return  CColor::FromGreyF( ConvType< tPixelType, float >( G() ), ConvType< tPixelType, float >( tSuperClass::GetAlpha() ) );
    }

    inline  void    SetColor( const CColor& iColor ) {
        G() = ConvType< float, tPixelType >( iColor.GreyF() );
        tSuperClass::SetAlpha( ConvType< float, tPixelType >( iColor.AlphaF() ) );
    }
ULIS_SPEC_PIXEL_ACCESSOR_END

ULIS_SPEC_PIXEL_ACCESSOR_START( RGB )
    ULIS_SPEC_COMPONENT( Red,   0           )
    ULIS_SPEC_COMPONENT( Green, 1           )
    ULIS_SPEC_COMPONENT( Blue,  2           )
    ULIS_SPEC_COMPONENT( R,     0           )
    ULIS_SPEC_COMPONENT( G,     1           )
    ULIS_SPEC_COMPONENT( B,     2           )
    inline  CColor  GetColor()  const {
        return  CColor::FromRGBF( ConvType< tPixelType, float >( R() )
                                , ConvType< tPixelType, float >( G() )
                                , ConvType< tPixelType, float >( B() )
                                , ConvType< tPixelType, float >( tSuperClass::GetAlpha() ) );
    }

    inline  void    SetColor( const CColor& iColor ) {
        R() = ConvType< float, tPixelType >( iColor.RedF() );
        G() = ConvType< float, tPixelType >( iColor.GreenF() );
        B() = ConvType< float, tPixelType >( iColor.BlueF() );
        tSuperClass::SetAlpha( ConvType< float, tPixelType >( iColor.AlphaF() ) );
    }
ULIS_SPEC_PIXEL_ACCESSOR_END

ULIS_SPEC_PIXEL_ACCESSOR_START( HSL )
    ULIS_SPEC_COMPONENT( Hue,           0   )
    ULIS_SPEC_COMPONENT( Saturation,    1   )
    ULIS_SPEC_COMPONENT( Lightness,     2   )
    ULIS_SPEC_COMPONENT( H,             0   )
    ULIS_SPEC_COMPONENT( S,             1   )
    ULIS_SPEC_COMPONENT( L,             2   )
    inline  CColor  GetColor()  const {
        return  CColor::FromHSLF( ConvType< tPixelType, float >( H() )
                                , ConvType< tPixelType, float >( S() )
                                , ConvType< tPixelType, float >( L() )
                                , ConvType< tPixelType, float >( tSuperClass::GetAlpha() ) );
    }

    inline  void    SetColor( const CColor& iColor ) {
        H() = ConvType< float, tPixelType >( iColor.HSLHueF() );
        S() = ConvType< float, tPixelType >( iColor.HSLSaturationF() );
        L() = ConvType< float, tPixelType >( iColor.LightnessF() );
        tSuperClass::SetAlpha( ConvType< float, tPixelType >( iColor.AlphaF() ) );
    }
ULIS_SPEC_PIXEL_ACCESSOR_END

ULIS_SPEC_PIXEL_ACCESSOR_START( HSV )
    ULIS_SPEC_COMPONENT( Hue,           0   )
    ULIS_SPEC_COMPONENT( Saturation,    1   )
    ULIS_SPEC_COMPONENT( Value,         2   )
    ULIS_SPEC_COMPONENT( H,             0   )
    ULIS_SPEC_COMPONENT( S,             1   )
    ULIS_SPEC_COMPONENT( V,             2   )
    inline  CColor  GetColor()  const {
        return  CColor::FromHSVF( ConvType< tPixelType, float >( H() )
                                , ConvType< tPixelType, float >( S() )
                                , ConvType< tPixelType, float >( V() )
                                , ConvType< tPixelType, float >( tSuperClass::GetAlpha() ) );
    }

    inline  void    SetColor( const CColor& iColor ) {
        H() = ConvType< float, tPixelType >( iColor.HSVHueF() );
        S() = ConvType< float, tPixelType >( iColor.HSVSaturationF() );
        V() = ConvType< float, tPixelType >( iColor.ValueF() );
        tSuperClass::SetAlpha( ConvType< float, tPixelType >( iColor.AlphaF() ) );
    }
ULIS_SPEC_PIXEL_ACCESSOR_END

ULIS_SPEC_PIXEL_ACCESSOR_START( CMYK )
    ULIS_SPEC_COMPONENT( Cyan,      0       )
    ULIS_SPEC_COMPONENT( Magenta,   1       )
    ULIS_SPEC_COMPONENT( Yellow,    2       )
    ULIS_SPEC_COMPONENT( Key,       3       )
    ULIS_SPEC_COMPONENT( Black,     3       )
    ULIS_SPEC_COMPONENT( C,         0       )
    ULIS_SPEC_COMPONENT( M,         1       )
    ULIS_SPEC_COMPONENT( Y,         2       )
    ULIS_SPEC_COMPONENT( K,         3       )
    inline  CColor  GetColor()  const {
        return  CColor::FromCMYKF( ConvType< tPixelType, float >( C() )
                                 , ConvType< tPixelType, float >( M() )
                                 , ConvType< tPixelType, float >( Y() )
                                 , ConvType< tPixelType, float >( K() )
                                 , ConvType< tPixelType, float >( tSuperClass::GetAlpha() ) );
    }

    inline  void    SetColor( const CColor& iColor ) {
        C() = ConvType< float, tPixelType >( iColor.CyanF() );
        M() = ConvType< float, tPixelType >( iColor.MagentaF() );
        Y() = ConvType< float, tPixelType >( iColor.YellowF() );
        K() = ConvType< float, tPixelType >( iColor.KeyF() );
        tSuperClass::SetAlpha( ConvType< float, tPixelType >( iColor.AlphaF() ) );
    }
ULIS_SPEC_PIXEL_ACCESSOR_END

ULIS_SPEC_PIXEL_ACCESSOR_START( Lab )
    ULIS_SPEC_COMPONENT( L, 0               )
    ULIS_SPEC_COMPONENT( a, 1               )
    ULIS_SPEC_COMPONENT( b, 2               )
    inline CColor GetColor()  const         { return  CColor( eCColorModel::kInvalid );    }
    inline void   SetColor( const CColor& ) { memset( tSuperClass::d, 0, tSpec::_nf._pd ); }
ULIS_SPEC_PIXEL_ACCESSOR_END

ULIS_SPEC_PIXEL_ACCESSOR_START( XYZ )
    ULIS_SPEC_COMPONENT( X, 0               )
    ULIS_SPEC_COMPONENT( Y, 1               )
    ULIS_SPEC_COMPONENT( Z, 2               )
    inline CColor GetColor()  const         { return  CColor( eCColorModel::kInvalid );    }
    inline void   SetColor( const CColor& ) { memset( tSuperClass::d, 0, tSpec::_nf._pd ); }
ULIS_SPEC_PIXEL_ACCESSOR_END

/////////////////////////////////////////////////////
// Forward Declarations
template< uint32_t _SH > class TPixelValue;
template< uint32_t _SH > class TPixelProxy;

/////////////////////////////////////////////////////
// TPixelValue
template< uint32_t _SH >
class TPixelValue final : public TPixelAcessor< _SH, tSpec::_nf._cm >
{
    typedef TPixelAcessor< _SH, tSpec::_nf._cm > tSuperClass;
    ULIS_FORWARD_TYPE_SELECTOR

public:
    // Construction / Destruction
    TPixelValue()
    {
        tSuperClass::d = new tPixelType[ tSpec::_nf._pd ];
        memset( tSuperClass::d, 0, tSpec::_nf._pd );
        tSuperClass::profile = nullptr;
    }

    TPixelValue( const TPixelProxy< _SH >& iProxy )
    {
        tSuperClass::d = new tPixelType[ tSpec::_nf._pd ];
        memcpy( tSuperClass::d, iProxy.Ptr(), tSpec::_nf._pd );
        tSuperClass::profile = nullptr;
    }

    virtual ~TPixelValue() {
        delete  tSuperClass::d;
        tSuperClass::profile = nullptr;
    }
};

/////////////////////////////////////////////////////
// TPixelProxy
template< uint32_t _SH >
class TPixelProxy final : public TPixelAcessor< _SH, tSpec::_nf._cm >
{
    typedef TPixelAcessor< _SH, tSpec::_nf._cm > tSuperClass;
    ULIS_FORWARD_TYPE_SELECTOR

public:
    // Construction / Destruction
    TPixelProxy( uint8* iPtr )
    {
        tSuperClass::d = (tPixelType*)iPtr;
        tSuperClass::profile = nullptr;
    }

    virtual ~TPixelProxy() {
        tSuperClass::profile = nullptr;
    }

public:
    // Public API
    inline  TPixelProxy< _SH >& operator=( const TPixelValue< _SH >& iOther ) { memcpy( tSuperClass::d, iOther.Ptr(), tSpec::_nf._pd ); return *this; }
};

/////////////////////////////////////////////////////
// Undefines
#undef tSpec
#undef tLayout

} // namespace ULIS

