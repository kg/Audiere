#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <algorithm>
#include "audiere.h"
#include "mixer.h"
#include "resampler.h"


namespace audiere {

  Mixer::Mixer() {
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

    // buffer in which to mix the audio
    static const int BUFFER_SIZE = 4096;
    s32 mix_buffer[BUFFER_SIZE];
    memset(mix_buffer, 0, sizeof(mix_buffer));

    // mix the output in chunks of BUFFER_SIZE samples
    s16* out = (s16*)samples;
    int left = sample_count;
    while (left > 0) {
      int to_mix = std::min(BUFFER_SIZE, left);

      for (SourceMap::iterator s = m_sources.begin();
           s != m_sources.end();
           ++s)
      {
        if (s->second.is_playing) {
          s16 stream_buffer[BUFFER_SIZE * 2];
          read(s->first, s->second, to_mix, stream_buffer);
          for (int i = 0; i < to_mix * 2; ++i) {
            mix_buffer[i] += stream_buffer[i];
          }
        }
      }

      // clamp each value in the buffer to the valid s16 range
      for (int i = 0; i < to_mix * 2; ++i) {
        s32 mixed = mix_buffer[i];
        s16 o;
        if (mixed < -32768) {
          o = -32768;
        } else if (mixed > 32767) {
          o = 32767;
        } else {
          o = mixed;
        }
        *out++ = o;
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
    sa.source = new RepeatableStream(new Resampler(source));
    sa.last_l = 0;
    sa.last_r = 0;
    sa.is_playing = false;
    sa.volume = 255;
    sa.pan = 0;

    m_sources[source] = sa;
  }


  void
  Mixer::removeSource(SampleSource* source) {
    m_sources.erase(source);
  }

  
  void
  Mixer::resetSource(SampleSource* source) {
    m_sources[source].source->reset();
  }


  void
  Mixer::setPlaying(SampleSource* source, bool is_playing) {
    m_sources[source].is_playing = is_playing;
  }


  bool
  Mixer::isPlaying(SampleSource* source) {
    return m_sources[source].is_playing;
  }


  void
  Mixer::setVolume(SampleSource* source, float volume) {
    m_sources[source].volume = (int)(volume * 255.0f + 0.5f);
  }


  float
  Mixer::getVolume(SampleSource* source) {
    return m_sources[source].volume / 255.0f;
  }


  void
  Mixer::setPan(SampleSource* source, float pan) {
    m_sources[source].pan = (int)(pan * 255.0f);    
  }


  float
  Mixer::getPan(SampleSource* source) {
    return m_sources[source].pan / 255.0f;
  }


  void
  Mixer::setRepeat(SampleSource* source, bool repeat) {
    m_sources[source].source->setRepeat(repeat);
  }


  bool
  Mixer::getRepeat(SampleSource* source) {
    return m_sources[source].source->getRepeat();
  }


  void
  Mixer::read(
    SampleSource* source,
    SourceAttributes& attr,
    int to_mix,
    s16* buffer)  // size = to_mix * 4
  {
    unsigned read = attr.source->read(to_mix, buffer);

    // if we are done with the sample source, stop and reset it
    if (read == 0) {
      attr.is_playing = false;
      resetSource(source);
    }

    // grab them early so we don't lose optimizations due to aliasing
    s16 l = attr.last_l;
    s16 r = attr.last_r;

    int l_volume, r_volume;
    if (attr.pan < 0) {
      l_volume = 255;
      r_volume = 255 + attr.pan;
    } else {
      l_volume = 255 - attr.pan;
      r_volume = 255;
    }

    l_volume *= attr.volume;
    r_volume *= attr.volume;

    s16* out = buffer;
    for (int i = 0; i < read; ++i) {
      *out = *out * l_volume / 255 / 255;
      ++out;
      *out = *out * r_volume / 255 / 255;
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
