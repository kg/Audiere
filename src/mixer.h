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

  /// Always produce 16-bit, stereo audio at the specified rate.
  class Mixer : public UnseekableSource {
  public:
    Mixer(int rate);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int ADR_CALL read(int sample_count, void* samples);
    void ADR_CALL reset();

    void ADR_CALL addSource(SampleSource* source);
    void ADR_CALL removeSource(SampleSource* source);

    void ADR_CALL resetSource(SampleSource* source);

    void ADR_CALL setPlaying(SampleSource* source, bool is_playing);
    bool ADR_CALL isPlaying(SampleSource* source);

    void  ADR_CALL setVolume(SampleSource* source, float volume);
    float ADR_CALL getVolume(SampleSource* source);

    void  ADR_CALL setPan(SampleSource* source, float pan);
    float ADR_CALL getPan(SampleSource* source);

    void ADR_CALL setRepeat(SampleSource* source, bool repeat);
    bool ADR_CALL getRepeat(SampleSource* source);

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
    int m_rate;
    SourceMap m_sources;
  };

}

#endif
