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

# include <unistd.h>
# include <fcntl.h>
# include <sys/ioctl.h>
# include <sys/audioio.h>
# include <string.h>
# include <errno.h>

# include "mad.h"
# include "audio.h"

# if defined(WORDS_BIGENDIAN)
#  define audio_pcm_s16  audio_pcm_s16be
# else
#  define audio_pcm_s16  audio_pcm_s16le
# endif

# define AUDIO_DEVICE	"/dev/audio"

static int sfd;

static
int init(struct audio_init *init)
{
  if (init->path == 0)
    init->path = AUDIO_DEVICE;

  sfd = open(init->path, O_WRONLY);
  if (sfd == -1) {
    audio_error = ":";
    return -1;
  }

  return 0;
}

static
int config(struct audio_config *config)
{
  audio_info_t info;

  if (ioctl(sfd, AUDIO_DRAIN, 0) == -1) {
    audio_error = ":ioctl(AUDIO_DRAIN)";
    return -1;
  }

  AUDIO_INITINFO(&info);

  info.play.sample_rate = config->speed;
  info.play.channels    = config->channels;
  info.play.precision   = 16;
  info.play.encoding    = AUDIO_ENCODING_LINEAR;

  if (ioctl(sfd, AUDIO_SETINFO, &info) == -1) {
    audio_error = ":ioctl(AUDIO_SETINFO)";
    return -1;
  }

  return 0;
}

static
int output(unsigned char const *ptr, unsigned int len)
{
  while (len) {
    int wrote;

    wrote = write(sfd, ptr, len);
    if (wrote == -1) {
      if (errno == EINTR)
	continue;
      else {
	audio_error = ":write";
	return -1;
      }
    }

    ptr += wrote;
    len -= wrote;
  }

  return 0;
}

static
int play(struct audio_play *play)
{
  unsigned char data[MAX_NSAMPLES * 2 * 2];
  unsigned int len;

  len = audio_pcm_s16(data, play->nsamples,
		      play->samples[0], play->samples[1], play->mode,
		      play->stats);

  return output(data, len);
}

static
int finish(struct audio_finish *finish)
{
  int result = 0;

  if (close(sfd) == -1 && result == 0) {
    audio_error = ":close";
    result = -1;
  }

  return result;
}

int audio_sun(union audio_control *control)
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
