#ifndef MIXER_HPP
#define MIXER_HPP


// always produce 44.1 KHz, 16-bit, stereo audio
class Mixer
{
public:
  Mixer();
  ~Mixer();
  void Read(void* buffer, int sample_count);

private:
  char m_last_sample[4];  // make this an int someday?
};


#endif
