#ifndef INPUT_WAV_HPP
#define INPUT_WAV_HPP


#include "audiere.h"
#include "types.h"


namespace audiere {

  class WAVInputStream : public DLLImplementation<SampleSource> {
  public:
    WAVInputStream();
    ~WAVInputStream();

    bool initialize(File* file);

    void getFormat(
      int& channel_count,
      int& sample_rate,
      int& bits_per_sample);
    int read(int sample_count, void* samples);
    void reset();

    bool isSeekable();
    int getLength();
    void setPosition(int position);
    int getPosition();

  private:
    bool FindFormatChunk();
    bool FindDataChunk();
    bool SkipBytes(int size);

  private:
    File* m_file;

    // from format chunk
    int m_channel_count;
    int m_bits_per_sample;
    int m_sample_rate;

    // from data chunk
    int m_data_chunk_location; // bytes
    int m_data_chunk_length;   // in samples

    int m_samples_left_in_chunk;
  };

}


#endif
