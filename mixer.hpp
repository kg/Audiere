#ifndef MIXER_HPP
#define MIXER_HPP


#include <map>


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
  char m_last_sample[4];  // make this an int someday?

  struct SourceAttributes {
    // mutable (set by external calls)
    bool is_playing;
    int volume;

    // mutable, internal (updated by mixing process)
    int something;

    // immutable, set when source is added to the list
    int channel_count;
    int sample_rate;
    int bits_per_sample;
  };

  std::map<ISampleSource*, SourceAttributes> m_sources;
};


#endif
