#ifndef OUTPUT_NULL_HPP
#define OUTPUT_NULL_HPP


#include "output.hpp"


class NullOutputContext : public IOutputContext
{
public:
  bool Initialize(const char* parameters);
  void Update();
  IOutputStream* OpenStream(ISampleSource* source);
};


class NullOutputStream : public IOutputStream
{
public:
  void Play();
  void Stop();
  void Reset();
  void ResetInputAndOutput();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();

private:
  NullOutputStream();

private:
  int m_volume;

  friend NullOutputContext;
};


#endif
