#ifndef INPUT_HPP
#define INPUT_HPP


#include "file.hpp"


// types

class ISampleSource
{
public:
  ~ISampleSource() { }

  virtual void GetFormat(
    int& channel_count,
    int& sample_rate,
    int& bits_per_sample) = 0;

  // |samples| should be
  // (sample_count * bits_per_sample * channel_count / 8) bytes
  virtual int Read(int sample_count, void* samples) = 0;
  virtual bool Reset() = 0;
};


extern ISampleSource* OpenInputStream(IFileSystem* fs, const char* filename);


#endif
