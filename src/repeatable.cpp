#include "debug.h"
#include "repeatable.h"
#include "utility.h"


namespace audiere {

  RepeatableStream::RepeatableStream(SampleSource* source) {
    m_repeat = false;
    m_source = source;

    // grab the sample size in bytes
    int channel_count, sample_rate;
    SampleFormat sample_format;
    m_source->getFormat(channel_count, sample_rate, sample_format);
    m_frame_size = channel_count * GetSampleSize(sample_format);
  }


  void
  RepeatableStream::setRepeat(bool repeat) {
    m_repeat = repeat;
  }


  bool
  RepeatableStream::getRepeat() {
    return m_repeat;
  }


  void
  RepeatableStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    m_source->getFormat(channel_count, sample_rate, sample_format);
  }


  int
  RepeatableStream::read(int frame_count, void* buffer) {
    if (m_repeat) {

      unsigned char* out = (unsigned char*)buffer;
      int samples_left = frame_count;
      while (samples_left > 0) {

        // read some samples
        int samples_read = m_source->read(samples_left, out);

        // if we couldn't read anything, reset the stream and try again
        if (samples_read == 0) {
          m_source->reset();
          samples_read = m_source->read(samples_left, out);
        }

        // if we still can't read anything, we're done
        if (samples_read == 0) {
          ADR_LOG("Can't read any samples even after reset");
          break;
        }

        samples_left -= samples_read;
        out += samples_read * m_frame_size;
      }

      return frame_count - samples_left;

    } else {

      return m_source->read(frame_count, buffer);

    }
  }


  void
  RepeatableStream::reset() {
    m_source->reset();
  }


  bool
  RepeatableStream::isSeekable() {
    return m_source->isSeekable();
  }


  int
  RepeatableStream::getLength() {
    return m_source->getLength();
  }


  void
  RepeatableStream::setPosition(int position) {
    m_source->setPosition(position);
  }

  
  int
  RepeatableStream::getPosition() {
    return m_source->getPosition();
  }

}
