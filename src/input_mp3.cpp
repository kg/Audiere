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


  class MyLoader : public Soundinputstream {
  public:
    MyLoader(File* file) {
      m_file = file;
      m_eof = false;
    }

    bool open(char* /*filename*/) {
      return true; // already open!
    }

    void close() {
      m_file = 0;
    }

    int getbytedirect() {
      u8 b;
      if (m_file->read(&b, 1) == 1) {
        return b;
      } else {
        seterrorcode(SOUND_ERROR_FILEREADFAIL);
        m_eof = true;
        return -1;
      }
    }

    bool _readbuffer(char* buffer, int size) {
      if (m_file->read(buffer, size) == size) {
        return true;
      } else {
        seterrorcode(SOUND_ERROR_FILEREADFAIL);
        m_eof = true;
        return false;
      }
    }

    bool eof() {
      return m_eof;
    }

    int getblock(char* buffer, int size) {
      int read = m_file->read(buffer, size);
      if (read != size) {
        m_eof = true;
      }
      return size;
    }

    int getsize() {
      int pos = m_file->tell();
      m_file->seek(0, File::END);
      int size = m_file->tell();
      m_file->seek(pos, File::BEGIN);
      return size;
    }

    void setposition(int pos) {
      m_file->seek(pos, File::BEGIN);
      m_eof = false;
    }

    int getposition() {
      return m_file->tell();
    }

  private:
    RefPtr<File> m_file;
    bool m_eof;
  };


  MP3InputStream::MP3InputStream() {
    m_file = 0;

    m_channel_count = 2;
    m_sample_rate = 44100;
    m_sample_format = SF_S16;

    m_decoder = 0;
    m_loader = 0;
  }

  
  MP3InputStream::~MP3InputStream() {
    if (m_decoder) {
      delete m_decoder;
    }
    if (m_loader) {
      delete m_loader;
    }
  }


  bool
  MP3InputStream::initialize(File* file) {
    m_file = file;
    m_loader = new MyLoader(file);
    m_decoder = new Mpegtoraw(m_loader, this);

    // empty filename because we don't use it
    m_decoder->initialize("");

    // this should call setsoundtype with the format of the stream
    if (!m_decoder->run(1)) {
      return false;
    }

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
  MP3InputStream::read(int frame_count, void* samples) {
    const int frame_size = m_channel_count * GetSampleSize(m_sample_format);

    int frames_read = 0;
    u8* out = (u8*)samples;

    while (frames_read < frame_count) {

      // no more samples?  ask the MP3 for more
      if (m_buffer.getSize() < frame_size) {
        if (!m_decoder->run(1)) {
          // done decoding?
          return frames_read;
        }

        // if the buffer is still empty, we are done
        if (m_buffer.getSize() < frame_size) {
          return frames_read;
        }
      }

      const int frames_left = frame_count - frames_read;
      const int frames_to_read = std::min(
        frames_left,
        m_buffer.getSize() / frame_size);

      m_buffer.read(out, frames_to_read * frame_size);
      out += frames_to_read * frame_size;
      frames_read += frames_to_read;
    }

    return frames_read;
  }


  void
  MP3InputStream::reset() {
    m_file->seek(0, File::BEGIN);

    if (m_decoder) {
      delete m_decoder;
    }
    if (m_loader) {
      delete m_loader;
    }

    m_loader = new MyLoader(m_file.get());
    m_decoder = new Mpegtoraw(m_loader, this);

    // empty filename because we don't use it
    m_decoder->initialize("");

    // this should call setsoundtype with the format of the stream
    if (!m_decoder->run(1)) {
      return;
    }
  }


  bool
  MP3InputStream::initialize(char* /*filename*/) {
    // nothing!
    return true;
  }


  bool
  MP3InputStream::setsoundtype(int stereo, int samplesize, int speed) {
    m_channel_count = (stereo ? 2 : 1);
    m_sample_rate = speed;

    if (samplesize == 8) {
      m_sample_format = SF_U8;
    } else if (samplesize == 16) {
      m_sample_format = SF_S16;
    } else {
      return false;
    }

    return true;
  }


  bool
  MP3InputStream::putblock(void* buffer, int size) {
    m_buffer.write(buffer, size);
    return true;
  }

}
