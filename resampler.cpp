#include "resampler.hpp"


////////////////////////////////////////////////////////////////////////////////

Resampler::Resampler(ISampleSource* source)
{
  m_source = source;
  m_source->GetFormat(
    m_native_channel_count,
    m_native_sample_rate,
    m_native_bits_per_sample);

  FillBuffer();
  ResetState();
}

////////////////////////////////////////////////////////////////////////////////

Resampler::~Resampler()
{
}

////////////////////////////////////////////////////////////////////////////////

void
Resampler::GetFormat(int& channel_count, int& sample_rate, int& bits_per_sample)
{
  channel_count = 2;
  sample_rate = 44100;
  bits_per_sample = 16;
}

////////////////////////////////////////////////////////////////////////////////

int
Resampler::Read(const int sample_count, void* samples)
{
  adr_u16* out = (adr_u16*)samples;

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
      FillBuffer();

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

////////////////////////////////////////////////////////////////////////////////

void
Resampler::Reset()
{
  m_source->Reset();
  FillBuffer();
  ResetState();
}

////////////////////////////////////////////////////////////////////////////////

inline adr_s16 u8tos16(adr_u8 u) {
  return (adr_s16(u) - 127) * (1 << 8);
}

void
Resampler::FillBuffer()
{
  // we only support channels in [1, 2] and bits in [8, 16] now
  adr_u8 initial_buffer[NATIVE_BUFFER_SIZE * 4];
  unsigned read = m_source->Read(NATIVE_BUFFER_SIZE, initial_buffer);

  adr_s16* out = m_native_buffer;

  if (m_native_channel_count == 1) {
    if (m_native_bits_per_sample == 8) {

      // channels = 1, bits = 8
      adr_u8* in = initial_buffer;
      for (unsigned i = 0; i < read; ++i) {
	adr_s16 sample = u8tos16(*in++);
	*out++ = sample;
	*out++ = sample;
      }

    } else {

      // channels = 1, bits = 16
      adr_s16* in = (adr_s16*)initial_buffer;
      for (unsigned i = 0; i < read; ++i) {
	adr_s16 sample = *in++;
	*out++ = sample;
	*out++ = sample;
      }

    }
  } else {
    if (m_native_bits_per_sample == 8) {

      // channels = 2, bits = 8
      adr_u8* in = initial_buffer;
      for (unsigned i = 0; i < read; ++i) {
	*out++ = u8tos16(*in++);
	*out++ = u8tos16(*in++);
      }

    } else {

      // channels = 2, bits = 16
      adr_s16* in = (adr_s16*)initial_buffer;
      for (unsigned i = 0; i < read; ++i) {
	*out++ = *in++;
	*out++ = *in++;
      }
      
    }
  }

  m_position = m_native_buffer;
  m_samples_left = read;
}

////////////////////////////////////////////////////////////////////////////////

void
Resampler::ResetState()
{
  m_time = 0;
  m_sl = 0;
  m_sr = 0;
}

////////////////////////////////////////////////////////////////////////////////
