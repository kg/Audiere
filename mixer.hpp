#ifndef MIXER_HPP
#define MIXER_HPP


#include <map>
#include "config.h"


class ISampleSource;  // #include "output.hpp"


// always produce 44.1 KHz, 16-bit, stereo audio
class Mixer
{
public:
  Mixer();
  ~Mixer();
  void Read(void* buffer, int sample_count);
  void AddSource(ISampleSource* source);
  void RemoveSource(ISampleSource* source);
  void SetPlaying(ISampleSource* source, bool is_playing);
  void SetVolume(ISampleSource* source, int volume);

private:
  struct SourceAttributes {
    // mutable (set by external calls)
    bool is_playing;
    int volume;

    // mutable, internal (updated by mixing process)

    // immutable, set when source is added to the list
    int channel_count;
    int sample_rate;
    int bits_per_sample;
  };

  typedef std::map<ISampleSource*, SourceAttributes> SourceMap;

private:
  void Read(ISampleSource* source,
	    const SourceAttributes& attr,
	    int to_mix,
	    adr_s16* buffer);

private:
  SourceMap m_sources;
};


#endif
