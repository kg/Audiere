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

# define AUDIO_FILE_MAGIC	0x2e736e64L	/* ".snd" */
# define AUDIO_UNKNOWN_SIZE	(~0)		/* (unsigned) -1 */
# define AUDIO_ENCODING_MULAW_8	1		/* 8-bit ISDN mu-law */

static FILE *outfile;

static unsigned long data_len;
static long prev_header;

/*
 * NAME:	int32()
 * DESCRIPTION:	store 32-bit big-endian integer
 */
static
void int32(unsigned char *ptr, unsigned long num)
{
  ptr[0] = num >> 24;
  ptr[1] = num >> 16;
  ptr[2] = num >>  8;
  ptr[3] = num >>  0;
}

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

  data_len    = 0;
  prev_header = 0;

  return 0;
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

# define close_data()	patch_length(prev_header - 16, data_len)

static
int config(struct audio_config *config)
{
  unsigned char header[24];

  if (prev_header)
    close_data();

  /* Sun/NeXT audio file header */

  int32(&header[0],  AUDIO_FILE_MAGIC);		/* magic */
  int32(&header[4],  sizeof(header));		/* hdr_size */
  int32(&header[8],  AUDIO_UNKNOWN_SIZE);	/* data_size */
  int32(&header[12], AUDIO_ENCODING_MULAW_8);	/* encoding */
  int32(&header[16], config->speed);		/* sample_rate */
  int32(&header[20], config->channels);		/* channels */

  if (fwrite(header, sizeof(header), 1, outfile) != 1) {
    audio_error = ":fwrite";
    return -1;
  }

  prev_header = ftell(outfile);
  if (prev_header == -1)
    prev_header = 0;

  data_len = 0;

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES];
  unsigned int len;

  len = audio_pcm_mulaw(data, play->nsamples,
			play->samples[0], play->samples[1], play->mode,
			play->stats);

  if (fwrite(data, play->samples[1] ? 2 : 1,
	     play->nsamples, outfile) != play->nsamples) {
    audio_error = ":fwrite";
    return -1;
  }

  data_len += len;

  return 0;
}

static
int finish(struct audio_finish *finish)
{
  if (prev_header)
    close_data();

  if (outfile != stdout &&
      fclose(outfile) == EOF) {
    audio_error = ":fclose";
    return -1;
  }

  return 0;
}

int audio_snd(union audio_control *control)
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
