#ifndef OUTPUT_NULL_HPP
#define OUTPUT_NULL_HPP


#include <set>
#include "config.h"
#include "output.hpp"


class NullOutputStream;


class NullOutputContext : public IOutputContext
{
public:
  NullOutputContext();
  ~NullOutputContext();

  bool Initialize(const char* parameters);
  void Update();
  IOutputStream* OpenStream(ISampleSource* source);

private:
  void RemoveStream(NullOutputStream* stream);

  typedef std::set<NullOutputStream*> StreamList;
  StreamList m_streams;

  friend class NullOutputStream;
};


class NullOutputStream : public IOutputStream
{
private:
  NullOutputStream(NullOutputContext* context, ISampleSource* source);
  ~NullOutputStream();

public:
  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();

private:
  void ResetTimer();
  void Update();
  int DummyRead(int samples_to_read);

  NullOutputContext* m_context;

  ISampleSource* m_source;
  int m_channel_count;    //
  int m_sample_rate;      // cached stream format
  int m_bits_per_sample;  //

  bool m_is_playing;
  int m_volume;

  adr_u64 m_last_update;

  friend class NullOutputContext;
};


#endif
