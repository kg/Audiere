#ifndef OUTPUT_AL_HPP
#define OUTPUT_AL_HPP


#include "output.hpp"


class ALOutputContext : public IOutputContext
{
public:
  bool Initialize(const char* parameters);

  void Update();

  IOutputStream* OpenStream(
    int channel_count,
    int sample_rate,
    int bits_per_sample,
    ADR_SAMPLE_SOURCE source,
    ADR_SAMPLE_RESET reset,
    void* opaque
  );

private:
  ALCdevice m_Device;
};


class ALOutputStream : public IOutputStream
{
public:
  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();
  void SetPan(int pan);
  int  GetPan();

private:
  ALOutputStream();

private:
  ALCwhatever

  friend ALOutputContext;
};


#endif
