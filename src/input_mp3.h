#ifndef INPUT_MP3_H
#define INPUT_MP3_H

#include <mpegsound.h>
#include "audiere.h"
#include "basic_source.h"
#include "types.h"
#include "utility.h"

namespace audiere {

  class MP3InputStream : public BasicSource, public Soundplayer {
  public:
    MP3InputStream();
    ~MP3InputStream();

    bool initialize(FilePtr file);

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& sample_format);
    int doRead(int frame_count, void* samples);
    void ADR_CALL reset();int previous_frame;

    bool setsoundtype(int stereo, int samplesize, int speed);
    bool putblock(void *buffer,int size);

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();


  private:
    void readID3v1Tags();
    void readID3v2Tags();

    FilePtr m_file;

    // from format chunk
    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    Mpegtoraw* m_decoder;
    Soundinputstream* m_loader;
    
    QueueBuffer m_buffer;

    int m_layer;
    int m_samples_per_frame;
    int m_total_frames;
  };

}

#endif
