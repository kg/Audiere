#ifndef OUTPUT_DS8_HPP
#define OUTPUT_DS8_HPP


// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
#include <list>
#include "output.hpp"


class DS8OutputStream;


// context
class DS8OutputContext : public IOutputContext
{
public:
  DS8OutputContext();
  ~DS8OutputContext();

  bool Initialize(const char* parameters);
  void Update();
  IOutputStream* OpenStream(ISampleSource* source);

private:
  typedef std::list<DS8OutputStream*> StreamList;

private:
  IDirectSound8* m_DirectSound;
  StreamList     m_OpenStreams;

  int m_BufferLength;  // in milliseconds

  HWND m_AnonymousWindow;

  friend DS8OutputStream;
};


// stream
class DS8OutputStream : public IOutputStream
{
  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();

private:
  DS8OutputStream(
    DS8OutputContext* context,
    IDirectSoundBuffer* buffer,
    int sample_size,   // in bytes
    int buffer_length, // in samples
    ISampleSource* source);
  ~DS8OutputStream();

  void FillStream();
  void Update();
  int StreamRead(int samples_to_read, void* buffer);
  
  // returns true if |position| is between |start| (inclusive) and
  // |end| (exclusive) in the buffer
  bool IsBetween(int position, int start, int end);

private:
  DS8OutputContext* m_Context;

  IDirectSoundBuffer* m_Buffer;
  int m_NextRead;  // offset (in samples) where we will read next
  int m_BufferLength;  // in samples

  // input
  ISampleSource* m_Source;

  // convenience
  int m_SampleSize;  // bits per sample * channel count / 8

  // state
  int m_Volume;

  BYTE* m_LastSample; // the last sample read (used for clickless silence)

  friend DS8OutputContext;
};


#endif
