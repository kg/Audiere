#ifndef ENDIAN_HPP
#define ENDIAN_HPP


// check for GNU autoconf definitions
#ifdef WORDS_BIGENDIAN
#  define BIG_ENDIAN
#else
#  ifndef LITTLE_ENDIAN
#    define LITTLE_ENDIAN
#  endif
#endif


#include "config.h"


// these aren't implemented in the nicest ways...
template<typename T>
void Swap(T& a, T& b) {
  T c = a;
  a = b;
  b = c;
}


inline adr_u32 SwapEndianness32(adr_u32 value) {
  adr_u8* u8 = (adr_u8*)value;
  Swap(u8[0], u8[3]);
  Swap(u8[1], u8[2]);
  return value;
}


#ifdef LITTLE_ENDIAN


  inline adr_u32 LittleToHost32(adr_u32 value) {
    return value;
  }

  inline adr_u32 BigToHost32(adr_u32 value) {
    return SwapEndianness32(value);
  }


#elif defined(BIG_ENDIAN)


  inline adr_u32 LittleToHost32(adr_u32 value) {
    return SwapEndianness32(value);
  }

  inline adr_u32 BigToHost32(adr_u32 value) {
    return value;
  }


#else

  #error Define LITTLE_ENDIAN or BIG_ENDIAN!

#endif


#endif
