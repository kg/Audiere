#include <string.h>
#include <vorbis/vorbisfile.h>
#include "acq_internal.hpp"
#include "resample.h"  // from MAD source


static const int RESAMPLED_BUFFER_SIZE = 4096;
static const int CONVERSION_BUFFER_SIZE = 4096;


struct OGG_INTERNAL
{
  OggVorbis_File vf;
  acq_u32 location;
  bool eof;
};


typedef ogg_int64_t int64_t;


static size_t FileRead(void* ptr, size_t size, size_t nmemb, void* datasource);
static int    FileSeek(void* datasource, int64_t offset, int whence);
static int    FileClose(void* datasource);
static long   FileTell(void* datasource);


////////////////////////////////////////////////////////////////////////////////

bool OGG_Open(ACQ_INTERNAL_STREAM* stream)
{
  // allocate the internal OGG structure
  OGG_INTERNAL* ogg_internal = new OGG_INTERNAL;
  stream->internal = ogg_internal;
  ogg_internal->location = 0;
  ogg_internal->eof      = false;

  ov_callbacks callbacks;
  callbacks.read_func  = FileRead;
  callbacks.seek_func  = FileSeek;
  callbacks.close_func = FileClose;
  callbacks.tell_func  = FileTell;

  // open ogg vorbis stream
  int result = ov_open_callbacks(stream, &ogg_internal->vf, NULL, 0, callbacks);
  if (result != 0) {
    delete ogg_internal;
    return false;
  }

  // calculate stream type
  vorbis_info* vi = ov_info(&ogg_internal->vf, -1);
  if (!vi) {
    delete ogg_internal;
    return false;
  }

  stream->num_channels    = vi->channels;
  stream->bits_per_sample = 16;
  stream->sample_rate     = vi->rate;

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void OGG_Close(ACQ_INTERNAL_STREAM* stream)
{
  OGG_INTERNAL* ogg_internal = (OGG_INTERNAL*)stream->internal;
  ov_clear(&ogg_internal->vf);
}

////////////////////////////////////////////////////////////////////////////////

int OGG_Read(ACQ_INTERNAL_STREAM* stream, void* samples, const int sample_count)
{
  OGG_INTERNAL* ogg_internal = (OGG_INTERNAL*)stream->internal;
  int sample_size = stream->bits_per_sample * stream->num_channels / 8;

  // if we're at the end of the file, we have no more samples
  if (ogg_internal->eof) {
    return 0;
  }
  
  acq_u8* out = (acq_u8*)samples;

  int samples_left = sample_count;
  int total_read = 0;
  while (samples_left > 0 && !ogg_internal->eof) {

    vorbis_info* vi = ov_info(&ogg_internal->vf, -1);
    if (vi) {

      // if the stream format has changed, treat it as an EndOfStream
      if (stream->sample_rate != vi->rate ||
          stream->num_channels != vi->channels) {
        ogg_internal->eof = true;
        break;
      }

    }

    int bitstream;
    long result = ov_read(
      &ogg_internal->vf,
      (char*)out,
      samples_left * sample_size,
      0,  // little endian
      2,  // 16-bit
      1,  // signed
      &bitstream
    );

    // error, ignore it
    if (result < 0) {
      continue;
    }

    // end of file
    if (result == 0) {
      ogg_internal->eof = true;
    }

    acq_u32 samples_read = (acq_u32)(result / sample_size);

    out += samples_read * sample_size;
    samples_left -= samples_read;
    total_read   += samples_read;
  }

  return total_read;
}

////////////////////////////////////////////////////////////////////////////////

bool OGG_Reset(ACQ_INTERNAL_STREAM* stream)
{
  OGG_Close(stream);
  stream->reset(stream->opaque);
  return OGG_Open(stream);
}

////////////////////////////////////////////////////////////////////////////////

size_t FileRead(void* ptr, size_t size, size_t nmemb, void* datasource)
{
  ACQ_INTERNAL_STREAM* stream = (ACQ_INTERNAL_STREAM*)datasource;
  OGG_INTERNAL* ogg_internal = (OGG_INTERNAL*)stream->internal;

  int bytes_read = stream->read(stream->opaque, ptr, size * nmemb);
  ogg_internal->location += bytes_read;
  return (bytes_read / size);
}

////////////////////////////////////////////////////////////////////////////////

static int64_t Skip(ACQ_INTERNAL_STREAM* stream, int64_t distance)
{
  static const int INPUT_BUFFER_SIZE = 4096;
  acq_u8 dummy[INPUT_BUFFER_SIZE];
  int64_t total_read = 0;

  while (distance > 0) {
    int64_t should_read = acq_min<int64_t>(INPUT_BUFFER_SIZE, distance);
    int64_t read = stream->read(stream->opaque, dummy, (int)should_read);

    // if we read 0, we're at the end
    if (read == 0) {
      break;
    }

    total_read += read;
    distance -= read;
  }

  return total_read;
}

////////////////////////////////////////////////////////////////////////////////

int FileSeek(void* datasource, int64_t offset, int whence)
{
  return -1;  // we can't seek, vorbisfile should still work

/*
  ACQ_INTERNAL_STREAM* stream = (ACQ_INTERNAL_STREAM*)datasource;
  OGG_INTERNAL* ogg_internal = (OGG_INTERNAL*)stream->internal;

  switch (whence) {
    case SEEK_CUR: {
      if (offset < 0) {
        stream->reset(stream->opaque);
        int64_t distance = ogg_internal->location + offset;
        return (Skip(stream, distance) == distance ? 0 : -1);
      } else {
        return (Skip(stream, offset) == offset ? 0 : 1);
      }
    }

    case SEEK_SET: {
      if (offset > ogg_internal->location) {
        int64_t distance = offset - ogg_internal->location;
        return (Skip(stream, distance) == distance ? 0 : -1);
      } else {
        stream->reset(stream->opaque);
        return (Skip(stream, offset) == offset ? 0 : -1);
      }
    }

    // we can't do this
    case SEEK_END: {
      return -1;
    }

    default: {
      return -1;
    }
  }
*/
}

////////////////////////////////////////////////////////////////////////////////

int FileClose(void* /*datasource*/)
{
  // we don't have to do anything
  return 0;
}

////////////////////////////////////////////////////////////////////////////////

long FileTell(void* datasource)
{
  ACQ_INTERNAL_STREAM* stream = (ACQ_INTERNAL_STREAM*)datasource;
  OGG_INTERNAL* ogg_internal = (OGG_INTERNAL*)stream->internal;

  return ogg_internal->location;
}

////////////////////////////////////////////////////////////////////////////////
