#ifndef RESAMPLER_H
#define RESAMPLER_H


#include "audiere.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  class Resampler : public UnseekableSource {
  public:
    Resampler(SampleSource* source, int rate);

    // for now, resamplers always return (2, rate, 16-bit)
    void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);

    int read(int sample_count, void* samples);
    void reset();

  private:
    void fillBuffer();
    void resetState();

  private:
    RefPtr<SampleSource> m_source;
    int m_rate;
    int m_native_channel_count;
    int m_native_sample_rate;
    SampleFormat m_native_sample_format;

    enum { NATIVE_BUFFER_SIZE = 4096 };
    s16 m_native_buffer[NATIVE_BUFFER_SIZE * 2];
    s16* m_position;
    unsigned m_samples_left;  // number of samples left to consume

    unsigned m_time;
    s16 m_sl; // left channel
    s16 m_sr; // right channel
  };

}

#endif
