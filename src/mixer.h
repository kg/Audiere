#ifndef MIXER_HPP
#define MIXER_HPP


#include <map>
#include "audiere.h"
#include "types.h"


namespace audiere {

  /// Always produce 44.1 KHz, 16-bit, stereo audio.
  class Mixer : public DLLImplementation<SampleSource> {
  public:
    Mixer();
    ~Mixer();

    void getFormat(int& channel_count, int& sample_rate, int& bits_per_sample);
    int read(int sample_count, void* samples);
    bool reset();

    void addSource(ISampleSource* source);
    void removeSource(ISampleSource* source);

    bool isPlaying(ISampleSource* source);
    void setPlaying(ISampleSource* source, bool is_playing);

    int getVolume(ISampleSource* source);
    void setVolume(ISampleSource* source, int volume);

  private:
    struct SourceAttributes {
      // immutable
      ISampleSource* resampler;
      adr_s16 last_l;  // left
      adr_s16 last_r;  // right

      // mutable (set by external calls)
      bool is_playing;
      int volume;
    };

    typedef std::map<SampleSource*, SourceAttributes> SourceMap;

  private:
    void read(SampleSource* source,
              SourceAttributes& attr,
              int to_mix,
              adr_s16* buffer);

  private:
    SourceMap m_sources;
  };

}

#endif
