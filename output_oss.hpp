#ifndef OUTPUT_OSS_HPP
#define OUTPUT_OSS_HPP


#include <memory>
#include "output.hpp"
#include "mixer.hpp"


class OSSOutputContext : public IOutputContext
{
public:
  OSSOutputContext();
  ~OSSOutputContext();

  bool Initialize(const char* parameters);
  void Update();
  IOutputStream* OpenStream(ISampleSource* source);

private:
  int m_output_device;

  Mixer m_mixer;
};


class OSSOutputStream : public IOutputStream
{
private:
  class Stopper : public ISampleSource {
  public:
    Stopper(ISampleSource* source) {
      m_source = source;
      m_stopped = false;
    }

    ~Stopper() {
      delete m_source;
    }

    bool IsStopped() {
      return m_stopped;
    }

    void SetStopped(bool stopped) {
      m_stopped = stopped;
    }
    
    void GetFormat(
      int& channel_count,
      int& sample_rate,
      int& bits_per_sample) {
      m_source->GetFormat(channel_count, sample_rate, bits_per_sample);
    }

    int Read(int sample_count, void* samples) {
      int read = m_source->Read(sample_count, samples);
      if (read == 0) {
	m_stopped = true;
      }
      return read;
    }

    bool Reset() {
      return m_source->Reset();
    }

  private:
    ISampleSource* m_source;
    bool m_stopped;
  };


private:
  OSSOutputStream(Mixer* mixer, ISampleSource* source);
  ~OSSOutputStream();

public:
  // IOutputStream
  void Play();
  void Stop();
  void Reset();
  bool IsPlaying();
  void SetVolume(int volume);
  int  GetVolume();

private:
  Mixer* m_mixer;
  Stopper* m_source;
  int m_volume;

  friend OSSOutputContext;
};


#endif
