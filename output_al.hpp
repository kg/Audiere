#ifndef OUTPUT_AL_HPP
#define OUTPUT_AL_HPP


#include <list>
#include <al/al.h>
#include <al/alc.h>
#include "output.hpp"


static const int AL_BUFFER_COUNT = 16;


class ALOutputStream;


class ALOutputContext : public IOutputContext
{
public:
  ALOutputContext();
  ~ALOutputContext();

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
  ALCdevice*  m_Device;
  ALCcontext* m_Context;

  typedef std::list<ALOutputStream*> StreamList;
  StreamList m_OpenStreams;

  friend ALOutputStream;
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
  ALOutputStream(
    ALOutputContext* context,
    ALuint* buffers,
    ALuint* sources,
    int channel_count,
    int sample_rate,
    int bits_per_sample);
  ~ALOutputStream();
  void Update();

private:
  ALOutputContext* m_Context;

  // informational
  int m_ChannelCount;
  int m_SampleRate;
  int m_BitsPerSample;
  
  // AL objects
  ALuint* m_Buffers;
  ALuint* m_Sources;

  bool m_IsPlaying;

  int m_Volume;
  int m_Pan;

  friend ALOutputContext;
};


#endif
