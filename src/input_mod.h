#ifndef INPUT_MOD_HPP
#define INPUT_MOD_HPP


#include <mikmod.h>  // 
#include <uniform.h> // mikmod headers
#include <mplayer.h> //
#include "audiere.h"
#include "types.h"
#include "utility.h"


// CRT_CALL should match the C runtime's calling convention

#ifdef WIN32
  #define CRT_CALL __cdecl
#else
  #define CRT_CALL
#endif


namespace audiere {

  class MODInputStream : public UnseekableSource {
  public:
    MODInputStream();
    ~MODInputStream();

    bool initialize(File* file);

    void getFormat(
      int& channel_count,
      int& sample_rate,
      int& bits_per_sample);
    int read(int sample_count, void* samples);
    void reset();

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
    static BOOL ADR_IsThere();
    static BOOL ADR_Init(MDRIVER* md, uint latency, void* optstr);
    static void ADR_Exit(MDRIVER* md);
    static void ADR_Update(MDRIVER* md);
    static BOOL ADR_SetSoftVoices(MDRIVER* md, uint voices);
    static BOOL ADR_SetMode(MDRIVER* md, uint mixspeed, uint mode,
                            uint channels, uint cpumode);
    static void ADR_GetMode(MDRIVER* md, uint* mixspeed, uint* mode,
                            uint* channels, uint* cpumode);

  private:
    enum { SAMPLE_BUFFER_SIZE = 4096 };

    File* m_file;

    // MikMod handles
    MDRIVER* m_driver;
    UNIMOD*  m_module;
    MPLAYER* m_player;

    MMSTREAM m_stream;

    u32  m_samples_left;
    u32* m_next_sample;
    u32  m_sample_buffer[SAMPLE_BUFFER_SIZE];

    bool m_at_eof;

    // mikmod driver description
    static MD_DEVICE drv_audiere;
  };

}


#endif
