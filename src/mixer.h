#ifndef MIXER_H
#define MIXER_H


#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <map>
#include "audiere.h"
#include "repeatable.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  /// Always produce 44.1 KHz, 16-bit, stereo audio.
  class Mixer : public UnseekableSource {
  public:
    Mixer();

    void getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int read(int sample_count, void* samples);
    void reset();

    void addSource(SampleSource* source);
    void removeSource(SampleSource* source);

    void resetSource(SampleSource* source);

    void setPlaying(SampleSource* source, bool is_playing);
    bool isPlaying(SampleSource* source);

    void setVolume(SampleSource* source, float volume);
    float getVolume(SampleSource* source);

    void setPan(SampleSource* source, float pan);
    float getPan(SampleSource* source);

    void setRepeat(SampleSource* source, bool repeat);
    bool getRepeat(SampleSource* source);

  private:
    struct SourceAttributes {
      // internal
      RefPtr<RepeatableStream> source;
      s16 last_l;  // left
      s16 last_r;  // right

      // set by external calls
      bool is_playing;
      int volume;  // [0, 255]
      int pan;     // [-255, 255]
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
