#ifndef INPUT_MOD_HPP
#define INPUT_MOD_HPP


#include <mikmod.h>  // 
#include <uniform.h> // mikmod headers
#include <mplayer.h> //
#include "config.h"
#include "input.hpp"


// should match the C runtime's calling convention

#ifdef _WIN32
  #define CRT_CALL __cdecl
#else
  #define CRT_CALL
#endif


class MODInputStream : public ISampleSource
{
public:
  MODInputStream();
  ~MODInputStream();

  bool Initialize(IFile* file);

  void GetFormat(
    int& channel_count,
    int& sample_rate,
    int& bits_per_sample);
  int Read(int sample_count, void* samples);
  bool Reset();

private:
  // MMIO file callbacks
  static int CRT_CALL MMRead(
    void* buffer, size_t size, size_t count, FILE* stream);
  static int CRT_CALL MMWrite(
    const void* buffer, size_t size, size_t count, FILE* stream);
  static int CRT_CALL MMGetC(FILE* stream);
  static int CRT_CALL MMPutC(int c, FILE* stream);
  static int CRT_CALL MMSeek(FILE* stream, long offset, int origin);
  static int CRT_CALL MMTell(FILE* stream);
  static int CRT_CALL MMEof(FILE* stream);

  // MikMod driver implementation
  static BOOL ACQ_IsThere();
  static BOOL ACQ_Init(MDRIVER* md, uint latency, void* optstr);
  static void ACQ_Exit(MDRIVER* md);
  static void ACQ_Update(MDRIVER* md);
  static BOOL ACQ_SetSoftVoices(MDRIVER* md, uint voices);
  static BOOL ACQ_SetMode(MDRIVER* md, uint mixspeed, uint mode,
                          uint channels, uint cpumode);
  static void ACQ_GetMode(MDRIVER* md, uint* mixspeed, uint* mode,
                          uint* channels, uint* cpumode);

private:
  enum { SAMPLE_BUFFER_SIZE = 4096 };

  IFile* m_file;

  // MikMod handles
  MDRIVER* m_driver;
  UNIMOD*  m_module;
  MPLAYER* m_player;

  MMSTREAM m_stream;

  adr_u32  m_samples_left;
  adr_u32* m_next_sample;
  adr_u32  m_sample_buffer[SAMPLE_BUFFER_SIZE];

  bool m_at_eof;

  // mikmod driver description
  static MD_DEVICE drv_acq;
};


#endif
