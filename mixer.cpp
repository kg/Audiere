#include <algorithm>
#include "audiere.h"
#include "mixer.hpp"
#include "output.hpp"


////////////////////////////////////////////////////////////////////////////////

Mixer::Mixer()
{
}

////////////////////////////////////////////////////////////////////////////////

Mixer::~Mixer()
{
  // assume all streams have been removed
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::GetFormat(int& channel_count, int& sample_rate, int& bits_per_sample)
{
  channel_count = 2;
  sample_rate = 44100;
  bits_per_sample = 16;
}

////////////////////////////////////////////////////////////////////////////////

int
Mixer::Read(const int sample_count, void* samples)
{
  if (m_sources.size() == 0) {
    memset(samples, 0, 4 * sample_count);
    return sample_count;
  }

  static const int BUFFER_SIZE = 4096;

  // create buffers in which to mix
  adr_s32 mix_buffer[BUFFER_SIZE];
  adr_s16 stream_buffer[BUFFER_SIZE * 2];
  std::fill(mix_buffer, mix_buffer + BUFFER_SIZE, 0);

  adr_s16* out = (adr_s16*)samples;
  int left = sample_count;
  while (left > 0) {
    int playing = 0;
    int to_mix = std::min(BUFFER_SIZE, left);

    SourceMap::iterator s = m_sources.begin();
    for (; s != m_sources.end(); ++s) {
      if (s->second.is_playing) {
	Read(s->first, s->second, to_mix, stream_buffer);
	for (int i = 0; i < to_mix * 2; ++i) {
          mix_buffer[i] += stream_buffer[i];
	}
	++playing;
      }
    }

    // do the division
    if (playing != 0) {
      for (int i = 0; i < to_mix * 2; ++i) {
	*out++ = mix_buffer[i] / playing;
      }
    }

    left -= to_mix;
  }

  return sample_count;
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::Reset()
{
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::AddSource(ISampleSource* source)
{
  // initial source attributes
  SourceAttributes sa;
  sa.is_playing = true;
  sa.volume = ADR_VOLUME_MAX;
  source->GetFormat(
    sa.channel_count,
    sa.sample_rate,
    sa.bits_per_sample);

  m_sources[source] = sa;
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::RemoveSource(ISampleSource* source)
{
  m_sources.erase(source);
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::SetPlaying(ISampleSource* source, bool is_playing)
{
  m_sources[source].is_playing = is_playing;
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::SetVolume(ISampleSource* source, int volume)
{
  m_sources[source].volume = volume;
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::Read(ISampleSource* source,
	    const SourceAttributes& attr,
	    int to_mix,
	    adr_s16* buffer)
{
  //  memset(buffer, 0, to_mix * 2);

  if (attr.bits_per_sample == 16 &&
      attr.sample_rate == 44100 &&
      attr.channel_count == 2) {
    source->Read(to_mix, buffer);
  }

  /*
  for (int i = 0; i < to_mix; ++i) {
    buffer[i] = buffer[i] * attr.volume / 255;
  }
  */
}

////////////////////////////////////////////////////////////////////////////////
