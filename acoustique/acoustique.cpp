#include <stddef.h>
#include "acoustique.h"
#include "acq_internal.hpp"


#define ACQ_VERSION "1.01"


////////////////////////////////////////////////////////////////////////////////

const char* ACQ_CALL AcqGetVersion()
{
  return ACQ_VERSION;
}

////////////////////////////////////////////////////////////////////////////////

ACQ_STREAM ACQ_CALL AcqOpenStream(
  void* opaque,
  ACQ_READ_CALLBACK read,
  ACQ_RESET_CALLBACK reset,
  int stream_format)
{
  // if callbacks are invalid, return failure
  if (!read || !reset) {
    return NULL;
  }

  // if we're asked to autodetect, simply try each type
  // check MP3 last, because it'll probably say any file is okay
  if (stream_format == ACQ_STREAM_AUTODETECT) {

    // OGG
    ACQ_STREAM s = AcqOpenStream(opaque, read, reset, ACQ_STREAM_OGG);
    if (s) {
      return s;
    } else {
      reset(opaque);
    }

    // WAV
    s = AcqOpenStream(opaque, read, reset, ACQ_STREAM_WAV);
    if (s) {
      return s;
    } else {
      reset(opaque);
    }

    // MOD
    s = AcqOpenStream(opaque, read, reset, ACQ_STREAM_MOD);
    if (s) {
      return s;
    } else {
      reset(opaque);
    }

    // MP3
    return AcqOpenStream(opaque, read, reset, ACQ_STREAM_MP3);
  }

  // create the new stream object
  ACQ_INTERNAL_STREAM* stream = new ACQ_INTERNAL_STREAM;
  stream->opaque        = opaque;
  stream->read          = read;
  stream->reset         = reset;
  stream->stream_format = stream_format;

  // initialize state pattern (Design Patterns!) callbacks
  switch (stream_format) {
    
    // OGG
    case ACQ_STREAM_OGG: {
      stream->stream_open  = OGG_Open;
      stream->stream_close = OGG_Close;
      stream->stream_read  = OGG_Read;
      stream->stream_reset = OGG_Reset;
    } break;

    // MP3
    case ACQ_STREAM_MP3: {
      stream->stream_open  = MP3_Open;
      stream->stream_close = MP3_Close;
      stream->stream_read  = MP3_Read;
      stream->stream_reset = MP3_Reset;
    } break;

    // WAV
    case ACQ_STREAM_WAV: {
      stream->stream_open  = WAV_Open;
      stream->stream_close = WAV_Close;
      stream->stream_read  = WAV_Read;
      stream->stream_reset = WAV_Reset;
    } break;

    // MOD
    case ACQ_STREAM_MOD: {
      stream->stream_open  = MOD_Open;
      stream->stream_close = MOD_Close;
      stream->stream_read  = MOD_Read;
      stream->stream_reset = MOD_Reset;
    } break;

    // FAIL!
    default: {
      delete stream;
      return NULL;
    }
  }

  // initialize the stream
  if (!stream->stream_open(stream)) {
    delete stream;
    return NULL;
  }

  return stream;
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_CALL AcqCloseStream(ACQ_STREAM s)
{
  ACQ_INTERNAL_STREAM* stream = (ACQ_INTERNAL_STREAM*)s;
  stream->stream_close(stream);
  delete stream;
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_CALL AcqGetStreamInformation(
  ACQ_STREAM s,
  int* num_channels,
  int* bits_per_sample,
  int* sample_rate)
{
  ACQ_INTERNAL_STREAM* stream = (ACQ_INTERNAL_STREAM*)s;
  *num_channels    = stream->num_channels;
  *bits_per_sample = stream->bits_per_sample;
  *sample_rate     = stream->sample_rate;
}

////////////////////////////////////////////////////////////////////////////////

int ACQ_CALL AcqReadStream(ACQ_STREAM s, void* samples, int sample_count)
{
  if (sample_count <= 0) {
    return 0;
  }

  ACQ_INTERNAL_STREAM* stream = (ACQ_INTERNAL_STREAM*)s;
  return stream->stream_read(stream, samples, sample_count);
}

////////////////////////////////////////////////////////////////////////////////

ACQ_BOOL ACQ_CALL AcqResetStream(ACQ_STREAM s)
{
  ACQ_INTERNAL_STREAM* stream = (ACQ_INTERNAL_STREAM*)s;
  return (stream->stream_reset(stream) != true);
}

////////////////////////////////////////////////////////////////////////////////
