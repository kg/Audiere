#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "acq_internal.hpp"
#include "mad.h"
#include "resample.h"


static const int INPUT_BUFFER_SIZE  = 4096;
static const int SAMPLE_SIZE        = 4;


struct MP3_INTERNAL
{
  mad_stream stream;
  mad_frame  frame;
  mad_synth  synth;

  // I'm not sure if I'm using MAD_BUFFER_GUARD correctly...
  acq_u8 buffer[INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD];

  // 1152 = number of samples in an MP3 frame
  // 6 = maximum resample factor

  int resampled_buffer_size;
  mad_fixed_t resampled_buffer_left [1152 * 6];
  mad_fixed_t resampled_buffer_right[1152 * 6];
  int current_output;  // offset from beginning of arrays

  resample_state rs;
  unsigned source_rate;  
};



// resamples MP3 frame from its format into 44100
static void ResampleFrame(MP3_INTERNAL* mp3_internal);


// renders MAD PCM samples into our desired format
static void RenderSamples(
  MP3_INTERNAL* mp3_internal,
  acq_s16* output,
  const int num_samples);


////////////////////////////////////////////////////////////////////////////////

bool MP3_Open(ACQ_INTERNAL_STREAM* stream)
{
  // initialize Acoustique PCM format flags
  //   MP3 supports three sample rates: 44100, 32000, and 48000 (maybe more?)
  //   however, we're just going to always return 44100 Hz audio
  stream->num_channels    = 2;
  stream->bits_per_sample = 16;
  stream->sample_rate     = 44100;

  MP3_INTERNAL* mp3_internal = new MP3_INTERNAL;
  stream->internal = mp3_internal;

  // zero-initalize the input buffer
  memset(mp3_internal->buffer, 0, INPUT_BUFFER_SIZE + MAD_BUFFER_GUARD);

  mp3_internal->resampled_buffer_size = 0;
  mp3_internal->current_output        = 0;

  // initialize the decoder
  mad_stream_init(&mp3_internal->stream);
  mad_frame_init (&mp3_internal->frame);
  mad_synth_init (&mp3_internal->synth);

  // stream a little of the file in
  int byte_count = stream->read(
    stream->opaque,
    mp3_internal->buffer,
    INPUT_BUFFER_SIZE
  );
  mad_stream_buffer(&mp3_internal->stream, mp3_internal->buffer, byte_count);

  // initialize resampling engine
  resample_init(&mp3_internal->rs, 44100, 44100);
  mp3_internal->source_rate = 44100;  // default source rate

  return true;
}

////////////////////////////////////////////////////////////////////////////////

void MP3_Close(ACQ_INTERNAL_STREAM* stream)
{
  MP3_INTERNAL* mp3_internal = (MP3_INTERNAL*)stream->internal;

  // destroy the decoder
  resample_finish  (&mp3_internal->rs);
  mad_synth_finish (&mp3_internal->synth);
  mad_frame_finish (&mp3_internal->frame);
  mad_stream_finish(&mp3_internal->stream);

  delete mp3_internal;
}

////////////////////////////////////////////////////////////////////////////////

int MP3_Read(ACQ_INTERNAL_STREAM* stream, void* samples, int sample_count)
{
  MP3_INTERNAL* mp3_internal = (MP3_INTERNAL*)stream->internal;
  mad_stream& c_stream = mp3_internal->stream;
  mad_frame&  c_frame  = mp3_internal->frame;
  mad_synth&  c_synth  = mp3_internal->synth;

  // initialize synth -> PCM render loop
  acq_u8* pcm_output = (acq_u8*)samples;
  int samples_left = sample_count;

  // while there are still samples to render
  while (samples_left > 0) {

    // if the synthesis buffer is empty, refill it
    if (mp3_internal->resampled_buffer_size == 0) {

      bool do_continue = false;

      // read data until we have a full frame
      int result = mad_frame_decode(&c_frame, &c_stream);
      while (result == -1) {

        // if the buffer was too small, read more data and try again
        if (c_stream.error == MAD_ERROR_BUFLEN) {

          // slide the unused chunk forward
          int tail_end = c_stream.bufend - c_stream.next_frame;
          memmove(
            mp3_internal->buffer,
            c_stream.next_frame,
            tail_end
          );

          // read the next chunk of data
          int length = stream->read(
            stream->opaque,
            mp3_internal->buffer + tail_end,
            INPUT_BUFFER_SIZE - tail_end
          );

          // if there are no more bytes to read, stop decoding
          if (length == 0) {
            return (sample_count - samples_left);
          }

          // add the bytes to the stream
          mad_stream_buffer(
            &c_stream,
            mp3_internal->buffer,
            length + tail_end
          );

          // attempt to decode the frame again
          result = mad_frame_decode(&c_frame, &c_stream);

        // otherwise, stop the decoding process
        } else if (!MAD_RECOVERABLE(c_stream.error)) {

          // skip this frame
          return (sample_count - samples_left);

        } else {

          do_continue = true;
          break;

        }
      }

      if (do_continue) {
        continue;
      }
      
      // synthesize the frame
      mad_synth_frame(&c_synth, &c_frame);
      ResampleFrame(mp3_internal);

      // if the output buffer is empty (resampling failed), just stop
      if (mp3_internal->resampled_buffer_size == 0) {
        return (sample_count - samples_left);
      }
    }
    
    // render the data in the synthesis buffer
    int samples_to_render = acq_min(
      samples_left,
      mp3_internal->resampled_buffer_size
    );
    RenderSamples(
      mp3_internal,
      (acq_s16*)pcm_output,
      samples_to_render
    );

    // update the output pointers
    pcm_output += samples_to_render * SAMPLE_SIZE;
    samples_left -= samples_to_render;
  }

  return sample_count;
}

////////////////////////////////////////////////////////////////////////////////

bool MP3_Reset(ACQ_INTERNAL_STREAM* stream)
{
  MP3_Close(stream);

  // reset the stream
  stream->reset(stream->opaque);

  return MP3_Open(stream);
}

////////////////////////////////////////////////////////////////////////////////

void ResampleFrame(MP3_INTERNAL* mp3_internal)
{
  mad_synth::mad_pcm& pcm = mp3_internal->synth.pcm;

  // if the sampling rate changed, reallocate the resampling engine
  if (pcm.samplerate != mp3_internal->source_rate) {
    mp3_internal->source_rate = pcm.samplerate;
    resample_finish(&mp3_internal->rs);
    int result = resample_init(&mp3_internal->rs, pcm.samplerate, 44100);

    // if we couldn't re-initialize the resampling buffer, fail
    if (result == -1) {
      mp3_internal->resampled_buffer_size = 0;
      mp3_internal->current_output = 0;

      // create a buffer that is sure to succeed so we have something to delete
      resample_init(&mp3_internal->rs, 44100, 44100);
      return;
    }
  }

  // do the left channel
  mp3_internal->resampled_buffer_size = resample_block(
    &mp3_internal->rs,
    pcm.length,
    pcm.samples[0],
    mp3_internal->resampled_buffer_left
  );

  if (pcm.channels == 1) {    // mono
    memcpy(
      mp3_internal->resampled_buffer_right,
      mp3_internal->resampled_buffer_left,
      mp3_internal->resampled_buffer_size * 4
    );
  } else {                    // stereo
    resample_block(
      &mp3_internal->rs,
      pcm.length,
      pcm.samples[1],
      mp3_internal->resampled_buffer_right
    );
  }

  mp3_internal->current_output = 0;

  // on return:
  // mp3_internal->resampled_buffer_size = number of samples
  // mp3_internal->resampled_buffer_[left|right] = left and right buffers
  // mp3_internal->current_output = 0
}

////////////////////////////////////////////////////////////////////////////////

/* utility to scale and round samples to 16 bits */

static inline acq_s16 scale(mad_fixed_t sample)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - 16));

  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

  /* quantize */
  return (acq_s16)(sample >> (MAD_F_FRACBITS + 1 - 16));
}

////////////////////////////////////////////////////////////////////////////////

static void scale_block(
  acq_s16* output,
  mad_fixed_t* input_left,
  mad_fixed_t* input_right,
  int sample_count)
{
  while (sample_count--) {
    *output++ = scale(*input_left++);
    *output++ = scale(*input_right++);
  }
}

////////////////////////////////////////////////////////////////////////////////

void RenderSamples(
  MP3_INTERNAL* mp3_internal,
  acq_s16* output,
  const int num_samples)
{
  int& co = mp3_internal->current_output;

  scale_block(
    output,
    co + mp3_internal->resampled_buffer_left,
    co + mp3_internal->resampled_buffer_right,
    num_samples
  );

  co += num_samples;
  mp3_internal->resampled_buffer_size -= num_samples;
}

////////////////////////////////////////////////////////////////////////////////
