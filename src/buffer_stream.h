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

    void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);

    int read(int sample_count, void* samples);
    void reset();

    bool isSeekable();
    int getLength();
    void setPosition(int position);
    int getPosition();

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
