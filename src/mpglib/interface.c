// MPGLIB modification by Jacky Chong
//
// 1) MODIFIED EXIT HANDLES TO RETURN A -1 INSTEAD.
// 2) gracefully quit on decode_header()
// 3) gracefully quit with read_head()
// 4) initialize mp->header with -1
// 5) bad frames check.
// 6) id3 tag support
// 7) headers are now read in a streaming kind of way.

#include <stdlib.h>
#include <stdio.h>

#include "mpg123.h"
#include "mpglib.h"

/* Global mp .. it's a hack */
struct mpstr *gmp;


BOOL InitMP3(struct mpstr *mp) 
{
	static int init = 0;

	memset(mp,0,sizeof(struct mpstr));

	mp->framesize = 0;
	mp->fsizeold = -1;
	mp->bsize = 0;
	mp->head = mp->tail = NULL;
	mp->fr.single = -1;
	mp->bsnum = 0;
	mp->synth_bo = 1;
  mp->header = 0;
  mp->bytes_to_skip = 0;
  memset(mp->id3_buffer, 0, 6);

	if(!init) {
		init = 1;
		make_decode_tables(32767);
		init_layer2();
		init_layer3(SBLIMIT);
	}

	return !0;
}

void ExitMP3(struct mpstr *mp)
{
	struct buf *b,*bn;
	
	b = mp->tail;
	while(b) {
		free(b->pnt);
		bn = b->next;
		free(b);
		b = bn;
	}
}

static struct buf *addbuf(struct mpstr *mp,char *buf,int size)
{
	struct buf *nbuf;

	nbuf = malloc( sizeof(struct buf) );
	if(!nbuf) {
		fprintf(stderr,"Out of memory!\n");
		return NULL;
	}
	nbuf->pnt = malloc(size);
	if(!nbuf->pnt) {
		free(nbuf);
		return NULL;
	}
	nbuf->size = size;
	memcpy(nbuf->pnt,buf,size);
	nbuf->next = NULL;
	nbuf->prev = mp->head;
	nbuf->pos = 0;

	if(!mp->tail) {
		mp->tail = nbuf;
	}
	else {
	  mp->head->next = nbuf;
	}

	mp->head = nbuf;
	mp->bsize += size;

	return nbuf;
}

static void remove_buf(struct mpstr *mp)
{
  struct buf *buf = mp->tail;
  
  mp->tail = buf->next;
  if(mp->tail)
    mp->tail->prev = NULL;
  else {
    mp->tail = mp->head = NULL;
  }
  
  free(buf->pnt);
  free(buf);

}

static int read_buf_byte(struct mpstr *mp)
{
	unsigned int b;

	int pos;

  if (mp->bsize == 0) {
    return -1;
  }

	pos = mp->tail->pos;
	while(pos >= mp->tail->size) {
		remove_buf(mp);
		pos = mp->tail->pos;
		if(!mp->tail) {
			fprintf(stderr,"Fatal error!\n");
			//exit(1);
      return -1;
		}
	}

	b = mp->tail->pnt[pos];
	mp->bsize--;
	mp->tail->pos++;
	

	return b;
}

static int skip_id3_tag(struct mpstr *mp, unsigned head)
{
  int bytes_to_read_now = 0;
  int c, i;
  unsigned s;

  // old v1 tags
  // in short, this is a 128 byte tag at the end of the file.
  if ((head>>8) == ('T'<<16)+('A'<<8)+'G') {
    mp->bytes_to_skip = 128 - 4; // 'TAG' + 1 data byte
    if (mp->bsize < 124) {
      mp->bytes_to_skip -= mp->bsize;
      bytes_to_read_now = mp->bsize;
    } else {
      bytes_to_read_now = 124;
      mp->bytes_to_skip = 0;
    }

    while (bytes_to_read_now) {
      read_buf_byte(mp);
      --bytes_to_read_now;
    }

    return 1;
  } 

  #define CATCH_BYTE(x) { if (x == -1) { return -1; } }
  // new id3 v2 tags
  // format of the header:
  // id,      3 bytes 'ID3'
  // version, 2 bytes
  // flags,   1 byte
  // size,    4 byte
  if ((head>>8) == ('I'<<16)+('D'<<8)+'3') {
    // since we have already read 4 bytes of the header, 
    // we need to skip 2 more bytes to get to the size!
    // and then read the 4 bytes!
    for (c=0; c<6; ++c)
    {
      if (mp->id3_buffer[c] == 0) {
        i = read_buf_byte(mp);
        CATCH_BYTE(i);
        mp->id3_buffer[c] = i;
      }
    }

    s = mp->id3_buffer[2] & 127;
    s <<= 7;
    s += mp->id3_buffer[3] & 127;
    s <<= 7;
    s += mp->id3_buffer[4] & 127;
    s <<= 7;
    s += mp->id3_buffer[5] & 127;

    mp->bytes_to_skip = s; 
    if (mp->bsize < s) {
      mp->bytes_to_skip -= mp->bsize;
      bytes_to_read_now = mp->bsize;
    } else {
      bytes_to_read_now = s;
      mp->bytes_to_skip = 0;
    }

    while (bytes_to_read_now) {
      read_buf_byte(mp);
      --bytes_to_read_now;
    }

    // reset our stuff!
    memset(mp->id3_buffer, 0, 6);
    return 1;
  }
  #undef CATCH_BYTE

  return 0;
}

extern int head_check(unsigned long head);
static int read_head(struct mpstr *mp)
{
  unsigned long head;
  unsigned char c;
  int i;


  #define CATCH_BYTE(x) { if (x == -1) { mp->header=head; return 1; } }

  head=mp->header;
  if (mp->header == 0) {
    // read the first four bytes
	  i = read_buf_byte(mp);
    CATCH_BYTE(i);
    c = i;
    head = c;
	  head <<= 8;
	  i = read_buf_byte(mp);
    CATCH_BYTE(i);
    c = i;
    head |= c;
	  head <<= 8;
	  i = read_buf_byte(mp);
    CATCH_BYTE(i);
    c = i;
    head |= c;
	  head <<= 8;
	  i = read_buf_byte(mp);
    CATCH_BYTE(i);
    c = i;
    head |= c;
  }

  i = 0;
  
  // see if the header is compliant!
  // AAAAAAAA AAABBCCD EEEEFFGH IIJJKLMM
  // A = frame sync
  // B = MPEG version
  // C = MPEG type
  // D = Protection type
  // E = Bitrate
  // F = Sampling rate
  // G = Padding type
  // H = Private
  // I = Channel Mode
  // J = Mode Extension
  // K = Copyright
  // L = Original
  // M = Emphasis

  // (note. this could all be done via head_check() but this is easier to understand)
  // we're gonna check if:
  // 1) sampling rate is valid. ie. both bits of F cannot be 1
  // 2) frame sync is valid. ie. all bits of A must be 1
  // 3) bitrate is valid. ie. all bits of E cannot be 1
  // 4) mpeg type is valid. ie. all bits of B cannot be 0
  if (head_check(head)) {
    i = 1;
  }

  // see if this is a id3 tag
  if (i != 1) {
    if (skip_id3_tag(mp, head) == 1) {
      mp->header = 0;
    }
  }

  // keep finding the magical header!
  while (i != 1) {
    i = read_buf_byte(mp);
    CATCH_BYTE(i);
    c = i;
    head <<= 8;
    head |= c;

    i = 0;
    if (head_check(head)) {
      i = 1;
    }
    if (i != 1) {
      if (skip_id3_tag(mp, head) == 1) {
        mp->header = 0;
      }
    }
  }

  // put the header in if there's no problem!
  mp->header = head;
  return 0;

  #undef CATCH_BYTE
}

int decodeMP3(struct mpstr *mp,char *in,int isize,char *out,
		int osize,int *done)
{
	int len;
  int bytes_to_skip_now;

	gmp = mp;

	if(osize < 4608) {
		fprintf(stderr,"To less out space\n");
		return MP3_ERR;
	}

	if(in) {
		if(addbuf(mp,in,isize) == NULL) {
			return MP3_ERR;
		}
	}

  // if we still have bytes to skip (from the id3 tags or something else), skip it now!
  if (mp->bytes_to_skip) {
    if (mp->bytes_to_skip < mp->bsize) {
      bytes_to_skip_now = mp->bytes_to_skip;
      mp->bytes_to_skip = 0;
    } else {
      bytes_to_skip_now = mp->bytes_to_skip - mp->bsize;
      mp->bytes_to_skip -= bytes_to_skip_now;
    }

    while (bytes_to_skip_now) {
      read_buf_byte(mp);
      --bytes_to_skip_now;
    }
  }

	/* First decode header */
	if(mp->framesize == 0) {
		if(mp->bsize < 4) {
			return MP3_NEED_MORE;
		}
    if (read_head(mp)==1) {
      return MP3_NEED_MORE;
    }
    decode_header(&mp->fr,mp->header);
		mp->framesize = mp->fr.framesize;
	}

	if(mp->fr.framesize > mp->bsize)
		return MP3_NEED_MORE;

	wordpointer = mp->bsspace[mp->bsnum] + 512;
	mp->bsnum = (mp->bsnum + 1) & 0x1;
	bitindex = 0;

	len = 0;
	while(len < mp->framesize) {
		int nlen;
		int blen = mp->tail->size - mp->tail->pos;
		if( (mp->framesize - len) <= blen) {
                  nlen = mp->framesize-len;
		}
		else {
                  nlen = blen;
                }
		memcpy(wordpointer+len,mp->tail->pnt+mp->tail->pos,nlen);
                len += nlen;
                mp->tail->pos += nlen;
		mp->bsize -= nlen;
                if(mp->tail->pos == mp->tail->size) {
                   remove_buf(mp);
                }
	}

	*done = 0;
	if(mp->fr.error_protection)
           getbits(16);
        switch(mp->fr.lay) {
          case 1:
	    do_layer1(&mp->fr,(unsigned char *) out,done);
            break;
          case 2:
	    do_layer2(&mp->fr,(unsigned char *) out,done);
            break;
          case 3:
	    do_layer3(&mp->fr,(unsigned char *) out,done);
            break;
        }

	mp->fsizeold = mp->framesize;
	mp->framesize = 0;
  mp->header = 0;

	return MP3_OK;
}

int set_pointer(long backstep)
{
  unsigned char *bsbufold;
  if(gmp->fsizeold < 0 && backstep > 0) {
    fprintf(stderr,"Can't step back %ld!\n",backstep);
    return MP3_ERR;
  }
  bsbufold = gmp->bsspace[gmp->bsnum] + 512;
  wordpointer -= backstep;
  if (backstep)
    memcpy(wordpointer,bsbufold+gmp->fsizeold-backstep,backstep);
  bitindex = 0;
  return MP3_OK;
}




