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

static
int init(struct audio_init *init)
{
  if (init->path && strcmp(init->path, "-") != 0) {
    outfile = fopen(init->path, "w");
    if (outfile == 0) {
      audio_error = ":";
      return -1;
    }
  }
  else
    outfile = stdout;

  return 0;
}

static
int config(struct audio_config *config)
{
  fprintf(outfile, "# %u channel%s, %u Hz\n",
	  config->channels, config->channels == 1 ? "" : "s", config->speed);

  return 0;
}

static
int play(struct audio_play *play)
{
  mad_fixed_t const *left, *right;
  unsigned int len;

  len   = play->nsamples;
  left  = play->samples[0];
  right = play->samples[1];

  switch (play->mode) {
  case AUDIO_MODE_ROUND:
    while (len--) {
      fprintf(outfile, "%06lX\n",
	      audio_linear_round(24, *left++, play->stats) & 0x00ffffffL);

      if (right) {
	fprintf(outfile, "%06lX\n",
		audio_linear_round(24, *right++, play->stats) & 0x00ffffffL);
      }
    }
    break;

  case AUDIO_MODE_DITHER:
    {
      static mad_fixed_t left_err, right_err;

      while (len--) {
	fprintf(outfile, "%06lX\n",
		audio_linear_dither(24, *left++, &left_err, play->stats) &
		0x00ffffffL);

	if (right) {
	  fprintf(outfile, "%06lX\n",
		  audio_linear_dither(24, *right++, &right_err, play->stats) &
		  0x00ffffffL);
	}
      }
    }
    break;
  }

  return 0;
}

static
int finish(struct audio_finish *finish)
{
  if (outfile != stdout &&
      fclose(outfile) == EOF) {
    audio_error = ":close";
    return -1;
  }

  return 0;
}

int audio_hex(union audio_control *control)
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
