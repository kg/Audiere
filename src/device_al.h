#ifndef DEVICE_AL_HPP
#define DEVICE_AL_HPP


#include <list>
#include <AL/al.h>
#include <AL/alc.h>
#include "audiere.h"
#include "internal.h"


namespace audiere {

  class ALOutputStream;


  class ALAudioDevice : public RefCountedImplementation<AudioDevice> {
  public:
    ALAudioDevice();
    ~ALAudioDevice();

    bool initialize(const char* parameters);
    void update();
    OutputStream* openStream(SampleSource* source);

  private:
    void removeStream(ALOutputStream* stream);

    ALCdevice*  m_device;
    ALCcontext* m_context;

    typedef std::list<ALOutputStream*> StreamList;
    StreamList m_open_streams;

    friend class ALOutputStream;
  };


  class ALOutputStream : public DLLImplementation<OutputStream> {
  public:
    void play();
    void stop();
    void reset();
    bool isPlaying();
    void  setVolume(float volume);
    float getVolume();

  private:
    ALOutputStream(
      ALAudioDevice* device,
      SampleSource* source,
      ALuint al_source,
      ALuint* buffers,
      ALenum format,
      int sample_rate);
    ~ALOutputStream();

    void update();
    int read(void* samples, int sample_count);
    void fillBuffers();

  private:
    ALAudioDevice* m_device;

    // sample stream
    SampleSource* m_source;

    // informational (convenience)
    ALenum m_format;
    int    m_sample_size;  // (num channels * bits per sample / 8)
    int    m_sample_rate;

    // the last sample read
    ALubyte* m_last_sample;  
  
    // AL objects
    ALuint  m_ALsource;
    ALuint* m_buffers;

    int m_buffer_length;  // in samples
    bool m_is_playing;
    float m_volume;

    friend class ALAudioDevice;
  };

}


#endif
