#ifndef RESAMPLER_HPP
#define RESAMPLER_HPP


#include "config.h"
#include "output.hpp"


class Resampler : public ISampleSource
{
public:
  Resampler(ISampleSource* source);
  ~Resampler();

  // for now, resamplers always return (2, 44100, 16)
  void GetFormat(int& channel_count, int& sample_rate, int& bits_per_sample);

  int Read(int sample_count, void* samples);
  void Reset();

private:
  void FillBuffer();
  void ResetState();

private:
  ISampleSource* m_source;
  int m_native_channel_count;
  int m_native_sample_rate;
  int m_native_bits_per_sample;

  static const int NATIVE_BUFFER_SIZE = 4096;
  adr_s16 m_native_buffer[NATIVE_BUFFER_SIZE * 2];
  adr_s16* m_position;
  unsigned m_samples_left;  // number of samples left to consume

  unsigned m_time;
  adr_s16 m_sl; // left channel
  adr_s16 m_sr; // right channel
};


#endif
