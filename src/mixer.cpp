#include <algorithm>
#include "audiere.h"
#include "mixer.h"
#include "resampler.h"


namespace audiere {

  Mixer::Mixer() {
  }


  Mixer::~Mixer() {
    // assume all streams have been removed
  }


  void
  Mixer::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = 2;
    sample_rate = 44100;
    sample_format = SF_S16_LE;
  }


  int
  Mixer::read(const int sample_count, void* samples) {
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
    s32 mix_buffer[BUFFER_SIZE];
    s16 stream_buffer[BUFFER_SIZE * 2];
    std::fill(mix_buffer, mix_buffer + BUFFER_SIZE, 0);

    s16* out = (s16*)samples;
    int left = sample_count;
    while (left > 0) {
      int playing = 0;
      int to_mix = std::min(BUFFER_SIZE, left);

      SourceMap::iterator s = m_sources.begin();
      for (; s != m_sources.end(); ++s) {
        if (s->second.is_playing) {
          read(s->first, s->second, to_mix, stream_buffer);
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


  void
  Mixer::reset() {
  }


  void
  Mixer::addSource(SampleSource* source) {
    // initial source attributes
    SourceAttributes sa;
    sa.resampler = new Resampler(source);
    sa.last_l = 0;
    sa.last_r = 0;
    sa.is_playing = true;
    sa.volume = 255;

    m_sources[source] = sa;
  }


  void
  Mixer::removeSource(SampleSource* source) {
    delete m_sources[source].resampler;
    m_sources.erase(source);
  }


  bool
  Mixer::isPlaying(SampleSource* source) {
    return m_sources[source].is_playing;
  }


  void
  Mixer::setPlaying(SampleSource* source, bool is_playing) {
    m_sources[source].is_playing = is_playing;
  }


  float
  Mixer::getVolume(SampleSource* source) {
    return m_sources[source].volume / 255.0;
  }


  void
  Mixer::setVolume(SampleSource* source, float volume) {
    m_sources[source].volume = (int)(volume * 255.0f + 0.5f);
  }


  void
  Mixer::read(
    SampleSource* source,
    SourceAttributes& attr,
    int to_mix,
    s16* buffer)  // size = to_mix * 4
  {
    unsigned read = attr.resampler->read(to_mix, buffer);

    if (read == 0) {
      attr.is_playing = false;
    }

    // grab them early so we don't lose optimizations due to aliasing
    s16 l = attr.last_l;
    s16 r = attr.last_r;

    s16* out = buffer;
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

}
