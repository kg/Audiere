#ifndef REPEATABLE_H
#define REPEATABLE_H


#include "audiere.h"


namespace audiere {

  class RepeatableStream : public RefImplementation<SampleSource> {
  public:
    RepeatableStream(SampleSource* source);

    void setRepeat(bool repeat);
    bool getRepeat();

    void ADR_CALL getFormat(
      int& channel_count,
      int& sample_rate,
      SampleFormat& bits_per_sample);

    int ADR_CALL read(int frame_count, void* buffer);
    void ADR_CALL reset();

    bool ADR_CALL isSeekable();
    int  ADR_CALL getLength();
    void ADR_CALL setPosition(int position);
    int  ADR_CALL getPosition();

  private:
    bool m_repeat;
    RefPtr<SampleSource> m_source;
    int m_sample_size;  // convenience
  };

}


#endif
