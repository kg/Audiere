#ifndef OUTPUT_DS3_HPP
#define OUTPUT_DS3_HPP


// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
#include <list>
#include "output.hpp"


class DS3OutputStream;


class DS3OutputContext : public IOutputContext
{
public:
  DS3OutputContext();
  ~DS3OutputContext();

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
  typedef std::list<DS3OutputStream*> StreamList;

private:
  IDirectSound*       m_DirectSound;
  IDirectSoundBuffer* m_PrimaryBuffer;
  StreamList          m_OpenStreams;

  int m_BufferLength;  // in milliseconds

  HWND m_AnonymousWindow;

  friend DS3OutputStream;
};


class DS3OutputStream : public IOutputStream
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
  DS3OutputStream(
    DS3OutputContext* context,
    IDirectSoundBuffer* buffer,
    int sample_size,   // in bytes
    int buffer_length, // in samples
    ADR_SAMPLE_SOURCE source,
    ADR_SAMPLE_RESET reset,
    void* opaque);
  ~DS3OutputStream();

  void FillStream();
  void Update();
  int StreamRead(int samples_to_read, void* buffer);

  // returns true if |position| is between |start| (inclusive) and
  // |end| (exclusive) in the buffer
  bool IsBetween(int position, int start, int end);

private:
  DS3OutputContext* m_Context;

  IDirectSoundBuffer* m_Buffer;
  int m_NextRead;  // offset (in samples) where we will read next
  int m_BufferLength;  // in samples

  // input
  ADR_SAMPLE_SOURCE m_Source;
  ADR_SAMPLE_RESET  m_Reset;
  void*             m_Opaque;

  // convenience
  int m_SampleSize;  // bits per sample * channel count / 8

  // state
  int m_Volume;
  int m_Pan;

  BYTE* m_LastSample; // the last sample read (used for clickless silence)

  friend DS3OutputContext;
};


#endif
