#ifndef STREAM_HPP
#define STREAM_HPP


#include <audiere.h>
#include "input.hpp"
#include "repeatable.hpp"
#include "threads.hpp"


class Context;
class IOutputStream;


class Stream
{
private:
  Stream();
  bool Initialize(Context* context, const char* filename);
  void Destructor();

public:
  ~Stream();

  void Play();
  void Pause();
  void Reset();
  bool IsPlaying();
  void SetRepeat(bool repeat);
  bool IsRepeating();
  void SetVolume(int volume);
  int  GetVolume();

private:
  Context* m_context;

  RepeatableStream* m_input_stream;
  IOutputStream*    m_output_stream;

  friend Context;
};


#endif
