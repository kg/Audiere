#ifndef UTILITY_HPP
#define UTILITY_HPP


#include <list>
#include <string>
#include <utility>
#include "audiere.h"
#include "types.h"


namespace audiere {

  // we can't use std::min or std::max in VC++ 6, so define our own

  template<typename T>
  inline T Min(T a, T b) {
    return (a < b ? a : b);
  }

  template<typename T>
  inline T Max(T a, T b) {
    return (a > b ? a : b);
  }

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


  inline int GetBytesPerSample(SampleFormat format) {
    switch (format) {
      case SF_U8:     return 1;
      case SF_S16_LE: return 2;
      default:        return 0;
    }
  }

}


#endif
