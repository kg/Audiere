#ifndef INPUT_MP3_H
#define INPUT_MP3_H

#include <mpg123.h>
#include <mpglib.h>
#include "audiere.h"
#include "types.h"
#include "utility.h"

namespace audiere {

  class MP3InputStream : public UnseekableSource {
  public:
    MP3InputStream();
    ~MP3InputStream();

    bool initialize(File* file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int ADR_CALL read(int sample_count, void* samples);
    void ADR_CALL reset();

  private:
    int read_header(struct frame* fr, unsigned long newhead);

  private:
    RefPtr<File> m_file;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    mpstr m_mp3;
    char* computed_buffer;

    int computed_buffer_pos;
    int computed_buffer_length;
  };

}

#endif
