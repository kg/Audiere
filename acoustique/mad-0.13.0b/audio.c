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

# include <string.h>

# include "audio.h"
# include "mad.h"

char const *audio_error;

static mad_fixed_t left_err, right_err;

/*
 * NAME:	audio_output()
 * DESCRIPTION: choose an audio output module from a specifier pathname
 */
audio_ctlfunc_t *audio_output(char const **path)
{
  char const *ext;
  int i;

  struct map {
    char const *name;
    audio_ctlfunc_t *module;
  };

  struct map const types[] = {
    { "raw",  audio_raw  },
    { "pcm",  audio_raw  },
    { "wave", audio_wave },
    { "wav",  audio_wave },
    { "snd",  audio_snd  },
    { "au",   audio_snd  },
# if defined(DEBUG)
    { "hex",  audio_hex  },
# endif
    { "null", audio_null }
  };

  struct map const exts[] = {
    { "raw",  audio_raw  },
    { "pcm",  audio_raw  },
    { "out",  audio_raw  },
    { "bin",  audio_raw  },
    { "wav",  audio_wave },
    { "snd",  audio_snd  },
    { "au",   audio_snd  },
# if defined(DEBUG)
    { "hex",  audio_hex  },
    { "txt",  audio_hex  }
# endif
  };

  if (path == 0)
    return AUDIO_DEFAULT;

  /* check for prefix specifier */

  ext = strchr(*path, ':');
  if (ext) {
    char const *type;

    type  = *path;
    *path = ext + 1;

    for (i = 0; i < sizeof(types) / sizeof(types[0]); ++i) {
      if (strncasecmp(type, types[i].name, ext - type) == 0 &&
	  strlen(types[i].name) == ext - type)
	return types[i].module;
    }

    *path = type;
    return 0;
  }

  if (strcmp(*path, "/dev/null") == 0)
    return audio_null;

  if (strncmp(*path, "/dev/", 5) == 0)
    return AUDIO_DEFAULT;

  /* check for file extension specifier */

  ext = strrchr(*path, '.');
  if (ext) {
    ++ext;

    for (i = 0; i < sizeof(exts) / sizeof(exts[0]); ++i) {
      if (strcasecmp(ext, exts[i].name) == 0)
	return exts[i].module;
    }
  }

  return 0;
}

/*
 * NAME:	audio_linear_round()
 * DESCRIPTION:	generic linear sample quantize routine
 */
inline
signed long audio_linear_round(unsigned int bits, mad_fixed_t sample,
			       struct audio_stats *stats)
{
  /* round */
  sample += (1L << (MAD_F_FRACBITS - bits));

# if 1
  /* clip */
  if (sample >= stats->peak_sample) {
    if (sample >= MAD_F_ONE) {
      ++stats->clipped_samples;
      if (sample - (MAD_F_ONE - 1) > stats->peak_clipping)
	stats->peak_clipping = sample - (MAD_F_ONE - 1);
      sample = MAD_F_ONE - 1;
    }
    stats->peak_sample = sample;
  }
  else if (sample < -stats->peak_sample) {
    if (sample < -MAD_F_ONE) {
      ++stats->clipped_samples;
      if (-MAD_F_ONE - sample > stats->peak_clipping)
	stats->peak_clipping = -MAD_F_ONE - sample;
      sample = -MAD_F_ONE;
    }
    stats->peak_sample = -sample;
  }
# else
  /* clip */
  if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;
# endif

  /* quantize and scale */
  return sample >> (MAD_F_FRACBITS + 1 - bits);
}

/*
 * NAME:	audio_linear_dither()
 * DESCRIPTION:	generic linear sample quantize and dither routine
 */
inline
signed long audio_linear_dither(unsigned int bits, mad_fixed_t sample,
				mad_fixed_t *error, struct audio_stats *stats)
{
  mad_fixed_t quantized;

  /* dither */
  sample += *error;

# if 1
  /* clip */
  quantized = sample;
  if (sample >= stats->peak_sample) {
    if (sample >= MAD_F_ONE) {
      quantized = MAD_F_ONE - 1;
      ++stats->clipped_samples;
      if (sample - quantized > stats->peak_clipping &&
	  mad_f_abs(*error) < (MAD_F_ONE >> (MAD_F_FRACBITS + 1 - bits)))
	stats->peak_clipping = sample - quantized;
    }
    stats->peak_sample = quantized;
  }
  else if (sample < -stats->peak_sample) {
    if (sample < -MAD_F_ONE) {
      quantized = -MAD_F_ONE;
      ++stats->clipped_samples;
      if (quantized - sample > stats->peak_clipping &&
	  mad_f_abs(*error) < (MAD_F_ONE >> (MAD_F_FRACBITS + 1 - bits)))
	stats->peak_clipping = quantized - sample;
    }
    stats->peak_sample = -quantized;
  }
# else
  /* clip */
  quantized = sample;
  if (sample >= MAD_F_ONE)
    quantized = MAD_F_ONE - 1;
  else if (sample < -MAD_F_ONE)
    quantized = -MAD_F_ONE;
# endif

  /* quantize */
  quantized &= ~((1L << (MAD_F_FRACBITS + 1 - bits)) - 1);

  /* error */
  *error = sample - quantized;

  /* scale */
  return quantized >> (MAD_F_FRACBITS + 1 - bits);
}

/*
 * NAME:	audio_pcm_u8()
 * DESCRIPTION:	write a block of unsigned 8-bit PCM samples
 */
unsigned int audio_pcm_u8(unsigned char *data, unsigned int nsamples,
			  mad_fixed_t const *left, mad_fixed_t const *right,
			  enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	data[0] = audio_linear_round(8, *left++,  stats) + 0x80;
	data[1] = audio_linear_round(8, *right++, stats) + 0x80;

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	data[0] = audio_linear_dither(8, *left++,  &left_err,  stats) + 0x80;
	data[1] = audio_linear_dither(8, *right++, &right_err, stats) + 0x80;

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--)
	*data++ = audio_linear_round(8, *left++, stats) + 0x80;
      break;

    case AUDIO_MODE_DITHER:
      while (len--)
	*data++ = audio_linear_dither(8, *left++, &left_err, stats) + 0x80;
      break;

    default:
      return 0;
    }

    return nsamples;
  }
}

/*
 * NAME:	audio_pcm_s16le()
 * DESCRIPTION:	write a block of signed 16-bit little-endian PCM samples
 */
unsigned int audio_pcm_s16le(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed int sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++,  stats);
	sample1 = audio_linear_round(16, *right++, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;
	data[2] = sample1 >> 0;
	data[3] = sample1 >> 8;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++,  &left_err,  stats);
	sample1 = audio_linear_dither(16, *right++, &right_err, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;
	data[2] = sample1 >> 0;
	data[3] = sample1 >> 8;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++, &left_err, stats);

	data[0] = sample0 >> 0;
	data[1] = sample0 >> 8;

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
}

/*
 * NAME:	audio_pcm_s16be()
 * DESCRIPTION:	write a block of signed 16-bit big-endian PCM samples
 */
unsigned int audio_pcm_s16be(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed int sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++,  stats);
	sample1 = audio_linear_round(16, *right++, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;
	data[2] = sample1 >> 8;
	data[3] = sample1 >> 0;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++,  &left_err,  stats);
	sample1 = audio_linear_dither(16, *right++, &right_err, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;
	data[2] = sample1 >> 8;
	data[3] = sample1 >> 0;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(16, *left++, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(16, *left++, &left_err, stats);

	data[0] = sample0 >> 8;
	data[1] = sample0 >> 0;

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
}

/*
 * NAME:	audio_pcm_s24le()
 * DESCRIPTION:	write a block of signed 24-bit little-endian PCM samples
 */
unsigned int audio_pcm_s24le(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data[3] = sample1 >>  0;
	data[4] = sample1 >>  8;
	data[5] = sample1 >> 16;

	data += 6;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_err,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_err, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data[3] = sample1 >>  0;
	data[4] = sample1 >>  8;
	data[5] = sample1 >> 16;

	data += 6;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data += 3;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++, &left_err, stats);

	data[0] = sample0 >>  0;
	data[1] = sample0 >>  8;
	data[2] = sample0 >> 16;

	data += 3;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3;
  }
}

/*
 * NAME:	audio_pcm_s24be()
 * DESCRIPTION:	write a block of signed 24-bit big-endian PCM samples
 */
unsigned int audio_pcm_s24be(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;

	data[3] = sample1 >> 16;
	data[4] = sample1 >>  8;
	data[5] = sample1 >>  0;

	data += 6;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_err,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_err, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;

	data[3] = sample1 >> 16;
	data[4] = sample1 >>  8;
	data[5] = sample1 >>  0;

	data += 6;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;

	data += 3;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample1 = audio_linear_dither(24, *left++, &left_err, stats);

	data[0] = sample1 >> 16;
	data[1] = sample1 >>  8;
	data[2] = sample1 >>  0;

	data += 3;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 3;
  }
}

/*
 * NAME:	audio_pcm_s32le()
 * DESCRIPTION:	write a block of signed 32-bit little-endian PCM samples
 */
unsigned int audio_pcm_s32le(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data[4] = 0;
	data[5] = sample1 >>  0;
	data[6] = sample1 >>  8;
	data[7] = sample1 >> 16;

	data += 8;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_err,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_err, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data[4] = 0;
	data[5] = sample1 >>  0;
	data[6] = sample1 >>  8;
	data[7] = sample1 >> 16;

	data += 8;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++, &left_err, stats);

	data[0] = 0;
	data[1] = sample0 >>  0;
	data[2] = sample0 >>  8;
	data[3] = sample0 >> 16;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4;
  }
}

/*
 * NAME:	audio_pcm_s32be()
 * DESCRIPTION:	write a block of signed 32-bit big-endian PCM samples
 */
unsigned int audio_pcm_s32be(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;
  register signed long sample0, sample1;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++,  stats);
	sample1 = audio_linear_round(24, *right++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data[4] = sample1 >> 16;
	data[5] = sample1 >>  8;
	data[6] = sample1 >>  0;
	data[7] = 0;

	data += 8;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++,  &left_err,  stats);
	sample1 = audio_linear_dither(24, *right++, &right_err, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data[4] = sample1 >> 16;
	data[5] = sample1 >>  8;
	data[6] = sample1 >>  0;
	data[7] = 0;

	data += 8;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4 * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	sample0 = audio_linear_round(24, *left++, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data += 4;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	sample0 = audio_linear_dither(24, *left++, &left_err, stats);

	data[0] = sample0 >> 16;
	data[1] = sample0 >>  8;
	data[2] = sample0 >>  0;
	data[3] = 0;

	data += 4;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 4;
  }
}

/*
 * NAME:	audio_mulaw_round()
 * DESCRIPTION:	convert a linear PCM value to 8-bit ISDN mu-law
 */
unsigned char audio_mulaw_round(mad_fixed_t sample)
{
  unsigned int sign, mulaw;

  enum {
    bias = (mad_fixed_t) ((0x10 << 1) + 1) << (MAD_F_FRACBITS - 13)
  };

  if (sample < 0) {
    sample = bias - sample;
    sign   = 0x7f;
  }
  else {
    sample = bias + sample;
    sign   = 0xff;
  }

  if (sample >= MAD_F_ONE)
    mulaw = 0x7f;
  else {
    unsigned int segment;
    unsigned long mask;

    segment = 7;
    for (mask = 1L << (MAD_F_FRACBITS - 1); !(sample & mask); mask >>= 1)
      --segment;

    mulaw = ((segment << 4) |
	     ((sample >> (MAD_F_FRACBITS - 1 - (7 - segment) - 4)) & 0x0f));
  }

  mulaw ^= sign;

# if 0
  if (mulaw == 0x00)
    mulaw = 0x02;
# endif

  return mulaw;
}

static
mad_fixed_t mulaw2linear(unsigned char mulaw)
{
  int sign, segment, mantissa, value;

  enum {
    bias = (0x10 << 1) + 1
  };

  mulaw = ~mulaw;

  sign = (mulaw >> 7) & 0x01;
  segment = (mulaw >> 4) & 0x07;
  mantissa = (mulaw >> 0) & 0x0f;

  value = ((0x21 | (mantissa << 1)) << segment) - bias;
  if (sign)
    value = -value;

  return (mad_fixed_t) value << (MAD_F_FRACBITS - 13);
}

/*
 * NAME:	audio_mulaw_dither()
 * DESCRIPTION:	convert a linear PCM value to dithered 8-bit ISDN mu-law
 */
unsigned char audio_mulaw_dither(mad_fixed_t sample, mad_fixed_t *error)
{
  int sign, mulaw;
  mad_fixed_t biased;

  enum {
    bias = (mad_fixed_t) ((0x10 << 1) + 1) << (MAD_F_FRACBITS - 13)
  };

  /* dither */
  sample += *error;

  if (sample < 0) {
    biased = bias - sample;
    sign   = 0x7f;
  }
  else {
    biased = bias + sample;
    sign   = 0xff;
  }

  if (biased >= MAD_F_ONE)
    mulaw = 0x7f;
  else {
    unsigned int segment;
    unsigned long mask;

    segment = 7;
    for (mask = 1L << (MAD_F_FRACBITS - 1); !(biased & mask); mask >>= 1)
      --segment;

    mulaw = ((segment << 4) |
	     ((biased >> (MAD_F_FRACBITS - 1 - (7 - segment) - 4)) & 0x0f));
  }

  mulaw ^= sign;

# if 0
  if (mulaw == 0x00)
    mulaw = 0x02;
# endif

  /* error */
  *error = sample - mulaw2linear(mulaw);

  return mulaw;
}

/*
 * NAME:	audio_pcm_mulaw()
 * DESCRIPTION:	write a block of 8-bit mu-law encoded samples
 */
unsigned int audio_pcm_mulaw(unsigned char *data, unsigned int nsamples,
			     mad_fixed_t const *left, mad_fixed_t const *right,
			     enum audio_mode mode, struct audio_stats *stats)
{
  unsigned int len;

  len = nsamples;

  if (right) {  /* stereo */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--) {
	data[0] = audio_mulaw_round(*left++);
	data[1] = audio_mulaw_round(*right++);

	data += 2;
      }
      break;

    case AUDIO_MODE_DITHER:
      while (len--) {
	data[0] = audio_mulaw_dither(*left++,  &left_err);
	data[1] = audio_mulaw_dither(*right++, &right_err);

	data += 2;
      }
      break;

    default:
      return 0;
    }

    return nsamples * 2;
  }
  else {  /* mono */
    switch (mode) {
    case AUDIO_MODE_ROUND:
      while (len--)
	*data++ = audio_mulaw_round(*left++);
      break;

    case AUDIO_MODE_DITHER:
      while (len--)
	*data++ = audio_mulaw_dither(*left++, &left_err);
      break;

    default:
      return 0;
    }

    return nsamples;
  }
}
