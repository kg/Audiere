#include <algorithm>
#include "audiere.h"
#include "mixer.hpp"
#include "output.hpp"
#include "resampler.hpp"


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
  // are any sources playing?
  bool any_playing = false;
  SourceMap::iterator i = m_sources.begin();
  while (i != m_sources.end()) {
    any_playing |= i->second.is_playing;
    ++i;
  }
  
  // if not, return zeroed samples
  if (!any_playing) {
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

bool
Mixer::Reset()
{
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::AddSource(ISampleSource* source)
{
  // initial source attributes
  SourceAttributes sa;
  sa.resampler = new Resampler(source);
  sa.last_l = 0;
  sa.last_r = 0;
  sa.is_playing = true;
  sa.volume = ADR_VOLUME_MAX;

  m_sources[source] = sa;
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::RemoveSource(ISampleSource* source)
{
  delete m_sources[source].resampler;
  m_sources.erase(source);
}

////////////////////////////////////////////////////////////////////////////////

bool
Mixer::IsPlaying(ISampleSource* source)
{
  return m_sources[source].is_playing;
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::SetPlaying(ISampleSource* source, bool is_playing)
{
  m_sources[source].is_playing = is_playing;
}

////////////////////////////////////////////////////////////////////////////////

int
Mixer::GetVolume(ISampleSource* source)
{
  return m_sources[source].volume;
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
	    SourceAttributes& attr,
	    int to_mix,
	    adr_s16* buffer)  // size = to_mix * 4
{
  unsigned read = attr.resampler->Read(to_mix, buffer);

  if (read == 0) {
    attr.is_playing = false;
  }

  // grab them early so we don't lose optimizations due to aliasing
  adr_s16 l = attr.last_l;
  adr_s16 r = attr.last_r;

  adr_s16* out = buffer;
  for (int i = 0; i < read; ++i) {
    *out = *out * attr.volume / 255;
    ++out;
    *out = *out * attr.volume / 255;
    ++out;
  }

  if (read >= 0) {
    l = out[-2];
    r = out[-1];
  }

  for (int i = read; i < to_mix; ++i) {
    *out++ = l;
    *out++ = r;
  }

  attr.last_l = l;
  attr.last_r = r;
}

////////////////////////////////////////////////////////////////////////////////
