#ifndef MIXER_HPP
#define MIXER_HPP


#include <map>
#include "config.h"
#include "input.hpp"


// always produce 44.1 KHz, 16-bit, stereo audio
class Mixer : public ISampleSource
{
public:
  Mixer();
  ~Mixer();

  void GetFormat(int& channel_count, int& sample_rate, int& bits_per_sample);
  int Read(int sample_count, void* samples);
  bool Reset();

  void AddSource(ISampleSource* source);
  void RemoveSource(ISampleSource* source);
  void SetPlaying(ISampleSource* source, bool is_playing);
  void SetVolume(ISampleSource* source, int volume);

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

  typedef std::map<ISampleSource*, SourceAttributes> SourceMap;

private:
  void Read(ISampleSource* source,
	    SourceAttributes& attr,
	    int to_mix,
	    adr_s16* buffer);

private:
  SourceMap m_sources;
};


#endif
