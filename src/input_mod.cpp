/*

  How It Works

  
  MikMod/Integration
  --
  We actually implement a MikMod output driver to render PCM
  data.  When AcqRead needs more PCM data, we ask MikMod to
  update until we have enough data.

*/


#include <stdio.h>
#include <string.h>
#include "input_mod.h"
#include "debug.h"
#include "utility.h"


namespace audiere {

  MD_DEVICE MODInputStream::drv_audiere = {
    "Audiere Output",
    "Internal Audiere Output Driver",
    0,
    VC_MAXVOICES,

    NULL,
    NULL,
    NULL,

    // Sample loading
    VC_SampleAlloc,
    VC_SampleGetPtr,
    VC_SampleLoad,
    VC_SampleUnload,
    VC_SampleSpace,
    VC_SampleLength,

    // Detection and initialization
    ADR_IsThere,
    ADR_Init,
    ADR_Exit,
    ADR_Update,
    VC_Preempt,

    NULL,
    ADR_SetSoftVoices,

    ADR_SetMode,
    ADR_GetMode,

    VC_SetVolume,
    VC_GetVolume,

    // Voice control and voice information
    VC_GetActiveVoices,

    VC_VoiceSetVolume,
    VC_VoiceGetVolume,
    VC_VoiceSetFrequency,
    VC_VoiceGetFrequency,
    VC_VoiceSetPosition,
    VC_VoiceGetPosition,
    VC_VoiceSetSurround,
    VC_VoiceSetResonance,

    VC_VoicePlay,
    VC_VoiceResume,
    VC_VoiceStop,
    VC_VoiceStopped,
    VC_VoiceReleaseSustain,

    VC_VoiceRealVolume
  };


  MODInputStream::MODInputStream() {
    m_file = 0;

    m_driver = 0;
    m_module = 0;
    m_player = 0;

    m_samples_left = 0;
    m_next_sample = m_sample_buffer;

    m_at_eof = false;
  }


  MODInputStream::~MODInputStream() {
    // were we initialized successfully?
    if (m_file) {
    
      Player_Free(m_player);
      Unimod_Free(m_module);
      Mikmod_Exit(m_driver);

      delete m_file;
      m_file = 0;
    }
  }


  bool
  MODInputStream::initialize(File* file) {
    m_file = file;

    // first time we run, initialize MikMod
    static bool initialized = false;
    if (!initialized) {
      ADR_GUARD("Initializing MikMod");

      Mikmod_RegisterLoader(load_it);
      Mikmod_RegisterLoader(load_xm);
      Mikmod_RegisterLoader(load_s3m);
      Mikmod_RegisterLoader(load_mod);
      Mikmod_RegisterLoader(load_stm);
    
      Mikmod_RegisterDriver(drv_audiere);

      initialized = true;
    }

    m_stream.fp      = (FILE*)this;
    m_stream.dp      = 0;
    m_stream.iobase  = 0;
    m_stream.seekpos = 0;
    m_stream.fread   = MMRead;
    m_stream.fwrite  = MMWrite;
    m_stream.fgetc   = MMGetC;
    m_stream.fputc   = MMPutC;
    m_stream.fseek   = MMSeek;
    m_stream.ftell   = MMTell;
    m_stream.feof    = MMEof;

    m_samples_left = 0;
    m_next_sample  = m_next_sample;

    m_at_eof = false;

    // create output device
    m_driver = Mikmod_Init(
      44100, 2400, this, MD_STEREO, CPU_AUTODETECT,
      DMODE_16BITS | DMODE_INTERP | DMODE_NOCLICK | DMODE_RESONANCE);
    if (!m_driver) {
      m_file = 0;
      return false;
    }

    ADR_LOG("Mikmod_Init succeeded");

    // load the song
    m_module = Unimod_LoadFP(
      m_driver,
      &m_stream,
      &m_stream,
      MM_STATIC);
    if (!m_module) {
      Mikmod_Exit(m_driver);
      m_driver = 0;
      m_file = 0;
      return false;
    }

    ADR_LOG("Unimod_LoadFP succeeded");

    // load the samples (???)
    if (SL_LoadSamples(m_driver)) {
      Unimod_Free(m_module);
      m_module = 0;
      Mikmod_Exit(m_driver);
      m_driver = 0;
      m_file = 0;
      return false;
    }

    ADR_LOG("SL_LoadSamples succeeded");

    // create a player
    m_player = Player_InitSong(m_module, NULL, 0, 64);
    if (!m_player) {
      Unimod_Free(m_module);
      m_module = 0;
      Mikmod_Exit(m_driver);
      m_driver = 0;
      m_file = 0;
      return false;
    }

    ADR_LOG("Player_InitSong succeeded");

    // start playback of the module
    // we won't actually get samples until the read call
    Player_Start(m_player);

    ADR_LOG("Player_Start succeeded");

    return true;
  }


  void
  MODInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = 2;
    sample_rate   = 44100;
    sample_format = SF_S16_LE;
  }


  int
  MODInputStream::read(int sample_count, void* samples) {
    ADR_GUARD("MODInputStream::read");

    u32* out = static_cast<u32*>(samples);

    int total_written = 0;
    while (sample_count > 0) {

      // if there are no samples in the buffer, tell mikmod to give us a few
      if (m_samples_left == 0) {

        // if the song isn't playing any more, just stop
        if (!Player_Active(m_player)) {
          break;
        }

        Mikmod_Update(m_driver);
      }

      // read data out of the buffer
      u32 samples_to_read = std::min<u32>(
        sample_count,
        m_samples_left);
      memcpy(out, m_next_sample, samples_to_read * 4);

      // update pointers and indices and counts, oh my
      out            += samples_to_read;    
      m_next_sample  += samples_to_read;
      m_samples_left -= samples_to_read;
      sample_count   -= samples_to_read;
      total_written  += samples_to_read;
    }

    return total_written;
  }


  void
  MODInputStream::reset() {
    ADR_GUARD("MOD_Reset");

    m_samples_left = 0;
    m_next_sample  = m_sample_buffer;

    Player_Stop(m_player);
    Player_Start(m_player);
  }


  BOOL
  MODInputStream::ADR_IsThere() {
    return 1;
  }


  BOOL
  MODInputStream::ADR_Init(MDRIVER* md, uint latency, void* optstr) {
    ADR_GUARD("ADR_Init");

    md->device.vc = VC_Init();
    if (!md->device.vc) {
      return 1;
    }

    md->device.local = optstr;
    return 0;
  }


  void
  MODInputStream::ADR_Exit(MDRIVER* md) {
    ADR_GUARD("ADR_Exit");

    VC_Exit(md->device.vc);
  }


  void
  MODInputStream::ADR_Update(MDRIVER* md) {
    ADR_GUARD("ADR_Update");

    MODInputStream* stream = reinterpret_cast<MODInputStream*>(md->device.local);

    // we should only write into the buffer if it's empty
    if (stream->m_samples_left != 0) {
      return;
    }

    VC_WriteBytes(
      md,
      (signed char*)stream->m_sample_buffer,
      SAMPLE_BUFFER_SIZE * sizeof(u32));
    stream->m_samples_left = SAMPLE_BUFFER_SIZE;
    stream->m_next_sample = stream->m_sample_buffer;
  }


  BOOL
  MODInputStream::ADR_SetSoftVoices(MDRIVER* md, uint voices) {
    return VC_SetSoftVoices(md->device.vc, voices);
  }


  BOOL
  MODInputStream::ADR_SetMode(
    MDRIVER* md, uint mixspeed, uint mode,
    uint channels, uint cpumode)
  {
    return VC_SetMode(md->device.vc, mixspeed, mode, channels, cpumode);
  }


  void
  MODInputStream::ADR_GetMode(
    MDRIVER* md, uint* mixspeed, uint* mode,
    uint* channels, uint* cpumode)
  {
    VC_GetMode(md->device.vc, mixspeed, mode, channels, cpumode);
  }


  int CRT_CALL
  MODInputStream::MMRead(
    void* buffer, size_t size, size_t count, FILE* stream)
  {
    MODInputStream* istream = reinterpret_cast<MODInputStream*>(stream);
    int result = istream->m_file->read(buffer, size * count) / size;
    if (result == 0) {
      istream->m_at_eof = true;
    }
    return result;
  }


  int CRT_CALL
  MODInputStream::MMWrite(
    const void* buffer, size_t size, size_t count, FILE* stream)
  {
    // we can't write
    return -1;
  }


  int CRT_CALL
  MODInputStream::MMGetC(FILE* stream) {
    unsigned char c;
    if (MMRead(&c, 1, 1, stream) == 1) {
      return c;
    } else {
     return EOF;
    }
  }


  int CRT_CALL
  MODInputStream::MMPutC(int c, FILE* stream) {
    char ch = (char)c;
    return MMWrite(&ch, 1, 1, stream);
  }


  int CRT_CALL
  MODInputStream::MMSeek(FILE* stream, long offset, int origin) {
    MODInputStream* istream = reinterpret_cast<MODInputStream*>(stream);
    File* file = istream->m_file;

    File::SeekMode seek_mode;
    switch (origin) {
      case SEEK_SET: seek_mode = File::BEGIN;   break;
      case SEEK_CUR: seek_mode = File::CURRENT; break;
      case SEEK_END: seek_mode = File::END;     break;
      default: return -1;
    }

    return file->seek(offset, seek_mode) ? 0 : -1;
  }


  int CRT_CALL
  MODInputStream::MMTell(FILE* stream) {
    MODInputStream* istream = reinterpret_cast<MODInputStream*>(stream);
    File* file = istream->m_file;
    return file->tell();
  }


  int CRT_CALL
  MODInputStream::MMEof(FILE* stream) {
    MODInputStream* istream = reinterpret_cast<MODInputStream*>(stream);
    return istream->m_at_eof ? 1 : 0;
  }

}
