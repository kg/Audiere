#include "repeatable.hpp"


////////////////////////////////////////////////////////////////////////////////

RepeatableStream::RepeatableStream(ISampleSource* source, bool initial_state)
{
  m_repeat = initial_state;
  m_source = source;

  // grab the sample size in bytes
  int channel_count, sample_rate, bits_per_sample;
  m_source->GetFormat(channel_count, sample_rate, bits_per_sample);
  m_sample_size = channel_count * bits_per_sample / 8;
}

////////////////////////////////////////////////////////////////////////////////

RepeatableStream::~RepeatableStream()
{
  delete m_source;
}

////////////////////////////////////////////////////////////////////////////////

void
RepeatableStream::SetRepeat(bool repeat)
{
  m_repeat = repeat;
}

////////////////////////////////////////////////////////////////////////////////

bool
RepeatableStream::GetRepeat()
{
  return m_repeat;
}

////////////////////////////////////////////////////////////////////////////////

void
RepeatableStream::GetFormat(
  int& channel_count,
  int& sample_rate,
  int& bits_per_sample)
{
  m_source->GetFormat(channel_count, sample_rate, bits_per_sample);
}

////////////////////////////////////////////////////////////////////////////////

int
RepeatableStream::Read(int sample_count, void* samples)
{
  if (m_repeat) {

    unsigned char* out = (unsigned char*)samples;
    int samples_left = sample_count;
    while (samples_left > 0) {

      // read some samples
      int samples_read = m_source->Read(samples_left, out);

      // if we couldn't read anything, reset the stream and try again
      if (samples_read == 0) {
        m_source->Reset();
        samples_read = m_source->Read(samples_left, samples);
      }

      // if we still can't read anything, we're done
      if (samples_read == 0) {
        break;
      }

      samples_left -= samples_read;
      out += samples_read * m_sample_size;
    }

    return sample_count - samples_left;

  } else {

    return m_source->Read(sample_count, samples);

  }
}

////////////////////////////////////////////////////////////////////////////////

bool
RepeatableStream::Reset()
{
  return m_source->Reset();
}

////////////////////////////////////////////////////////////////////////////////
