#ifndef DEVICE_AL_H
#define DEVICE_AL_H


extern "C" {
  #include <dmedia/audio.h>
}

#include "audiere.h"
#include "mixer.h"
#include "resampler.h"


namespace audiere {

  class ALAudioDevice : public RefImplementation<AudioDevice> {
  public:
    static ALAudioDevice* create(const ParameterList& parameters);

  private:
    ALAudioDevice(ALport port, int rate);
    ~ALAudioDevice();

  public:
    void update();
    OutputStream* openStream(SampleSource* source);
    OutputStream* openBuffer(
      void* samples, int sample_count,
      int channel_count, int sample_rate, SampleFormat sample_format);

  private:
    ALport m_port;
    RefPtr<Mixer> m_mixer;

    friend class ALOutputStream;
  };


  class ALOutputStream : public RefImplementation<OutputStream> {
  private:
    ALOutputStream(ALAudioDevice* device, SampleSource* source);
    ~ALOutputStream();

  public:
    void play();
    void stop();
    bool isPlaying();
    void reset();

    void setRepeat(bool repeat);
    bool getRepeat();
    void setVolume(float volume);
    float getVolume();
    void setPan(float pan);
    float getPan();

    bool isSeekable();
    int  getLength();
    void setPosition(int position);
    int  getPosition();

  private:
    Mixer& getMixer();

    RefPtr<ALAudioDevice> m_device;
    SampleSource* m_source;

    friend class ALAudioDevice;
  };

}


#endif
