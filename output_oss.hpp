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
  IOutputStream* OpenStream(ISampleSource source);

private:
  int m_output_file;

  std::auto_ptr<Mixer> m_mixer;

  friend OSSOutputStream;
};


class OSSOutputStream : public IOutputStream
{
public:
  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();

private:
};


#endif
