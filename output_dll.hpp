#ifndef OUTPUT_DLL_HPP
#define OUTPUT_DLL_HPP


// disable 'identifier too long' warning
#ifdef _MSC_VER
#pragma warning(disable : 4786)
#endif


#include "output.hpp"


class DLLOutputStream;


typedef unsigned char AO_BOOL;
#define AO_TRUE  1
#define AO_FALSE 0


class DLLOutputContext : public IOutputContext
{
public:
  DLLOutputContext();
  ~DLLOutputContext();

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
  typedef void* AO_STREAM;

private:
  AO_BOOL (ADR_CALL *AO_OpenDriver)(const char* parameters);
  void (ADR_CALL *AO_CloseDriver)();

  void (ADR_CALL *AO_Update)();

  AO_STREAM (ADR_CALL *AO_OpenStream)(
    int channel_count,
    int sample_rate,
    int bits_per_sample,
    ADR_SAMPLE_SOURCE source,
    ADR_SAMPLE_RESET reset,
    void* opaque);

  void (ADR_CALL *AO_CloseStream)(AO_STREAM stream);

  void    (ADR_CALL *AO_PlayStream)(AO_STREAM stream);
  void    (ADR_CALL *AO_StopStream)(AO_STREAM stream);
  void    (ADR_CALL *AO_ResetStream)(AO_STREAM stream);
  AO_BOOL (ADR_CALL *AO_IsStreamPlaying)(AO_STREAM stream);

  void (ADR_CALL *AO_SetVolume)(AO_STREAM stream, int volume);
  int  (ADR_CALL *AO_GetVolume)(AO_STREAM stream);
  void (ADR_CALL *AO_SetPan)(AO_STREAM stream, int pan);
  int  (ADR_CALL *AO_GetPan)(AO_STREAM stream);

  HINSTANCE m_DLL;

  friend DLLOutputStream;
};


class DLLOutputStream : public IOutputStream
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
  DLLOutputStream(
    DLLOutputContext* context,
    DLLOutputContext::AO_STREAM stream);
  ~DLLOutputStream();

private:
  DLLOutputContext*           m_Context;
  DLLOutputContext::AO_STREAM m_Stream;

  friend DLLOutputContext;
};


#endif
