#include <stdio.h>

#include <algorithm>
#include "audiere.h"
#include "mixer.hpp"
#include "output.hpp"


////////////////////////////////////////////////////////////////////////////////

Mixer::Mixer()
{
  //  memset(m_last_sample, 0, 4);
}

////////////////////////////////////////////////////////////////////////////////

Mixer::~Mixer()
{
  // assume all streams have been removed
}

////////////////////////////////////////////////////////////////////////////////

void
Mixer::Read(void* buffer, const int sample_count)
{
  puts("trying...");

  if (m_sources.size() == 0) {
    memset(buffer, 0, 4 * sample_count);
    return;
  }

  static const int BUFFER_SIZE = 4096;

  // create buffers in which to mix
  adr_u32 mix_buffer[BUFFER_SIZE];
  adr_u16 stream_buffer[BUFFER_SIZE];
  std::fill(mix_buffer, mix_buffer + BUFFER_SIZE, 0);

  adr_u16* out = (adr_u16*)buffer;

  puts("here?");

  int left = sample_count;
  while (left > 0) {
    puts("?");
    int to_mix = std::max(BUFFER_SIZE, left);
    SourceMap::iterator s = m_sources.begin();
    for (; s != m_sources.end(); ++s) {
      puts("sourcey");
      Read(s->first, s->second, to_mix, stream_buffer);
      for (int i = 0; i < to_mix; ++i) {
	mix_buffer[i] += stream_buffer[i];
      }
    }
    puts(":");

    // do the division
    for (int i = 0; i < to_mix; ++i) {
      // this is freezing the thread?
      //      *out++ = mix_buffer[i] / m_sources.size();
      printf("%d %d\n", to_mix, i);
    }

    left -= to_mix;

    printf("%d\n", left);
  }

  puts("out");
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
	    adr_u16* buffer)
{
  if (!attr.is_playing) {
    return;
  }

  memset(buffer, 0, to_mix * 2);

  if (attr.bits_per_sample == 16 &&
      attr.sample_rate == 44100 &&
      attr.channel_count == 2) {
    source->Read(to_mix, buffer);
  }

  for (int i = 0; i < to_mix; ++i) {
    buffer[i] = buffer[i] * attr.volume / 255;
  }
}

////////////////////////////////////////////////////////////////////////////////
