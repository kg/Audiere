#ifndef REPEATABLE_HPP
#define REPEATABLE_HPP


#include "audiere.h"


namespace audiere {

  class RepeatableStream : public DLLImplementation<SampleSource> {
  public:
    RepeatableStream(SampleSource* source, bool initial_state);
    ~RepeatableStream();

    void setRepeat(bool repeat);
    bool getRepeat();

    // ISampleSource implementation
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
    bool m_repeat;
    SampleSource* m_source;
    int m_sample_size;  // convenience
  };

}


#endif
