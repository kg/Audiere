#ifndef OUTPUT_DS_HPP
#define OUTPUT_DS_HPP


// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include <windows.h>
#include <mmreg.h>
#include <dsound.h>
#include <list>
#include "output.hpp"


class DSOutputStream;


class DSOutputContext : public IOutputContext
{
public:
  DSOutputContext();
  ~DSOutputContext();

  bool Initialize(const char* parameters);
  void Update();
  IOutputStream* OpenStream(ISampleSource* source);

private:
  typedef std::list<DSOutputStream*> StreamList;

  void RemoveStream(DSOutputStream* stream);

  IDirectSound* m_DirectSound;
  StreamList    m_OpenStreams;

  int m_BufferLength;  // in milliseconds

  HWND m_AnonymousWindow;


  // these must be overridden
  virtual REFCLSID GetCLSID() = 0;
  virtual DWORD GetCooperativeLevel() = 0;
  virtual bool CreatePrimarySoundBuffer(IDirectSound* ds) = 0;

  friend class DSOutputStream;
};


class DSOutputStream : public IOutputStream
{
private:
  DSOutputStream(
    DSOutputContext* context,
    IDirectSoundBuffer* buffer,
    int sample_size,   // in bytes
    int buffer_length, // in samples
    ISampleSource* source);
  ~DSOutputStream();

public:
  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();

  void FillStream();
  void Update();
  int StreamRead(int samples_to_read, void* buffer);
  
  // returns true if |position| is between |start| (inclusive) and
  // |end| (exclusive) in the buffer
  bool IsBetween(int position, int start, int end);

private:
  DSOutputContext* m_Context;

  IDirectSoundBuffer* m_Buffer;
  int m_BufferLength;  // in samples
  int m_NextRead;  // offset (in samples) where we will read next

  ISampleSource* m_Source;
  int m_SampleSize;  // convenience: bits per sample * channel count / 8

  int m_Volume;
  BYTE* m_LastSample; // the last sample read (used for clickless silence)

  friend class DSOutputContext;
};


#endif
