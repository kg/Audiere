#ifndef STREAM_HPP
#define STREAM_HPP


#include <acoustique.h>
#include "output.hpp"
#include "threads.hpp"


class Context;


class Stream : public Synchronized, public ISampleSource
{
private:
  Stream() { }  // Initialize() is the constructor
  bool Initialize(Context* context, const char* filename);

public:
  ~Stream();

  void Play();
  void Pause();
  void ResetInputAndOutput(); // annoying name - conflict with ISampleSource
  bool IsPlaying();
  void SetRepeat(bool repeat);
  bool IsRepeating();
  void SetVolume(int volume);
  int  GetVolume();

private:
  // ISampleSource implementation
  void GetFormat(int& channel_count, int& sample_rate, int& bits_per_sample);
  int Read(int sample_count, void* samples);
  void Reset();

  static int  ACQ_CALL FileRead(void* opaque, void* bytes, int byte_count);
  static void ACQ_CALL FileReset(void* opaque);
  
  static int GetFileFormat(const char* filename);

private:
  Context*       m_context;
  ADR_FILE       m_file;
  ACQ_STREAM     m_input_stream;
  IOutputStream* m_output_stream;

  int m_sample_size;  // convenience
  bool m_repeat;

private:
  friend Context;
};


#endif
