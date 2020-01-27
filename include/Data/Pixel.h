// Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000
/**
*
*   ULIS2
*__________________
*
* @file         Pixel.h
* @author       Clement Berthaud
* @brief        This file provides the declaration for the Pixel classes.
* @copyright    Copyright � 2018-2020 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
#pragma once
#include "Base/Core.h"

ULIS2_NAMESPACE_BEGIN
class IPixel;
class FPixelValue;
class FPixelProxy;
class FColorProfile;
/////////////////////////////////////////////////////
/// @class      IPixel
/// @brief      The pure virtual IPixel class provides a mean of storing and manipulating pixels in various formats.
/// @details    The pixel format, type, layout, and other informations can be retrieved in the pixel format member.
class ULIS2_API IPixel
{
public:
    // Construction / Destruction
    virtual ~IPixel() = 0;
    IPixel( uint32 iFormat, FColorProfile* iProfile = nullptr );

public:
    // Public API
    tByte*          Ptr();
    void            AssignProfile( FColorProfile* iProfile );
    const tByte*    Ptr()               const;
    uint8           BytesPerSample()    const;
    uint8           Depth()             const;
    tFormat         Format()            const;
    eColorModel     Model()             const;
    eType           Type()              const;
    bool            HasAlpha()          const;
    bool            Swapped()           const;
    bool            Reversed()          const;
    uint8           NumSamples()        const;
    uint8           NumColorChannels()  const;
    FColorProfile*  Profile()           const;
    uint8           RedirectedIndex( uint8 iIndex ) const;
    uint8           AlphaIndex()                    const;

    void AssignMemoryUnsafe( const IPixel& iOther );

public:
    // Generic access API
    tByte*                              SamplePtr( uint8 iIndex );
    template< typename T > T*           SamplePtrT( uint8 iIndex );
    const tByte*                        SamplePtr( uint8 iIndex )               const;
    template< typename T > const T*     SamplePtrT( uint8 iIndex )              const;
    template< typename T > T            GetValueRaw( uint8 iIndex )             const;
    template< typename T > void         SetValueRaw( uint8 iIndex, T iValue  ) ;
    template< typename T > T&           GetRefRaw( uint8 iIndex );
    template< typename T > const T&     GetConstRefRaw( uint8 iIndex )          const;
    template< typename T > T            GetValue( uint8 iIndex )                const;
    template< typename T > T&           GetRef( uint8 iIndex );
    template< typename T > const T&     GetConstRef( uint8 iIndex )             const;
    template< typename T > void         SetValue( uint8 iIndex, T iValue );

public:
    // Model Access API
    template< typename T > ULIS2_FORCEINLINE T Alpha() const                     { return  HasAlpha() ? GetValueRaw< T >( AlphaIndex() ) : MaxType< T >(); }
    template< typename T > ULIS2_FORCEINLINE T A() const                         { return  HasAlpha() ? GetValueRaw< T >( AlphaIndex() ) : MaxType< T >(); }
    template< typename T > ULIS2_FORCEINLINE T Grey() const                      { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T Red() const                       { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T Green() const                     { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T Blue() const                      { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T R() const                         { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T G() const                         { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T B() const                         { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T Hue() const                       { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T Saturation() const                { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T Lightness() const                 { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T Value() const                     { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T Cyan() const                      { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T Magenta() const                   { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T Yellow() const                    { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T Key() const                       { return  GetValue< T >( 3 );                   }
    template< typename T > ULIS2_FORCEINLINE T Luma() const                      { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T U() const                         { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T V() const                         { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T K() const                         { return  GetValue< T >( 3 );                   }
    template< typename T > ULIS2_FORCEINLINE T X() const                         { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T Y() const                         { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T Z() const                         { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T L() const                         { return  GetValue< T >( 0 );                   }
    template< typename T > ULIS2_FORCEINLINE T a() const                         { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T b() const                         { return  GetValue< T >( 2 );                   }
    template< typename T > ULIS2_FORCEINLINE T x() const                         { return  GetValue< T >( 1 );                   }
    template< typename T > ULIS2_FORCEINLINE T y() const                         { return  GetValue< T >( 2 );                   }
    ULIS2_FORCEINLINE uint8 Alpha8() const                                      { return  Alpha< uint8 >();                     }
    ULIS2_FORCEINLINE uint8 A8() const                                          { return  A< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 Grey8() const                                       { return  Grey< uint8 >();                      }
    ULIS2_FORCEINLINE uint8 Red8() const                                        { return  Red< uint8 >();                       }
    ULIS2_FORCEINLINE uint8 Green8() const                                      { return  Green< uint8 >();                     }
    ULIS2_FORCEINLINE uint8 Blue8() const                                       { return  Blue< uint8 >();                      }
    ULIS2_FORCEINLINE uint8 R8() const                                          { return  R< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 G8() const                                          { return  G< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 B8() const                                          { return  B< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 Hue8() const                                        { return  Hue< uint8 >();                       }
    ULIS2_FORCEINLINE uint8 Saturation8() const                                 { return  Saturation< uint8 >();                }
    ULIS2_FORCEINLINE uint8 Lightness8() const                                  { return  Lightness< uint8 >();                 }
    ULIS2_FORCEINLINE uint8 Value8() const                                      { return  Value< uint8 >();                     }
    ULIS2_FORCEINLINE uint8 Cyan8() const                                       { return  Cyan< uint8 >();                      }
    ULIS2_FORCEINLINE uint8 Magenta8() const                                    { return  Magenta< uint8 >();                   }
    ULIS2_FORCEINLINE uint8 Yellow8() const                                     { return  Yellow< uint8 >();                    }
    ULIS2_FORCEINLINE uint8 Key8() const                                        { return  Key< uint8 >();                       }
    ULIS2_FORCEINLINE uint8 Luma8() const                                       { return  Luma< uint8 >();                      }
    ULIS2_FORCEINLINE uint8 U8() const                                          { return  U< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 V8() const                                          { return  V< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 K8() const                                          { return  K< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 X8() const                                          { return  X< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 Y8() const                                          { return  Y< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 Z8() const                                          { return  Z< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 L8() const                                          { return  L< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 a8() const                                          { return  a< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 b8() const                                          { return  b< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 x8() const                                          { return  x< uint8 >();                         }
    ULIS2_FORCEINLINE uint8 y8() const                                          { return  y< uint8 >();                         }
    ULIS2_FORCEINLINE uint16 Alpha16() const                                    { return  Alpha< uint16 >();                    }
    ULIS2_FORCEINLINE uint16 A16() const                                        { return  A< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 Grey16() const                                     { return  Grey< uint16 >();                     }
    ULIS2_FORCEINLINE uint16 Red16() const                                      { return  Red< uint16 >();                      }
    ULIS2_FORCEINLINE uint16 Green16() const                                    { return  Green< uint16 >();                    }
    ULIS2_FORCEINLINE uint16 Blue16() const                                     { return  Blue< uint16 >();                     }
    ULIS2_FORCEINLINE uint16 R16() const                                        { return  R< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 G16() const                                        { return  G< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 B16() const                                        { return  B< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 Hue16() const                                      { return  Hue< uint16 >();                      }
    ULIS2_FORCEINLINE uint16 Saturation16() const                               { return  Saturation< uint16 >();               }
    ULIS2_FORCEINLINE uint16 Lightness16() const                                { return  Lightness< uint16 >();                }
    ULIS2_FORCEINLINE uint16 Value16() const                                    { return  Value< uint16 >();                    }
    ULIS2_FORCEINLINE uint16 Cyan16() const                                     { return  Cyan< uint16 >();                     }
    ULIS2_FORCEINLINE uint16 Magenta16() const                                  { return  Magenta< uint16 >();                  }
    ULIS2_FORCEINLINE uint16 Yellow16() const                                   { return  Yellow< uint16 >();                   }
    ULIS2_FORCEINLINE uint16 Key16() const                                      { return  Key< uint16 >();                      }
    ULIS2_FORCEINLINE uint16 Luma16() const                                     { return  Luma< uint16 >();                     }
    ULIS2_FORCEINLINE uint16 U16() const                                        { return  U< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 V16() const                                        { return  V< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 K16() const                                        { return  K< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 X16() const                                        { return  X< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 Y16() const                                        { return  Y< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 Z16() const                                        { return  Z< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 L16() const                                        { return  L< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 a16() const                                        { return  a< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 b16() const                                        { return  b< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 x16() const                                        { return  x< uint16 >();                        }
    ULIS2_FORCEINLINE uint16 y16() const                                        { return  y< uint16 >();                        }
    ULIS2_FORCEINLINE uint32 Alpha32() const                                    { return  Alpha< uint32 >();                    }
    ULIS2_FORCEINLINE uint32 A32() const                                        { return  A< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 Grey32() const                                     { return  Grey< uint32 >();                     }
    ULIS2_FORCEINLINE uint32 Red32() const                                      { return  Red< uint32 >();                      }
    ULIS2_FORCEINLINE uint32 Green32() const                                    { return  Green< uint32 >();                    }
    ULIS2_FORCEINLINE uint32 Blue32() const                                     { return  Blue< uint32 >();                     }
    ULIS2_FORCEINLINE uint32 R32() const                                        { return  R< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 G32() const                                        { return  G< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 B32() const                                        { return  B< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 Hue32() const                                      { return  Hue< uint32 >();                      }
    ULIS2_FORCEINLINE uint32 Saturation32() const                               { return  Saturation< uint32 >();               }
    ULIS2_FORCEINLINE uint32 Lightness32() const                                { return  Lightness< uint32 >();                }
    ULIS2_FORCEINLINE uint32 Value32() const                                    { return  Value< uint32 >();                    }
    ULIS2_FORCEINLINE uint32 Cyan32() const                                     { return  Cyan< uint32 >();                     }
    ULIS2_FORCEINLINE uint32 Magenta32() const                                  { return  Magenta< uint32 >();                  }
    ULIS2_FORCEINLINE uint32 Yellow32() const                                   { return  Yellow< uint32 >();                   }
    ULIS2_FORCEINLINE uint32 Key32() const                                      { return  Key< uint32 >();                      }
    ULIS2_FORCEINLINE uint32 Luma32() const                                     { return  Luma< uint32 >();                     }
    ULIS2_FORCEINLINE uint32 U32() const                                        { return  U< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 V32() const                                        { return  V< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 K32() const                                        { return  K< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 X32() const                                        { return  X< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 Y32() const                                        { return  Y< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 Z32() const                                        { return  Z< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 L32() const                                        { return  L< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 a32() const                                        { return  a< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 b32() const                                        { return  b< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 x32() const                                        { return  x< uint32 >();                        }
    ULIS2_FORCEINLINE uint32 y32() const                                        { return  y< uint32 >();                        }
    ULIS2_FORCEINLINE float AlphaF() const                                      { return  Alpha< float >();                     }
    ULIS2_FORCEINLINE float AF() const                                          { return  A< float >();                         }
    ULIS2_FORCEINLINE float GreyF() const                                       { return  Grey< float >();                      }
    ULIS2_FORCEINLINE float RedF() const                                        { return  Red< float >();                       }
    ULIS2_FORCEINLINE float GreenF() const                                      { return  Green< float >();                     }
    ULIS2_FORCEINLINE float BlueF() const                                       { return  Blue< float >();                      }
    ULIS2_FORCEINLINE float RF() const                                          { return  R< float >();                         }
    ULIS2_FORCEINLINE float GF() const                                          { return  G< float >();                         }
    ULIS2_FORCEINLINE float BF() const                                          { return  B< float >();                         }
    ULIS2_FORCEINLINE float HueF() const                                        { return  Hue< float >();                       }
    ULIS2_FORCEINLINE float SaturationF() const                                 { return  Saturation< float >();                }
    ULIS2_FORCEINLINE float LightnessF() const                                  { return  Lightness< float >();                 }
    ULIS2_FORCEINLINE float ValueF() const                                      { return  Value< float >();                     }
    ULIS2_FORCEINLINE float CyanF() const                                       { return  Cyan< float >();                      }
    ULIS2_FORCEINLINE float MagentaF() const                                    { return  Magenta< float >();                   }
    ULIS2_FORCEINLINE float YellowF() const                                     { return  Yellow< float >();                    }
    ULIS2_FORCEINLINE float KeyF() const                                        { return  Key< float >();                       }
    ULIS2_FORCEINLINE float LumaF() const                                       { return  Luma< float >();                      }
    ULIS2_FORCEINLINE float UF() const                                          { return  U< float >();                         }
    ULIS2_FORCEINLINE float VF() const                                          { return  V< float >();                         }
    ULIS2_FORCEINLINE float KF() const                                          { return  K< float >();                         }
    ULIS2_FORCEINLINE float XF() const                                          { return  X< float >();                         }
    ULIS2_FORCEINLINE float YF() const                                          { return  Y< float >();                         }
    ULIS2_FORCEINLINE float ZF() const                                          { return  Z< float >();                         }
    ULIS2_FORCEINLINE float LF() const                                          { return  L< float >();                         }
    ULIS2_FORCEINLINE float aF() const                                          { return  a< float >();                         }
    ULIS2_FORCEINLINE float bF() const                                          { return  b< float >();                         }
    ULIS2_FORCEINLINE float xF() const                                          { return  x< float >();                         }
    ULIS2_FORCEINLINE float yF() const                                          { return  y< float >();                         }
    ULIS2_FORCEINLINE double AlphaD() const                                     { return  Alpha< double >();                    }
    ULIS2_FORCEINLINE double AD() const                                         { return  A< double >();                        }
    ULIS2_FORCEINLINE double GreyD() const                                      { return  Grey< double >();                     }
    ULIS2_FORCEINLINE double RedD() const                                       { return  Red< double >();                      }
    ULIS2_FORCEINLINE double GreenD() const                                     { return  Green< double >();                    }
    ULIS2_FORCEINLINE double BlueD() const                                      { return  Blue< double >();                     }
    ULIS2_FORCEINLINE double RD() const                                         { return  R< double >();                        }
    ULIS2_FORCEINLINE double GD() const                                         { return  G< double >();                        }
    ULIS2_FORCEINLINE double BD() const                                         { return  B< double >();                        }
    ULIS2_FORCEINLINE double HueD() const                                       { return  Hue< double >();                      }
    ULIS2_FORCEINLINE double SaturationD() const                                { return  Saturation< double >();               }
    ULIS2_FORCEINLINE double LightnessD() const                                 { return  Lightness< double >();                }
    ULIS2_FORCEINLINE double ValueD() const                                     { return  Value< double >();                    }
    ULIS2_FORCEINLINE double CyanD() const                                      { return  Cyan< double >();                     }
    ULIS2_FORCEINLINE double MagentaD() const                                   { return  Magenta< double >();                  }
    ULIS2_FORCEINLINE double YellowD() const                                    { return  Yellow< double >();                   }
    ULIS2_FORCEINLINE double KeyD() const                                       { return  Key< double >();                      }
    ULIS2_FORCEINLINE double LumaD() const                                      { return  Luma< double >();                     }
    ULIS2_FORCEINLINE double UD() const                                         { return  U< double >();                        }
    ULIS2_FORCEINLINE double VD() const                                         { return  V< double >();                        }
    ULIS2_FORCEINLINE double KD() const                                         { return  K< double >();                        }
    ULIS2_FORCEINLINE double XD() const                                         { return  X< double >();                        }
    ULIS2_FORCEINLINE double YD() const                                         { return  Y< double >();                        }
    ULIS2_FORCEINLINE double ZD() const                                         { return  Z< double >();                        }
    ULIS2_FORCEINLINE double LD() const                                         { return  L< double >();                        }
    ULIS2_FORCEINLINE double aD() const                                         { return  a< double >();                        }
    ULIS2_FORCEINLINE double bD() const                                         { return  b< double >();                        }
    ULIS2_FORCEINLINE double xD() const                                         { return  x< double >();                        }
    ULIS2_FORCEINLINE double yD() const                                         { return  y< double >();                        }

    template< typename T > ULIS2_FORCEINLINE void SetAlpha(      T iValue )      { if( HasAlpha() ) SetValueRaw< T >( AlphaIndex(), iValue ); }
    template< typename T > ULIS2_FORCEINLINE void SetA(          T iValue )      { if( HasAlpha() ) SetValueRaw< T >( AlphaIndex(), iValue );     }
    template< typename T > ULIS2_FORCEINLINE void SetGrey(       T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetRed(        T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetGreen(      T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetBlue(       T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetR(          T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetG(          T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetB(          T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetHue(        T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetSaturation( T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetLightness(  T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetValue(      T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetCyan(       T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetMagenta(    T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetYellow(     T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetKey(        T iValue )      { SetValue< T >( 3, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetLuma(       T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetU(          T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetV(          T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetK(          T iValue )      { SetValue< T >( 3, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetX(          T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetY(          T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetZ(          T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void SetL(          T iValue )      { SetValue< T >( 0, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void Seta(          T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void Setb(          T iValue )      { SetValue< T >( 2, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void Setx(          T iValue )      { SetValue< T >( 1, iValue );                   }
    template< typename T > ULIS2_FORCEINLINE void Sety(          T iValue )      { SetValue< T >( 2, iValue );                   }
    ULIS2_FORCEINLINE void SetAlpha8(                           uint8 iValue )  { SetAlpha< uint8 >( iValue );                  }
    ULIS2_FORCEINLINE void SetA8(                               uint8 iValue )  { SetA< uint8 >( iValue );                      }
    ULIS2_FORCEINLINE void SetGrey8(                            uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetRed8(                             uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetGreen8(                           uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetBlue8(                            uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetR8(                               uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetG8(                               uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetB8(                               uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetHue8(                             uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetSaturation8(                      uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetLightness8(                       uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetValue8(                           uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetCyan8(                            uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetMagenta8(                         uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetYellow8(                          uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetKey8(                             uint8 iValue )  { SetValue< uint8 >( 3, iValue );               }
    ULIS2_FORCEINLINE void SetLuma8(                            uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetU8(                               uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetV8(                               uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetK8(                               uint8 iValue )  { SetValue< uint8 >( 3, iValue );               }
    ULIS2_FORCEINLINE void SetX8(                               uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetY8(                               uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetZ8(                               uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetL8(                               uint8 iValue )  { SetValue< uint8 >( 0, iValue );               }
    ULIS2_FORCEINLINE void Seta8(                               uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void Setb8(                               uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void Setx8(                               uint8 iValue )  { SetValue< uint8 >( 1, iValue );               }
    ULIS2_FORCEINLINE void Sety8(                               uint8 iValue )  { SetValue< uint8 >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetAlpha16(                          uint16 iValue ) { SetAlpha< uint16 >( iValue );                 }
    ULIS2_FORCEINLINE void SetA16(                              uint16 iValue ) { SetA< uint16 >( iValue );                     }
    ULIS2_FORCEINLINE void SetGrey16(                           uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetRed16(                            uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetGreen16(                          uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetBlue16(                           uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetR16(                              uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetG16(                              uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetB16(                              uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetHue16(                            uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetSaturation16(                     uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetLightness16(                      uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetValue16(                          uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetCyan16(                           uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetMagenta16(                        uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetYellow16(                         uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetKey16(                            uint16 iValue ) { SetValue< uint16 >( 3, iValue );              }
    ULIS2_FORCEINLINE void SetLuma16(                           uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetU16(                              uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetV16(                              uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetK16(                              uint16 iValue ) { SetValue< uint16 >( 3, iValue );              }
    ULIS2_FORCEINLINE void SetX16(                              uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetY16(                              uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetZ16(                              uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetL16(                              uint16 iValue ) { SetValue< uint16 >( 0, iValue );              }
    ULIS2_FORCEINLINE void Seta16(                              uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void Setb16(                              uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void Setx16(                              uint16 iValue ) { SetValue< uint16 >( 1, iValue );              }
    ULIS2_FORCEINLINE void Sety16(                              uint16 iValue ) { SetValue< uint16 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetAlpha32(                          uint32 iValue ) { SetAlpha< uint32 >( iValue );                 }
    ULIS2_FORCEINLINE void SetA32(                              uint32 iValue ) { SetA< uint32 >( iValue );                     }
    ULIS2_FORCEINLINE void SetGrey32(                           uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetRed32(                            uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetGreen32(                          uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetBlue32(                           uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetR32(                              uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetG32(                              uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetB32(                              uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetHue32(                            uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetSaturation32(                     uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetLightness32(                      uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetValue32(                          uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetCyan32(                           uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetMagenta32(                        uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetYellow32(                         uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetKey32(                            uint32 iValue ) { SetValue< uint32 >( 3, iValue );              }
    ULIS2_FORCEINLINE void SetLuma32(                           uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetU32(                              uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetV32(                              uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetK32(                              uint32 iValue ) { SetValue< uint32 >( 3, iValue );              }
    ULIS2_FORCEINLINE void SetX32(                              uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetY32(                              uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetZ32(                              uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetL32(                              uint32 iValue ) { SetValue< uint32 >( 0, iValue );              }
    ULIS2_FORCEINLINE void Seta32(                              uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void Setb32(                              uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void Setx32(                              uint32 iValue ) { SetValue< uint32 >( 1, iValue );              }
    ULIS2_FORCEINLINE void Sety32(                              uint32 iValue ) { SetValue< uint32 >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetAlphaF(                           float iValue )  { SetAlpha< float >( iValue );                  }
    ULIS2_FORCEINLINE void SetAF(                               float iValue )  { SetA< float >( iValue );                      }
    ULIS2_FORCEINLINE void SetGreyF(                            float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetRedF(                             float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetGreenF(                           float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetBlueF(                            float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetRF(                               float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetGF(                               float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetBF(                               float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetHueF(                             float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetSaturationF(                      float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetLightnessF(                       float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetValueF(                           float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetCyanF(                            float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetMagentaF(                         float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetYellowF(                          float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetKeyF(                             float iValue )  { SetValue< float >( 3, iValue );               }
    ULIS2_FORCEINLINE void SetLumaF(                            float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetUF(                               float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetVF(                               float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetKF(                               float iValue )  { SetValue< float >( 3, iValue );               }
    ULIS2_FORCEINLINE void SetXF(                               float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetYF(                               float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetZF(                               float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetLF(                               float iValue )  { SetValue< float >( 0, iValue );               }
    ULIS2_FORCEINLINE void SetaF(                               float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetbF(                               float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetxF(                               float iValue )  { SetValue< float >( 1, iValue );               }
    ULIS2_FORCEINLINE void SetyF(                               float iValue )  { SetValue< float >( 2, iValue );               }
    ULIS2_FORCEINLINE void SetAlphaD(                           double iValue ) { SetAlpha< double >( iValue );                 }
    ULIS2_FORCEINLINE void SetAD(                               double iValue ) { SetA< double >( iValue );                     }
    ULIS2_FORCEINLINE void SetGreyD(                            double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetRedD(                             double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetGreenD(                           double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetBlueD(                            double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetRD(                               double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetGD(                               double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetBD(                               double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetHueD(                             double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetSaturationD(                      double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetLightnessD(                       double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetValueD(                           double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetCyanD(                            double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetMagentaD(                         double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetYellowD(                          double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetKeyD(                             double iValue ) { SetValue< double >( 3, iValue );              }
    ULIS2_FORCEINLINE void SetLumaD(                            double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetUD(                               double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetVD(                               double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetKD(                               double iValue ) { SetValue< double >( 3, iValue );              }
    ULIS2_FORCEINLINE void SetXD(                               double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetYD(                               double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetZD(                               double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetLD(                               double iValue ) { SetValue< double >( 0, iValue );              }
    ULIS2_FORCEINLINE void SetaD(                               double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetbD(                               double iValue ) { SetValue< double >( 2, iValue );              }
    ULIS2_FORCEINLINE void SetxD(                               double iValue ) { SetValue< double >( 1, iValue );              }
    ULIS2_FORCEINLINE void SetyD(                               double iValue ) { SetValue< double >( 2, iValue );              }

protected:
    // Protected Data Members
    tByte*          mData;
    tFormat         mFormat;
    FColorProfile*  mProfile;
};

/////////////////////////////////////////////////////
/// @class      FPixelValue
/// @brief      The FPixelValue class provides a mean of storing and manipulating pixels value in various formats.
/// @details    A pixel value allocates its own storage space and is responsible for its memory.
class ULIS2_API FPixelValue : public IPixel
{
    typedef IPixel tParent;

public:
    // Construction / Destruction
    virtual ~FPixelValue();
    FPixelValue( uint32 iFormat, FColorProfile* iProfile = nullptr );
    FPixelValue( const tByte* iData, tFormat iFormat, FColorProfile* iProfile = nullptr );
    FPixelValue( const FPixelProxy& iProxy );

    template< typename T >
    FPixelValue( uint32 iFormat, std::initializer_list< T > iValues, FColorProfile* iProfile = nullptr )
        : tParent( iFormat, iProfile )
    {
        mData = new tByte[ Depth() ];
        switch( Type() )
        {
            case TYPE_UINT8:        Set_imp< T, uint8   >( iValues ); return;
            case TYPE_UINT16:       Set_imp< T, uint16  >( iValues ); return;
            case TYPE_UINT32:       Set_imp< T, uint32  >( iValues ); return;
            case TYPE_UFLOAT:       Set_imp< T, ufloat  >( iValues ); return;
            case TYPE_UDOUBLE:      Set_imp< T, udouble >( iValues ); return;
        }
    }

private:
    template< typename T1, typename T2 >
    ULIS2_FORCEINLINE void Set_imp( const std::initializer_list< T1 > iValues )
    {
        ULIS2_ASSERT( iValues.size() == NumSamples(), "Bad input values" );
        for( int i = 0; i < iValues.size(); ++i )
            SetValue< T2 >( i, ConvType< T1, T2 >( *(iValues.begin() + i) ) );
    }

public:
    // Named static constructors
    /*
    static FPixel FromGreyA8(   uint8  iGrey,   uint8  iA = UINT8_MAX   );
    static FPixel FromGreyA16(  uint16 iGrey,   uint16 iA = UINT16_MAX  );
    static FPixel FromGreyA32(  uint32 iGrey,   uint32 iA = UINT32_MAX  );
    static FPixel FromGreyAF(   float  iGrey,   float  iA = 1.f         );
    static FPixel FromGreyAD(   double iGrey,   double iA = 1.0         );
    static FPixel FromRGBA8(    uint8  iR,  uint8  iG,  uint8  iB,  uint8  iA = UINT8_MAX   );
    static FPixel FromRGBA16(   uint16 iR,  uint16 iG,  uint16 iB,  uint16 iA = UINT16_MAX  );
    static FPixel FromRGBA32(   uint32 iR,  uint32 iG,  uint32 iB,  uint32 iA = UINT32_MAX  );
    static FPixel FromRGBAF(    float  iR,  float  iG,  float  iB,  float  iA = 1.f         );
    static FPixel FromRGBAD(    double iR,  double iG,  double iB,  double iA = 1.0         );
    static FPixel FromHSVA8(    uint8  iH,  uint8  iS,  uint8  iV,  uint8  iA = UINT8_MAX   );
    static FPixel FromHSVA16(   uint16 iH,  uint16 iS,  uint16 iV,  uint16 iA = UINT16_MAX  );
    static FPixel FromHSVA32(   uint32 iH,  uint32 iS,  uint32 iV,  uint32 iA = UINT32_MAX  );
    static FPixel FromHSVAF(    float  iH,  float  iS,  float  iV,  float  iA = 1.f         );
    static FPixel FromHSVAD(    double iH,  double iS,  double iV,  double iA = 1.0         );
    static FPixel FromHSLA8(    uint8  iH,  uint8  iS,  uint8  iL,  uint8  iA = UINT8_MAX   );
    static FPixel FromHSLA16(   uint16 iH,  uint16 iS,  uint16 iL,  uint16 iA = UINT16_MAX  );
    static FPixel FromHSLA32(   uint32 iH,  uint32 iS,  uint32 iL,  uint32 iA = UINT32_MAX  );
    static FPixel FromHSLAF(    float  iH,  float  iS,  float  iL,  float  iA = 1.f         );
    static FPixel FromHSLAD(    double iH,  double iS,  double iL,  double iA = 1.0         );
    static FPixel FromCMYA8(    uint8  iC,  uint8  iM,  uint8  iY,  uint8  iA = UINT8_MAX   );
    static FPixel FromCMYA16(   uint16 iC,  uint16 iM,  uint16 iY,  uint16 iA = UINT16_MAX  );
    static FPixel FromCMYA32(   uint32 iC,  uint32 iM,  uint32 iY,  uint32 iA = UINT32_MAX  );
    static FPixel FromCMYAF(    float  iC,  float  iM,  float  iY,  float  iA = 1.f         );
    static FPixel FromCMYAD(    double iC,  double iM,  double iY,  double iA = 1.0         );
    static FPixel FromCMYKA8(   uint8  iC,  uint8  iM,  uint8  iY,  uint8  iK,  uint8  iA = UINT8_MAX   );
    static FPixel FromCMYKA16(  uint16 iC,  uint16 iM,  uint16 iY,  uint16 iK,  uint16 iA = UINT16_MAX  );
    static FPixel FromCMYKA32(  uint32 iC,  uint32 iM,  uint32 iY,  uint32 iK,  uint32 iA = UINT32_MAX  );
    static FPixel FromCMYKAF(   float  iC,  float  iM,  float  iY,  float  iK,  float  iA = 1.f         );
    static FPixel FromCMYKAD(   double iC,  double iM,  double iY,  double iK,  double iA = 1.0         );
    static FPixel FromYUVA8(    uint8  iY,  uint8  iU,  uint8  iV,  uint8  iA = UINT8_MAX   );
    static FPixel FromYUVA16(   uint16 iY,  uint16 iU,  uint16 iV,  uint16 iA = UINT16_MAX  );
    static FPixel FromYUVA32(   uint32 iY,  uint32 iU,  uint32 iV,  uint32 iA = UINT32_MAX  );
    static FPixel FromYUVAF(    float  iY,  float  iU,  float  iV,  float  iA = 1.f         );
    static FPixel FromYUVAD(    double iY,  double iU,  double iV,  double iA = 1.0         );
    static FPixel FromLabA8(    uint8  iL,  uint8  ia,  uint8  ib,  uint8  iA = UINT8_MAX   );
    static FPixel FromLabA16(   uint16 iL,  uint16 ia,  uint16 ib,  uint16 iA = UINT16_MAX  );
    static FPixel FromLabA32(   uint32 iL,  uint32 ia,  uint32 ib,  uint32 iA = UINT32_MAX  );
    static FPixel FromLabAF(    float  iL,  float  ia,  float  ib,  float  iA = 1.f         );
    static FPixel FromLabAD(    double iL,  double ia,  double ib,  double iA = 1.0         );
    static FPixel FromXYZA8(    uint8  iX,  uint8  iY,  uint8  iZ,  uint8  iA = UINT8_MAX   );
    static FPixel FromXYZA16(   uint16 iX,  uint16 iY,  uint16 iZ,  uint16 iA = UINT16_MAX  );
    static FPixel FromXYZA32(   uint32 iX,  uint32 iY,  uint32 iZ,  uint32 iA = UINT32_MAX  );
    static FPixel FromXYZAF(    float  iX,  float  iY,  float  iZ,  float  iA = 1.f         );
    static FPixel FromXYZAD(    double iX,  double iY,  double iZ,  double iA = 1.0         );
    static FPixel FromYxyA8(    uint8  iY,  uint8  ix,  uint8  iy,  uint8  iA = UINT8_MAX   );
    static FPixel FromYxyA16(   uint16 iY,  uint16 ix,  uint16 iy,  uint16 iA = UINT16_MAX  );
    static FPixel FromYxyA32(   uint32 iY,  uint32 ix,  uint32 iy,  uint32 iA = UINT32_MAX  );
    static FPixel FromYxyAF(    float  iY,  float  ix,  float  iy,  float  iA = 1.f         );
    static FPixel FromYxyAD(    double iY,  double ix,  double iy,  double iA = 1.0         );
    */
};
typedef FPixelValue FPixel;

/////////////////////////////////////////////////////
/// @class      FPixelProxy
/// @brief      The FPixelProxy class provides a mean of storing and manipulating pixels value in various formats.
/// @details    A pixel proxy is a view to a block memory space and is not responsible for this memory.
class ULIS2_API FPixelProxy : public IPixel
{
    typedef IPixel tParent;

public:
    // Construction / Destruction
    virtual ~FPixelProxy();
    FPixelProxy( tByte* iData, tFormat iFormat, FColorProfile* iProfile = nullptr );
    FPixelProxy( const tByte* iData, tFormat iFormat, FColorProfile* iProfile = nullptr );
    void SetPtr( tByte* iPtr );
    void SetPtr( const tByte* iPtr );
};

/////////////////////////////////////////////////////
// Template Instanciations
#define X_DO_A( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE i* IPixel::SamplePtrT< i >( uint8 );
#define X_DO_B( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE const i* IPixel::SamplePtrT< i >( uint8 ) const;
#define X_DO_C( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE i IPixel::GetValueRaw< i >( uint8 ) const;
#define X_DO_D( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE i& IPixel::GetRefRaw< i >( uint8 );
#define X_DO_E( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE const i& IPixel::GetConstRefRaw< i >( uint8 ) const;
#define X_DO_F( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE i IPixel::GetValue< i >( uint8 ) const;
#define X_DO_G( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE i& IPixel::GetRef< i >( uint8 );
#define X_DO_H( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE const i& IPixel::GetConstRef< i >( uint8 ) const;
#define X_DO_I( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE void IPixel::SetValueRaw< i >( uint8, i iValue );
#define X_DO_J( i, _E0, _E1, _E2, _E3 ) ULIS2_API_TEMPLATE void IPixel::SetValue< i >( uint8, i iValue );
ULIS2_FOR_ALL_TYPES_DO( X_DO_A, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_B, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_C, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_D, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_E, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_F, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_G, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_H, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_I, 0, 0, 0, 0 )
ULIS2_FOR_ALL_TYPES_DO( X_DO_J, 0, 0, 0, 0 )
#undef X_DO_A
#undef X_DO_B
#undef X_DO_C
#undef X_DO_D
#undef X_DO_E
#undef X_DO_F
#undef X_DO_G
#undef X_DO_H
#undef X_DO_I
#undef X_DO_J
ULIS2_NAMESPACE_END
