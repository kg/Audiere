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


#include <algorithm>
#include "types.h"


namespace audiere {

  inline u32 SwapEndianness32(u32 value) {
    u8* u = (u8*)&value;
    std::swap(u[0], u[3]);
    std::swap(u[1], u[2]);
    return value;
  }


  #ifdef LITTLE_ENDIAN

    inline u32 LittleToHost32(u32 value) {
      return value;
    }

    inline u32 BigToHost32(u32 value) {
      return SwapEndianness32(value);
    }

  #elif defined(BIG_ENDIAN)

    inline u32 LittleToHost32(u32 value) {
      return SwapEndianness32(value);
    }

    inline u32 BigToHost32(u32 value) {
      return value;
    }

  #else

    #error Define LITTLE_ENDIAN or BIG_ENDIAN!

  #endif

}

#endif
