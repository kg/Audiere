/*

  How It Works

  
  MikMod/Integration
  --
  We actually implement a MikMod output driver to render PCM
  data.  When AcqRead needs more PCM data, we ask MikMod to
  update until we have enough data.


  File Input/Output
  --
  MikMod wants an array-like file (with seeking).  Acoustique
  simply is given a stream of bytes.  In order to map the byte
  stream into something more palatable for MikMod, we lazily
  build a doubly-linked list of data pages that represent the
  file.  We can do traversals by walking the list in either
  direction.

  Q)  Well, why can't we just read the whole stream into an
      array when we first load the file?
  A)  If the stream has no end (i.e. if it's dynamically
      generated), we'd be loading forever...  Also, appending
      new chunks to the end of the array would be O(n) as we'd
      have to copy the old buffer into the new one each time.

*/


#include <stdio.h>
#include <string.h>
#include <mikmod.h>  // 
#include <uniform.h> // mikmod headers
#include <mplayer.h> //
#include "acq_internal.hpp"


static const int SAMPLE_BUFFER_SIZE = 4096;
static const int MAX_PAGE_SIZE = 64 * 1024;  // 64k


// data page
struct PAGE
{
  PAGE* next; // must be NULL if page_size != MAX_PAGE_SIZE
  PAGE* back;

  int    size;  // amount of |data| that is valid
  acq_u8 data[MAX_PAGE_SIZE];
};


struct MOD_INTERNAL
{
  // MikMod handles
  MDRIVER* driver;
  UNIMOD*  module;
  MPLAYER* player;

  MMSTREAM stream;

  acq_u32  samples_left;
  acq_u32* next_sample;
  acq_u32  sample_buffer[SAMPLE_BUFFER_SIZE];

  // linked list of data pages
  PAGE* first_page;
  PAGE* current_page;
  acq_u32 position;

  bool read_whole_file;  // true if we have already hit EndOfStream
  acq_u32 total_file_size;  // the amount of the file we've read so far
};


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

static void ReadPage(ACQ_STREAM stream);


static MD_DEVICE drv_acq =
{
  "Acoustique Output",
  "Internal Acoustique Output Driver",
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
  ACQ_IsThere,
  ACQ_Init,
  ACQ_Exit,
  ACQ_Update,
  VC_Preempt,

  NULL,
  ACQ_SetSoftVoices,

  ACQ_SetMode,
  ACQ_GetMode,

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


////////////////////////////////////////////////////////////////////////////////

bool MOD_Open(ACQ_STREAM stream)
{
  // first time we run, initialize MikMod
  static bool initialized = false;
  if (!initialized) {
    Mikmod_RegisterLoader(load_it);
    Mikmod_RegisterLoader(load_xm);
    Mikmod_RegisterLoader(load_s3m);
    Mikmod_RegisterLoader(load_mod);
    Mikmod_RegisterLoader(load_stm);
    
    Mikmod_RegisterDriver(drv_acq);

    initialized = true;
  }

  // create internal MOD object
  MOD_INTERNAL* mod_internal = new MOD_INTERNAL;
  
  // initialize outer stream object
  stream->internal        = mod_internal;
  stream->sample_rate     = 44100;
  stream->bits_per_sample = 16;
  stream->num_channels    = 2;

  mod_internal->samples_left = 0;
  mod_internal->next_sample  = NULL;

  // initialize the stream
  mod_internal->stream.fp      = (FILE*)stream;
  mod_internal->stream.dp      = NULL;
  mod_internal->stream.iobase  = 0;
  mod_internal->stream.seekpos = 0;
  mod_internal->stream.fread   = MMRead;
  mod_internal->stream.fwrite  = MMWrite;
  mod_internal->stream.fgetc   = MMGetC;
  mod_internal->stream.fputc   = MMPutC;
  mod_internal->stream.fseek   = MMSeek;
  mod_internal->stream.ftell   = MMTell;
  mod_internal->stream.feof    = MMEof;

  mod_internal->first_page      = NULL;
  mod_internal->current_page    = NULL;
  mod_internal->read_whole_file = false;
  mod_internal->position        = 0;
  mod_internal->total_file_size = 0;

  // create output device
  mod_internal->driver = Mikmod_Init(
    44100, 2400, mod_internal, MD_STEREO, CPU_AUTODETECT,
    DMODE_16BITS | DMODE_INTERP | DMODE_NOCLICK
  );
  if (!mod_internal->driver) {
    delete mod_internal;
    return false;
  }

  // load the song
  mod_internal->module = Unimod_LoadFP(
    mod_internal->driver,
    &mod_internal->stream,
    &mod_internal->stream,
    MM_STATIC
  );
  if (!mod_internal->module) {
    Mikmod_Exit(mod_internal->driver);
    delete mod_internal;
    return false;
  }

  // load the samples (???)
  if (SL_LoadSamples(mod_internal->driver)) {
    Unimod_Free(mod_internal->module);
    Mikmod_Exit(mod_internal->driver);
    delete mod_internal;
    return false;
  }

  // create a player
  mod_internal->player = Player_InitSong(mod_internal->module, NULL, 0, 64);
  if (!mod_internal->player) {
    Unimod_Free(mod_internal->module);
    Mikmod_Exit(mod_internal->driver);
    delete mod_internal;
    return false;
  }

  // start playback of the module
  // we won't actually get samples until the update call
  Player_Start(mod_internal->player);

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void MOD_Close(ACQ_STREAM stream)
{
  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)stream->internal;

  Player_Free(mod_internal->player);
  Unimod_Free(mod_internal->module);
  Mikmod_Exit(mod_internal->driver);

  // destroy the data pages
  PAGE* p = mod_internal->first_page;
  while (p) {
    PAGE* doomed = p;
    p = p->next;
    delete doomed;
  }

  delete mod_internal;
}

////////////////////////////////////////////////////////////////////////////////

int MOD_Read(ACQ_STREAM stream, void* samples, int sample_count)
{
  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)stream->internal;

  acq_u32* out = (acq_u32*)samples;

  int total_written = 0;
  while (sample_count > 0) {

    // if there are no samples in the buffer, tell mikmod to give us a few
    if (mod_internal->samples_left == 0) {

      // if the song isn't playing any more, just stop
      if (!Player_Active(mod_internal->player)) {
        break;
      }

      Mikmod_Update(mod_internal->driver);
    }

    // read data out of the buffer
    acq_u32 samples_to_read = acq_min<acq_u32>(
      sample_count,
      mod_internal->samples_left
    );
    memcpy(out, mod_internal->next_sample, samples_to_read * 4);

    // update pointers and indices and counts, oh my
    out                        += samples_to_read;    
    mod_internal->next_sample  += samples_to_read;
    mod_internal->samples_left -= samples_to_read;
    sample_count               -= samples_to_read;
    total_written              += samples_to_read;
  }

  return total_written;
}

////////////////////////////////////////////////////////////////////////////////

bool MOD_Reset(ACQ_STREAM stream)
{
  // XXX possible optimization: keep data pages that we've already loaded
  MOD_Close(stream);
  stream->reset(stream->opaque);
  return MOD_Open(stream);
}

////////////////////////////////////////////////////////////////////////////////

BOOL ACQ_IsThere()
{
  return 1;
}

////////////////////////////////////////////////////////////////////////////////

BOOL ACQ_Init(MDRIVER* md, uint latency, void* optstr)
{
  md->device.vc = VC_Init();
  if (!md->device.vc) {
    return 1;
  }

  md->device.local = optstr;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_Exit(MDRIVER* md)
{
  VC_Exit(md->device.vc);
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_Update(MDRIVER* md)
{
  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)md->device.local;

  // we should only write into the buffer if it's empty
  if (mod_internal->samples_left != 0) {
    return;
  }

  VC_WriteBytes(
    md,
    (signed char*)mod_internal->sample_buffer,
    SAMPLE_BUFFER_SIZE * sizeof(acq_u32)
  );
  mod_internal->samples_left = SAMPLE_BUFFER_SIZE;
  mod_internal->next_sample = mod_internal->sample_buffer;
}

////////////////////////////////////////////////////////////////////////////////

BOOL ACQ_SetSoftVoices(MDRIVER* md, uint voices)
{
  return VC_SetSoftVoices(md->device.vc, voices);
}

////////////////////////////////////////////////////////////////////////////////

BOOL ACQ_SetMode(MDRIVER* md, uint mixspeed, uint mode,
                 uint channels, uint cpumode)
{
  return VC_SetMode(md->device.vc, mixspeed, mode, channels, cpumode);
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_GetMode(MDRIVER* md, uint* mixspeed, uint* mode,
                 uint* channels, uint* cpumode)
{
  VC_GetMode(md->device.vc, mixspeed, mode, channels, cpumode);
}

////////////////////////////////////////////////////////////////////////////////

int CRT_CALL MMRead(void* buffer, size_t size, size_t count, FILE* stream)
{
  ACQ_STREAM internal = (ACQ_STREAM)stream;
  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)internal->internal;

  // SETUP

  acq_u32 beginning = mod_internal->position;
  acq_u32 end       = mod_internal->position + size * count;

  // if the end is past what we've read, try to read more
  while (!mod_internal->read_whole_file &&
         end >= mod_internal->total_file_size) {
    ReadPage(internal);
  }

  // if the end is still too far out, truncate the read
  if (end > mod_internal->total_file_size) {
    end = mod_internal->total_file_size;
  }

  // READ

  acq_u8* out = (acq_u8*)buffer;
  acq_u32 bytes_left = end - beginning;

  // where we are in the current page
  acq_u32 offset = mod_internal->position % MAX_PAGE_SIZE;
  
  while (bytes_left > 0) {

    // calculate how much to read from this page
    acq_u32 bytes_left_in_page = MAX_PAGE_SIZE - offset;
    acq_u32 bytes_to_read = acq_min(bytes_left, bytes_left_in_page);

    memcpy(out, mod_internal->current_page->data + offset, bytes_to_read);

    // should we skip to the next page?
    if (offset + bytes_to_read == MAX_PAGE_SIZE) {
      mod_internal->current_page = mod_internal->current_page->next;
    }

    // update pointers and indices
    out += bytes_to_read;
    offset = (offset + bytes_to_read) % MAX_PAGE_SIZE;
    bytes_left -= bytes_to_read;
    mod_internal->position += bytes_to_read;
  }

  // how many bytes we've read
  return mod_internal->position - beginning;
}

////////////////////////////////////////////////////////////////////////////////

int CRT_CALL MMWrite(const void* buffer, size_t size, size_t count, FILE* stream)
{
  // we can't write
  return -1;
}

////////////////////////////////////////////////////////////////////////////////

int CRT_CALL MMGetC(FILE* stream)
{
  unsigned char c;
  if (MMRead(&c, 1, 1, stream) == 1) {
    return c;
  } else {
    return EOF;
  }
}

////////////////////////////////////////////////////////////////////////////////

int CRT_CALL MMPutC(int c, FILE* stream)
{
  char ch = (char)c;
  return MMWrite(&ch, 1, 1, stream);
}

////////////////////////////////////////////////////////////////////////////////

int CRT_CALL MMSeek(FILE* stream, long offset, int origin)
{
  ACQ_STREAM internal = (ACQ_STREAM)stream;
  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)internal->internal;

  // calculate destination position
  long pos;
  switch (origin) {
    case SEEK_SET: {
      pos = offset;
    } break;

    case SEEK_CUR: {
      pos = mod_internal->position + offset;
    } break;

    // not supported  
    case SEEK_END:
    default: {
      return -1;
    }
  }

  // can't seek to a negative position
  if (pos < 0) {
    return -1;
  }

  // we're unsigned now
  acq_u32 position = (acq_u32)pos;

  // if we're not at the end of the file, and we haven't read enough, read more
  while (!mod_internal->read_whole_file &&
         position >= mod_internal->total_file_size) {

    ReadPage(internal);
  }

  // if the file simply isn't big enough, fail
  if (position >= mod_internal->total_file_size) {
    return -1;
  }

  // seek backwards
  if (position < mod_internal->position) {

    int cur_page = mod_internal->position / MAX_PAGE_SIZE;
    int dst_page = position               / MAX_PAGE_SIZE;

    while (cur_page > dst_page) {
      mod_internal->current_page = mod_internal->current_page->back;
      cur_page--;
    }

  // seek forwards
  } else if (position > mod_internal->position) {

    int cur_page = mod_internal->position / MAX_PAGE_SIZE;
    int dst_page = position               / MAX_PAGE_SIZE;

    while (cur_page < dst_page) {
      mod_internal->current_page = mod_internal->current_page->next;
      cur_page++;
    }
  }

  mod_internal->position = position;
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

int CRT_CALL MMTell(FILE* stream)
{
  ACQ_STREAM internal = (ACQ_STREAM)stream;
  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)internal->internal;

  return mod_internal->position;
}

////////////////////////////////////////////////////////////////////////////////

int CRT_CALL MMEof(FILE* stream)
{
  ACQ_STREAM internal = (ACQ_STREAM)stream;
  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)internal->internal;
  
  return (
    mod_internal->read_whole_file &&
    (mod_internal->position == mod_internal->total_file_size)
  );
}

////////////////////////////////////////////////////////////////////////////////

void ReadPage(ACQ_STREAM stream)
{
  // XXX optimization - use current page rather than head to search

  MOD_INTERNAL* mod_internal = (MOD_INTERNAL*)stream->internal;

  PAGE* page = new PAGE;
  page->next = NULL;
  page->size = stream->read(stream->opaque, page->data, MAX_PAGE_SIZE);

  // if we hit the end of the stream (page->size == 0), allocate a new page anyway

  // otherwise, stick this page on the end of the list
  if (!mod_internal->first_page) {

    page->back = NULL;
    mod_internal->first_page = page;

    // assume we don't have a current page yet either
    mod_internal->current_page = page;

  } else {

    // find the end
    PAGE* p = mod_internal->first_page;
    while (p->next) {
      p = p->next;
    }

    page->back = p;
    p->next = page;
    
  }

  mod_internal->total_file_size += page->size;
  mod_internal->read_whole_file = (page->size != MAX_PAGE_SIZE);
}

////////////////////////////////////////////////////////////////////////////////
