#ifndef DEVICE_OSS_HPP
#define DEVICE_OSS_HPP


#include <memory>
#include "audiere.h"
#include "mixer.h"


namespace audiere {

  class OSSAudioDevice : public DLLImplementation<AudioDevice> {
  public:
    OSSAudioDevice();
    ~OSSAudioDevice();

    bool initialize(const char* parameters);
    void update();
    OutputStream* openStream(SampleSource* source);
    OutputStream* openBuffer(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format);

  private:
    int m_output_device;

    Mixer m_mixer;
  };


  class OSSOutputStream : public DLLImplementation<OutputStream> {
  private:
    OSSOutputStream(Mixer* mixer, SampleSource* source);
    ~OSSOutputStream();

  public:
    void play();
    void stop();
    bool isPlaying();
    void reset();

    void setRepeat(bool repeat);
    bool getRepeat();
    void  setVolume(float volume);
    float getVolume();
    void  setPan(float pan);
    float getPan();

    bool isSeekable();
    int  getLength();
    void setPosition(int position);
    int  getPosition();

  private:
    Mixer* m_mixer;
    SampleSource* m_source;
    float m_volume;

    friend class OSSAudioDevice;
  };

}


#endif
