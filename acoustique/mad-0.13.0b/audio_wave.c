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

# include <stdio.h>
# include <string.h>

# include "mad.h"
# include "audio.h"

static FILE *outfile;

static unsigned long riff_len, chunk_len;
static long prev_chunk;

# define WAVE_FORMAT_PCM	0x0001

static
int init(struct audio_init *init)
{
  if (init->path && strcmp(init->path, "-") != 0) {
    outfile = fopen(init->path, "wb");
    if (outfile == 0) {
      audio_error = ":";
      return -1;
    }
  }
  else
    outfile = stdout;

  /* RIFF header and (WAVE) data type identifier */

  if (fwrite("RIFF\0\0\0\0WAVE", 8 + 4, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  riff_len   = 4;
  prev_chunk = 0;

  return 0;
}

/*
 * NAME:	int32()
 * DESCRIPTION:	store 32-bit little-endian integer
 */
static
void int32(unsigned char *ptr, unsigned long num)
{
  ptr[0] = num >>  0;
  ptr[1] = num >>  8;
  ptr[2] = num >> 16;
  ptr[3] = num >> 24;
}

/*
 * NAME:	int16
 * DESCRIPTION:	store 16-bit little-endian integer
 */
static
void int16(unsigned char *ptr, unsigned int num)
{
  ptr[0] = num >> 0;
  ptr[1] = num >> 8;
}

static
int patch_length(long address, unsigned long length)
{
  unsigned char dword[4];

  if (fseek(outfile, address, SEEK_SET) == -1)
    return -1;

  int32(dword, length);
  if (fwrite(dword, sizeof(dword), 1, outfile) != 1)
    return -1;

  if (fseek(outfile, 0, SEEK_END) == -1)
    return -1;

  return 0;
}

# define close_chunk()	patch_length(prev_chunk + 4, chunk_len)

static
int config(struct audio_config *config)
{
  unsigned char chunk[24];
  unsigned int block_al;
  unsigned long bytes_ps;

  if (prev_chunk)
    close_chunk();

  /* "fmt " chunk */

  block_al = config->channels * (16 / 8);
  bytes_ps = config->speed * block_al;

  memcpy(&chunk[0], "fmt ", 4);
  int32(&chunk[4], 16);

  int16(&chunk[8],  WAVE_FORMAT_PCM);	/* wFormatTag */
  int16(&chunk[10], config->channels);	/* wChannels */
  int32(&chunk[12], config->speed);	/* dwSamplesPerSec */
  int32(&chunk[16], bytes_ps);		/* dwAvgBytesPerSec */
  int16(&chunk[20], block_al);		/* wBlockAlign */

  /* PCM-format-specific */

  int16(&chunk[22], 16);		/* wBitsPerSample */

  if (fwrite(chunk, sizeof(chunk), 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  /* save current file position for later patching */

  prev_chunk = ftell(outfile);
  if (prev_chunk == -1)
    prev_chunk = 0;

  /* "data" chunk */

  if (fwrite("data\0\0\0\0", 8, 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  chunk_len = 0;
  riff_len += 24 + 8;

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES * 2 * 2];
  unsigned int len;

  len = audio_pcm_s16le(data, play->nsamples,
			play->samples[0], play->samples[1], play->mode,
			play->stats);

  if (fwrite(data, play->samples[1] ? 4 : 2,
	     play->nsamples, outfile) != play->nsamples) {
    audio_error = ":fwrite";
    return -1;
  }

  chunk_len += len;
  riff_len  += len;

  return 0;
}

static
int finish(struct audio_finish *finish)
{
  if (prev_chunk)
    close_chunk();

  patch_length(4, riff_len);

  if (outfile != stdout &&
      fclose(outfile) == EOF) {
    audio_error = ":fclose";
    return -1;
  }

  return 0;
}

int audio_wave(union audio_control *control)
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
