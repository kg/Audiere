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
# include <stdarg.h>
# include <stdlib.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <unistd.h>
# include <string.h>
# include <errno.h>
# include <time.h>
# include <locale.h>
# include <math.h>
# include <assert.h>

# if defined(HAVE_MMAP)
#  include <sys/mman.h>
# endif

# if !defined(O_BINARY)
#  define O_BINARY  0
# endif

# include "player.h"
# include "mad.h"
# include "audio.h"
# include "resample.h"
# include "id3.h"
# include "filter.h"
# include "equalizer.h"

# define MPEG_BUFSZ	40000	/* 2.5 s at 128 kbps; 1 s at 320 kbps */
# define FREQ_TOLERANCE	6	/* percent sampling frequency tolerance */

/*
 * NAME:	player_init()
 * DESCRIPTION:	initialize player structure
 */
void player_init(struct player *player)
{
  player->verbosity = 0;

  player->flags  = 0;
  player->repeat = 1;

  player->playlist.entries = 0;
  player->playlist.length  = 0;
  player->playlist.current = 0;

  player->global_start = mad_timer_zero;
  player->global_stop  = mad_timer_zero;

  player->fade_in      = mad_timer_zero;
  player->fade_out     = mad_timer_zero;
  player->gap          = mad_timer_zero;

  player->input.path   = 0;
  player->input.fd     = -1;
# if defined(HAVE_MMAP)
  player->input.fdm    = 0;
# endif
  player->input.data   = 0;
  player->input.length = 0;
  player->input.eof    = 0;

  player->output.mode         = AUDIO_MODE_DITHER;
  player->output.attenuate    = MAD_F_ONE;
  player->output.filters      = 0;
  player->output.channels_in  = 0;
  player->output.channels_out = 0;
  player->output.select       = PLAYER_CHANNEL_DEFAULT;
  player->output.speed_in     = 0;
  player->output.speed_out    = 0;
  player->output.path         = 0;
  player->output.command      = 0;
  /* player->output.resample */
  player->output.resampled    = 0;

  player->stats.global_timer          = mad_timer_zero;
  player->stats.absolute_timer        = mad_timer_zero;
  player->stats.play_timer            = mad_timer_zero;
  player->stats.global_framecount     = 0;
  player->stats.absolute_framecount   = 0;
  player->stats.play_framecount       = 0;
  player->stats.mute_frame            = 0;
  player->stats.vbr                   = 0;
  player->stats.bitrate               = 0;
  player->stats.vbr_frames            = 0;
  player->stats.vbr_rate              = 0;
  player->stats.nsecs                 = 0;
  player->stats.audio.clipped_samples = 0;
  player->stats.audio.peak_clipping   = 0;
  player->stats.audio.peak_sample     = 0;
}

/*
 * NAME:	player_finish()
 * DESCRIPTION:	destroy a player structure
 */
void player_finish(struct player *player)
{
  if (player->output.filters)
    filter_free(player->output.filters);

  if (player->output.resampled) {
    resample_finish(&player->output.resample[0]);
    resample_finish(&player->output.resample[1]);

    free(player->output.resampled);
    player->output.resampled = 0;
  }
}

static
int on_same_line;

/*
 * NAME:	message()
 * DESCRIPTION:	show a message, possibly overwriting a previous w/o newline
 */
static
int message(char const *format, ...)
{
  int len, newline, result;
  va_list args;

  len = strlen(format);
  newline = (format[len - 1] == '\n');

  if (on_same_line && newline && len > 1)
    fputc('\n', stderr);

  va_start(args, format);
  result = vfprintf(stderr, format, args);
  va_end(args);

  if (on_same_line && !newline && result < on_same_line) {
    unsigned int i;

    i = on_same_line - result;
    while (i--)
      putc(' ', stderr);
  }

  on_same_line = newline ? 0 : result;

  if (on_same_line) {
    fputc('\r', stderr);
    fflush(stderr);
  }

  return result;
}

/*
 * NAME:	error()
 * DESCRIPTION:	show an error using proper interaction with message()
 */
static
void error(char const *id, char const *format, ...)
{
  int err;
  va_list args;

  err = errno;

  if (on_same_line)
    message("\n");

  if (id)
    fprintf(stderr, "%s: ", id);

  va_start(args, format);

  if (*format == ':') {
    if (format[1] == 0) {
      format = va_arg(args, char const *);
      errno = err;
      perror(format);
    }
    else {
      errno = err;
      perror(format + 1);
    }
  }
  else {
    vfprintf(stderr, format, args);
    fputc('\n', stderr);
  }

  va_end(args);
}

# if defined(HAVE_MMAP)
/*
 * NAME:	map_file()
 * DESCRIPTION:	map the contents of a file into memory, with a buffer guard
 */
static
void *map_file(int fd, unsigned long *length)
{
  void *fdm;

  *length += MAD_BUFFER_GUARD;

  fdm = mmap(0, *length, PROT_READ, MAP_SHARED, fd, 0);
  if (fdm == MAP_FAILED)
    return 0;

# if defined(HAVE_MADVISE)
  madvise(fdm, *length, MADV_SEQUENTIAL);
# endif

  return fdm;
}

/*
 * NAME:	unmap_file()
 * DESCRIPTION:	undo a file mapping
 */
static
int unmap_file(void *fdm, unsigned long length)
{
  if (munmap(fdm, length) == -1)
    return -1;

  return 0;
}
# endif

/*
 * NAME:	decode->input()
 * DESCRIPTION:	(re)fill decoder input buffer
 */
static
enum mad_flow decode_input(void *data, struct mad_stream *stream)
{
  struct player *player = data;
  struct input *input = &player->input;
  int len;

  if (player->input.eof)
    return MAD_FLOW_STOP;

# if defined(HAVE_MMAP)
  if (input->fdm) {
    unsigned long skip = 0;

    if (stream->next_frame) {
      struct stat stat;

      if (fstat(input->fd, &stat) == -1)
	return MAD_FLOW_BREAK;

      if (stat.st_size + MAD_BUFFER_GUARD <= input->length)
	return MAD_FLOW_STOP;

      /* file size changed; update memory map */

      skip = stream->next_frame - input->data;

      if (unmap_file(input->fdm, input->length) == -1) {
	input->fdm  = 0;
	input->data = 0;
	return MAD_FLOW_BREAK;
      }

      input->length = stat.st_size;

      input->fdm = map_file(input->fd, &input->length);
      if (input->fdm == 0) {
	input->data = 0;
	return MAD_FLOW_BREAK;
      }

      input->data = input->fdm;
    }

    mad_stream_buffer(stream, input->data + skip, input->length - skip);

    return MAD_FLOW_CONTINUE;
  }
# endif

  if (stream->next_frame) {
    memmove(input->data, stream->next_frame,
	    input->length = &input->data[input->length] - stream->next_frame);
  }

  do {
    len = read(input->fd, input->data + input->length,
	       MPEG_BUFSZ - input->length);
  }
  while (len == -1 && errno == EINTR);

  if (len == -1) {
    error("input", ":read");
    return MAD_FLOW_BREAK;
  }
  else if (len == 0) {
    player->input.eof = 1;

    assert(MPEG_BUFSZ - input->length >= MAD_BUFFER_GUARD);

    while (len < MAD_BUFFER_GUARD)
      input->data[input->length + len++] = 0;
  }

  mad_stream_buffer(stream, input->data, input->length += len);

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode->header()
 * DESCRIPTION:	decide whether to continue decoding based on header
 */
static
enum mad_flow decode_header(void *data, struct mad_header const *header)
{
  struct player *player = data;

  if ((player->flags & PLAYER_FLAG_TIMED) &&
      mad_timer_compare(player->stats.global_timer, player->global_stop) > 0)
    return MAD_FLOW_STOP;

  ++player->stats.absolute_framecount;
  mad_timer_add(&player->stats.absolute_timer, header->duration);

  ++player->stats.global_framecount;
  mad_timer_add(&player->stats.global_timer, header->duration);

  if ((player->flags & PLAYER_FLAG_SKIP) &&
      mad_timer_compare(player->stats.global_timer, player->global_start) < 0)
    return MAD_FLOW_IGNORE;

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode->filter()
 * DESCRIPTION:	perform filtering on decoded frame
 */
static
enum mad_flow decode_filter(void *data, struct mad_frame *frame)
{
  struct player *player = data;

  return filter_run(player->output.filters, frame);
}

static
char const *const layer_str[3] = { N_("I"), N_("II"), N_("III") };

static
char const *const mode_str[4] = {
  N_("single channel"), N_("dual channel"), N_("joint stereo"), N_("stereo")
};

/*
 * NAME:	show_status()
 * DESCRIPTION:	generate and output stream statistics
 */
static
void show_status(struct stats *stats,
		 struct mad_header const *header, char const *label)
{
  unsigned int bitrate;
  signed long seconds;

  if (header) {
    bitrate = header->bitrate / 1000;

    stats->vbr_rate += bitrate;
    stats->vbr_frames++;

    stats->vbr += (stats->bitrate && stats->bitrate != bitrate) ? 128 : -1;
    if (stats->vbr < 0)
      stats->vbr = 0;
    else if (stats->vbr > 512)
      stats->vbr = 512;

    stats->bitrate = bitrate;
  }

  seconds = mad_timer_count(stats->global_timer, MAD_UNITS_SECONDS);
  if (seconds != stats->nsecs || !on_same_line) {
    char time_str[18];
    char const *joint_str = "";

    stats->nsecs = seconds;

    mad_timer_string(stats->global_timer, time_str,
		     " %02lu:%02u:%02u", MAD_UNITS_HOURS, 0, 0);
    if (mad_timer_sign(stats->global_timer) < 0)
      time_str[0] = '-';

    if (label)
      message("%s %s", time_str, label);
    else if (header) {
      if (header->mode == MAD_MODE_JOINT_STEREO) {
	switch (header->flags & (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO)) {
	case 0:
	  joint_str = _(" (LR)");
	  break;

	case MAD_FLAG_MS_STEREO:
	  joint_str = _(" (MS)");
	  break;

	case MAD_FLAG_I_STEREO:
	  joint_str = _(" (I)");
	  break;

	case (MAD_FLAG_MS_STEREO | MAD_FLAG_I_STEREO):
	  joint_str = _(" (MS+I)");
	  break;
	}
      }

      message(_("%s Layer %s, %s%u kbps%s, %u Hz, %s%s, %s"),
	      time_str, gettext(layer_str[header->layer - 1]),
	      stats->vbr ? _("VBR (avg ") : "",
	      stats->vbr ? ((stats->vbr_rate * 2) /
			    stats->vbr_frames + 1) / 2 : stats->bitrate,
	      stats->vbr ? _(")") : "",
	      header->samplerate, gettext(mode_str[header->mode]), joint_str,
	      (header->flags & MAD_FLAG_PROTECTION) ? _("CRC") : _("no CRC"));
    }
    else
      message("%s", time_str);
  }
}

/*
 * NAME:	decode->output()
 * DESCRIPTION: configure audio module and output decoded samples
 */
static
enum mad_flow decode_output(void *data, struct mad_header const *header,
			    struct mad_pcm *pcm)
{
  struct player *player = data;
  struct output *output = &player->output;
  mad_fixed_t const *ch1, *ch2;
  unsigned int nchannels;
  union audio_control control;

  ch1 = pcm->samples[0];
  ch2 = pcm->samples[1];

  switch (nchannels = pcm->channels) {
  case 1:
    ch2 = 0;
    if (output->select == PLAYER_CHANNEL_STEREO) {
      ch2 = ch1;
      nchannels = 2;
    }
    break;

  case 2:
    switch (output->select) {
    case PLAYER_CHANNEL_RIGHT:
      ch1 = ch2;
      /* fall through */

    case PLAYER_CHANNEL_LEFT:
      ch2 = 0;
      nchannels = 1;
      /* fall through */

    case PLAYER_CHANNEL_STEREO:
      break;

    default:
      if (header->mode == MAD_MODE_DUAL_CHANNEL) {
	if (output->select == PLAYER_CHANNEL_DEFAULT) {
	  if (player->verbosity >= -1) {
	    error("output",
		  _("no channel selected for dual channel; using first"));
	  }

	  output->select = -PLAYER_CHANNEL_LEFT;
	}

	ch2 = 0;
	nchannels = 1;
      }
    }
  }

  if (output->channels_in != nchannels ||
      output->speed_in != pcm->samplerate) {
    if (player->verbosity >= 1 &&
	pcm->samplerate != header->samplerate) {
      error("output", _("using output sample frequency %u Hz"),
	    pcm->samplerate);
    }

    control.command = AUDIO_COMMAND_CONFIG;

    control.config.channels = nchannels;
    control.config.speed    = pcm->samplerate;

    if (output->command(&control) == -1) {
      error("output", audio_error);
      return MAD_FLOW_BREAK;
    }

    output->channels_in  = nchannels;
    output->channels_out = control.config.channels;
    output->speed_in     = pcm->samplerate;
    output->speed_out    = control.config.speed;

    if (player->verbosity >= -1 &&
	output->channels_in != output->channels_out) {
      if (output->channels_in == 1)
	error("output", _("mono output not available; forcing stereo"));
      else {
	error("output", _("stereo output not available; using first channel "
			  "(use -m to mix)"));
      }
    }

    if (player->verbosity >= 0 &&
	output->speed_in != output->speed_out) {
      error("output", _("sample frequency %u Hz not available; closest %u Hz"),
	    pcm->samplerate, control.config.speed);
    }

    /* check whether resampling is necessary */
    if (abs(output->speed_out - output->speed_in) <
	(long) FREQ_TOLERANCE * output->speed_in / 100) {
      if (output->resampled) {
	resample_finish(&output->resample[0]);
	resample_finish(&output->resample[1]);

	free(output->resampled);
	output->resampled = 0;
      }
    }
    else {
      if (output->resampled) {
	resample_finish(&output->resample[0]);
	resample_finish(&output->resample[1]);
      }
      else {
	output->resampled = malloc(sizeof(*output->resampled));
	if (output->resampled == 0) {
	  error("output",
		_("not enough memory to allocate resampling buffer"));
	  return MAD_FLOW_BREAK;
	}
      }

      if (resample_init(&output->resample[0],
			output->speed_in, output->speed_out) == -1 ||
	  resample_init(&output->resample[1],
			output->speed_in, output->speed_out) == -1) {
	error("output", _("cannot resample %u Hz to %u Hz"),
	      output->speed_in, output->speed_out);

	free(output->resampled);
	output->resampled = 0;

	return MAD_FLOW_BREAK;
      }
      else if (player->verbosity >= -1) {
	error("output", _("resampling %u Hz to %u Hz"),
	      output->speed_in, output->speed_out);
      }
    }
  }

  control.command = AUDIO_COMMAND_PLAY;

  if (output->channels_in != output->channels_out)
    ch2 = (output->channels_out == 2) ? ch1 : 0;

  if (output->resampled) {
    control.play.nsamples = resample_block(&output->resample[0],
					   pcm->length, ch1,
					   (*output->resampled)[0]);
    control.play.samples[0] = (*output->resampled)[0];

    if (ch2 == ch1)
      control.play.samples[1] = control.play.samples[0];
    else if (ch2) {
      resample_block(&output->resample[1], pcm->length, ch2,
		     (*output->resampled)[1]);
      control.play.samples[1] = (*output->resampled)[1];
    }
    else
      control.play.samples[1] = 0;
  }
  else {
    control.play.nsamples   = pcm->length;
    control.play.samples[0] = ch1;
    control.play.samples[1] = ch2;
  }

  control.play.mode  = output->mode;
  control.play.stats = &player->stats.audio;

  if (output->command(&control) == -1) {
    error("output", audio_error);
    return MAD_FLOW_BREAK;
  }

  ++player->stats.play_framecount;
  mad_timer_add(&player->stats.play_timer, header->duration);

  if (player->verbosity > 0)
    show_status(&player->stats, header, 0);

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	error_str()
 * DESCRIPTION:	return a string describing a MAD error
 */
static
char const *error_str(enum mad_error error)
{
  static char str[17];

  switch (error) {
  case MAD_ERROR_BUFLEN:
  case MAD_ERROR_BUFPTR:
    /* these errors are handled specially and/or should not occur */
    break;

  case MAD_ERROR_NOMEM:		 return _("not enough memory");
  case MAD_ERROR_LOSTSYNC:	 return _("lost synchronization");
  case MAD_ERROR_BADLAYER:	 return _("reserved header layer value");
  case MAD_ERROR_BADBITRATE:	 return _("forbidden bitrate value");
  case MAD_ERROR_BADSAMPLERATE:	 return _("reserved sample frequency value");
  case MAD_ERROR_BADEMPHASIS:	 return _("reserved emphasis value");
  case MAD_ERROR_BADCRC:	 return _("CRC check failed");
  case MAD_ERROR_BADBITALLOC:	 return _("forbidden bit allocation value");
  case MAD_ERROR_BADSCALEFACTOR: return _("bad scalefactor index");
  case MAD_ERROR_BADFRAMELEN:	 return _("bad frame length");
  case MAD_ERROR_BADBIGVALUES:	 return _("bad big_values count");
  case MAD_ERROR_BADBLOCKTYPE:	 return _("reserved block_type");
  case MAD_ERROR_BADSCFSI:	 return _("bad scalefactor selection info");
  case MAD_ERROR_BADDATAPTR:	 return _("bad main_data_begin pointer");
  case MAD_ERROR_BADPART3LEN:	 return _("bad audio data length");
  case MAD_ERROR_BADHUFFTABLE:	 return _("bad Huffman table select");
  case MAD_ERROR_BADHUFFDATA:	 return _("Huffman data overrun");
  case MAD_ERROR_BADSTEREO:	 return _("incompatible block_type for JS");
  }

  sprintf(str, "error 0x%04x", error);
  return str;
}

/*
 * NAME:	decode->error()
 * DESCRIPTION:	handle a decoding error
 */
static
enum mad_flow decode_error(void *data, struct mad_stream *stream,
			   struct mad_frame *frame)
{
  struct player *player = data;

  switch (stream->error) {
  case MAD_ERROR_BADDATAPTR:
    return MAD_FLOW_CONTINUE;

  case MAD_ERROR_LOSTSYNC:
    if (strncmp(stream->this_frame, "ID3", 3) == 0) {
      unsigned long tagsize;

      /* ID3v2 tag */

# if defined(HAVE_MMAP)
      if (player->input.fdm &&
	  lseek(player->input.fd,
		stream->this_frame - stream->buffer, SEEK_SET) == -1) {
	error("error", ":lseek");
	return MAD_FLOW_BREAK;
      }
# endif

      if (id3_v2_read(message, stream->this_frame,
		      stream->bufend - stream->this_frame, player->input.fd,
		      player->verbosity < 0, &tagsize) == -1) {
	error("ID3", id3_error);
	return MAD_FLOW_BREAK;
      }
      else if (tagsize > stream->bufend - stream->this_frame)
	mad_stream_skip(stream, stream->bufend - stream->this_frame);
      else
	mad_stream_skip(stream, tagsize);

      return MAD_FLOW_CONTINUE;
    }
    else if (strncmp(stream->this_frame, "TAG", 3) == 0) {
      /* ID3v1 tag */
      mad_stream_skip(stream, 128);

      return MAD_FLOW_CONTINUE;
    }

    /* fall through */

  default:
    if (player->verbosity >= -1 &&
	(stream->error == MAD_ERROR_LOSTSYNC || stream->sync)) {
      error("error", _("frame %lu: %s"),
	    player->stats.absolute_framecount, error_str(stream->error));
    }
  }

  if (stream->error == MAD_ERROR_BADCRC) {
    if (player->stats.global_framecount == player->stats.mute_frame)
      mad_frame_mute(frame);

    player->stats.mute_frame = player->stats.global_framecount + 1;

    return MAD_FLOW_IGNORE;
  }

  return MAD_FLOW_CONTINUE;
}

/*
 * NAME:	decode()
 * DESCRIPTION:	decode and output audio for an open file
 */
static
int decode(struct player *player)
{
  struct mad_decoder decoder;
  int result;

  /* check for ID3v1 tag */
  if (player->verbosity >= 0 &&
      lseek(player->input.fd, -128, SEEK_END) != -1) {
    unsigned char id3v1[128], *ptr;
    int len, count;

    for (ptr = id3v1, count = 128; count; count -= len, ptr += len) {
      do {
	len = read(player->input.fd, ptr, count);
      }
      while (len == -1 && errno == EINTR);

      if (len == -1) {
	error("decode", ":read");
	return -1;
      }
      else if (len == 0)
	break;
    }

    if (count == 0 && strncmp(id3v1, "TAG", 3) == 0)
      id3_v1_show(message, id3v1);

    if (lseek(player->input.fd, 0, SEEK_SET) == -1) {
      error("decode", ":lseek");
      return -1;
    }
  }

  /* prepare input buffers */

# if defined(HAVE_MMAP)
  {
    struct stat stat;

    if (fstat(player->input.fd, &stat) == -1) {
      error("decode", ":fstat");
      return -1;
    }

    if (S_ISREG(stat.st_mode) && stat.st_size > 0) {
      player->input.length = stat.st_size;

      player->input.fdm = map_file(player->input.fd, &player->input.length);
      if (player->input.fdm == 0) {
	error("decode", ":mmap");
	return -1;
      }

      player->input.data = player->input.fdm;
    }
  }
# endif

  if (player->input.data == 0) {
    player->input.data = malloc(MPEG_BUFSZ);
    if (player->input.data == 0) {
      error("decode", _("not enough memory to allocate input buffer"));
      return -1;
    }

    player->input.length = 0;
  }

  player->input.eof = 0;

  /* reset statistics */
  player->stats.absolute_framecount   = 0;
  player->stats.play_framecount       = 0;
  player->stats.absolute_timer        = mad_timer_zero;
  player->stats.play_timer            = mad_timer_zero;
  player->stats.vbr                   = 0;
  player->stats.bitrate               = 0;
  player->stats.vbr_frames            = 0;
  player->stats.vbr_rate              = 0;
  player->stats.audio.clipped_samples = 0;
  player->stats.audio.peak_clipping   = 0;
  player->stats.audio.peak_sample     = 0;

  mad_decoder_init(&decoder, player, decode_input, decode_header,
		   player->output.filters ? decode_filter : 0,
		   player->output.command ? decode_output : 0,
		   decode_error, 0);

  if (player->flags & PLAYER_FLAG_DOWNSAMPLE)
    mad_decoder_options(&decoder, MAD_OPTION_HALFSAMPLERATE);

  result = mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

  mad_decoder_finish(&decoder);

# if defined(HAVE_MMAP)
  if (player->input.fdm) {
    if (unmap_file(player->input.fdm, player->input.length) == -1) {
      error("decode", ":munmap");
      result = -1;
    }

    player->input.fdm  = 0;
    player->input.data = 0;
  }
# endif

  if (player->input.data) {
    free(player->input.data);
    player->input.data = 0;
  }

  return result;
}

/*
 * NAME:	play()
 * DESCRIPTION:	open and play a single file
 */
static
int play(struct player *player)
{
  char const *file = player->playlist.entries[player->playlist.current];
  int result;

  if (strcmp(file, "-") == 0) {
    player->input.path = _("stdin");
    player->input.fd   = STDIN_FILENO;
  }
  else {
    player->input.path = file;
    player->input.fd   = open(player->input.path, O_RDONLY | O_BINARY);
    if (player->input.fd == -1) {
      error(0, ":", player->input.path);
      return -1;
    }
  }

  if (player->playlist.length > 1 && player->verbosity >= 0)
    message("... %s\n", player->input.path);

  result = decode(player);

  if (result == 0 && player->verbosity >= 0) {
    char time_str[19], *point;
    mad_fixed_t peak;

    mad_timer_string(player->stats.play_timer, time_str, "%lu:%02u:%02u.%1u",
		     MAD_UNITS_HOURS, MAD_UNITS_DECISECONDS, 0);

    point = strchr(time_str, '.');
    if (point)
      *point = *localeconv()->decimal_point;

    peak = MAD_F_ONE + player->stats.audio.peak_clipping;
    if (peak == MAD_F_ONE)
      peak = player->stats.audio.peak_sample;

    message(_("%lu frames decoded (%s), "
	      "%+.1f dB peak amplitude, "
	      "%lu clipped samples\n"),
	    player->stats.play_framecount, time_str,
	    20 * log10(mad_f_todouble(peak)),
	    player->stats.audio.clipped_samples);
  }

  if (player->input.fd != STDIN_FILENO && close(player->input.fd) == -1) {
    error(0, ":", player->input.path);
    result = -1;
  }

  return result;
}

/*
 * NAME:	addfilter()
 * DESCRIPTION:	insert a filter at the beginning of the filter chain
 */
static
int addfilter(struct player *player, filter_func_t *func, void *data)
{
  struct filter *filter;

  filter = filter_new(func, data, player->output.filters);
  if (filter == 0)
    return -1;

  player->output.filters = filter;

  return 0;
}

/*
 * NAME:	setup_filters()
 * DESCRIPTION:	create output filters
 */
static
int setup_filters(struct player *player)
{
  static struct equalizer attenuation;

  /* filters must be added in reverse-order */

# if defined(EXPERIMENTAL)
  {
    if ((player->flags & PLAYER_FLAG_EXTERNALMIX) &&
	addfilter(player, mixer_filter, stdout) == -1)
      return -1;

    if ((player->flags & PLAYER_FLAG_EXPERIMENTAL) &&
	addfilter(player, experimental_filter, 0) == -1)
      return -1;
  }
# endif

  if ((player->flags & PLAYER_FLAG_FADEIN) &&
      addfilter(player, fadein_filter, player) == -1)
    return -1;

  if (player->output.attenuate != MAD_F_ONE) {
    int sb;

    equalizer_init(&attenuation);

    for (sb = 0; sb < 32; ++sb) {
      attenuation.bandfactors[0][sb] =
      attenuation.bandfactors[1][sb] = player->output.attenuate;
    }

    if (addfilter(player, equalizer_filter, &attenuation) == -1)
      return -1;
  }

  if (player->output.select == PLAYER_CHANNEL_MONO &&
      addfilter(player, mono_filter, player) == -1)
    return -1;

  return 0;
}

/*
 * NAME:	silence()
 * DESCRIPTION:	output silence for a period of time
 */
static
int silence(struct player *player, mad_timer_t duration, char const *label)
{
  union audio_control control;
  unsigned int nchannels, speed, nsamples;
  mad_fixed_t *samples;
  mad_timer_t unit;
  int result = 0;

  control.command         = AUDIO_COMMAND_CONFIG;
  control.config.channels = 2;
  control.config.speed    = 44100;

  if (player->output.command(&control) == -1) {
    error("audio", audio_error);
    return -1;
  }

  nchannels = control.config.channels;
  speed     = control.config.speed;
  nsamples  = speed > MAX_NSAMPLES ? MAX_NSAMPLES : speed;

  player->output.channels_in  = nchannels;
  player->output.channels_out = nchannels;
  player->output.speed_in     = speed;
  player->output.speed_out    = speed;

  samples = calloc(nsamples, sizeof(mad_fixed_t));
  if (samples == 0) {
    error("silence", _("not enough memory to allocate sample buffer"));
    return -1;
  }

  control.command         = AUDIO_COMMAND_PLAY;
  control.play.nsamples   = nsamples;
  control.play.samples[0] = samples;
  control.play.samples[1] = (nchannels == 2) ? samples : 0;
  control.play.mode       = player->output.mode;
  control.play.stats      = &player->stats.audio;

  mad_timer_set(&unit, 0, nsamples, speed);

  for (mad_timer_negate(&duration);
       mad_timer_sign(duration) < 0;
       mad_timer_add(&duration, unit)) {
    if (mad_timer_compare(unit, mad_timer_abs(duration)) > 0) {
      unit = mad_timer_abs(duration);
      control.play.nsamples = mad_timer_fraction(unit, speed);
    }

    if (player->output.command(&control) == -1) {
      error("audio", audio_error);
      goto fail;
    }

    mad_timer_add(&player->stats.global_timer, unit);

    if (player->verbosity > 0)
      show_status(&player->stats, 0, label);
  }

  goto done;

 fail:
  result = -1;

 done:
  free(samples);

  return result;
}

/*
 * NAME:	player->run()
 * DESCRIPTION:	begin playback
 */
int player_run(struct player *player, int argc, char const *argv[])
{
  union audio_control control;
  int count, i, j, result = 0;
  struct playlist *playlist = &player->playlist;
  char const *tmp;

  playlist->entries = argv;
  playlist->length  = argc;

  if (setup_filters(player) == -1) {
    error("filter", _("not enough memory to allocate filters"));
    return -1;
  }

  if (player->output.command) {
    control.command   = AUDIO_COMMAND_INIT;
    control.init.path = player->output.path;

    if (player->output.command(&control) == -1) {
      error("audio", audio_error, control.init.path);
      return -1;
    }

    if ((player->flags & PLAYER_FLAG_SKIP) &&
	mad_timer_sign(player->global_start) < 0) {
      player->stats.global_timer = player->global_start;

      if (silence(player, mad_timer_abs(player->global_start),
		  _("lead-in")) == -1)
	return -1;
    }
  }

  count = playlist->length;

  if (player->flags & PLAYER_FLAG_SHUFFLE) {
    srand(time(0));

    /* initial shuffle */
    for (i = 0; i < count; ++i) {
      j = rand() % count;

      tmp = playlist->entries[i];
      playlist->entries[i] = playlist->entries[j];
      playlist->entries[j] = tmp;
    }
  }

  while (count && (player->repeat == -1 || player->repeat--)) {
    while (playlist->current < playlist->length) {
      i = playlist->current;

      if ((player->flags & PLAYER_FLAG_SHUFFLE) && player->repeat) {
	/* pick something from the next half only */
	j = (i + rand() % ((playlist->length + 1) / 2)) % playlist->length;

	tmp = playlist->entries[i];
	playlist->entries[i] = playlist->entries[j];
	playlist->entries[j] = tmp;
      }

      if (playlist->entries[i] == 0)
	continue;

      if (play(player) == -1) {
	playlist->entries[i] = 0;
	--count;

	result = -1;
      }

      if ((player->flags & PLAYER_FLAG_TIMED) &&
	  mad_timer_compare(player->stats.global_timer,
			    player->global_stop) > 0) {
	count = 0;
	break;
      }

      if (playlist->current == i)
	++playlist->current;
    }

    playlist->current = 0;
  }

  if (player->output.command) {
    control.command = AUDIO_COMMAND_FINISH;

    if (player->output.command(&control) == -1) {
      error("audio", audio_error);
      result = -1;
    }
  }

  return result;
}
