#ifndef OUTPUT_OSS_HPP
#define OUTPUT_OSS_HPP


#include <memory>
#include "output.hpp"
#include "mixer.hpp"


class OSSOutputContext : public IOutputContext
{
public:
  OSSOutputContext();
  ~OSSOutputContext();

  bool Initialize(const char* parameters);
  void Update();
  IOutputStream* OpenStream(ISampleSource* source);

private:
  int m_output_device;

  Mixer m_mixer;
};


class OSSOutputStream : public IOutputStream
{
private:
  OSSOutputStream(Mixer* mixer, ISampleSource* source);
  ~OSSOutputStream();

public:
  // IOutputStream
  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();

private:
  Mixer* m_mixer;
  ISampleSource* m_source;
  int m_volume;

  friend OSSOutputContext;
};


#endif
