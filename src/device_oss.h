#ifndef OUTPUT_OSS_HPP
#define OUTPUT_OSS_HPP


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
    void reset();
    bool isPlaying();
    void setVolume(int volume);
    int  getVolume();

  private:
    Mixer* m_mixer;
    SampleSource* m_source;
    int m_volume;

    friend class OSSOutputContext;
  };

}


#endif
