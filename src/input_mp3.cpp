/*
  THIS IS WRITTEN FOR CHAD AUSTIN FOR AUDIERE
  By Jacky Chong

  In short, this is some wierd shit I wrote up because Chad
  didn't want to touch this. Also, I have no idea what I've
  done as well to get it work as well.
*/

#include <string.h>
#include "input_mp3.h"
#include "utility.h"
#include "debug.h"

#define MP3_IN_BUFFER_SIZE (8192*4)
#define MP3_OUT_BUFFER_SIZE (8192*4)
#define MP3_BUFFER_SIZE (8192)

namespace audiere {

  MP3InputStream::MP3InputStream() {
    m_file = 0;

    m_channel_count = 2;
    m_sample_rate = 44100;
    m_sample_format = SF_S16;
  }

  
  MP3InputStream::~MP3InputStream() {
    if (m_file) {
      ExitMP3(&m_mp3);
      free(computed_buffer);
    }
  }


  bool
  MP3InputStream::initialize(File* file) {
    m_file = file;
    frame fr;

    InitMP3(&m_mp3);
    
    u32 head;
    u8 byte;

    int read = m_file->read(&byte, 1);
    head = byte;
    head <<= 8;
    read += m_file->read(&byte, 1);
    head |= byte;
    head <<= 8;
    read += m_file->read(&byte, 1);
    head |= byte;
    head <<= 8;
    read += m_file->read(&byte, 1);
    head |= byte;
    if (read != 4) {
      return false;
    }
    
    // see if it's an id3 tag first!
    if (head>>8 == ('I'<<16)+('D'<<8)+'3') {
      unsigned int skip;
      char buffer[6];
      if (m_file->read(&buffer, 6) != 6) {
        return false;
      }
 
      skip = buffer[2] & 127;
      skip <<= 7;
      skip += buffer[3] & 127;
      skip <<= 7;
      skip += buffer[4] & 127;
      skip <<= 7;
      skip += buffer[5] & 127;

      // skip the stupid id3 tag to get our stuff.
      char* temp = new char[skip];
      read = m_file->read(temp, skip);
      delete[] temp;
      if (read != skip) {
        return false;
      }

      // re-read the header again.
      read = m_file->read(&byte, 1);
      head = byte;
      head <<= 8;
      read += m_file->read(&byte, 1);
      head |= byte;
      head <<= 8;
      read += m_file->read(&byte, 1);
      head |= byte;
      head <<= 8;
      read += m_file->read(&byte, 1);
      head |= byte;
      if (read != 4) {
        return false;
      }
    }


    if (read_header(&fr, head) == FALSE) {
      m_file = 0;
      ExitMP3(&m_mp3);
      return false;
    }

    ADR_LOG("decoder_header succeeded. This is a valid mp3.");

    // reset the file since we are going to reread it anyway! (mindless hack #2)
    m_file->seek(0, File::BEGIN);

    // get the info we need from this evil frame header!
    m_sample_rate = fr.sampling_frequency;
    m_channel_count = fr.stereo;

    // temporary stuff
    computed_buffer = 0;
    computed_buffer_pos = 0;
    computed_buffer_length = 0;

    // we're now set!
    return true;
  }


  void
  MP3InputStream::getFormat(
    int& channel_count, 
    int& sample_rate, 
    SampleFormat& sample_format) 
  {
    channel_count = m_channel_count;
    sample_rate = m_sample_rate;
    sample_format = m_sample_format;
  }

  
  int
  MP3InputStream::read(int sample_count, void* samples) {
    char buffer_in[MP3_IN_BUFFER_SIZE];
    char buffer_out[MP3_OUT_BUFFER_SIZE];

    int read_left = sample_count * 4;
    int sample_read = 0;

    // mindless hack #3, getting it to read!
    while (read_left > 0) {

      // fill the decoded buffer up!
      if (computed_buffer_pos >= computed_buffer_length) {
        int in_length = m_file->read(buffer_in, MP3_IN_BUFFER_SIZE);
        int out_length = computed_buffer_pos = computed_buffer_length = 0;
        int status = decodeMP3(&m_mp3, buffer_in, in_length, buffer_out, MP3_OUT_BUFFER_SIZE, &out_length);
        if (status == MP3_ERR) {
          return sample_read / 4;
        }

        while (status == MP3_OK) {
          if ((computed_buffer_pos + out_length) > computed_buffer_length || computed_buffer == 0) {
            computed_buffer_length = computed_buffer_pos + out_length;
            computed_buffer = (char*)realloc(computed_buffer, computed_buffer_length);
          }

          memcpy(computed_buffer+computed_buffer_pos, buffer_out, out_length);
          computed_buffer_pos += out_length;
          status = decodeMP3(&m_mp3, NULL, 0, buffer_out, MP3_OUT_BUFFER_SIZE, &out_length);
        }

        computed_buffer_pos = 0; 
      }


      int read_count = 0;

      if (read_left > 0) {
        if (computed_buffer_pos + read_left < computed_buffer_length) {
          memcpy(samples, computed_buffer+computed_buffer_pos, read_left);
          sample_read = read_left;
          read_count = read_left;
          read_left = 0;
        } else {
          if (computed_buffer_length > 0) {
            // copy as much as we can!
            memcpy(samples, computed_buffer+computed_buffer_pos, computed_buffer_length-computed_buffer_pos);
            read_left -= computed_buffer_length-computed_buffer_pos;
            read_count = computed_buffer_length-computed_buffer_pos;
            sample_read += read_count;
          } else {
            // bomb out!
            read_left = 0;
            read_count = 0;
          }
        }

        computed_buffer_pos += read_count;
      }
    }

    return (int)(sample_read/4);
  }


  void
  MP3InputStream::reset() {
    ExitMP3(&m_mp3);
    InitMP3(&m_mp3);
    m_file->seek(0, File::BEGIN);
  }



  // in case you're wondering, yes the code is ripped from the mpglib common.c
  int tabsel_123[2][3][16] = {
    { {0,32,64,96,128,160,192,224,256,288,320,352,384,416,448,},
      {0,32,48,56, 64, 80, 96,112,128,160,192,224,256,320,384,},
      {0,32,40,48, 56, 64, 80, 96,112,128,160,192,224,256,320,} },

    { {0,32,48,56,64,80,96,112,128,144,160,176,192,224,256,},
      {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,},
      {0,8,16,24,32,40,48,56,64,80,96,112,128,144,160,} }
  };

  long freqs[9] = { 44100, 48000, 32000,
                    22050, 24000, 16000,
                    11025, 12000, 8000 };

  /*
   * the code a header and write the information
   * into the frame structure
   */
  int MP3InputStream::read_header(struct frame *fr,unsigned long newhead)
  {
    if( newhead & (1<<20) ) {
      fr->lsf = (newhead & (1<<19)) ? 0x0 : 0x1;
      fr->mpeg25 = 0;
    }
    else {
      fr->lsf = 1;
      fr->mpeg25 = 1;
    }
  
    fr->lay = 4-((newhead>>17)&3);
    if( ((newhead>>10)&0x3) == 0x3) {
      fprintf(stderr,"Stream error\n");
      exit(1);
    }
    if(fr->mpeg25) {
      fr->sampling_frequency = 6 + ((newhead>>10)&0x3);
    }
    else
      fr->sampling_frequency = ((newhead>>10)&0x3) + (fr->lsf*3);
    fr->error_protection = ((newhead>>16)&0x1)^0x1;

    if(fr->mpeg25) /* allow Bitrate change for 2.5 ... */
      fr->bitrate_index = ((newhead>>12)&0xf);

    fr->bitrate_index = ((newhead>>12)&0xf);
    fr->padding   = ((newhead>>9)&0x1);
    fr->extension = ((newhead>>8)&0x1);
    fr->mode      = ((newhead>>6)&0x3);
    fr->mode_ext  = ((newhead>>4)&0x3);
    fr->copyright = ((newhead>>3)&0x1);
    fr->original  = ((newhead>>2)&0x1);
    fr->emphasis  = newhead & 0x3;

    fr->stereo    = (fr->mode == MPG_MD_MONO) ? 1 : 2;

    if(!fr->bitrate_index)
    {
      fprintf(stderr,"Free format not supported.\n");
      return (0);
    }

    switch(fr->lay)
    {
      case 1:
      #ifdef LAYER1
        #if 0
        fr->jsbound = (fr->mode == MPG_MD_JOINT_STEREO) ? (fr->mode_ext<<2)+4 : 32;
        #endif
        fr->framesize  = (long) tabsel_123[fr->lsf][0][fr->bitrate_index] * 12000;
        fr->framesize /= freqs[fr->sampling_frequency];
        fr->framesize  = ((fr->framesize+fr->padding)<<2)-4;
      #else
        fprintf(stderr,"Not supported!\n");
      #endif
        break;

      case 2:
      #ifdef LAYER2
        #if 0
        fr->jsbound = (fr->mode == MPG_MD_JOINT_STEREO) ? (fr->mode_ext<<2)+4 : fr->II_sblimit;
        #endif
        fr->framesize = (long) tabsel_123[fr->lsf][1][fr->bitrate_index] * 144000;
        fr->framesize /= freqs[fr->sampling_frequency];
        fr->framesize += fr->padding - 4;
      #else
        fprintf(stderr,"Not supported!\n");
      #endif
        break;

      case 3:
      #if 0
        fr->do_layer = do_layer3;
        if(fr->lsf)
          ssize = (fr->stereo == 1) ? 9 : 17;
        else
          ssize = (fr->stereo == 1) ? 17 : 32;
      #endif

      #if 0
        if(fr->error_protection)
          ssize += 2;
      #endif
          fr->framesize  = (long) tabsel_123[fr->lsf][2][fr->bitrate_index] * 144000;
          fr->framesize /= freqs[fr->sampling_frequency]<<(fr->lsf);
          fr->framesize = fr->framesize + fr->padding - 4;
        break;
      
      default:
        fprintf(stderr,"Sorry, unknown layer type.\n"); 
        return (0);
    }

    fr->sampling_frequency = freqs[fr->sampling_frequency];
    return 1;
  }
}
