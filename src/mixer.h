#ifndef MIXER_HPP
#define MIXER_HPP


#include <map>
#include "audiere.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  /// Always produce 44.1 KHz, 16-bit, stereo audio.
  class Mixer : public UnseekableSource {
  public:
    Mixer();
    ~Mixer();

    void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int read(int sample_count, void* samples);
    void reset();

    void addSource(SampleSource* source);
    void removeSource(SampleSource* source);

    bool isPlaying(SampleSource* source);
    void setPlaying(SampleSource* source, bool is_playing);

    int getVolume(SampleSource* source);
    void setVolume(SampleSource* source, int volume);

  private:
    struct SourceAttributes {
      // immutable
      SampleSource* resampler;
      s16 last_l;  // left
      s16 last_r;  // right

      // mutable (set by external calls)
      bool is_playing;
      int volume;
    };

    typedef std::map<SampleSource*, SourceAttributes> SourceMap;

  private:
    void read(SampleSource* source,
              SourceAttributes& attr,
              int to_mix,
              s16* buffer);

  private:
    SourceMap m_sources;
  };

}

#endif
