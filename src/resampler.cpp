#include "resampler.h"


namespace audiere {

  Resampler::Resampler(SampleSource* source, int rate) {
    m_source = source;
    m_rate = rate;
    m_source->getFormat(
      m_native_channel_count,
      m_native_sample_rate,
      m_native_sample_format);

    m_shift = 1;

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
    sample_rate = m_rate;
    sample_format = SF_S16;
  }

  int
  Resampler::read(const int frame_count, void* buffer) {
    u16* out = (u16*)buffer;

    int left = frame_count;

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
          return frame_count - left;
        }
      }

      m_sl = *m_position++;
      m_sr = *m_position++;
      --m_samples_left;

      m_time += m_rate / m_shift;
      while (m_time > m_native_sample_rate && left > 0) {
        m_time -= m_native_sample_rate;
        *out++ = m_sl;
        *out++ = m_sr;
        --left;
      }

    }
    return frame_count;
  }

  void
  Resampler::reset() {
    m_source->reset();
    fillBuffer();
    resetState();
  }


  inline s16 u8tos16(u8 u) {
    return (s16(u) - 128) * 256;
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

  bool
  Resampler::isSeekable() {
    return m_source->isSeekable();
  }

  int
  Resampler::getLength() {
    return m_source->getLength();
  }

  void
  Resampler::setPosition(int position) {
    m_source->setPosition(position);
    fillBuffer();
    resetState();
  }

  int
  Resampler::getPosition() {
    int position = m_source->getPosition() - m_samples_left;
    while (position < 0) {
      position += m_source->getLength();
    }
    return position;
  }

  bool
  Resampler::getRepeat() {
    return m_source->getRepeat();
  }

  void
  Resampler::setRepeat(bool repeat) {
    /// @todo if we've already read to the end, do we try to read more?
    m_source->setRepeat(repeat);
  }

  void
  Resampler::setPitchShift(float shift) {
    m_shift = shift;
  }

  float
  Resampler::getPitchShift() {
    return m_shift;
  }

}
