#ifndef DEVICE_OSS_H
#define DEVICE_OSS_H


#include "audiere.h"
#include "mixer.h"


namespace audiere {

  class OSSAudioDevice : public RefImplementation<AudioDevice> {
  public:
    static OSSAudioDevice* create(const ParameterList& parameters);

  private:
    OSSAudioDevice(int output_device);
    ~OSSAudioDevice();

  public:
    void update();
    OutputStream* openStream(SampleSource* source);
    OutputStream* openBuffer(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format);

  private:
    int m_output_device;
    Mixer m_mixer;

    friend class OSSOutputStream;
  };


  class OSSOutputStream : public RefImplementation<OutputStream> {
  private:
    OSSOutputStream(OSSAudioDevice* device, SampleSource* source);
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
    Mixer& getMixer();

    RefPtr<OSSAudioDevice> m_device;
    SampleSource* m_source;

    friend class OSSAudioDevice;
  };

}


#endif
