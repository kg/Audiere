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

# ifndef PLAYER_H
# define PLAYER_H

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "mad.h"
# include "audio.h"
# include "resample.h"
# include "filter.h"

enum {
  PLAYER_FLAG_SHUFFLE      = 0x0001,
  PLAYER_FLAG_DOWNSAMPLE   = 0x0002,

  PLAYER_FLAG_SKIP         = 0x0010,
  PLAYER_FLAG_TIMED        = 0x0020,

  PLAYER_FLAG_FADEIN       = 0x0100,
  PLAYER_FLAG_FADEOUT      = 0x0200,
  PLAYER_FLAG_GAP          = 0x0400,
  PLAYER_FLAG_CROSSFADE    = 0x0800,

# if defined(EXPERIMENTAL)
  PLAYER_FLAG_EXTERNALMIX  = 0x1000,
  PLAYER_FLAG_EXPERIMENTAL = 0x2000
# endif
};

enum player_channel {
  PLAYER_CHANNEL_DEFAULT = 0,
  PLAYER_CHANNEL_LEFT    = 1,
  PLAYER_CHANNEL_RIGHT   = 2,
  PLAYER_CHANNEL_MONO    = 3,
  PLAYER_CHANNEL_STEREO  = 4
};

struct player {
  int verbosity;

  int flags;
  int repeat;

  struct playlist {
    char const **entries;
    int length;
    int current;
  } playlist;

  mad_timer_t global_start;
  mad_timer_t global_stop;

  mad_timer_t fade_in;
  mad_timer_t fade_out;
  mad_timer_t gap;

  struct input {
    char const *path;

    int fd;
# if defined(HAVE_MMAP)
    void *fdm;
# endif

    unsigned char *data;
    unsigned long length;

    int eof;
  } input;

  struct output {
    enum audio_mode mode;

    mad_fixed_t attenuate;
    struct filter *filters;

    unsigned int channels_in;
    unsigned int channels_out;
    enum player_channel select;

    unsigned int speed_in;
    unsigned int speed_out;

    char const *path;
    audio_ctlfunc_t *command;

    struct resample_state resample[2];
    mad_fixed_t (*resampled)[2][MAX_NSAMPLES];
  } output;

  struct stats {
    mad_timer_t global_timer;
    mad_timer_t absolute_timer;
    mad_timer_t play_timer;

    unsigned long global_framecount;
    unsigned long absolute_framecount;
    unsigned long play_framecount;

    unsigned long mute_frame;

    int vbr;
    unsigned int bitrate;
    unsigned long vbr_frames;
    unsigned long vbr_rate;

    signed long nsecs;

    struct audio_stats audio;
  } stats;
};

void player_init(struct player *);
void player_finish(struct player *);

int player_run(struct player *, int, char const *[]);

# endif
