#include "buffer_stream.h"
#include "utility.h"


namespace audiere {

  BufferStream::BufferStream(
    void* samples, int sample_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    m_buffer_length = sample_count;
    m_position      = 0;
    
    m_channel_count = channel_count;
    m_sample_rate   = sample_rate;
    m_sample_format = sample_format;

    m_frame_size = m_channel_count * GetSampleSize(m_sample_format);

    m_buffer = new u8[m_buffer_length * m_frame_size];
    memcpy(m_buffer, samples, m_buffer_length * m_frame_size);
  }

  BufferStream::~BufferStream() {
    delete[] m_buffer;
  }

  void
  BufferStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
  }

  int
  BufferStream::read(int frame_count, void* samples) {
    int to_read = std::min(frame_count, m_buffer_length - m_position);
    memcpy(
      samples,
      m_buffer + m_position * m_frame_size,
      to_read * m_frame_size);
    m_position += to_read;
    return to_read;
  }

  void
  BufferStream::reset() {
    setPosition(0);
  }

  bool
  BufferStream::isSeekable() {
    return true;
  }

  int
  BufferStream::getLength() {
    return m_buffer_length;
  }

  void
  BufferStream::setPosition(int position) {
    m_position = position;
  }

  int
  BufferStream::getPosition() {
    return m_position;
  }



}
