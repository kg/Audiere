#ifndef INPUT_MP3_H
#define INPUT_MP3_H

#include <mpegsound.h>
#include "audiere.h"
#include "types.h"
#include "utility.h"

namespace audiere {

  class MP3InputStream : public UnseekableSource, public Soundplayer {
  public:
    MP3InputStream();
    ~MP3InputStream();

    bool initialize(File* file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int ADR_CALL read(int frame_count, void* samples);
    void ADR_CALL reset();

    bool setsoundtype(int stereo, int samplesize, int speed);
    void set8bitmode();
    bool putblock(void *buffer,int size);
    int  putblock_nt(void *buffer, int size);

  private:
    RefPtr<File> m_file;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    Mpegtoraw* m_decoder;
    Soundinputstream* m_loader;
    
    QueueBuffer m_buffer;
  };

}

#endif
