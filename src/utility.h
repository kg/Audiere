#ifndef UTILITY_HPP
#define UTILITY_HPP


#include <list>
#include <string>
#include <utility>
#include "audiere.h"
#include "types.h"


#if defined(_MSC_VER) && _MSC_VER <= 1200

  // std::min and std::max are broken in VC++ 6, so define our own.
  // Unfortunately, this means we must include utility.h to use
  // std::min and std::max
  namespace std {

    #ifdef min
      #undef min
    #endif

    #ifdef max
      #undef max
    #endif

    template<typename T>
    inline T min(T a, T b) {
      return (a < b ? a : b);
    }

    template<typename T>
    inline T max(T a, T b) {
      return (a > b ? a : b);
    }
  }

#else

  #include <algorithm>

#endif


namespace audiere {


  typedef std::pair<std::string, std::string> Parameter;
  typedef std::list<Parameter> ParameterList;

  ParameterList ParseParameters(const char* parameters_string);

  int strcmp_case(const char* a, const char* b);


  inline u16 read16_le(const u8* b) {
    return b[0] + (b[1] << 8);
  }

  inline u16 read16_be(const u8* b) {
    return (b[0] << 8) + b[1];
  }

  inline u32 read32_le(const u8* b) {
    return read16_le(b) + (read16_le(b + 2) << 16);
  }

  inline u32 read32_be(const u8* b) {
    return (read16_be(b) << 16) + read16_be(b + 2);
  }


  class UnseekableSource : public DLLImplementation<SampleSource> {
  public:
    bool isSeekable() {
      return false;
    }

    int getLength() {
      return 0;
    }

    void setPosition(int /*position*/) {
    }

    int getPosition() {
      return 0;
    }
  };

}


#endif
