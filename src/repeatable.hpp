#ifndef REPEATABLE_HPP
#define REPEATABLE_HPP


#include "input.hpp"


class RepeatableStream : public ISampleSource
{
public:
  RepeatableStream(ISampleSource* source, bool initial_state);
  ~RepeatableStream();

  void SetRepeat(bool repeat);
  bool GetRepeat();

  // ISampleSource implementation
  void GetFormat(
    int& channel_count,
    int& sample_rate,
    int& bits_per_sample);

  int Read(int sample_count, void* samples);
  bool Reset();

private:
  bool m_repeat;
  ISampleSource* m_source;
  int m_sample_size;  // convenience
};


#endif
