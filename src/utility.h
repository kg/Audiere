#ifndef UTILITY_H
#define UTILITY_H


#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <map>
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

  class ParameterList {
  public:
    ParameterList(const char* parameters);
    std::string getValue(std::string key, std::string defValue) const;

  private:
    std::map<std::string, std::string> m_values;
  };

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


  class UnseekableSource : public RefImplementation<SampleSource> {
  public:
    bool ADR_CALL isSeekable()                  { return false; }
    int ADR_CALL getLength()                    { return 0;     }
    void ADR_CALL setPosition(int /*position*/) {               }
    int ADR_CALL getPosition()                  { return 0;     }
  };


  inline SampleSource* OpenBufferStream(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return CreateSampleBuffer(
      samples, sample_count,
      channel_count, sample_rate, sample_format)->openStream();
  }

}


#endif
