#include <string.h>
#include "acq_internal.hpp"
#include "endian.hpp"


static const acq_u32 INPUT_BUFFER_SIZE = 4096;


struct WAV_INTERNAL
{
  bool in_data_chunk;

  // these are only relevant if in_data_chunk is true
  int bytes_left_in_chunk;
  int bytes_left_in_buffer;
  acq_u8 buffer[INPUT_BUFFER_SIZE];

  bool at_eof;
};


////////////////////////////////////////////////////////////////////////////////

static inline acq_u16 Read16(acq_u8* m) {
  return (acq_u16)(
    (m[0] << 0) +
    (m[1] << 8)
  );
}

static inline acq_u32 Read32(acq_u8* m) {
  return (acq_u32)(
    (m[0] << 0) +
    (m[1] << 8) +
    (m[2] << 16)  +
    (m[3] << 24)
  );
}

static inline bool IsValidSampleRate(acq_u32 rate) {
  return (
    rate == 48000 ||
    rate == 44100 ||
    rate == 32000 ||
    rate == 24000 ||
    rate == 22050 ||
    rate == 16000 ||
    rate == 12000 ||
    rate == 11025 ||
    rate == 8000
  );
}

static inline bool IsValidSampleSize(acq_u32 size) {
  return (size == 8 || size == 16);
}

////////////////////////////////////////////////////////////////////////////////

static acq_u32 SkipBytes(ACQ_INTERNAL_STREAM* stream, acq_u32 size)
{
  acq_u8 dummy[INPUT_BUFFER_SIZE];  // may as well use the input buffer size
  int total_read = 0;

  while (size > 0) {
    int should_read = acq_min<acq_u32>(INPUT_BUFFER_SIZE, size);
    int read = stream->read(stream->opaque, dummy, should_read);

    // if we read 0, we're at the end
    if (read == 0) {
      break;
    }

    total_read += read;
    size -= read;
  }

  return total_read;
}

////////////////////////////////////////////////////////////////////////////////

bool WAV_Open(ACQ_INTERNAL_STREAM* stream)
{
  WAV_INTERNAL* wav_internal = new WAV_INTERNAL;
  wav_internal->in_data_chunk        = false;
  wav_internal->bytes_left_in_chunk  = 0;
  wav_internal->bytes_left_in_buffer = 0;
  wav_internal->at_eof               = false;

  // read the RIFF header
  char    riff_id[4];
  acq_u32 riff_length;
  char    riff_datatype[4];

  acq_u32 size = 0;
  size += stream->read(stream->opaque, riff_id, 4);
  size += stream->read(stream->opaque, &riff_length, 4);
  size += stream->read(stream->opaque, riff_datatype, 4);

  riff_length = LittleToHost32(riff_length);

  if (size != 12 ||
      memcmp(riff_id, "RIFF", 4) != 0 ||
      riff_length == 0 ||
      memcmp(riff_datatype, "WAVE", 4) != 0) {

    delete wav_internal;
    return false;
  }

  // search for a format chunk
  bool has_format = false;
  while (!has_format) {
    char    chunk_id[4];
    acq_u32 chunk_length;
    size = 0;

    size += stream->read(stream->opaque, chunk_id, 4);
    size += stream->read(stream->opaque, &chunk_length, 4);
    chunk_length = LittleToHost32(chunk_length);

    // if we couldn't read enough, we're done
    if (size != 8) {
      break;
    }

    // if we found a format chunk, excellent!
    if (memcmp(chunk_id, "fmt ", 4) == 0 &&
        chunk_length >= 16) {

      // read format chunk
      acq_u8 chunk[16];
      size = stream->read(stream->opaque, chunk, 16);
      chunk_length -= size;

      // we couldn't read the entire format chunk...
      if (size < 16) {
        break;
      }

      // parse the memory into useful information
      acq_u16 format_tag         = Read16(chunk + 0);
      acq_u16 channel_count      = Read16(chunk + 2);
      acq_u32 samples_per_second = Read32(chunk + 4);
      acq_u32 bytes_per_second   = Read32(chunk + 8);
      acq_u16 block_align        = Read16(chunk + 12);
      acq_u16 bits_per_sample    = Read16(chunk + 14);

      // format_tag must be 1 (WAVE_FORMAT_PCM)
      // we only support mono and stereo
      if (format_tag != 1 ||
          channel_count > 2 ||
          !IsValidSampleRate(samples_per_second) ||
          !IsValidSampleSize(bits_per_sample)) {
        break;
      }

      // skip the rest of the chunk
      if (SkipBytes(stream, chunk_length) != chunk_length) {
        // oops, end of stream
        break;
      }

      // set format
      stream->num_channels    = channel_count;
      stream->bits_per_sample = bits_per_sample;
      stream->sample_rate     = samples_per_second;
      has_format = true;

    } else {

      // skip the rest of the chunk
      if (SkipBytes(stream, chunk_length) != chunk_length) {
        // oops, end of stream
        break;
      }

    }
  }

  // if we never found a format, stop
  if (!has_format) {
    delete wav_internal;
    return false;
  }


  stream->internal = wav_internal;
  return true;
}

////////////////////////////////////////////////////////////////////////////////

void WAV_Close(ACQ_INTERNAL_STREAM* stream)
{
  WAV_INTERNAL* wav_internal = (WAV_INTERNAL*)stream->internal;
  delete wav_internal;
}

////////////////////////////////////////////////////////////////////////////////

int WAV_Read(ACQ_INTERNAL_STREAM* stream, void* samples, const int sample_count)
{
  WAV_INTERNAL* wav_internal = (WAV_INTERNAL*)stream->internal;
  acq_u8* out = (acq_u8*)samples;

  // calculate size in bytes per sample
  const int sample_size = stream->num_channels * stream->bits_per_sample / 8;

  // while we still have samples to output...
  int samples_left = sample_count;
  while (samples_left > 0) {

    // if we have no data in the buffer, read some
    if (wav_internal->bytes_left_in_buffer < sample_size) {

      // if we're at the end of the file, there's nothing else to read
      if (wav_internal->at_eof) {
        return 0;
      }

      // if there are no bytes left in the chunk, we're not in 'data' anymore
      if (wav_internal->bytes_left_in_chunk == 0) {
        wav_internal->in_data_chunk = false;
      }

      // if we're not in a data chunk, find one
      while (!wav_internal->in_data_chunk) {
        
        acq_u32 chunk_length;
        char    chunk_id[4];
        int     size = 0;

        size += stream->read(stream->opaque, chunk_id, 4);
        size += stream->read(stream->opaque, &chunk_length, 4);
        if (size < 8) {
          // we're at the end of the file...
          wav_internal->at_eof = true;
          return (sample_count - samples_left);
        }

        chunk_length = LittleToHost32(chunk_length);

        // test to see if it's a data chunk
        if (memcmp(chunk_id, "data", 4) == 0) {
          wav_internal->in_data_chunk        = true;
          wav_internal->bytes_left_in_chunk  = chunk_length;
          wav_internal->bytes_left_in_buffer = 0;

        } else {
          // skip the chunk and try again
          acq_u32 bytes_skipped = SkipBytes(stream, chunk_length);

          // end of file?
          if (bytes_skipped < chunk_length) {
            wav_internal->at_eof = true;
            return (sample_count - samples_left);
          }
        }

      } // done searching for chunk
      
      // now try to grab some data from it
      int bytes_to_read = acq_min<int>(
        wav_internal->bytes_left_in_chunk,
        INPUT_BUFFER_SIZE - wav_internal->bytes_left_in_buffer
      );
      int bytes_read = stream->read(
        stream->opaque,
        wav_internal->buffer + wav_internal->bytes_left_in_buffer,
        bytes_to_read
      );
      if (bytes_read < bytes_to_read) {
        wav_internal->at_eof = true;
      }

      wav_internal->bytes_left_in_buffer += bytes_read;
      wav_internal->bytes_left_in_chunk -= bytes_read;
    }

    // copy the data from the input byte buffer into the output sample buffer
    int bytes_left = wav_internal->bytes_left_in_buffer;
    const int samples_in_buffer = bytes_left / sample_size;
    acq_u8* start = wav_internal->buffer + INPUT_BUFFER_SIZE - bytes_left;

    int samples_to_copy = acq_min(samples_in_buffer, samples_left);
    int copy_size = samples_to_copy * sample_size;
    memcpy(out, start, copy_size);
    out += copy_size;
    wav_internal->bytes_left_in_buffer -= copy_size;
    samples_left -= samples_to_copy;
  }

  // if we got here, we read all of the samples we needed to
  return sample_count;
}

////////////////////////////////////////////////////////////////////////////////

bool WAV_Reset(ACQ_INTERNAL_STREAM* stream)
{
  WAV_Close(stream);

  // reset the stream
  stream->reset(stream->opaque);

  return WAV_Open(stream);
}

////////////////////////////////////////////////////////////////////////////////
