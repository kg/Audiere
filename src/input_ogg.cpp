#include <string.h>
#include "input_ogg.hpp"
#include "config.h"
#include "utility.hpp"


struct OGG_INTERNAL
{
  OggVorbis_File vf;
  adr_u32 location;
  bool eof;
};


typedef ogg_int64_t int64_t;


////////////////////////////////////////////////////////////////////////////////

OGGInputStream::OGGInputStream()
{
  m_file = 0;
//  m_eof  = false;

  m_channel_count   = 0;
  m_sample_rate     = 0;
  m_bits_per_sample = 0;
}

////////////////////////////////////////////////////////////////////////////////

OGGInputStream::~OGGInputStream()
{
  // were we initialized successfully?
  if (m_file) {
    ov_clear(&m_vorbis_file);

    delete m_file;
    m_file = 0;
  }
}

////////////////////////////////////////////////////////////////////////////////

bool
OGGInputStream::Initialize(IFile* file)
{
  m_file = file;

  // custom ogg vorbis callbacks
  ov_callbacks callbacks;
  callbacks.read_func  = FileRead;
  callbacks.seek_func  = FileSeek;
  callbacks.close_func = FileClose;
  callbacks.tell_func  = FileTell;

  // open ogg vorbis stream
  int result = ov_open_callbacks(file, &m_vorbis_file, 0, 0, callbacks);
  if (result) {
    m_file = 0;
    return false;
  }

  // calculate stream type
  vorbis_info* vi = ov_info(&m_vorbis_file, -1);
  if (!vi) {
    ov_clear(&m_vorbis_file);
    m_file = 0;
    return false;
  }

  m_channel_count   = vi->channels;
  m_bits_per_sample = 16;
  m_sample_rate     = vi->rate;

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void
OGGInputStream::GetFormat(
  int& channel_count,
  int& sample_rate,
  int& bits_per_sample)
{
  channel_count   = m_channel_count;
  sample_rate     = m_sample_rate;
  bits_per_sample = m_bits_per_sample;
}

////////////////////////////////////////////////////////////////////////////////

int
OGGInputStream::Read(int sample_count, void* samples)
{
  int sample_size = m_bits_per_sample * m_channel_count / 8;

  // if we're at the end of the file, we have no more samples
//  if (m_eof) {
//    return 0;
//  }
  
  adr_u8* out = (adr_u8*)samples;

  int samples_left = sample_count;
  int total_read = 0;
  while (samples_left > 0 /*&& !m_eof*/) {

    // check to see if the stream format has changed
    // if so, treat it as an EndOfStream
    vorbis_info* vi = ov_info(&m_vorbis_file, -1);
    if (vi && (m_sample_rate != vi->rate || m_channel_count != vi->channels)) {
//      m_eof = true;
      break;
    }

    int bitstream;
    long result = ov_read(
      &m_vorbis_file,
      (char*)out,
      samples_left * sample_size,
      0,  // little endian
      2,  // 16-bit
      1,  // signed
      &bitstream);

    if (result < 0) {
      // if error, ignore it
      continue;
    } else if (result == 0) {
//      m_eof = true;
      break;
    }

    adr_u32 samples_read = (adr_u32)(result / sample_size);

    out += samples_read * sample_size;
    samples_left -= samples_read;
    total_read   += samples_read;
  }

  return total_read;
}

////////////////////////////////////////////////////////////////////////////////

bool
OGGInputStream::Reset()
{
  return (0 == ov_pcm_seek(&m_vorbis_file, 0));
}

////////////////////////////////////////////////////////////////////////////////

size_t
OGGInputStream::FileRead(void* buffer, size_t size, size_t n, void* opaque)
{
  IFile* file = reinterpret_cast<IFile*>(opaque);
  return file->Read(buffer, size * n) / size;
}

////////////////////////////////////////////////////////////////////////////////

int
OGGInputStream::FileSeek(void* opaque, int64_t offset, int whence)
{
  IFile* file = reinterpret_cast<IFile*>(opaque);
  ADR_SEEK_TYPE type;
  switch (whence) {
    case SEEK_SET: type = ADR_BEGIN;   break;
    case SEEK_CUR: type = ADR_CURRENT; break;
    case SEEK_END: type = ADR_END;     break;
    default: return -1;
  }
  return (file->Seek((int)offset, type) ? 0 : -1);
}

////////////////////////////////////////////////////////////////////////////////

int
OGGInputStream::FileClose(void* /*opaque*/)
{
  // we don't have to do anything
  // (read: don't trust ogg vorbis with handling file closes)
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

long
OGGInputStream::FileTell(void* opaque)
{
  IFile* file = reinterpret_cast<IFile*>(opaque);
  return file->Tell();
}

////////////////////////////////////////////////////////////////////////////////
