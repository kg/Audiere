#include <stddef.h>
#include "acoustique.h"
#include "acq_internal.hpp"
#include "debug.hpp"


#define ACQ_VERSION "1.0.3"


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
  ACQ_GUARD("AcqOpenStream");

  // if callbacks are invalid, return failure
  if (!read || !reset) {
    return NULL;
  }

  // if we're asked to autodetect, simply try each type
  // check MP3 last, because it'll probably say any file is okay
  if (stream_format == ACQ_STREAM_AUTODETECT) {

    ACQ_STREAM s;

    #define TRY_STREAM_TYPE(type)                   \
      s = AcqOpenStream(opaque, read, reset, type); \
      if (s) {                                      \
        return s;                                   \
      } else {                                      \
        reset(opaque);                              \
      }

    TRY_STREAM_TYPE(ACQ_STREAM_OGG);
    TRY_STREAM_TYPE(ACQ_STREAM_WAV);
    TRY_STREAM_TYPE(ACQ_STREAM_MOD);
    #ifdef WITH_MP3
      TRY_STREAM_TYPE(ACQ_STREAM_MP3);
    #endif

    return NULL;
  }

  // create the new stream object
  ACQ_STREAM stream = new ACQ_STREAMimp;
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

#ifdef WITH_MP3
    // MP3
    case ACQ_STREAM_MP3: {
      stream->stream_open  = MP3_Open;
      stream->stream_close = MP3_Close;
      stream->stream_read  = MP3_Read;
      stream->stream_reset = MP3_Reset;
    } break;
#endif

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

  ACQ_LOG("Initializing stream");

  // initialize the stream
  if (!stream->stream_open(stream)) {
    delete stream;
    return NULL;
  }

  ACQ_LOG("Stream opened successfully");

  return stream;
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_CALL AcqCloseStream(ACQ_STREAM stream)
{
  ACQ_GUARD("AcqCloseStream");

  stream->stream_close(stream);
  delete stream;
}

////////////////////////////////////////////////////////////////////////////////

void ACQ_CALL AcqGetStreamInformation(
  ACQ_STREAM stream,
  int* num_channels,
  int* bits_per_sample,
  int* sample_rate)
{
  *num_channels    = stream->num_channels;
  *bits_per_sample = stream->bits_per_sample;
  *sample_rate     = stream->sample_rate;
}

////////////////////////////////////////////////////////////////////////////////

int ACQ_CALL AcqReadStream(ACQ_STREAM stream, void* samples, int sample_count)
{
  ACQ_GUARD("AcqReadStream");

  if (sample_count <= 0) {
    return 0;
  }

  return stream->stream_read(stream, samples, sample_count);
}

////////////////////////////////////////////////////////////////////////////////

ACQ_BOOL ACQ_CALL AcqResetStream(ACQ_STREAM stream)
{
  ACQ_GUARD("AcqResetStream");

  return (stream->stream_reset(stream) != true);
}

////////////////////////////////////////////////////////////////////////////////
