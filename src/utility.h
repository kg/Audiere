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
    int  ADR_CALL getLength()                   { return 0;     }
    void ADR_CALL setPosition(int /*position*/) {               }
    int  ADR_CALL getPosition()                 { return 0;     }
  };


  inline SampleSource* OpenBufferStream(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return CreateSampleBuffer(
      samples, sample_count,
      channel_count, sample_rate, sample_format)->openStream();
  }


  class QueueBuffer {
  public:
    QueueBuffer() {
      m_buffer_capacity = 256;
      m_buffer_size = 0;

      m_buffer = (u8*)malloc(m_buffer_capacity);
    }

    ~QueueBuffer() {
      m_buffer = (u8*)realloc(m_buffer, 0);
    }

    int getSize() {
      return m_buffer_size;
    }

    void write(void* buffer, int size) {
      bool need_realloc = false;
      while (size + m_buffer_size > m_buffer_capacity) {
        m_buffer_capacity *= 2;
        need_realloc = true;
      }

      if (need_realloc) {
        m_buffer = (u8*)realloc(m_buffer, m_buffer_capacity);
      }

      memcpy(m_buffer + m_buffer_size, buffer, size);
      m_buffer_size += size;
    }

    int read(void* buffer, int size) {
      int to_read = std::min(size, m_buffer_size);
      memcpy(buffer, m_buffer, to_read);
      memmove(m_buffer, m_buffer + to_read, m_buffer_size - to_read);
      m_buffer_size -= to_read;
      return to_read;
    }

  private:
    u8* m_buffer;
    int m_buffer_capacity;
    int m_buffer_size;
  };

}


#endif
