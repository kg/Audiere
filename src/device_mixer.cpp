#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <algorithm>
#include "device_mixer.h"
#include "buffer_stream.h"
#include "resampler.h"


namespace audiere {

  MixerDevice::MixerDevice(int rate) {
    m_rate = rate;
  }


  OutputStream*
  MixerDevice::openStream(SampleSource* source) {
    return (source ? new MixerStream(this, source, m_rate) : 0);
  }


  OutputStream*
  MixerDevice::openBuffer(
    void* samples, int frame_count,
    int channel_count, int sample_rate, SampleFormat sample_format)
  {
    return openStream(new BufferStream(
      samples, frame_count,
      channel_count, sample_rate, sample_format));
  }


  int
  MixerDevice::read(const int sample_count, void* samples) {
/*
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
*/

    // buffer in which to mix the audio
    static const int BUFFER_SIZE = 4096;

    // mix the output in chunks of BUFFER_SIZE samples
    s16* out = (s16*)samples;
    int left = sample_count;
    while (left > 0) {
      int to_mix = std::min(BUFFER_SIZE, left);

      s32 mix_buffer[BUFFER_SIZE];
      memset(mix_buffer, 0, sizeof(mix_buffer));
    
      for (std::list<MixerStream*>::iterator s = m_streams.begin();
           s != m_streams.end();
           ++s)
      {
        if ((*s)->isPlaying()) {
          s16 stream_buffer[BUFFER_SIZE * 2];
          (*s)->read(to_mix, stream_buffer);
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


  MixerStream::MixerStream(
    MixerDevice* device,
    SampleSource* source,
    int rate)
  {
    m_device     = device;
    m_source     = new RepeatableStream(new Resampler(source, rate));
    m_last_l     = 0;
    m_last_r     = 0;
    m_is_playing = false;
    m_volume     = 255;
    m_pan        = 0;

    m_device->m_streams.push_back(this);
  }


  MixerStream::~MixerStream() {
    m_device->m_streams.remove(this);
  }


  void
  MixerStream::play() {
    m_is_playing = true;
  }


  void
  MixerStream::stop() {
    m_is_playing = false;
  }


  bool
  MixerStream::isPlaying() {
    return m_is_playing;
  }


  void
  MixerStream::reset() {
    m_source->reset();
  }


  void
  MixerStream::setRepeat(bool repeat) {
    m_source->setRepeat(repeat);
  }


  bool
  MixerStream::getRepeat() {
    return m_source->getRepeat();
  }


  void
  MixerStream::setVolume(float volume) {
    m_volume = int(volume * 255.0f + 0.5f);
  }


  float
  MixerStream::getVolume() {
    return (m_volume / 255.0f);
  }


  void
  MixerStream::setPan(float pan) {
    m_pan = int(pan * 255.0f);
  }


  float
  MixerStream::getPan() {
    return m_pan = 255.0f;
  }


  bool
  MixerStream::isSeekable() {
    return m_source->isSeekable();
  }


  int
  MixerStream::getLength() {
    return m_source->getLength();
  }


  void
  MixerStream::setPosition(int position) {
    m_source->setPosition(position);
  }


  int
  MixerStream::getPosition() {
    return m_source->getPosition();
  }


  void
  MixerStream::read(int frame_count, s16* buffer) {
    unsigned read = m_source->read(frame_count, buffer);

    // if we are done with the sample source, stop and reset it
    if (read == 0) {
      m_is_playing = false;
      m_source->reset();
    }

    int l_volume, r_volume;
    if (m_pan < 0) {
      l_volume = 255;
      r_volume = 255 + m_pan;
    } else {
      l_volume = 255 - m_pan;
      r_volume = 255;
    }

    l_volume *= m_volume;
    r_volume *= m_volume;

    s16* out = buffer;
    for (int i = 0; i < read; ++i) {
      *out = *out * l_volume / 255 / 255;
      ++out;
      *out = *out * r_volume / 255 / 255;
      ++out;
    }

    int l = m_last_l;
    int r = m_last_r;

    if (read > 0) {
      l = out[-2];
      r = out[-1];
    }

    for (int i = read; i < frame_count; ++i) {
      *out++ = m_last_l;
      *out++ = m_last_r;
    }

    m_last_l = l;
    m_last_r = r;
  }

}
