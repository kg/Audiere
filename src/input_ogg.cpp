#include <string.h>
#include "input_ogg.h"
#include "types.h"
#include "utility.h"


namespace audiere {

  typedef ogg_int64_t int64_t;


  OGGInputStream::OGGInputStream() {
    m_file = 0;

    m_channel_count = 0;
    m_sample_rate   = 0;
    m_sample_format = SF_S16_LE;
  }


  OGGInputStream::~OGGInputStream() {
    // were we initialized successfully?
    if (m_file) {
      ov_clear(&m_vorbis_file);
    }
  }


  bool
  OGGInputStream::initialize(File* file) {
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

    m_channel_count = vi->channels;
    m_sample_rate   = vi->rate;
    m_sample_format = SF_S16_LE; // see constructor

    return true;
  }


  void
  OGGInputStream::getFormat(
    int& channel_count,
    int& sample_rate,
    SampleFormat& sample_format)
  {
    channel_count = m_channel_count;
    sample_rate   = m_sample_rate;
    sample_format = m_sample_format;
  }


  int
  OGGInputStream::read(int sample_count, void* samples) {
    int sample_size = m_channel_count * GetSampleSize(m_sample_format);

    // if we're at the end of the file, we have no more samples
  //  if (m_eof) {
  //    return 0;
  //  }
  
    u8* out = (u8*)samples;

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

      u32 samples_read = (u32)(result / sample_size);

      out += samples_read * sample_size;
      samples_left -= samples_read;
      total_read   += samples_read;
    }

    return total_read;
  }


  void
  OGGInputStream::reset() {
    ov_pcm_seek(&m_vorbis_file, 0);
  }


  size_t
  OGGInputStream::FileRead(void* buffer, size_t size, size_t n, void* opaque) {
    File* file = reinterpret_cast<File*>(opaque);
    return file->read(buffer, size * n) / size;
  }


  int
  OGGInputStream::FileSeek(void* opaque, ogg_int64_t offset, int whence) {
    File* file = reinterpret_cast<File*>(opaque);
    File::SeekMode type;
    switch (whence) {
      case SEEK_SET: type = File::BEGIN;   break;
      case SEEK_CUR: type = File::CURRENT; break;
      case SEEK_END: type = File::END;     break;
      default: return -1;
    }
    return (file->seek((int)offset, type) ? 0 : -1);
  }


  int
  OGGInputStream::FileClose(void* /*opaque*/) {
    // we don't have to do anything
    // (read: don't trust ogg vorbis with handling file closes)
    return 0;
  }


  long
  OGGInputStream::FileTell(void* opaque) {
    File* file = reinterpret_cast<File*>(opaque);
    return file->tell();
  }

}
