#ifndef INPUT_FLAC_H
#define INPUT_FLAC_H


#include <FLAC++/decoder.h>
#include "audiere.h"
#include "utility.h"


namespace audiere {

  class FLACInputStream : public RefImplementation<audiere::SampleSource> {
  public:
    FLACInputStream();
    ~FLACInputStream();

    bool initialize(File* file);

    void ADR_CALL getFormat(
      int& channel_count, 
      int& sample_rate, 
      SampleFormat& sampleformat);
    int ADR_CALL read(int frame_count, void* samples);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    FLAC__StreamDecoderWriteStatus write(
      const FLAC__Frame* frame,
      const FLAC__int32* const buffer[]);

    RefPtr<File> m_file;

    FLAC::Decoder::SeekableStream* m_decoder;

    /**
     * this is the buffer used to combine the different channels from FLAC
     * into an interleaved stream.  it is stored in the class so we don't
     * have to do a costly allocation whenever FLAC has more data.
     */
    SizedBuffer m_multiplexer;

    /**
     * this stores a queue of sample data coming from FLAC and being read by
     * the client of the stream
     */
    QueueBuffer m_buffer;

    int m_channel_count;
    int m_sample_rate;
    SampleFormat m_sample_format;

    int m_length;
    int m_position;

    friend class MyFLACDecoder;
  };

}


#endif
