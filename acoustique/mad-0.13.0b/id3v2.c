/* C code produced by gperf version 2.7 */
/* Command-line: gperf -tCTonD -K id -N id3v2_hash -s -3 -k * id3v2.gperf  */
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
 * Id: id3v2.gperf,v 1.4 2001/01/21 00:18:09 rob Exp 
 */

# ifdef HAVE_CONFIG_H
#  include "config.h"
# endif

# include "global.h"

# include <string.h>

# include "id3.h"
# include "id3v2.h"

#define TOTAL_KEYWORDS 137
#define MIN_WORD_LENGTH 3
#define MAX_WORD_LENGTH 4
#define MIN_HASH_VALUE 29
#define MAX_HASH_VALUE 237
/* maximum key range = 209, duplicates = 17 */

#ifdef __GNUC__
__inline
#endif
static unsigned int
hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const unsigned char asso_values[] =
    {
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238,  26,
       28,  16,   0, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238,   6,   1,  35,   3,   5,
       53,  30, 238,  52, 238,  50,   9,  11,  24,  60,
       21,  12,  51,  63,  25,  52,   0,  63,  57,  53,
       15, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238, 238, 238, 238, 238,
      238, 238, 238, 238, 238, 238
    };
  register int hval = 0;

  switch (len)
    {
      default:
      case 4:
        hval += asso_values[(unsigned char)str[3]];
      case 3:
        hval += asso_values[(unsigned char)str[2]];
      case 2:
        hval += asso_values[(unsigned char)str[1]];
      case 1:
        hval += asso_values[(unsigned char)str[0]];
        break;
    }
  return hval;
}

#ifdef __GNUC__
__inline
#endif
const struct id3v2_frame *
id3v2_hash (str, len)
     register const char *str;
     register unsigned int len;
{
  static const struct id3v2_frame wordlist[] =
    {
      {"MLL",	N_("MPEG location lookup table"),			0},
      {"TDA",	N_("Date"),						id3_text},
      {"TLE",	N_("Length"),						id3_text},
      {"TAL",	N_("Album/Movie/Show title"),				id3_text},
      {"TLA",	N_("Language(s)"),					id3_text},
      {"TALB",	N_("Album/Movie/Show title"),				id3_text},
      {"TMED",	N_("Media type"),					id3_text},
      {"TP4",	N_("Interpreted, remixed, or otherwise modified by"),	id3_text},
      {"TBP",	N_("BPM (Beats Per Minute)"),				id3_text},
      {"TPB",	N_("Publisher"),					id3_text},
      {"TPE4",	N_("Interpreted, remixed, or otherwise modified by"),	id3_text},
      {"TPA",	N_("Part of a set"),					id3_text},
      {"MLLT",	N_("MPEG location lookup table"),			0},
      {"TEN",	N_("Encoded by"),					id3_text},
      {"REV",	N_("Reverb"),						0},
      {"RVA",	N_("Relative volume adjustment"),			0},
      {"TBPM",	N_("BPM (Beats Per Minute)"),				id3_text},
      {"TDAT",	N_("Date"),						id3_text},
      {"RVAD",	N_("Relative volume adjustment"),			0},
      {"TMT",	N_("Media type"),					id3_text},
      {"TP3",	N_("Conductor/Performer refinement"),			id3_text},
      {"TLEN",	N_("Length"),						id3_text},
      {"TLAN",	N_("Language(s)"),					id3_text},
      {"ETC",	N_("Event timing codes"),				0},
      {"TT3",	N_("Subtitle/Description refinement"),			id3_text},
      {"TPE3",	N_("Conductor/Performer refinement"),			id3_text},
      {"EQU",	N_("Equalization"),					0},
      {"AENC",	N_("Audio encryption"),					0},
      {"TCM",	N_("Composer"),						id3_text},
      {"TP1",	N_("Lead artist(s)/Lead performer(s)/Soloist(s)/Performing group"),	id3_text},
      {"TP2",	N_("Band/Orchestra/Accompaniment"),			id3_text},
      {"EQUA",	N_("Equalization"),					0},
      {"TT1",	N_("Content group description"),			id3_text},
      {"TPE1",	N_("Lead performer(s)/Soloist(s)"),			id3_text},
      {"TT2",	N_("Title/Songname/Content description"),		id3_text},
      {"TPE2",	N_("Band/Orchestra/Accompaniment"),			id3_text},
      {"TRD",	N_("Recording dates"),					id3_text},
      {"TKE",	N_("Initial key"),					id3_text},
      {"TDY",	N_("Playlist delay"),					id3_text},
      {"IPL",	N_("Involved people list"),				0},
      {"TYE",	N_("Year"),						id3_text},
      {"LNK",	N_("Linked information"),				0},
      {"CNT",	N_("Play counter"),					0},
      {"WPB",	N_("Publishers official webpage"),			0},
      {"TRDA",	N_("Recording dates"),					id3_text},
      {"ULT",	N_("Unsychronized lyric/text transcription"),		0},
      {"TIM",	N_("Time"),						id3_text},
      {"TENC",	N_("Encoded by"),					id3_text},
      {"TDLY",	N_("Playlist delay"),					id3_text},
      {"TOA",	N_("Original artist(s)/performer(s)"),			id3_text},
      {"CRA",	N_("Audio encryption"),					0},
      {"TIME",	N_("Time"),						id3_text},
      {"TOL",	N_("Original lyricist(s)/text writer(s)"),		id3_text},
      {"GEO",	N_("General encapsulated object"),			0},
      {"GEOB",	N_("General encapsulated object"),			0},
      {"CRM",	N_("Encrypted meta frame"),				0},
      {"SLT",	N_("Synchronized lyric/text"),				0},
      {"MCI",	N_("Music CD Identifier"),				0},
      {"TPUB",	N_("Publisher"),					id3_text},
      {"TOAL",	N_("Original album/Movie/Show title"),			id3_text},
      {"MCDI",	N_("Music CD Identifier"),				0},
      {"POP",	N_("Popularimeter"),					0},
      {"TFT",	N_("File type"),					id3_text},
      {"RVRB",	N_("Reverb"),						0},
      {"PCNT",	N_("Play counter"),					0},
      {"BUF",	N_("Recommended buffer size"),				0},
      {"COM",	N_("Comments"),						0},
      {"TXT",	N_("Lyricist/Text writer"),				id3_text},
      {"PIC",	N_("Attached picture"),					0},
      {"WCM",	N_("Commercial information"),				0},
      {"TOT",	N_("Original album/Movie/Show title"),			id3_text},
      {"TOPE",	N_("Original artist(s)/performer(s)"),			id3_text},
      {"TCR",	N_("Copyright message"),				id3_text},
      {"TRC",	N_("ISRC (International Standard Recording Code)"),	id3_text},
      {"TEXT",	N_("Lyricist/Text writer"),				id3_text},
      {"TFLT",	N_("File type"),					id3_text},
      {"POPM",	N_("Popularimeter"),					0},
      {"APIC",	N_("Attached picture"),					0},
      {"ENCR",	N_("Encryption method registration"),			0},
      {"COMM",	N_("Comments"),						id3_comment},
      {"TIT3",	N_("Subtitle/Description refinement"),			id3_text},
      {"WCP",	N_("Copyright/Legal information"),			0},
      {"WAR",	N_("Official artist/performer webpage"),		0},
      {"TCO",	N_("Content type"),					id3_text},
      {"WAF",	N_("Official audio file webpage"),			0},
      {"STC",	N_("Synced tempo codes"),				0},
      {"PRIV",	N_("Private frame"),					0},
      {"ETCO",	N_("Event timing codes"),				0},
      {"TRK",	N_("Track number/Position in set"),			id3_text},
      {"TIT1",	N_("Content group description"),			id3_text},
      {"TIT2",	N_("Title/Songname/Content description"),		id3_text},
      {"TCOM",	N_("Composer"),						id3_text},
      {"WAS",	N_("Official audio source webpage"),			0},
      {"TKEY",	N_("Initial key"),					id3_text},
      {"TYER",	N_("Year"),						id3_text},
      {"LINK",	N_("Linked information"),				0},
      {"GRID",	N_("Group identification registration"),		0},
      {"TOR",	N_("Original release year"),				id3_text},
      {"WPUB",	N_("Publishers official webpage"),			0},
      {"TOF",	N_("Original filename"),				id3_text},
      {"TXX",	N_("User defined text information frame"),		0},
      {"TSI",	N_("Size"),						id3_text},
      {"TCOP",	N_("Copyright message"),				id3_text},
      {"WPAY",	N_("Payment"),						0},
      {"TCON",	N_("Content type"),					id3_text},
      {"IPLS",	N_("Involved people list"),				0},
      {"TOLY",	N_("Original lyricist(s)/text writer(s)"),		id3_text},
      {"USLT",	N_("Unsychronized lyric/text transcription"),		0},
      {"SYLT",	N_("Synchronized lyric/text"),				0},
      {"TSS",	N_("Software/Hardware and settings used for encoding"),	id3_text},
      {"OWNE",	N_("Ownership frame"),					0},
      {"TSIZ",	N_("Size"),						id3_text},
      {"TSSE",	N_("Software/Hardware and settings used for encoding"),	id3_text},
      {"COMR",	N_("Commercial frame"),					0},
      {"RBUF",	N_("Recommended buffer size"),				0},
      {"UFI",	N_("Unique file identifier"),				0},
      {"UFID",	N_("Unique file identifier"),				0},
      {"TRCK",	N_("Track number/Position in set"),			id3_text},
      {"TOFN",	N_("Original filename"),				id3_text},
      {"TRSN",	N_("Internet radio station name"),			id3_text},
      {"WCOM",	N_("Commercial information"),				0},
      {"TPOS",	N_("Part of a set"),					id3_text},
      {"USER",	N_("Terms of use"),					0},
      {"TOWN",	N_("File owner/licensee"),				id3_text},
      {"TSRC",	N_("ISRC (International Standard Recording Code)"),	id3_text},
      {"SYTC",	N_("Synchronized tempo codes"),				0},
      {"WXX",	N_("User defined URL link frame"),			0},
      {"WCOP",	N_("Copyright/Legal information"),			0},
      {"WOAR",	N_("Official artist/performer webpage"),		0},
      {"WOAF",	N_("Official audio file webpage"),			0},
      {"TORY",	N_("Original release year"),				id3_text},
      {"WOAS",	N_("Official audio source webpage"),			0},
      {"TXXX",	N_("User defined text information frame"),		0},
      {"TRSO",	N_("Internet radio station owner"),			id3_text},
      {"POSS",	N_("Position synchronisation frame"),			0},
      {"WXXX",	N_("User defined URL link frame"),			0},
      {"WORS",	N_("Official Internet radio station homepage"),		0}
    };

  static const short lookup[] =
    {
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,    0,   -1,   -1,
        -1,   -1,    1,   -1,   -1,   -1,   -1,    2,
      -180,    5, -134,   -2,    6,   -1,    7, -186,
      -129,   -2,   -1,   10,   11,   -1, -342,   -1,
        14,   15,   16,   17,   18,   19,   20,   21,
        22,   23,   24,   25,   -1,   26,   27,   28,
        29,   -1,   30,   31,   32,   33,   34, -340,
        37,   38,   39, -338,   42, -335,   45,   -1,
        46,   47,   48,   49,   50,   51,   52,   53,
        54, -331,   57,   58,   59,   60,   61, -328,
        -1,   64, -325,   67,   68,   69,   70, -323,
      -304,   76,   77,   78,   -1,   79,   80,   81,
      -302,   -1,   84,   85,   86,   87,   88,   -1,
        89,   -1,   90,   91,   92,   93,   94,   95,
      -291,   98,   99,  100,  101,  102,   -1,  103,
       104,  105,   -1,  106,   -1,  107,  108,  109,
       110,  -41,   -2,  111,  112, -296,  -24,   -3,
       116,  117,  118,  119,  -55,   -2,  -63,   -2,
        -1, -321,   -1,  122,  123,   -1,  124,   -1,
       125,  126,   -1,  127,  128,   -1,  129,  -17,
        -2,  -66,   -3,  -72,   -2,  130,  -75,   -2,
       131,  -82,   -2,   -1,  132,  -94,   -2,  133,
       -97,   -2, -102,   -2, -125,   -2,   -1,  134,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,   -1,   -1,   -1,   -1,   -1,   -1,
        -1,   -1,  135,   -1,   -1,  136
    };

  if (len <= MAX_WORD_LENGTH && len >= MIN_WORD_LENGTH)
    {
      register int key = hash (str, len);

      if (key <= MAX_HASH_VALUE && key >= 0)
        {
          register int index = lookup[key];

          if (index >= 0)
            {
              register const char *s = wordlist[index].id;

              if (*str == *s && !strcmp (str + 1, s + 1))
                return &wordlist[index];
            }
          else if (index < -TOTAL_KEYWORDS)
            {
              register int offset = - 1 - TOTAL_KEYWORDS - index;
              register const struct id3v2_frame *wordptr = &wordlist[TOTAL_KEYWORDS + lookup[offset]];
              register const struct id3v2_frame *wordendptr = wordptr + -lookup[offset + 1];

              while (wordptr < wordendptr)
                {
                  register const char *s = wordptr->id;

                  if (*str == *s && !strcmp (str + 1, s + 1))
                    return wordptr;
                  wordptr++;
                }
            }
        }
    }
  return 0;
}
