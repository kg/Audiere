#ifndef OUTPUT_AL_HPP
#define OUTPUT_AL_HPP


#include <list>
#include <al/al.h>
#include <al/alc.h>
#include "output.hpp"


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
    ADR_SAMPLE_SOURCE source,
    ADR_SAMPLE_RESET reset,
    void* opaque,
    ALuint* buffers,
    ALuint* sources,
    int channel_count,
    int sample_rate,
    int bits_per_sample);
  ~ALOutputStream();
  void Update();

  void ReadDeinterleaved(
    void* deinterleaved,
    void* interleaved,  // so the read doesn't have to allocate (slow) anything
    int block_count);
  void FillBuffers();

private:
  ALOutputContext* m_Context;

  // sample stream
  ADR_SAMPLE_SOURCE m_Source;
  ADR_SAMPLE_RESET  m_Reset;
  void*             m_Opaque;

  // informational
  int m_ChannelCount;
  int m_SampleRate;
  int m_BitsPerSample;
  int m_SampleSize;    // convenience

  // the last sample read
  ALubyte* m_LastBlock;  
  
  // AL objects
  ALuint* m_Buffers;
  ALuint* m_Sources;

  int m_BufferLength;  // in samples

  bool m_IsPlaying;

  int m_Volume;
  int m_Pan;

  friend ALOutputContext;
};


#endif
