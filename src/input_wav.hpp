#ifndef INPUT_WAV_HPP
#define INPUT_WAV_HPP


#include "config.h"
#include "input.hpp"
#include "file.hpp"


class WAVInputStream : public ISampleSource
{
public:
  WAVInputStream();
  ~WAVInputStream();

  bool Initialize(IFile* file);

  void GetFormat(
    int& channel_count,
    int& sample_rate,
    int& bits_per_sample);
  int Read(int sample_count, void* samples);
  bool Reset();

private:
  bool FindFormatChunk();
  bool FindDataChunk();
  bool SkipBytes(int size);

private:
  IFile* m_file;

  // from format chunk
  int m_channel_count;
  int m_bits_per_sample;
  int m_sample_rate;

  // from data chunk
  int m_data_chunk_location; // bytes
  int m_data_chunk_length;   // in samples

  int m_samples_left_in_chunk;
};


#endif
