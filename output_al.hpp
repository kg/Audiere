#ifndef OUTPUT_AL_HPP
#define OUTPUT_AL_HPP


#include <list>
#include <AL/al.h>
#include <AL/alc.h>
#include "output.hpp"


// XXX hack around the fact that the Linux headers don't have ALCcontext yet
#ifndef _WIN32
typedef void ALCcontext;
#endif


class ALOutputStream;


class ALOutputContext : public IOutputContext
{
public:
  ALOutputContext();
  ~ALOutputContext();

  bool Initialize(const char* parameters);
  void Update();
  IOutputStream* OpenStream(ISampleSource* source);

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

private:
  ALOutputStream(
    ALOutputContext* context,
    ISampleSource* source,
    ALuint al_source,
    ALuint* buffers,
    ALenum format,
    int sample_rate);
  ~ALOutputStream();
  void Update();

  int Read(void* samples, int sample_count);
  void FillBuffers();

private:
  ALOutputContext* m_Context;

  // sample stream
  ISampleSource* m_Source;

  // informational (convenience)
  ALenum m_Format;
  int    m_SampleSize;  // (num channels * bits per sample / 8)
  int    m_SampleRate;

  // the last sample read
  ALubyte* m_LastSample;  
  
  // AL objects
  ALuint  m_ALSource;
  ALuint* m_Buffers;

  int m_BufferLength;  // in samples
  bool m_IsPlaying;
  int m_Volume;

  friend ALOutputContext;
};


#endif
