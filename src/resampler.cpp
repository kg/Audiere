#include "resampler.h"


namespace audiere {

  Resampler::Resampler(SampleSource* source) {
    m_source = source;
    m_source->getFormat(
      m_native_channel_count,
      m_native_sample_rate,
      m_native_sample_format);

    fillBuffer();
    resetState();
  }

  void
  Resampler::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = 2;
    sample_rate = 44100;
    sample_format = SF_S16_LE;
  }

  int
  Resampler::read(const int sample_count, void* samples) {
    u16* out = (u16*)samples;

    int left = sample_count;

    // if we didn't finish resampling last time...
    while (m_time > m_native_sample_rate && left > 0) {
      m_time -= m_native_sample_rate;
      *out++ = m_sl;
      *out++ = m_sr;
      --left;
    }

    while (left > 0) {

      if (m_samples_left == 0) {
        // try to fill the native buffer
        fillBuffer();

        // could we read any more?
        if (m_samples_left == 0) {
          return sample_count - left;
        }
      }

      m_sl = *m_position++;
      m_sr = *m_position++;
      --m_samples_left;

      m_time += 44100;
      while (m_time > m_native_sample_rate && left > 0) {
        m_time -= m_native_sample_rate;
        *out++ = m_sl;
        *out++ = m_sr;
        --left;
      }

    }
    return sample_count;
  }

  void
  Resampler::reset() {
    m_source->reset();
    fillBuffer();
    resetState();
  }


  inline s16 u8tos16(u8 u) {
    return (s16(u) - 127) * (1 << 8);
  }

  void
  Resampler::fillBuffer() {
    // we only support channels in [1, 2] and bits in [8, 16] now
    u8 initial_buffer[NATIVE_BUFFER_SIZE * 4];
    unsigned read = m_source->read(NATIVE_BUFFER_SIZE, initial_buffer);

    s16* out = m_native_buffer;

    if (m_native_channel_count == 1) {
      if (m_native_sample_format == SF_U8) {

        // channels = 1, bits = 8
        u8* in = initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          s16 sample = u8tos16(*in++);
          *out++ = sample;
          *out++ = sample;
        }

      } else {

        // channels = 1, bits = 16
        s16* in = (s16*)initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          s16 sample = *in++;
          *out++ = sample;
          *out++ = sample;
        }

      }
    } else {
      if (m_native_sample_format == SF_U8) {

        // channels = 2, bits = 8
        u8* in = initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          *out++ = u8tos16(*in++);
          *out++ = u8tos16(*in++);
        }

      } else {

        // channels = 2, bits = 16
        s16* in = (s16*)initial_buffer;
        for (unsigned i = 0; i < read; ++i) {
          *out++ = *in++;
          *out++ = *in++;
        }
      
      }
    }

    m_position = m_native_buffer;
    m_samples_left = read;
  }

  void
  Resampler::resetState() {
    m_time = 0;
    m_sl = 0;
    m_sr = 0;
  }

}
