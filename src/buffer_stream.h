#ifndef BUFFER_STREAM_H
#define BUFFER_STREAM_H


#include "audiere.h"
#include "types.h"


namespace audiere {

  class BufferStream : public RefImplementation<SampleSource> {
  public:
    BufferStream(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format);
    ~BufferStream();

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);

    int  ADR_CALL read(int sample_count, void* samples);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    u8* m_buffer;
    int m_buffer_length; // in samples
    int m_position;      // in samples

    int m_block_size; // m_channel_count * GetSampleSize(m_sample_format)
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;
  };

}


#endif
