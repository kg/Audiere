/*
 * mad - MPEG audio decoder
 * Copyright (C) 2000-2001 Robert Leslie
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * $Id$
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <windows.h>
# include <mmsystem.h>

# include "mad.h"
# include "audio.h"

static int opened;

static HWAVEOUT wave_handle;

# define NSLOTS		25	/* about 2 seconds of audio data (44.1 kHz) */
# define NBUFFERS	 3	/* triple buffer */

static struct buffer {
  WAVEHDR wave_header;
  HANDLE event_handle;
  int playing;
  unsigned int pcm_length;
  unsigned char pcm_data[NSLOTS * MAX_NSAMPLES * 2 * 2];
} output[NBUFFERS];

static int bindex;

static
char const *error_text(MMRESULT result)
{
  static char buffer[MAXERRORLENGTH];

  if (waveOutGetErrorText(result, buffer, sizeof(buffer)) != MMSYSERR_NOERROR)
    return _("error getting error text");

  return buffer;
}

static
int init(struct audio_init *init)
{
  int i;

  opened = 0;

  for (i = 0; i < NBUFFERS; ++i) {
    output[i].event_handle = CreateEvent(0, FALSE /* manual reset */,
					 FALSE /* initial state */, 0);
    if (output[i].event_handle == NULL) {
      while (i--)
	CloseHandle(output[i].event_handle);

      audio_error = _("failed to create synchronization object");
      return -1;
    }

    output[i].playing    = 0;
    output[i].pcm_length = 0;
  }

  bindex = 0;

  /* try to obtain high priority status */

  SetPriorityClass(GetCurrentProcess(), HIGH_PRIORITY_CLASS);
  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);

  return 0;
}

static
void CALLBACK callback(HWAVEOUT handle, UINT message, DWORD data,
		       DWORD param1, DWORD param2)
{
  WAVEHDR *header;
  struct buffer *buffer;

  switch (message) {
  case WOM_DONE:
    header = (WAVEHDR *) param1;
    buffer = (struct buffer *) header->dwUser;

    if (SetEvent(buffer->event_handle) == 0) {
      /* error */
    }
    break;

  case WOM_OPEN:
  case WOM_CLOSE:
    break;
  }
}

static
void set_format(WAVEFORMATEX *format, unsigned int channels,
		unsigned int speed, unsigned int bits)
{
  unsigned int block_al;
  unsigned long bytes_ps;

  block_al = channels * bits / 8;
  bytes_ps = speed * block_al;

  format->wFormatTag      = WAVE_FORMAT_PCM;
  format->nChannels       = channels;
  format->nSamplesPerSec  = speed;
  format->nAvgBytesPerSec = bytes_ps;
  format->nBlockAlign     = block_al;
  format->wBitsPerSample  = bits;
  format->cbSize          = 0;
}

static
int open_dev(HWAVEOUT *handle, unsigned int channels, unsigned int speed)
{
  WAVEFORMATEX format;
  MMRESULT error;

  set_format(&format, channels, speed, 16);

  error = waveOutOpen(handle, WAVE_MAPPER, &format,
		      (DWORD) callback, 0, CALLBACK_FUNCTION);
  if (error != MMSYSERR_NOERROR) {
    audio_error = error_text(error);
    return -1;
  }

  opened = 1;

  return 0;
}

static
int close_dev(HWAVEOUT handle)
{
  MMRESULT error;

  error = waveOutClose(handle);
  if (error != MMSYSERR_NOERROR) {
    audio_error = error_text(error);
    return -1;
  }

  opened = 0;

  return 0;
}

static
int write_dev(HWAVEOUT handle, struct buffer *buffer)
{
  MMRESULT error;

  buffer->wave_header.lpData         = buffer->pcm_data;
  buffer->wave_header.dwBufferLength = buffer->pcm_length;
  buffer->wave_header.dwUser         = (DWORD) buffer;
  buffer->wave_header.dwFlags        = 0;

  error = waveOutPrepareHeader(handle, &buffer->wave_header,
			       sizeof(buffer->wave_header));
  if (error != MMSYSERR_NOERROR) {
    audio_error = error_text(error);
    return -1;
  }

  error = waveOutWrite(handle, &buffer->wave_header,
		       sizeof(buffer->wave_header));
  if (error != MMSYSERR_NOERROR) {
    audio_error = error_text(error);
    return -1;
  }

  buffer->playing = 1;

  return 0;
}

static
int wait(struct buffer *buffer)
{
  MMRESULT error;

  if (!buffer->playing)
    return 0;

  switch (WaitForSingleObject(buffer->event_handle, INFINITE)) {
  case WAIT_OBJECT_0:
    break;

  case WAIT_ABANDONED:
    audio_error = _("wait abandoned");
    return -1;

  case WAIT_TIMEOUT:
    audio_error = _("wait timeout");
    return -1;

  case WAIT_FAILED:
  default:
    audio_error = _("wait failed");
    return -1;
  }

  buffer->playing = 0;

  error = waveOutUnprepareHeader(wave_handle, &buffer->wave_header,
				 sizeof(buffer->wave_header));
  if (error != MMSYSERR_NOERROR) {
    audio_error = error_text(error);
    return -1;
  }

  return 0;
}

static
int flush(void)
{
  int i, result = 0;

  if (output[bindex].pcm_length &&
      write_dev(wave_handle, &output[bindex]) == -1)
    result = -1;

  for (i = 0; i < NBUFFERS; ++i) {
    if (wait(&output[i]) == -1)
      result = -1;
  }

  output[bindex].pcm_length = 0;

  return result;
}

static
int config(struct audio_config *config)
{
  if (opened) {
    flush();

    if (close_dev(wave_handle) == -1)
      return -1;
  }

  if (open_dev(&wave_handle, config->channels, config->speed) == -1)
    return -1;

  return 0;
}

static
int play(struct audio_play *play)
{
  struct buffer *buffer;
  unsigned int len;

  buffer = &output[bindex];

  /* wait for block to finish playing */

  if (buffer->pcm_length == 0 && wait(buffer) == -1)
    return -1;

  /* prepare block */

  len = audio_pcm_s16le(&buffer->pcm_data[buffer->pcm_length], play->nsamples,
			play->samples[0], play->samples[1], play->mode,
			play->stats);

  buffer->pcm_length += len;

  if (buffer->pcm_length + MAX_NSAMPLES * 2 * 2 > sizeof(buffer->pcm_data)) {
    write_dev(wave_handle, buffer);

    bindex = (bindex + 1) % NBUFFERS;
    output[bindex].pcm_length = 0;
  }

  return 0;
}

static
int finish(struct audio_finish *finish)
{
  int i, result = 0;

  if (opened) {
    if (flush() == -1)
      result = -1;
    if (close_dev(wave_handle) == -1)
      result = -1;
  }

  /* restore priority status */

  SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
  SetPriorityClass(GetCurrentProcess(), NORMAL_PRIORITY_CLASS);

  for (i = 0; i < NBUFFERS; ++i) {
    if (CloseHandle(output[i].event_handle) == 0 && result == 0) {
      audio_error = _("failed to close synchronization object");
      result = -1;
    }
  }

  return result;
}

int audio_win32(union audio_control *control)
{
  audio_error = 0;

  switch (control->command) {
  case AUDIO_COMMAND_INIT:
    return init(&control->init);

  case AUDIO_COMMAND_CONFIG:
    return config(&control->config);

  case AUDIO_COMMAND_PLAY:
    return play(&control->play);

  case AUDIO_COMMAND_FINISH:
    return finish(&control->finish);
  }

  return 0;
}
